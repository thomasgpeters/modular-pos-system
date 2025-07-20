#include "../../include/events/EventManager.hpp"
#include "../../include/utils/LoggingUtils.hpp"

#include <algorithm>
#include <iostream>
#include <chrono>
#include <sstream>

EventManager::EventManager() 
    : nextHandle_(1)
    , logger_(Logger::getInstance())
    , totalEventsPublished_(0)
    , totalEventHandlerInvocations_(0)
    , totalEventHandlerErrors_(0)
{
    logger_.info("EventManager initialized");
    LOG_OPERATION_STATUS(logger_, "EventManager initialization", true);
}

EventManager::~EventManager() {
    logger_.info("EventManager destructor called");
    
    // Log final statistics
    logSubscriptionStatistics();
    
    logger_.info(LoggingUtils::formatStatus("EventManager", "DESTROYED", 
                "Final stats: " + formatEventStatistics()));
}

EventManager::SubscriptionHandle EventManager::subscribe(const std::string& eventType, 
                                                        EventHandler handler, 
                                                        const std::string& subscriberName) {
    SubscriptionHandle handle = nextHandle_++;
    
    Subscription subscription(handle, std::move(handler), subscriberName);
    eventSubscriptions_[eventType].push_back(std::move(subscription));
    
    // Log the subscription
    logSubscriptionAction("SUBSCRIBE", eventType, handle, subscriberName);
    
    LOG_KEY_VALUE(logger_, debug, "Total event types", getTotalEventTypes());
    LOG_KEY_VALUE(logger_, debug, "Total subscriptions", getTotalSubscriptions());
    
    return handle;
}

void EventManager::unsubscribe(SubscriptionHandle handle, const std::string& subscriberName) {
    bool found = false;
    std::string eventType = "unknown";
    
    for (auto& [type, subscriptions] : eventSubscriptions_) {
        auto it = std::find_if(subscriptions.begin(), subscriptions.end(),
            [handle](const Subscription& sub) { return sub.handle == handle; });
        
        if (it != subscriptions.end()) {
            eventType = type;
            
            // Log subscription details before removal
            std::ostringstream detailsMsg;
            detailsMsg << "Subscription active for " 
                      << std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::system_clock::now() - it->subscriptionTime).count()
                      << " seconds, invoked " << it->invocationCount << " times";
            logger_.debug(detailsMsg.str());
            
            subscriptions.erase(it);
            found = true;
            break;
        }
    }
    
    if (found) {
        logSubscriptionAction("UNSUBSCRIBE", eventType, handle, subscriberName);
    } else {
        LOG_COMPONENT_ERROR(logger_, "EventManager", "unsubscribe", 
                           "Handle not found: " + LoggingUtils::toString(handle));
    }
    
    LOG_KEY_VALUE(logger_, debug, "Total subscriptions after unsubscribe", getTotalSubscriptions());
}

void EventManager::publish(const std::string& eventType, 
                          const std::any& data, 
                          const std::string& publisherName) {
    auto it = eventSubscriptions_.find(eventType);
    size_t subscriberCount = 0;
    size_t successfulInvocations = 0;
    size_t failedInvocations = 0;
    
    if (it != eventSubscriptions_.end()) {
        subscriberCount = it->second.size();
        
        for (auto& subscription : it->second) {
            try {
                subscription.handler(data);
                subscription.invocationCount++;
                successfulInvocations++;
                totalEventHandlerInvocations_++;
                
            } catch (const std::exception& e) {
                failedInvocations++;
                totalEventHandlerErrors_++;
                logEventHandlerError(eventType, subscription.handle, 
                                   subscription.subscriberName, e.what());
            } catch (...) {
                failedInvocations++;
                totalEventHandlerErrors_++;
                logEventHandlerError(eventType, subscription.handle, 
                                   subscription.subscriberName, "Unknown exception");
            }
        }
    }
    
    totalEventsPublished_++;
    
    // Log the publication
    logPublishAction(eventType, subscriberCount, publisherName);
    
    if (failedInvocations > 0) {
        std::ostringstream errorMsg;
        errorMsg << "Event publication completed with errors: " << failedInvocations 
                 << " failed, " << successfulInvocations << " succeeded";
        logger_.warn(errorMsg.str());
    }
    
    // Debug logging for subscriber details
    if (logger_.getLogLevel() >= LogLevel::DEBUG) {
        LOG_KEY_VALUE(logger_, debug, "Successful invocations", successfulInvocations);
        LOG_KEY_VALUE(logger_, debug, "Failed invocations", failedInvocations);
    }
}

void EventManager::publish(const std::string& eventType, const std::string& publisherName) {
    publish(eventType, std::any{}, publisherName);
}

size_t EventManager::getSubscriberCount(const std::string& eventType) const {
    auto it = eventSubscriptions_.find(eventType);
    return (it != eventSubscriptions_.end()) ? it->second.size() : 0;
}

