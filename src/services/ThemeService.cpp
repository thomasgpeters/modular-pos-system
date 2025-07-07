// ============================================================================
// Fixed ThemeService Implementation - Core Functionality Only
// File: src/services/ThemeService.cpp
// ============================================================================

#include "../../include/services/ThemeService.hpp"
#include <Wt/WApplication.h>
#include <iostream>
#include <fstream>
#include <algorithm>

ThemeService::ThemeService(Wt::WApplication* app)
    : app_(app)
    , currentTheme_(Theme::BASE)
    , preferredTheme_(Theme::BASE)
    , nextCallbackId_(1)
    , frameworkLoaded_(false)
{
    if (!app_) {
        throw std::invalid_argument("ThemeService requires a valid WApplication instance");
    }
    
    initializeThemeMetadata();
    loadThemePreference();
    detectSystemTheme();
    
    std::cout << "[ThemeService] Initialized with theme: " << getThemeName(currentTheme_) << std::endl;
}

void ThemeService::initializeThemeMetadata() {
    // Theme Names
    themeNames_[Theme::BASE] = "Bootstrap Base";
    themeNames_[Theme::LIGHT] = "Light Theme";
    themeNames_[Theme::DARK] = "Dark Theme";
    themeNames_[Theme::WARM] = "Warm Theme";
    themeNames_[Theme::COOL] = "Cool Theme";
    themeNames_[Theme::AUTO] = "Auto Theme";
    
    // Theme Descriptions
    themeDescriptions_[Theme::BASE] = "Clean, professional Bootstrap default";
    themeDescriptions_[Theme::LIGHT] = "Bright, high contrast for visibility";
    themeDescriptions_[Theme::DARK] = "Easy on the eyes, modern dark mode";
    themeDescriptions_[Theme::WARM] = "Restaurant-friendly earth tones";
    themeDescriptions_[Theme::COOL] = "Modern, tech-focused blue tones";
    themeDescriptions_[Theme::AUTO] = "Automatic based on system preference";
    
    // Theme CSS Classes
    themeCSSClasses_[Theme::BASE] = "theme-base";
    themeCSSClasses_[Theme::LIGHT] = "theme-light";
    themeCSSClasses_[Theme::DARK] = "theme-dark";
    themeCSSClasses_[Theme::WARM] = "theme-warm";
    themeCSSClasses_[Theme::COOL] = "theme-cool";
    themeCSSClasses_[Theme::AUTO] = "theme-auto";
    
    // Theme Icons
    themeIcons_[Theme::BASE] = "⚪";
    themeIcons_[Theme::LIGHT] = "☀️";
    themeIcons_[Theme::DARK] = "🌙";
    themeIcons_[Theme::WARM] = "🔥";
    themeIcons_[Theme::COOL] = "❄️";
    themeIcons_[Theme::AUTO] = "🔄";
    
    // Theme Primary Colors
    themePrimaryColors_[Theme::BASE] = "#007bff";
    themePrimaryColors_[Theme::LIGHT] = "#0066cc";
    themePrimaryColors_[Theme::DARK] = "#4dabf7";
    themePrimaryColors_[Theme::WARM] = "#8b4513";
    themePrimaryColors_[Theme::COOL] = "#1e3a8a";
    themePrimaryColors_[Theme::AUTO] = "#007bff";
    
    // Theme CSS Paths - Detect CSS location automatically
    std::string cssBase = detectCSSBasePath();
    
    themeCSSPaths_[Theme::BASE] = cssBase + "theme-framework.css";
    themeCSSPaths_[Theme::LIGHT] = cssBase + "theme-light.css";
    themeCSSPaths_[Theme::DARK] = cssBase + "theme-dark.css";
    themeCSSPaths_[Theme::WARM] = cssBase + "theme-warm.css";
    themeCSSPaths_[Theme::COOL] = cssBase + "theme-cool.css";
    themeCSSPaths_[Theme::AUTO] = cssBase + "theme-framework.css";
    
    // Theme Categories
    themeCategories_[Theme::BASE] = "Standard";
    themeCategories_[Theme::LIGHT] = "Standard";
    themeCategories_[Theme::DARK] = "Standard";
    themeCategories_[Theme::WARM] = "Themed";
    themeCategories_[Theme::COOL] = "Themed";
    themeCategories_[Theme::AUTO] = "Automatic";
    
    std::cout << "[ThemeService] Theme metadata initialized with CSS base: " << cssBase << std::endl;
}

