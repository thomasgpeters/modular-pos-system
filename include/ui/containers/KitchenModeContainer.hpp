//============================================================================
// KITCHEN MODE CONTAINER
//============================================================================

#ifndef KITCHENMODECONTAINER_H
#define KITCHENMODECONTAINER_H

#include "../../services/POSService.hpp"
#include "../../events/POSEvents.hpp"
#include "../../events/EventManager.hpp"
#include "../components/ActiveOrdersDisplay.hpp"
#include "../components/KitchenStatusDisplay.hpp"
#include "../../utils/UIStyleHelper.hpp"


#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WText.h>

#include <memory>

/**
 * @class KitchenModeContainer
 * @brief Container for Kitchen mode layout and components
 */
class KitchenModeContainer : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the Kitchen mode container
     * @param posService POS service for business operations
     * @param eventManager Event manager for notifications
     */
    KitchenModeContainer(std::shared_ptr<POSService> posService,
                        std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Refreshes all components
     */
    void refresh();
    
    /**
     * @brief Opens an order for kitchen preparation details
     * @param order Order to view details for
     */
    void viewOrderDetails(std::shared_ptr<Order> order);

protected:
    void initializeUI();
    void setupLayout();
    void createLeftPanel();   // Active Orders (Kitchen View)
    void createRightPanel();  // Kitchen Status
    void setupEventListeners();

private:
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // Layout containers
    Wt::WContainerWidget* leftPanel_;
    Wt::WContainerWidget* rightPanel_;
    
    // Components
    ActiveOrdersDisplay* activeOrdersDisplay_;
    KitchenStatusDisplay* kitchenStatusDisplay_;
    
    // UI elements
    Wt::WText* modeTitle_;
    
    // Event handlers
    void handleKitchenStatusChanged(const std::any& eventData);
    void handleOrderStatusChanged(const std::any& eventData);
    
    // Event subscriptions
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
};

#endif // KITCHENMODECONTAINER_H
