//=============================================================================
// IMPLEMENTATION FILE: ConfigurationManager.cpp
//=============================================================================

#include "../../include/core/ConfigurationManager.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

ConfigurationManager::ConfigurationManager() {
}

void ConfigurationManager::initialize() {
    std::cout << "Initializing ConfigurationManager..." << std::endl;
    
    // Load default configuration first
    loadDefaults();
    
    // Try to load from configuration file
    if (!loadFromFile("pos_config.json")) {
        std::cout << "No configuration file found, using defaults" << std::endl;
    }
    
    // Load from environment variables (these override file settings)
    loadFromEnvironment("POS_");
    
    std::cout << "✓ ConfigurationManager initialized" << std::endl;
}

bool ConfigurationManager::loadFromFile(const std::string& filePath) {
    std::cout << "Loading configuration from file: " << filePath << std::endl;
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "ConfigurationManager: Could not open file: " << filePath << std::endl;
        return false;
    }
    
    try {
        std::string line;
        std::string section = "general";
        
        while (std::getline(file, line)) {
            // Remove whitespace
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // Check for section headers [section_name]
            if (line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Parse key=value pairs
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);
                
                // Remove quotes if present
                if (value.length() >= 2 && value[0] == '"' && value.back() == '"') {
                    value = value.substr(1, value.length() - 2);
                }
                
                // Store in appropriate section
                auto& configSection = getOrCreateSection(section);
                
                // Try to convert to appropriate type
                if (value == "true" || value == "false") {
                    configSection[key] = (value == "true");
                } else if (value.find('.') != std::string::npos) {
                    try {
                        configSection[key] = std::stod(value);
                    } catch (...) {
                        configSection[key] = value; // Store as string if conversion fails
                    }
                } else {
                    try {
                        configSection[key] = std::stoi(value);
                    } catch (...) {
                        configSection[key] = value; // Store as string if conversion fails
                    }
                }
            }
        }
        
        lastLoadedFile_ = filePath;
        std::cout << "✓ Configuration loaded from file: " << filePath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ConfigurationManager: Error loading file: " << e.what() << std::endl;
        return false;
    }
}

void ConfigurationManager::loadFromEnvironment(const std::string& prefix) {
    std::cout << "Loading configuration from environment variables with prefix: " << prefix << std::endl;
    
    // Get all environment variables
    extern char **environ;
    int count = 0;
    
    for (char **env = environ; *env; ++env) {
        std::string envVar(*env);
        
        // Check if it starts with our prefix
        if (envVar.substr(0, prefix.length()) == prefix) {
            size_t equalPos = envVar.find('=');
            if (equalPos != std::string::npos) {
                std::string key = envVar.substr(prefix.length(), equalPos - prefix.length());
                std::string value = envVar.substr(equalPos + 1);
                
                // Convert underscores to dots and lowercase
                std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                std::replace(key.begin(), key.end(), '_', '.');
                
                // Parse section and key
                auto [section, keyName] = parseKey(key);
                if (section.empty()) section = "general";
                
                auto& configSection = getOrCreateSection(section);
                
                // Store as string (environment variables are always strings)
                // Type conversion will happen when getValue is called
                configSection[keyName] = value;
                count++;
            }
        }
    }
    
    std::cout << "✓ Loaded " << count << " configuration values from environment" << std::endl;
}

void ConfigurationManager::loadDefaults() {
    std::cout << "Loading default configuration..." << std::endl;
    
    setDefaultRestaurantConfig();
    setDefaultServerConfig();
    setDefaultOrderConfig();
    setDefaultKitchenConfig();
    setDefaultUIConfig();
    setDefaultFeatureFlags();
    setDefaultPaymentConfig();
    
    std::cout << "✓ Default configuration loaded" << std::endl;
}

void ConfigurationManager::setDefaultRestaurantConfig() {
    auto& section = getOrCreateSection("restaurant");
    section["name"] = std::string("Restaurant POS System");
    section["address"] = std::string("123 Main Street, City, State 12345");
    section["phone"] = std::string("(555) 123-4567");
    section["tax.rate"] = 0.0825; // 8.25%
}

void ConfigurationManager::setDefaultServerConfig() {
    auto& section = getOrCreateSection("server");
    section["port"] = 8081;
    section["address"] = std::string("0.0.0.0");
    section["session.timeout"] = 3600; // 1 hour
}

void ConfigurationManager::setDefaultOrderConfig() {
    auto& section = getOrCreateSection("order");
    section["starting.id"] = 1001;
    section["timeout"] = 30; // 30 minutes
    section["max.items"] = 50;
}