std::string ThemeService::detectCSSBasePath() {
    // Try different CSS locations in order of preference
    std::vector<std::string> possiblePaths = {
        "css/",                          // Web-accessible (relative to docroot)
        "../docroot/css/",               // From build directory
        "docroot/css/",                  // From project root
        "build/resources/css/",          // External resources
        "resources/css/"                 // Internal resources
    };
    
    for (const auto& path : possiblePaths) {
        std::string testFile = path + "theme-framework.css";
        std::ifstream file(testFile);
        if (file.good()) {
            std::cout << "[ThemeService] Found CSS files at: " << path << std::endl;
            return path;
        }
    }
    
    std::cout << "[ThemeService] CSS files not found, using default path: css/" << std::endl;
    return "css/"; // Default fallback
}

void ThemeService::setTheme(Theme theme, bool savePreference) {
    if (theme == currentTheme_) {
        return; // No change needed
    }
    
    Theme oldTheme = currentTheme_;
    Theme effectiveTheme = resolveTheme(theme);
    
    // Load the theme framework if not already loaded
    ensureFrameworkLoaded();
    
    // Remove old theme classes
    removeThemeClasses();
    
    // Unload old theme CSS if it's not the base theme
    if (currentTheme_ != Theme::BASE && currentTheme_ != Theme::AUTO) {
        unloadThemeCSS(currentTheme_);
    }
    
    // Load new theme CSS
    if (effectiveTheme != Theme::BASE && effectiveTheme != Theme::AUTO) {
        loadThemeCSS(effectiveTheme);
    }
    
    // Update current theme
    currentTheme_ = effectiveTheme;
    preferredTheme_ = theme;
    
    // Apply theme classes
    applyThemeClasses();
    
    // Save preference if requested
    if (savePreference) {
        saveThemePreference();
    }
    
    // Notify callbacks
    notifyThemeChange(oldTheme, currentTheme_);
    
    std::cout << "[ThemeService] Theme changed from " << themeToString(oldTheme) 
              << " to " << themeToString(currentTheme_) << std::endl;
}

void ThemeService::loadThemeFramework() {
    if (frameworkLoaded_) {
        return;
    }
    
    // Load the base theme framework CSS
    std::string frameworkPath = themeCSSPaths_[Theme::BASE];
    
    try {
        // Use Wt's built-in CSS loading
        app_->useStyleSheet(frameworkPath);
        loadedCSSFiles_.push_back(frameworkPath);
        frameworkLoaded_ = true;
        
        std::cout << "[ThemeService] Framework loaded from " << frameworkPath << std::endl;
        
        // Notify CSS loader callback if set
        if (cssLoaderCallback_) {
            cssLoaderCallback_(frameworkPath, true);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ThemeService] Failed to load framework CSS: " << e.what() << std::endl;
    }
}

void ThemeService::loadThemeCSS(Theme theme) {
    if (theme == Theme::BASE || theme == Theme::AUTO) {
        return; // Base theme is included in framework
    }
    
    auto it = themeCSSPaths_.find(theme);
    if (it != themeCSSPaths_.end()) {
        const std::string& cssPath = it->second;
        
        try {
            // Use Wt's built-in CSS loading
            app_->useStyleSheet(cssPath);
            loadedCSSFiles_.push_back(cssPath);
            
            std::cout << "[ThemeService] Loaded theme CSS: " << cssPath << std::endl;
            
            // Notify CSS loader callback if set
            if (cssLoaderCallback_) {
                cssLoaderCallback_(cssPath, true);
            }
        } catch (const std::exception& e) {
            std::cerr << "[ThemeService] Failed to load theme CSS " << cssPath << ": " << e.what() << std::endl;
        }
    }
}

void ThemeService::unloadThemeCSS(Theme theme) {
    if (theme == Theme::BASE || theme == Theme::AUTO) {
        return; // Don't unload base theme
    }
    
    auto it = themeCSSPaths_.find(theme);
    if (it != themeCSSPaths_.end()) {
        const std::string& cssPath = it->second;
        
        // Remove from loaded files list
        auto fileIt = std::find(loadedCSSFiles_.begin(), loadedCSSFiles_.end(), cssPath);
        if (fileIt != loadedCSSFiles_.end()) {
            loadedCSSFiles_.erase(fileIt);
            
            std::cout << "[ThemeService] Unloaded theme CSS: " << cssPath << std::endl;
            
            // Notify CSS loader callback if set
            if (cssLoaderCallback_) {
                cssLoaderCallback_(cssPath, false);
            }
        }
    }
}

