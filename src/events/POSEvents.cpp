#include "../include/events/POSEvents.hpp"

/**
 * @file POSEvents.cpp
 * @brief Implementation of POS event constants
 */

namespace POSEvents {
    
    // =================================================================
    // Event Type Constants
    // =================================================================
    
    // Order Events
    const std::string ORDER_CREATED = "order_created";
    const std::string ORDER_MODIFIED = "order_modified";
    const std::string ORDER_ITEM_ADDED = "order_item_added";
    const std::string ORDER_ITEM_REMOVED = "order_item_removed";
    const std::string ORDER_COMPLETED = "order_completed";
    const std::string ORDER_CANCELLED = "order_cancelled";
    const std::string ORDER_STATUS_CHANGED = "order_status_changed";
    
    // Kitchen Events
    const std::string ORDER_SENT_TO_KITCHEN = "order_sent_to_kitchen";
    const std::string KITCHEN_STATUS_CHANGED = "kitchen_status_changed";
    const std::string KITCHEN_QUEUE_UPDATED = "kitchen_queue_updated";
    const std::string KITCHEN_BUSY_STATE_CHANGED = "kitchen_busy_state_changed";
    
    // Payment Events
    const std::string PAYMENT_INITIATED = "payment_initiated";
    const std::string PAYMENT_COMPLETED = "payment_completed";
    const std::string PAYMENT_FAILED = "payment_failed";
    const std::string REFUND_PROCESSED = "refund_processed";
    
    // UI Events
    const std::string THEME_CHANGED = "theme_changed";
    const std::string NOTIFICATION_REQUESTED = "notification_requested";
    const std::string UI_REFRESH_REQUESTED = "ui_refresh_requested";
    const std::string TABLE_SELECTION_CHANGED = "table_selection_changed";
    
    // System Events
    const std::string SYSTEM_ERROR = "system_error";
    const std::string CONFIGURATION_CHANGED = "configuration_changed";
    const std::string SERVICE_STATUS_CHANGED = "service_status_changed";
}
