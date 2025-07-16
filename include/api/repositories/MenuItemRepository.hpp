//============================================================================
// include/api/repositories/MenuItemRepository.hpp
//============================================================================

#ifndef MENUITEMREPOSITORY_H
#define MENUITEMREPOSITORY_H

#include "../APIRepository.hpp"
#include "../../MenuItem.hpp"
#include <string>
#include <functional>
#include <vector>
#include <map>

/**
 * @class MenuItemRepository
 * @brief Repository for MenuItem entity API operations
 * 
 * Handles CRUD operations for MenuItem entities through the API middleware,
 * providing JSON:API compliant data serialization and deserialization.
 * Simplified version that works with basic MenuItem class.
 */
class MenuItemRepository : public APIRepository<MenuItem> {
public:
    /**
     * @brief Constructs repository with API client
     * @param client Shared pointer to API client
     */
    explicit MenuItemRepository(std::shared_ptr<APIClient> client) 
        : APIRepository(client, "/MenuItem") {}
    
    /**
     * @brief Virtual destructor
     */
    virtual ~MenuItemRepository() = default;
    
    // =================================================================
    // Specialized Query Methods
    // =================================================================
    
    /**
     * @brief Finds menu items by category
     * @param category Menu item category
     * @param callback Callback with results
     */
    void findByCategory(MenuItem::Category category,
                       std::function<void(std::vector<MenuItem>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[category]"] = std::to_string(static_cast<int>(category));
        findAll(params, callback);
    }
    
    /**
     * @brief Finds available menu items only
     * @param callback Callback with results
     */
    void findAvailable(std::function<void(std::vector<MenuItem>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[available]"] = "true";
        findAll(params, callback);
    }
    
    /**
     * @brief Finds menu items by price range
     * @param minPrice Minimum price
     * @param maxPrice Maximum price
     * @param callback Callback with results
     */
    void findByPriceRange(double minPrice, double maxPrice,
                         std::function<void(std::vector<MenuItem>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[price][gte]"] = std::to_string(minPrice);
        params["filter[price][lte]"] = std::to_string(maxPrice);
        findAll(params, callback);
    }
    
    /**
     * @brief Searches menu items by name (partial match)
     * @param searchTerm Search term to match against item names
     * @param callback Callback with results
     */
    void searchByName(const std::string& searchTerm,
                     std::function<void(std::vector<MenuItem>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[name][like]"] = searchTerm;
        findAll(params, callback);
    }

protected:
    /**
     * @brief Converts JSON:API data to MenuItem object
     * @param json JSON value from API response
     * @return Unique pointer to MenuItem or nullptr if conversion fails
     */
    std::unique_ptr<MenuItem> fromJson(const Wt::Json::Value& json) override {
        if (json.type() != Wt::Json::Type::Object) {
            return nullptr;
        }
        
        auto jsonObj = static_cast<const Wt::Json::Object&>(json);
        
        // Get the ID from the top level
        int itemId = safeGetInt(jsonObj, "id");
        
        // Get attributes object
        auto attributesValue = safeGetValue(jsonObj, "attributes");
        if (attributesValue.type() != Wt::Json::Type::Object) {
            return nullptr;
        }
        
        auto attrs = static_cast<const Wt::Json::Object&>(attributesValue);
        
        // Extract basic menu item information
        std::string name = safeGetString(attrs, "name");
        double price = safeGetDouble(attrs, "price");
        
        // Get category - use integer mapping for simplicity
        int categoryInt = safeGetInt(attrs, "category", 2); // Default to MAIN_COURSE
        MenuItem::Category category = static_cast<MenuItem::Category>(categoryInt);
        
        // Create menu item object using basic constructor
        auto menuItem = std::make_unique<MenuItem>(itemId, name, price, category);
        
        // Set availability if provided
        bool available = safeGetBool(attrs, "available", true);
        menuItem->setAvailable(available);
        
        return menuItem;
    }
    
    /**
     * @brief Converts MenuItem object to JSON:API format
     * @param menuItem MenuItem object to convert
     * @return JSON object in JSON:API format
     */
    Wt::Json::Object toJson(const MenuItem& menuItem) override {
        Wt::Json::Object jsonObj;
        
        // JSON:API structure
        jsonObj["type"] = Wt::Json::Value("MenuItem");
        
        // Include ID if present
        if (menuItem.getId() > 0) {
            jsonObj["id"] = Wt::Json::Value(std::to_string(menuItem.getId()));
        }
        
        // Create attributes object with basic properties
        Wt::Json::Object attributes;
        attributes["name"] = Wt::Json::Value(menuItem.getName());
        attributes["price"] = Wt::Json::Value(menuItem.getPrice());
        attributes["category"] = Wt::Json::Value(static_cast<int>(menuItem.getCategory()));
        attributes["available"] = Wt::Json::Value(menuItem.isAvailable());
        
        jsonObj["attributes"] = attributes;
        
        return jsonObj;
    }

private:
    /**
     * @brief Converts category enum to string for display
     * @param category Category enum
     * @return Category string
     */
    std::string categoryToString(MenuItem::Category category) const {
        switch (category) {
            case MenuItem::APPETIZER:    return "appetizer";
            case MenuItem::MAIN_COURSE:  return "main_course";
            case MenuItem::DESSERT:      return "dessert";
            case MenuItem::BEVERAGE:     return "beverage";
            case MenuItem::SPECIAL:      return "special";
            default:                     return "main_course";
        }
    }
    
    /**
     * @brief Converts string to category enum
     * @param categoryStr Category string
     * @return Category enum
     */
    MenuItem::Category stringToCategory(const std::string& categoryStr) const {
        if (categoryStr == "appetizer")    return MenuItem::APPETIZER;
        if (categoryStr == "main_course")  return MenuItem::MAIN_COURSE;
        if (categoryStr == "dessert")      return MenuItem::DESSERT;
        if (categoryStr == "beverage")     return MenuItem::BEVERAGE;
        if (categoryStr == "special")      return MenuItem::SPECIAL;
        return MenuItem::MAIN_COURSE; // Default
    }
};

#endif // MENUITEMREPOSITORY_H
