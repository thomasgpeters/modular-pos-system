#include "../../include/core/ConfigurationManager.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <regex>
#include <iomanip>

ConfigurationManager::ConfigurationManager() {
    // Constructor - initialization happens in initialize()
}

void ConfigurationManager::initialize() {
    std::cout << "Initializing ConfigurationManager..." << std::endl;
    
    // Load default configuration first
    loadDefaults();
    
    // Try to load from configuration file
    std::vector<std::string> configFiles = {
        "config/pos_config.json",
        "pos_config.json",
        "../config/pos_config.json"
    };
    
    bool configLoaded = false;
    for (const auto& configFile : configFiles) {
        if (loadFromFile(configFile)) {
            std::cout << "✓ Configuration loaded from: " << configFile << std::endl;
            configLoaded = true;
            break;
        }
    }
    
    if (!configLoaded) {
        std::cout << "No configuration file found, using defaults" << std::endl;
    }
    
    // Load environment variables (these override file settings)
    loadFromEnvironment();
    
    std::cout << "✓ ConfigurationManager initialized" << std::endl;
}

bool ConfigurationManager::loadFromFile(const std::string& filePath) {
    std::ifstream configFile(filePath);
    if (!configFile.is_open()) {
        return false;
    }
    
    lastLoadedFile_ = filePath;
    
    // For now, we'll implement a simple key=value parser
    // In a real implementation, you'd use a JSON/XML/YAML parser
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(configFile, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Parse key=value format
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            std::cerr << "Warning: Invalid configuration line " << lineNumber 
                      << " in " << filePath << ": " << line << std::endl;
            continue;
        }
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Remove quotes from value if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.length() - 2);
        }
        
        if (isValidKey(key)) {
            setValue(key, value);
        } else {
            std::cerr << "Warning: Invalid configuration key: " << key << std::endl;
        }
    }
    
    configFile.close();
    std::cout << "Configuration loaded from file: " << filePath << std::endl;
    return true;
}

void ConfigurationManager::loadFromEnvironment(const std::string& prefix) {
    std::cout << "Loading configuration from environment variables (prefix: " 
              << prefix << ")..." << std::endl;
    
    // List of environment variables to check
    std::vector<std::string> envVars = {
        prefix + "RESTAURANT_NAME",
        prefix + "RESTAURANT_ADDRESS",
        prefix + "RESTAURANT_PHONE",
        prefix + "TAX_RATE",
        prefix + "SERVER_PORT",
        prefix + "SERVER_ADDRESS",
        prefix + "DEFAULT_THEME",
        prefix + "UI_UPDATE_INTERVAL",
        prefix + "KITCHEN_REFRESH_RATE"
    };
    
    int loadedCount = 0;
    for (const auto& envVar : envVars) {
        const char* value = std::getenv(envVar.c_str());
        if (value) {
            // Convert environment variable name to configuration key
            std::string key = envVar.substr(prefix.length());
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            std::replace(key.begin(), key.end(), '_', '.');
            
            setValue(key, std::string(value));
            loadedCount++;
        }
    }
    
    if (loadedCount > 0) {
        std::cout << "✓ Loaded " << loadedCount << " configuration values from environment" << std::endl;
    }
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
    auto& restaurantConfig = getOrCreateSection("restaurant");
    
    restaurantConfig["name"] = std::string("Demo Restaurant");
    restaurantConfig["address"] = std::string("123 Main Street, Pittsburgh, PA 15213");
    restaurantConfig["phone"] = std::string("(412) 555-0123");
    restaurantConfig["tax.rate"] = 0.0825; // 8.25% tax rate
}

void ConfigurationManager::setDefaultServerConfig() {
    auto& serverConfig = getOrCreateSection("server");
    
    serverConfig["port"] = 8081;
    serverConfig["address"] = std::string("0.0.0.0");
    serverConfig["session.timeout"] = 1800; // 30 minutes
}