void ThemeService::applyThemeClasses() {
    if (!app_) return;
    
    auto root = app_->root();
    if (!root) return;
    
    // Add theme-specific class to body
    std::string themeClass = getThemeCSSClass(currentTheme_);
    root->addStyleClass(themeClass);
    
    // Also use JavaScript to ensure body class is set correctly
    std::string script = 
        "document.body.className = document.body.className.replace(/theme-\\w+/g, '');"
        "document.body.classList.add('" + themeClass + "');";
    
    app_->doJavaScript(script);
    
    std::cout << "[ThemeService] Applied theme class: " << themeClass << std::endl;
}

void ThemeService::removeThemeClasses() {
    if (!app_) return;
    
    auto root = app_->root();
    if (!root) return;
    
    // Remove all theme classes
    for (const auto& pair : themeCSSClasses_) {
        root->removeStyleClass(pair.second);
    }
    
    // Also remove from body via JavaScript
    app_->doJavaScript("document.body.className = document.body.className.replace(/theme-\\w+/g, '');");
}

void ThemeService::ensureFrameworkLoaded() {
    if (!frameworkLoaded_) {
        loadThemeFramework();
    }
}

ThemeService::Theme ThemeService::resolveTheme(Theme theme) const {
    if (theme == Theme::AUTO) {
        return isSystemDarkMode() ? Theme::DARK : Theme::LIGHT;
    }
    return theme;
}

bool ThemeService::isSystemDarkMode() const {
    // Simplified system theme detection
    return false; // Default to light mode
}

void ThemeService::detectSystemTheme() {
    // Simple detection for now
    if (preferredTheme_ == Theme::AUTO) {
        setTheme(isSystemDarkMode() ? Theme::DARK : Theme::LIGHT, false);
    }
}

std::vector<ThemeService::Theme> ThemeService::getAvailableThemes() const {
    return {
        Theme::BASE,
        Theme::LIGHT,
        Theme::DARK,
        Theme::WARM,
        Theme::COOL,
        Theme::AUTO
    };
}

std::string ThemeService::getThemeName(Theme theme) const {
    auto it = themeNames_.find(theme);
    return it != themeNames_.end() ? it->second : "Unknown";
}

std::string ThemeService::getThemeDescription(Theme theme) const {
    auto it = themeDescriptions_.find(theme);
    return it != themeDescriptions_.end() ? it->second : "";
}

std::string ThemeService::getThemeCSSClass(Theme theme) const {
    auto it = themeCSSClasses_.find(theme);
    return it != themeCSSClasses_.end() ? it->second : "theme-base";
}

std::string ThemeService::getThemeIcon(Theme theme) const {
    auto it = themeIcons_.find(theme);
    return it != themeIcons_.end() ? it->second : "⚪";
}

std::string ThemeService::getThemePrimaryColor(Theme theme) const {
    auto it = themePrimaryColors_.find(theme);
    return it != themePrimaryColors_.end() ? it->second : "#007bff";
}

std::string ThemeService::getThemeCSSPath(Theme theme) const {
    auto it = themeCSSPaths_.find(theme);
    return it != themeCSSPaths_.end() ? it->second : "";
}

std::string ThemeService::getThemeCategory(Theme theme) const {
    auto it = themeCategories_.find(theme);
    return it != themeCategories_.end() ? it->second : "Standard";
}

size_t ThemeService::onThemeChanged(ThemeChangeCallback callback) {
    size_t handle = nextCallbackId_++;
    callbacks_[handle] = callback;
    return handle;
}

void ThemeService::removeThemeChangeCallback(size_t handle) {
    callbacks_.erase(handle);
}

void ThemeService::notifyThemeChange(Theme oldTheme, Theme newTheme) {
    for (const auto& pair : callbacks_) {
        try {
            pair.second(oldTheme, newTheme);
        } catch (const std::exception& e) {
            std::cerr << "[ThemeService] Error in theme change callback: " << e.what() << std::endl;
        }
    }
}

void ThemeService::setCSSLoaderCallback(CSSLoaderCallback callback) {
    cssLoaderCallback_ = callback;
}

void ThemeService::loadThemePreference() {
    // Simple default for now
    preferredTheme_ = Theme::BASE;
    currentTheme_ = Theme::BASE;
}

