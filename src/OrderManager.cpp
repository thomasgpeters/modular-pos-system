#include "../include/OrderManager.hpp"

OrderManager::OrderManager() : nextOrderId_(1000) {}

std::shared_ptr<Order> OrderManager::createOrder(int tableNumber) {
    auto order = std::make_shared<Order>(nextOrderId_++, tableNumber);
    activeOrders_[order->getOrderId()] = order;
    
    // Call extension point
    onOrderCreated(order);
    
    return order;
}

std::shared_ptr<Order> OrderManager::getOrder(int orderId) {
    auto it = activeOrders_.find(orderId);
    return (it != activeOrders_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Order>> OrderManager::getActiveOrders() {
    std::vector<std::shared_ptr<Order>> orders;
    orders.reserve(activeOrders_.size());
    
    for (const auto& [id, order] : activeOrders_) {
        orders.push_back(order);
    }
    
    return orders;
}

std::vector<std::shared_ptr<Order>> OrderManager::getCompletedOrders() {
    return completedOrders_;
}

std::vector<std::shared_ptr<Order>> OrderManager::getOrdersByTable(int tableNumber) {
    std::vector<std::shared_ptr<Order>> orders;
    
    for (const auto& [id, order] : activeOrders_) {
        if (order->getTableNumber() == tableNumber) {
            orders.push_back(order);
        }
    }
    
    return orders;
}

std::vector<std::shared_ptr<Order>> OrderManager::getOrdersByStatus(Order::Status status) {
    std::vector<std::shared_ptr<Order>> orders;
    
    for (const auto& [id, order] : activeOrders_) {
        if (order->getStatus() == status) {
            orders.push_back(order);
        }
    }
    
    return orders;
}

bool OrderManager::completeOrder(int orderId) {
    auto order = removeFromActive(orderId);
    if (order) {
        order->setStatus(Order::SERVED);
        completedOrders_.push_back(order);
        
        // Call extension point
        onOrderCompleted(order);
        
        return true;
    }
    return false;
}

bool OrderManager::cancelOrder(int orderId) {
    auto order = removeFromActive(orderId);
    if (order) {
        order->setStatus(Order::CANCELLED);
        completedOrders_.push_back(order);
        
        // Call extension point
        onOrderCancelled(order);
        
        return true;
    }
    return false;
}

bool OrderManager::updateOrderStatus(int orderId, Order::Status status) {
    auto order = getOrder(orderId);
    if (order) {
        Order::Status oldStatus = order->getStatus();
        order->setStatus(status);
        
        // Call extension point
        onOrderStatusChanged(order, oldStatus, status);
        
        return true;
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
