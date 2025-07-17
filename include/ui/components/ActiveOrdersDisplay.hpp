//============================================================================
// Enhanced ActiveOrdersDisplay.hpp - API Integration Header
//============================================================================

#ifndef ACTIVEORDERSDISPLAY_H
#define ACTIVEORDERSDISPLAY_H

#include "../../utils/UIStyleHelper.hpp"
#include "../../services/EnhancedPOSService.hpp"
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
 * @brief Enhanced active orders display with API integration
 * 
 * This component displays and manages active orders, supporting both
 * local data (fallback) and API data fetching for real-time updates.
 * 
 * @author Restaurant POS Team
 * @version 3.0.0 - Enhanced with API integration
 */

/**
 * @class ActiveOrdersDisplay
 * @brief UI component for displaying and managing active orders with API support
 * 
 * The ActiveOrdersDisplay shows all currently active orders with
 * functionality to view details, complete orders, and cancel orders.
 * Enhanced with API integration for real-time data from the middleware.
 */
class ActiveOrdersDisplay : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the active orders display
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     * @param showHeader Whether to show the component header (default: true)
     */
    ActiveOrdersDisplay(std::shared_ptr<POSService> posService,
                       std::shared_ptr<EventManager> eventManager,
                       bool showHeader = true);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ActiveOrdersDisplay() = default;
    
    /**
     * @brief Refreshes the orders display from API or local service
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
    
    /**
     * @brief Applies current theme to all components
     */
    void applyCurrentTheme();

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
     * @brief Loads orders from API (async)
     */
    void loadOrdersFromAPI();
    
    /**
     * @brief Displays orders received from API
     * @param orders Vector of orders from API
     */
    void displayAPIOrders(const std::vector<std::shared_ptr<Order>>& orders);
    
    /**
     * @brief Displays orders from local service (fallback)
     */
    void displayLocalOrders();
    
    /**
     * @brief Filters orders based on display settings
     * @param orders Input orders to filter
     * @return Filtered orders
     */
    std::vector<std::shared_ptr<Order>> filterOrders(const std::vector<std::shared_ptr<Order>>& orders) const;
    
    /**
     * @brief Shows loading state while fetching from API
     */
    void showLoadingState();
    
    /**
     * @brief Clears loading state
     */
    void clearLoadingState();
    
    /**
     * @brief Updates order count directly with specified value
     * @param count Number of orders to display in badge
     */
    void updateOrderCountDirect(int count);
    
    /**
     * @brief Updates the orders table with current data (legacy method)
     */
    void updateOrdersTable() { refresh(); }
    
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
 
    // Show or hide the Header for this component
    bool showHeader_;

    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers (enhanced for API integration)
    void handleOrderCreated(const std::any& eventData);
    void handleOrderModified(const std::any& eventData);
    void handleOrderCompleted(const std::any& eventData);
    void handleOrderCancelled(const std::any& eventData);
    void handleKitchenStatusChanged(const std::any& eventData);
    
    // UI action handlers (enhanced for API integration)
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
    
    void createHeader();
    void showEmptyOrdersMessage();
    void hideEmptyOrdersMessage();
    void applyTableStyling();
    
    /**
     * @brief Updates order count (legacy compatibility method)
     */
    void updateOrderCount();
    
    // Constants
    static constexpr int DEFAULT_MAX_ORDERS = 20;
};

#endif // ACTIVEORDERSDISPLAY_H