void EventManager::clear() {
    size_t totalSubscriptionsCleared = getTotalSubscriptions();
    size_t totalEventTypesCleared = getTotalEventTypes();
    
    eventSubscriptions_.clear();
    nextHandle_ = 1;
    
    std::ostringstream clearMsg;
    clearMsg << "Cleared " << totalSubscriptionsCleared << " subscriptions across " 
             << totalEventTypesCleared << " event types";
    logger_.info(clearMsg.str());
    
    LOG_OPERATION_STATUS(logger_, "EventManager clear", true);
}

void EventManager::logSubscriptionStatistics() const {
    logger_.info("=== EventManager Statistics ===");
    LOG_KEY_VALUE(logger_, info, "Total event types", getTotalEventTypes());
    LOG_KEY_VALUE(logger_, info, "Total active subscriptions", getTotalSubscriptions());
    LOG_KEY_VALUE(logger_, info, "Total events published", totalEventsPublished_);
    LOG_KEY_VALUE(logger_, info, "Total handler invocations", totalEventHandlerInvocations_);
    LOG_KEY_VALUE(logger_, info, "Total handler errors", totalEventHandlerErrors_);
    
    // Calculate success rate
    if (totalEventHandlerInvocations_ > 0) {
        double successRate = (double)(totalEventHandlerInvocations_ - totalEventHandlerErrors_) 
                           / totalEventHandlerInvocations_ * 100.0;
        std::ostringstream rateMsg;
        rateMsg << std::fixed << std::setprecision(2) << successRate << "%";
        LOG_KEY_VALUE(logger_, info, "Handler success rate", rateMsg.str());
    }
    
    logger_.info("==============================");
}

void EventManager::logAllSubscriptions() const {
    logger_.debug("=== Active Event Subscriptions ===");
    
    if (eventSubscriptions_.empty()) {
        logger_.debug("No active subscriptions");
        return;
    }
    
    for (const auto& [eventType, subscriptions] : eventSubscriptions_) {
        std::ostringstream eventMsg;
        eventMsg << "Event Type: " << eventType << " (" << subscriptions.size() << " subscribers)";
        logger_.debug(eventMsg.str());
        
        for (const auto& subscription : subscriptions) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - subscription.subscriptionTime).count();
            
            std::ostringstream subMsg;
            subMsg << "  - Handle: " << subscription.handle 
                   << ", Subscriber: " << subscription.subscriberName
                   << ", Active: " << duration << "s"
                   << ", Invocations: " << subscription.invocationCount;
            logger_.debug(subMsg.str());
        }
    }
    
    logger_.debug("=================================");
}

size_t EventManager::getTotalEventTypes() const {
    return eventSubscriptions_.size();
}

size_t EventManager::getTotalSubscriptions() const {
    size_t total = 0;
    for (const auto& [eventType, subscriptions] : eventSubscriptions_) {
        total += subscriptions.size();
    }
    return total;
}

// Private helper methods

void EventManager::logSubscriptionAction(const std::string& action, 
                                        const std::string& eventType, 
                                        SubscriptionHandle handle, 
                                        const std::string& subscriberName) const {
    std::ostringstream actionMsg;
    actionMsg << "[" << action << "] " << subscriberName 
              << " -> " << eventType 
              << " (handle: " << handle << ")";
    logger_.info(actionMsg.str());
    
    // Additional debug info
    if (logger_.getLogLevel() >= LogLevel::DEBUG) {
        LOG_KEY_VALUE(logger_, debug, "Subscribers for this event", getSubscriberCount(eventType));
    }
}

void EventManager::logPublishAction(const std::string& eventType, 
                                   size_t subscriberCount, 
                                   const std::string& publisherName) const {
    std::ostringstream publishMsg;
    publishMsg << "[PUBLISH] " << publisherName 
               << " -> " << eventType 
               << " (" << subscriberCount << " subscribers)";
    
    if (subscriberCount == 0) {
        logger_.debug(publishMsg.str() + " - NO SUBSCRIBERS");
    } else {
        logger_.debug(publishMsg.str());
    }
}

void EventManager::logEventHandlerError(const std::string& eventType, 
                                       SubscriptionHandle handle, 
                                       const std::string& subscriberName,
                                       const std::string& error) const {
    std::ostringstream errorMsg;
    errorMsg << "Handler error in " << subscriberName 
             << " for event " << eventType 
             << " (handle: " << handle << "): " << error;
    logger_.error(errorMsg.str());
}

std::string EventManager::formatEventStatistics() const {
    std::ostringstream stats;
    stats << "Events: " << totalEventsPublished_ 
          << ", Invocations: " << totalEventHandlerInvocations_
          << ", Errors: " << totalEventHandlerErrors_;
    return stats.str();
}