void ConfigurationManager::setDefaultOrderConfig() {
    auto& orderConfig = getOrCreateSection("order");
    
    orderConfig["starting.id"] = 1000;
    orderConfig["timeout"] = 60; // 60 minutes
    orderConfig["max.items"] = 50;
}

void ConfigurationManager::setDefaultKitchenConfig() {
    auto& kitchenConfig = getOrCreateSection("kitchen");
    
    kitchenConfig["refresh.rate"] = 5; // 5 seconds
    kitchenConfig["busy.threshold"] = 10; // 10 orders
    
    // Default preparation times by category (in minutes)
    kitchenConfig["prep.time.appetizer"] = 10;
    kitchenConfig["prep.time.main_course"] = 20;
    kitchenConfig["prep.time.dessert"] = 8;
    kitchenConfig["prep.time.beverage"] = 2;
}

void ConfigurationManager::setDefaultUIConfig() {
    auto& uiConfig = getOrCreateSection("ui");
    
    uiConfig["default.theme"] = std::string("bootstrap");
    uiConfig["update.interval"] = 5; // 5 seconds
    uiConfig["group.menu.by.category"] = true;
}

void ConfigurationManager::setDefaultFeatureFlags() {
    auto& featuresConfig = getOrCreateSection("features");
    
    featuresConfig["inventory"] = false;
    featuresConfig["staff.management"] = false;
    featuresConfig["customer.management"] = false;
    featuresConfig["reporting"] = true;
    featuresConfig["loyalty.program"] = false;
}

void ConfigurationManager::setDefaultPaymentConfig() {
    auto& paymentConfig = getOrCreateSection("payment");
    
    paymentConfig["enabled.methods"] = std::string("cash,credit_card,debit_card");
    paymentConfig["tip.suggestions"] = std::string("15,18,20,25");
}

// =================================================================
// Configuration Access
// =================================================================

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
    const auto* section = getSection(sectionName);
    if (!section) {
        return {};
    }
    
    std::vector<std::string> keys;
    for (const auto& pair : *section) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

// =================================================================
// Restaurant Configuration
// =================================================================

std::string ConfigurationManager::getRestaurantName() const {
    return getValue<std::string>("restaurant.name", "Demo Restaurant");
}

void ConfigurationManager::setRestaurantName(const std::string& name) {
    setValue("restaurant.name", name);
}

std::string ConfigurationManager::getRestaurantAddress() const {
    return getValue<std::string>("restaurant.address", "123 Main Street, Pittsburgh, PA 15213");
}

void ConfigurationManager::setRestaurantAddress(const std::string& address) {
    setValue("restaurant.address", address);
}

std::string ConfigurationManager::getRestaurantPhone() const {
    return getValue<std::string>("restaurant.phone", "(412) 555-0123");
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

// =================================================================
// Server Configuration
// =================================================================

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
    return getValue<int>("server.session.timeout", 1800);
}

void ConfigurationManager::setSessionTimeout(int timeoutSeconds) {
    setValue("server.session.timeout", timeoutSeconds);
}

// =================================================================
// Order Configuration
// =================================================================

int ConfigurationManager::getStartingOrderId() const {
    return getValue<int>("order.starting.id", 1000);
}

void ConfigurationManager::setStartingOrderId(int startId) {
    setValue("order.starting.id", startId);
}

int ConfigurationManager::getOrderTimeout() const {
    return getValue<int>("order.timeout", 60);
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

// =================================================================
// Kitchen Configuration
// =================================================================

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
    prepTimes["main_course"] = getValue<int>("kitchen.prep.time.main_course", 20);
    prepTimes["dessert"] = getValue<int>("kitchen.prep.time.dessert", 8);
    prepTimes["beverage"] = getValue<int>("kitchen.prep.time.beverage", 2);
    
    return prepTimes;
}

void ConfigurationManager::setKitchenPrepTime(const std::string& category, int minutes) {
    setValue("kitchen.prep.time." + category, minutes);
}

// =================================================================
// UI Configuration
// =================================================================

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

