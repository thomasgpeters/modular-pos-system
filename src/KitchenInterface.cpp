#include "../include/KitchenInterface.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

/**
 * @file KitchenInterface.cpp
 * @brief Implementation of the KitchenInterface class
 */

KitchenInterface::KitchenInterface() : wasKitchenBusy_(false) {
}

bool KitchenInterface::sendOrderToKitchen(std::shared_ptr<Order> order) {
    if (!order || order->getItems().empty()) {
        return false;
    }
    
    // Create kitchen ticket
    KitchenTicket ticket = createKitchenTicket(order);
    
    // Add to active tickets
    activeTickets_.push_back(ticket);
    
    // Create JSON message for kitchen displays
    Wt::Json::Object kitchenMessage;
    kitchenMessage["type"] = Wt::Json::Value("NEW_ORDER");
    kitchenMessage["orderId"] = Wt::Json::Value(ticket.orderId);
    kitchenMessage["tableNumber"] = Wt::Json::Value(ticket.tableNumber);
    kitchenMessage["timestamp"] = Wt::Json::Value(getCurrentTimestamp());
    kitchenMessage["estimatedPrepTime"] = Wt::Json::Value(ticket.estimatedPrepTime);
    
    Wt::Json::Array itemsArray;
    for (const auto& item : ticket.items) {
        itemsArray.push_back(Wt::Json::Value(item));
    }
    kitchenMessage["items"] = itemsArray;
    
    if (!ticket.specialInstructions.empty()) {
        kitchenMessage["specialInstructions"] = Wt::Json::Value(ticket.specialInstructions);
    }
    
    // Send to kitchen displays
    bool success = broadcastToKitchen(kitchenMessage);
    
    if (success) {
        // Update order status
        order->setStatus(Order::SENT_TO_KITCHEN);
        
        // Check if kitchen became busy
        if (!wasKitchenBusy_ && isKitchenBusy()) {
            wasKitchenBusy_ = true;
            onKitchenBusy(activeTickets_.size());
        }
        
        // Notify observers
        onOrderSentToKitchen(order, ticket);
    }
    
    return success;
}

bool KitchenInterface::updateKitchenStatus(int orderId, KitchenStatus status) {
    int index = findTicketIndex(orderId);
    if (index == -1) {
        return false;
    }
    
    KitchenStatus oldStatus = activeTickets_[index].status;
    activeTickets_[index].status = status;
    
    // Create status update message
    Wt::Json::Object statusMessage;
    statusMessage["type"] = Wt::Json::Value("STATUS_UPDATE");
    statusMessage["orderId"] = Wt::Json::Value(orderId);
    statusMessage["status"] = Wt::Json::Value(static_cast<int>(status));
    statusMessage["statusName"] = Wt::Json::Value(kitchenStatusToString(status));
    statusMessage["timestamp"] = Wt::Json::Value(getCurrentTimestamp());
    
    // Broadcast status update
    broadcastToKitchen(statusMessage);
    
    // Remove ticket if order is served
    if (status == SERVED) {
        activeTickets_.erase(activeTickets_.begin() + index);
        
        // Check if kitchen is no longer busy
        if (wasKitchenBusy_ && !isKitchenBusy()) {
            wasKitchenBusy_ = false;
            onKitchenFree(activeTickets_.size());
        }
    }
    
    // Notify observers
    onKitchenStatusUpdated(orderId, oldStatus, status);
    
    return true;
}

Wt::Json::Object KitchenInterface::getKitchenQueueStatus() {
    Wt::Json::Object queueStatus;
    Wt::Json::Array activeTicketsArray;
    
    for (const auto& ticket : activeTickets_) {
        Wt::Json::Object ticketObj;
        ticketObj["orderId"] = Wt::Json::Value(ticket.orderId);
        ticketObj["tableNumber"] = Wt::Json::Value(ticket.tableNumber);
        ticketObj["status"] = Wt::Json::Value(static_cast<int>(ticket.status));
        ticketObj["statusName"] = Wt::Json::Value(kitchenStatusToString(ticket.status));
        ticketObj["estimatedPrepTime"] = Wt::Json::Value(ticket.estimatedPrepTime);
        
        // Calculate elapsed time
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - ticket.timestamp);
        ticketObj["elapsedTime"] = Wt::Json::Value(static_cast<int>(elapsed.count()));
        
        Wt::Json::Array itemsArray;
        for (const auto& item : ticket.items) {
            itemsArray.push_back(Wt::Json::Value(item));
        }
        ticketObj["items"] = itemsArray;
        
        if (!ticket.specialInstructions.empty()) {
            ticketObj["specialInstructions"] = Wt::Json::Value(ticket.specialInstructions);
        }
        
        activeTicketsArray.push_back(ticketObj);
    }
    
    queueStatus["activeTickets"] = activeTicketsArray;
    queueStatus["queueLength"] = Wt::Json::Value(static_cast<int>(activeTickets_.size()));
    queueStatus["estimatedWaitTime"] = Wt::Json::Value(getEstimatedWaitTime());
    queueStatus["isKitchenBusy"] = Wt::Json::Value(isKitchenBusy());
    queueStatus["lastUpdated"] = Wt::Json::Value(getCurrentTimestamp());
    
    return queueStatus;
}

