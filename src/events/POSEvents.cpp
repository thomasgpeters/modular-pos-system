#include "../../include/events/POSEvents.hpp"

namespace POSEvents {
    // Order Events
    const std::string ORDER_CREATED = "order.created";
    const std::string ORDER_MODIFIED = "order.modified";
    const std::string ORDER_ITEM_ADDED = "order.item.added";
    const std::string ORDER_ITEM_REMOVED = "order.item.removed";
    const std::string ORDER_COMPLETED = "order.completed";
    const std::string ORDER_CANCELLED = "order.cancelled";
    const std::string ORDER_STATUS_CHANGED = "order.status.changed";
    
    // Kitchen Events
    const std::string ORDER_SENT_TO_KITCHEN = "kitchen.order.sent";
    const std::string KITCHEN_STATUS_CHANGED = "kitchen.status.changed";
    const std::string KITCHEN_QUEUE_UPDATED = "kitchen.queue.updated";
    const std::string KITCHEN_BUSY_STATE_CHANGED = "kitchen.busy.changed";
    
    // Payment Events
    const std::string PAYMENT_INITIATED = "payment.initiated";
    const std::string PAYMENT_COMPLETED = "payment.completed";
    const std::string PAYMENT_FAILED = "payment.failed";
    const std::string REFUND_PROCESSED = "payment.refund";
    
    // UI Events
    const std::string THEME_CHANGED = "ui.theme.changed";
    const std::string NOTIFICATION_REQUESTED = "ui.notification.requested";
    const std::string UI_REFRESH_REQUESTED = "ui.refresh.requested";
    const std::string TABLE_SELECTION_CHANGED = "ui.table.changed";
    
    // System Events
    const std::string SYSTEM_ERROR = "system.error";
    const std::string CONFIGURATION_CHANGED = "system.config.changed";
    const std::string SERVICE_STATUS_CHANGED = "system.service.status";
}
