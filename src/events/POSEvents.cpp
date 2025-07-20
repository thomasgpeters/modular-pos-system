#include "../../include/events/POSEvents.hpp"
#include "../../include/utils/LoggingUtils.hpp"

#include <sstream>
#include <iomanip>

namespace POSEvents {
    
    // =================================================================
    // Event Type Constants Definitions
    // =================================================================
    
    // Order Events
    const std::string ORDER_CREATED = "ORDER_CREATED";
    const std::string ORDER_MODIFIED = "ORDER_MODIFIED";
    const std::string ORDER_ITEM_ADDED = "ORDER_ITEM_ADDED";
    const std::string ORDER_ITEM_REMOVED = "ORDER_ITEM_REMOVED";
    const std::string ORDER_COMPLETED = "ORDER_COMPLETED";
    const std::string ORDER_CANCELLED = "ORDER_CANCELLED";
    const std::string ORDER_STATUS_CHANGED = "ORDER_STATUS_CHANGED";
    const std::string CURRENT_ORDER_CHANGED = "CURRENT_ORDER_CHANGED";
    
    // Menu Events
    const std::string MENU_UPDATED = "MENU_UPDATED";
    const std::string MENU_ITEM_AVAILABILITY_CHANGED = "MENU_ITEM_AVAILABILITY_CHANGED";
    
    // Kitchen Events
    const std::string ORDER_SENT_TO_KITCHEN = "ORDER_SENT_TO_KITCHEN";
    const std::string KITCHEN_STATUS_CHANGED = "KITCHEN_STATUS_CHANGED";
    const std::string KITCHEN_QUEUE_UPDATED = "KITCHEN_QUEUE_UPDATED";
    const std::string KITCHEN_BUSY_STATE_CHANGED = "KITCHEN_BUSY_STATE_CHANGED";
    
    // Payment Events
    const std::string PAYMENT_INITIATED = "PAYMENT_INITIATED";
    const std::string PAYMENT_COMPLETED = "PAYMENT_COMPLETED";
    const std::string PAYMENT_FAILED = "PAYMENT_FAILED";
    const std::string REFUND_PROCESSED = "REFUND_PROCESSED";
    
    // UI Events
    const std::string THEME_CHANGED = "THEME_CHANGED";
    const std::string NOTIFICATION_REQUESTED = "NOTIFICATION_REQUESTED";
    const std::string UI_REFRESH_REQUESTED = "UI_REFRESH_REQUESTED";
    const std::string TABLE_SELECTION_CHANGED = "TABLE_SELECTION_CHANGED";
    
    // System Events
    const std::string SYSTEM_ERROR = "SYSTEM_ERROR";
    const std::string CONFIGURATION_CHANGED = "CONFIGURATION_CHANGED";
    const std::string SERVICE_STATUS_CHANGED = "SERVICE_STATUS_CHANGED";
    
    // =================================================================
    // EventLogger Implementation
    // =================================================================
    
    void EventLogger::logOrderEvent(const std::string& eventType, 
                                   std::shared_ptr<Order> order, 
                                   const std::string& additionalInfo,
                                   LogLevel logLevel) {
        if (!order) {
            LOG_COMPONENT_ERROR(getLogger(), "EventLogger", "logOrderEvent", "Null order provided");
            return;
        }
        
        std::ostringstream eventMsg;
        eventMsg << "[" << eventType << "] "
                 << "Order ID: " << order->getOrderId()
                 << ", Table: " << order->getTableIdentifier()
                 << ", Status: " << static_cast<int>(order->getStatus())
                 << ", Total: $" << std::fixed << std::setprecision(2) << order->getTotal();
        
        if (!additionalInfo.empty()) {
            eventMsg << " - " << additionalInfo;
        }
        
        switch (logLevel) {
            case LogLevel::DEBUG: getLogger().debug(eventMsg.str()); break;
            case LogLevel::INFO:  getLogger().info(eventMsg.str()); break;
            case LogLevel::WARN:  getLogger().warn(eventMsg.str()); break;
            case LogLevel::ERROR: getLogger().error(eventMsg.str()); break;
            default: getLogger().info(eventMsg.str()); break;
        }
    }
    
    void EventLogger::logKitchenEvent(const std::string& eventType,
                                     int orderId,
                                     KitchenInterface::KitchenStatus status,
                                     KitchenInterface::KitchenStatus previousStatus,
                                     LogLevel logLevel) {
        std::ostringstream eventMsg;
        eventMsg << "[" << eventType << "] "
                 << "Order ID: " << orderId
                 << ", Status: " << kitchenStatusToString(status);
        
        if (previousStatus != status) {
            eventMsg << " (was: " << kitchenStatusToString(previousStatus) << ")";
        }
        
        switch (logLevel) {
            case LogLevel::DEBUG: getLogger().debug(eventMsg.str()); break;
            case LogLevel::INFO:  getLogger().info(eventMsg.str()); break;
            case LogLevel::WARN:  getLogger().warn(eventMsg.str()); break;
            case LogLevel::ERROR: getLogger().error(eventMsg.str()); break;
            default: getLogger().info(eventMsg.str()); break;
        }
    }
    
