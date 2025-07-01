#include "../../include/events/EventManager.hpp"
#include <algorithm>
#include <iostream>

EventManager::EventManager() : nextHandle_(1) {}

EventManager::SubscriptionHandle EventManager::subscribe(const std::string& eventType, EventHandler handler) {
    SubscriptionHandle handle = nextHandle_++;
    
    Subscription subscription;
    subscription.handle = handle;
    subscription.handler = std::move(handler);
    
    eventSubscriptions_[eventType].push_back(std::move(subscription));
    
    return handle;
}

void EventManager::unsubscribe(SubscriptionHandle handle) {
    for (auto& [eventType, subscriptions] : eventSubscriptions_) {
        auto it = std::remove_if(subscriptions.begin(), subscriptions.end(),
            [handle](const Subscription& sub) { return sub.handle == handle; });
        
        if (it != subscriptions.end()) {
            subscriptions.erase(it, subscriptions.end());
            break;
        }
    }
}

void EventManager::publish(const std::string& eventType, const std::any& data) {
    auto it = eventSubscriptions_.find(eventType);
    if (it != eventSubscriptions_.end()) {
        for (const auto& subscription : it->second) {
            try {
                subscription.handler(data);
            } catch (const std::exception& e) {
                std::cerr << "Error in event handler for " << eventType << ": " << e.what() << std::endl;
            }
        }
    }
}

void EventManager::publish(const std::string& eventType) {
    publish(eventType, std::any{});
}

size_t EventManager::getSubscriberCount(const std::string& eventType) const {
    auto it = eventSubscriptions_.find(eventType);
    return (it != eventSubscriptions_.end()) ? it->second.size() : 0;
}

void EventManager::clear() {
    eventSubscriptions_.clear();
    nextHandle_ = 1;
}