void ConfigurationManager::setDefaultKitchenConfig() {
    auto& section = getOrCreateSection("kitchen");
    section["refresh.rate"] = 5; // 5 seconds
    section["busy.threshold"] = 10; // 10 orders
    section["prep.time.appetizer"] = 10; // 10 minutes
    section["prep.time.main"] = 20; // 20 minutes
    section["prep.time.dessert"] = 8; // 8 minutes
    section["prep.time.beverage"] = 3; // 3 minutes
}

void ConfigurationManager::setDefaultUIConfig() {
    auto& section = getOrCreateSection("ui");
    section["default.theme"] = std::string("bootstrap");
    section["update.interval"] = 5; // 5 seconds
    section["group.menu.by.category"] = true;
}

void ConfigurationManager::setDefaultFeatureFlags() {
    auto& section = getOrCreateSection("features");
    section["inventory"] = false;
    section["staff.management"] = false;
    section["customer.management"] = false;
    section["reporting"] = true;
    section["loyalty.program"] = false;
}

void ConfigurationManager::setDefaultPaymentConfig() {
    auto& section = getOrCreateSection("payment");
    section["enabled.methods"] = std::string("cash,credit_card,debit_card");
    section["tip.suggestions"] = std::string("15,18,20,25");
}

// Generic configuration access
bool ConfigurationManager::hasKey(const std::string& key) const {
    auto [sectionName, keyName] = parseKey(key);
    
    const auto* section = getSection(sectionName);
    if (!section) {
        return false;
    }
    
    return section->find(keyName) != section->end();
}

bool ConfigurationManager::removeKey(const std::string& key) {
    auto [sectionName, keyName] = parseKey(key);
    
    auto* section = getSection(sectionName);
    if (!section) {
        return false;
    }
    
    auto it = section->find(keyName);
    if (it != section->end()) {
        section->erase(it);
        return true;
    }
    
    return false;
}

