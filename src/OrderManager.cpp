#include "../include/OrderManager.hpp"

#include <algorithm>
#include <iostream>

OrderManager::OrderManager() : nextOrderId_(1000) {
    std::cout << "[OrderManager] Initialized with starting order ID: " << nextOrderId_ << std::endl;
}

std::shared_ptr<Order> OrderManager::createOrder(const std::string& tableIdentifier) {
    if (!Order::isValidTableIdentifier(tableIdentifier)) {
        std::cerr << "[OrderManager] Invalid table identifier: " << tableIdentifier << std::endl;
        return nullptr;
    }
    
    // Check if table identifier is already in use
    if (isTableIdentifierInUse(tableIdentifier)) {
        std::cerr << "[OrderManager] Table identifier already in use: " << tableIdentifier << std::endl;
        return nullptr;
    }
    
    try {
        // Create new order with string table identifier
        auto order = std::make_shared<Order>(nextOrderId_, tableIdentifier);
        
        // Add to active orders
        activeOrders_[nextOrderId_] = order;
        
        // Increment order ID for next order
        nextOrderId_++;
        
        // Call extension point
        onOrderCreated(order);
        
        std::cout << "[OrderManager] Created order #" << order->getOrderId() 
                  << " for " << tableIdentifier << std::endl;
        
        return order;
    } catch (const std::exception& e) {
        std::cerr << "[OrderManager] Failed to create order for " << tableIdentifier 
                  << ": " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<Order> OrderManager::createOrder(int tableNumber) {
    // Legacy compatibility - convert to string identifier
    std::string tableIdentifier = generateTableIdentifier(tableNumber);
    return createOrder(tableIdentifier);
}

std::shared_ptr<Order> OrderManager::getOrder(int orderId) {
    auto it = activeOrders_.find(orderId);
    if (it != activeOrders_.end()) {
        return it->second;
    }
    
    // Check completed orders as well
    auto completedIt = std::find_if(completedOrders_.begin(), completedOrders_.end(),
        [orderId](const std::shared_ptr<Order>& order) {
            return order->getOrderId() == orderId;
        });
    
    if (completedIt != completedOrders_.end()) {
        return *completedIt;
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<Order>> OrderManager::getActiveOrders() {
    std::vector<std::shared_ptr<Order>> orders;
    orders.reserve(activeOrders_.size());
    
    for (const auto& pair : activeOrders_) {
        orders.push_back(pair.second);
    }
    
    return orders;
}

std::vector<std::shared_ptr<Order>> OrderManager::getCompletedOrders() {
    return completedOrders_;
}

std::vector<std::shared_ptr<Order>> OrderManager::getOrdersByTableIdentifier(const std::string& tableIdentifier) {
    std::vector<std::shared_ptr<Order>> orders;
    
    for (const auto& pair : activeOrders_) {
        if (pair.second->getTableIdentifier() == tableIdentifier) {
            orders.push_back(pair.second);
        }
    }
    
    return orders;
}

std::vector<std::shared_ptr<Order>> OrderManager::getOrdersByTable(int tableNumber) {
    // Legacy compatibility
    std::string tableIdentifier = generateTableIdentifier(tableNumber);
    return getOrdersByTableIdentifier(tableIdentifier);
}

std::vector<std::shared_ptr<Order>> OrderManager::getOrdersByStatus(Order::Status status) {
    std::vector<std::shared_ptr<Order>> orders;
    
    for (const auto& pair : activeOrders_) {
        if (pair.second->getStatus() == status) {
            orders.push_back(pair.second);
        }
    }
    
    return orders;
}

std::vector<std::shared_ptr<Order>> OrderManager::getOrdersByType(const std::string& orderType) {
    std::vector<std::shared_ptr<Order>> orders;
    
    for (const auto& pair : activeOrders_) {
        if (matchesOrderType(pair.second, orderType)) {
            orders.push_back(pair.second);
        }
    }
    
    return orders;
}

std::vector<std::shared_ptr<Order>> OrderManager::getDineInOrders() {
    return getOrdersByType("Dine-In");
}

std::vector<std::shared_ptr<Order>> OrderManager::getDeliveryOrders() {
    return getOrdersByType("Delivery");
}

std::vector<std::shared_ptr<Order>> OrderManager::getWalkInOrders() {
    return getOrdersByType("Walk-In");
}

bool OrderManager::completeOrder(int orderId) {
    auto order = removeFromActive(orderId);
    if (order) {
        order->setStatus(Order::SERVED);
        completedOrders_.push_back(order);
        
        // Call extension point
        onOrderCompleted(order);
        
        std::cout << "[OrderManager] Completed order #" << orderId << std::endl;
        return true;
    }
    
    std::cerr << "[OrderManager] Failed to complete order #" << orderId 
              << " - order not found" << std::endl;
    return false;
}

bool OrderManager::cancelOrder(int orderId) {
    auto order = removeFromActive(orderId);
    if (order) {
        Order::Status oldStatus = order->getStatus();
        order->setStatus(Order::CANCELLED);
        completedOrders_.push_back(order);
        
        // Call extension points
        onOrderStatusChanged(order, oldStatus, Order::CANCELLED);
        onOrderCancelled(order);
        
        std::cout << "[OrderManager] Cancelled order #" << orderId << std::endl;
        return true;
    }
    
    std::cerr << "[OrderManager] Failed to cancel order #" << orderId 
              << " - order not found" << std::endl;
    return false;
}

bool OrderManager::updateOrderStatus(int orderId, Order::Status status) {
    auto it = activeOrders_.find(orderId);
    if (it != activeOrders_.end()) {
        Order::Status oldStatus = it->second->getStatus();
        it->second->setStatus(status);
        
        // Call extension point
        onOrderStatusChanged(it->second, oldStatus, status);
        
        std::cout << "[OrderManager] Updated order #" << orderId 
                  << " status to " << Order::statusToString(status) << std::endl;
        return true;
    }
    
    std::cerr << "[OrderManager] Failed to update order #" << orderId 
              << " - order not found" << std::endl;
    return false;
}

size_t OrderManager::getActiveOrderCountByType(const std::string& orderType) const {
    size_t count = 0;
    for (const auto& pair : activeOrders_) {
        if (matchesOrderType(pair.second, orderType)) {
            count++;
        }
    }
    return count;
}

std::vector<std::string> OrderManager::getActiveTableIdentifiers() const {
    std::vector<std::string> identifiers;
    identifiers.reserve(activeOrders_.size());
    
    for (const auto& pair : activeOrders_) {
        identifiers.push_back(pair.second->getTableIdentifier());
    }
    
    return identifiers;
}

bool OrderManager::isTableIdentifierInUse(const std::string& tableIdentifier) const {
    for (const auto& pair : activeOrders_) {
        if (pair.second->getTableIdentifier() == tableIdentifier) {
            return true;
        }
    }
    return false;
}

std::shared_ptr<Order> OrderManager::removeFromActive(int orderId) {
    auto it = activeOrders_.find(orderId);
    if (it != activeOrders_.end()) {
        auto order = it->second;
        activeOrders_.erase(it);
        return order;
    }
    return nullptr;
}

std::string OrderManager::generateTableIdentifier(int tableNumber) const {
    if (tableNumber <= 0) {
        return "walk-in"; // Default for invalid table numbers
    }
    return "table " + std::to_string(tableNumber);
}

bool OrderManager::matchesOrderType(const std::shared_ptr<Order>& order, const std::string& orderType) const {
    if (!order) return false;
    
    if (orderType == "Dine-In") {
        return order->isDineIn();
    } else if (orderType == "Delivery") {
        return order->isDelivery();
    } else if (orderType == "Walk-In") {
        return order->isWalkIn();
    }
    
    return false;
}
