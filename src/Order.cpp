#include "../include/Order.hpp"

/**
 * @file Order.cpp
 * @brief Implementation of the OrderItem and Order classes
 */

// OrderItem Implementation
OrderItem::OrderItem(const MenuItem& menuItem, int quantity)
    : menuItem_(menuItem), quantity_(quantity) {
    totalPrice_ = menuItem_.getPrice() * quantity_;
}

void OrderItem::setQuantity(int quantity) {
    quantity_ = quantity;
    totalPrice_ = menuItem_.getPrice() * quantity_;
}

Wt::Json::Object OrderItem::toJson() const {
    Wt::Json::Object obj;
    obj["menuItem"] = menuItem_.toJson();
    obj["quantity"] = Wt::Json::Value(quantity_);
    obj["totalPrice"] = Wt::Json::Value(totalPrice_);
    obj["specialInstructions"] = Wt::Json::Value(specialInstructions_);
    return obj;
}

// Order Implementation
Order::Order(int orderId, int tableNumber)
    : orderId_(orderId), tableNumber_(tableNumber), status_(PENDING),
      subtotal_(0.0), tax_(0.0), total_(0.0),
      timestamp_(std::chrono::system_clock::now()) {
}

void Order::addItem(const OrderItem& item) {
    items_.push_back(item);
    calculateTotals();
}

void Order::removeItem(size_t index) {
    if (index < items_.size()) {
        items_.erase(items_.begin() + index);
        calculateTotals();
    }
}

void Order::updateItemQuantity(size_t index, int quantity) {
    if (index < items_.size()) {
        items_[index].setQuantity(quantity);
        calculateTotals();
    }
}

void Order::calculateTotals() {
    subtotal_ = 0.0;
    for (const auto& item : items_) {
        subtotal_ += item.getTotalPrice();
    }
    tax_ = subtotal_ * TAX_RATE;
    total_ = subtotal_ + tax_;
}

Wt::Json::Object Order::toJson() const {
    Wt::Json::Object obj;
    obj["orderId"] = Wt::Json::Value(orderId_);
    obj["tableNumber"] = Wt::Json::Value(tableNumber_);
    obj["status"] = Wt::Json::Value(static_cast<int>(status_));
    obj["statusName"] = Wt::Json::Value(statusToString(status_));
    obj["subtotal"] = Wt::Json::Value(subtotal_);
    obj["tax"] = Wt::Json::Value(tax_);
    obj["total"] = Wt::Json::Value(total_);
    
    // Convert timestamp to string
    auto timestamp_t = std::chrono::system_clock::to_time_t(timestamp_);
    obj["timestamp"] = Wt::Json::Value(static_cast<int64_t>(timestamp_t));
    
    Wt::Json::Array itemsArray;
    for (const auto& item : items_) {
        itemsArray.push_back(item.toJson());
    }
    obj["items"] = itemsArray;
    
    return obj;
}

std::string Order::statusToString(Status status) {
    switch (status) {
        case PENDING:           return "Pending";
        case SENT_TO_KITCHEN:   return "In Kitchen";
        case PREPARING:         return "Preparing";
        case READY:             return "Ready";
        case SERVED:            return "Served";
        case CANCELLED:         return "Cancelled";
        default:                return "Unknown";
    }
}