std::vector<std::string> ConfigurationManager::getSectionKeys(const std::string& sectionName) const {
    std::vector<std::string> keys;
    
    const auto* section = getSection(sectionName);
    if (!section) {
        return keys;
    }
    
    for (const auto& pair : *section) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

// Restaurant configuration
std::string ConfigurationManager::getRestaurantName() const {
    return getValue<std::string>("restaurant.name", "Restaurant POS System");
}

void ConfigurationManager::setRestaurantName(const std::string& name) {
    setValue("restaurant.name", name);
}

std::string ConfigurationManager::getRestaurantAddress() const {
    return getValue<std::string>("restaurant.address", "");
}

void ConfigurationManager::setRestaurantAddress(const std::string& address) {
    setValue("restaurant.address", address);
}

std::string ConfigurationManager::getRestaurantPhone() const {
    return getValue<std::string>("restaurant.phone", "");
}

void ConfigurationManager::setRestaurantPhone(const std::string& phone) {
    setValue("restaurant.phone", phone);
}

double ConfigurationManager::getTaxRate() const {
    return getValue<double>("restaurant.tax.rate", 0.0825);
}

void ConfigurationManager::setTaxRate(double rate) {
    setValue("restaurant.tax.rate", rate);
}

// Server configuration
int ConfigurationManager::getServerPort() const {
    return getValue<int>("server.port", 8081);
}

void ConfigurationManager::setServerPort(int port) {
    setValue("server.port", port);
}

std::string ConfigurationManager::getServerAddress() const {
    return getValue<std::string>("server.address", "0.0.0.0");
}

void ConfigurationManager::setServerAddress(const std::string& address) {
    setValue("server.address", address);
}

int ConfigurationManager::getSessionTimeout() const {
    return getValue<int>("server.session.timeout", 3600);
}

void ConfigurationManager::setSessionTimeout(int timeoutSeconds) {
    setValue("server.session.timeout", timeoutSeconds);
}

// Order configuration
int ConfigurationManager::getStartingOrderId() const {
    return getValue<int>("order.starting.id", 1001);
}

void ConfigurationManager::setStartingOrderId(int startId) {
    setValue("order.starting.id", startId);
}

int ConfigurationManager::getOrderTimeout() const {
    return getValue<int>("order.timeout", 30);
}

void ConfigurationManager::setOrderTimeout(int timeoutMinutes) {
    setValue("order.timeout", timeoutMinutes);
}

int ConfigurationManager::getMaxItemsPerOrder() const {
    return getValue<int>("order.max.items", 50);
}

void ConfigurationManager::setMaxItemsPerOrder(int maxItems) {
    setValue("order.max.items", maxItems);
}

// Kitchen configuration
int ConfigurationManager::getKitchenRefreshRate() const {
    return getValue<int>("kitchen.refresh.rate", 5);
}

void ConfigurationManager::setKitchenRefreshRate(int rateSeconds) {
    setValue("kitchen.refresh.rate", rateSeconds);
}

int ConfigurationManager::getKitchenBusyThreshold() const {
    return getValue<int>("kitchen.busy.threshold", 10);
}

void ConfigurationManager::setKitchenBusyThreshold(int threshold) {
    setValue("kitchen.busy.threshold", threshold);
}

std::unordered_map<std::string, int> ConfigurationManager::getKitchenPrepTimes() const {
    std::unordered_map<std::string, int> prepTimes;
    
    prepTimes["appetizer"] = getValue<int>("kitchen.prep.time.appetizer", 10);
    prepTimes["main"] = getValue<int>("kitchen.prep.time.main", 20);
    prepTimes["dessert"] = getValue<int>("kitchen.prep.time.dessert", 8);
    prepTimes["beverage"] = getValue<int>("kitchen.prep.time.beverage", 3);
    
    return prepTimes;
}

void ConfigurationManager::setKitchenPrepTime(const std::string& category, int minutes) {
    setValue("kitchen.prep.time." + category, minutes);
}

// UI configuration
std::string ConfigurationManager::getDefaultTheme() const {
    return getValue<std::string>("ui.default.theme", "bootstrap");
}

void ConfigurationManager::setDefaultTheme(const std::string& themeId) {
    setValue("ui.default.theme", themeId);
}

int ConfigurationManager::getUIUpdateInterval() const {
    return getValue<int>("ui.update.interval", 5);
}

void ConfigurationManager::setUIUpdateInterval(int intervalSeconds) {
    setValue("ui.update.interval", intervalSeconds);
}

bool ConfigurationManager::getGroupMenuByCategory() const {
    return getValue<bool>("ui.group.menu.by.category", true);
}

void ConfigurationManager::setGroupMenuByCategory(bool group) {
    setValue("ui.group.menu.by.category", group);
}

// Feature flags
bool ConfigurationManager::isFeatureEnabled(const std::string& featureName) const {
    return getValue<bool>("features." + featureName, false);
}

void ConfigurationManager::setFeatureEnabled(const std::string& featureName, bool enabled) {
    setValue("features." + featureName, enabled);
}

bool ConfigurationManager::isInventoryEnabled() const {
    return isFeatureEnabled("inventory");
}

bool ConfigurationManager::isStaffManagementEnabled() const {
    return isFeatureEnabled("staff.management");
}

bool ConfigurationManager::isCustomerManagementEnabled() const {
    return isFeatureEnabled("customer.management");
}

bool ConfigurationManager::isReportingEnabled() const {
    return isFeatureEnabled("reporting");
}

bool ConfigurationManager::isLoyaltyProgramEnabled() const {
    return isFeatureEnabled("loyalty.program");
}

// Payment configuration
std::vector<std::string> ConfigurationManager::getEnabledPaymentMethods() const {
    std::string methodsStr = getValue<std::string>("payment.enabled.methods", "cash,credit_card,debit_card");
    std::vector<std::string> methods;
    
    std::stringstream ss(methodsStr);
    std::string method;
    while (std::getline(ss, method, ',')) {
        // Trim whitespace
        method.erase(std::remove_if(method.begin(), method.end(), ::isspace), method.end());
        if (!method.empty()) {
            methods.push_back(method);
        }
    }
    
    return methods;
}

void ConfigurationManager::setPaymentMethodEnabled(const std::string& method, bool enabled) {
    auto methods = getEnabledPaymentMethods();
    
    auto it = std::find(methods.begin(), methods.end(), method);
    
    if (enabled && it == methods.end()) {
        methods.push_back(method);
    } else if (!enabled && it != methods.end()) {
        methods.erase(it);
    }
    
    // Convert back to string
    std::string methodsStr;
    for (size_t i = 0; i < methods.size(); ++i) {
        if (i > 0) methodsStr += ",";
        methodsStr += methods[i];
    }
    
    setValue("payment.enabled.methods", methodsStr);
}

std::vector<double> ConfigurationManager::getTipSuggestions() const {
    std::string suggestionsStr = getValue<std::string>("payment.tip.suggestions", "15,18,20,25");
    std::vector<double> suggestions;
    
    std::stringstream ss(suggestionsStr);
    std::string suggestion;
    while (std::getline(ss, suggestion, ',')) {
        try {
            double value = std::stod(suggestion);
            suggestions.push_back(value);
        } catch (...) {
            // Skip invalid values
        }
    }
    
    return suggestions;
}

void ConfigurationManager::setTipSuggestions(const std::vector<double>& suggestions) {
    std::string suggestionsStr;
    for (size_t i = 0; i < suggestions.size(); ++i) {
        if (i > 0) suggestionsStr += ",";
        suggestionsStr += std::to_string(suggestions[i]);
    }
    
    setValue("payment.tip.suggestions", suggestionsStr);
}

// Persistence
bool ConfigurationManager::saveToFile(const std::string& filePath) const {
    std::cout << "Saving configuration to file: " << filePath << std::endl;
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "ConfigurationManager: Could not open file for writing: " << filePath << std::endl;
        return false;
    }
    
    try {
        file << "# Restaurant POS Configuration File\n";
        file << "# Generated automatically - edit with care\n\n";
        
        for (const auto& sectionPair : config_) {
            file << "[" << sectionPair.first << "]\n";
            
            for (const auto& keyPair : sectionPair.second) {
                file << keyPair.first << "=";
                
                // Handle different types
                try {
                    auto strValue = std::any_cast<std::string>(keyPair.second);
                    file << "\"" << strValue << "\"";
                } catch (const std::bad_any_cast&) {
                    try {
                        auto intValue = std::any_cast<int>(keyPair.second);
                        file << intValue;
                    } catch (const std::bad_any_cast&) {
                        try {
                            auto doubleValue = std::any_cast<double>(keyPair.second);
                            file << doubleValue;
                        } catch (const std::bad_any_cast&) {
                            try {
                                auto boolValue = std::any_cast<bool>(keyPair.second);
                                file << (boolValue ? "true" : "false");
                            } catch (const std::bad_any_cast&) {
                                file << "\"unknown\"";
                            }
                        }
                    }
                }
                
                file << "\n";
            }
            
            file << "\n";
        }
        
        std::cout << "✓ Configuration saved to: " << filePath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ConfigurationManager: Error saving file: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigurationManager::reload() {
    if (lastLoadedFile_.empty()) {
        std::cerr << "ConfigurationManager: No file to reload" << std::endl;
        return false;
    }
    
    std::cout << "Reloading configuration from: " << lastLoadedFile_ << std::endl;
    
    // Clear current configuration
    config_.clear();
    
    // Reload defaults and file
    loadDefaults();
    bool success = loadFromFile(lastLoadedFile_);
    loadFromEnvironment("POS_");
    
    return success;
}

// Helper methods
std::pair<std::string, std::string> ConfigurationManager::parseKey(const std::string& key) const {
    size_t dotPos = key.find('.');
    if (dotPos == std::string::npos) {
        return {"general", key};
    }
    
    return {key.substr(0, dotPos), key.substr(dotPos + 1)};
}

ConfigurationManager::ConfigSection* ConfigurationManager::getSection(const std::string& sectionName) {
    auto it = config_.find(sectionName);
    return (it != config_.end()) ? &it->second : nullptr;
}

const ConfigurationManager::ConfigSection* ConfigurationManager::getSection(const std::string& sectionName) const {
    auto it = config_.find(sectionName);
    return (it != config_.end()) ? &it->second : nullptr;
}

ConfigurationManager::ConfigSection& ConfigurationManager::getOrCreateSection(const std::string& sectionName) {
    return config_[sectionName];
}

bool ConfigurationManager::isValidKey(const std::string& key) const {
    return !key.empty() && key.find('=') == std::string::npos;
}

// Type conversion helper
template<typename T>
T ConfigurationManager::convertValue(const ConfigValue& value, const T& defaultValue) const {
    try {
        return std::any_cast<T>(value);
    } catch (const std::bad_any_cast&) {
        // Try string conversion for basic types
        try {
            auto strValue = std::any_cast<std::string>(value);
            
            if constexpr (std::is_same_v<T, int>) {
                return std::stoi(strValue);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::stod(strValue);
            } else if constexpr (std::is_same_v<T, bool>) {
                return (strValue == "true" || strValue == "1" || strValue == "yes");
            } else if constexpr (std::is_same_v<T, std::string>) {
                return strValue;
            }
        } catch (...) {
            // Fall through to default
        }
        
        return defaultValue;
    }
}

// Explicit template instantiations for common types
template int ConfigurationManager::convertValue<int>(const ConfigValue&, const int&) const;
template double ConfigurationManager::convertValue<double>(const ConfigValue&, const double&) const;
template bool ConfigurationManager::convertValue<bool>(const ConfigValue&, const bool&) const;
template std::string ConfigurationManager::convertValue<std::string>(const ConfigValue&, const std::string&) const;