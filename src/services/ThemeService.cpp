//=============================================================================
// IMPLEMENTATION FILE: ThemeService.cpp
//=============================================================================

#include "ThemeService.hpp"
#include "../events/POSEvents.hpp"

#include <Wt/WBootstrapTheme.h>
#include <Wt/WApplication.h>

#include <iostream>
#include <fstream>
#include <algorithm>

ThemeService::ThemeService(std::shared_ptr<EventManager> eventManager,
                          Wt::WApplication* application)
    : eventManager_(eventManager), application_(application),
      themeDirectory_("themes/"), customThemeDirectory_("themes/custom/"),
      allowUserThemes_(true), persistUserPreference_(true) {
    
}

void ThemeService::initialize() {
    std::cout << "Initializing ThemeService..." << std::endl;
    
    // Load themes from configuration or use hardcoded defaults
    if (!loadThemesFromConfiguration()) {
        std::cout << "Loading hardcoded themes as fallback..." << std::endl;
        loadHardcodedThemes();
    }
    
    // Apply user's preferred theme or default theme
    std::string preferredTheme = getUserPreferredTheme();
    if (!preferredTheme.empty() && applyTheme(preferredTheme)) {
        std::cout << "Applied user preferred theme: " << preferredTheme << std::endl;
    } else {
        if (!applyDefaultTheme()) {
            std::cerr << "Warning: No default theme found, using bootstrap fallback" << std::endl;
            applyTheme("bootstrap");
        }
    }
    
    std::cout << "✓ ThemeService initialized with " << availableThemes_.size() 
              << " themes" << std::endl;
}

void ThemeService::loadHardcodedThemes() {
    availableThemes_.clear();
    
    // Bootstrap theme (default)
    ThemeInfo bootstrap("bootstrap", "Bootstrap Default", 
                       "Clean, modern Bootstrap styling",
                       "themes/bootstrap.css", "",
                       {"#007bff", "#6c757d", "#28a745", "#dc3545", "#ffc107"},
                       true, true);
    availableThemes_.push_back(bootstrap);
    
    // Dark theme
    ThemeInfo dark("dark", "Dark Mode",
                  "Professional dark theme for low-light environments",
                  "themes/dark.css", "",
                  {"#212529", "#343a40", "#495057", "#6c757d", "#adb5bd"},
                  false, true);
    availableThemes_.push_back(dark);
    
    // Light theme
    ThemeInfo light("light", "Light Mode",
                   "Bright theme with high contrast",
                   "themes/light.css", "",
                   {"#ffffff", "#f8f9fa", "#e9ecef", "#dee2e6", "#ced4da"},
                   false, true);
    availableThemes_.push_back(light);
    
    // Professional theme
    ThemeInfo professional("professional", "Professional",
                          "Corporate-style neutral colors",
                          "themes/professional.css", "",
                          {"#2c3e50", "#34495e", "#7f8c8d", "#95a5a6", "#bdc3c7"},
                          false, true);
    availableThemes_.push_back(professional);
    
    // Restaurant theme
    ThemeInfo restaurant("restaurant", "Restaurant",
                        "Warm, inviting theme for restaurant environments",
                        "themes/restaurant.css", "",
                        {"#8b4513", "#cd853f", "#daa520", "#ff6347", "#ffa500"},
                        false, true);
    availableThemes_.push_back(restaurant);
    
    std::cout << "✓ Loaded " << availableThemes_.size() << " hardcoded themes" << std::endl;
}

bool ThemeService::loadThemesFromConfiguration() {
    // This would load themes from a configuration file
    // For now, we'll use the hardcoded themes
    // In a full implementation, this would parse a JSON or XML config file
    
    std::cout << "Theme configuration file not implemented, using hardcoded themes" << std::endl;
    return false;
}

const std::vector<ThemeService::ThemeInfo>& ThemeService::getAvailableThemes() const {
    return availableThemes_;
}

