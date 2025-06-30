#include "../../include/services/NotificationService.hpp"

#include <Wt/WPushButton.h>
#include <Wt/WIcon.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WApplication.h>

#include <iostream>
#include <algorithm>

NotificationService::NotificationService(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager)
    , notificationContainer_(nullptr)
    , autoRemovalTimer_(nullptr)
    , queueProcessTimer_(nullptr)
    , maxNotificationCount_(DEFAULT_MAX_NOTIFICATIONS)
    , defaultDuration_(DEFAULT_DURATION_MS)
    , soundsEnabled_(false)
    , notificationPosition_("top-right") {
}

void NotificationService::initialize() {
    std::cout << "Initializing NotificationService..." << std::endl;
    
    setupNotificationContainer();
    setupEventListeners();
    
    // Set up auto-removal timer
    if (Wt::WApplication::instance()) {
        autoRemovalTimer_ = Wt::WApplication::instance()->root()->addChild(
            std::make_unique<Wt::WTimer>());
        autoRemovalTimer_->setInterval(std::chrono::milliseconds(AUTO_REMOVAL_CHECK_INTERVAL_MS));
        autoRemovalTimer_->timeout().connect([this] { autoremoveExpiredNotifications(); });
        autoRemovalTimer_->start();
        
        // Set up queue processing timer
        queueProcessTimer_ = Wt::WApplication::instance()->root()->addChild(
            std::make_unique<Wt::WTimer>());
        queueProcessTimer_->setInterval(std::chrono::milliseconds(QUEUE_PROCESS_INTERVAL_MS));
        queueProcessTimer_->timeout().connect([this] { processNotificationQueue(); });
        queueProcessTimer_->start();
    }
    
    std::cout << "✓ NotificationService initialized" << std::endl;
}

void NotificationService::setupNotificationContainer() {
    if (!Wt::WApplication::instance()) {
        std::cerr << "Warning: No WApplication instance available for notification container" << std::endl;
        return;
    }
    
    // Create notification container
    auto container = std::make_unique<Wt::WContainerWidget>();
    notificationContainer_ = container.get();
    
    // Set up container styling
    notificationContainer_->addStyleClass("notification-container");
    notificationContainer_->addStyleClass("position-fixed");
    notificationContainer_->addStyleClass("d-flex");
    notificationContainer_->addStyleClass("flex-column");
    
    // Apply position styling
    setNotificationPosition(notificationPosition_);
    
    // Add to application root
    Wt::WApplication::instance()->root()->addWidget(std::move(container));
    
    std::cout << "✓ Notification container created" << std::endl;
}