void ThemeService::saveThemePreference() {
    std::cout << "[ThemeService] Saving theme preference: " << themeToString(preferredTheme_) << std::endl;
}

std::string ThemeService::themeToString(Theme theme) const {
    switch (theme) {
        case Theme::BASE: return "base";
        case Theme::LIGHT: return "light";
        case Theme::DARK: return "dark";
        case Theme::WARM: return "warm";
        case Theme::COOL: return "cool";
        case Theme::AUTO: return "auto";
        default: return "unknown";
    }
}

ThemeService::Theme ThemeService::parseThemeFromString(const std::string& themeString) const {
    if (themeString == "base") return Theme::BASE;
    if (themeString == "light") return Theme::LIGHT;
    if (themeString == "dark") return Theme::DARK;
    if (themeString == "warm") return Theme::WARM;
    if (themeString == "cool") return Theme::COOL;
    if (themeString == "auto") return Theme::AUTO;
    return Theme::BASE;
}

void ThemeService::toggleTheme() {
    switch (currentTheme_) {
        case Theme::LIGHT:
            setTheme(Theme::DARK);
            break;
        case Theme::DARK:
            setTheme(Theme::WARM);
            break;
        case Theme::WARM:
            setTheme(Theme::COOL);
            break;
        case Theme::COOL:
            setTheme(Theme::LIGHT);
            break;
        default:
            setTheme(Theme::LIGHT);
            break;
    }
}

void ThemeService::cycleTheme() {
    auto themes = getAvailableThemes();
    auto it = std::find(themes.begin(), themes.end(), currentTheme_);
    if (it != themes.end()) {
        ++it;
        if (it == themes.end()) {
            it = themes.begin();
        }
        setTheme(*it);
    } else {
        setTheme(Theme::BASE);
    }
}

void ThemeService::setAutoTheme() {
    setTheme(Theme::AUTO);
}

void ThemeService::reloadCurrentTheme() {
    Theme currentTheme = currentTheme_;
    setTheme(Theme::BASE, false);
    setTheme(currentTheme, false);
}

void ThemeService::unloadAllThemes() {
    for (const auto& cssFile : loadedCSSFiles_) {
        if (cssLoaderCallback_) {
            cssLoaderCallback_(cssFile, false);
        }
    }
    loadedCSSFiles_.clear();
    frameworkLoaded_ = false;
}

void ThemeService::applyThemeToContainer(Wt::WContainerWidget* container, Theme theme) {
    if (!container) {
        std::cerr << "[ThemeService] Cannot apply theme to null container" << std::endl;
        return;
    }
    
    Theme effectiveTheme = (theme == Theme::AUTO) ? currentTheme_ : theme;
    
    // Remove any existing theme classes
    removeThemeFromContainer(container);
    
    // Apply the new theme class
    std::string themeClass = getThemeCSSClass(effectiveTheme);
    container->addStyleClass(themeClass);
    
    // Apply additional theme-specific styling
    std::string additionalClasses;
    switch (effectiveTheme) {
        case Theme::DARK:
            additionalClasses = "bg-dark text-light";
            break;
        case Theme::LIGHT:
            additionalClasses = "bg-light text-dark";
            break;
        case Theme::WARM:
            additionalClasses = "theme-warm-bg theme-warm-text";
            break;
        case Theme::COOL:
            additionalClasses = "theme-cool-bg theme-cool-text";
            break;
        case Theme::BASE:
        case Theme::AUTO:
        default:
            additionalClasses = "bg-white text-dark";
            break;
    }
    
    if (!additionalClasses.empty()) {
        container->addStyleClass(additionalClasses);
    }
    
    std::cout << "[ThemeService] Applied theme " << getThemeName(effectiveTheme) 
              << " to container with classes: " << themeClass << " " << additionalClasses << std::endl;
}

void ThemeService::removeThemeFromContainer(Wt::WContainerWidget* container) {
    if (!container) {
        return;
    }
    
    // Remove all theme classes
    for (const auto& pair : themeCSSClasses_) {
        container->removeStyleClass(pair.second);
    }
    
    // Remove additional styling classes
    std::vector<std::string> stylesToRemove = {
        "bg-dark", "text-light", "bg-light", "text-dark",
        "bg-white", "text-dark", "theme-warm-bg", "theme-warm-text",
        "theme-cool-bg", "theme-cool-text"
    };
    
    for (const auto& styleClass : stylesToRemove) {
        container->removeStyleClass(styleClass);
    }
}

