#ifndef ORDERENTRYPANEL_H
#define ORDERENTRYPANEL_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"
#include "MenuDisplay.hpp"
#include "CurrentOrderDisplay.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WSpinBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

#include <memory>

/**
 * @file OrderEntryPanel.hpp
 * @brief Order entry UI component for the Restaurant POS System
 * 
 * This component handles the left panel of the POS interface, including
 * table selection, menu display, and current order management.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class OrderEntryPanel
 * @brief UI component for order entry and menu selection
 * 
 * The OrderEntryPanel manages the order creation workflow, including
 * table number selection, menu browsing, and current order building.
 * It coordinates with the POSService for business operations and uses
 * the event system for communication with other components.
 */
class OrderEntryPanel : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the order entry panel
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     */
    OrderEntryPanel(std::shared_ptr<POSService> posService,
                   std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~OrderEntryPanel() = default;
    
    /**
     * @brief Refreshes the panel data from the service
     */
    void refresh();
    
    /**
     * @brief Sets the current table number
     * @param tableNumber Table number to set
     */
    void setTableNumber(int tableNumber);
    
    /**
     * @brief Gets the current table number
     * @return Current table number
     */
    int getTableNumber() const;
    
    /**
     * @brief Enables or disables the order entry controls
     * @param enabled True to enable, false to disable
     */
    void setOrderEntryEnabled(bool enabled);

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
     * @brief Creates the table selection section
     * @return Container widget with table selection controls
     */
    std::unique_ptr<Wt::WWidget> createTableSelectionSection();
    
    /**
     * @brief Creates the order actions section
     * @return Container widget with order action buttons
     */
    std::unique_ptr<Wt::WWidget> createOrderActionsSection();

private:
    // Services and dependencies
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // UI components
    Wt::WSpinBox* tableNumberEdit_;
    Wt::WPushButton* newOrderButton_;
    Wt::WPushButton* sendToKitchenButton_;
    Wt::WPushButton* processPaymentButton_;
    
    // Child components
    std::unique_ptr<MenuDisplay> menuDisplay_;
    std::unique_ptr<CurrentOrderDisplay> currentOrderDisplay_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers
    void handleOrderCreated(const std::any& eventData);
    void handleOrderModified(const std::any& eventData);
    void handleCurrentOrderChanged(const std::any& eventData);
    
    // UI action handlers
    void onNewOrderClicked();
    void onSendToKitchenClicked();
    void onProcessPaymentClicked();
    void onTableNumberChanged();
    
    // Helper methods
    void updateOrderActionButtons();
    void validateCurrentOrder();
    bool hasCurrentOrder() const;
    void showOrderValidationError(const std::string& message);
};

#endif // ORDERENTRYPANEL_H

