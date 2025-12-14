#include "../../include/core/ConfigurationManager.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>

ConfigurationManager::ConfigurationManager()
    : lastLoadedFile_("") {
    
    // Initialize with default values
    loadDefaults();
}

void ConfigurationManager::initialize() {
    std::cout << "[ConfigurationManager] Initializing with default configuration" << std::endl;
    
    // Load defaults if not already loaded
    if (config_.empty()) {
        loadDefaults();
    }
    
    std::cout << "[ConfigurationManager] Initialized successfully" << std::endl;
}

void ConfigurationManager::loadDefaults() {
    // Restaurant configuration
    setDefaultRestaurantConfig();
    
    // Server configuration
    setDefaultServerConfig();
    
    // Order configuration
    setDefaultOrderConfig();
    
    // Kitchen configuration
    setDefaultKitchenConfig();
    
    // UI configuration
    setDefaultUIConfig();
    
    // Feature flags
    setDefaultFeatureFlags();
    
    // Payment configuration
    setDefaultPaymentConfig();
    
    // NEW: API configuration
    setDefaultAPIConfig();

    // LLM configuration
    setDefaultLLMConfig();

    std::cout << "[ConfigurationManager] Default configuration loaded with API and LLM settings" << std::endl;
}

// NEW: Set default API configuration
void ConfigurationManager::setDefaultAPIConfig() {
    auto& api = getOrCreateSection("api");
    
    // Basic API settings
    api["enabled"] = false;  // Disabled by default for safety
    api["base_url"] = std::string("http://localhost:5656/api");
    api["auth_token"] = std::string("");  // Empty by default
    api["timeout"] = 30;  // 30 seconds
    api["enable_caching"] = true;
    api["debug_mode"] = false;
    
    // Retry settings
    api["max_retries"] = 3;
    api["retry_delay_ms"] = 1000;
    
    // Cache settings
    api["cache_timeout_minutes"] = 5;
    
    std::cout << "[ConfigurationManager] API configuration defaults set" << std::endl;
}

void ConfigurationManager::setDefaultRestaurantConfig() {
    auto& restaurant = getOrCreateSection("restaurant");
    restaurant["name"] = std::string("Sample Restaurant");
    restaurant["address"] = std::string("123 Main Street, City, State 12345");
    restaurant["phone"] = std::string("(555) 123-4567");
    restaurant["tax_rate"] = 0.08;
}

void ConfigurationManager::setDefaultServerConfig() {
    auto& server = getOrCreateSection("server");
    server["port"] = 9090;
    server["address"] = std::string("0.0.0.0");
    server["session_timeout"] = 3600; // 1 hour
}

void ConfigurationManager::setDefaultOrderConfig() {
    auto& order = getOrCreateSection("order");
    order["starting_id"] = 1000;
    order["timeout"] = 30; // minutes
    order["max_items"] = 50;
}

void ConfigurationManager::setDefaultKitchenConfig() {
    auto& kitchen = getOrCreateSection("kitchen");
    kitchen["refresh_rate"] = 5; // seconds
    kitchen["busy_threshold"] = 10;
    
    // Default prep times (in minutes)
    std::unordered_map<std::string, int> prepTimes;
    prepTimes["appetizer"] = 8;
    prepTimes["main_course"] = 15;
    prepTimes["dessert"] = 5;
    prepTimes["beverage"] = 2;
    prepTimes["special"] = 20;
    kitchen["prep_times"] = prepTimes;
}

void ConfigurationManager::setDefaultUIConfig() {
    auto& ui = getOrCreateSection("ui");
    ui["default_theme"] = std::string("light");
    ui["update_interval"] = 10; // seconds
    ui["group_menu_by_category"] = true;
    ui["show_descriptions"] = true;
    ui["max_themes"] = 10;
}

void ConfigurationManager::setDefaultFeatureFlags() {
    auto& features = getOrCreateSection("features");
    features["inventory"] = false;
    features["staff_management"] = false;
    features["customer_management"] = false;
    features["reporting"] = true;
    features["loyalty_program"] = false;
}

