#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>
#include <Wt/Json/Object.h>
#include <Wt/Json/Value.h>

/**
 * @file MenuItem.h
 * @brief Menu item management for the Restaurant POS System
 * 
 * This file contains the MenuItem class which represents individual menu items
 * with pricing, categories, and availability status. Designed for easy extension
 * with features like ingredients, allergen information, and nutritional data.
 * 
 * @author Restaurant POS Team
 * @version 1.0.0
 */

/**
 * @class MenuItem
 * @brief Represents a menu item with pricing and customization options
 * 
 * The MenuItem class encapsulates all information about a restaurant menu item
 * including name, price, category, and availability. It provides JSON serialization
 * for API communication and is designed to be easily extended with additional
 * properties like ingredients, allergens, and preparation time.
 */
class MenuItem {
public:
    /**
     * @enum Category
     * @brief Menu item categories for organization and display
     */
    enum Category { 
        APPETIZER,      ///< Appetizers and starters
        MAIN_COURSE,    ///< Main dishes and entrees  
        DESSERT,        ///< Desserts and sweets
        BEVERAGE,       ///< Drinks and beverages
        SPECIAL         ///< Daily specials and limited items
    };
    
    /**
     * @brief Constructs a new MenuItem
     * @param id Unique identifier for the menu item
     * @param name Display name of the menu item
     * @param price Base price of the menu item
     * @param category Category classification of the item
     */
    MenuItem(int id, const std::string& name, double price, Category category);
    
    // Getters
    /**
     * @brief Gets the unique ID of the menu item
     * @return The menu item ID
     */
    int getId() const { return id_; }
    
    /**
     * @brief Gets the display name of the menu item
     * @return The menu item name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Gets the current price of the menu item
     * @return The menu item price
     */
    double getPrice() const { return price_; }
    
    /**
     * @brief Gets the category of the menu item
     * @return The menu item category
     */
    Category getCategory() const { return category_; }
    
    /**
     * @brief Checks if the menu item is currently available
     * @return True if available, false otherwise
     */
    bool isAvailable() const { return available_; }
    
    // Setters for dynamic pricing and availability
    /**
     * @brief Updates the price of the menu item
     * @param price New price for the menu item
     */
    void setPrice(double price) { price_ = price; }
    
    /**
     * @brief Sets the availability status of the menu item
     * @param available True to make available, false to mark unavailable
     */
    void setAvailable(bool available) { available_ = available; }
    
    /**
     * @brief Converts the menu item to JSON format for API communication
     * @return JSON object representation of the menu item
     */
    Wt::Json::Object toJson() const;
    
    /**
     * @brief Gets the string representation of a category
     * @param category The category to convert
     * @return String representation of the category
     */
    static std::string categoryToString(Category category);

private:
    int id_;                ///< Unique identifier
    std::string name_;      ///< Display name
    double price_;          ///< Current price
    Category category_;     ///< Menu category
    bool available_;        ///< Availability status
};

#endif // MENUITEM_H

