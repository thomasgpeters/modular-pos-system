#include "../../include/services/POSService.hpp"

#include <iostream>
#include <algorithm>
#include <sstream>

POSService::POSService(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager)
    , currentOrder_(nullptr) {
    
    initializeComponents();
    setupEventListeners();
    initializeMenu();
}

void POSService::initializeComponents() {
    std::cout << "Initializing POSService components..." << std::endl;
    
    // Initialize core business components
    orderManager_ = std::make_unique<OrderManager>();
    paymentProcessor_ = std::make_unique<PaymentProcessor>();
    kitchenInterface_ = std::make_unique<KitchenInterface>();
    
    std::cout << "✓ Core business components initialized" << std::endl;
}

void POSService::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "Warning: EventManager not available for POSService" << std::endl;
        return;
    }
    
    std::cout << "✓ POSService event listeners setup (business logic coordinator ready)" << std::endl;
}

void POSService::initializeMenu() {
    std::cout << "Initializing menu with sample items..." << std::endl;
    
    // Clear existing menu
    menuItems_.clear();
    
    // Appetizers
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Caesar Salad", "Fresh romaine lettuce with parmesan and croutons", 8.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Buffalo Wings", "Spicy wings with blue cheese dip", 12.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Spinach Dip", "Creamy spinach and artichoke dip with tortilla chips", 9.99, MenuItem::APPETIZER));
    
    // Main courses
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Grilled Salmon", "Atlantic salmon with lemon herb butter", 18.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Ribeye Steak", "12oz prime ribeye with garlic mashed potatoes", 24.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Chicken Parmesan", "Breaded chicken breast with marinara and mozzarella", 16.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Vegetable Stir-fry", "Fresh seasonal vegetables with teriyaki sauce", 14.99, MenuItem::MAIN_COURSE));
    
    // Desserts
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Chocolate Cake", "Rich chocolate layer cake with vanilla ice cream", 7.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Cheesecake", "New York style cheesecake with berry compote", 6.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Apple Pie", "Traditional apple pie with cinnamon ice cream", 6.99, MenuItem::DESSERT));
    
    // Beverages
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Coffee", "Fresh brewed coffee", 2.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Iced Tea", "Freshly brewed iced tea", 2.49, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Soda", "Coca-Cola, Pepsi, Sprite", 2.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(
        "Wine", "House red or white wine", 8.99, MenuItem::BEVERAGE));
    
    std::cout << "✓ Menu initialized with " << menuItems_.size() << " items" << std::endl;
}

// =================================================================
// Menu Management
// =================================================================

const std::vector<std::shared_ptr<MenuItem>>& POSService::getMenuItems() const {
    return menuItems_;
}

std::vector<std::shared_ptr<MenuItem>> POSService::getMenuItemsByCategory(MenuItem::Category category) const {
    std::vector<std::shared_ptr<MenuItem>> categoryItems;
    
    for (const auto& item : menuItems_) {
        if (item->getCategory() == category) {
            categoryItems.push_back(item);
        }
    }
    
    return categoryItems;
}

void POSService::addMenuItem(std::shared_ptr<MenuItem> item) {
    if (!item) {
        std::cerr << "Error: Cannot add null menu item" << std::endl;
        return;
    }
    
    menuItems_.push_back(item);
    
    // Publish menu updated event
    if (eventManager_) {
        eventManager_->publish(POSEvents::CONFIGURATION_CHANGED);
    }
    
    std::cout << "Menu item added: " << item->getName() << std::endl;
}

bool POSService::updateMenuItem(int itemId, std::shared_ptr<MenuItem> updatedItem) {
    auto it = std::find_if(menuItems_.begin(), menuItems_.end(),
        [itemId](const std::shared_ptr<MenuItem>& item) {
            return item->getId() == itemId;
        });
    
    if (it != menuItems_.end()) {
        *it = updatedItem;
        
        // Publish menu updated event
        if (eventManager_) {
            eventManager_->publish(POSEvents::CONFIGURATION_CHANGED);
        }
        
        std::cout << "Menu item updated: " << updatedItem->getName() << std::endl;
        return true;
    }
    
    std::cerr << "Error: Menu item with ID " << itemId << " not found" << std::endl;
    return false;
}

// =================================================================
// Order Management
// =================================================================

