#include "../include/services/NotificationService.hpp"

#include <iostream>
#include <algorithm>
#include <sstream>

/**
 * @file NotificationService.cpp
 * @brief Implementation of the NotificationService class
 */

NotificationService::NotificationService(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager), 
      notificationContainer_(nullptr),
      application_(nullptr),
      nextNotificationId_(1) {
    
    // Get current Wt application instance
    application_ = Wt::WApplication::instance();
    
    std::cout << "NotificationService initialized" << std::endl;
}

void NotificationService::initialize() {
    std::cout << "Initializing NotificationService..." << std::endl;
    
    // Set up event listeners for automatic notifications
    setupEventListeners();
    
    // Create notification container if not already set
    if (!notificationContainer_ && application_) {
        notificationContainer_ = createNotificationContainer();
        if (notificationContainer_) {
            application_->root()->addWidget(std::unique_ptr<Wt::WWidget>(notificationContainer_));
            positionNotificationContainer();
        }
    }
    
    std::cout << "✓ NotificationService initialization complete" << std::endl;
}

// =================================================================
// Public Notification Methods
// =================================================================

int NotificationService::showInfo(const std::string& message, int duration) {
    return showNotification(message, INFO, duration);
}

int NotificationService::showSuccess(const std::string& message, int duration) {
    return showNotification(message, SUCCESS, duration);
}

int NotificationService::showWarning(const std::string& message, int duration) {
    return showNotification(message, WARNING, duration);
}

int NotificationService::showError(const std::string& message, int duration) {
    return showNotification(message, ERROR, duration);
}

int NotificationService::showNotification(const std::string& message, NotificationType type, int duration) {
    if (!notificationContainer_ || !application_) {
        std::cerr << "NotificationService: Container not initialized, cannot show notification" << std::endl;
        return -1;
    }
    
    // Use default duration if not specified
    if (duration == 0) {
        duration = config_.defaultDuration;
    }
    
    // Create notification widget
    auto widget = createNotificationWidget(message, type);
    if (!widget) {
        std::cerr << "NotificationService: Failed to create notification widget" << std::endl;
        return -1;
    }
    
    int notificationId = nextNotificationId_++;
    Wt::WContainerWidget* widgetPtr = widget.get();
    
    // Add to container
    notificationContainer_->addWidget(std::move(widget));
    
    // Create and store notification data
    auto notification = std::make_unique<ActiveNotification>(notificationId, widgetPtr, type, message);
    
    // Set up auto-dismiss timer if duration > 0
    if (duration > 0) {
        setupNotificationTimer(notification.get(), duration);
    }
    
    // Add to active notifications
    activeNotifications_.push_back(std::move(notification));
    
    // Manage stack overflow
    if (config_.autoStackManagement) {
        manageNotificationStack();
    }
    
    std::cout << "Notification shown: " << message << " (ID: " << notificationId << ")" << std::endl;
    
    return notificationId;
}

int NotificationService::showNotificationWithAction(const std::string& message,
                                                   NotificationType type,
                                                   const std::string& actionLabel,
                                                   ActionCallback callback,
                                                   int duration) {
    if (!notificationContainer_ || !application_) {
        std::cerr << "NotificationService: Container not initialized" << std::endl;
        return -1;
    }
    
    // Use default duration if not specified
    if (duration == 0) {
        duration = config_.defaultDuration;
    }
    
    // Create notification widget with action button
    auto widget = createNotificationWidget(message, type, actionLabel, callback);
    if (!widget) {
        std::cerr << "NotificationService: Failed to create notification widget with action" << std::endl;
        return -1;
    }
    
    int notificationId = nextNotificationId_++;
    Wt::WContainerWidget* widgetPtr = widget.get();
    
    // Add to container
    notificationContainer_->addWidget(std::move(widget));
    
    // Create and store notification data
    auto notification = std::make_unique<ActiveNotification>(notificationId, widgetPtr, type, message);
    
    // Set up auto-dismiss timer if duration > 0
    if (duration > 0) {
        setupNotificationTimer(notification.get(), duration);
    }
    
    // Add to active notifications
    activeNotifications_.push_back(std::move(notification));
    
    // Manage stack overflow
    if (config_.autoStackManagement) {
        manageNotificationStack();
    }
    
    std::cout << "Action notification shown: " << message << " (ID: " << notificationId << ")" << std::endl;
    
    return notificationId;
}

// =================================================================
// Dismissal Methods
// =================================================================