void NotificationService::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "Error: EventManager not available for NotificationService" << std::endl;
        return;
    }
    
    // Subscribe to notification events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::NOTIFICATION_REQUESTED,
            [this](const std::any& data) { handleNotification(data); }));
    
    // Subscribe to business events for automatic notifications
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::SYSTEM_ERROR,
            [this](const std::any& data) { handleSystemError(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED,
            [this](const std::any& data) { handleOrderCreated(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_COMPLETED,
            [this](const std::any& data) { handleOrderCompleted(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::PAYMENT_COMPLETED,
            [this](const std::any& data) { handlePaymentProcessed(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::KITCHEN_STATUS_CHANGED,
            [this](const std::any& data) { handleKitchenStatusChanged(data); }));
    
    std::cout << "✓ NotificationService event listeners setup complete" << std::endl;
}

std::string NotificationService::showInfo(const std::string& message, int duration) {
    return showNotification(NotificationData(message, INFO, duration));
}

std::string NotificationService::showSuccess(const std::string& message, int duration) {
    return showNotification(NotificationData(message, SUCCESS, duration));
}

std::string NotificationService::showWarning(const std::string& message, int duration) {
    return showNotification(NotificationData(message, WARNING, duration));
}

std::string NotificationService::showError(const std::string& message, int duration) {
    return showNotification(NotificationData(message, ERROR, duration));
}

std::string NotificationService::showNotification(const NotificationData& notification) {
    if (!notificationContainer_) {
        std::cerr << "Warning: Notification container not available, queuing notification" << std::endl;
        const_cast<NotificationService*>(this)->addNotificationToQueue(notification);
        return notification.id;
    }
    
    // Enforce max notification limit
    enforceMaxNotificationLimit();
    
    // Create notification widget
    auto widget = createNotificationWidget(notification);
    auto widgetPtr = widget.get();
    
    // Add to container
    notificationContainer_->addWidget(std::move(widget));
    
    // Store references
    activeNotifications_.push_back(notification);
    notificationWidgets_[notification.id] = widgetPtr;
    
    // Play sound if enabled
    if (soundsEnabled_) {
        playNotificationSound(notification.type);
    }
    
    // Trigger triggerUpdate to ensure the notification is displayed
    if (Wt::WApplication::instance()) {
        Wt::WApplication::instance()->triggerUpdate();
    }
    
    std::cout << "Notification displayed: " << notification.message << std::endl;
    return notification.id;
}

std::string NotificationService::showNotificationWithAction(
    const std::string& message,
    NotificationType type,
    const std::string& actionText,
    std::function<void()> actionCallback,
    int duration) {
    
    NotificationData notification(message, type, duration, false, actionText, actionCallback);
    return showNotification(notification);
}

std::unique_ptr<Wt::WWidget> NotificationService::createNotificationWidget(
    const NotificationData& notification) {
    
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("alert");
    container->addStyleClass(getNotificationCssClass(notification.type));
    container->addStyleClass("alert-dismissible");
    container->addStyleClass("notification-item");
    container->addStyleClass("mb-2");
    container->addStyleClass("shadow-sm");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Icon
    auto icon = std::make_unique<Wt::WText>(getNotificationIcon(notification.type));
    icon->addStyleClass("notification-icon");
    icon->addStyleClass("me-2");
    layout->addWidget(std::move(icon));
    
    // Message
    auto message = std::make_unique<Wt::WText>(notification.message);
    message->addStyleClass("notification-message");
    message->addStyleClass("flex-grow-1");
    layout->addWidget(std::move(message), 1);
    
    // Action button (if provided)
    if (!notification.actionText.empty() && notification.actionCallback) {
        auto actionBtn = std::make_unique<Wt::WPushButton>(notification.actionText);
        actionBtn->addStyleClass("btn");
        actionBtn->addStyleClass("btn-sm");
        actionBtn->addStyleClass("btn-outline-secondary");
        actionBtn->addStyleClass("me-2");
        actionBtn->clicked().connect([notification]() {
            if (notification.actionCallback) {
                notification.actionCallback();
            }
        });
        layout->addWidget(std::move(actionBtn));
    }
    
    // Close button
    auto closeBtn = std::make_unique<Wt::WPushButton>("×");
    closeBtn->addStyleClass("btn-close");
    closeBtn->clicked().connect([this, id = notification.id]() {
        dismissNotification(id);
    });
    layout->addWidget(std::move(closeBtn));
    
    container->setLayout(std::move(layout));
    
    // Add fade-in animation
    container->addStyleClass("notification-fade-in");
    
    return std::move(container);
}

bool NotificationService::dismissNotification(const std::string& notificationId) {
    auto it = std::find_if(activeNotifications_.begin(), activeNotifications_.end(),
        [&notificationId](const NotificationData& n) { return n.id == notificationId; });
    
    if (it == activeNotifications_.end()) {
        return false;
    }
    
    // Remove from active notifications
    activeNotifications_.erase(it);
    
    // Remove widget
    removeNotificationWidget(notificationId);
    
    return true;
}

void NotificationService::dismissAllNotifications() {
    // Clear all active notifications
    activeNotifications_.clear();
    
    // Remove all widgets
    if (notificationContainer_) {
        notificationContainer_->clear();
    }
    notificationWidgets_.clear();
    
    std::cout << "All notifications dismissed" << std::endl;
}

void NotificationService::dismissNotificationsByType(NotificationType type) {
    // Find notifications of the specified type
    auto it = activeNotifications_.begin();
    while (it != activeNotifications_.end()) {
        if (it->type == type) {
            removeNotificationWidget(it->id);
            it = activeNotifications_.erase(it);
        } else {
            ++it;
        }
    }
    
    std::cout << "Dismissed all " << getNotificationTypeString(type) << " notifications" << std::endl;
}

void NotificationService::removeNotificationWidget(const std::string& notificationId) {
    auto widgetIt = notificationWidgets_.find(notificationId);
    if (widgetIt != notificationWidgets_.end()) {
        if (notificationContainer_ && widgetIt->second) {
            // Add fade-out animation
            widgetIt->second->addStyleClass("notification-fade-out");
            
            // Remove after animation
            notificationContainer_->removeWidget(widgetIt->second);
        }
        notificationWidgets_.erase(widgetIt);
    }
}

void NotificationService::autoremoveExpiredNotifications() {
    auto now = std::chrono::steady_clock::now();
    
    auto it = activeNotifications_.begin();
    while (it != activeNotifications_.end()) {
        if (it->duration > 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - it->timestamp).count();
            
            if (elapsed >= it->duration) {
                removeNotificationWidget(it->id);
                it = activeNotifications_.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void NotificationService::addNotificationToQueue(const NotificationData& notification) {
    notificationQueue_.push(notification);
}

void NotificationService::processNotificationQueue() {
    if (notificationQueue_.empty() || !notificationContainer_) {
        return;
    }
    
    // Process one notification from queue
    auto notification = notificationQueue_.front();
    notificationQueue_.pop();
    
    showNotification(notification);
}

void NotificationService::enforceMaxNotificationLimit() {
    if (maxNotificationCount_ <= 0) {
        return; // No limit
    }
    
    while (activeNotifications_.size() >= static_cast<size_t>(maxNotificationCount_)) {
        // Remove oldest notification
        if (!activeNotifications_.empty()) {
            auto oldest = activeNotifications_.front();
            dismissNotification(oldest.id);
        }
    }
}

void NotificationService::handleNotification(const std::any& eventData) {
    try {
        auto notificationData = std::any_cast<POSEvents::NotificationEventData>(eventData);
        
        NotificationType type = INFO;
        if (notificationData.type == "success") type = SUCCESS;
        else if (notificationData.type == "warning") type = WARNING;
        else if (notificationData.type == "error") type = ERROR;
        
        showNotification(NotificationData(notificationData.message, type, 
                                        notificationData.duration));
        
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error handling notification event: invalid data type" << std::endl;
    }
}

void NotificationService::handleSystemError(const std::any& eventData) {
    try {
        auto errorData = std::any_cast<POSEvents::ErrorEventData>(eventData);
        
        std::string message = errorData.errorMessage;
        if (!errorData.component.empty()) {
            message = errorData.component + ": " + message;
        }
        
        showError(message, errorData.isCritical ? 0 : 5000);
        
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error handling system error event: invalid data type" << std::endl;
    }
}

void NotificationService::handleOrderCreated(const std::any& eventData) {
    try {
        auto orderData = std::any_cast<POSEvents::OrderEventData>(eventData);
        showSuccess("Order created for Table " + 
                   std::to_string(orderData.order->getTableNumber()));
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error handling order created event: invalid data type" << std::endl;
    }
}

void NotificationService::handleOrderCompleted(const std::any& eventData) {
    try {
        auto orderData = std::any_cast<POSEvents::OrderEventData>(eventData);
        showSuccess("Order #" + std::to_string(orderData.order->getId()) + " completed!");
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error handling order completed event: invalid data type" << std::endl;
    }
}

void NotificationService::handlePaymentProcessed(const std::any& eventData) {
    try {
        auto paymentData = std::any_cast<POSEvents::PaymentEventData>(eventData);
        if (paymentData.result.success) {
            showSuccess("Payment processed successfully - $" + 
                       std::to_string(paymentData.result.amountCharged));
        } else {
            showError("Payment failed: " + paymentData.result.errorMessage);
        }
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error handling payment event: invalid data type" << std::endl;
    }
}

void NotificationService::handleKitchenStatusChanged(const std::any& eventData) {
    try {
        auto kitchenData = std::any_cast<POSEvents::KitchenEventData>(eventData);
        
        std::string statusMsg;
        switch (kitchenData.status) {
            case KitchenInterface::ORDER_RECEIVED:
                statusMsg = "Order #" + std::to_string(kitchenData.orderId) + " received by kitchen";
                showInfo(statusMsg);
                break;
            case KitchenInterface::IN_PREPARATION:
                statusMsg = "Order #" + std::to_string(kitchenData.orderId) + " is being prepared";
                showInfo(statusMsg);
                break;
            case KitchenInterface::READY_TO_SERVE:
                statusMsg = "Order #" + std::to_string(kitchenData.orderId) + " is ready to serve!";
                showSuccess(statusMsg, 10000); // Longer duration for important status
                break;
            case KitchenInterface::SERVED:
                statusMsg = "Order #" + std::to_string(kitchenData.orderId) + " has been served";
                showSuccess(statusMsg);
                break;
        }
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error handling kitchen status event: invalid data type" << std::endl;
    }
}

// Getters and setters
size_t NotificationService::getActiveNotificationCount() const {
    return activeNotifications_.size();
}

std::vector<std::string> NotificationService::getActiveNotificationIds() const {
    std::vector<std::string> ids;
    for (const auto& notification : activeNotifications_) {
        ids.push_back(notification.id);
    }
    return ids;
}

bool NotificationService::isNotificationActive(const std::string& notificationId) const {
    return std::any_of(activeNotifications_.begin(), activeNotifications_.end(),
        [&notificationId](const NotificationData& n) { return n.id == notificationId; });
}

void NotificationService::setMaxNotificationCount(int maxCount) {
    maxNotificationCount_ = maxCount;
    enforceMaxNotificationLimit();
}

int NotificationService::getMaxNotificationCount() const {
    return maxNotificationCount_;
}

void NotificationService::setDefaultDuration(int duration) {
    defaultDuration_ = duration;
}

int NotificationService::getDefaultDuration() const {
    return defaultDuration_;
}

void NotificationService::setSoundsEnabled(bool enabled) {
    soundsEnabled_ = enabled;
}

bool NotificationService::areSoundsEnabled() const {
    return soundsEnabled_;
}

void NotificationService::setNotificationPosition(const std::string& position) {
    notificationPosition_ = position;
    
    if (notificationContainer_) {
        // Remove existing position classes
        notificationContainer_->removeStyleClass("top-right");
        notificationContainer_->removeStyleClass("top-left");
        notificationContainer_->removeStyleClass("bottom-right");
        notificationContainer_->removeStyleClass("bottom-left");
        
        // Add new position class
        notificationContainer_->addStyleClass(position);
        
        // Apply specific styling based on position
        if (position == "top-right") {
            notificationContainer_->addStyleClass("top-0 end-0");
        } else if (position == "top-left") {
            notificationContainer_->addStyleClass("top-0 start-0");
        } else if (position == "bottom-right") {
            notificationContainer_->addStyleClass("bottom-0 end-0");
        } else if (position == "bottom-left") {
            notificationContainer_->addStyleClass("bottom-0 start-0");
        }
    }
}

// Helper methods
std::string NotificationService::getNotificationIcon(NotificationType type) const {
    switch (type) {
        case INFO:    return "ℹ️";
        case SUCCESS: return "✅";
        case WARNING: return "⚠️";
        case ERROR:   return "❌";
        default:      return "ℹ️";
    }
}

std::string NotificationService::getNotificationCssClass(NotificationType type) const {
    switch (type) {
        case INFO:    return "alert-info";
        case SUCCESS: return "alert-success";
        case WARNING: return "alert-warning";
        case ERROR:   return "alert-danger";
        default:      return "alert-info";
    }
}

std::string NotificationService::getNotificationTypeString(NotificationType type) const {
    switch (type) {
        case INFO:    return "info";
        case SUCCESS: return "success";
        case WARNING: return "warning";
        case ERROR:   return "error";
        default:      return "info";
    }
}

void NotificationService::playNotificationSound(NotificationType type) {
    // For now, just log that a sound should be played
    // In a real implementation, you might use HTML5 audio or system sounds
    std::cout << "♪ Playing " << getNotificationTypeString(type) << " notification sound" << std::endl;
}