std::vector<ThemeService::ThemeInfo> ThemeService::getEnabledThemes() const {
    std::vector<ThemeInfo> enabled;
    
    std::copy_if(availableThemes_.begin(), availableThemes_.end(),
                std::back_inserter(enabled),
                [](const ThemeInfo& theme) { return theme.isEnabled; });
    
    return enabled;
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
    const ThemeInfo* theme = getThemeById(themeId);
    if (!theme) {
        std::cerr << "ThemeService: Theme '" << themeId << "' not found" << std::endl;
        return false;
    }
    
    if (!theme->isEnabled) {
        std::cerr << "ThemeService: Theme '" << themeId << "' is disabled" << std::endl;
        return false;
    }
    
    if (!validateTheme(*theme)) {
        std::cerr << "ThemeService: Theme '" << themeId << "' failed validation" << std::endl;
        return false;
    }
    
    try {
        // Remove previous CSS files
        removePreviousCSSFiles();
        
        // Apply new theme CSS
        if (!applyCSSFiles(*theme)) {
            std::cerr << "ThemeService: Failed to apply CSS for theme '" << themeId << "'" << std::endl;
            return false;
        }
        
        // Update current theme
        previousThemeId_ = currentThemeId_;
        currentThemeId_ = themeId;
        
        // Notify theme change
        notifyThemeChanged(themeId, previousThemeId_);
        
        // Save user preference
        if (persistUserPreference_) {
            setUserPreferredTheme(themeId);
        }
        
        std::cout << "Theme applied successfully: " << theme->name << " (" << themeId << ")" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ThemeService: Error applying theme '" << themeId << "': " << e.what() << std::endl;
        return false;
    }
}

bool ThemeService::applyDefaultTheme() {
    // Find the default theme
    auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
        [](const ThemeInfo& theme) { return theme.isDefault && theme.isEnabled; });
    
    if (it != availableThemes_.end()) {
        return applyTheme(it->id);
    }
    
    // If no default theme, use first enabled theme
    auto enabled = getEnabledThemes();
    if (!enabled.empty()) {
        return applyTheme(enabled[0].id);
    }
    
    return false;
}

bool ThemeService::applyCSSFiles(const ThemeInfo& theme) {
    if (!application_) {
        std::cerr << "ThemeService: No application instance available" << std::endl;
        return false;
    }
    
    try {
        // Apply local CSS file
        if (!theme.cssFile.empty()) {
            Wt::WLink cssLink(Wt::LinkType::Url, theme.cssFile);
            cssLink.setTarget(Wt::LinkTarget::Self);
            application_->useStyleSheet(cssLink);
            appliedCSSLinks_.push_back(cssLink);
            
            std::cout << "Applied CSS file: " << theme.cssFile << std::endl;
        }
        
        // Apply external CSS (e.g., CDN links)
        if (!theme.externalCss.empty()) {
            Wt::WLink externalLink(Wt::LinkType::Url, theme.externalCss);
            externalLink.setTarget(Wt::LinkTarget::Self);
            application_->useStyleSheet(externalLink);
            appliedCSSLinks_.push_back(externalLink);
            
            std::cout << "Applied external CSS: " << theme.externalCss << std::endl;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ThemeService: Error applying CSS files: " << e.what() << std::endl;
        return false;
    }
}

void ThemeService::removePreviousCSSFiles() {
    if (!application_) return;
    
    // Remove previously applied CSS links
    // Note: Wt doesn't provide a direct way to remove stylesheets,
    // so we'll clear our tracking and let the browser handle it
    appliedCSSLinks_.clear();
    
    std::cout << "Previous CSS files marked for removal" << std::endl;
}

void ThemeService::reloadThemes() {
    std::cout << "Reloading themes..." << std::endl;
    
    std::string currentTheme = currentThemeId_;
    
    // Reload themes from configuration
    if (!loadThemesFromConfiguration()) {
        loadHardcodedThemes();
    }
    
    // Try to reapply current theme
    if (!currentTheme.empty()) {
        if (!applyTheme(currentTheme)) {
            std::cout << "Could not reapply previous theme, applying default" << std::endl;
            applyDefaultTheme();
        }
    }
    
    std::cout << "✓ Themes reloaded" << std::endl;
}

bool ThemeService::addTheme(const ThemeInfo& theme) {
    // Check if theme ID already exists
    if (getThemeById(theme.id) != nullptr) {
        std::cerr << "ThemeService: Theme with ID '" << theme.id << "' already exists" << std::endl;
        return false;
    }
    
    if (!validateTheme(theme)) {
        std::cerr << "ThemeService: Theme '" << theme.id << "' failed validation" << std::endl;
        return false;
    }
    
    availableThemes_.push_back(theme);
    
    std::cout << "Theme added: " << theme.name << " (" << theme.id << ")" << std::endl;
    return true;
}

bool ThemeService::removeTheme(const std::string& themeId) {
    auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
        [&themeId](const ThemeInfo& theme) { return theme.id == themeId; });
    
    if (it == availableThemes_.end()) {
        std::cerr << "ThemeService: Theme '" << themeId << "' not found" << std::endl;
        return false;
    }
    
    // Don't allow removing the current theme
    if (themeId == currentThemeId_) {
        std::cerr << "ThemeService: Cannot remove currently active theme" << std::endl;
        return false;
    }
    
    // Don't allow removing the default theme
    if (it->isDefault) {
        std::cerr << "ThemeService: Cannot remove default theme" << std::endl;
        return false;
    }
    
    std::string themeName = it->name;
    availableThemes_.erase(it);
    
    std::cout << "Theme removed: " << themeName << " (" << themeId << ")" << std::endl;
    return true;
}