void ConfigurationManager::setDefaultPaymentConfig() {
    auto& payment = getOrCreateSection("payment");
    
    // Enabled payment methods
    std::vector<std::string> methods = {"cash", "credit_card", "debit_card", "mobile_pay"};
    payment["enabled_methods"] = methods;
    
    // Tip suggestions (percentages)
    std::vector<double> tips = {0.15, 0.18, 0.20, 0.25};
    payment["tip_suggestions"] = tips;
}

// Generic configuration access
std::pair<std::string, std::string> ConfigurationManager::parseKey(const std::string& key) const {
    size_t dotPos = key.find('.');
    if (dotPos == std::string::npos) {
        return {"general", key};
    }
    
    return {key.substr(0, dotPos), key.substr(dotPos + 1)};
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
    if (section) {
        for (const auto& pair : *section) {
            keys.push_back(pair.first);
        }
    }
    
    return keys;
}

// Type conversion helper
template<typename T>
T ConfigurationManager::convertValue(const ConfigValue& value, const T& defaultValue) const {
    try {
        return std::any_cast<T>(value);
    } catch (const std::bad_any_cast& e) {
        std::cerr << "[ConfigurationManager] Type conversion error: " << e.what() << std::endl;
        return defaultValue;
    }
}

// Explicit template instantiations for common types
template int ConfigurationManager::convertValue<int>(const ConfigValue& value, const int& defaultValue) const;

// Restaurant configuration getters
std::string ConfigurationManager::getRestaurantName() const {
    return getValue<std::string>("restaurant.name", "Sample Restaurant");
}

void ConfigurationManager::setRestaurantName(const std::string& name) {
    setValue<std::string>("restaurant.name", name);
}

std::string ConfigurationManager::getRestaurantAddress() const {
    return getValue<std::string>("restaurant.address", "");
}

void ConfigurationManager::setRestaurantAddress(const std::string& address) {
    setValue<std::string>("restaurant.address", address);
}

std::string ConfigurationManager::getRestaurantPhone() const {
    return getValue<std::string>("restaurant.phone", "");
}

void ConfigurationManager::setRestaurantPhone(const std::string& phone) {
    setValue<std::string>("restaurant.phone", phone);
}

double ConfigurationManager::getTaxRate() const {
    return getValue<double>("restaurant.tax_rate", 0.08);
}

void ConfigurationManager::setTaxRate(double rate) {
    setValue<double>("restaurant.tax_rate", rate);
}

// Server configuration
int ConfigurationManager::getServerPort() const {
    return getValue<int>("server.port", 9090);
}

void ConfigurationManager::setServerPort(int port) {
    setValue<int>("server.port", port);
}

std::string ConfigurationManager::getServerAddress() const {
    return getValue<std::string>("server.address", "0.0.0.0");
}

void ConfigurationManager::setServerAddress(const std::string& address) {
    setValue<std::string>("server.address", address);
}

int ConfigurationManager::getSessionTimeout() const {
    return getValue<int>("server.session_timeout", 3600);
}

void ConfigurationManager::setSessionTimeout(int timeoutSeconds) {
    setValue<int>("server.session_timeout", timeoutSeconds);
}

// Order configuration
int ConfigurationManager::getStartingOrderId() const {
    return getValue<int>("order.starting_id", 1000);
}

void ConfigurationManager::setStartingOrderId(int startId) {
    setValue<int>("order.starting_id", startId);
}

int ConfigurationManager::getOrderTimeout() const {
    return getValue<int>("order.timeout", 30);
}

void ConfigurationManager::setOrderTimeout(int timeoutMinutes) {
    setValue<int>("order.timeout", timeoutMinutes);
}

int ConfigurationManager::getMaxItemsPerOrder() const {
    return getValue<int>("order.max_items", 50);
}

void ConfigurationManager::setMaxItemsPerOrder(int maxItems) {
    setValue<int>("order.max_items", maxItems);
}

// Kitchen configuration
int ConfigurationManager::getKitchenRefreshRate() const {
    return getValue<int>("kitchen.refresh_rate", 5);
}

