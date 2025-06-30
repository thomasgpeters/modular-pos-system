#ifndef POSEVENTS_H
#define POSEVENTS_H

#include "../Order.hpp"
#include "../KitchenInterface.hpp"
#include "../PaymentProcessor.hpp"
#include <memory>
#include <string>

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
    // Event Helper Functions
    // =================================================================
    
    /**
     * @brief Creates an order created event data
     * @param order The order that was created
     * @return OrderEventData for the event
     */
    inline OrderEventData createOrderCreatedEvent(std::shared_ptr<Order> order) {
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
    inline OrderItemEventData createOrderItemAddedEvent(std::shared_ptr<Order> order, 
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
    inline KitchenEventData createKitchenStatusChangedEvent(int orderId, 
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
    inline PaymentEventData createPaymentCompletedEvent(const PaymentProcessor::PaymentResult& result,
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
    inline NotificationEventData createNotificationEvent(const std::string& message,
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
    inline ThemeEventData createThemeChangedEvent(const std::string& newThemeId,
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
    inline ErrorEventData createErrorEvent(const std::string& message,
                                          const std::string& code = "",
                                          const std::string& component = "",
                                          bool critical = false) {
        return ErrorEventData(message, code, component, critical);
    }
}

#endif // POSEVENTS_H
