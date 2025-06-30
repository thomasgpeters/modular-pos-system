#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include "../events/EventManager.hpp"
#include "../events/POSEvents.hpp"

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTimer.h>

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <queue>

/**
 * @file NotificationService.hpp
 * @brief Centralized notification system for user feedback
 * 
 * This service provides toast-style notifications, alerts, and other
 * user feedback mechanisms throughout the POS system.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class NotificationService
 * @brief Service for managing user notifications and feedback
 * 
 * The NotificationService provides a centralized system for displaying
 * notifications to users, including toast messages, alerts, and action
 * prompts. It integrates with the event system to automatically show
 * notifications for business events.
 */
class NotificationService {
public:
    /**
     * @brief Notification types
     */
    enum NotificationType {
        INFO,
        SUCCESS,
        WARNING,
        ERROR
    };
    
    /**
     * @brief Notification position on screen
     */
    enum NotificationPosition {
        TOP_RIGHT,
        TOP_LEFT,
        BOTTOM_RIGHT,
        BOTTOM_LEFT,
        TOP_CENTER,
        BOTTOM_CENTER
    };
    
    /**
     * @brief Action callback function type
     */
    using ActionCallback = std::function<void()>;
    
    /**
     * @struct NotificationConfig
     * @brief Configuration for notification appearance and behavior
     */
    struct NotificationConfig {
        NotificationPosition position = TOP_RIGHT;
        int defaultDuration = 3000;          // milliseconds
        int maxNotifications = 5;
        bool enableSounds = false;
        bool enableAnimations = true;
        bool autoStackManagement = true;
        
        NotificationConfig() = default;
    };
    
    /**
     * @brief Constructs the notification service
     * @param eventManager Shared event manager for automatic notifications
     */
    explicit NotificationService(std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~NotificationService() = default;
    
    /**
     * @brief Initializes the notification service
     * Sets up event listeners and notification container
     */
    void initialize();
    
    /**
     * @brief Shows an info notification
     * @param message Message to display
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    int showInfo(const std::string& message, int duration = 0);
    
    /**
     * @brief Shows a success notification
     * @param message Message to display
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    int showSuccess(const std::string& message, int duration = 0);
    
    /**
     * @brief Shows a warning notification
     * @param message Message to display
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    int showWarning(const std::string& message, int duration = 0);
    
    /**
     * @brief Shows an error notification
     * @param message Message to display
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    int showError(const std::string& message, int duration = 0);
    
    /**
     * @brief Shows a notification with custom type
     * @param message Message to display
     * @param type Notification type
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    int showNotification(const std::string& message, NotificationType type, int duration = 0);
    
    /**
     * @brief Shows a notification with an action button
     * @param message Message to display
     * @param type Notification type
     * @param actionLabel Label for the action button
     * @param callback Function to call when action is clicked
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    int showNotificationWithAction(const std::string& message,
                                  NotificationType type,
                                  const std::string& actionLabel,
                                  ActionCallback callback,
                                  int duration = 0);
    
    /**
     * @brief Dismisses a specific notification
     * @param notificationId ID of notification to dismiss
     * @return True if notification was found and dismissed
     */
    bool dismissNotification(int notificationId);
    
    /**
     * @brief Dismisses all notifications
     */
    void dismissAllNotifications();
    
    /**
     * @brief Dismisses all notifications of a specific type
     * @param type Type of notifications to dismiss
     */
    void dismissNotificationsByType(NotificationType type);
    
    /**
     * @brief Gets the current notification configuration
     * @return Current configuration settings
     */
    const NotificationConfig& getConfiguration() const;
    
    /**
     * @brief Updates notification configuration
     * @param config New configuration settings
     */
    void setConfiguration(const NotificationConfig& config);
    
    /**
     * @brief Gets the number of active notifications
     * @return Number of currently displayed notifications
     */
    size_t getActiveNotificationCount() const;
    
    /**
     * @brief Sets the notification container widget
     * @param container Container widget for notifications
     */
    void setNotificationContainer(Wt::WContainerWidget* container);
    
    /**
     * @brief Handles notification events from the event system
     * @param eventData Event data containing notification information
     */
    void handleNotification(const std::any& eventData);

protected:
    /**
     * @brief Sets up event listeners for automatic notifications
     */
    void setupEventListeners();
    
    /**
     * @brief Creates the notification container
     * @return Pointer to the notification container widget
     */
    Wt::WContainerWidget* createNotificationContainer();
    
    /**
     * @brief Creates a notification widget
     * @param message Message to display
     * @param type Notification type
     * @param actionLabel Optional action button label
     * @param callback Optional action callback
     * @return Unique pointer to the notification widget
     */
    std::unique_ptr<Wt::WContainerWidget> createNotificationWidget(
        const std::string& message,
        NotificationType type,
        const std::string& actionLabel = "",
        ActionCallback callback = nullptr);
    
    /**
     * @brief Gets CSS class for notification type
     * @param type Notification type
     * @return CSS class name
     */
    std::string getNotificationTypeClass(NotificationType type) const;
    
    /**
     * @brief Gets icon for notification type
     * @param type Notification type
     * @return Icon string (emoji or class name)
     */
    std::string getNotificationIcon(NotificationType type) const;
    
    /**
     * @brief Manages notification stack overflow
     */
    void manageNotificationStack();

private:
    /**
     * @struct ActiveNotification
     * @brief Information about an active notification
     */
    struct ActiveNotification {
        int id;
        Wt::WContainerWidget* widget;
        NotificationType type;
        std::unique_ptr<Wt::WTimer> timer;
        std::string message;
        
        ActiveNotification(int notificationId, Wt::WContainerWidget* w, 
                         NotificationType t, const std::string& msg)
            : id(notificationId), widget(w), type(t), message(msg) {}
    };
    
    // Dependencies
    std::shared_ptr<EventManager> eventManager_;
    
    // Configuration
    NotificationConfig config_;
    
    // UI components
    Wt::WContainerWidget* notificationContainer_;
    Wt::WApplication* application_;
    
    // Notification management
    std::vector<std::unique_ptr<ActiveNotification>> activeNotifications_;
    std::queue<POSEvents::NotificationEventData> pendingNotifications_;
    int nextNotificationId_;
    
    // Event subscriptions
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Helper methods
    void removeNotificationWidget(int notificationId);
    void processNotificationEvent(const POSEvents::NotificationEventData& data);
    void setupNotificationTimer(ActiveNotification* notification, int duration);
    void applyNotificationStyling(Wt::WContainerWidget* widget, NotificationType type);
    void positionNotificationContainer();
    
    // Business event handlers
    void handleOrderEvent(const std::any& eventData);
    void handlePaymentEvent(const std::any& eventData);
    void handleKitchenEvent(const std::any& eventData);
    void handleSystemError(const std::any& eventData);
};

#endif // NOTIFICATIONSERVICE_H

