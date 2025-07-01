#include "../include/Order.hpp"
#include <chrono>

// OrderItem Implementation
OrderItem::OrderItem(const MenuItem& menuItem, int quantity)
    : menuItem_(menuItem), quantity_(quantity), specialInstructions_("") {
    setQuantity(quantity);
}

void OrderItem::setQuantity(int quantity) {
    quantity_ = std::max(1, quantity);
    totalPrice_ = quantity_ * menuItem_.getPrice();
}

Wt::Json::Object OrderItem::toJson() const {
    Wt::Json::Object json;
    json["menuItem"] = menuItem_.toJson();
    json["quantity"] = Wt::Json::Value(quantity_);
    json["totalPrice"] = Wt::Json::Value(totalPrice_);
    json["specialInstructions"] = Wt::Json::Value(specialInstructions_);
    return json;
}

// Order Implementation
Order::Order(int orderId, int tableNumber)
    : orderId_(orderId), tableNumber_(tableNumber), status_(PENDING),
      subtotal_(0.0), tax_(0.0), total_(0.0),
      timestamp_(std::chrono::system_clock::now()) {}

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
    Wt::Json::Object json;
    json["orderId"] = Wt::Json::Value(orderId_);
    json["tableNumber"] = Wt::Json::Value(tableNumber_);
    json["status"] = Wt::Json::Value(static_cast<int>(status_));
    json["statusName"] = Wt::Json::Value(statusToString(status_));
    json["subtotal"] = Wt::Json::Value(subtotal_);
    json["tax"] = Wt::Json::Value(tax_);
    json["total"] = Wt::Json::Value(total_);
    json["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
        std::chrono::system_clock::to_time_t(timestamp_)));
    
    Wt::Json::Array itemsArray;
    for (const auto& item : items_) {
        itemsArray.push_back(item.toJson());
    }
    json["items"] = itemsArray;
    
    return json;
}

std::string Order::statusToString(Status status) {
    switch (status) {
        case PENDING:         return "Pending";
        case SENT_TO_KITCHEN: return "Sent to Kitchen";
        case PREPARING:       return "Preparing";
        case READY:           return "Ready";
        case SERVED:          return "Served";
        case CANCELLED:       return "Cancelled";
        default:              return "Unknown";
    }
}
