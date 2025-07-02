#include "../../include/services/POSService.hpp"

#include <algorithm>
#include <iostream>
#include <regex>

POSService::POSService(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager), currentOrder_(nullptr), 
      orderCreatedCallback_(nullptr), orderModifiedCallback_(nullptr) {
    
    initializeSubsystems();
    initializeMenuItems();
    
    std::cout << "POSService initialized with OrderManager, KitchenInterface, and PaymentProcessor" << std::endl;
}

void POSService::initializeSubsystems() {
    // Initialize the major subsystems
    orderManager_ = std::make_unique<OrderManager>();
    kitchenInterface_ = std::make_unique<KitchenInterface>();
    paymentProcessor_ = std::make_unique<PaymentProcessor>();
    
    std::cout << "POS subsystems initialized" << std::endl;
}

void POSService::initializeMenuItems() {
    menuItems_.clear();
    
    // Create some default menu items using the correct constructor
    // MenuItem(int id, const std::string& name, double price, Category category)
    menuItems_.push_back(std::make_shared<MenuItem>(1, "Caesar Salad", 8.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(2, "Garlic Bread", 5.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(3, "Grilled Chicken", 15.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(4, "Beef Steak", 22.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(5, "Chocolate Cake", 6.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(6, "Ice Cream", 4.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(7, "Coffee", 2.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(8, "Soft Drink", 2.49, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(9, "Today's Special", 18.99, MenuItem::SPECIAL));
    
    std::cout << "Loaded " << menuItems_.size() << " menu items" << std::endl;
}

// =====================================================================
// Order Management Methods (Delegate to OrderManager)
// =====================================================================

std::shared_ptr<Order> POSService::createOrder(const std::string& tableIdentifier) {
    if (!orderManager_) {
        std::cerr << "OrderManager not initialized" << std::endl;
        return nullptr;
    }
    
    auto order = orderManager_->createOrder(tableIdentifier);
    
    if (order && eventManager_) {
        auto eventData = POSEvents::createOrderCreatedEvent(order);
        eventManager_->publish(POSEvents::ORDER_CREATED, eventData);
        
        // Also call the UI callback if registered
        if (orderCreatedCallback_) {
            orderCreatedCallback_(order);
        }
    }
    
    return order;
}

std::shared_ptr<Order> POSService::createOrder(int tableNumber) {
    // Convert table number to string identifier for compatibility
    std::string tableIdentifier = "table " + std::to_string(tableNumber);
    return createOrder(tableIdentifier);
}

std::vector<std::shared_ptr<Order>> POSService::getActiveOrders() const {
    if (!orderManager_) {
        return {};
    }
    
    return orderManager_->getActiveOrders();
}

std::shared_ptr<Order> POSService::getOrderById(int orderId) const {
    if (!orderManager_) {
        return nullptr;
    }
    
    return orderManager_->getOrder(orderId);
}

std::shared_ptr<Order> POSService::getCurrentOrder() const {
    return currentOrder_;
}

void POSService::setCurrentOrder(std::shared_ptr<Order> order) {
    currentOrder_ = order;
    
    if (eventManager_) {
        // Create a simple JSON object for current order changed event
        Wt::Json::Object eventData;
        if (order) {
            eventData["orderId"] = Wt::Json::Value(order->getOrderId());
            eventData["tableNumber"] = Wt::Json::Value(order->getTableNumber());
        } else {
            eventData["orderId"] = Wt::Json::Value(-1);
            eventData["tableNumber"] = Wt::Json::Value(-1);
        }
        eventManager_->publish("CURRENT_ORDER_CHANGED", eventData);
    }
}

bool POSService::cancelOrder(int orderId) {
    if (!orderManager_) {
        return false;
    }
    
    bool success = orderManager_->cancelOrder(orderId);
    
    if (success) {
        // Clear current order if it's the one being cancelled
        if (currentOrder_ && currentOrder_->getOrderId() == orderId) {
            currentOrder_ = nullptr;
        }
        
        if (eventManager_) {
            auto order = orderManager_->getOrder(orderId);
            if (order) {
                auto eventData = POSEvents::createOrderModifiedEvent(order);
                eventManager_->publish(POSEvents::ORDER_CANCELLED, eventData);
            }
        }
    }
    
    return success;
}

bool POSService::isTableIdentifierInUse(const std::string& tableIdentifier) const {
    if (!orderManager_) {
        return false;
    }
    
    return orderManager_->isTableIdentifierInUse(tableIdentifier);
}

bool POSService::isValidTableIdentifier(const std::string& tableIdentifier) const {
    return Order::isValidTableIdentifier(tableIdentifier);
}

// =====================================================================
// Current Order Management
// =====================================================================

bool POSService::addItemToCurrentOrder(std::shared_ptr<MenuItem> item) {
    if (!item || !item->isAvailable()) {
        std::cerr << "Invalid or unavailable menu item" << std::endl;
        return false;
    }
    
    if (!currentOrder_) {
        std::cerr << "No current order to add item to" << std::endl;
        return false;
    }
    
    try {
        // Create OrderItem with the MenuItem and add to order
        OrderItem orderItem(*item, 1); // quantity = 1
        currentOrder_->addItem(orderItem);
        
        if (eventManager_) {
            auto eventData = POSEvents::createOrderModifiedEvent(currentOrder_);
            eventManager_->publish(POSEvents::ORDER_ITEM_ADDED, eventData);
            
            // Also call the UI callback if registered
            if (orderModifiedCallback_) {
                orderModifiedCallback_(currentOrder_);
            }
        }
        
        std::cout << "Added " << item->getName() << " to current order" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error adding item to order: " << e.what() << std::endl;
        return false;
    }
}

bool POSService::removeItemFromCurrentOrder(size_t itemIndex) {
    if (!currentOrder_) {
        return false;
    }
    
    const auto& items = currentOrder_->getItems();
    if (itemIndex >= items.size()) {
        return false;
    }
    
    try {
        currentOrder_->removeItem(itemIndex);
        
        if (eventManager_) {
            auto eventData = POSEvents::createOrderModifiedEvent(currentOrder_);
            eventManager_->publish(POSEvents::ORDER_ITEM_REMOVED, eventData);
            
            // Also call the UI callback if registered
            if (orderModifiedCallback_) {
                orderModifiedCallback_(currentOrder_);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error removing item from order: " << e.what() << std::endl;
        return false;
    }
}

bool POSService::updateCurrentOrderItemQuantity(size_t itemIndex, int newQuantity) {
    if (!currentOrder_ || newQuantity <= 0) {
        return false;
    }
    
    const auto& items = currentOrder_->getItems();
    if (itemIndex >= items.size()) {
        return false;
    }
    
    try {
        currentOrder_->updateItemQuantity(itemIndex, newQuantity);
        
        if (eventManager_) {
            auto eventData = POSEvents::createOrderModifiedEvent(currentOrder_);
            eventManager_->publish(POSEvents::ORDER_MODIFIED, eventData);
            
            // Also call the UI callback if registered
            if (orderModifiedCallback_) {
                orderModifiedCallback_(currentOrder_);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error updating item quantity: " << e.what() << std::endl;
        return false;
    }
}

bool POSService::sendCurrentOrderToKitchen() {
    if (!currentOrder_ || !kitchenInterface_) {
        std::cerr << "No current order or kitchen interface not available" << std::endl;
        return false;
    }
    
    if (currentOrder_->getItems().empty()) {
        std::cerr << "Cannot send empty order to kitchen" << std::endl;
        return false;
    }
    
    try {
        // Send order to kitchen using the KitchenInterface
        bool success = kitchenInterface_->sendOrderToKitchen(currentOrder_);
        
        if (success) {
            // Update order status
            currentOrder_->setStatus(Order::SENT_TO_KITCHEN);
            
            if (eventManager_) {
                auto eventData = POSEvents::createOrderModifiedEvent(currentOrder_);
                eventManager_->publish(POSEvents::ORDER_SENT_TO_KITCHEN, eventData);
            }
            
            std::cout << "Order #" << currentOrder_->getOrderId() << " sent to kitchen" << std::endl;
            
            // Clear current order after sending to kitchen
            currentOrder_ = nullptr;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "Error sending order to kitchen: " << e.what() << std::endl;
        return false;
    }
}

// =====================================================================
// Kitchen Interface Methods
// =====================================================================

std::vector<KitchenInterface::KitchenTicket> POSService::getKitchenTickets() const {
    if (!kitchenInterface_) {
        return {};
    }
    
    return kitchenInterface_->getActiveTickets();
}

int POSService::getEstimatedWaitTime() const {
    if (!kitchenInterface_) {
        return 0;
    }
    
    return kitchenInterface_->getEstimatedWaitTime();
}

Wt::Json::Object POSService::getKitchenQueueStatus() const {
    if (!kitchenInterface_) {
        Wt::Json::Object emptyStatus;
        emptyStatus["queueSize"] = 0;
        emptyStatus["estimatedWaitTime"] = 0;
        return emptyStatus;
    }
    
    return kitchenInterface_->getKitchenQueueStatus();
}

// =====================================================================
// Menu Management Methods
// =====================================================================

std::vector<std::shared_ptr<MenuItem>> POSService::getMenuItems() const {
    return menuItems_;
}

std::vector<std::shared_ptr<MenuItem>> POSService::getMenuItemsByCategory(MenuItem::Category category) const {
    std::vector<std::shared_ptr<MenuItem>> categoryItems;
    
    for (const auto& item : menuItems_) {
        if (item && item->getCategory() == category) {
            categoryItems.push_back(item);
        }
    }
    
    return categoryItems;
}

// =====================================================================
// Payment Processing Methods
// =====================================================================

PaymentProcessor::PaymentResult POSService::processPayment(
    std::shared_ptr<Order> order,
    PaymentProcessor::PaymentMethod method,
    double amount,
    double tipAmount) {
    
    if (!paymentProcessor_) {
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "Payment processor not available";
        return result;
    }
    
    auto result = paymentProcessor_->processPayment(order, method, amount, tipAmount);
    
    if (result.success && eventManager_) {
        auto eventData = POSEvents::createPaymentCompletedEvent(result, order);
        eventManager_->publish(POSEvents::PAYMENT_COMPLETED, eventData);
    }
    
    return result;
}

std::vector<PaymentProcessor::PaymentResult> POSService::getTransactionHistory() const {
    if (!paymentProcessor_) {
        return {};
    }
    
    return paymentProcessor_->getTransactionHistory();
}

// =====================================================================
// Configuration Methods
// =====================================================================

std::vector<std::string> POSService::getEnabledPaymentMethods() const {
    // Return default enabled payment methods
    return {"cash", "credit_card", "debit_card", "mobile_pay"};
}

std::vector<double> POSService::getTipSuggestions() const {
    // Return default tip suggestions (as decimal percentages)
    return {0.15, 0.18, 0.20, 0.25};
}

// =====================================================================
// Initialization and Callback Registration Methods
// =====================================================================

void POSService::initializeMenu() {
    // This method is called by RestaurantPOSApp to initialize the menu
    initializeMenuItems();
    std::cout << "Menu initialized via initializeMenu() call" << std::endl;
}

void POSService::onOrderCreated(std::function<void(std::shared_ptr<Order>)> callback) {
    orderCreatedCallback_ = callback;
    std::cout << "Order created callback registered" << std::endl;
}

void POSService::onOrderModified(std::function<void(std::shared_ptr<Order>)> callback) {
    orderModifiedCallback_ = callback;
    std::cout << "Order modified callback registered" << std::endl;
}

// =====================================================================
// Helper Methods
// =====================================================================

std::vector<std::string> POSService::convertOrderItemsToStringList(const std::vector<OrderItem>& items) const {
    std::vector<std::string> itemList;
    
    for (const auto& item : items) {
        std::string itemString = std::to_string(item.getQuantity()) + "x " + 
                                item.getMenuItem().getName();
        
        if (!item.getSpecialInstructions().empty()) {
            itemString += " (" + item.getSpecialInstructions() + ")";
        }
        
        itemList.push_back(itemString);
    }
    
    return itemList;
}