std::map<std::string, std::string> ThemeService::getThemeCSSVariables() const {
    std::map<std::string, std::string> variables;
    
    switch (currentTheme_) {
        case Theme::LIGHT:
            variables["--primary-bg"] = "#ffffff";
            variables["--primary-text"] = "#212529";
            variables["--secondary-bg"] = "#f8f9fa";
            variables["--accent-color"] = "#0066cc";
            break;
        case Theme::DARK:
            variables["--primary-bg"] = "#212529";
            variables["--primary-text"] = "#ffffff";
            variables["--secondary-bg"] = "#343a40";
            variables["--accent-color"] = "#4dabf7";
            break;
        case Theme::WARM:
            variables["--primary-bg"] = "#fdf6e3";
            variables["--primary-text"] = "#5d4e37";
            variables["--secondary-bg"] = "#f4f1e8";
            variables["--accent-color"] = "#8b4513";
            break;
        case Theme::COOL:
            variables["--primary-bg"] = "#f0f8ff";
            variables["--primary-text"] = "#1e3a8a";
            variables["--secondary-bg"] = "#e6f3ff";
            variables["--accent-color"] = "#1e40af";
            break;
        case Theme::BASE:
        case Theme::AUTO:
        default:
            variables["--primary-bg"] = "#ffffff";
            variables["--primary-text"] = "#212529";
            variables["--secondary-bg"] = "#f8f9fa";
            variables["--accent-color"] = "#007bff";
            break;
    }
    
    return variables;
}

void ThemeService::injectThemeCSS() {
    if (!app_) return;
    
    auto variables = getThemeCSSVariables();
    std::string cssVariables = ":root { ";
    
    for (const auto& pair : variables) {
        cssVariables += pair.first + ": " + pair.second + "; ";
    }
    cssVariables += "}";
    
    // Inject CSS variables via JavaScript
    std::string script = 
        "var style = document.createElement('style');"
        "style.textContent = '" + cssVariables + "';"
        "document.head.appendChild(style);";
    
    app_->doJavaScript(script);
    
    std::cout << "[ThemeService] Injected CSS variables for theme: " << getThemeName(currentTheme_) << std::endl;
}

bool ThemeService::isThemeDark(Theme theme) const {
    return theme == Theme::DARK;
}

double ThemeService::getThemeContrastRatio(Theme theme) const {
    // Simplified contrast ratio calculation
    // In a real implementation, you'd calculate based on actual colors
    switch (theme) {
        case Theme::LIGHT:
            return 7.0; // High contrast
        case Theme::DARK:
            return 6.5; // Good contrast
        case Theme::WARM:
            return 5.8; // Medium contrast
        case Theme::COOL:
            return 6.2; // Good contrast
        case Theme::BASE:
        case Theme::AUTO:
        default:
            return 6.0; // Standard contrast
    }
}

bool ThemeService::isThemeAccessible(Theme theme) const {
    // WCAG AA standard requires contrast ratio of at least 4.5:1
    return getThemeContrastRatio(theme) >= 4.5;
}

// Simplified ThemeUtils namespace (no widget dependencies)
namespace ThemeUtils {

ThemeService::Theme getRecommendedTheme(const std::string& context) {
    if (context == "restaurant") {
        return ThemeService::Theme::WARM;
    } else if (context == "office" || context == "tech") {
        return ThemeService::Theme::COOL;
    }
    return ThemeService::Theme::BASE;
}

bool shouldAnimateThemeTransitions() {
    return true; // Enable animations by default
}

void applyThemeTransition(Wt::WApplication* app, int duration) {
    if (!app) return;
    
    std::string script = 
        "document.body.style.transition = 'all " + std::to_string(duration) + "ms ease';"
        "setTimeout(function() { document.body.style.transition = ''; }, " + std::to_string(duration) + ");";
    
    app->doJavaScript(script);
}

ThemeService::CSSLoaderCallback createCSSLoader(Wt::WApplication* app) {
    return [app](const std::string& cssPath, bool load) {
        if (load) {
            app->useStyleSheet(cssPath);
            std::cout << "[CSSLoader] Loaded: " << cssPath << std::endl;
        } else {
            // Note: Wt doesn't provide a way to unload stylesheets dynamically
            std::cout << "[CSSLoader] Marked for unload: " << cssPath << std::endl;
        }
    };
}

} // namespace ThemeUtils
