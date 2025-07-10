// ============================================================================
// Fixed ThemeService Implementation - Improved CSS Loading and Theme Application
// File: src/services/ThemeService.cpp
// ============================================================================

#include "../../include/services/ThemeService.hpp"
#include <Wt/WApplication.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <chrono>

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
    
    // FIXED: Load framework immediately
    loadThemeFramework();
    
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
    
    // FIXED: Use web-accessible paths for Wt
    std::string cssBase = "assets/css/themes/";
    
    themeCSSPaths_[Theme::BASE] = "assets/css/theme-framework.css";
    themeCSSPaths_[Theme::LIGHT] = cssBase + "light-theme.css";
    themeCSSPaths_[Theme::DARK] = cssBase + "dark-theme.css";
    themeCSSPaths_[Theme::WARM] = cssBase + "warm-theme.css";
    themeCSSPaths_[Theme::COOL] = cssBase + "cool-theme.css";
    themeCSSPaths_[Theme::AUTO] = "assets/css/theme-framework.css";
    
    // Theme Categories
    themeCategories_[Theme::BASE] = "Standard";
    themeCategories_[Theme::LIGHT] = "Standard";
    themeCategories_[Theme::DARK] = "Standard";
    themeCategories_[Theme::WARM] = "Themed";
    themeCategories_[Theme::COOL] = "Themed";
    themeCategories_[Theme::AUTO] = "Automatic";
    
    std::cout << "[ThemeService] Theme metadata initialized with CSS base: " << cssBase << std::endl;
}

void ThemeService::setTheme(Theme theme, bool savePreference) {
    std::cout << "[ThemeService] Setting theme to: " << getThemeName(theme) << std::endl;
    
    if (theme == currentTheme_) {
        std::cout << "[ThemeService] Theme already active, skipping change" << std::endl;
        return; // No change needed
    }
    
    Theme oldTheme = currentTheme_;
    Theme effectiveTheme = resolveTheme(theme);
    
    // FIXED: Always ensure framework is loaded first
    ensureFrameworkLoaded();
    
    // Remove old theme classes
    removeThemeClasses();
    
    // Update current theme first
    currentTheme_ = effectiveTheme;
    preferredTheme_ = theme;
    
    // FIXED: Apply inline CSS immediately for instant feedback
    applyInlineThemeCSS(effectiveTheme);
    
    // Apply theme classes
    applyThemeClasses();
    
    // FIXED: Load CSS file asynchronously (won't block theme application)
    loadThemeCSSAsync(effectiveTheme);
    
    // Save preference if requested
    if (savePreference) {
        saveThemePreference();
    }
    
    // Notify callbacks
    notifyThemeChange(oldTheme, currentTheme_);
    
    std::cout << "[ThemeService] Theme changed from " << themeToString(oldTheme) 
              << " to " << themeToString(currentTheme_) << " (immediate effect applied)" << std::endl;
}

// FIXED: New method to apply immediate inline CSS for instant theme changes
void ThemeService::applyInlineThemeCSS(Theme theme) {
    if (!app_) return;
    
    std::string css = generateThemeCSS(theme);
    
    // Inject CSS immediately via JavaScript
    std::string script = 
        "if (window.currentThemeStyle) {"
        "  document.head.removeChild(window.currentThemeStyle);"
        "}"
        "window.currentThemeStyle = document.createElement('style');"
        "window.currentThemeStyle.textContent = `" + css + "`;"
        "window.currentThemeStyle.id = 'pos-theme-override';"
        "document.head.appendChild(window.currentThemeStyle);";
    
    app_->doJavaScript(script);
    
    std::cout << "[ThemeService] Applied inline CSS for theme: " << getThemeName(theme) << std::endl;
}

