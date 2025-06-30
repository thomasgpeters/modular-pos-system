#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include "../events/EventManager.hpp"
#include "../events/POSEvents.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WTimer.h>
#include <Wt/WApplication.h>

#include <memory>
#include <queue>
#include <string>
#include <chrono>

/**
 * @file NotificationService.hpp
 * @brief Centralized notification management service
 * 
 * This service handles all user notifications including success messages,
 * warnings, errors, and informational alerts. It provides toast-style
 * notifications and manages their display, queuing, and auto-dismissal.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class NotificationService
 * @brief Service for managing user notifications and alerts
 * 
 * The NotificationService provides centralized notification management,
 * displaying messages to users in a non-intrusive toast-style format.
 * It handles notification queuing, auto-dismissal, and different severity levels.
 */
class NotificationService {
public:
    /**
     * @enum NotificationType
     * @brief Types of notifications supported
     */
    enum NotificationType {
        INFO,       ///< Informational message (blue)
        SUCCESS,    ///< Success message (green)
        WARNING,    ///< Warning message (yellow/orange)
        ERROR       ///< Error message (red)
    };
    
    /**
     * @struct NotificationData
     * @brief Data structure for a notification
     */
    struct NotificationData {
        std::string message;                        ///< Notification message
        NotificationType type;                      ///< Notification type
        int duration;                              ///< Duration in milliseconds (0 = permanent)
        std::chrono::steady_clock::time_point timestamp; ///< When notification was created
        std::string id;                            ///< Unique notification ID
        bool persistent;                           ///< Whether notification persists across page changes
        std::string actionText;                    ///< Optional action button text
        std::function<void()> actionCallback;      ///< Optional action callback
        
