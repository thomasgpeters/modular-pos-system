#ifndef KITCHENSTATUSDISPLAY_H
#define KITCHENSTATUSDISPLAY_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"
#include "../../KitchenInterface.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WProgressBar.h>

#include <memory>
#include <vector>

/**
 * @file KitchenStatusDisplay.hpp
 * @brief Kitchen status display component for the Restaurant POS System
 * 
 * This component displays kitchen queue status, preparation times,
 * and kitchen performance metrics.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class KitchenStatusDisplay
 * @brief UI component for displaying kitchen status and metrics
 * 
 * The KitchenStatusDisplay shows real-time kitchen information including
 * queue size, estimated wait times, current tickets, and kitchen load.
 */
class KitchenStatusDisplay : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the kitchen status display
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     */
    KitchenStatusDisplay(std::shared_ptr<POSService> posService,
                        std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~KitchenStatusDisplay() = default;
    
    /**
     * @brief Refreshes the kitchen status from the service
     */
    void refresh();
    
    /**
     * @brief Sets whether to show detailed kitchen metrics
     * @param showDetailed True to show detailed metrics
     */
    void setShowDetailedMetrics(bool showDetailed);
    
    /**
     * @brief Checks if detailed metrics are shown
     * @return True if detailed metrics are shown
     */
    bool getShowDetailedMetrics() const;

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
     * @brief Creates the status header
     * @return Container widget with header
     */
    std::unique_ptr<Wt::WWidget> createStatusHeader();
    
    /**
     * @brief Creates the kitchen metrics section
     * @return Container widget with metrics
     */
    std::unique_ptr<Wt::WWidget> createKitchenMetrics();
    
    /**
     * @brief Creates the queue status section
     * @return Container widget with queue status
     */
    std::unique_ptr<Wt::WWidget> createQueueStatus();
    
    /**
     * @brief Updates all status displays
     */
    void updateAllStatus();
    
    /**
     * @brief Updates the kitchen metrics
     */
    void updateKitchenMetrics();
    
    /**
     * @brief Updates the queue status
     */
    void updateQueueStatus();

private:
    // Services and dependencies
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // Configuration
    bool showDetailedMetrics_;
    
    // UI components
    Wt::WText* statusHeaderText_;
    Wt::WText* queueSizeText_;
    Wt::WText* estimatedWaitText_;
    Wt::WText* kitchenLoadText_;
    Wt::WProgressBar* kitchenLoadBar_;
    Wt::WContainerWidget* metricsContainer_;
    Wt::WContainerWidget* queueContainer_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers
    void handleOrderSentToKitchen(const std::any& eventData);
    void handleKitchenStatusChanged(const std::any& eventData);
    void handleKitchenQueueUpdated(const std::any& eventData);
    
    // Helper methods
    std::string getKitchenStatusText() const;
    std::string getKitchenStatusIcon() const;
    std::string getKitchenStatusColor() const;
    int calculateKitchenLoad() const;
    std::string formatWaitTime(int minutes) const;
    std::string formatKitchenMetrics() const;
    std::string formatKitchenTicketStatus(KitchenInterface::KitchenStatus status) const; // ADDED: Missing method declaration
    void updateStatusColors();
    
    // Status calculation methods
    bool isKitchenBusy() const;
    bool isKitchenOverloaded() const;
    int getAveragePreparationTime() const;
    
    // Constants
    static constexpr int BUSY_THRESHOLD = 5;
    static constexpr int OVERLOADED_THRESHOLD = 10;
    static constexpr int MAX_REASONABLE_WAIT = 45; // minutes
};

#endif // KITCHENSTATUSDISPLAY_H