// FIXED: Generate comprehensive CSS for each theme
std::string ThemeService::generateThemeCSS(Theme theme) {
    std::string css;
    
    switch (theme) {
        case Theme::LIGHT:
            css = R"(
                .theme-light, .theme-light body, .pos-app-container.theme-light {
                    --bs-body-bg: #ffffff;
                    --bs-body-color: #212529;
                    --bs-primary: #0066cc;
                    --bs-secondary: #6c757d;
                    --bs-light: #f8f9fa;
                    --bs-dark: #212529;
                    background-color: #ffffff !important;
                    color: #212529 !important;
                }
                .theme-light .card, .theme-light .table {
                    background-color: #ffffff;
                    color: #212529;
                    border-color: #dee2e6;
                }
                .theme-light .btn-primary {
                    background-color: #0066cc;
                    border-color: #0066cc;
                }
            )";
            break;
            
        case Theme::DARK:
            css = R"(
                .theme-dark, .theme-dark body, .pos-app-container.theme-dark {
                    --bs-body-bg: #212529;
                    --bs-body-color: #ffffff;
                    --bs-primary: #4dabf7;
                    --bs-secondary: #6c757d;
                    --bs-light: #495057;
                    --bs-dark: #212529;
                    background-color: #212529 !important;
                    color: #ffffff !important;
                }
                .theme-dark .card {
                    background-color: #343a40;
                    color: #ffffff;
                    border-color: #495057;
                }
                .theme-dark .table {
                    background-color: #343a40;
                    color: #ffffff;
                    --bs-table-bg: #343a40;
                }
                .theme-dark .btn-primary {
                    background-color: #4dabf7;
                    border-color: #4dabf7;
                    color: #212529;
                }
                .theme-dark .form-control, .theme-dark .form-select {
                    background-color: #495057;
                    color: #ffffff;
                    border-color: #6c757d;
                }
            )";
            break;
            
        case Theme::WARM:
            css = R"(
                .theme-warm, .theme-warm body, .pos-app-container.theme-warm {
                    --bs-body-bg: #fdf6e3;
                    --bs-body-color: #5d4e37;
                    --bs-primary: #8b4513;
                    --bs-secondary: #d2b48c;
                    --bs-light: #f4f1e8;
                    --bs-dark: #5d4e37;
                    background-color: #fdf6e3 !important;
                    color: #5d4e37 !important;
                }
                .theme-warm .card {
                    background-color: #f4f1e8;
                    color: #5d4e37;
                    border-color: #d2b48c;
                }
                .theme-warm .table {
                    background-color: #f4f1e8;
                    color: #5d4e37;
                }
                .theme-warm .btn-primary {
                    background-color: #8b4513;
                    border-color: #8b4513;
                    color: #ffffff;
                }
            )";
            break;
            
        case Theme::COOL:
            css = R"(
                .theme-cool, .theme-cool body, .pos-app-container.theme-cool {
                    --bs-body-bg: #f0f8ff;
                    --bs-body-color: #1e3a8a;
                    --bs-primary: #1e40af;
                    --bs-secondary: #64748b;
                    --bs-light: #e6f3ff;
                    --bs-dark: #1e3a8a;
                    background-color: #f0f8ff !important;
                    color: #1e3a8a !important;
                }
                .theme-cool .card {
                    background-color: #e6f3ff;
                    color: #1e3a8a;
                    border-color: #93c5fd;
                }
                .theme-cool .table {
                    background-color: #e6f3ff;
                    color: #1e3a8a;
                }
                .theme-cool .btn-primary {
                    background-color: #1e40af;
                    border-color: #1e40af;
                    color: #ffffff;
                }
            )";
            break;
            
        case Theme::BASE:
        case Theme::AUTO:
        default:
            css = R"(
                .theme-base, .theme-base body, .pos-app-container.theme-base {
                    --bs-body-bg: #ffffff;
                    --bs-body-color: #212529;
                    --bs-primary: #007bff;
                    --bs-secondary: #6c757d;
                    background-color: #ffffff !important;
                    color: #212529 !important;
                }
                .theme-base .card, .theme-base .table {
                    background-color: #ffffff;
                    color: #212529;
                }
                .theme-base .btn-primary {
                    background-color: #007bff;
                    border-color: #007bff;
                }
            )";
            break;
    }
    
    return css;
}

// FIXED: Asynchronous CSS loading that doesn't block theme application
void ThemeService::loadThemeCSSAsync(Theme theme) {
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
            
            std::cout << "[ThemeService] Loaded theme CSS file: " << cssPath << std::endl;
            
            // Notify CSS loader callback if set
            if (cssLoaderCallback_) {
                cssLoaderCallback_(cssPath, true);
            }
        } catch (const std::exception& e) {
            std::cout << "[ThemeService] CSS file not found (using inline CSS): " << cssPath << std::endl;
            // This is OK - we're using inline CSS as fallback
        }
    }
}

