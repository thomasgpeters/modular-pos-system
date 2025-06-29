#include "../include/MenuItem.hpp"

/**
 * @file MenuItem.cpp
 * @brief Implementation of the MenuItem class
 */

MenuItem::MenuItem(int id, const std::string& name, double price, Category category)
    : id_(id), name_(name), price_(price), category_(category), available_(true) {
}

Wt::Json::Object MenuItem::toJson() const {
    Wt::Json::Object obj;
    obj["id"] = Wt::Json::Value(id_);
    obj["name"] = Wt::Json::Value(name_);
    obj["price"] = Wt::Json::Value(price_);
    obj["category"] = Wt::Json::Value(static_cast<int>(category_));
    obj["categoryName"] = Wt::Json::Value(categoryToString(category_));
    obj["available"] = Wt::Json::Value(available_);
    return obj;
}

std::string MenuItem::categoryToString(Category category) {
    switch (category) {
        case APPETIZER:     return "Appetizer";
        case MAIN_COURSE:   return "Main Course";
        case DESSERT:       return "Dessert";
        case BEVERAGE:      return "Beverage";
        case SPECIAL:       return "Special";
        default:            return "Unknown";
    }
}