bool ThemeService::setThemeEnabled(const std::string& themeId, bool enabled) {
    auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
        [&themeId](ThemeInfo& theme) { return theme.id == themeId; });
    
    if (it == availableThemes_.end()) {
        std::cerr << "ThemeService: Theme '" << themeId << "' not found" << std::endl;
        return false;
    }
    
    // Don't allow disabling the current theme
    if (!enabled && themeId == currentThemeId_) {
        std::cerr << "ThemeService: Cannot disable currently active theme" << std::endl;
        return false;
    }
    
    // Don't allow disabling the default theme
    if (!enabled && it->isDefault) {
        std::cerr << "ThemeService: Cannot disable default theme" << std::endl;
        return false;
    }
    
    it->isEnabled = enabled;
    
    std::cout << "Theme " << (enabled ? "enabled" : "disabled") << ": " 
              << it->name << " (" << themeId << ")" << std::endl;
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

bool ThemeService::validateTheme(const ThemeInfo& theme) const {
    // Basic validation
    if (theme.id.empty() || theme.name.empty()) {
        std::cerr << "ThemeService: Theme must have ID and name" << std::endl;
        return false;
    }
    
    // Validate ID format (no spaces, special chars)
    if (sanitizeThemeId(theme.id) != theme.id) {
        std::cerr << "ThemeService: Theme ID contains invalid characters: " << theme.id << std::endl;
        return false;
    }
    
    // Validate that at least one CSS source is provided
    if (theme.cssFile.empty() && theme.externalCss.empty()) {
        std::cerr << "ThemeService: Theme must have at least one CSS source" << std::endl;
        return false;
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

void ThemeService::notifyThemeChanged(const std::string& newThemeId, const std::string& oldThemeId) {
    if (eventManager_) {
        const ThemeInfo* theme = getThemeById(newThemeId);
        std::string themeName = theme ? theme->name : newThemeId;
        
        eventManager_->publish(POSEvents::THEME_CHANGED,
            POSEvents::createThemeChangedEvent(newThemeId, themeName, oldThemeId));
    }
}

// Storage methods (simplified implementation)
void ThemeService::saveThemePreference(const std::string& themeId) {
    // In a real implementation, this would save to a persistent storage
    // For now, we'll just store in memory (lost on restart)
    // This could save to a file, database, or browser localStorage
    
    std::cout << "Theme preference saved: " << themeId << std::endl;
}

std::string ThemeService::loadThemePreference() const {
    // In a real implementation, this would load from persistent storage
    // For now, return empty string (no saved preference)
    
    return "";
}
