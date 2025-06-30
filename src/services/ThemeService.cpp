#include "../../include/services/ThemeService.hpp"

#include <Wt/WBootstrapTheme.h>
#include <Wt/WEnvironment.h>
#include <Wt/WServer.h>

#include <iostream>
#include <fstream>
#include <algorithm>

ThemeService::ThemeService(std::shared_ptr<EventManager> eventManager,
                          Wt::WApplication* application)
    : eventManager_(eventManager)
    , application_(application)
    , themeDirectory_("themes/")
    , customThemeDirectory_("themes/custom/")
    , allowUserThemes_(true)
    , persistUserPreference_(true) {
}

void ThemeService::initialize() {
    std::cout << "Initializing ThemeService..." << std::endl;
    
    // Load available themes
    loadHardcodedThemes();
    
    // Try to load themes from configuration file
    if (!loadThemesFromConfiguration()) {
        std::cout << "No theme configuration file found, using hardcoded themes" << std::endl;
    }
    
    // Apply user's preferred theme or default
    std::string preferredTheme = getUserPreferredTheme();
    if (!preferredTheme.empty() && applyTheme(preferredTheme)) {
        std::cout << "Applied user's preferred theme: " << preferredTheme << std::endl;
    } else {
        applyDefaultTheme();
    }
    
    std::cout << "✓ ThemeService initialized with " << availableThemes_.size() 
              << " available themes" << std::endl;
}

void ThemeService::loadHardcodedThemes() {
    availableThemes_.clear();
    
    // Bootstrap Theme (Default)
    availableThemes_.emplace_back(
        "bootstrap",
        "Bootstrap",
        "Clean, modern Bootstrap-based theme",
        "themes/bootstrap.css",
        "",
        std::vector<std::string>{"#007bff", "#6c757d", "#28a745", "#dc3545"},
        true,  // isDefault
        true   // isEnabled
    );
    
    // Dark Theme
    availableThemes_.emplace_back(
        "dark",
        "Dark Mode",
        "Professional dark theme for low-light environments",
        "themes/dark.css",
        "",
        std::vector<std::string>{"#1a1a1a", "#333333", "#007bff", "#28a745"},
        false, // isDefault
        true   // isEnabled
    );
    
    // Light Theme
    availableThemes_.emplace_back(
        "light",
        "Light Mode",
        "Bright, clean theme with high contrast",
        "themes/light.css",
        "",
        std::vector<std::string>{"#ffffff", "#f8f9fa", "#0066cc", "#28a745"},
        false, // isDefault
        true   // isEnabled
    );
    
    // Professional Theme
    availableThemes_.emplace_back(
        "professional",
        "Professional",
        "Corporate-style theme with neutral colors",
        "themes/professional.css",
        "",
        std::vector<std::string>{"#2c3e50", "#34495e", "#3498db", "#27ae60"},
        false, // isDefault
        true   // isEnabled
    );
    
    // Restaurant Theme
    availableThemes_.emplace_back(
        "restaurant",
        "Restaurant",
        "Warm, inviting theme designed for restaurant environments",
        "themes/restaurant.css",
        "",
        std::vector<std::string>{"#8B4513", "#D2691E", "#FF6347", "#32CD32"},
        false, // isDefault
        true   // isEnabled
    );
    
    std::cout << "✓ Loaded " << availableThemes_.size() << " hardcoded themes" << std::endl;
}

bool ThemeService::loadThemesFromConfiguration() {
    // This would load themes from a configuration file
    // For now, we'll just return false to indicate no config file
    // In a real implementation, you'd parse JSON/XML/YAML config files
    
    std::ifstream configFile(themeDirectory_ + "themes.json");
    if (!configFile.is_open()) {
        return false;
    }
    
    // TODO: Parse configuration file and add themes
    // This is where you'd parse JSON configuration like:
    /*
    {
        "themes": [
            {
                "id": "custom1",
                "name": "Custom Theme 1",
                "description": "A custom theme",
                "cssFile": "themes/custom1.css",
                "previewColors": ["#ff0000", "#00ff00", "#0000ff"],
                "enabled": true
            }
        ]
    }
    */
    
    configFile.close();
    return false; // Return true when actually implemented
}

void ThemeService::initializeDefaultThemes() {
    // This is called by loadHardcodedThemes()
}

const std::vector<ThemeService::ThemeInfo>& ThemeService::getAvailableThemes() const {
    return availableThemes_;
}

std::vector<ThemeService::ThemeInfo> ThemeService::getEnabledThemes() const {
    std::vector<ThemeInfo> enabledThemes;
    
    for (const auto& theme : availableThemes_) {
        if (theme.isEnabled) {
            enabledThemes.push_back(theme);
        }
    }
    
    return enabledThemes;
}

