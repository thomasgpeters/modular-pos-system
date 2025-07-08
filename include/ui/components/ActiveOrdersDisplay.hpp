#ifndef ACTIVEORDERSDISPLAY_H
#define ACTIVEORDERSDISPLAY_H

#include "../../utils/UIStyleHelper.hpp" // Include our styling helper

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"
#include "../../Order.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WMessageBox.h>

#include <memory>
#include <vector>

/**
 * @file ActiveOrdersDisplay.hpp
 * @brief Active orders display component for the Restaurant POS System
 * 
 * This component displays and manages active orders in the system.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class ActiveOrdersDisplay
 * @brief UI component for displaying and managing active orders
 * 
 * The ActiveOrdersDisplay shows all currently active orders with
 * functionality to view details, complete orders, and cancel orders.
 */
class ActiveOrdersDisplay : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the active orders display
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     */
    ActiveOrdersDisplay(std::shared_ptr<POSService> posService,
                       std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ActiveOrdersDisplay() = default;
    
    /**
     * @brief Refreshes the orders display from the service
     */
    void refresh();
    
    /**
     * @brief Sets the maximum number of orders to display
     * @param maxOrders Maximum orders to show (0 = no limit)
     */
    void setMaxOrdersToDisplay(int maxOrders);
    
    /**
     * @brief Gets the maximum number of orders displayed
     * @return Maximum orders displayed
     */
    int getMaxOrdersToDisplay() const;
    
    /**
     * @brief Sets whether to show completed orders
     * @param showCompleted True to show completed orders
     */
    void setShowCompletedOrders(bool showCompleted);
    
    /**
     * @brief Gets whether completed orders are shown
     * @return True if completed orders are shown
     */
    bool getShowCompletedOrders() const;

protected:
    /**
     * @brief Initializes the UI components
     */
    void initializeUI();
    
    /**
     * @brief Initializes table headers
     */
    void initializeTableHeaders();
    
    /**
     * @brief Sets up event listeners
     */
    void setupEventListeners();
    
    /**
     * @brief Creates the display header
     * @return Container widget with header
     */
    std::unique_ptr<Wt::WWidget> createDisplayHeader();
    
    /**
     * @brief Updates the orders table with current data
     */
    void updateOrdersTable();
    
    /**
     * @brief Adds a row for an order to the table
     * @param order Order to add
     * @param row Row number in the table
     */
    void addOrderRow(std::shared_ptr<Order> order, int row);
    
    /**
     * @brief Applies consistent styling to a table row
     * @param row Row number to style
     * @param isEven True if this is an even-numbered row
     */
    void applyRowStyling(int row, bool isEven);

private:
    // Services and dependencies
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // Configuration
    int maxOrdersToDisplay_;
    bool showCompletedOrders_;
    
    // UI components (raw pointers - Wt manages lifetime)
    Wt::WText* headerText_;
    Wt::WText* orderCountText_;
    Wt::WTable* ordersTable_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers
    void handleOrderCreated(const std::any& eventData);
    void handleOrderModified(const std::any& eventData);
    void handleOrderCompleted(const std::any& eventData);
    void handleOrderCancelled(const std::any& eventData);
    void handleKitchenStatusChanged(const std::any& eventData);
    
    // UI action handlers
    void onViewOrderClicked(int orderId);
    void onCompleteOrderClicked(int orderId);
    void onCancelOrderClicked(int orderId);
    
    // Helper methods
    std::vector<std::shared_ptr<Order>> getDisplayOrders() const;
    std::string formatOrderId(int orderId) const;
    std::string formatOrderStatus(Order::Status status) const;
    std::string formatOrderTime(const std::shared_ptr<Order>& order) const;
    std::string formatCurrency(double amount) const;
    
    /**
     * @brief Gets the appropriate badge variant for an order status
     * @param status Order status
     * @return Bootstrap badge variant string (e.g., "success", "warning", "danger")
     */
    std::string getStatusBadgeVariant(Order::Status status) const;
    
    void showEmptyOrdersMessage();
    void hideEmptyOrdersMessage();
    void applyTableStyling();
    void updateOrderCount();
    
    // Constants
    static constexpr int DEFAULT_MAX_ORDERS = 20;
};

#endif // ACTIVEORDERSDISPLAY_H
