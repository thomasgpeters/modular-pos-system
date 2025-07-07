#ifndef POSMODECONTAINER_H
#define POSMODECONTAINER_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"

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
 * @brief Container for POS mode layout and components
 */
class POSModeContainer : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the POS mode container
     * @param posService POS service for business operations
     * @param eventManager Event manager for notifications
     */
    POSModeContainer(std::shared_ptr<POSService> posService,
                    std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Refreshes all components
     */
    void refresh();
    
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
     * @brief Closes the current order being edited
     */
    void closeCurrentOrder();
    
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
     * @brief Sets up the layout
     */
    void setupLayout();
    
    /**
     * @brief Creates the left panel (Active Orders)
     */
    void createLeftPanel();
    
    /**
     * @brief Creates the right panel (Order Work Area)
     */
    void createRightPanel();
    
    /**
     * @brief Creates the order entry area (for new orders)
     */
    void createOrderEntryArea();
    
    /**
     * @brief Creates the order edit area (for editing existing orders)
     */
    void createOrderEditArea();
    
    /**
     * @brief Sets up event listeners
     */
    void setupEventListeners();
    
    /**
     * @brief Updates the work area based on current state
     */
    void updateWorkArea();
    
    /**
     * @brief Shows the order entry interface
     */
    void showOrderEntry();
    
    /**
     * @brief Shows the order edit interface
     */
    void showOrderEdit();

private:
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // Layout containers
    Wt::WContainerWidget* leftPanel_;
    Wt::WContainerWidget* rightPanel_;
    Wt::WContainerWidget* workArea_;
    
    // Components
    ActiveOrdersDisplay* activeOrdersDisplay_;
    OrderEntryPanel* orderEntryPanel_;
    MenuDisplay* menuDisplay_;
    CurrentOrderDisplay* currentOrderDisplay_;
    
    // UI state
    Wt::WText* workAreaTitle_;
    Wt::WContainerWidget* orderEntryArea_;
    Wt::WContainerWidget* orderEditArea_;
    Wt::WPushButton* newOrderButton_;
    Wt::WPushButton* closeOrderButton_;
    
    // Event handlers
    void handleOrderCreated(const std::any& eventData);
    void handleCurrentOrderChanged(const std::any& eventData);
    void handleOrderSelected(int orderId);
    
    // Event subscriptions
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
};

#endif // POSMODECONTAINER_H
