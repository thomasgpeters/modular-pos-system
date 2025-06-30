#include "../include/OrderManager.hpp"
#include <algorithm>

/**
 * @file OrderManager.cpp
 * @brief Implementation of the OrderManager class
 */

OrderManager::OrderManager() : nextOrderId_(1000) {
}

std::shared_ptr<Order> OrderManager::createOrder(int tableNumber) {
    auto order = std::make_shared<Order>(nextOrderId_++, tableNumber);
    activeOrders_[order->getOrderId()] = order;
    
    // Notify observers
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
    
    for (const auto& pair : activeOrders_) {
        orders.push_back(pair.second);
    }
    
    return orders;
}

std::vector<std::shared_ptr<Order>> OrderManager::getCompletedOrders() {
    return completedOrders_;
}

std::vector<std::shared_ptr<Order>> OrderManager::getOrdersByTable(int tableNumber) {
    std::vector<std::shared_ptr<Order>> orders;
    
    for (const auto& pair : activeOrders_) {
        if (pair.second->getTableNumber() == tableNumber) {
            orders.push_back(pair.second);
        }
    }
    
    return orders;
}

std::vector<std::shared_ptr<Order>> OrderManager::getOrdersByStatus(Order::Status status) {
    std::vector<std::shared_ptr<Order>> orders;
    
    for (const auto& pair : activeOrders_) {
        if (pair.second->getStatus() == status) {
            orders.push_back(pair.second);
        }
    }
    
    // Also check completed orders if looking for SERVED status
    if (status == Order::SERVED) {
        for (const auto& order : completedOrders_) {
            if (order->getStatus() == status) {
                orders.push_back(order);
            }
        }
    }
    
    return orders;
}

bool OrderManager::completeOrder(int orderId) {
    auto order = removeFromActive(orderId);
    if (order) {
        Order::Status oldStatus = order->getStatus();
        order->setStatus(Order::SERVED);
        completedOrders_.push_back(order);
        
        // Notify observers
        onOrderStatusChanged(order, oldStatus, Order::SERVED);
        onOrderCompleted(order);
        
        return true;
    }
    return false;
}

bool OrderManager::cancelOrder(int orderId) {
    auto order = removeFromActive(orderId);
    if (order) {
        Order::Status oldStatus = order->getStatus();
        order->setStatus(Order::CANCELLED);
        completedOrders_.push_back(order);
        
        // Notify observers
        onOrderStatusChanged(order, oldStatus, Order::CANCELLED);
        onOrderCancelled(order);
        
        return true;
    }
    return false;
}

bool OrderManager::updateOrderStatus(int orderId, Order::Status status) {
    auto it = activeOrders_.find(orderId);
    if (it != activeOrders_.end()) {
        Order::Status oldStatus = it->second->getStatus();
        it->second->setStatus(status);
        
        // Notify observers
        onOrderStatusChanged(it->second, oldStatus, status);
        onOrderModified(it->second);
        
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