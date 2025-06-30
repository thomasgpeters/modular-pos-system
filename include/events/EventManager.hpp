#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

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
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class EventManager
 * @brief Centralized event management system for component communication
 * 
 * The EventManager provides a publish-subscribe pattern for components to
 * communicate without tight coupling. Components can publish events and
 * subscribe to events they're interested in.
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
     * @brief Constructs a new EventManager
     */
    EventManager();
    
    /**
     * @brief Virtual destructor
     */
    virtual ~EventManager() = default;
    
    /**
     * @brief Subscribes to an event type
     * @param eventType Type of event to subscribe to
     * @param handler Function to call when event occurs
     * @return Subscription handle for unsubscribing
     */
    SubscriptionHandle subscribe(const std::string& eventType, EventHandler handler);
    
    /**
     * @brief Unsubscribes from an event
     * @param handle Subscription handle returned from subscribe()
     */
    void unsubscribe(SubscriptionHandle handle);
    
    /**
     * @brief Publishes an event to all subscribers
     * @param eventType Type of event being published
     * @param data Event data (can be any type)
     */
    void publish(const std::string& eventType, const std::any& data);
    
    /**
     * @brief Publishes an event with no data
     * @param eventType Type of event being published
     */
    void publish(const std::string& eventType);
    
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

private:
    struct Subscription {
        SubscriptionHandle handle;
        EventHandler handler;
    };
    
    std::unordered_map<std::string, std::vector<Subscription>> eventSubscriptions_;
    SubscriptionHandle nextHandle_;
};

#endif // EVENTMANAGER_H