std::shared_ptr<Order> POSService::createOrder(int tableNumber) {
    if (!orderManager_) {
        std::cerr << "Error: OrderManager not available" << std::endl;
        return nullptr;
    }
    
    auto order = orderManager_->createOrder(tableNumber);
    if (order) {
        setCurrentOrder(order);
        handleOrderCreated(order);
    }
    
    return order;
}

std::shared_ptr<Order> POSService::getCurrentOrder() const {
    return currentOrder_;
}

void POSService::setCurrentOrder(std::shared_ptr<Order> order) {
    currentOrder_ = order;
    
    // Publish current order changed event
    if (eventManager_) {
        if (order) {
            eventManager_->publish(POSEvents::ORDER_MODIFIED, 
                POSEvents::createOrderCreatedEvent(order));
        } else {
            eventManager_->publish(POSEvents::ORDER_MODIFIED);
        }
    }
}

bool POSService::addItemToCurrentOrder(std::shared_ptr<MenuItem> menuItem, 
                                      int quantity, 
                                      const std::string& specialInstructions) {
    if (!validateCurrentOrder()) {
        std::cerr << "Error: No current order to add item to" << std::endl;
        return false;
    }
    
    if (!menuItem) {
        std::cerr << "Error: Cannot add null menu item to order" << std::endl;
        return false;
    }
    
    if (quantity <= 0) {
        std::cerr << "Error: Invalid quantity for menu item" << std::endl;
        return false;
    }
    
    // Add item to current order
    currentOrder_->addItem(menuItem, quantity, specialInstructions);
    
    // Handle order modification
    handleOrderModified(currentOrder_);
    
    std::cout << "Added " << quantity << "x " << menuItem->getName() 
              << " to order #" << currentOrder_->getId() << std::endl;
    
    return true;
}

bool POSService::removeItemFromCurrentOrder(size_t index) {
    if (!validateCurrentOrder()) {
        std::cerr << "Error: No current order to remove item from" << std::endl;
        return false;
    }
    
    const auto& items = currentOrder_->getItems();
    if (index >= items.size()) {
        std::cerr << "Error: Invalid item index for removal" << std::endl;
        return false;
    }
    
    std::string itemName = items[index].menuItem->getName();
    currentOrder_->removeItem(index);
    
    // Handle order modification
    handleOrderModified(currentOrder_);
    
    std::cout << "Removed " << itemName << " from order #" << currentOrder_->getId() << std::endl;
    
    return true;
}

bool POSService::updateCurrentOrderItemQuantity(size_t index, int newQuantity) {
    if (!validateCurrentOrder()) {
        std::cerr << "Error: No current order to update" << std::endl;
        return false;
    }
    
    const auto& items = currentOrder_->getItems();
    if (index >= items.size()) {
        std::cerr << "Error: Invalid item index for quantity update" << std::endl;
        return false;
    }
    
    if (newQuantity <= 0) {
        // Remove item if quantity is 0 or negative
        return removeItemFromCurrentOrder(index);
    }
    
    currentOrder_->updateItemQuantity(index, newQuantity);
    
    // Handle order modification
    handleOrderModified(currentOrder_);
    
    std::cout << "Updated quantity for " << items[index].menuItem->getName() 
              << " to " << newQuantity << std::endl;
    
    return true;
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
        std::cerr << "Error: No current order to send to kitchen" << std::endl;
        return false;
    }
    
    return sendOrderToKitchen(currentOrder_);
}

bool POSService::sendOrderToKitchen(std::shared_ptr<Order> order) {
    if (!order) {
        std::cerr << "Error: Cannot send null order to kitchen" << std::endl;
        return false;
    }
    
    if (!kitchenInterface_) {
        std::cerr << "Error: KitchenInterface not available" << std::endl;
        return false;
    }
    
    if (order->getItems().empty()) {
        std::cerr << "Error: Cannot send empty order to kitchen" << std::endl;
        return false;
    }
    
    bool success = kitchenInterface_->sendOrderToKitchen(order);
    
    if (success) {
        // Update order status
        order->setStatus(Order::Status::SENT_TO_KITCHEN);
        
        // Publish kitchen event
        if (eventManager_) {
            eventManager_->publish(POSEvents::ORDER_SENT_TO_KITCHEN,
                POSEvents::createOrderCreatedEvent(order));
        }
        
        // Clear current order if it was sent successfully
        if (order == currentOrder_) {
            setCurrentOrder(nullptr);
        }
        
        std::cout << "Order #" << order->getId() << " sent to kitchen successfully" << std::endl;
    } else {
        std::cerr << "Failed to send order #" << order->getId() << " to kitchen" << std::endl;
    }
    
    return success;
}