void ConfigurationManager::setKitchenRefreshRate(int rateSeconds) {
    setValue<int>("kitchen.refresh_rate", rateSeconds);
}

int ConfigurationManager::getKitchenBusyThreshold() const {
    return getValue<int>("kitchen.busy_threshold", 10);
}

void ConfigurationManager::setKitchenBusyThreshold(int threshold) {
    setValue<int>("kitchen.busy_threshold", threshold);
}

// UI configuration
std::string ConfigurationManager::getDefaultTheme() const {
    return getValue<std::string>("ui.default_theme", "light");
}

void ConfigurationManager::setDefaultTheme(const std::string& themeId) {
    setValue<std::string>("ui.default_theme", themeId);
}

int ConfigurationManager::getUIUpdateInterval() const {
    return getValue<int>("ui.update_interval", 10);
}

void ConfigurationManager::setUIUpdateInterval(int intervalSeconds) {
    setValue<int>("ui.update_interval", intervalSeconds);
}

bool ConfigurationManager::getGroupMenuByCategory() const {
    return getValue<bool>("ui.group_menu_by_category", true);
}

void ConfigurationManager::setGroupMenuByCategory(bool group) {
    setValue<bool>("ui.group_menu_by_category", group);
}

// Feature flags
bool ConfigurationManager::isFeatureEnabled(const std::string& featureName) const {
    return getValue<bool>("features." + featureName, false);
}

void ConfigurationManager::setFeatureEnabled(const std::string& featureName, bool enabled) {
    setValue<bool>("features." + featureName, enabled);
}

bool ConfigurationManager::isInventoryEnabled() const {
    return isFeatureEnabled("inventory");
}

bool ConfigurationManager::isStaffManagementEnabled() const {
    return isFeatureEnabled("staff_management");
}

bool ConfigurationManager::isCustomerManagementEnabled() const {
    return isFeatureEnabled("customer_management");
}

bool ConfigurationManager::isReportingEnabled() const {
    return isFeatureEnabled("reporting");
}

bool ConfigurationManager::isLoyaltyProgramEnabled() const {
    return isFeatureEnabled("loyalty_program");
}

// Payment configuration
std::vector<std::string> ConfigurationManager::getEnabledPaymentMethods() const {
    return getValue<std::vector<std::string>>("payment.enabled_methods", 
        {"cash", "credit_card", "debit_card", "mobile_pay"});
}

void ConfigurationManager::setPaymentMethodEnabled(const std::string& method, bool enabled) {
    auto methods = getEnabledPaymentMethods();
    auto it = std::find(methods.begin(), methods.end(), method);
    
    if (enabled && it == methods.end()) {
        methods.push_back(method);
    } else if (!enabled && it != methods.end()) {
        methods.erase(it);
    }
    
    setValue<std::vector<std::string>>("payment.enabled_methods", methods);
}

std::vector<double> ConfigurationManager::getTipSuggestions() const {
    return getValue<std::vector<double>>("payment.tip_suggestions", {0.15, 0.18, 0.20, 0.25});
}

void ConfigurationManager::setTipSuggestions(const std::vector<double>& suggestions) {
    setValue<std::vector<double>>("payment.tip_suggestions", suggestions);
}

// File operations (basic implementations)
bool ConfigurationManager::loadFromFile(const std::string& filePath) {
    std::cout << "[ConfigurationManager] File loading not implemented, using defaults" << std::endl;
    lastLoadedFile_ = filePath;
    return true;
}

void ConfigurationManager::loadFromEnvironment(const std::string& prefix) {
    std::cout << "[ConfigurationManager] Environment loading not implemented" << std::endl;
}

bool ConfigurationManager::saveToFile(const std::string& filePath) const {
    std::cout << "[ConfigurationManager] File saving not implemented" << std::endl;
    return true;
}

bool ConfigurationManager::reload() {
    if (!lastLoadedFile_.empty()) {
        return loadFromFile(lastLoadedFile_);
    }
    return false;
}

