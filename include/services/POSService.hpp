#ifndef POSSERVICE_H
#define POSSERVICE_H

#include "../OrderManager.hpp"
#include "../PaymentProcessor.hpp"
#include "../KitchenInterface.hpp"
#include "../MenuItem.hpp"
#include "../events/EventManager.hpp"
#include "../events/POSEvents.hpp"

#include <memory>
#include <vector>
#include <functional>

/**
 * @file POSService.hpp
 * @brief Central business logic coordinator for the Restaurant POS System
 * 
 * This service acts as the main business logic coordinator, providing a clean
 * interface between the UI layer and the core business components. It handles
 * all business operations and coordinates between the three-legged foundation.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class POSService
 * @brief Central service for coordinating all POS business operations
 * 
 * The POSService acts as a facade and coordinator for all business operations
 * in the POS system. It provides a clean, high-level interface for UI components
 * while managing the complexity of coordinating between OrderManager, 
 * PaymentProcessor, and KitchenInterface.
 */
class POSService {
public:
    /**
     * @brief Constructs the POS service with dependencies
     * @param eventManager Shared event manager for component communication
     */
    explicit POSService(std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~POSService() = default;
    
    // =================================================================
    // Menu Management
    // =================================================================
    
    /**
     * @brief Initializes the menu with sample items
     * This method populates the menu with default restaurant items
     */
    void initializeMenu();
    
    /**
     * @brief Gets all available menu items
     * @return Vector of menu items
     */
    const std::vector<std::shared_ptr<MenuItem>>& getMenuItems() const;
    
    /**
     * @brief Gets menu items by category
     * @param category Menu category to filter by
     * @return Vector of menu items in the specified category
     */
    std::vector<std::shared_ptr<MenuItem>> getMenuItemsByCategory(MenuItem::Category category) const;
    
    /**
     * @brief Adds a new menu item
     * @param item Menu item to add
     */
    void addMenuItem(std::shared_ptr<MenuItem> item);
    
    /**
     * @brief Updates an existing menu item
     * @param itemId ID of the item to update
     * @param updatedItem Updated menu item data
     * @return True if item was found and updated, false otherwise
     */
    bool updateMenuItem(int itemId, std::shared_ptr<MenuItem> updatedItem);
    
    // =================================================================
    // Order Management
    // =================================================================
    
    /**
     * @brief Creates a new order for a table
     * @param tableNumber Table number for the order
     * @return Shared pointer to the created order
     */
    std::shared_ptr<Order> createOrder(int tableNumber);
    
    /**
     * @brief Gets the current active order being built
     * @return Pointer to current order, or nullptr if none
     */
    std::shared_ptr<Order> getCurrentOrder() const;
    
    /**
     * @brief Sets the current order being worked on
     * @param order Order to set as current
     */
    void setCurrentOrder(std::shared_ptr<Order> order);
    
    /**
     * @brief Adds an item to the current order
     * @param menuItem Menu item to add
     * @param quantity Quantity to add (default: 1)
     * @param specialInstructions Special preparation instructions
     * @return True if successfully added, false otherwise
     */
    bool addItemToCurrentOrder(std::shared_ptr<MenuItem> menuItem, 
                              int quantity = 1, 
                              const std::string& specialInstructions = "");
    
    /**
     * @brief Removes an item from the current order
     * @param index Index of the item to remove
     * @return True if successfully removed, false otherwise
     */
    bool removeItemFromCurrentOrder(size_t index);
    
    /**
     * @brief Updates the quantity of an item in the current order
     * @param index Index of the item to update
     * @param newQuantity New quantity for the item
     * @return True if successfully updated, false otherwise
     */
    bool updateCurrentOrderItemQuantity(size_t index, int newQuantity);
    
    /**
     * @brief Gets all active orders
     * @return Vector of active orders
     */
    std::vector<std::shared_ptr<Order>> getActiveOrders() const;
    
    /**
     * @brief Gets orders by table number
     * @param tableNumber Table number to filter by
     * @return Vector of orders for the specified table
     */
    std::vector<std::shared_ptr<Order>> getOrdersByTable(int tableNumber) const;
    
    /**
     * @brief Gets order by ID
     * @param orderId Order ID to look up
     * @return Pointer to order, or nullptr if not found
     */
    std::shared_ptr<Order> getOrderById(int orderId) const;
    
    // =================================================================
    // Kitchen Operations
    // =================================================================
    
    /**
     * @brief Sends the current order to the kitchen
     * @return True if successfully sent, false otherwise
     */
    bool sendCurrentOrderToKitchen();
    
    /**
     * @brief Sends a specific order to the kitchen
     * @param order Order to send to kitchen
     * @return True if successfully sent, false otherwise
     */
    bool sendOrderToKitchen(std::shared_ptr<Order> order);
    
    /**
     * @brief Updates the kitchen status of an order
     * @param orderId Order ID to update
     * @param status New kitchen status
     * @return True if successfully updated, false otherwise
     */
    bool updateKitchenStatus(int orderId, KitchenInterface::KitchenStatus status);
    
    /**
     * @brief Gets all active kitchen tickets
     * @return Vector of active kitchen tickets
     */
    std::vector<KitchenInterface::KitchenTicket> getKitchenTickets() const;
    
    /**
     * @brief Gets kitchen queue status information
     * @return JSON object with queue status
     */
    Wt::Json::Object getKitchenQueueStatus() const;
    
    /**
     * @brief Gets estimated wait time for new orders
     * @return Estimated wait time in minutes
     */
    int getEstimatedWaitTime() const;
    
    // =================================================================
    // Payment Operations
    // =================================================================
    
    /**
     * @brief Processes payment for the current order
     * @param method Payment method to use
     * @param amount Payment amount
     * @param tipAmount Tip amount (default: 0.0)
     * @return Payment result with transaction details
     */
    PaymentProcessor::PaymentResult processCurrentOrderPayment(
        PaymentProcessor::PaymentMethod method,
        double amount,
        double tipAmount = 0.0);
    
    /**
     * @brief Processes payment for a specific order
     * @param order Order to process payment for
     * @param method Payment method to use
     * @param amount Payment amount
     * @param tipAmount Tip amount
     * @return Payment result with transaction details
     */
    PaymentProcessor::PaymentResult processOrderPayment(
        std::shared_ptr<Order> order,
        PaymentProcessor::PaymentMethod method,
        double amount,
        double tipAmount = 0.0);
    
    /**
     * @brief Processes a split payment
     * @param order Order to process payment for
     * @param payments Vector of payment method and amount pairs
     * @return Vector of payment results
     */
    std::vector<PaymentProcessor::PaymentResult> processSplitPayment(
        std::shared_ptr<Order> order,
        const std::vector<std::pair<PaymentProcessor::PaymentMethod, double>>& payments);
    
    /**
     * @brief Gets all transaction history
     * @return Vector of all payment results
     */
    const std::vector<PaymentProcessor::PaymentResult>& getTransactionHistory() const;
    
    // =================================================================
    // Business Operations
    // =================================================================
    
    /**
     * @brief Completes an order (payment processed, ready to serve)
     * @param orderId Order ID to complete
     * @return True if successfully completed, false otherwise
     */
    bool completeOrder(int orderId);
    
    /**
     * @brief Cancels an active order
     * @param orderId Order ID to cancel
     * @return True if successfully cancelled, false otherwise
     */
    bool cancelOrder(int orderId);
    
    /**
     * @brief Gets business statistics
     * @return JSON object with business metrics
     */
    Wt::Json::Object getBusinessStatistics() const;
    
    // =================================================================
    // Event Callbacks (for UI notifications)
    // =================================================================
    
    /**
     * @brief Callback type for order events
     */
    using OrderEventCallback = std::function<void(std::shared_ptr<Order>)>;
    
    /**
     * @brief Callback type for kitchen events
     */
    using KitchenEventCallback = std::function<void(int orderId, KitchenInterface::KitchenStatus status)>;
    
    /**
     * @brief Callback type for payment events
     */
    using PaymentEventCallback = std::function<void(const PaymentProcessor::PaymentResult&)>;
    
    /**
     * @brief Registers callback for order creation events
     * @param callback Function to call when orders are created
     */
    void onOrderCreated(OrderEventCallback callback);
    
    /**
     * @brief Registers callback for order modification events
     * @param callback Function to call when orders are modified
     */
    void onOrderModified(OrderEventCallback callback);
    
    /**
     * @brief Registers callback for kitchen status changes
     * @param callback Function to call when kitchen status changes
     */
    void onKitchenStatusChanged(KitchenEventCallback callback);
    
    /**
     * @brief Registers callback for payment completion
     * @param callback Function to call when payments are processed
     */
    void onPaymentProcessed(PaymentEventCallback callback);

    /**
     * @brief Gets the kitchen status of an order
     * @param orderId Order ID to check status for
     * @return Current kitchen status of the order
     */
    KitchenInterface::KitchenStatus getKitchenStatus(int orderId) const;

protected:
    /**
     * @brief Initializes the core components
     */
    void initializeComponents();
    
    /**
     * @brief Sets up event listeners for component coordination
     */
    void setupEventListeners();
    
    /**
     * @brief Validates that a current order exists
     * @return True if current order exists, false otherwise
     */
    bool validateCurrentOrder() const;

private:
    // Core components (three-legged foundation)
    std::unique_ptr<OrderManager> orderManager_;
    std::unique_ptr<PaymentProcessor> paymentProcessor_;
    std::unique_ptr<KitchenInterface> kitchenInterface_;
    
    // Event management
    std::shared_ptr<EventManager> eventManager_;
    
    // Application data
    std::vector<std::shared_ptr<MenuItem>> menuItems_;
    std::shared_ptr<Order> currentOrder_;
    
    // Event callbacks
    std::vector<OrderEventCallback> orderCreatedCallbacks_;
    std::vector<OrderEventCallback> orderModifiedCallbacks_;
    std::vector<KitchenEventCallback> kitchenStatusCallbacks_;
    std::vector<PaymentEventCallback> paymentCallbacks_;
    
    // Internal event handlers
    void handleOrderCreated(std::shared_ptr<Order> order);
    void handleOrderModified(std::shared_ptr<Order> order);
    void handleKitchenStatusChanged(int orderId, KitchenInterface::KitchenStatus status);
    void handlePaymentProcessed(const PaymentProcessor::PaymentResult& result);
    
    // Utility methods
    void notifyOrderCreated(std::shared_ptr<Order> order);
    void notifyOrderModified(std::shared_ptr<Order> order);
    void notifyKitchenStatusChanged(int orderId, KitchenInterface::KitchenStatus status);
    void notifyPaymentProcessed(const PaymentProcessor::PaymentResult& result);
};

#endif // POSSERVICE_H
