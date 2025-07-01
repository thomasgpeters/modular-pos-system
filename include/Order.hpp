#ifndef ORDER_H
#define ORDER_H

#include "MenuItem.hpp"

#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Value.h>

/**
 * @file Order.h
 * @brief Order management classes for the Restaurant POS System
 * 
 * This file contains the OrderItem and Order classes which handle individual
 * order items and complete customer orders. Designed for extensibility with
 * features like order modifications, timing tracking, and customer information.
 * 
 * @author Restaurant POS Team
 * @version 1.0.0
 */

/**
 * @class OrderItem
 * @brief Represents an individual item in an order with customizations
 * 
 * The OrderItem class encapsulates a menu item within an order context,
 * including quantity, special instructions, and calculated pricing.
 * Supports order modifications and customizations.
 */
class OrderItem {
public:
    /**
     * @brief Constructs a new OrderItem
     * @param menuItem The menu item being ordered
     * @param quantity Number of this item ordered (default: 1)
     */
    OrderItem(const MenuItem& menuItem, int quantity = 1);
    
    // Getters
    /**
     * @brief Gets the associated menu item
     * @return Reference to the menu item
     */
    const MenuItem& getMenuItem() const { return menuItem_; }
    
    /**
     * @brief Gets the quantity ordered
     * @return The quantity of this item
     */
    int getQuantity() const { return quantity_; }
    
    /**
     * @brief Gets the total price for this order item
     * @return Total price (quantity * unit price)
     */
    double getTotalPrice() const { return totalPrice_; }
    
    /**
     * @brief Gets any special instructions for this item
     * @return Special instructions string
     */
    const std::string& getSpecialInstructions() const { return specialInstructions_; }
    
    // Setters
    /**
     * @brief Updates the quantity for this order item
     * @param quantity New quantity (recalculates total price)
     */
    void setQuantity(int quantity);
    
    /**
     * @brief Sets special instructions for this order item
     * @param instructions Special preparation instructions
     */
    void setSpecialInstructions(const std::string& instructions) { 
        specialInstructions_ = instructions; 
    }
    
    /**
     * @brief Converts the order item to JSON format
     * @return JSON object representation of the order item
     */
    Wt::Json::Object toJson() const;

private:
    MenuItem menuItem_;             ///< The menu item being ordered
    int quantity_;                  ///< Quantity ordered
    double totalPrice_;             ///< Calculated total price
    std::string specialInstructions_; ///< Special preparation instructions
};

/**
 * @class Order
 * @brief Represents a complete customer order
 * 
 * The Order class manages a complete customer order including multiple items,
 * status tracking, pricing calculations, and timing information. Supports
 * order modifications and lifecycle management.
 */
class Order {
public:
    /**
     * @enum Status
     * @brief Order status throughout its lifecycle
     */
    enum Status { 
        PENDING,            ///< Order created but not sent to kitchen
        SENT_TO_KITCHEN,    ///< Order transmitted to kitchen
        PREPARING,          ///< Kitchen is preparing the order
        READY,              ///< Order ready for pickup/serving
        SERVED,             ///< Order delivered to customer
        CANCELLED           ///< Order cancelled
    };
    
    /**
     * @brief Constructs a new Order
     * @param orderId Unique identifier for the order
     * @param tableNumber Table number for the order
     */
    Order(int orderId, int tableNumber);
    
    // Order management
    /**
     * @brief Adds an item to the order
     * @param item OrderItem to add to the order
     */
    void addItem(const OrderItem& item);
    
    /**
     * @brief Removes an item from the order by index
     * @param index Index of the item to remove
     */
    void removeItem(size_t index);
    
    /**
     * @brief Updates the quantity of an existing order item
     * @param index Index of the item to update
     * @param quantity New quantity for the item
     */
    void updateItemQuantity(size_t index, int quantity);
    
    // Getters
    /**
     * @brief Gets the unique order ID
     * @return The order ID
     */
    int getOrderId() const { return orderId_; }
    
    /**
     * @brief Gets the table number for this order
     * @return The table number
     */
    int getTableNumber() const { return tableNumber_; }
    
    /**
     * @brief Gets the current order status
     * @return The order status
     */
    Status getStatus() const { return status_; }
    
    /**
     * @brief Gets all items in the order
     * @return Vector of order items
     */
    const std::vector<OrderItem>& getItems() const { return items_; }
    
    /**
     * @brief Gets the subtotal (before tax)
     * @return The subtotal amount
     */
    double getSubtotal() const { return subtotal_; }
    
    /**
     * @brief Gets the tax amount
     * @return The tax amount
     */
    double getTax() const { return tax_; }
    
    /**
     * @brief Gets the total amount (subtotal + tax)
     * @return The total amount
     */
    double getTotal() const { return total_; }
    
    /**
     * @brief Gets the order creation timestamp
     * @return The timestamp when the order was created
     */
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    // Status management
    /**
     * @brief Updates the order status
     * @param status New status for the order
     */
    void setStatus(Status status) { status_ = status; }
    
    /**
     * @brief Converts the order to JSON format
     * @return JSON object representation of the order
     */
    Wt::Json::Object toJson() const;
    
    /**
     * @brief Gets the string representation of an order status
     * @param status The status to convert
     * @return String representation of the status
     */
    static std::string statusToString(Status status);

private:
    /**
     * @brief Recalculates order totals when items change
     */
    void calculateTotals();
    
    int orderId_;                       ///< Unique order identifier
    int tableNumber_;                   ///< Table number
    Status status_;                     ///< Current order status
    std::vector<OrderItem> items_;      ///< Items in the order
    double subtotal_;                   ///< Subtotal amount
    double tax_;                        ///< Tax amount
    double total_;                      ///< Total amount
    std::chrono::system_clock::time_point timestamp_; ///< Creation timestamp
    
    static constexpr double TAX_RATE = 0.08; ///< Tax rate (8% - configurable)
};

#endif // ORDER_H
