#ifndef POSEVENTS_H
#define POSEVENTS_H

#include "../Order.hpp"
#include "../KitchenInterface.hpp"
#include "../PaymentProcessor.hpp"
#include "../utils/Logging.hpp"
#include "../utils/LoggingUtils.hpp"

#include <memory>
#include <string>
#include <chrono>
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Value.h>

/**
 * @file POSEvents.hpp
 * @brief POS-specific event types and data structures with logging integration
 * 
 * This file defines all the event types used throughout the POS system
 * and provides helper functions for creating and handling these events.
 * Enhanced with comprehensive logging capabilities for debugging and monitoring.
 * 
 * @author Restaurant POS Team
 * @version 2.2.0 - Enhanced with logging integration
 */

/**
 * @namespace POSEvents
 * @brief Contains all POS-specific event types, utilities, and logging helpers
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
    extern const std::string CURRENT_ORDER_CHANGED;      // ADDED: For when current order is set/cleared
    
    // Menu Events
    extern const std::string MENU_UPDATED;               // ADDED: For when menu items change
    extern const std::string MENU_ITEM_AVAILABILITY_CHANGED; // ADDED: For when items become available/unavailable
    
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
     * @struct CurrentOrderEventData
     * @brief Data structure for current order change events
     */
    struct CurrentOrderEventData {
        std::shared_ptr<Order> newOrder;
        std::shared_ptr<Order> previousOrder;
        std::string reason; // "created", "cleared", "changed"
        
        CurrentOrderEventData(std::shared_ptr<Order> newOrd, 
                             std::shared_ptr<Order> prevOrd = nullptr,
                             const std::string& rsn = "")
            : newOrder(newOrd), previousOrder(prevOrd), reason(rsn) {}
    };
    
    /**
     * @struct MenuEventData
     * @brief Data structure for menu-related events
     */
    struct MenuEventData {
        std::string menuVersion;
        int itemCount;
        std::string updateReason; // "refresh", "item_added", "item_removed", "availability_changed"
        
        MenuEventData(const std::string& version = "", int count = 0, const std::string& reason = "")
            : menuVersion(version), itemCount(count), updateReason(reason) {}
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
    // Event Logging Utilities
    // =================================================================
    
    /**
     * @class EventLogger
     * @brief Specialized logging utilities for POS events
     */
    class EventLogger {
    public:
        /**
         * @brief Log an order event with detailed information
         * @param eventType The type of order event
         * @param order The order involved
         * @param additionalInfo Additional context information
         * @param logLevel Log level (default: INFO)
         */
        static void logOrderEvent(const std::string& eventType, 
                                 std::shared_ptr<Order> order, 
                                 const std::string& additionalInfo = "",
                                 LogLevel logLevel = LogLevel::INFO);
        
        /**
         * @brief Log a kitchen event with status details
         * @param eventType The type of kitchen event
         * @param orderId Order ID involved
         * @param status Current kitchen status
         * @param previousStatus Previous kitchen status
         * @param logLevel Log level (default: INFO)
         */
        static void logKitchenEvent(const std::string& eventType,
                                   int orderId,
                                   KitchenInterface::KitchenStatus status,
                                   KitchenInterface::KitchenStatus previousStatus = KitchenInterface::ORDER_RECEIVED,
                                   LogLevel logLevel = LogLevel::INFO);
        
        /**
         * @brief Log a payment event with transaction details
         * @param eventType The type of payment event
         * @param result Payment processing result
         * @param order Order involved in payment
         * @param logLevel Log level (default: INFO)
         */
        static void logPaymentEvent(const std::string& eventType,
                                   const PaymentProcessor::PaymentResult& result,
                                   std::shared_ptr<Order> order,
                                   LogLevel logLevel = LogLevel::INFO);
        
        /**
         * @brief Log a menu event with item details
         * @param eventType The type of menu event
         * @param itemCount Number of menu items
         * @param updateReason Reason for the menu update
         * @param logLevel Log level (default: INFO)
         */
        static void logMenuEvent(const std::string& eventType,
                                int itemCount,
                                const std::string& updateReason = "",
                                LogLevel logLevel = LogLevel::INFO);
        
        /**
         * @brief Log a UI event with context
         * @param eventType The type of UI event
         * @param context Event context or description
         * @param logLevel Log level (default: DEBUG)
         */
        static void logUIEvent(const std::string& eventType,
                              const std::string& context = "",
                              LogLevel logLevel = LogLevel::DEBUG);
        
        /**
         * @brief Log event publishing statistics
         * @param eventType Event type being published
         * @param subscriberCount Number of subscribers
         * @param publisherName Name of the publishing component
         */
        static void logEventPublication(const std::string& eventType,
                                       size_t subscriberCount,
                                       const std::string& publisherName);
        
        /**
         * @brief Get string representation of kitchen status
         * @param status Kitchen status enum value
         * @return Human-readable status string
         */
        static std::string kitchenStatusToString(KitchenInterface::KitchenStatus status);
        
        /**
         * @brief Get string representation of payment method
         * @param method Payment method enum value
         * @return Human-readable payment method string
         */
        static std::string paymentMethodToString(PaymentProcessor::PaymentMethod method);
        
        /**
         * @brief Create a detailed event summary for logging
         * @param eventType Type of event
         * @param summary Brief event description
         * @param details Detailed event information
         * @return Formatted log message
         */
        static std::string formatEventSummary(const std::string& eventType,
                                             const std::string& summary,
                                             const std::string& details = "");
    
    private:
        static Logger& getLogger() { return Logger::getInstance(); }
    };
    
    // =================================================================
    // Enhanced Event Data Creation Functions (with Optional Logging)
    // =================================================================
    
    /**
     * @brief Creates an order created event data with optional logging
     * @param order The order that was created
     * @param enableLogging Whether to log this event creation (default: true)
     * @return OrderEventData for the event
     */
    inline OrderEventData createOrderCreatedData(std::shared_ptr<Order> order, bool enableLogging = true) {
        if (enableLogging && order) {
            EventLogger::logOrderEvent(ORDER_CREATED, order, "Order created for table " + order->getTableIdentifier());
        }
        std::string info = order ? ("Order created for table " + order->getTableIdentifier()) : "Order created";
        return OrderEventData(order, info);
    }
    
    /**
     * @brief Creates a current order changed event data with optional logging
     * @param newOrder The new current order (can be nullptr)
     * @param previousOrder The previous current order (can be nullptr)
     * @param reason Reason for the change
     * @param enableLogging Whether to log this event creation (default: true)
     * @return CurrentOrderEventData for the event
     */
    inline CurrentOrderEventData createCurrentOrderChangedData(std::shared_ptr<Order> newOrder,
                                                               std::shared_ptr<Order> previousOrder = nullptr,
                                                               const std::string& reason = "",
                                                               bool enableLogging = true) {
        if (enableLogging) {
            std::string context = "Current order changed: " + reason;
            if (newOrder) {
                context += " (Order ID: " + std::to_string(newOrder->getOrderId()) + ")";
            } else {
                context += " (Order cleared)";
            }
            EventLogger::logUIEvent(CURRENT_ORDER_CHANGED, context);
        }
        return CurrentOrderEventData(newOrder, previousOrder, reason);
    }
    
    /**
     * @brief Creates a menu updated event data with optional logging
     * @param itemCount Number of items in the updated menu
     * @param reason Reason for the menu update
     * @param enableLogging Whether to log this event creation (default: true)
     * @return MenuEventData for the event
     */
    inline MenuEventData createMenuUpdatedData(int itemCount, const std::string& reason = "refresh", bool enableLogging = true) {
        if (enableLogging) {
            EventLogger::logMenuEvent(MENU_UPDATED, itemCount, reason);
        }
        return MenuEventData("1.0", itemCount, reason);
    }
    
    /**
     * @brief Creates an order item added event data with optional logging
     * @param order The order that was modified
     * @param itemIndex Index of the added item
     * @param itemName Name of the added item
     * @param quantity Quantity added
     * @param enableLogging Whether to log this event creation (default: true)
     * @return OrderItemEventData for the event
     */
    inline OrderItemEventData createOrderItemAddedData(std::shared_ptr<Order> order, 
                                                       size_t itemIndex, 
                                                       const std::string& itemName, 
                                                       int quantity,
                                                       bool enableLogging = true) {
        if (enableLogging && order) {
            std::string context = "Added " + std::to_string(quantity) + "x " + itemName + " to order " + std::to_string(order->getOrderId());
            EventLogger::logOrderEvent(ORDER_ITEM_ADDED, order, context);
        }
        return OrderItemEventData(order, itemIndex, itemName, quantity);
    }
    
    /**
     * @brief Creates a kitchen status changed event data with optional logging
     * @param orderId ID of the order with changed status
     * @param newStatus New kitchen status
     * @param oldStatus Previous kitchen status
     * @param enableLogging Whether to log this event creation (default: true)
     * @return KitchenEventData for the event
     */
    inline KitchenEventData createKitchenStatusChangedData(int orderId, 
                                                          KitchenInterface::KitchenStatus newStatus,
                                                          KitchenInterface::KitchenStatus oldStatus,
                                                          bool enableLogging = true) {
        if (enableLogging) {
            EventLogger::logKitchenEvent(KITCHEN_STATUS_CHANGED, orderId, newStatus, oldStatus);
        }
        return KitchenEventData(orderId, newStatus, oldStatus);
    }
    
    /**
     * @brief Creates a payment completed event data with optional logging
     * @param result Payment processing result
     * @param order Order that was paid for
     * @param enableLogging Whether to log this event creation (default: true)
     * @return PaymentEventData for the event
     */
    inline PaymentEventData createPaymentCompletedData(const PaymentProcessor::PaymentResult& result,
                                                       std::shared_ptr<Order> order,
                                                       bool enableLogging = true) {
        if (enableLogging) {
            EventLogger::logPaymentEvent(PAYMENT_COMPLETED, result, order);
        }
        return PaymentEventData(result, order);
    }
    
    /**
     * @brief Creates a notification event data with optional logging
     * @param message Notification message
     * @param type Notification type ("info", "success", "warning", "error")
     * @param duration Duration in milliseconds (0 = permanent)
     * @param enableLogging Whether to log this event creation (default: true)
     * @return NotificationEventData for the event
     */
    inline NotificationEventData createNotificationData(const std::string& message,
                                                        const std::string& type = "info",
                                                        int duration = 3000,
                                                        bool enableLogging = true) {
        if (enableLogging) {
            LogLevel level = LogLevel::INFO;
            if (type == "error") level = LogLevel::ERROR;
            else if (type == "warning") level = LogLevel::WARN;
            else if (type == "success") level = LogLevel::INFO;
            
            EventLogger::logUIEvent(NOTIFICATION_REQUESTED, message + " (" + type + ")", level);
        }
        return NotificationEventData(message, type, duration);
    }
    
    /**
     * @brief Creates a theme changed event data with optional logging
     * @param newThemeId ID of the new theme
     * @param newThemeName Name of the new theme
     * @param oldThemeId ID of the previous theme
     * @param enableLogging Whether to log this event creation (default: true)
     * @return ThemeEventData for the event
     */
    inline ThemeEventData createThemeChangedData(const std::string& newThemeId,
                                                 const std::string& newThemeName,
                                                 const std::string& oldThemeId = "",
                                                 bool enableLogging = true) {
        if (enableLogging) {
            std::string context = "Theme changed from " + oldThemeId + " to " + newThemeId + " (" + newThemeName + ")";
            EventLogger::logUIEvent(THEME_CHANGED, context);
        }
        return ThemeEventData(newThemeId, newThemeName, oldThemeId);
    }
    
    /**
     * @brief Creates an error event data with optional logging
     * @param message Error message
     * @param code Error code (optional)
     * @param component Component that generated the error (optional)
     * @param critical Whether this is a critical error
     * @param enableLogging Whether to log this event creation (default: true)
     * @return ErrorEventData for the event
     */
    inline ErrorEventData createErrorData(const std::string& message,
                                         const std::string& code = "",
                                         const std::string& component = "",
                                         bool critical = false,
                                         bool enableLogging = true) {
        if (enableLogging) {
            LogLevel level = critical ? LogLevel::ERROR : LogLevel::WARN;
            std::string context = component + " error";
            if (!code.empty()) context += " [" + code + "]";
            context += ": " + message;
            
            EventLogger::logUIEvent(SYSTEM_ERROR, context, level);
        }
        return ErrorEventData(message, code, component, critical);
    }
    
    // =================================================================
    // Original JSON Event Creation Functions (unchanged for compatibility)
    // =================================================================
    
    /**
     * @brief Creates a JSON object for order created events
     * @param order The order that was created
     * @return JSON object suitable for EventManager
     */
    inline Wt::Json::Object createOrderCreatedEvent(std::shared_ptr<Order> order) {
        Wt::Json::Object event;
        event["orderId"] = Wt::Json::Value(order->getOrderId());
        event["tableIdentifier"] = Wt::Json::Value(order->getTableIdentifier());
        event["status"] = Wt::Json::Value(static_cast<int>(order->getStatus()));
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(order->getTimestamp())));
        event["orderData"] = order->toJson();
        event["message"] = Wt::Json::Value("Order created for " + order->getTableIdentifier());
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
        event["tableIdentifier"] = Wt::Json::Value(order->getTableIdentifier());
        event["status"] = Wt::Json::Value(static_cast<int>(order->getStatus()));
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        event["orderData"] = order->toJson();
        event["message"] = Wt::Json::Value("Order " + std::to_string(order->getOrderId()) + " modified");
        return event;
    }
    
    /**
     * @brief Creates a JSON object for current order changed events
     * @param newOrder The new current order (can be nullptr)
     * @param previousOrder The previous current order (can be nullptr)
     * @param reason Reason for the change
     * @return JSON object suitable for EventManager
     */
    inline Wt::Json::Object createCurrentOrderChangedEvent(std::shared_ptr<Order> newOrder,
                                                           std::shared_ptr<Order> previousOrder = nullptr,
                                                           const std::string& reason = "") {
        Wt::Json::Object event;
        
        if (newOrder) {
            event["orderId"] = Wt::Json::Value(newOrder->getOrderId());
            event["tableIdentifier"] = Wt::Json::Value(newOrder->getTableIdentifier());
            event["hasCurrentOrder"] = Wt::Json::Value(true);
        } else {
            event["orderId"] = Wt::Json::Value(-1);
            event["tableIdentifier"] = Wt::Json::Value("");
            event["hasCurrentOrder"] = Wt::Json::Value(false);
        }
        
        if (previousOrder) {
            event["previousOrderId"] = Wt::Json::Value(previousOrder->getOrderId());
        }
        
        event["reason"] = Wt::Json::Value(reason);
        event["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        event["message"] = Wt::Json::Value("Current order changed: " + reason);
        
        return event;
    }
    
    // [Additional JSON creation functions remain unchanged...]
    // Note: For brevity, I'm including just the main ones. The rest follow the same pattern.
}

#endif // POSEVENTS_H
