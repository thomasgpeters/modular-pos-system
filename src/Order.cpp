#include "../include/Order.hpp"

#include <algorithm>
#include <sstream>
#include <regex>

// OrderItem Implementation
OrderItem::OrderItem(const MenuItem& menuItem, int quantity)
    : menuItem_(menuItem), quantity_(quantity), specialInstructions_("")
{
    setQuantity(quantity); // This will calculate totalPrice_
}

void OrderItem::setQuantity(int quantity) {
    quantity_ = std::max(1, quantity); // Ensure minimum quantity of 1
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
Order::Order(int orderId, const std::string& tableIdentifier)
    : orderId_(orderId)
    , tableIdentifier_(tableIdentifier)
    , status_(PENDING)
    , subtotal_(0.0)
    , tax_(0.0)
    , total_(0.0)
    , timestamp_(std::chrono::system_clock::now())
{
    if (!isValidTableIdentifier(tableIdentifier)) {
        throw std::invalid_argument("Invalid table identifier: " + tableIdentifier);
    }
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

int Order::getTableNumber() const {
    return extractTableNumber();
}

bool Order::isDineIn() const {
    return tableIdentifier_.find("table") == 0;
}

bool Order::isDelivery() const {
    return tableIdentifier_ == "grubhub" || tableIdentifier_ == "ubereats";
}

bool Order::isWalkIn() const {
    return tableIdentifier_ == "walk-in";
}

std::string Order::getOrderType() const {
    if (isDineIn()) {
        return "Dine-In";
    } else if (isDelivery()) {
        return "Delivery";
    } else if (isWalkIn()) {
        return "Walk-In";
    }
    return "Unknown";
}

Wt::Json::Object Order::toJson() const {
    Wt::Json::Object json;
    json["orderId"] = Wt::Json::Value(orderId_);
    json["tableIdentifier"] = Wt::Json::Value(tableIdentifier_);
    json["tableNumber"] = Wt::Json::Value(getTableNumber()); // Legacy compatibility
    json["status"] = Wt::Json::Value(statusToString(status_));
    json["orderType"] = Wt::Json::Value(getOrderType());
    json["subtotal"] = Wt::Json::Value(subtotal_);
    json["tax"] = Wt::Json::Value(tax_);
    json["total"] = Wt::Json::Value(total_);
    
    // Convert timestamp to string
    auto time_t = std::chrono::system_clock::to_time_t(timestamp_);
    std::ostringstream oss;
    oss << time_t;
    json["timestamp"] = Wt::Json::Value(oss.str());
    
    // Add items array
    Wt::Json::Array itemsArray;
    for (const auto& item : items_) {
        itemsArray.push_back(Wt::Json::Value(item.toJson()));
    }
    json["items"] = Wt::Json::Value(itemsArray);
    
    return json;
}

std::string Order::statusToString(Status status) {
    switch (status) {
        case PENDING: return "Pending";
        case SENT_TO_KITCHEN: return "Sent to Kitchen";
        case PREPARING: return "Preparing";
        case READY: return "Ready";
        case SERVED: return "Served";
        case CANCELLED: return "Cancelled";
        default: return "Unknown";
    }
}

bool Order::isValidTableIdentifier(const std::string& identifier) {
    if (identifier.empty()) {
        return false;
    }
    
    // Check for valid patterns
    // Table numbers: "table 1", "table 2", etc.
    std::regex tablePattern(R"(^table \d+$)");
    if (std::regex_match(identifier, tablePattern)) {
        return true;
    }
    
    // Special locations
    if (identifier == "walk-in" || identifier == "grubhub" || identifier == "ubereats") {
        return true;
    }
    
    return false;
}

std::vector<std::string> Order::getTableIdentifierOptions() {
    std::vector<std::string> options;
    
    // Add table numbers 1-20
    for (int i = 1; i <= 20; ++i) {
        options.push_back("table " + std::to_string(i));
    }
    
    // Add special locations
    options.push_back("walk-in");
    options.push_back("grubhub");
    options.push_back("ubereats");
    
    return options;
}

void Order::calculateTotals() {
    subtotal_ = 0.0;
    
    for (const auto& item : items_) {
        subtotal_ += item.getTotalPrice();
    }
    
    tax_ = subtotal_ * TAX_RATE;
    total_ = subtotal_ + tax_;
}

int Order::extractTableNumber() const {
    // Extract numeric table number for legacy compatibility
    if (isDineIn()) {
        std::regex tablePattern(R"(^table (\d+)$)");
        std::smatch match;
        if (std::regex_match(tableIdentifier_, match, tablePattern)) {
            return std::stoi(match[1].str());
        }
    }
    return 0; // Non-table orders return 0
}

// Example usage and helper functions for the main application

namespace POSHelpers {
    
    /**
     * @brief Generates table identifier options for UI combo box
     * @param maxTables Maximum number of tables to include
     * @return Vector of formatted table identifier options
     */
    std::vector<std::string> generateTableIdentifierOptions(int maxTables = 20) {
        std::vector<std::string> options;
        
        // Add table numbers
        for (int i = 1; i <= maxTables; ++i) {
            options.push_back("table " + std::to_string(i));
        }
        
        // Add special locations
        options.push_back("walk-in");
        options.push_back("grubhub");
        options.push_back("ubereats");
        
        return options;
    }
    
    /**
     * @brief Extracts clean table identifier from display text
     * @param displayText Text from UI combo box (may include icons)
     * @return Clean table identifier
     */
    std::string extractTableIdentifier(const std::string& displayText) {
        // Remove emoji prefix if present (e.g., "🪑 table 5" -> "table 5")
        size_t spacePos = displayText.find(' ');
        if (spacePos != std::string::npos && spacePos < 3) {
            return displayText.substr(spacePos + 1);
        }
        return displayText;
    }
    
    /**
     * @brief Gets appropriate icon for table identifier
     * @param tableIdentifier Table identifier to get icon for
     * @return Unicode emoji icon
     */
    std::string getTableIdentifierIcon(const std::string& tableIdentifier) {
        if (tableIdentifier.find("table") == 0) {
            return "🪑"; // Chair icon for dine-in
        } else if (tableIdentifier == "grubhub" || tableIdentifier == "ubereats") {
            return "🚗"; // Car icon for delivery
        } else if (tableIdentifier == "walk-in") {
            return "🚶"; // Walking icon for walk-in
        }
        return "📋"; // Default clipboard icon
    }
    
    /**
     * @brief Formats table identifier for display
     * @param tableIdentifier Table identifier to format
     * @return Formatted display text with icon
     */
    std::string formatTableIdentifierForDisplay(const std::string& tableIdentifier) {
        return getTableIdentifierIcon(tableIdentifier) + " " + tableIdentifier;
    }
}

// Example of how to use the new Order system in your main application:

/*
// Creating orders with string table identifiers
auto dineInOrder = std::make_shared<Order>(1001, "table 5");
auto deliveryOrder = std::make_shared<Order>(1002, "grubhub");
auto walkInOrder = std::make_shared<Order>(1003, "walk-in");

// Check order types
if (dineInOrder->isDineIn()) {
    std::cout << "This is a dine-in order for " << dineInOrder->getTableIdentifier() << std::endl;
}

if (deliveryOrder->isDelivery()) {
    std::cout << "This is a delivery order via " << deliveryOrder->getTableIdentifier() << std::endl;
}

// Get formatted display text for UI
std::string displayText = POSHelpers::formatTableIdentifierForDisplay("table 7");
// Result: "🪑 table 7"

// Generate options for combo box
auto options = POSHelpers::generateTableIdentifierOptions(15);
// Results in: ["table 1", "table 2", ..., "walk-in", "grubhub", "ubereats"]

// In your UI code, you can add icons when populating the combo box:
for (const auto& identifier : options) {
    std::string displayText = POSHelpers::formatTableIdentifierForDisplay(identifier);
    tableCombo->addItem(displayText);
}
*/
