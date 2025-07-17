#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <any>

/**
 * @file ConfigurationManager.hpp
 * @brief Centralized configuration management for the Restaurant POS System
 * 
 * This manager handles all configuration settings including restaurant
 * information, system settings, feature flags, and API integration.
 * 
 * @author Restaurant POS Team
 * @version 2.1.0 - Enhanced with API configuration
 */

 
/**
 * @class ConfigurationManager
 * @brief Service for managing application configuration
 * 
 * The ConfigurationManager provides centralized access to all configuration
 * settings with support for different sources (files, environment variables,
 * defaults) and type-safe access methods. Enhanced with API integration settings.
 */
class ConfigurationManager {
public:
    /**
     * @brief Configuration value type
     */
    using ConfigValue = std::any;
    
    /**
     * @brief Configuration section type
     */
    using ConfigSection = std::unordered_map<std::string, ConfigValue>;
    
    /**
     * @brief Constructs the configuration manager
     */
    ConfigurationManager();
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ConfigurationManager() = default;
    
    /**
     * @brief Initializes the configuration manager
     * Loads configuration from files and sets up defaults
     */
    void initialize();
    
    /**
     * @brief Loads configuration from a file
     * @param filePath Path to configuration file
     * @return True if file was loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filePath);
    
    /**
     * @brief Loads configuration from environment variables
     * @param prefix Prefix to look for in environment variables
     */
    void loadFromEnvironment(const std::string& prefix = "POS_");
    
    /**
     * @brief Sets up default configuration values
     */
    void loadDefaults();
    
    // =================================================================
    // Generic Configuration Access
    // =================================================================
    
    /**
     * @brief Gets a configuration value with type checking
     * @tparam T Expected type of the value
     * @param key Configuration key in dot notation (e.g., "server.port")
     * @param defaultValue Default value if key is not found
     * @return Configuration value or default
     */
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue = T{}) const;
    
    /**
     * @brief Sets a configuration value
     * @tparam T Type of the value
     * @param key Configuration key in dot notation
     * @param value Value to set
     */
    template<typename T>
    void setValue(const std::string& key, const T& value);
    
    /**
     * @brief Checks if a configuration key exists
     * @param key Configuration key to check
     * @return True if key exists, false otherwise
     */
    bool hasKey(const std::string& key) const;
    
    /**
     * @brief Removes a configuration key
     * @param key Configuration key to remove
     * @return True if key was found and removed, false otherwise
     */
    bool removeKey(const std::string& key);
    
    /**
     * @brief Gets all keys in a section
     * @param sectionName Section name (e.g., "server")
     * @return Vector of keys in the section
     */
    std::vector<std::string> getSectionKeys(const std::string& sectionName) const;
    
    // =================================================================
    // Restaurant Configuration
    // =================================================================
    
    std::string getRestaurantName() const;
    void setRestaurantName(const std::string& name);
    
    std::string getRestaurantAddress() const;
    void setRestaurantAddress(const std::string& address);
    
    std::string getRestaurantPhone() const;
    void setRestaurantPhone(const std::string& phone);
    
    double getTaxRate() const;
    void setTaxRate(double rate);
    
    // =================================================================
    // Server Configuration
    // =================================================================
    
    int getServerPort() const;
    void setServerPort(int port);
    
    std::string getServerAddress() const;
    void setServerAddress(const std::string& address);
    
    int getSessionTimeout() const;
    void setSessionTimeout(int timeoutSeconds);
    
    // =================================================================
    // Order Configuration
    // =================================================================
    
    int getStartingOrderId() const;
    void setStartingOrderId(int startId);
    
    int getOrderTimeout() const;
    void setOrderTimeout(int timeoutMinutes);
    
    int getMaxItemsPerOrder() const;
    void setMaxItemsPerOrder(int maxItems);
    
    // =================================================================
    // Kitchen Configuration
    // =================================================================
    
    int getKitchenRefreshRate() const;
    void setKitchenRefreshRate(int rateSeconds);
    
    int getKitchenBusyThreshold() const;
    void setKitchenBusyThreshold(int threshold);
    
    std::unordered_map<std::string, int> getKitchenPrepTimes() const;
    void setKitchenPrepTime(const std::string& category, int minutes);
    
    // =================================================================
    // UI Configuration
    // =================================================================
    
    std::string getDefaultTheme() const;
    void setDefaultTheme(const std::string& themeId);
    
    int getUIUpdateInterval() const;
    void setUIUpdateInterval(int intervalSeconds);
    
    bool getGroupMenuByCategory() const;
    void setGroupMenuByCategory(bool group);
    
    // =================================================================
    // Feature Flags
    // =================================================================
    
    bool isFeatureEnabled(const std::string& featureName) const;
    void setFeatureEnabled(const std::string& featureName, bool enabled);
    
    // Common feature flags
    bool isInventoryEnabled() const;
    bool isStaffManagementEnabled() const;
    bool isCustomerManagementEnabled() const;
    bool isReportingEnabled() const;
    bool isLoyaltyProgramEnabled() const;
    
    // =================================================================
    // Payment Configuration
    // =================================================================
    
    std::vector<std::string> getEnabledPaymentMethods() const;
    void setPaymentMethodEnabled(const std::string& method, bool enabled);
    
    std::vector<double> getTipSuggestions() const;
    void setTipSuggestions(const std::vector<double>& suggestions);
    
    // =================================================================
    // Persistence
    // =================================================================
    
    /**
     * @brief Saves current configuration to file
     * @param filePath Path to save configuration to
     * @return True if saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filePath) const;
    
    /**
     * @brief Reloads configuration from the last loaded file
     * @return True if reloaded successfully, false otherwise
     */
    bool reload();