std::vector<KitchenInterface::KitchenTicket> KitchenInterface::getActiveTickets() {
    return activeTickets_;
}

const KitchenInterface::KitchenTicket* KitchenInterface::getTicketByOrderId(int orderId) {
    int index = findTicketIndex(orderId);
    return (index != -1) ? &activeTickets_[index] : nullptr;
}

bool KitchenInterface::removeTicket(int orderId) {
    int index = findTicketIndex(orderId);
    if (index != -1) {
        activeTickets_.erase(activeTickets_.begin() + index);
        
        // Check if kitchen is no longer busy
        if (wasKitchenBusy_ && !isKitchenBusy()) {
            wasKitchenBusy_ = false;
            onKitchenFree(activeTickets_.size());
        }
        
        return true;
    }
    return false;
}

int KitchenInterface::getEstimatedWaitTime() {
    if (activeTickets_.empty()) {
        return 0;
    }
    
    // Simple estimation: average remaining prep time of active orders
    int totalEstimatedTime = 0;
    for (const auto& ticket : activeTickets_) {
        // Calculate remaining time based on status
        int remainingTime = ticket.estimatedPrepTime;
        if (ticket.status == PREP_STARTED) {
            remainingTime = std::max(0, remainingTime / 2); // Assume half done
        } else if (ticket.status == READY_FOR_PICKUP) {
            remainingTime = 1; // Almost done
        }
        totalEstimatedTime += remainingTime;
    }
    
    // Add base time for new order
    return totalEstimatedTime + 10; // 10 minutes base prep time
}

std::string KitchenInterface::kitchenStatusToString(KitchenStatus status) {
    switch (status) {
        case ORDER_RECEIVED:    return "Order Received";
        case PREP_STARTED:      return "Prep Started";
        case READY_FOR_PICKUP:  return "Ready for Pickup";
        case SERVED:            return "Served";
        default:                return "Unknown";
    }
}

bool KitchenInterface::broadcastMessage(const std::string& message) {
    Wt::Json::Object messageObj;
    messageObj["type"] = Wt::Json::Value("BROADCAST");
    messageObj["message"] = Wt::Json::Value(message);
    messageObj["timestamp"] = Wt::Json::Value(getCurrentTimestamp());
    
    return broadcastToKitchen(messageObj);
}

// Protected methods

KitchenInterface::KitchenTicket KitchenInterface::createKitchenTicket(std::shared_ptr<Order> order) {
    KitchenTicket ticket;
    ticket.orderId = order->getOrderId();
    ticket.tableNumber = order->getTableNumber();
    ticket.timestamp = std::chrono::system_clock::now();
    ticket.status = ORDER_RECEIVED;
    ticket.estimatedPrepTime = estimatePreparationTime(order);
    
    // Extract items and special instructions
    std::vector<std::string> allInstructions;
    
    for (const auto& orderItem : order->getItems()) {
        // Format: "Quantity x Item Name"
        std::stringstream itemStr;
        itemStr << orderItem.getQuantity() << "x " << orderItem.getMenuItem().getName();
        ticket.items.push_back(itemStr.str());
        
        // Collect special instructions
        if (!orderItem.getSpecialInstructions().empty()) {
            allInstructions.push_back(orderItem.getSpecialInstructions());
        }
    }
    
    // Combine all special instructions
    if (!allInstructions.empty()) {
        std::stringstream instructionsStr;
        for (size_t i = 0; i < allInstructions.size(); ++i) {
            if (i > 0) instructionsStr << "; ";
            instructionsStr << allInstructions[i];
        }
        ticket.specialInstructions = instructionsStr.str();
    }
    
    return ticket;
}

bool KitchenInterface::broadcastToKitchen(const Wt::Json::Object& message) {
    // Placeholder for actual WebSocket broadcast implementation
    // In a real implementation, this would:
    // 1. Get list of connected kitchen display clients
    // 2. Send JSON message to each client via WebSocket
    // 3. Handle connection errors and retries
    // 4. Log successful/failed transmissions
    
    // For now, simulate successful broadcast
    return true;
}

int KitchenInterface::estimatePreparationTime(std::shared_ptr<Order> order) {
    int totalTime = 0;
    
    for (const auto& orderItem : order->getItems()) {
        int itemTime = 5; // Base 5 minutes per item
        
        // Adjust based on category
        switch (orderItem.getMenuItem().getCategory()) {
            case MenuItem::APPETIZER:   itemTime = 8; break;
            case MenuItem::MAIN_COURSE: itemTime = 15; break;
            case MenuItem::DESSERT:     itemTime = 6; break;
            case MenuItem::BEVERAGE:    itemTime = 2; break;
            case MenuItem::SPECIAL:     itemTime = 20; break;
        }
        
        // Multiply by quantity
        totalTime += itemTime * orderItem.getQuantity();
    }
    
    // Add complexity factor for multiple items
    if (order->getItems().size() > 3) {
        totalTime += 5; // Additional coordination time
    }
    
    return std::max(5, totalTime); // Minimum 5 minutes
}

// Private methods

std::string KitchenInterface::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto timestamp_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&timestamp_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

int KitchenInterface::findTicketIndex(int orderId) {
    for (size_t i = 0; i < activeTickets_.size(); ++i) {
        if (activeTickets_[i].orderId == orderId) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
