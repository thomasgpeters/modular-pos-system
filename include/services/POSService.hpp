#ifndef POSSERVICE_H
#define POSSERVICE_H

#include "../Order.hpp"
#include "../MenuItem.hpp"
#include "../OrderManager.hpp"
#include "../KitchenInterface.hpp"
#include "../PaymentProcessor.hpp"
#include "../events/EventManager.hpp"
#include "../events/POSEvents.hpp"

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

/**
 * @file POSService.hpp
 * @brief Main service class for the Restaurant POS System
 * 
 * This service class coordinates between all the major components:
 * OrderManager, KitchenInterface, PaymentProcessor, and EventManager.
 * 
 * @author Restaurant POS Team
 * @version 2.1.0 - Enhanced with quantity/instructions support and proper events
 */

/**
 * @class POSService
 * @brief Main service coordinator for the POS system
 * 
 * The POSService acts as a facade and coordinator between the major
 * subsystems of the POS: orders, kitchen, payments, and events.
 */
class POSService {
public:
    /**
     * @brief Constructs the POS service
     * @param eventManager Event manager for notifications
     */
    explicit POSService(std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~POSService() = default;
    
    // =====================================================================
    // Order Management Methods (Delegate to OrderManager)
    // =====================================================================
    
    /**
     * @brief Creates a new order for a table identifier
     * @param tableIdentifier Table identifier (e.g., "table 5", "walk-in", "grubhub")
     * @return Shared pointer to the created order
     */
    std::shared_ptr<Order> createOrder(const std::string& tableIdentifier);
    
    /**
     * @brief Creates a new order for a table number (legacy)
     * @param tableNumber Table number for the order
     * @return Shared pointer to the created order
     */
    std::shared_ptr<Order> createOrder(int tableNumber);
    
    /**
     * @brief Gets all active orders
     * @return Vector of active orders
     */
    std::vector<std::shared_ptr<Order>> getActiveOrders() const;
    
    /**
     * @brief Gets an order by its ID
     * @param orderId Order ID to search for
     * @return Shared pointer to order, or nullptr if not found
     */
    std::shared_ptr<Order> getOrderById(int orderId) const;
    
    /**
     * @brief Gets the current order being worked on
     * @return Shared pointer to current order, or nullptr if none
     */
    std::shared_ptr<Order> getCurrentOrder() const;
    
    /**
     * @brief Sets the current order
     * @param order Order to set as current
     */
    void setCurrentOrder(std::shared_ptr<Order> order);
    
    /**
     * @brief Cancels an order
     * @param orderId Order ID to cancel
     * @return True if cancellation successful
     */
    bool cancelOrder(int orderId);
    
    /**
     * @brief Checks if a table identifier is in use
     * @param tableIdentifier Table identifier to check
     * @return True if in use
     */
    bool isTableIdentifierInUse(const std::string& tableIdentifier) const;
    
    /**
     * @brief Validates a table identifier format
     * @param tableIdentifier Table identifier to validate
     * @return True if valid format
     */
    bool isValidTableIdentifier(const std::string& tableIdentifier) const;
    
    // =====================================================================
    // Current Order Management (ENHANCED)
    // =====================================================================
    
    /**
     * @brief Adds an item to the current order (legacy method)
     * @param item Menu item to add
     * @return True if successful
     */
    bool addItemToCurrentOrder(std::shared_ptr<MenuItem> item);
    
    /**
     * @brief Adds an item to the current order with quantity and instructions
     * @param item Menu item to add
     * @param quantity Number of items to add
     * @param instructions Special preparation instructions
     * @return True if successful
     */
    bool addItemToCurrentOrder(const MenuItem& item, int quantity = 1, const std::string& instructions = "");
    
    /**
     * @brief Removes an item from current order by index
     * @param itemIndex Index of item to remove
     * @return True if successful
     */
    bool removeItemFromCurrentOrder(size_t itemIndex);
    
    /**
     * @brief Updates quantity of item in current order
     * @param itemIndex Index of item to update
     * @param newQuantity New quantity
     * @return True if successful
     */
    bool updateCurrentOrderItemQuantity(size_t itemIndex, int newQuantity);
    
    /**
     * @brief Sends current order to kitchen
     * @return True if successful
     */
    bool sendCurrentOrderToKitchen();
    
    // =====================================================================
    // Kitchen Interface Methods
    // =====================================================================
    
    /**
     * @brief Gets kitchen tickets
     * @return Vector of kitchen tickets
     */
    std::vector<KitchenInterface::KitchenTicket> getKitchenTickets() const;
    
    /**
     * @brief Gets estimated wait time
     * @return Wait time in minutes
     */
    int getEstimatedWaitTime() const;
    
    /**
     * @brief Gets kitchen queue status as JSON
     * @return Kitchen queue status
     */
    Wt::Json::Object getKitchenQueueStatus() const;
    
    // =====================================================================
    // Menu Management Methods (ENHANCED)
    // =====================================================================
    
    /**
     * @brief Gets all menu items
     * @return Vector of menu items
     */
    std::vector<std::shared_ptr<MenuItem>> getMenuItems() const;
    
    /**
     * @brief Gets menu items by category
     * @param category Category to filter by
     * @return Vector of menu items in category
     */
    std::vector<std::shared_ptr<MenuItem>> getMenuItemsByCategory(MenuItem::Category category) const;
    
    /**
     * @brief Gets a menu item by its ID
     * @param itemId ID of the menu item to find
     * @return Shared pointer to menu item, or nullptr if not found
     */
    std::shared_ptr<MenuItem> getMenuItemById(int itemId) const;
    
    /**
     * @brief Refreshes the menu and publishes update event
     */
    void refreshMenu();
    
    // =====================================================================
    // Payment Processing Methods
    // =====================================================================
    
    /**
     * @brief Processes payment for an order
     * @param order Order to process payment for
     * @param method Payment method
     * @param amount Payment amount
     * @param tipAmount Tip amount
     * @return Payment result
     */
    PaymentProcessor::PaymentResult processPayment(
        std::shared_ptr<Order> order,
        PaymentProcessor::PaymentMethod method,
        double amount,
        double tipAmount = 0.0);
    
    /**
     * @brief Gets transaction history
     * @return Vector of payment results
     */
    std::vector<PaymentProcessor::PaymentResult> getTransactionHistory() const;
    
    // =====================================================================
    // Configuration Methods (for UIComponentFactory)
    // =====================================================================
    
    /**
     * @brief Gets enabled payment methods
     * @return Vector of payment method strings
     */
    std::vector<std::string> getEnabledPaymentMethods() const;
    
    /**
     * @brief Gets tip suggestions
     * @return Vector of tip percentages
     */
    std::vector<double> getTipSuggestions() const;

    /**
     * @brief Initializes the menu items (called by RestaurantPOSApp)
     */
    void initializeMenu();
    
    /**
     * @brief Registers a callback for order created events
     * @param callback Function to call when orders are created
     */
    void onOrderCreated(std::function<void(std::shared_ptr<Order>)> callback);
    
    /**
     * @brief Registers a callback for order modified events
     * @param callback Function to call when orders are modified
     */
    void onOrderModified(std::function<void(std::shared_ptr<Order>)> callback);

private:
    // Core subsystem components
    std::shared_ptr<EventManager> eventManager_;
    std::unique_ptr<OrderManager> orderManager_;
    std::unique_ptr<KitchenInterface> kitchenInterface_;
    std::unique_ptr<PaymentProcessor> paymentProcessor_;
    
    // Current state
    std::shared_ptr<Order> currentOrder_;
    
    // Menu items storage
    std::vector<std::shared_ptr<MenuItem>> menuItems_;
    
    // UI callback functions
    std::function<void(std::shared_ptr<Order>)> orderCreatedCallback_;
    std::function<void(std::shared_ptr<Order>)> orderModifiedCallback_;
    
    // Helper methods
    void initializeMenuItems();
    void initializeSubsystems();
    std::vector<std::string> convertOrderItemsToStringList(const std::vector<OrderItem>& items) const;
};

#endif // POSSERVICE_H
