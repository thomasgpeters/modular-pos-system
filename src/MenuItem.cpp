#include "../include/MenuItem.hpp"

MenuItem::MenuItem(int id, const std::string& name, double price, Category category)
    : id_(id), name_(name), price_(price), category_(category), available_(true) {}

Wt::Json::Object MenuItem::toJson() const {
    Wt::Json::Object json;
    json["id"] = Wt::Json::Value(id_);
    json["name"] = Wt::Json::Value(name_);
    json["price"] = Wt::Json::Value(price_);
    json["category"] = Wt::Json::Value(static_cast<int>(category_));
    json["categoryName"] = Wt::Json::Value(categoryToString(category_));
    json["available"] = Wt::Json::Value(available_);
    return json;
}

std::string MenuItem::categoryToString(Category category) {
    switch (category) {
        case APPETIZER:   return "Appetizer";
        case MAIN_COURSE: return "Main Course";
        case DESSERT:     return "Dessert";
        case BEVERAGE:    return "Beverage";
        case SPECIAL:     return "Special";
        default:          return "Unknown";
    }
}
