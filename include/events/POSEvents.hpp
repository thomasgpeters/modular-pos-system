#ifndef POSEVENTS_H
#define POSEVENTS_H

#include "../Order.hpp"
#include "../KitchenInterface.hpp"
#include "../PaymentProcessor.hpp"
#include <memory>
#include <string>
#include <chrono>
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Value.h>

/**
 * @file POSEvents.hpp
 * @brief POS-specific event types and data structures
 * 
 * This file defines all the event types used throughout the POS system
 * and provides helper functions for creating and handling these events.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @namespace POSEvents
 * @brief Contains all POS-specific event types and utilities
 */
namespace POSEvents {
    
    // =================================================================
    // Event Type Constants
    // =================================================================
    
    // Order Events
    extern const std::string ORDER_CREATED;
    extern const std::string ORDER_MODIFIED;
    extern const std::string ORDER_ITEM_ADDED;
    extern const std::string ORDER_ITEM_REMOVED;
    extern const std::string ORDER_COMPLETED;
    extern const std::string ORDER_CANCELLED;
    extern const std::string ORDER_STATUS_CHANGED;
    
    // Kitchen Events
    extern const std::string ORDER_SENT_TO_KITCHEN;
    extern const std::string KITCHEN_STATUS_CHANGED;
    extern const std::string KITCHEN_QUEUE_UPDATED;
    extern const std::string KITCHEN_BUSY_STATE_CHANGED;
    
    // Payment Events
    extern const std::string PAYMENT_INITIATED;
    extern const std::string PAYMENT_COMPLETED;
    extern const std::string PAYMENT_FAILED;
    extern const std::string REFUND_PROCESSED;
    
    // UI Events
    extern const std::string THEME_CHANGED;
    extern const std::string NOTIFICATION_REQUESTED;
    extern const std::string UI_REFRESH_REQUESTED;
    extern const std::string TABLE_SELECTION_CHANGED;
    
    // System Events
    extern const std::string SYSTEM_ERROR;
    extern const std::string CONFIGURATION_CHANGED;
    extern const std::string SERVICE_STATUS_CHANGED;
    
    // =================================================================
    // Event Data Structures
    // =================================================================
    
    /**
     * @struct OrderEventData
     * @brief Data structure for order-related events
     */
    struct OrderEventData {
        std::shared_ptr<Order> order;
        std::string additionalInfo;
        
        OrderEventData(std::shared_ptr<Order> ord, const std::string& info = "") 
            : order(ord), additionalInfo(info) {}
    };
    
    /**
     * @struct OrderItemEventData
     * @brief Data structure for order item events
     */
    struct OrderItemEventData {
        std::shared_ptr<Order> order;
        size_t itemIndex;
        std::string itemName;
        int quantity;
        
        OrderItemEventData(std::shared_ptr<Order> ord, size_t idx, 
                          const std::string& name, int qty)
            : order(ord), itemIndex(idx), itemName(name), quantity(qty) {}
    };
    
    /**
     * @struct KitchenEventData
     * @brief Data structure for kitchen-related events
     */
    struct KitchenEventData {
        int orderId;
        KitchenInterface::KitchenStatus status;
        KitchenInterface::KitchenStatus previousStatus;
        std::string additionalInfo;
        
        KitchenEventData(int id, KitchenInterface::KitchenStatus stat, 
                        KitchenInterface::KitchenStatus prevStat = KitchenInterface::ORDER_RECEIVED,
                        const std::string& info = "")
            : orderId(id), status(stat), previousStatus(prevStat), additionalInfo(info) {}
    };
    
    /**
     * @struct PaymentEventData
     * @brief Data structure for payment-related events
     */
    struct PaymentEventData {
        PaymentProcessor::PaymentResult result;
        std::shared_ptr<Order> order;
        
        PaymentEventData(const PaymentProcessor::PaymentResult& res, std::shared_ptr<Order> ord)
            : result(res), order(ord) {}
    };
    
    /**
     * @struct NotificationEventData
     * @brief Data structure for notification events
     */
    struct NotificationEventData {
        std::string message;
        std::string type; // "info", "success", "warning", "error"
        int duration; // duration in milliseconds, 0 = permanent
        
        NotificationEventData(const std::string& msg, const std::string& t = "info", int dur = 3000)
            : message(msg), type(t), duration(dur) {}
    };
    
    /**
     * @struct ThemeEventData
     * @brief Data structure for theme change events
     */
    struct ThemeEventData {
        std::string themeId;
        std::string themeName;
        std::string previousThemeId;
        
        ThemeEventData(const std::string& id, const std::string& name, const std::string& prevId = "")
            : themeId(id), themeName(name), previousThemeId(prevId) {}
    };
    