const ThemeService::ThemeInfo* ThemeService::getThemeById(const std::string& themeId) const {
    auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
        [&themeId](const ThemeInfo& theme) { return theme.id == themeId; });
    
    return (it != availableThemes_.end()) ? &(*it) : nullptr;
}

const std::string& ThemeService::getCurrentThemeId() const {
    return currentThemeId_;
}

const ThemeService::ThemeInfo* ThemeService::getCurrentTheme() const {
    return getThemeById(currentThemeId_);
}

bool ThemeService::applyTheme(const std::string& themeId) {
    const auto* theme = getThemeById(themeId);
    if (!theme) {
        std::cerr << "Error: Theme '" << themeId << "' not found" << std::endl;
        return false;
    }
    
    if (!theme->isEnabled) {
        std::cerr << "Error: Theme '" << themeId << "' is disabled" << std::endl;
        return false;
    }
    
    if (!validateTheme(*theme)) {
        std::cerr << "Error: Theme '" << themeId << "' failed validation" << std::endl;
        return false;
    }
    
    // Store previous theme
    previousThemeId_ = currentThemeId_;
    
    // Remove previous CSS files
    removePreviousCSSFiles();
    
    // Apply new theme CSS
    if (!applyCSSFiles(*theme)) {
        std::cerr << "Error: Failed to apply CSS files for theme '" << themeId << "'" << std::endl;
        return false;
    }
    
    // Update current theme
    currentThemeId_ = themeId;
    
    // Save user preference
    if (persistUserPreference_) {
        saveThemePreference(themeId);
    }
    
    // Notify about theme change
    notifyThemeChanged(themeId, previousThemeId_);
    
    std::cout << "✓ Applied theme: " << theme->name << " (" << themeId << ")" << std::endl;
    return true;
}

bool ThemeService::applyDefaultTheme() {
    auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
        [](const ThemeInfo& theme) { return theme.isDefault && theme.isEnabled; });
    
    if (it != availableThemes_.end()) {
        return applyTheme(it->id);
    }
    
    // If no default theme found, use the first enabled theme
    auto enabledThemes = getEnabledThemes();
    if (!enabledThemes.empty()) {
        return applyTheme(enabledThemes[0].id);
    }
    
    std::cerr << "Error: No default or enabled themes available" << std::endl;
    return false;
}

bool ThemeService::applyCSSFiles(const ThemeInfo& theme) {
    if (!application_) {
        std::cerr << "Error: WApplication not available for CSS application" << std::endl;
        return false;
    }
    
    bool success = true;
    
    // Apply main CSS file
    if (!theme.cssFile.empty()) {
        try {
            Wt::WLink cssLink(theme.cssFile);
            application_->useStyleSheet(cssLink);
            appliedCSSLinks_.push_back(cssLink);
            std::cout << "Applied CSS file: " << theme.cssFile << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error applying CSS file " << theme.cssFile << ": " << e.what() << std::endl;
            success = false;
        }
    }
    
    // Apply external CSS (if any)
    if (!theme.externalCss.empty()) {
        try {
            Wt::WLink externalLink(Wt::LinkType::Url, theme.externalCss);
            application_->useStyleSheet(externalLink);
            appliedCSSLinks_.push_back(externalLink);
            std::cout << "Applied external CSS: " << theme.externalCss << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error applying external CSS " << theme.externalCss << ": " << e.what() << std::endl;
            success = false;
        }
    }
    
    return success;
}

void ThemeService::removePreviousCSSFiles() {
    // Note: Wt doesn't provide a direct way to remove stylesheets
    // In practice, theme switching often requires page reload
    // For dynamic theme switching, you'd typically use CSS custom properties (CSS variables)
    
    appliedCSSLinks_.clear();
    std::cout << "Previous CSS files marked for removal" << std::endl;
}

void ThemeService::reloadThemes() {
    std::cout << "Reloading themes..." << std::endl;
    
    std::string currentTheme = currentThemeId_;
    
    // Reload theme definitions
    loadHardcodedThemes();
    loadThemesFromConfiguration();
    
    // Reapply current theme if it still exists
    if (getThemeById(currentTheme)) {
        applyTheme(currentTheme);
    } else {
        applyDefaultTheme();
    }
    
    std::cout << "✓ Themes reloaded" << std::endl;
}

bool ThemeService::addTheme(const ThemeInfo& theme) {
    // Check if theme ID already exists
    if (getThemeById(theme.id)) {
        std::cerr << "Error: Theme with ID '" << theme.id << "' already exists" << std::endl;
        return false;
    }
    
    if (!validateTheme(theme)) {
        std::cerr << "Error: Theme validation failed for '" << theme.id << "'" << std::endl;
        return false;
    }
    
    availableThemes_.push_back(theme);
    
    std::cout << "Added theme: " << theme.name << " (" << theme.id << ")" << std::endl;
    return true;
}

