//============================================================================
// include/api/repositories/OrderRepository.hpp
//============================================================================

#ifndef ORDERREPOSITORY_H
#define ORDERREPOSITORY_H

#include "../APIRepository.hpp"
#include "../../Order.hpp"
#include <string>
#include <functional>
#include <vector>
#include <map>

/**
 * @class OrderRepository
 * @brief Repository for Order entity API operations
 * 
 * Handles CRUD operations for Order entities through the API middleware,
 * providing JSON:API compliant data serialization and deserialization.
 * Simplified version that works with basic Order class.
 */
class OrderRepository : public APIRepository<Order> {
public:
    /**
     * @brief Constructs repository with API client
     * @param client Shared pointer to API client
     */
    explicit OrderRepository(std::shared_ptr<APIClient> client) 
        : APIRepository(client, "/Order") {}
    
    /**
     * @brief Virtual destructor
     */
    virtual ~OrderRepository() = default;
    
    // =================================================================
    // Specialized Query Methods
    // =================================================================
    
    /**
     * @brief Finds orders by status
     * @param status Order status
     * @param callback Callback with results
     */
    void findByStatus(Order::Status status,
                     std::function<void(std::vector<Order>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[status]"] = std::to_string(static_cast<int>(status));
        findAll(params, callback);
    }
    
    /**
     * @brief Finds orders by table identifier
     * @param tableIdentifier Table identifier
     * @param callback Callback with results
     */
    void findByTableIdentifier(const std::string& tableIdentifier,
                              std::function<void(std::vector<Order>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[table_identifier]"] = tableIdentifier;
        findAll(params, callback);
    }
    
    /**
     * @brief Finds active orders (not served or cancelled)
     * @param callback Callback with results
     */
    void findActive(std::function<void(std::vector<Order>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        // Use integer values for status filtering
        params["filter[status]"] = "0,1,2,3"; // PENDING,SENT_TO_KITCHEN,PREPARING,READY
        findAll(params, callback);
    }
    
    /**
     * @brief Finds orders by date range
     * @param startDate Start date (ISO format)
     * @param endDate End date (ISO format)
     * @param callback Callback with results
     */
    void findByDateRange(const std::string& startDate, const std::string& endDate,
                        std::function<void(std::vector<Order>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[created_at][gte]"] = startDate;
        params["filter[created_at][lte]"] = endDate;
        findAll(params, callback);
    }
    
    /**
     * @brief Finds orders above a certain total amount
     * @param minAmount Minimum total amount
     * @param callback Callback with results
     */
    void findByMinimumTotal(double minAmount,
                           std::function<void(std::vector<Order>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[total][gte]"] = std::to_string(minAmount);
        findAll(params, callback);
    }

protected:
    /**
     * @brief Converts JSON:API data to Order object
     * @param json JSON value from API response
     * @return Unique pointer to Order or nullptr if conversion fails
     */
    std::unique_ptr<Order> fromJson(const Wt::Json::Value& json) override {
        if (json.type() != Wt::Json::Type::Object) {
            return nullptr;
        }
        
        auto jsonObj = static_cast<const Wt::Json::Object&>(json);
        
        // Get the ID from the top level
        int orderId = safeGetInt(jsonObj, "id");
        
        // Get attributes object
        auto attributesValue = safeGetValue(jsonObj, "attributes");
        if (attributesValue.type() != Wt::Json::Type::Object) {
            return nullptr;
        }
        
        auto attrs = static_cast<const Wt::Json::Object&>(attributesValue);
        
        // Extract basic order information
        std::string tableIdentifier = safeGetString(attrs, "table_identifier");
        
        // Create order object
        auto order = std::make_unique<Order>(orderId, tableIdentifier);
        
        // Set status using integer mapping
        int statusInt = safeGetInt(attrs, "status", 0); // Default to PENDING
        Order::Status status = static_cast<Order::Status>(statusInt);
        order->setStatus(status);
        
        // Parse order items if included
        auto itemsValue = safeGetValue(attrs, "items");
        if (itemsValue.type() == Wt::Json::Type::Array) {
            auto itemsArray = static_cast<const Wt::Json::Array&>(itemsValue);
            
            for (const auto& itemJson : itemsArray) {
                if (itemJson.type() == Wt::Json::Type::Object) {
                    auto itemObj = static_cast<const Wt::Json::Object&>(itemJson);
                    
                    // Create a simplified MenuItem for the order item
                    int menuItemId = safeGetInt(itemObj, "menu_item_id");
                    std::string itemName = safeGetString(itemObj, "name");
                    double price = safeGetDouble(itemObj, "price");
                    
                    // Create a basic MenuItem
                    MenuItem menuItem(menuItemId, itemName, price, MenuItem::MAIN_COURSE);
                    
                    int quantity = safeGetInt(itemObj, "quantity", 1);
                    std::string instructions = safeGetString(itemObj, "special_instructions");
                    
                    OrderItem orderItem(menuItem, quantity);
                    if (!instructions.empty()) {
                        orderItem.setSpecialInstructions(instructions);
                    }
                    
                    order->addItem(orderItem);
                }
            }
        }
        
        return order;
    }
    
    /**
     * @brief Converts Order object to JSON:API format
     * @param order Order object to convert
     * @return JSON object in JSON:API format
     */
    Wt::Json::Object toJson(const Order& order) override {
        Wt::Json::Object jsonObj;
        
        // JSON:API structure
        jsonObj["type"] = Wt::Json::Value("Order");
        
        // Include ID if present
        if (order.getOrderId() > 0) {
            jsonObj["id"] = Wt::Json::Value(std::to_string(order.getOrderId()));
        }
        
        // Create attributes object
        Wt::Json::Object attributes;
        attributes["table_identifier"] = Wt::Json::Value(order.getTableIdentifier());
        attributes["status"] = Wt::Json::Value(static_cast<int>(order.getStatus()));
        
        // Add order items
        Wt::Json::Array itemsArray;
        for (const auto& item : order.getItems()) {
            Wt::Json::Object itemObj;
            itemObj["menu_item_id"] = Wt::Json::Value(item.getMenuItem().getId());
            itemObj["name"] = Wt::Json::Value(item.getMenuItem().getName());
            itemObj["price"] = Wt::Json::Value(item.getMenuItem().getPrice());
            itemObj["quantity"] = Wt::Json::Value(item.getQuantity());
            
            if (!item.getSpecialInstructions().empty()) {
                itemObj["special_instructions"] = Wt::Json::Value(item.getSpecialInstructions());
            }
            
            itemsArray.push_back(itemObj);
        }
        attributes["items"] = itemsArray;
        
        // Add total
        attributes["total"] = Wt::Json::Value(order.getTotal());
        
        jsonObj["attributes"] = attributes;
        
        return jsonObj;
    }

private:
    /**
     * @brief Converts status enum to string for display
     * @param status Status enum
     * @return Status string
     */
    std::string statusToString(Order::Status status) const {
        switch (status) {
            case Order::PENDING:           return "pending";
            case Order::SENT_TO_KITCHEN:   return "sent_to_kitchen";
            case Order::PREPARING:         return "preparing";
            case Order::READY:             return "ready";
            case Order::SERVED:            return "served";
            case Order::CANCELLED:         return "cancelled";
            default:                       return "pending";
        }
    }
    
    /**
     * @brief Converts string to status enum
     * @param statusStr Status string
     * @return Status enum
     */
    Order::Status stringToStatus(const std::string& statusStr) const {
        if (statusStr == "pending")           return Order::PENDING;
        if (statusStr == "sent_to_kitchen")   return Order::SENT_TO_KITCHEN;
        if (statusStr == "preparing")         return Order::PREPARING;
        if (statusStr == "ready")             return Order::READY;
        if (statusStr == "served")            return Order::SERVED;
        if (statusStr == "cancelled")         return Order::CANCELLED;
        return Order::PENDING; // Default
    }
};

#endif // ORDERREPOSITORY_H