protected:
    /**
     * @brief Parses a dot-notation key into section and key parts
     * @param key Full key in dot notation
     * @return Pair of section name and key name
     */
    std::pair<std::string, std::string> parseKey(const std::string& key) const;
    
    /**
     * @brief Gets a configuration section
     * @param sectionName Name of the section
     * @return Pointer to section, or nullptr if not found
     */
    ConfigSection* getSection(const std::string& sectionName);
    
    /**
     * @brief Gets a configuration section (const version)
     * @param sectionName Name of the section
     * @return Pointer to section, or nullptr if not found
     */
    const ConfigSection* getSection(const std::string& sectionName) const;
    
    /**
     * @brief Creates a section if it doesn't exist
     * @param sectionName Name of the section to create
     * @return Reference to the section
     */
    ConfigSection& getOrCreateSection(const std::string& sectionName);

private:
    // Configuration data
    std::unordered_map<std::string, ConfigSection> config_;
    
    // File management
    std::string lastLoadedFile_;
    
    // Helper methods
    void setDefaultRestaurantConfig();
    void setDefaultServerConfig();
    void setDefaultOrderConfig();
    void setDefaultKitchenConfig();
    void setDefaultUIConfig();
    void setDefaultFeatureFlags();
    void setDefaultPaymentConfig();
    void setDefaultAPIConfig();  // ADDED: API configuration method
    
    // Type conversion helpers
    template<typename T>
    T convertValue(const ConfigValue& value, const T& defaultValue) const;
    
    bool isValidKey(const std::string& key) const;
};

// Template implementation
template<typename T>
T ConfigurationManager::getValue(const std::string& key, const T& defaultValue) const {
    auto [sectionName, keyName] = parseKey(key);
    
    const auto* section = getSection(sectionName);
    if (!section) {
        return defaultValue;
    }
    
    auto it = section->find(keyName);
    if (it == section->end()) {
        return defaultValue;
    }
    
    return convertValue<T>(it->second, defaultValue);
}

template<typename T>
void ConfigurationManager::setValue(const std::string& key, const T& value) {
    auto [sectionName, keyName] = parseKey(key);
    auto& section = getOrCreateSection(sectionName);
    section[keyName] = value;
}

#endif // CONFIGURATIONMANAGER_H