    /**
     * @struct ErrorEventData
     * @brief Data structure for system error events
     */
    struct ErrorEventData {
        std::string errorMessage;
        std::string errorCode;
        std::string component;
        bool isCritical;
        
        ErrorEventData(const std::string& msg, const std::string& code = "", 
                      const std::string& comp = "", bool critical = false)
            : errorMessage(msg), errorCode(code), component(comp), isCritical(critical) {}
    };
    
    // =================================================================
    // Event Data Creation Functions (Return Structs)
    // =================================================================
    
    /**
     * @brief Creates an order created event data
     * @param order The order that was created
     * @return OrderEventData for the event
     */
    inline OrderEventData createOrderCreatedData(std::shared_ptr<Order> order) {
        return OrderEventData(order, "Order created for table " + std::to_string(order->getTableNumber()));
    }
    
    /**
     * @brief Creates an order item added event data
     * @param order The order that was modified
     * @param itemIndex Index of the added item
     * @param itemName Name of the added item
     * @param quantity Quantity added
     * @return OrderItemEventData for the event
     */
    inline OrderItemEventData createOrderItemAddedData(std::shared_ptr<Order> order, 
                                                       size_t itemIndex, 
                                                       const std::string& itemName, 
                                                       int quantity) {
        return OrderItemEventData(order, itemIndex, itemName, quantity);
    }
    
    /**
     * @brief Creates a kitchen status changed event data
     * @param orderId ID of the order with changed status
     * @param newStatus New kitchen status
     * @param oldStatus Previous kitchen status
     * @return KitchenEventData for the event
     */
    inline KitchenEventData createKitchenStatusChangedData(int orderId, 
                                                          KitchenInterface::KitchenStatus newStatus,
                                                          KitchenInterface::KitchenStatus oldStatus) {
        return KitchenEventData(orderId, newStatus, oldStatus);
    }
    
    /**
     * @brief Creates a payment completed event data
     * @param result Payment processing result
     * @param order Order that was paid for
     * @return PaymentEventData for the event
     */
    inline PaymentEventData createPaymentCompletedData(const PaymentProcessor::PaymentResult& result,
                                                       std::shared_ptr<Order> order) {
        return PaymentEventData(result, order);
    }
    
    /**
     * @brief Creates a notification event data
     * @param message Notification message
     * @param type Notification type ("info", "success", "warning", "error")
     * @param duration Duration in milliseconds (0 = permanent)
     * @return NotificationEventData for the event
     */
    inline NotificationEventData createNotificationData(const std::string& message,
                                                        const std::string& type = "info",
                                                        int duration = 3000) {
        return NotificationEventData(message, type, duration);
    }
    
    /**
     * @brief Creates a theme changed event data
     * @param newThemeId ID of the new theme
     * @param newThemeName Name of the new theme
     * @param oldThemeId ID of the previous theme
     * @return ThemeEventData for the event
     */
    inline ThemeEventData createThemeChangedData(const std::string& newThemeId,
                                                 const std::string& newThemeName,
                                                 const std::string& oldThemeId = "") {
        return ThemeEventData(newThemeId, newThemeName, oldThemeId);
    }
    
    /**
     * @brief Creates an error event data
     * @param message Error message
     * @param code Error code (optional)
     * @param component Component that generated the error (optional)
     * @param critical Whether this is a critical error
     * @return ErrorEventData for the event
     */
    inline ErrorEventData createErrorData(const std::string& message,
                                         const std::string& code = "",
                                         const std::string& component = "",
                                         bool critical = false) {
        return ErrorEventData(message, code, component, critical);
    }
    
    // =================================================================
    // JSON Event Creation Functions (for EventManager compatibility)
    // =================================================================
    
