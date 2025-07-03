#include "../../include/events/POSEvents.hpp"

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
    
}