// Kitchen prep times (placeholder implementation)
std::unordered_map<std::string, int> ConfigurationManager::getKitchenPrepTimes() const {
    std::unordered_map<std::string, int> prepTimes;
    prepTimes["appetizer"] = 8;
    prepTimes["main_course"] = 15;
    prepTimes["dessert"] = 5;
    prepTimes["beverage"] = 2;
    prepTimes["special"] = 20;
    return prepTimes;
}

void ConfigurationManager::setKitchenPrepTime(const std::string& category, int minutes) {
    // Placeholder implementation
    std::cout << "[ConfigurationManager] Setting prep time for " << category << ": " << minutes << " minutes" << std::endl;
}

// =================================================================
// LLM Configuration Implementation
// =================================================================

void ConfigurationManager::setDefaultLLMConfig() {
    auto& llm = getOrCreateSection("llm");

    // Basic LLM settings
    llm["enabled"] = false;  // Disabled by default
    llm["provider"] = std::string("anthropic");
    llm["api_key"] = std::string("");  // Empty by default - must be set
    llm["model"] = std::string("claude-3-sonnet-20240229");
    llm["base_url"] = std::string("");  // Use default for provider
    llm["timeout"] = 60;  // 60 seconds
    llm["max_tokens"] = 4096;
    llm["debug_mode"] = false;

    // Cache settings
    llm["cache_enabled"] = true;
    llm["cache_ttl_minutes"] = 30;

    // Geolocation settings
    llm["default_radius_km"] = 5.0;
    llm["max_radius_km"] = 50.0;

    std::cout << "[ConfigurationManager] LLM configuration defaults set" << std::endl;
}

bool ConfigurationManager::isLLMEnabled() const {
    return getValue<bool>("llm.enabled", false);
}

void ConfigurationManager::setLLMEnabled(bool enabled) {
    setValue<bool>("llm.enabled", enabled);
}

std::string ConfigurationManager::getLLMProvider() const {
    return getValue<std::string>("llm.provider", "anthropic");
}

void ConfigurationManager::setLLMProvider(const std::string& provider) {
    setValue<std::string>("llm.provider", provider);
}

std::string ConfigurationManager::getLLMApiKey() const {
    std::string key = getValue<std::string>("llm.api_key", "");

    // Check environment variable if config value is empty or a placeholder
    if (key.empty() || key.find("${") != std::string::npos) {
        const char* envKey = std::getenv("LLM_API_KEY");
        if (envKey) {
            return std::string(envKey);
        }
    }

    return key;
}

void ConfigurationManager::setLLMApiKey(const std::string& apiKey) {
    setValue<std::string>("llm.api_key", apiKey);
}

std::string ConfigurationManager::getLLMModel() const {
    return getValue<std::string>("llm.model", "claude-3-sonnet-20240229");
}

void ConfigurationManager::setLLMModel(const std::string& model) {
    setValue<std::string>("llm.model", model);
}

std::string ConfigurationManager::getLLMBaseUrl() const {
    return getValue<std::string>("llm.base_url", "");
}

void ConfigurationManager::setLLMBaseUrl(const std::string& baseUrl) {
    setValue<std::string>("llm.base_url", baseUrl);
}

int ConfigurationManager::getLLMTimeout() const {
    return getValue<int>("llm.timeout", 60);
}

void ConfigurationManager::setLLMTimeout(int timeoutSeconds) {
    setValue<int>("llm.timeout", timeoutSeconds);
}

int ConfigurationManager::getLLMMaxTokens() const {
    return getValue<int>("llm.max_tokens", 4096);
}

void ConfigurationManager::setLLMMaxTokens(int maxTokens) {
    setValue<int>("llm.max_tokens", maxTokens);
}

bool ConfigurationManager::isLLMDebugMode() const {
    return getValue<bool>("llm.debug_mode", false);
}

void ConfigurationManager::setLLMDebugMode(bool enabled) {
    setValue<bool>("llm.debug_mode", enabled);
}

double ConfigurationManager::getLLMDefaultRadius() const {
    return getValue<double>("llm.default_radius_km", 5.0);
}

double ConfigurationManager::getLLMMaxRadius() const {
    return getValue<double>("llm.max_radius_km", 50.0);
}