    /**
     * @brief Creates a JSON object for order created events
     * @param order The order that was created
     * @return JSON object suitable for EventManager
     */
    inline Wt::Json::Object createOrderCreatedEvent(std::shared_ptr<Order> order) {
        Wt::Json::Object event;
        event["orderId"] = Wt::Json::Value(order->getOrderId());
        event["tableNumber"] = Wt::Json::Value(order->getTableNumber());
        event["status"] = Wt::Json::Value(static_cast<int>(order->getStatus()));
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(order->getTimestamp())));
        event["orderData"] = order->toJson();
        event["message"] = Wt::Json::Value("Order created for table " + std::to_string(order->getTableNumber()));
        return event;
    }
    
    /**
     * @brief Creates a JSON object for order modified events
     * @param order The order that was modified
     * @return JSON object suitable for EventManager
     */
    inline Wt::Json::Object createOrderModifiedEvent(std::shared_ptr<Order> order) {
        Wt::Json::Object event;
        event["orderId"] = Wt::Json::Value(order->getOrderId());
        event["tableNumber"] = Wt::Json::Value(order->getTableNumber());
        event["status"] = Wt::Json::Value(static_cast<int>(order->getStatus()));
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        event["orderData"] = order->toJson();
        event["message"] = Wt::Json::Value("Order " + std::to_string(order->getOrderId()) + " modified");
        return event;
    }
    
    /**
     * @brief Creates a JSON object for kitchen status changed events
     * @param orderId ID of the order with changed status
     * @param newStatus New kitchen status
     * @param oldStatus Previous kitchen status
     * @return JSON object suitable for EventManager
     */
    inline Wt::Json::Object createKitchenStatusChangedEvent(int orderId, 
                                                           KitchenInterface::KitchenStatus newStatus,
                                                           KitchenInterface::KitchenStatus oldStatus) {
        Wt::Json::Object event;
        event["orderId"] = Wt::Json::Value(orderId);
        event["newStatus"] = Wt::Json::Value(static_cast<int>(newStatus));
        event["previousStatus"] = Wt::Json::Value(static_cast<int>(oldStatus));
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        event["message"] = Wt::Json::Value("Kitchen status changed for order " + std::to_string(orderId));
        return event;
    }
    
    /**
     * @brief Creates a JSON object for payment completed events
     * @param result Payment processing result
     * @param order Order that was paid for
     * @return JSON object suitable for EventManager
     */
    inline Wt::Json::Object createPaymentCompletedEvent(const PaymentProcessor::PaymentResult& result,
                                                        std::shared_ptr<Order> order) {
        Wt::Json::Object event;
        event["orderId"] = Wt::Json::Value(order->getOrderId());
        event["paymentSuccess"] = Wt::Json::Value(result.success);
        event["paymentAmount"] = Wt::Json::Value(result.amountProcessed);
        event["paymentMethod"] = Wt::Json::Value(static_cast<int>(result.method));
        event["transactionId"] = Wt::Json::Value(result.transactionId);
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(result.timestamp)));
        
        if (!result.success) {
            event["errorMessage"] = Wt::Json::Value(result.errorMessage);
            event["message"] = Wt::Json::Value("Payment failed for order " + std::to_string(order->getOrderId()));
        } else {
            event["message"] = Wt::Json::Value("Payment completed for order " + std::to_string(order->getOrderId()));
        }
        
        return event;
    }
    
    /**
     * @brief Creates a JSON object for notification events
     * @param message Notification message
     * @param type Notification type ("info", "success", "warning", "error")
     * @param duration Duration in milliseconds (0 = permanent)
     * @return JSON object suitable for EventManager
     */
    inline Wt::Json::Object createNotificationEvent(const std::string& message,
                                                    const std::string& type = "info",
                                                    int duration = 3000) {
        Wt::Json::Object event;
        event["message"] = Wt::Json::Value(message);
        event["type"] = Wt::Json::Value(type);
        event["duration"] = Wt::Json::Value(duration);
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        return event;
    }
    
    /**
     * @brief Creates a JSON object for theme changed events
     * @param newThemeId ID of the new theme
     * @param newThemeName Name of the new theme
     * @param oldThemeId ID of the previous theme
     * @return JSON object suitable for EventManager
     */
    inline Wt::Json::Object createThemeChangedEvent(const std::string& newThemeId,
                                                    const std::string& newThemeName,
                                                    const std::string& oldThemeId = "") {
        Wt::Json::Object event;
        event["newThemeId"] = Wt::Json::Value(newThemeId);
        event["newThemeName"] = Wt::Json::Value(newThemeName);
        event["previousThemeId"] = Wt::Json::Value(oldThemeId);
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        event["message"] = Wt::Json::Value("Theme changed to " + newThemeName);
        return event;
    }
    
    /**
     * @brief Creates a JSON object for error events
     * @param message Error message
     * @param code Error code (optional)
     * @param component Component that generated the error (optional)
     * @param critical Whether this is a critical error
     * @return JSON object suitable for EventManager
     */
    inline Wt::Json::Object createErrorEvent(const std::string& message,
                                            const std::string& code = "",
                                            const std::string& component = "",
                                            bool critical = false) {
        Wt::Json::Object event;
        event["errorMessage"] = Wt::Json::Value(message);
        event["errorCode"] = Wt::Json::Value(code);
        event["component"] = Wt::Json::Value(component);
        event["isCritical"] = Wt::Json::Value(critical);
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        return event;
    }
}

#endif // POSEVENTS_H