bool NotificationService::dismissNotification(int notificationId) {
    auto it = std::find_if(activeNotifications_.begin(), activeNotifications_.end(),
        [notificationId](const std::unique_ptr<ActiveNotification>& notification) {
            return notification->id == notificationId;
        });
    
    if (it != activeNotifications_.end()) {
        removeNotificationWidget(notificationId);
        activeNotifications_.erase(it);
        std::cout << "Notification dismissed: ID " << notificationId << std::endl;
        return true;
    }
    
    return false;
}

void NotificationService::dismissAllNotifications() {
    std::cout << "Dismissing all notifications (" << activeNotifications_.size() << " active)" << std::endl;
    
    while (!activeNotifications_.empty()) {
        int id = activeNotifications_.back()->id;
        dismissNotification(id);
    }
}

void NotificationService::dismissNotificationsByType(NotificationType type) {
    // Collect IDs to dismiss (avoid modifying vector while iterating)
    std::vector<int> idsToRemove;
    
    for (const auto& notification : activeNotifications_) {
        if (notification->type == type) {
            idsToRemove.push_back(notification->id);
        }
    }
    
    // Dismiss collected notifications
    for (int id : idsToRemove) {
        dismissNotification(id);
    }
    
    std::cout << "Dismissed " << idsToRemove.size() << " notifications of specified type" << std::endl;
}

// =================================================================
// Configuration Management
// =================================================================

const NotificationService::NotificationConfig& NotificationService::getConfiguration() const {
    return config_;
}

void NotificationService::setConfiguration(const NotificationConfig& config) {
    config_ = config;
    
    // Reposition container if position changed
    if (notificationContainer_) {
        positionNotificationContainer();
    }
    
    std::cout << "Notification configuration updated" << std::endl;
}

size_t NotificationService::getActiveNotificationCount() const {
    return activeNotifications_.size();
}

void NotificationService::setNotificationContainer(Wt::WContainerWidget* container) {
    notificationContainer_ = container;
    
    if (container) {
        positionNotificationContainer();
        std::cout << "Custom notification container set" << std::endl;
    }
}

void NotificationService::handleNotification(const std::any& eventData) {
    try {
        auto notificationData = std::any_cast<POSEvents::NotificationEventData>(eventData);
        processNotificationEvent(notificationData);
    } catch (const std::bad_any_cast& e) {
        std::cerr << "NotificationService: Invalid event data type for notification" << std::endl;
    }
}

// =================================================================
// Protected Methods
// =================================================================

void NotificationService::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "NotificationService: No event manager available for automatic notifications" << std::endl;
        return;
    }
    
    std::cout << "Setting up notification event listeners..." << std::endl;
    
    // Subscribe to notification events
    auto notificationHandle = eventManager_->subscribe(POSEvents::NOTIFICATION_REQUESTED,
        [this](const std::any& data) { handleNotification(data); });
    eventSubscriptions_.push_back(notificationHandle);
    
    // Subscribe to business events for automatic notifications
    auto orderCreatedHandle = eventManager_->subscribe(POSEvents::ORDER_CREATED,
        [this](const std::any& data) { handleOrderEvent(data); });
    eventSubscriptions_.push_back(orderCreatedHandle);
    
    auto paymentCompletedHandle = eventManager_->subscribe(POSEvents::PAYMENT_COMPLETED,
        [this](const std::any& data) { handlePaymentEvent(data); });
    eventSubscriptions_.push_back(paymentCompletedHandle);
    
    auto kitchenStatusHandle = eventManager_->subscribe(POSEvents::KITCHEN_STATUS_CHANGED,
        [this](const std::any& data) { handleKitchenEvent(data); });
    eventSubscriptions_.push_back(kitchenStatusHandle);
    
    auto systemErrorHandle = eventManager_->subscribe(POSEvents::SYSTEM_ERROR,
        [this](const std::any& data) { handleSystemError(data); });
    eventSubscriptions_.push_back(systemErrorHandle);
    
    std::cout << "✓ Event listeners setup complete" << std::endl;
}

Wt::WContainerWidget* NotificationService::createNotificationContainer() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("notification-container");
    container->setId("notification-container");
    
    return container.release();
}