        NotificationData(const std::string& msg, NotificationType t = INFO, 
                        int dur = 3000, bool persist = false,
                        const std::string& action = "",
                        std::function<void()> callback = nullptr)
            : message(msg), type(t), duration(dur), 
              timestamp(std::chrono::steady_clock::now()),
              persistent(persist), actionText(action), actionCallback(callback) {
            // Generate unique ID
            id = "notification_" + std::to_string(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    timestamp.time_since_epoch()).count());
        }
    };
    
    /**
     * @brief Constructs the notification service
     * @param eventManager Shared event manager for listening to notification events
     */
    explicit NotificationService(std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~NotificationService() = default;
    
    /**
     * @brief Initializes the notification service
     * Sets up the notification container and event listeners
     */
    void initialize();
    
    // =================================================================
    // Notification Display Methods
    // =================================================================
    
    /**
     * @brief Shows an informational notification
     * @param message Message to display
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    std::string showInfo(const std::string& message, int duration = 3000);
    
    /**
     * @brief Shows a success notification
     * @param message Message to display
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    std::string showSuccess(const std::string& message, int duration = 3000);
    
    /**
     * @brief Shows a warning notification
     * @param message Message to display
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    std::string showWarning(const std::string& message, int duration = 5000);
    
    /**
     * @brief Shows an error notification
     * @param message Message to display
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    std::string showError(const std::string& message, int duration = 0);
    
    /**
     * @brief Shows a custom notification
     * @param notification Notification data structure
     * @return Notification ID for later reference
     */
    std::string showNotification(const NotificationData& notification);
    
    /**
     * @brief Shows a notification with an action button
     * @param message Message to display
     * @param type Notification type
     * @param actionText Text for the action button
     * @param actionCallback Function to call when action is clicked
     * @param duration Duration in milliseconds (0 = permanent)
     * @return Notification ID for later reference
     */
    std::string showNotificationWithAction(const std::string& message,
                                          NotificationType type,
                                          const std::string& actionText,
                                          std::function<void()> actionCallback,
                                          int duration = 0);
    
    // =================================================================
    // Notification Management
    // =================================================================
    
    /**
     * @brief Dismisses a notification by ID
     * @param notificationId ID of notification to dismiss
     * @return True if notification was found and dismissed
     */
    bool dismissNotification(const std::string& notificationId);
    
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
     * @brief Gets the number of active notifications
     * @return Number of currently displayed notifications
     */
    size_t getActiveNotificationCount() const;
    
    /**
     * @brief Gets all active notification IDs
     * @return Vector of active notification IDs
     */
    std::vector<std::string> getActiveNotificationIds() const;
    
    /**
     * @brief Checks if a notification is currently active
     * @param notificationId ID to check
     * @return True if notification is active
     */
    bool isNotificationActive(const std::string& notificationId) const;
    
    // =================================================================
    // Event Handling
    // =================================================================
    
    /**
     * @brief Handles notification events from the event system
     * @param eventData Event data containing notification information
     */
    void handleNotification(const std::any& eventData);
    
    // =================================================================
    // Configuration
    // =================================================================
    
    /**
     * @brief Sets the maximum number of notifications to display
     * @param maxCount Maximum number of notifications (0 = unlimited)
     */
    void setMaxNotificationCount(int maxCount);
    
    /**
     * @brief Gets the maximum notification count
     * @return Maximum number of notifications
     */
    int getMaxNotificationCount() const;
    
    /**
     * @brief Sets the default notification duration
     * @param duration Default duration in milliseconds
     */
    void setDefaultDuration(int duration);
    
    /**
     * @brief Gets the default notification duration
     * @return Default duration in milliseconds
     */
    int getDefaultDuration() const;
    
    /**
     * @brief Enables or disables notification sounds
     * @param enabled True to enable sounds
     */
    void setSoundsEnabled(bool enabled);
    
    /**
     * @brief Checks if notification sounds are enabled
     * @return True if sounds are enabled
     */
    bool areSoundsEnabled() const;
    
    /**
     * @brief Sets the notification container position
     * @param position CSS position ("top-right", "top-left", "bottom-right", "bottom-left")
     */
    void setNotificationPosition(const std::string& position);

protected:
    /**
     * @brief Sets up the notification container UI
     */
    void setupNotificationContainer();
    
    /**
     * @brief Sets up event listeners for notification events
     */
    void setupEventListeners();
    
    /**
     * @brief Creates a notification widget
     * @param notification Notification data
     * @return Widget representing the notification
     */
    std::unique_ptr<Wt::WWidget> createNotificationWidget(const NotificationData& notification);
    
    /**
     * @brief Adds a notification to the display queue
     * @param notification Notification to add
     */
    void addNotificationToQueue(const NotificationData& notification);
    
    /**
     * @brief Processes the notification queue
     */
    void processNotificationQueue();
    
    /**
     * @brief Auto-dismisses expired notifications
     */
    void autoremoveExpiredNotifications();

private:
    // Dependencies
    std::shared_ptr<EventManager> eventManager_;
    
    // UI components
    Wt::WContainerWidget* notificationContainer_;
    
    // Notification management
    std::queue<NotificationData> notificationQueue_;
    std::vector<NotificationData> activeNotifications_;
    std::map<std::string, Wt::WWidget*> notificationWidgets_;
    
    // Timers
    Wt::WTimer* autoRemovalTimer_;
    Wt::WTimer* queueProcessTimer_;
    
    // Configuration
    int maxNotificationCount_;
    int defaultDuration_;
    bool soundsEnabled_;
    std::string notificationPosition_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Helper methods
    std::string getNotificationIcon(NotificationType type) const;
    std::string getNotificationCssClass(NotificationType type) const;
    std::string getNotificationTypeString(NotificationType type) const;
    void playNotificationSound(NotificationType type);
    void removeNotificationWidget(const std::string& notificationId);
    void repositionNotifications();
    void enforceMaxNotificationLimit();
    
    // Event handlers
    void handleSystemError(const std::any& eventData);
    void handleOrderCreated(const std::any& eventData);
    void handleOrderCompleted(const std::any& eventData);
    void handlePaymentProcessed(const std::any& eventData);
    void handleKitchenStatusChanged(const std::any& eventData);
    
    // Constants
    static constexpr int DEFAULT_MAX_NOTIFICATIONS = 5;
    static constexpr int DEFAULT_DURATION_MS = 3000;
    static constexpr int AUTO_REMOVAL_CHECK_INTERVAL_MS = 1000;
    static constexpr int QUEUE_PROCESS_INTERVAL_MS = 100;
};

#endif // NOTIFICATIONSERVICE_H