bool POSService::updateKitchenStatus(int orderId, KitchenInterface::KitchenStatus status) {
    if (!kitchenInterface_) {
        std::cerr << "Error: KitchenInterface not available" << std::endl;
        return false;
    }
    
    bool success = kitchenInterface_->updateOrderStatus(orderId, status);
    
    if (success) {
        handleKitchenStatusChanged(orderId, status);
        std::cout << "Kitchen status updated for order #" << orderId << std::endl;
    } else {
        std::cerr << "Failed to update kitchen status for order #" << orderId << std::endl;
    }
    
    return success;
}

std::vector<KitchenInterface::KitchenTicket> POSService::getKitchenTickets() const {
    if (!kitchenInterface_) {
        return {};
    }
    
    return kitchenInterface_->getActiveTickets();
}

Wt::Json::Object POSService::getKitchenQueueStatus() const {
    if (!kitchenInterface_) {
        return Wt::Json::Object();
    }
    
    return kitchenInterface_->getKitchenStatus();
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
    
    if (!order) {
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "Invalid order for payment processing";
        return result;
    }
    
    if (!paymentProcessor_) {
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "Payment processor not available";
        return result;
    }
    
    // Process payment
    auto result = paymentProcessor_->processPayment(method, amount, tipAmount);
    
    if (result.success) {
        // Update order status
        order->setStatus(Order::Status::PAID);
        
        // Complete the order
        completeOrder(order->getId());
        
        // Handle payment processed
        handlePaymentProcessed(result);
        
        std::cout << "Payment processed successfully for order #" << order->getId() 
                  << " - Amount: $" << result.amountCharged << std::endl;
    } else {
        std::cerr << "Payment failed for order #" << order->getId() 
                  << " - Error: " << result.errorMessage << std::endl;
    }
    
    return result;
}