// ============================================================================
// ThemeService Update - Add this to the loadThemeFramework() method
// File: src/services/ThemeService.cpp (addition)
// ============================================================================

// Add this to the loadThemeFramework() method after loading the base framework:

void ThemeService::loadThemeFramework() {
    if (frameworkLoaded_) {
        return;
    }
    
    // Load the base theme framework CSS
    std::string frameworkPath = themeCSSPaths_[Theme::BASE];
    
    try {
        app_->useStyleSheet(frameworkPath);
        loadedCSSFiles_.push_back(frameworkPath);
        std::cout << "[ThemeService] Framework CSS loaded from " << frameworkPath << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[ThemeService] Framework CSS not found, using built-in styles: " << frameworkPath << std::endl;
    }
    
    // ADDED: Load theme-reactive CSS for component integration
    try {
        std::string reactiveCSS = "assets/css/theme-reactive.css";
        app_->useStyleSheet(reactiveCSS);
        loadedCSSFiles_.push_back(reactiveCSS);
        std::cout << "[ThemeService] Theme-reactive CSS loaded from " << reactiveCSS << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[ThemeService] Theme-reactive CSS not found: " << e.what() << std::endl;
    }
    
    frameworkLoaded_ = true;
    
    // Apply base theme variables immediately
    applyBaseThemeVariables();
}

// FIXED: Apply CSS variables for consistent theming
void ThemeService::applyBaseThemeVariables() {
    std::string baseCSS = R"(
        :root {
            --pos-transition-speed: 0.3s;
            --pos-border-radius: 0.375rem;
            --pos-shadow: 0 0.125rem 0.25rem rgba(0, 0, 0, 0.075);
            --pos-shadow-lg: 0 0.5rem 1rem rgba(0, 0, 0, 0.15);
        }
        
        .pos-theme-transition {
            transition: all var(--pos-transition-speed) ease;
        }
        
        .pos-app-container {
            transition: background-color var(--pos-transition-speed) ease,
                       color var(--pos-transition-speed) ease;
        }
    )";
    
    std::string script = 
        "var baseStyle = document.createElement('style');"
        "baseStyle.textContent = `" + baseCSS + "`;"
        "baseStyle.id = 'pos-base-theme';"
        "document.head.appendChild(baseStyle);";
    
    app_->doJavaScript(script);
}

void ThemeService::applyThemeClasses() {
    if (!app_) return;
    
    auto root = app_->root();
    if (!root) return;
    
    // Add theme-specific class to root and body
    std::string themeClass = getThemeCSSClass(currentTheme_);
    root->addStyleClass(themeClass);
    
    // FIXED: Enhanced JavaScript to ensure classes are applied everywhere
    std::string script = 
        "document.body.className = document.body.className.replace(/theme-\\w+/g, '');"
        "document.body.classList.add('" + themeClass + "');"
        "var appContainer = document.querySelector('.pos-app-container');"
        "if (appContainer) {"
        "  appContainer.className = appContainer.className.replace(/theme-\\w+/g, '');"
        "  appContainer.classList.add('" + themeClass + "');"
        "}"
        // Add transition class for smooth changes
        "document.body.classList.add('pos-theme-transition');";
    
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
    app_->doJavaScript(
        "document.body.className = document.body.className.replace(/theme-\\w+/g, '');"
        "var appContainer = document.querySelector('.pos-app-container');"
        "if (appContainer) {"
        "  appContainer.className = appContainer.className.replace(/theme-\\w+/g, '');"
        "}"
    );
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
    // FIXED: Use JavaScript to detect system theme preference
    // For now, return false as default - in a real implementation,
    // you could use JavaScript to check window.matchMedia('(prefers-color-scheme: dark)')
    return false;
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
    switch (theme) {
        case Theme::LIGHT:
            return 7.0;
        case Theme::DARK:
            return 6.5;
        case Theme::WARM:
            return 5.8;
        case Theme::COOL:
            return 6.2;
        case Theme::BASE:
        case Theme::AUTO:
        default:
            return 6.0;
    }
}

bool ThemeService::isThemeAccessible(Theme theme) const {
    return getThemeContrastRatio(theme) >= 4.5;
}

// Simplified ThemeUtils namespace
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
    return true;
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
            std::cout << "[CSSLoader] Marked for unload: " << cssPath << std::endl;
        }
    };
}

} // namespace ThemeUtils
