#include "../../include/services/POSService.hpp"

POSService::POSService(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager) {
    initializeComponents();
    setupEventListeners();
}

void POSService::initializeComponents() {
    // Initialize core components
    orderManager_ = std::make_unique<OrderManager>();
    paymentProcessor_ = std::make_unique<PaymentProcessor>();
    kitchenInterface_ = std::make_unique<KitchenInterface>();
}

void POSService::setupEventListeners() {
    // Set up internal event handling between components
    // This would typically involve connecting the components together
}

void POSService::initializeMenu() {
    menuItems_.clear();
    
    // Initialize with sample menu items
    menuItems_.push_back(std::make_shared<MenuItem>(1, "Caesar Salad", 12.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(2, "Buffalo Wings", 14.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(3, "Grilled Salmon", 24.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(4, "Ribeye Steak", 32.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(5, "Margherita Pizza", 18.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(6, "Chocolate Cake", 8.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(7, "Tiramisu", 9.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(8, "Coca Cola", 3.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(9, "Coffee", 4.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(10, "Chef's Special", 28.99, MenuItem::SPECIAL));
}

const std::vector<std::shared_ptr<MenuItem>>& POSService::getMenuItems() const {
    return menuItems_;
}

std::vector<std::shared_ptr<MenuItem>> POSService::getMenuItemsByCategory(MenuItem::Category category) const {
    std::vector<std::shared_ptr<MenuItem>> filteredItems;
    
    for (const auto& item : menuItems_) {
        if (item->getCategory() == category && item->isAvailable()) {
            filteredItems.push_back(item);
        }
    }
    
    return filteredItems;
}

void POSService::addMenuItem(std::shared_ptr<MenuItem> item) {
    if (item) {
        menuItems_.push_back(item);
    }
}

bool POSService::updateMenuItem(int itemId, std::shared_ptr<MenuItem> updatedItem) {
    auto it = std::find_if(menuItems_.begin(), menuItems_.end(),
        [itemId](const std::shared_ptr<MenuItem>& item) { return item->getId() == itemId; });
    
    if (it != menuItems_.end() && updatedItem) {
        *it = updatedItem;
        return true;
    }
    return false;
}

std::shared_ptr<Order> POSService::createOrder(int tableNumber) {
    auto order = orderManager_->createOrder(tableNumber);
    if (order) {
        handleOrderCreated(order);
    }
    return order;
}

std::shared_ptr<Order> POSService::getCurrentOrder() const {
    return currentOrder_;
}

void POSService::setCurrentOrder(std::shared_ptr<Order> order) {
    currentOrder_ = order;
}

bool POSService::addItemToCurrentOrder(std::shared_ptr<MenuItem> menuItem, 
                                      int quantity, 
                                      const std::string& specialInstructions) {
    if (!validateCurrentOrder() || !menuItem) return false;
    
    OrderItem orderItem(*menuItem, quantity);
    if (!specialInstructions.empty()) {
        orderItem.setSpecialInstructions(specialInstructions);
    }
    
    currentOrder_->addItem(orderItem);
    handleOrderModified(currentOrder_);
    
    return true;
}

bool POSService::removeItemFromCurrentOrder(size_t index) {
    if (!validateCurrentOrder()) return false;
    
    if (index < currentOrder_->getItems().size()) {
        currentOrder_->removeItem(index);
        handleOrderModified(currentOrder_);
        return true;
    }
    return false;
}

bool POSService::updateCurrentOrderItemQuantity(size_t index, int newQuantity) {
    if (!validateCurrentOrder()) return false;
    
    if (index < currentOrder_->getItems().size() && newQuantity > 0) {
        currentOrder_->updateItemQuantity(index, newQuantity);
        handleOrderModified(currentOrder_);
        return true;
    }
    return false;
}

std::vector<std::shared_ptr<Order>> POSService::getActiveOrders() const {
    return orderManager_->getActiveOrders();
}

std::vector<std::shared_ptr<Order>> POSService::getOrdersByTable(int tableNumber) const {
    return orderManager_->getOrdersByTable(tableNumber);
}

std::shared_ptr<Order> POSService::getOrderById(int orderId) const {
    return orderManager_->getOrder(orderId);
}

bool POSService::sendCurrentOrderToKitchen() {
    if (!validateCurrentOrder()) return false;
    
    return sendOrderToKitchen(currentOrder_);
}

bool POSService::sendOrderToKitchen(std::shared_ptr<Order> order) {
    if (!order || order->getItems().empty()) return false;
    
    bool success = kitchenInterface_->sendOrderToKitchen(order);
    if (success) {
        orderManager_->updateOrderStatus(order->getOrderId(), Order::SENT_TO_KITCHEN);
        // Publish event
        eventManager_->publish(POSEvents::ORDER_SENT_TO_KITCHEN, 
            POSEvents::createOrderCreatedEvent(order));
    }
    
    return success;
}

bool POSService::updateKitchenStatus(int orderId, KitchenInterface::KitchenStatus status) {
    bool success = kitchenInterface_->updateKitchenStatus(orderId, status);
    if (success) {
        handleKitchenStatusChanged(orderId, status);
    }
    return success;
}

std::vector<KitchenInterface::KitchenTicket> POSService::getKitchenTickets() const {
    return kitchenInterface_->getActiveTickets();
}

Wt::Json::Object POSService::getKitchenQueueStatus() const {
    return kitchenInterface_->getKitchenQueueStatus();
}

int POSService::getEstimatedWaitTime() const {
    return kitchenInterface_->getEstimatedWaitTime();
}

PaymentProcessor::PaymentResult POSService::processCurrentOrderPayment(
    PaymentProcessor::PaymentMethod method,
    double amount,
    double tipAmount) {
    
    if (!validateCurrentOrder()) {
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "No current order";
        return result;
    }
    
    return processOrderPayment(currentOrder_, method, amount, tipAmount);
}

PaymentProcessor::PaymentResult POSService::processOrderPayment(
    std::shared_ptr<Order> order,
    PaymentProcessor::PaymentMethod method,
    double amount,
    double tipAmount) {
    
    auto result = paymentProcessor_->processPayment(order, method, amount, tipAmount);
    handlePaymentProcessed(result);
    
    return result;
}

std::vector<PaymentProcessor::PaymentResult> POSService::processSplitPayment(
    std::shared_ptr<Order> order,
    const std::vector<std::pair<PaymentProcessor::PaymentMethod, double>>& payments) {
    
    auto results = paymentProcessor_->processSplitPayment(order, payments);
    
    for (const auto& result : results) {
        handlePaymentProcessed(result);
    }
    
    return results;
}

const std::vector<PaymentProcessor::PaymentResult>& POSService::getTransactionHistory() const {
    return paymentProcessor_->getTransactionHistory();
}

bool POSService::completeOrder(int orderId) {
    bool success = orderManager_->completeOrder(orderId);
    if (success) {
        kitchenInterface_->removeTicket(orderId);
        // Publish event
        auto order = orderManager_->getOrder(orderId);
        if (order) {
            eventManager_->publish(POSEvents::ORDER_COMPLETED, 
                POSEvents::createOrderCreatedEvent(order));
        }
    }
    return success;
}

bool POSService::cancelOrder(int orderId) {
    bool success = orderManager_->cancelOrder(orderId);
    if (success) {
        kitchenInterface_->removeTicket(orderId);
        // Publish event
        auto order = orderManager_->getOrder(orderId);
        if (order) {
            eventManager_->publish(POSEvents::ORDER_CANCELLED, 
                POSEvents::createOrderCreatedEvent(order));
        }
    }
    return success;
}

Wt::Json::Object POSService::getBusinessStatistics() const {
    Wt::Json::Object stats;
    
    auto activeOrders = getActiveOrders();
    auto transactions = getTransactionHistory();
    
    stats["activeOrderCount"] = Wt::Json::Value(static_cast<int>(activeOrders.size()));
    stats["totalTransactions"] = Wt::Json::Value(static_cast<int>(transactions.size()));
    stats["kitchenQueueSize"] = Wt::Json::Value(static_cast<int>(getKitchenTickets().size()));
    stats["estimatedWaitTime"] = Wt::Json::Value(getEstimatedWaitTime());
    
    // Calculate total revenue
    double totalRevenue = 0.0;
    for (const auto& transaction : transactions) {
        if (transaction.success) {
            totalRevenue += transaction.amountProcessed;
        }
    }
    stats["totalRevenue"] = Wt::Json::Value(totalRevenue);
    
    return stats;
}

KitchenInterface::KitchenStatus POSService::getKitchenStatus(int orderId) const {
    const auto* ticket = kitchenInterface_->getTicketByOrderId(orderId);
    return ticket ? ticket->status : KitchenInterface::ORDER_RECEIVED;
}

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

bool POSService::validateCurrentOrder() const {
    return currentOrder_ != nullptr;
}

void POSService::handleOrderCreated(std::shared_ptr<Order> order) {
    notifyOrderCreated(order);
    eventManager_->publish(POSEvents::ORDER_CREATED, 
        POSEvents::createOrderCreatedEvent(order));
}

void POSService::handleOrderModified(std::shared_ptr<Order> order) {
    notifyOrderModified(order);
    eventManager_->publish(POSEvents::ORDER_MODIFIED, 
        POSEvents::createOrderModifiedEvent(order));
}

void POSService::handleKitchenStatusChanged(int orderId, KitchenInterface::KitchenStatus status) {
    notifyKitchenStatusChanged(orderId, status);
    eventManager_->publish(POSEvents::KITCHEN_STATUS_CHANGED, 
        POSEvents::createKitchenStatusChangedEvent(orderId, status, KitchenInterface::ORDER_RECEIVED));
}

void POSService::handlePaymentProcessed(const PaymentProcessor::PaymentResult& result) {
    notifyPaymentProcessed(result);
    
    if (result.success) {
        eventManager_->publish(POSEvents::PAYMENT_COMPLETED, result);
    } else {
        eventManager_->publish(POSEvents::PAYMENT_FAILED, result);
    }
}

void POSService::notifyOrderCreated(std::shared_ptr<Order> order) {
    for (auto& callback : orderCreatedCallbacks_) {
        callback(order);
    }
}

void POSService::notifyOrderModified(std::shared_ptr<Order> order) {
    for (auto& callback : orderModifiedCallbacks_) {
        callback(order);
    }
}

void POSService::notifyKitchenStatusChanged(int orderId, KitchenInterface::KitchenStatus status) {
    for (auto& callback : kitchenStatusCallbacks_) {
        callback(orderId, status);
    }
}

void POSService::notifyPaymentProcessed(const PaymentProcessor::PaymentResult& result) {
    for (auto& callback : paymentCallbacks_) {
        callback(result);
    }
}
