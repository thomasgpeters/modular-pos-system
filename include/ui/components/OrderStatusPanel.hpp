#ifndef ORDERSTATUSPANEL_H
#define ORDERSTATUSPANEL_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"
#include "ActiveOrdersDisplay.hpp"
#include "KitchenStatusDisplay.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WTimer.h>

#include <memory>
#include <vector>

/**
 * @file OrderStatusPanel.hpp
 * @brief Order status panel component for the Restaurant POS System
 * 
 * This component manages the right panel of the POS interface, showing
 * active orders, kitchen status, and real-time updates.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class OrderStatusPanel
 * @brief UI component for displaying order status and kitchen information
 * 
 * The OrderStatusPanel manages the status display section of the POS,
 * including active orders tracking, kitchen queue status, and real-time
 * updates of order progress.
 */
class OrderStatusPanel : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the order status panel
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     */
    OrderStatusPanel(std::shared_ptr<POSService> posService,
                    std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~OrderStatusPanel() = default;
    
    /**
     * @brief Refreshes the panel data from the service
     */
    void refresh();
    
    /**
     * @brief Sets the auto-refresh interval
     * @param intervalSeconds Refresh interval in seconds (0 to disable)
     */
    void setAutoRefreshInterval(int intervalSeconds);
    
    /**
     * @brief Gets the current auto-refresh interval
     * @return Refresh interval in seconds
     */
    int getAutoRefreshInterval() const;
    
    /**
     * @brief Enables or disables auto-refresh
     * @param enabled True to enable auto-refresh
     */
    void setAutoRefreshEnabled(bool enabled);
    
    /**
     * @brief Checks if auto-refresh is enabled
     * @return True if auto-refresh is enabled
     */
    bool isAutoRefreshEnabled() const;

protected:
    /**
     * @brief Initializes the UI components
     */
    void initializeUI();
    
    /**
     * @brief Sets up event listeners
     */
    void setupEventListeners();
    
    /**
     * @brief Sets up auto-refresh timer
     */
    void setupAutoRefresh();
    
    /**
     * @brief Creates the panel header
     * @return Container widget with header
     */
    std::unique_ptr<Wt::WWidget> createPanelHeader();
    
    /**
     * @brief Creates the status summary section
     * @return Container widget with status summary
     */
    std::unique_ptr<Wt::WWidget> createStatusSummary();

private:
    // Services and dependencies
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // Configuration
    int autoRefreshInterval_;
    bool autoRefreshEnabled_;
    
    // UI components
    Wt::WText* statusTitleText_;
    Wt::WText* lastUpdateText_;
    Wt::WContainerWidget* statusSummaryContainer_;
    std::unique_ptr<ActiveOrdersDisplay> activeOrdersDisplay_;
    std::unique_ptr<KitchenStatusDisplay> kitchenStatusDisplay_;
    
    // Auto-refresh timer
    Wt::WTimer* refreshTimer_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers
    void handleOrderCreated(const std::any& eventData);
    void handleOrderModified(const std::any& eventData);
    void handleOrderSentToKitchen(const std::any& eventData);
    void handleOrderCompleted(const std::any& eventData);
    void handleKitchenStatusChanged(const std::any& eventData);
    
    // Timer handlers
    void onAutoRefreshTimer();
    
    // Helper methods
    void updateStatusSummary();
    void updateLastUpdateTime();
    std::string formatLastUpdateTime() const;
    int getActiveOrderCount() const;
    int getKitchenQueueSize() const;
    std::string getKitchenStatusSummary() const;
    
    // Constants
    static constexpr int DEFAULT_REFRESH_INTERVAL = 5; // seconds
};

#endif // ORDERSTATUSPANEL_H