std::unique_ptr<Wt::WContainerWidget> NotificationService::createNotificationWidget(
    const std::string& message,
    NotificationType type,
    const std::string& actionLabel,
    ActionCallback callback) {
    
    auto notification = std::make_unique<Wt::WContainerWidget>();
    notification->addStyleClass("notification");
    notification->addStyleClass(getNotificationTypeClass(type));
    
    // Main content container
    auto contentContainer = std::make_unique<Wt::WContainerWidget>();
    contentContainer->addStyleClass("notification-content");
    
    // Icon
    auto iconContainer = std::make_unique<Wt::WContainerWidget>();
    iconContainer->addStyleClass("notification-icon");
    auto icon = std::make_unique<Wt::WText>(getNotificationIcon(type));
    icon->setTextFormat(Wt::TextFormat::Plain);
    iconContainer->addWidget(std::move(icon));
    contentContainer->addWidget(std::move(iconContainer));
    
    // Message
    auto messageContainer = std::make_unique<Wt::WContainerWidget>();
    messageContainer->addStyleClass("notification-message");
    auto messageText = std::make_unique<Wt::WText>(message);
    messageText->setTextFormat(Wt::TextFormat::Plain);
    messageContainer->addWidget(std::move(messageText));
    contentContainer->addWidget(std::move(messageContainer));
    
    notification->addWidget(std::move(contentContainer));
    
    // Actions container
    auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
    actionsContainer->addStyleClass("notification-actions");
    
    // Action button (if provided)
    if (!actionLabel.empty() && callback) {
        auto actionButton = std::make_unique<Wt::WPushButton>(actionLabel);
        actionButton->addStyleClass("notification-action-btn");
        actionButton->clicked().connect([callback, this, notificationId = nextNotificationId_]() {
            callback();
            dismissNotification(notificationId);
        });
        actionsContainer->addWidget(std::move(actionButton));
    }
    
    // Close button
    auto closeButton = std::make_unique<Wt::WPushButton>("×");
    closeButton->addStyleClass("notification-close-btn");
    closeButton->clicked().connect([this, notificationId = nextNotificationId_]() {
        dismissNotification(notificationId);
    });
    actionsContainer->addWidget(std::move(closeButton));
    
    notification->addWidget(std::move(actionsContainer));
    
    // Apply styling
    applyNotificationStyling(notification.get(), type);
    
    return notification;
}

std::string NotificationService::getNotificationTypeClass(NotificationType type) const {
    switch (type) {
        case INFO:    return "notification-info";
        case SUCCESS: return "notification-success";
        case WARNING: return "notification-warning";
        case ERROR:   return "notification-error";
        default:      return "notification-info";
    }
}

std::string NotificationService::getNotificationIcon(NotificationType type) const {
    switch (type) {
        case INFO:    return "ℹ️";
        case SUCCESS: return "✅";
        case WARNING: return "⚠️";
        case ERROR:   return "❌";
        default:      return "ℹ️";
    }
}

void NotificationService::manageNotificationStack() {
    // Remove oldest notifications if we exceed the maximum
    while (activeNotifications_.size() > static_cast<size_t>(config_.maxNotifications)) {
        if (!activeNotifications_.empty()) {
            int oldestId = activeNotifications_.front()->id;
            dismissNotification(oldestId);
        }
    }
}

// =================================================================
// Private Methods
// =================================================================

void NotificationService::removeNotificationWidget(int notificationId) {
    auto it = std::find_if(activeNotifications_.begin(), activeNotifications_.end(),
        [notificationId](const std::unique_ptr<ActiveNotification>& notification) {
            return notification->id == notificationId;
        });
    
    if (it != activeNotifications_.end() && (*it)->widget) {
        // Stop timer if running
        if ((*it)->timer) {
            (*it)->timer->stop();
        }
        
        // Remove widget from container
        if (notificationContainer_) {
            notificationContainer_->removeWidget((*it)->widget);
        }
    }
}

void NotificationService::processNotificationEvent(const POSEvents::NotificationEventData& data) {
    NotificationType type = INFO;
    
    // Convert string type to enum
    if (data.type == "success") {
        type = SUCCESS;
    } else if (data.type == "warning") {
        type = WARNING;
    } else if (data.type == "error") {
        type = ERROR;
    }
    
    showNotification(data.message, type, data.duration);
}

void NotificationService::setupNotificationTimer(ActiveNotification* notification, int duration) {
    if (!notification) return;
    
    notification->timer = std::make_unique<Wt::WTimer>();
    notification->timer->setInterval(std::chrono::milliseconds(duration));
    notification->timer->setSingleShot(true);
    
    notification->timer->timeout().connect([this, id = notification->id]() {
        dismissNotification(id);
    });
    
    notification->timer->start();
}

