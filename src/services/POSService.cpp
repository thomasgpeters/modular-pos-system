//=============================================================================
// IMPLEMENTATION FILE: POSService.cpp
//=============================================================================

#include "POSService.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>

POSService::POSService(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager) {
    
    initializeComponents();
    setupEventListeners();
}

void POSService::initializeComponents() {
    std::cout << "Initializing POSService components..." << std::endl;
    
    // Initialize the three-legged foundation
    orderManager_ = std::make_unique<OrderManager>();
    paymentProcessor_ = std::make_unique<PaymentProcessor>();
    kitchenInterface_ = std::make_unique<KitchenInterface>();
    
    // Initialize menu with sample items
    initializeMenu();
    
    std::cout << "✓ POSService components initialized" << std::endl;
}

void POSService::setupEventListeners() {
    if (!eventManager_) return;
    
    std::cout << "Setting up POSService event listeners..." << std::endl;
    
    // Set up internal event handlers for coordination
    // These would be implemented if the foundation classes support event callbacks
    
    std::cout << "✓ POSService event listeners setup complete" << std::endl;
}

// =================================================================
// Menu Management
// =================================================================

void POSService::initializeMenu() {
    std::cout << "Initializing menu items..." << std::endl;
    
    menuItems_.clear();
    
    // Appetizers
    menuItems_.push_back(std::make_shared<MenuItem>(1, "Caesar Salad", "Fresh romaine lettuce with Caesar dressing", 8.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(2, "Buffalo Wings", "Spicy chicken wings with blue cheese", 12.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(3, "Mozzarella Sticks", "Crispy breaded mozzarella with marinara", 9.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(4, "Loaded Nachos", "Tortilla chips with cheese, jalapeños, sour cream", 11.99, MenuItem::APPETIZER));
    
    // Main Courses
    menuItems_.push_back(std::make_shared<MenuItem>(5, "Grilled Salmon", "Atlantic salmon with lemon herb butter", 22.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(6, "Ribeye Steak", "12oz ribeye steak cooked to perfection", 28.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(7, "Chicken Parmesan", "Breaded chicken breast with marinara and mozzarella", 18.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(8, "Vegetarian Pasta", "Penne pasta with seasonal vegetables", 16.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(9, "Fish and Chips", "Beer-battered cod with hand-cut fries", 17.99, MenuItem::MAIN_COURSE));
    
    // Desserts
    menuItems_.push_back(std::make_shared<MenuItem>(10, "Chocolate Cake", "Rich chocolate layer cake with ganache", 7.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(11, "Tiramisu", "Classic Italian coffee-flavored dessert", 8.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(12, "Ice Cream Sundae", "Vanilla ice cream with chocolate sauce", 5.99, MenuItem::DESSERT));
    
    // Beverages
    menuItems_.push_back(std::make_shared<MenuItem>(13, "Fresh Lemonade", "Freshly squeezed lemonade", 3.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(14, "Craft Beer", "Selection of local craft beers", 5.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(15, "House Wine", "Red or white wine by the glass", 7.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(16, "Coffee", "Freshly brewed coffee", 2.99, MenuItem::BEVERAGE));
    
    // Specials
    menuItems_.push_back(std::make_shared<MenuItem>(17, "Chef's Special", "Daily special - ask your server", 24.99, MenuItem::SPECIAL));
    menuItems_.push_back(std::make_shared<MenuItem>(18, "Soup of the Day", "Fresh daily soup selection", 6.99, MenuItem::SPECIAL));
    
    std::cout << "✓ Menu initialized with " << menuItems_.size() << " items" << std::endl;
}

const std::vector<std::shared_ptr<MenuItem>>& POSService::getMenuItems() const {
    return menuItems_;
}

std::vector<std::shared_ptr<MenuItem>> POSService::getMenuItemsByCategory(MenuItem::Category category) const {
    std::vector<std::shared_ptr<MenuItem>> categoryItems;
    
    std::copy_if(menuItems_.begin(), menuItems_.end(), std::back_inserter(categoryItems),
        [category](const std::shared_ptr<MenuItem>& item) {
            return item->getCategory() == category;
        });
    
    return categoryItems;
}

void POSService::addMenuItem(std::shared_ptr<MenuItem> item) {
    if (!item) {
        std::cerr << "POSService: Cannot add null menu item" << std::endl;
        return;
    }
    
    menuItems_.push_back(item);
    
    std::cout << "Menu item added: " << item->getName() << " - $" << item->getPrice() << std::endl;
}

bool POSService::updateMenuItem(int itemId, std::shared_ptr<MenuItem> updatedItem) {
    if (!updatedItem) {
        std::cerr << "POSService: Cannot update with null menu item" << std::endl;
        return false;
    }
    
    auto it = std::find_if(menuItems_.begin(), menuItems_.end(),
        [itemId](const std::shared_ptr<MenuItem>& item) {
            return item->getId() == itemId;
        });
    
    if (it != menuItems_.end()) {
        *it = updatedItem;
        std::cout << "Menu item updated: " << updatedItem->getName() << std::endl;
        return true;
    }
    
    std::cerr << "POSService: Menu item with ID " << itemId << " not found" << std::endl;
    return false;
}

// =================================================================
// Order Management
// =================================================================

std::shared_ptr<Order> POSService::createOrder(int tableNumber) {
    if (!orderManager_) {
        std::cerr << "POSService: OrderManager not initialized" << std::endl;
        return nullptr;
    }
    
    auto order = orderManager_->createOrder(tableNumber);
    if (order) {
        std::cout << "Order created for table " << tableNumber << " (ID: " << order->getId() << ")" << std::endl;
        
        // Publish order created event
        if (eventManager_) {
            eventManager_->publish(POSEvents::ORDER_CREATED, 
                POSEvents::createOrderCreatedEvent(order));
        }
        
        // Notify callbacks
        notifyOrderCreated(order);
    }
    
    return order;
}

std::shared_ptr<Order> POSService::getCurrentOrder() const {
    return currentOrder_;
}

void POSService::setCurrentOrder(std::shared_ptr<Order> order) {
    currentOrder_ = order;
    
    if (currentOrder_) {
        std::cout << "Current order set to ID: " << currentOrder_->getId() 
                  << " (Table: " << currentOrder_->getTableNumber() << ")" << std::endl;
    } else {
        std::cout << "Current order cleared" << std::endl;
    }
}

bool POSService::addItemToCurrentOrder(std::shared_ptr<MenuItem> menuItem, 
                                      int quantity, 
                                      const std::string& specialInstructions) {
    if (!validateCurrentOrder()) {
        std::cerr << "POSService: No current order to add item to" << std::endl;
        return false;
    }
    
    if (!menuItem || quantity <= 0) {
        std::cerr << "POSService: Invalid menu item or quantity" << std::endl;
        return false;
    }
    
    try {
        // Create order item
        OrderItem orderItem(menuItem, quantity, specialInstructions);
        
        // Add to current order
        currentOrder_->addItem(orderItem);
        
        std::cout << "Added " << quantity << "x " << menuItem->getName() 
                  << " to order " << currentOrder_->getId() << std::endl;
        
        // Publish order modified event
        if (eventManager_) {
            eventManager_->publish(POSEvents::ORDER_MODIFIED, 
                POSEvents::createOrderModifiedEvent(currentOrder_));
        }
        
        // Notify callbacks
        notifyOrderModified(currentOrder_);
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "POSService: Error adding item to order: " << e.what() << std::endl;
        return false;
    }
}

bool POSService::removeItemFromCurrentOrder(size_t index) {
    if (!validateCurrentOrder()) {
        std::cerr << "POSService: No current order to remove item from" << std::endl;
        return false;
    }
    
    try {
        auto& items = currentOrder_->getItems();
        if (index >= items.size()) {
            std::cerr << "POSService: Invalid item index: " << index << std::endl;
            return false;
        }
        
        std::string itemName = items[index].getMenuItem()->getName();
        currentOrder_->removeItem(index);
        
        std::cout << "Removed " << itemName << " from order " << currentOrder_->getId() << std::endl;
        
        // Publish order modified event
        if (eventManager_) {
            eventManager_->publish(POSEvents::ORDER_MODIFIED,
                POSEvents::createOrderModifiedEvent(currentOrder_));
        }
        
        // Notify callbacks
        notifyOrderModified(currentOrder_);
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "POSService: Error removing item from order: " << e.what() << std::endl;
        return false;
    }
}

bool POSService::updateCurrentOrderItemQuantity(size_t index, int newQuantity) {
    if (!validateCurrentOrder()) {
        std::cerr << "POSService: No current order to update" << std::endl;
        return false;
    }
    
    if (newQuantity <= 0) {
        std::cerr << "POSService: Invalid quantity: " << newQuantity << std::endl;
        return false;
    }
    
    try {
        auto& items = currentOrder_->getItems();
        if (index >= items.size()) {
            std::cerr << "POSService: Invalid item index: " << index << std::endl;
            return false;
        }
        
        items[index].setQuantity(newQuantity);
        currentOrder_->calculateTotals(); // Recalculate totals
        
        std::cout << "Updated quantity for " << items[index].getMenuItem()->getName() 
                  << " to " << newQuantity << std::endl;
        
        // Publish order modified event
        if (eventManager_) {
            eventManager_->publish(POSEvents::ORDER_MODIFIED,
                POSEvents::createOrderModifiedEvent(currentOrder_));
        }
        
        // Notify callbacks
        notifyOrderModified(currentOrder_);
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "POSService: Error updating item quantity: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<Order>> POSService::getActiveOrders() const {
    if (!orderManager_) {
        return {};
    }
    
    return orderManager_->getActiveOrders();
}

std::vector<std::shared_ptr<Order>> POSService::getOrdersByTable(int tableNumber) const {
    if (!orderManager_) {
        return {};
    }
    
    return orderManager_->getOrdersByTable(tableNumber);
}

std::shared_ptr<Order> POSService::getOrderById(int orderId) const {
    if (!orderManager_) {
        return nullptr;
    }
    
    return orderManager_->getOrderById(orderId);
}

// =================================================================
// Kitchen Operations
// =================================================================

bool POSService::sendCurrentOrderToKitchen() {
    if (!validateCurrentOrder()) {
        std::cerr << "POSService: No current order to send to kitchen" << std::endl;
        return false;
    }
    
    return sendOrderToKitchen(currentOrder_);
}

bool POSService::sendOrderToKitchen(std::shared_ptr<Order> order) {
    if (!order || !kitchenInterface_) {
        std::cerr << "POSService: Invalid order or kitchen interface not available" << std::endl;
        return false;
    }
    
    if (order->getItems().empty()) {
        std::cerr << "POSService: Cannot send empty order to kitchen" << std::endl;
        return false;
    }
    
    try {
        bool success = kitchenInterface_->sendOrderToKitchen(order);
        
        if (success) {
            order->setStatus(Order::SENT_TO_KITCHEN);
            
            std::cout << "Order " << order->getId() << " sent to kitchen successfully" << std::endl;
            
            // Publish kitchen event
            if (eventManager_) {
                eventManager_->publish(POSEvents::ORDER_SENT_TO_KITCHEN,
                    POSEvents::createKitchenStatusChangedEvent(order->getId(), 
                        KitchenInterface::ORDER_RECEIVED, KitchenInterface::ORDER_RECEIVED));
            }
            
            // Clear current order after sending to kitchen
            if (order == currentOrder_) {
                currentOrder_.reset();
            }
        } else {
            std::cerr << "Failed to send order " << order->getId() << " to kitchen" << std::endl;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "POSService: Error sending order to kitchen: " << e.what() << std::endl;
        return false;
    }
}

bool POSService::updateKitchenStatus(int orderId, KitchenInterface::KitchenStatus status) {
    if (!kitchenInterface_) {
        std::cerr << "POSService: Kitchen interface not available" << std::endl;
        return false;
    }
    
    try {
        auto previousStatus = kitchenInterface_->getOrderStatus(orderId);
        bool success = kitchenInterface_->updateOrderStatus(orderId, status);
        
        if (success) {
            std::cout << "Kitchen status updated for order " << orderId << std::endl;
            
            // Publish kitchen status event
            if (eventManager_) {
                eventManager_->publish(POSEvents::KITCHEN_STATUS_CHANGED,
                    POSEvents::createKitchenStatusChangedEvent(orderId, status, previousStatus));
            }
            
            // Notify callbacks
            notifyKitchenStatusChanged(orderId, status);
        }
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "POSService: Error updating kitchen status: " << e.what() << std::endl;
        return false;
    }
}

std::vector<KitchenInterface::KitchenTicket> POSService::getKitchenTickets() const {
    if (!kitchenInterface_) {
        return {};
    }
    
    return kitchenInterface_->getActiveTickets();
}

Wt::Json::Object POSService::getKitchenQueueStatus() const {
    if (!kitchenInterface_) {
        Wt::Json::Object empty;
        return empty;
    }
    
    return kitchenInterface_->getQueueStatus();
}

int POSService::getEstimatedWaitTime() const {
    if (!kitchenInterface_) {
        return 0;
    }
    
    return kitchenInterface_->getEstimatedWaitTime();
}

// =================================================================
// Payment Operations
// =================================================================

PaymentProcessor::PaymentResult POSService::processCurrentOrderPayment(
    PaymentProcessor::PaymentMethod method,
    double amount,
    double tipAmount) {
    
    if (!validateCurrentOrder()) {
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "No current order to process payment for";
        return result;
    }
    
    return processOrderPayment(currentOrder_, method, amount, tipAmount);
}

PaymentProcessor::PaymentResult POSService::processOrderPayment(
    std::shared_ptr<Order> order,
    PaymentProcessor::PaymentMethod method,
    double amount,
    double tipAmount) {
    
    PaymentProcessor::PaymentResult result;
    
    if (!order || !paymentProcessor_) {
        result.success = false;
        result.errorMessage = "Invalid order or payment processor not available";
        return result;
    }
    
    try {
        result = paymentProcessor_->processPayment(method, amount, tipAmount);
        
        if (result.success) {
            order->setStatus(Order::COMPLETED);
            
            std::cout << "Payment processed successfully for order " << order->getId() 
                      << " - Amount: $" << amount << std::endl;
            
            // Publish payment event
            if (eventManager_) {
                eventManager_->publish(POSEvents::PAYMENT_COMPLETED,
                    POSEvents::createPaymentCompletedEvent(result, order));
            }
            
            // Notify callbacks
            notifyPaymentProcessed(result);
            
            // Complete the order
            completeOrder(order->getId());
            
        } else {
            std::cerr << "Payment failed for order " << order->getId() 
                      << ": " << result.errorMessage << std::endl;
            
            // Publish payment failed event
            if (eventManager_) {
                eventManager_->publish(POSEvents::PAYMENT_FAILED,
                    POSEvents::createPaymentCompletedEvent(result, order));
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = "Payment processing error: " + std::string(e.what());
        std::cerr << "POSService: " << result.errorMessage << std::endl;
        return result;
    }
}

std::vector<PaymentProcessor::PaymentResult> POSService::processSplitPayment(
    std::shared_ptr<Order> order,
    const std::vector<std::pair<PaymentProcessor::PaymentMethod, double>>& payments) {
    
    std::vector<PaymentProcessor::PaymentResult> results;
    
    if (!order || !paymentProcessor_) {
        PaymentProcessor::PaymentResult errorResult;
        errorResult.success = false;
        errorResult.errorMessage = "Invalid order or payment processor not available";
        results.push_back(errorResult);
        return results;
    }
    
    double totalProcessed = 0.0;
    bool allSuccessful = true;
    
    for (const auto& payment : payments) {
        auto result = paymentProcessor_->processPayment(payment.first, payment.second, 0.0);
        results.push_back(result);
        
        if (result.success) {
            totalProcessed += payment.second;
        } else {
            allSuccessful = false;
        }
    }
    
    if (allSuccessful && totalProcessed >= order->getTotalWithTax()) {
        order->setStatus(Order::COMPLETED);
        completeOrder(order->getId());
        
        std::cout << "Split payment completed successfully for order " << order->getId() << std::endl;
    } else {
        std::cerr << "Split payment incomplete or failed for order " << order->getId() << std::endl;
    }
    
    return results;
}

const std::vector<PaymentProcessor::PaymentResult>& POSService::getTransactionHistory() const {
    if (!paymentProcessor_) {
        static std::vector<PaymentProcessor::PaymentResult> empty;
        return empty;
    }
    
    return paymentProcessor_->getTransactionHistory();
}

// =================================================================
// Business Operations
// =================================================================

bool POSService::completeOrder(int orderId) {
    if (!orderManager_) {
        std::cerr << "POSService: OrderManager not available" << std::endl;
        return false;
    }
    
    try {
        auto order = orderManager_->getOrderById(orderId);
        if (!order) {
            std::cerr << "POSService: Order " << orderId << " not found" << std::endl;
            return false;
        }
        
        bool success = orderManager_->completeOrder(orderId);
        
        if (success) {
            std::cout << "Order " << orderId << " completed successfully" << std::endl;
            
            // Publish order completed event
            if (eventManager_) {
                eventManager_->publish(POSEvents::ORDER_COMPLETED,
                    POSEvents::createOrderCreatedEvent(order));
            }
        }
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "POSService: Error completing order: " << e.what() << std::endl;
        return false;
    }
}

bool POSService::cancelOrder(int orderId) {
    if (!orderManager_) {
        std::cerr << "POSService: OrderManager not available" << std::endl;
        return false;
    }
    
    try {
        auto order = orderManager_->getOrderById(orderId);
        if (!order) {
            std::cerr << "POSService: Order " << orderId << " not found" << std::endl;
            return false;
        }
        
        bool success = orderManager_->cancelOrder(orderId);
        
        if (success) {
            std::cout << "Order " << orderId << " cancelled successfully" << std::endl;
            
            // Publish order cancelled event
            if (eventManager_) {
                eventManager_->publish(POSEvents::ORDER_CANCELLED,
                    POSEvents::createOrderCreatedEvent(order));
            }
        }
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "POSService: Error cancelling order: " << e.what() << std::endl;
        return false;
    }
}

Wt::Json::Object POSService::getBusinessStatistics() const {
    Wt::Json::Object stats;
    
    try {
        // Order statistics
        auto activeOrders = getActiveOrders();
        stats["activeOrderCount"] = static_cast<int>(activeOrders.size());
        
        // Kitchen statistics
        auto kitchenStatus = getKitchenQueueStatus();
        stats["kitchenQueue"] = kitchenStatus;
        stats["estimatedWaitTime"] = getEstimatedWaitTime();
        
        // Payment statistics
        auto transactions = getTransactionHistory();
        double totalRevenue = 0.0;
        int successfulTransactions = 0;
        
        for (const auto& transaction : transactions) {
            if (transaction.success) {
                totalRevenue += transaction.amount;
                successfulTransactions++;
            }
        }
        
        stats["totalRevenue"] = totalRevenue;
        stats["transactionCount"] = successfulTransactions;
        stats["menuItemCount"] = static_cast<int>(menuItems_.size());
        
    } catch (const std::exception& e) {
        std::cerr << "POSService: Error generating business statistics: " << e.what() << std::endl;
    }
    
    return stats;
}

// =================================================================
// Event Callbacks and Notifications
// =================================================================

void POSService::onOrderCreated(OrderEventCallback callback) {
    orderCreatedCallbacks_.push_back(callback);
}

void POSService::onOrderModified(OrderEventCallback callback) {
    orderModifiedCallbacks_.push_back(callback);
}

void POSService::onKitchenStatusChanged(KitchenEventCallback callback) {
    kitchenStatusCallbacks_.push_back(callback);
}

void POSService::onPaymentProcessed(PaymentEventCallback callback) {
    paymentCallbacks_.push_back(callback);
}

// Validation methods
bool POSService::validateCurrentOrder() const {
    return currentOrder_ != nullptr;
}

// Notification methods
void POSService::notifyOrderCreated(std::shared_ptr<Order> order) {
    for (const auto& callback : orderCreatedCallbacks_) {
        try {
            callback(order);
        } catch (const std::exception& e) {
            std::cerr << "POSService: Error in order created callback: " << e.what() << std::endl;
        }
    }
}

void POSService::notifyOrderModified(std::shared_ptr<Order> order) {
    for (const auto& callback : orderModifiedCallbacks_) {
        try {
            callback(order);
        } catch (const std::exception& e) {
            std::cerr << "POSService: Error in order modified callback: " << e.what() << std::endl;
        }
    }
}

void POSService::notifyKitchenStatusChanged(int orderId, KitchenInterface::KitchenStatus status) {
    for (const auto& callback : kitchenStatusCallbacks_) {
        try {
            callback(orderId, status);
        } catch (const std::exception& e) {
            std::cerr << "POSService: Error in kitchen status callback: " << e.what() << std::endl;
        }
    }
}

void POSService::notifyPaymentProcessed(const PaymentProcessor::PaymentResult& result) {
    for (const auto& callback : paymentCallbacks_) {
        try {
            callback(result);
        } catch (const std::exception& e) {
            std::cerr << "POSService: Error in payment processed callback: " << e.what() << std::endl;
        }
    }
}
