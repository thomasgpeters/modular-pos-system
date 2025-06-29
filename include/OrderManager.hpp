#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include "../include/Order.hpp"
#include <memory>
#include <map>
#include <vector>

/**
 * @file OrderManager.h
 * @brief Order management system for the Restaurant POS
 * 
 * This file contains the OrderManager class which handles the lifecycle
 * of customer orders from creation to completion. Part of the three-legged
 * foundation of the POS system (Order Management, Payment Processing, Kitchen Interface).
 * 
 * @author Restaurant POS Team
 * @version 1.0.0
 */

/**
 * @class OrderManager
 * @brief Manages the lifecycle of customer orders (LEG 1 of POS Foundation)
 * 
 * The OrderManager class handles creation, modification, and tracking of customer orders.
 * It maintains active orders in memory and provides methods for order lifecycle management.
 * Designed with extension points for database persistence, order analytics, and workflow automation.
 */
class OrderManager {
public:
    /**
     * @brief Constructs a new OrderManager
     * Initializes with starting order ID of 1000
     */
    OrderManager();
    
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~OrderManager() = default;
    
    /**
     * @brief Creates a new order for a table
     * @param tableNumber Table number for the order
     * @return Shared pointer to the created order
     */
    std::shared_ptr<Order> createOrder(int tableNumber);
    
    /**
     * @brief Retrieves an existing order by ID
     * @param orderId The order ID to look up
     * @return Shared pointer to the order, or nullptr if not found
     */
    std::shared_ptr<Order> getOrder(int orderId);
    
    /**
     * @brief Gets all active orders
     * @return Vector of all active orders
     */
    std::vector<std::shared_ptr<Order>> getActiveOrders();
    
    /**
     * @brief Gets all completed orders
     * @return Vector of all completed orders
     */
    std::vector<std::shared_ptr<Order>> getCompletedOrders();
    
    /**
     * @brief Gets orders by table number
     * @param tableNumber Table number to filter by
     * @return Vector of orders for the specified table
     */
    std::vector<std::shared_ptr<Order>> getOrdersByTable(int tableNumber);
    
    /**
     * @brief Gets orders by status
     * @param status Order status to filter by
     * @return Vector of orders with the specified status
     */
    std::vector<std::shared_ptr<Order>> getOrdersByStatus(Order::Status status);
    
    /**
     * @brief Completes an order and moves it to history
     * @param orderId The order to complete
     * @return True if order was found and completed, false otherwise
     */
    bool completeOrder(int orderId);
    
    /**
     * @brief Cancels an active order
     * @param orderId The order to cancel
     * @return True if order was found and cancelled, false otherwise
     */
    bool cancelOrder(int orderId);
    
    /**
     * @brief Updates the status of an order
     * @param orderId The order to update
     * @param status New status for the order
     * @return True if order was found and updated, false otherwise
     */
    bool updateOrderStatus(int orderId, Order::Status status);
    
    /**
     * @brief Gets the total number of active orders
     * @return Number of active orders
     */
    size_t getActiveOrderCount() const { return activeOrders_.size(); }
    
    /**
     * @brief Gets the total number of completed orders
     * @return Number of completed orders
     */
    size_t getCompletedOrderCount() const { return completedOrders_.size(); }
    
    /**
     * @brief Gets the next order ID that will be assigned
     * @return Next order ID
     */
    int getNextOrderId() const { return nextOrderId_; }
    
    // Extension points for future features
    /**
     * @brief Called when a new order is created
     * Override in derived classes for custom behavior
     * @param order The newly created order
     */
    virtual void onOrderCreated(std::shared_ptr<Order> order) {}
    
    /**
     * @brief Called when an order is modified
     * Override in derived classes for custom behavior
     * @param order The modified order
     */
    virtual void onOrderModified(std::shared_ptr<Order> order) {}
    
    /**
     * @brief Called when an order is completed
     * Override in derived classes for custom behavior
     * @param order The completed order
     */
    virtual void onOrderCompleted(std::shared_ptr<Order> order) {}
    
    /**
     * @brief Called when an order is cancelled
     * Override in derived classes for custom behavior
     * @param order The cancelled order
     */
    virtual void onOrderCancelled(std::shared_ptr<Order> order) {}
    
    /**
     * @brief Called when an order status changes
     * Override in derived classes for custom behavior
     * @param order The order with updated status
     * @param oldStatus Previous status
     * @param newStatus New status
     */
    virtual void onOrderStatusChanged(std::shared_ptr<Order> order, 
                                    Order::Status oldStatus, 
                                    Order::Status newStatus) {}

protected:
    /**
     * @brief Removes an order from active orders
     * Used internally by complete and cancel methods
     * @param orderId Order ID to remove
     * @return Shared pointer to removed order, or nullptr if not found
     */
    std::shared_ptr<Order> removeFromActive(int orderId);

private:
    int nextOrderId_;                                       ///< Next order ID to assign
    std::map<int, std::shared_ptr<Order>> activeOrders_;   ///< Active orders by ID
    std::vector<std::shared_ptr<Order>> completedOrders_;  ///< Completed order history
};

#endif // ORDERMANAGER_H