// =================================================================
// Feature Flags
// =================================================================

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

// =================================================================
// Payment Configuration
// =================================================================

std::vector<std::string> ConfigurationManager::getEnabledPaymentMethods() const {
    std::string methodsStr = getValue<std::string>("payment.enabled.methods", "cash,credit_card,debit_card");
    
    std::vector<std::string> methods;
    std::stringstream ss(methodsStr);
    std::string method;
    
    while (std::getline(ss, method, ',')) {
        // Trim whitespace
        method.erase(0, method.find_first_not_of(" \t"));
        method.erase(method.find_last_not_of(" \t") + 1);
        
        if (!method.empty()) {
            methods.push_back(method);
        }
    }
    
    return methods;
}

void ConfigurationManager::setPaymentMethodEnabled(const std::string& method, bool enabled) {
    auto currentMethods = getEnabledPaymentMethods();
    
    auto it = std::find(currentMethods.begin(), currentMethods.end(), method);
    
    if (enabled && it == currentMethods.end()) {
        // Add method if not present
        currentMethods.push_back(method);
    } else if (!enabled && it != currentMethods.end()) {
        // Remove method if present
        currentMethods.erase(it);
    }
    
    // Convert back to string
    std::stringstream ss;
    for (size_t i = 0; i < currentMethods.size(); ++i) {
        if (i > 0) ss << ",";
        ss << currentMethods[i];
    }
    
    setValue("payment.enabled.methods", ss.str());
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
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid tip suggestion value: " << suggestion << std::endl;
        }
    }
    
    return suggestions;
}

void ConfigurationManager::setTipSuggestions(const std::vector<double>& suggestions) {
    std::stringstream ss;
    for (size_t i = 0; i < suggestions.size(); ++i) {
        if (i > 0) ss << ",";
        ss << std::fixed << std::setprecision(1) << suggestions[i];
    }
    
    setValue("payment.tip.suggestions", ss.str());
}

// =================================================================
// Persistence
// =================================================================

bool ConfigurationManager::saveToFile(const std::string& filePath) const {
    std::ofstream configFile(filePath);
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open configuration file for writing: " << filePath << std::endl;
        return false;
    }
    
    configFile << "# Restaurant POS Configuration File\n";
    configFile << "# Generated automatically - edit with care\n\n";
    
    for (const auto& sectionPair : config_) {
        const std::string& sectionName = sectionPair.first;
        const ConfigSection& section = sectionPair.second;
        
        configFile << "# " << sectionName << " configuration\n";
        
        for (const auto& keyPair : section) {
            const std::string& keyName = keyPair.first;
            const ConfigValue& value = keyPair.second;
            
            std::string fullKey = sectionName + "." + keyName;
            
            // Convert value to string for output
            try {
                if (value.type() == typeid(std::string)) {
                    configFile << fullKey << "=" << std::any_cast<std::string>(value) << "\n";
                } else if (value.type() == typeid(int)) {
                    configFile << fullKey << "=" << std::any_cast<int>(value) << "\n";
                } else if (value.type() == typeid(double)) {
                    configFile << fullKey << "=" << std::fixed << std::setprecision(4) 
                              << std::any_cast<double>(value) << "\n";
                } else if (value.type() == typeid(bool)) {
                    configFile << fullKey << "=" << (std::any_cast<bool>(value) ? "true" : "false") << "\n";
                } else {
                    std::cerr << "Warning: Unknown type for configuration key: " << fullKey << std::endl;
                }
            } catch (const std::bad_any_cast& e) {
                std::cerr << "Error converting configuration value for key: " << fullKey << std::endl;
            }
        }
        
        configFile << "\n";
    }
    
    configFile.close();
    std::cout << "Configuration saved to: " << filePath << std::endl;
    return true;
}

