#include "../include/KitchenInterface.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

KitchenInterface::KitchenInterface() : wasKitchenBusy_(false) {}

bool KitchenInterface::sendOrderToKitchen(std::shared_ptr<Order> order) {
    if (!order) return false;
    
    // Create kitchen ticket
    KitchenTicket ticket = createKitchenTicket(order);
    
    // Update order status
    order->setStatus(Order::SENT_TO_KITCHEN);
    
    // Add to active tickets
    activeTickets_.push_back(ticket);
    
    // Check for busy state change
    bool isBusy = isKitchenBusy();
    if (isBusy && !wasKitchenBusy_) {
        wasKitchenBusy_ = true;
        onKitchenBusy(activeTickets_.size());
    }
    
    // Call extension point
    onOrderSentToKitchen(order, ticket);
    
    // Broadcast to kitchen displays
    Wt::Json::Object message;
    message["type"] = Wt::Json::Value("new_order");
    message["orderId"] = Wt::Json::Value(order->getOrderId());
    message["tableNumber"] = Wt::Json::Value(order->getTableNumber());
    message["items"] = Wt::Json::Value(static_cast<int>(order->getItems().size()));
    message["timestamp"] = Wt::Json::Value(getCurrentTimestamp());
    
    return broadcastToKitchen(message);
}

bool KitchenInterface::updateKitchenStatus(int orderId, KitchenStatus status) {
    int ticketIndex = findTicketIndex(orderId);
    if (ticketIndex == -1) return false;
    
    KitchenStatus oldStatus = activeTickets_[ticketIndex].status;
    activeTickets_[ticketIndex].status = status;
    
    // Update order status based on kitchen status
    // Note: In a real system, we'd need access to OrderManager here
    
    // Call extension point
    onKitchenStatusUpdated(orderId, oldStatus, status);
    
    // Check for busy state change
    bool isBusy = isKitchenBusy();
    if (!isBusy && wasKitchenBusy_) {
        wasKitchenBusy_ = false;
        onKitchenFree(activeTickets_.size());
    }
    
    // Broadcast status update
    Wt::Json::Object message;
    message["type"] = Wt::Json::Value("status_update");
    message["orderId"] = Wt::Json::Value(orderId);
    message["status"] = Wt::Json::Value(static_cast<int>(status));
    message["statusName"] = Wt::Json::Value(kitchenStatusToString(status));
    message["timestamp"] = Wt::Json::Value(getCurrentTimestamp());
    
    return broadcastToKitchen(message);
}

Wt::Json::Object KitchenInterface::getKitchenQueueStatus() {
    Wt::Json::Object status;
    status["queueLength"] = Wt::Json::Value(static_cast<int>(activeTickets_.size()));
    status["estimatedWaitTime"] = Wt::Json::Value(getEstimatedWaitTime());
    status["isKitchenBusy"] = Wt::Json::Value(isKitchenBusy());
    
    // Add status breakdown
    std::map<KitchenStatus, int> statusCounts;
    for (const auto& ticket : activeTickets_) {
        statusCounts[ticket.status]++;
    }
    
    Wt::Json::Object statusBreakdown;
    statusBreakdown["received"] = Wt::Json::Value(statusCounts[ORDER_RECEIVED]);
    statusBreakdown["preparing"] = Wt::Json::Value(statusCounts[PREP_STARTED]);
    statusBreakdown["ready"] = Wt::Json::Value(statusCounts[READY_FOR_PICKUP]);
    
    status["statusBreakdown"] = statusBreakdown;
    status["lastUpdated"] = Wt::Json::Value(getCurrentTimestamp());
    
    return status;
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
        
        // Check for busy state change
        bool isBusy = isKitchenBusy();
        if (!isBusy && wasKitchenBusy_) {
            wasKitchenBusy_ = false;
            onKitchenFree(activeTickets_.size());
        }
        
        return true;
    }
    return false;
}

int KitchenInterface::getEstimatedWaitTime() {
    if (activeTickets_.empty()) return 0;
    
    int totalTime = 0;
    for (const auto& ticket : activeTickets_) {
        if (ticket.status == ORDER_RECEIVED || ticket.status == PREP_STARTED) {
            totalTime += ticket.estimatedPrepTime;
        }
    }
    
    // Add base wait time based on queue length
    int baseWaitTime = std::min(static_cast<int>(activeTickets_.size()) * 2, 15);
    
    return std::max(totalTime / 2, baseWaitTime); // Average estimate
}

std::string KitchenInterface::kitchenStatusToString(KitchenStatus status) {
    switch (status) {
        case ORDER_RECEIVED:    return "Order Received";
        case PREP_STARTED:      return "Preparing";
        case READY_FOR_PICKUP:  return "Ready for Pickup";
        case SERVED:            return "Served";
        default:                return "Unknown";
    }
}

bool KitchenInterface::broadcastMessage(const std::string& message) {
    Wt::Json::Object jsonMessage;
    jsonMessage["type"] = Wt::Json::Value("broadcast");
    jsonMessage["message"] = Wt::Json::Value(message);
    jsonMessage["timestamp"] = Wt::Json::Value(getCurrentTimestamp());
    
    return broadcastToKitchen(jsonMessage);
}

KitchenInterface::KitchenTicket KitchenInterface::createKitchenTicket(std::shared_ptr<Order> order) {
    KitchenTicket ticket;
    ticket.orderId = order->getOrderId();
    ticket.tableNumber = order->getTableNumber();
    ticket.timestamp = std::chrono::system_clock::now();
    ticket.status = ORDER_RECEIVED;
    ticket.estimatedPrepTime = estimatePreparationTime(order);
    
    // Extract item names and special instructions
    for (const auto& item : order->getItems()) {
        for (int i = 0; i < item.getQuantity(); ++i) {
            ticket.items.push_back(item.getMenuItem().getName());
        }
        
        if (!item.getSpecialInstructions().empty()) {
            if (!ticket.specialInstructions.empty()) {
                ticket.specialInstructions += "; ";
            }
            ticket.specialInstructions += item.getSpecialInstructions();
        }
    }
    
    return ticket;
}

bool KitchenInterface::broadcastToKitchen(const Wt::Json::Object& message) {
    // In a real implementation, this would send to kitchen display systems
    // For demo purposes, we'll just return true
    return true;
}

int KitchenInterface::estimatePreparationTime(std::shared_ptr<Order> order) {
    // Simple estimation based on item count and types
    int baseTime = 5; // 5 minutes base
    int itemTime = order->getItems().size() * 3; // 3 minutes per item
    
    return std::min(baseTime + itemTime, 30); // Cap at 30 minutes
}

std::string KitchenInterface::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

int KitchenInterface::findTicketIndex(int orderId) {
    auto it = std::find_if(activeTickets_.begin(), activeTickets_.end(),
        [orderId](const KitchenTicket& ticket) { return ticket.orderId == orderId; });
    
    return (it != activeTickets_.end()) ? std::distance(activeTickets_.begin(), it) : -1;
}
