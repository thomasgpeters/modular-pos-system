#ifndef POSMODECONTAINER_H
#define POSMODECONTAINER_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../utils/UIStyleHelper.hpp"

#include "../components/ActiveOrdersDisplay.hpp"
#include "../components/OrderEntryPanel.hpp"
#include "../components/MenuDisplay.hpp"
#include "../components/CurrentOrderDisplay.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>

#include <memory>

/**
 * @class POSModeContainer
 * @brief Container for POS mode layout with proper panel management
 * 
 * Layout modes:
 * - Order Entry Mode: Left Panel = Active Orders, Right Panel = Order Entry
 * - Order Edit Mode: Left Panel = Menu Display, Right Panel = Current Order
 */
class POSModeContainer : public Wt::WContainerWidget {
public:
    /**
     * @enum UIMode
     * @brief Track current UI display mode to avoid unnecessary recreation
     */
    enum UIMode {
        UI_MODE_NONE,
        UI_MODE_ORDER_ENTRY,    // Active Orders (left) + Order Entry (right)
        UI_MODE_ORDER_EDIT      // Menu Display (left) + Current Order (right)
    };
    
    /**
     * @brief Constructs the POS mode container
     * @param posService POS service for business operations
     * @param eventManager Event manager for notifications
     */
    POSModeContainer(std::shared_ptr<POSService> posService,
                    std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~POSModeContainer();
    
    /**
     * @brief Refreshes all components (smart refresh - only changes mode if needed)
     */
    void refresh();
    
    /**
     * @brief Refreshes only data without recreating UI components (for periodic updates)
     */
    void refreshDataOnly();
    
    /**
     * @brief Creates a new order
     * @param tableIdentifier Table identifier for the new order
     */
    void createNewOrder(const std::string& tableIdentifier);
    
    /**
     * @brief Opens an existing order for editing
     * @param order Order to open for editing
     */
    void openOrderForEditing(std::shared_ptr<Order> order);

    /**
     * @brief Opens an existing order for editing by ID
     * @param orderId ID of the order to open
     */
    void openOrderForEditing(int orderId);

    /**
     * @brief Checks if an order is currently being edited
     * @return True if an order is being edited
     */
    bool hasCurrentOrder() const;

protected:
    /**
     * @brief Initializes the UI components
     */
    void initializeUI();
    
    /**
     * @brief Sets up the main layout (left and right panels)
     */
    void setupLayout();
    
    /**
     * @brief Creates the left panel container (will hold Active Orders or Menu Display)
     */
    void createLeftPanel();
    
    /**
     * @brief Creates the right panel container (will hold Order Entry or Current Order)
     */
    void createRightPanel();
    
    /**
     * @brief Sets up event listeners
     */
    void setupEventListeners();
    
    /**
     * @brief Updates the work area based on current state
     */
    void updateWorkArea();
    
    /**
     * @brief Switches to order entry mode (Active Orders + Order Entry Panel)
     */
    void showOrderEntryMode();
    
    /**
     * @brief Switches to order edit mode (Menu Display + Current Order Display)
     */
    void showOrderEditMode();
    
    /**
     * @brief Shows the order entry interface (Active Orders + Order Entry Panel)
     */
    void showOrderEntry();
    
    /**
     * @brief Shows the order edit interface (Menu Display + Current Order Display)
     */
    void showOrderEdit();
    
    /**
     * @brief Shows the Active Orders display (switches back to order entry mode)
     */
    void showActiveOrdersDisplay();
    
    /**
     * @brief Hides the Active Orders display (legacy method - kept for compatibility)
     * @deprecated Use showOrderEdit() instead
     */
    void hideActiveOrdersDisplay();
    
    /**
     * @brief Sends the current order to kitchen and returns to order entry mode
     */
    void sendCurrentOrderToKitchen();
    
    /**
     * @brief Clears the left panel safely
     */
    void clearLeftPanel();
    
    /**
     * @brief Clears the work area safely
     */
    void clearWorkArea();

private:
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // Layout containers
    Wt::WContainerWidget* leftPanel_;   // Holds Active Orders OR Menu Display
    Wt::WContainerWidget* rightPanel_;  // Holds Order Entry OR Current Order
    Wt::WContainerWidget* workArea_;    // Dynamic content area within right panel
    
    // Components (only one set active at a time)
    ActiveOrdersDisplay* activeOrdersDisplay_;      // In Order Entry mode (left panel)
    OrderEntryPanel* orderEntryPanel_;              // In Order Entry mode (right panel)
    MenuDisplay* menuDisplay_;                      // In Order Edit mode (left panel)
    CurrentOrderDisplay* currentOrderDisplay_;     // In Order Edit mode (right panel)
    
    // UI control elements
    Wt::WText* workAreaTitle_;
    Wt::WPushButton* sendToKitchenButton_;      // Send order to kitchen
    Wt::WPushButton* toggleOrdersButton_;       // Toggle back to Active Orders view
    
    // State management
    UIMode currentUIMode_;                      // Track current UI mode
    bool isDestroying_;                         // Destruction safety flag
    
    // Event handlers
    void handleOrderCreated(const std::any& eventData);
    void handleCurrentOrderChanged(const std::any& eventData);
    
    // Helper methods
    bool hasOrderWithItems() const;
    void updateSendToKitchenButton();
    void showOrderSentFeedback(int orderId);
    
    // Event subscriptions
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
};

#endif // POSMODECONTAINER_H