std::vector<PaymentProcessor::PaymentResult> POSService::processSplitPayment(
    std::shared_ptr<Order> order,
    const std::vector<std::pair<PaymentProcessor::PaymentMethod, double>>& payments) {
    
    std::vector<PaymentProcessor::PaymentResult> results;
    
    if (!order || !paymentProcessor_) {
        PaymentProcessor::PaymentResult failResult;
        failResult.success = false;
        failResult.errorMessage = "Invalid order or payment processor not available";
        results.push_back(failResult);
        return results;
    }
    
    double totalAmountProcessed = 0.0;
    bool allSuccessful = true;
    
    // Process each payment
    for (const auto& payment : payments) {
        auto result = paymentProcessor_->processPayment(payment.first, payment.second);
        results.push_back(result);
        
        if (result.success) {
            totalAmountProcessed += result.amountCharged;
        } else {
            allSuccessful = false;
        }
    }
    
    if (allSuccessful && totalAmountProcessed >= order->getTotal()) {
        // All payments successful and total covered
        order->setStatus(Order::Status::PAID);
        completeOrder(order->getId());
        
        std::cout << "Split payment completed successfully for order #" << order->getId() 
                  << " - Total: $" << totalAmountProcessed << std::endl;
    } else {
        std::cerr << "Split payment incomplete for order #" << order->getId() 
                  << " - Processed: $" << totalAmountProcessed 
                  << " / Required: $" << order->getTotal() << std::endl;
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
    auto order = getOrderById(orderId);
    if (!order) {
        std::cerr << "Error: Order #" << orderId << " not found for completion" << std::endl;
        return false;
    }
    
    // Update order status
    order->setStatus(Order::Status::COMPLETED);
    
    // Publish order completed event
    if (eventManager_) {
        eventManager_->publish(POSEvents::ORDER_COMPLETED,
            POSEvents::createOrderCreatedEvent(order));
    }
    
    std::cout << "Order #" << orderId << " marked as completed" << std::endl;
    return true;
}

bool POSService::cancelOrder(int orderId) {
    auto order = getOrderById(orderId);
    if (!order) {
        std::cerr << "Error: Order #" << orderId << " not found for cancellation" << std::endl;
        return false;
    }
    
    // Update order status
    order->setStatus(Order::Status::CANCELLED);
    
    // Remove from kitchen if it was sent
    if (kitchenInterface_) {
        kitchenInterface_->cancelOrder(orderId);
    }
    
    // Clear current order if it's the one being cancelled
    if (order == currentOrder_) {
        setCurrentOrder(nullptr);
    }
    
    // Publish order cancelled event
    if (eventManager_) {
        eventManager_->publish(POSEvents::ORDER_CANCELLED,
            POSEvents::createOrderCreatedEvent(order));
    }
    
    std::cout << "Order #" << orderId << " cancelled" << std::endl;
    return true;
}

Wt::Json::Object POSService::getBusinessStatistics() const {
    Wt::Json::Object stats;
    
    // Order statistics
    auto activeOrders = getActiveOrders();
    stats["activeOrders"] = static_cast<int>(activeOrders.size());
    
    // Kitchen statistics
    auto kitchenTickets = getKitchenTickets();
    stats["kitchenTickets"] = static_cast<int>(kitchenTickets.size());
    stats["estimatedWaitTime"] = getEstimatedWaitTime();
    
    // Menu statistics
    stats["menuItems"] = static_cast<int>(menuItems_.size());
    
    // Payment statistics
    const auto& transactions = getTransactionHistory();
    stats["totalTransactions"] = static_cast<int>(transactions.size());
    
    double totalRevenue = 0.0;
    for (const auto& transaction : transactions) {
        if (transaction.success) {
            totalRevenue += transaction.amountCharged;
        }
    }
    stats["totalRevenue"] = totalRevenue;
    
    return stats;
}

// =================================================================
// Event Callbacks and Handlers
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

void POSService::handleOrderCreated(std::shared_ptr<Order> order) {
    // Notify callbacks
    notifyOrderCreated(order);
    
    // Publish event
    if (eventManager_) {
        eventManager_->publish(POSEvents::ORDER_CREATED,
            POSEvents::createOrderCreatedEvent(order));
    }
}

void POSService::handleOrderModified(std::shared_ptr<Order> order) {
    // Notify callbacks
    notifyOrderModified(order);
    
    // Publish event
    if (eventManager_) {
        eventManager_->publish(POSEvents::ORDER_MODIFIED,
            POSEvents::createOrderCreatedEvent(order));
    }
}

void POSService::handleKitchenStatusChanged(int orderId, KitchenInterface::KitchenStatus status) {
    // Notify callbacks
    notifyKitchenStatusChanged(orderId, status);
    
    // Publish event
    if (eventManager_) {
        eventManager_->publish(POSEvents::KITCHEN_STATUS_CHANGED,
            POSEvents::createKitchenStatusChangedEvent(orderId, status, status));
    }
}

void POSService::handlePaymentProcessed(const PaymentProcessor::PaymentResult& result) {
    // Notify callbacks
    notifyPaymentProcessed(result);
    
    // Publish event
    if (eventManager_) {
        // Create payment event data (you'll need to adapt this based on your POSEvents structure)
        eventManager_->publish(POSEvents::PAYMENT_COMPLETED);
    }
}

// Notification methods
void POSService::notifyOrderCreated(std::shared_ptr<Order> order) {
    for (const auto& callback : orderCreatedCallbacks_) {
        try {
            callback(order);
        } catch (const std::exception& e) {
            std::cerr << "Error in order created callback: " << e.what() << std::endl;
        }
    }
}

void POSService::notifyOrderModified(std::shared_ptr<Order> order) {
    for (const auto& callback : orderModifiedCallbacks_) {
        try {
            callback(order);
        } catch (const std::exception& e) {
            std::cerr << "Error in order modified callback: " << e.what() << std::endl;
        }
    }
}

void POSService::notifyKitchenStatusChanged(int orderId, KitchenInterface::KitchenStatus status) {
    for (const auto& callback : kitchenStatusCallbacks_) {
        try {
            callback(orderId, status);
        } catch (const std::exception& e) {
            std::cerr << "Error in kitchen status callback: " << e.what() << std::endl;
        }
    }
}

void POSService::notifyPaymentProcessed(const PaymentProcessor::PaymentResult& result) {
    for (const auto& callback : paymentCallbacks_) {
        try {
            callback(result);
        } catch (const std::exception& e) {
            std::cerr << "Error in payment processed callback: " << e.what() << std::endl;
        }
    }
}

// =================================================================
// Utility Methods
// =================================================================

bool POSService::validateCurrentOrder() const {
    return currentOrder_ != nullptr;
}