bool ThemeService::removeTheme(const std::string& themeId) {
    auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
        [&themeId](const ThemeInfo& theme) { return theme.id == themeId; });
    
    if (it == availableThemes_.end()) {
        std::cerr << "Error: Theme '" << themeId << "' not found for removal" << std::endl;
        return false;
    }
    
    // Don't allow removal of the currently active theme
    if (themeId == currentThemeId_) {
        std::cerr << "Error: Cannot remove currently active theme '" << themeId << "'" << std::endl;
        return false;
    }
    
    std::string themeName = it->name;
    availableThemes_.erase(it);
    
    std::cout << "Removed theme: " << themeName << " (" << themeId << ")" << std::endl;
    return true;
}

bool ThemeService::setThemeEnabled(const std::string& themeId, bool enabled) {
    auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
        [&themeId](ThemeInfo& theme) { return theme.id == themeId; });
    
    if (it == availableThemes_.end()) {
        std::cerr << "Error: Theme '" << themeId << "' not found" << std::endl;
        return false;
    }
    
    // Don't allow disabling the currently active theme
    if (!enabled && themeId == currentThemeId_) {
        std::cerr << "Error: Cannot disable currently active theme '" << themeId << "'" << std::endl;
        return false;
    }
    
    it->isEnabled = enabled;
    
    std::cout << (enabled ? "Enabled" : "Disabled") << " theme: " << it->name 
              << " (" << themeId << ")" << std::endl;
    return true;
}

std::string ThemeService::getUserPreferredTheme() const {
    if (!persistUserPreference_) {
        return "";
    }
    
    return loadThemePreference();
}

void ThemeService::setUserPreferredTheme(const std::string& themeId) {
    if (!persistUserPreference_) {
        return;
    }
    
    saveThemePreference(themeId);
}

void ThemeService::clearUserPreferredTheme() {
    if (!persistUserPreference_) {
        return;
    }
    
    saveThemePreference("");
}

void ThemeService::notifyThemeChanged(const std::string& newThemeId, const std::string& oldThemeId) {
    if (!eventManager_) {
        return;
    }
    
    const auto* newTheme = getThemeById(newThemeId);
    const auto* oldTheme = getThemeById(oldThemeId);
    
    std::string newThemeName = newTheme ? newTheme->name : newThemeId;
    std::string oldThemeName = oldTheme ? oldTheme->name : oldThemeId;
    
    // Create theme event data
    POSEvents::ThemeEventData themeData(newThemeId, newThemeName, oldThemeId);
    
    // Publish theme changed event
    eventManager_->publish(POSEvents::THEME_CHANGED, themeData);
    
    std::cout << "Theme change event published: " << oldThemeName 
              << " → " << newThemeName << std::endl;
}

bool ThemeService::validateTheme(const ThemeInfo& theme) const {
    // Basic validation
    if (theme.id.empty()) {
        std::cerr << "Theme validation failed: empty ID" << std::endl;
        return false;
    }
    
    if (theme.name.empty()) {
        std::cerr << "Theme validation failed: empty name" << std::endl;
        return false;
    }
    
    // Sanitize theme ID
    std::string sanitizedId = sanitizeThemeId(theme.id);
    if (sanitizedId != theme.id) {
        std::cerr << "Theme validation failed: invalid characters in ID" << std::endl;
        return false;
    }
    
    // Check if CSS file exists (basic check)
    if (!theme.cssFile.empty()) {
        std::ifstream cssFile(theme.cssFile);
        if (!cssFile.good()) {
            std::cerr << "Theme validation warning: CSS file '" << theme.cssFile 
                      << "' not accessible" << std::endl;
            // Don't fail validation, as file might be created later
        }
    }
    
    return true;
}

std::string ThemeService::sanitizeThemeId(const std::string& themeId) const {
    std::string sanitized;
    
    for (char c : themeId) {
        if (std::isalnum(c) || c == '_' || c == '-') {
            sanitized += c;
        }
    }
    
    return sanitized;
}

void ThemeService::saveThemePreference(const std::string& themeId) {
    // In a real implementation, this would save to a database, 
    // configuration file, or browser storage
    // For now, we'll just log it
    
    std::cout << "Saving theme preference: " << themeId << std::endl;
    
    // Example implementation might save to a user preference file:
    /*
    std::ofstream prefFile("user_preferences.txt");
    if (prefFile.is_open()) {
        prefFile << "theme=" << themeId << std::endl;
        prefFile.close();
    }
    */
}

std::string ThemeService::loadThemePreference() const {
    // In a real implementation, this would load from storage
    // For now, we'll return empty string
    
    // Example implementation might load from a user preference file:
    /*
    std::ifstream prefFile("user_preferences.txt");
    if (prefFile.is_open()) {
        std::string line;
        while (std::getline(prefFile, line)) {
            if (line.find("theme=") == 0) {
                return line.substr(6); // Return everything after "theme="
            }
        }
        prefFile.close();
    }
    */
    
    return ""; // No preference found
}