    void EventLogger::logPaymentEvent(const std::string& eventType,
                                     const PaymentProcessor::PaymentResult& result,
                                     std::shared_ptr<Order> order,
                                     LogLevel logLevel) {
        if (!order) {
            LOG_COMPONENT_ERROR(getLogger(), "EventLogger", "logPaymentEvent", "Null order provided");
            return;
        }
        
        std::ostringstream eventMsg;
        eventMsg << "[" << eventType << "] "
                 << "Order ID: " << order->getOrderId()
                 << ", Amount: $" << std::fixed << std::setprecision(2) << result.amountProcessed
                 << ", Method: " << paymentMethodToString(result.method)
                 << ", Success: " << LoggingUtils::boolToString(result.success);
        
        if (!result.success && !result.errorMessage.empty()) {
            eventMsg << ", Error: " << result.errorMessage;
        }
        
        if (!result.transactionId.empty()) {
            eventMsg << ", Transaction: " << result.transactionId;
        }
        
        switch (logLevel) {
            case LogLevel::DEBUG: getLogger().debug(eventMsg.str()); break;
            case LogLevel::INFO:  getLogger().info(eventMsg.str()); break;
            case LogLevel::WARN:  getLogger().warn(eventMsg.str()); break;
            case LogLevel::ERROR: getLogger().error(eventMsg.str()); break;
            default: getLogger().info(eventMsg.str()); break;
        }
    }
    
    void EventLogger::logMenuEvent(const std::string& eventType,
                                  int itemCount,
                                  const std::string& updateReason,
                                  LogLevel logLevel) {
        std::ostringstream eventMsg;
        eventMsg << "[" << eventType << "] "
                 << "Item count: " << itemCount;
        
        if (!updateReason.empty()) {
            eventMsg << ", Reason: " << updateReason;
        }
        
        switch (logLevel) {
            case LogLevel::DEBUG: getLogger().debug(eventMsg.str()); break;
            case LogLevel::INFO:  getLogger().info(eventMsg.str()); break;
            case LogLevel::WARN:  getLogger().warn(eventMsg.str()); break;
            case LogLevel::ERROR: getLogger().error(eventMsg.str()); break;
            default: getLogger().info(eventMsg.str()); break;
        }
    }
    
    void EventLogger::logUIEvent(const std::string& eventType,
                                const std::string& context,
                                LogLevel logLevel) {
        std::ostringstream eventMsg;
        eventMsg << "[" << eventType << "]";
        
        if (!context.empty()) {
            eventMsg << " " << context;
        }
        
        switch (logLevel) {
            case LogLevel::DEBUG: getLogger().debug(eventMsg.str()); break;
            case LogLevel::INFO:  getLogger().info(eventMsg.str()); break;
            case LogLevel::WARN:  getLogger().warn(eventMsg.str()); break;
            case LogLevel::ERROR: getLogger().error(eventMsg.str()); break;
            default: getLogger().debug(eventMsg.str()); break;
        }
    }
    
    void EventLogger::logEventPublication(const std::string& eventType,
                                         size_t subscriberCount,
                                         const std::string& publisherName) {
        std::ostringstream pubMsg;
        pubMsg << "[EVENT_PUBLISH] " << publisherName 
               << " -> " << eventType 
               << " (" << subscriberCount << " subscribers)";
        
        if (subscriberCount == 0) {
            getLogger().debug(pubMsg.str() + " - NO SUBSCRIBERS");
        } else {
            getLogger().debug(pubMsg.str());
        }
    }
    
    std::string EventLogger::kitchenStatusToString(KitchenInterface::KitchenStatus status) {
        switch (status) {
            case KitchenInterface::ORDER_RECEIVED:    return "ORDER_RECEIVED";
            default: return "UNKNOWN_STATUS(" + std::to_string(static_cast<int>(status)) + ")";
        }
    }
    
    std::string EventLogger::paymentMethodToString(PaymentProcessor::PaymentMethod method) {
        switch (method) {
            case PaymentProcessor::CASH:           return "CASH";
            case PaymentProcessor::CREDIT_CARD:    return "CREDIT_CARD";
            case PaymentProcessor::DEBIT_CARD:     return "DEBIT_CARD";
            case PaymentProcessor::MOBILE_PAY:     return "MOBILE_PAY";
            case PaymentProcessor::GIFT_CARD:      return "GIFT_CARD";
            default: return "UNKNOWN_METHOD(" + std::to_string(static_cast<int>(method)) + ")";
        }
    }
    
    std::string EventLogger::formatEventSummary(const std::string& eventType,
                                               const std::string& summary,
                                               const std::string& details) {
        std::ostringstream formatted;
        formatted << "[" << eventType << "] " << summary;
        
        if (!details.empty()) {
            formatted << " - " << details;
        }
        
        return formatted.str();
    }
}
