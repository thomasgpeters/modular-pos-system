#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "../utils/Logging.hpp"

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <any>

/**
 * @file EventManager.hpp
 * @brief Event management system for loose coupling between components
 * 
 * This provides a centralized event system that allows components to communicate
 * without direct dependencies, promoting modularity and testability.
 * Enhanced with comprehensive logging for debugging and monitoring.
 * 
 * @author Restaurant POS Team
 * @version 2.1.0 - Enhanced with logging integration
 */

/**
 * @class EventManager
 * @brief Centralized event management system for component communication
 * 
 * The EventManager provides a publish-subscribe pattern for components to
 * communicate without tight coupling. Components can publish events and
 * subscribe to events they're interested in.
 * 
 * Enhanced features:
 * - Comprehensive logging of all event operations
 * - Subscription tracking and debugging
 * - Error handling with detailed logging
 * - Performance monitoring capabilities
 */
class EventManager {
public:
    /**
     * @brief Event handler function type
     */
    using EventHandler = std::function<void(const std::any& data)>;
    
    /**
     * @brief Event subscription handle for unsubscribing
     */
    using SubscriptionHandle = size_t;
    
    /**
     * @brief Constructs a new EventManager with logging
     */
    EventManager();
    
    /**
     * @brief Virtual destructor
     */
    virtual ~EventManager();
    
    /**
     * @brief Subscribes to an event type
     * @param eventType Type of event to subscribe to
     * @param handler Function to call when event occurs
     * @param subscriberName Optional name for debugging (default: "unnamed")
     * @return Subscription handle for unsubscribing
     */
    SubscriptionHandle subscribe(const std::string& eventType, 
                               EventHandler handler, 
                               const std::string& subscriberName = "unnamed");
    
    /**
     * @brief Unsubscribes from an event
     * @param handle Subscription handle returned from subscribe()
     * @param subscriberName Optional name for debugging
     */
    void unsubscribe(SubscriptionHandle handle, const std::string& subscriberName = "unnamed");
    
    /**
     * @brief Publishes an event to all subscribers
     * @param eventType Type of event being published
     * @param data Event data (can be any type)
     * @param publisherName Optional name for debugging (default: "unnamed")
     */
    void publish(const std::string& eventType, 
                const std::any& data, 
                const std::string& publisherName = "unnamed");
    
    /**
     * @brief Publishes an event with no data
     * @param eventType Type of event being published
     * @param publisherName Optional name for debugging (default: "unnamed")
     */
    void publish(const std::string& eventType, const std::string& publisherName = "unnamed");
    
    /**
     * @brief Gets the number of subscribers for an event type
     * @param eventType Event type to check
     * @return Number of subscribers
     */
    size_t getSubscriberCount(const std::string& eventType) const;
    
    /**
     * @brief Clears all event subscriptions
     */
    void clear();
    
    /**
     * @brief Logs current subscription statistics
     */
    void logSubscriptionStatistics() const;
    
    /**
     * @brief Logs all active subscriptions (for debugging)
     */
    void logAllSubscriptions() const;
    
    /**
     * @brief Gets total number of event types with subscribers
     * @return Number of different event types
     */
    size_t getTotalEventTypes() const;
    
    /**
     * @brief Gets total number of active subscriptions
     * @return Total subscription count across all event types
     */
    size_t getTotalSubscriptions() const;

private:
    struct Subscription {
        SubscriptionHandle handle;
        EventHandler handler;
        std::string subscriberName;
        std::chrono::system_clock::time_point subscriptionTime;
        size_t invocationCount;
        
        Subscription(SubscriptionHandle h, EventHandler hdlr, const std::string& name)
            : handle(h)
            , handler(std::move(hdlr))
            , subscriberName(name)
            , subscriptionTime(std::chrono::system_clock::now())
            , invocationCount(0) {}
    };
    
    // Core event system
    std::unordered_map<std::string, std::vector<Subscription>> eventSubscriptions_;
    SubscriptionHandle nextHandle_;
    
    // Logging integration
    Logger& logger_;
    
    // Statistics tracking
    mutable size_t totalEventsPublished_;
    mutable size_t totalEventHandlerInvocations_;
    mutable size_t totalEventHandlerErrors_;
    
    // Helper methods
    void logSubscriptionAction(const std::string& action, 
                              const std::string& eventType, 
                              SubscriptionHandle handle, 
                              const std::string& subscriberName) const;
    
    void logPublishAction(const std::string& eventType, 
                         size_t subscriberCount, 
                         const std::string& publisherName) const;
    
    void logEventHandlerError(const std::string& eventType, 
                             SubscriptionHandle handle, 
                             const std::string& subscriberName,
                             const std::string& error) const;
    
    std::string formatEventStatistics() const;
};

#endif // EVENTMANAGER_H