bool ConfigurationManager::reload() {
    if (lastLoadedFile_.empty()) {
        std::cerr << "No configuration file to reload" << std::endl;
        return false;
    }
    
    std::cout << "Reloading configuration from: " << lastLoadedFile_ << std::endl;
    
    // Clear current configuration
    config_.clear();
    
    // Reload defaults first
    loadDefaults();
    
    // Reload from file
    return loadFromFile(lastLoadedFile_);
}

// =================================================================
// Helper Methods
// =================================================================

std::pair<std::string, std::string> ConfigurationManager::parseKey(const std::string& key) const {
    size_t dotPos = key.find('.');
    
    if (dotPos == std::string::npos) {
        // No section specified, use "general" as default
        return std::make_pair("general", key);
    }
    
    std::string sectionName = key.substr(0, dotPos);
    std::string keyName = key.substr(dotPos + 1);
    
    return std::make_pair(sectionName, keyName);
}

ConfigurationManager::ConfigSection* ConfigurationManager::getSection(const std::string& sectionName) {
    auto it = config_.find(sectionName);
    return (it != config_.end()) ? &(it->second) : nullptr;
}

const ConfigurationManager::ConfigSection* ConfigurationManager::getSection(const std::string& sectionName) const {
    auto it = config_.find(sectionName);
    return (it != config_.end()) ? &(it->second) : nullptr;
}

ConfigurationManager::ConfigSection& ConfigurationManager::getOrCreateSection(const std::string& sectionName) {
    return config_[sectionName];
}

bool ConfigurationManager::isValidKey(const std::string& key) const {
    // Basic validation - key should not be empty and should contain only valid characters
    if (key.empty()) {
        return false;
    }
    
    // Check for valid characters (alphanumeric, dots, underscores, hyphens)
    std::regex validKeyRegex("^[a-zA-Z][a-zA-Z0-9._-]*$");
    return std::regex_match(key, validKeyRegex);
}

// Template specializations for type conversion
template<>
std::string ConfigurationManager::convertValue<std::string>(const ConfigValue& value, const std::string& defaultValue) const {
    try {
        if (value.type() == typeid(std::string)) {
            return std::any_cast<std::string>(value);
        } else if (value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(value));
        } else if (value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(value));
        } else if (value.type() == typeid(bool)) {
            return std::any_cast<bool>(value) ? "true" : "false";
        }
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error converting configuration value to string" << std::endl;
    }
    
    return defaultValue;
}

template<>
int ConfigurationManager::convertValue<int>(const ConfigValue& value, const int& defaultValue) const {
    try {
        if (value.type() == typeid(int)) {
            return std::any_cast<int>(value);
        } else if (value.type() == typeid(std::string)) {
            return std::stoi(std::any_cast<std::string>(value));
        } else if (value.type() == typeid(double)) {
            return static_cast<int>(std::any_cast<double>(value));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error converting configuration value to int: " << e.what() << std::endl;
    }
    
    return defaultValue;
}

template<>
double ConfigurationManager::convertValue<double>(const ConfigValue& value, const double& defaultValue) const {
    try {
        if (value.type() == typeid(double)) {
            return std::any_cast<double>(value);
        } else if (value.type() == typeid(std::string)) {
            return std::stod(std::any_cast<std::string>(value));
        } else if (value.type() == typeid(int)) {
            return static_cast<double>(std::any_cast<int>(value));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error converting configuration value to double: " << e.what() << std::endl;
    }
    
    return defaultValue;
}

template<>
bool ConfigurationManager::convertValue<bool>(const ConfigValue& value, const bool& defaultValue) const {
    try {
        if (value.type() == typeid(bool)) {
            return std::any_cast<bool>(value);
        } else if (value.type() == typeid(std::string)) {
            std::string strValue = std::any_cast<std::string>(value);
            std::transform(strValue.begin(), strValue.end(), strValue.begin(), ::tolower);
            return strValue == "true" || strValue == "yes" || strValue == "1" || strValue == "on";
        } else if (value.type() == typeid(int)) {
            return std::any_cast<int>(value) != 0;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error converting configuration value to bool: " << e.what() << std::endl;
    }
    
    return defaultValue;
}