void NotificationService::applyNotificationStyling(Wt::WContainerWidget* widget, NotificationType type) {
    if (!widget) return;
    
    // Base styles using Wt methods
    widget->setInline(false);
    widget->setMargin(5, Wt::Side::Bottom);
    widget->setPadding(12);
    
    // Add CSS for modern appearance using setAttributeValue for style
    std::stringstream css;
    css << "border-radius: 8px; ";
    css << "box-shadow: 0 4px 12px rgba(0,0,0,0.1); ";
    css << "display: flex; ";
    css << "align-items: center; ";
    css << "justify-content: space-between; ";
    css << "min-width: 300px; ";
    css << "max-width: 500px; ";
    css << "font-size: 14px; ";
    css << "line-height: 1.4; ";
    
    // Type-specific colors
    switch (type) {
        case INFO:
            css << "background-color: #e3f2fd; ";
            css << "border-left: 4px solid #2196f3; ";
            css << "color: #1565c0; ";
            break;
        case SUCCESS:
            css << "background-color: #e8f5e8; ";
            css << "border-left: 4px solid #4caf50; ";
            css << "color: #2e7d32; ";
            break;
        case WARNING:
            css << "background-color: #fff3e0; ";
            css << "border-left: 4px solid #ff9800; ";
            css << "color: #ef6c00; ";
            break;
        case ERROR:
            css << "background-color: #ffebee; ";
            css << "border-left: 4px solid #f44336; ";
            css << "color: #c62828; ";
            break;
    }
    
    // Add animations if enabled
    if (config_.enableAnimations) {
        css << "transition: all 0.3s ease-in-out; ";
        css << "animation: slideInRight 0.3s ease-out; ";
    }
    
    // Use setAttributeValue to set the style attribute
    widget->setAttributeValue("style", css.str());
}

void NotificationService::positionNotificationContainer() {
    if (!notificationContainer_) return;
    
    std::stringstream css;
    css << "position: fixed; ";
    css << "z-index: 9999; ";
    css << "pointer-events: none; ";
    css << "padding: 20px; ";
    
    // Position based on configuration
    switch (config_.position) {
        case TOP_RIGHT:
            css << "top: 0; right: 0; ";
            break;
        case TOP_LEFT:
            css << "top: 0; left: 0; ";
            break;
        case BOTTOM_RIGHT:
            css << "bottom: 0; right: 0; ";
            break;
        case BOTTOM_LEFT:
            css << "bottom: 0; left: 0; ";
            break;
        case TOP_CENTER:
            css << "top: 0; left: 50%; transform: translateX(-50%); ";
            break;
        case BOTTOM_CENTER:
            css << "bottom: 0; left: 50%; transform: translateX(-50%); ";
            break;
    }
    
    // Use setAttributeValue to set the style attribute
    notificationContainer_->setAttributeValue("style", css.str());
    
    // Add CSS animation keyframes to the application if animations are enabled
    if (config_.enableAnimations && application_) {
        // Add keyframe animations via CSS in the HTML head
        std::string animationCSS = R"(
            <style>
            @keyframes slideInRight {
                from { transform: translateX(100%); opacity: 0; }
                to { transform: translateX(0); opacity: 1; }
            }
            </style>
        )";
        // Note: This would need to be added to the HTML head section
        // For now, we'll rely on external CSS file for animations
    }
}

// =================================================================
// Business Event Handlers
// =================================================================

void NotificationService::handleOrderEvent(const std::any& eventData) {
    try {
        // Extract order information from event data
        // This would need to match the actual event data structure
        showSuccess("New order created successfully");
    } catch (const std::exception& e) {
        std::cerr << "NotificationService: Error handling order event: " << e.what() << std::endl;
    }
}

void NotificationService::handlePaymentEvent(const std::any& eventData) {
    try {
        // Extract payment information from event data
        showSuccess("Payment processed successfully");
    } catch (const std::exception& e) {
        std::cerr << "NotificationService: Error handling payment event: " << e.what() << std::endl;
    }
}

void NotificationService::handleKitchenEvent(const std::any& eventData) {
    try {
        // Extract kitchen status information from event data
        showInfo("Kitchen status updated");
    } catch (const std::exception& e) {
        std::cerr << "NotificationService: Error handling kitchen event: " << e.what() << std::endl;
    }
}

void NotificationService::handleSystemError(const std::any& eventData) {
    try {
        // Extract error information from event data
        showError("System error occurred");
    } catch (const std::exception& e) {
        std::cerr << "NotificationService: Error handling system error event: " << e.what() << std::endl;
    }
}
