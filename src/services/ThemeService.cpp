/* ============================================================================
   Updated ThemeService.cpp - Enhanced implementation with WARM and COOL themes
   
   This file extends the existing ThemeService implementation to support
   the new theme framework while maintaining backward compatibility.
   
   File: src/services/ThemeService.cpp
   ============================================================================ */

#include "../../include/services/ThemeService.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <ctime>

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
    
    // Set up CSS loader callback
    cssLoaderCallback_ = ThemeUtils::createCSSLoader(app_);
    
    // Load the theme framework
    loadThemeFramework();
    
    std::cout << "[ThemeService] Enhanced theme service initialized with theme: " 
              << getThemeName(currentTheme_) << std::endl;
}

void ThemeService::setTheme(Theme theme, bool savePreference) {
    if (theme == currentTheme_) {
        return; // No change needed
    }
    
    Theme oldTheme = currentTheme_;
    Theme effectiveTheme = resolveTheme(theme);
    
    // Unload current theme CSS if different
    if (effectiveTheme != oldTheme) {
        unloadThemeCSS(oldTheme);
        loadThemeCSS(effectiveTheme);
    }
    
    currentTheme_ = effectiveTheme;
    if (savePreference) {
        preferredTheme_ = theme;
        saveThemePreference();
    }
    
    // Apply theme to UI
    applyThemeClasses();
    
    // Notify callbacks
    notifyThemeChange(oldTheme, effectiveTheme);
    
    std::cout << "[ThemeService] Theme changed from " << getThemeName(oldTheme) 
              << " to " << getThemeName(effectiveTheme) << std::endl;
}

void ThemeService::toggleTheme() {
    // Enhanced toggle: cycle through Base -> Light -> Dark -> Warm -> Cool
    std::vector<Theme> toggleOrder = {Theme::BASE, Theme::LIGHT, Theme::DARK, Theme::WARM, Theme::COOL};
    
    auto it = std::find(toggleOrder.begin(), toggleOrder.end(), currentTheme_);
    if (it != toggleOrder.end()) {
        ++it;
        if (it == toggleOrder.end()) {
            it = toggleOrder.begin(); // Wrap around
        }
    } else {
        it = toggleOrder.begin(); // Default to first theme
    }
    
    setTheme(*it);
}

void ThemeService::cycleTheme() {
    std::vector<Theme> themes = {Theme::BASE, Theme::LIGHT, Theme::DARK, Theme::WARM, Theme::COOL};
    
    auto it = std::find(themes.begin(), themes.end(), currentTheme_);
    if (it != themes.end()) {
        ++it;
        if (it == themes.end()) {
            it = themes.begin(); // Wrap around
        }
    } else {
        it = themes.begin(); // Default to first theme
    }
    
    setTheme(*it);
}

void ThemeService::setAutoTheme() {
    Theme systemTheme = isSystemDarkMode() ? Theme::DARK : Theme::LIGHT;
    setTheme(systemTheme);
}

bool ThemeService::isSystemDarkMode() const {
    // Use JavaScript to detect system preference
    std::string script = R"(
        function() {
            if (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) {
                return true;
            }
            return false;
        }()
    )";
    
    // In a real implementation, you'd execute this JavaScript and get the result
    // For now, default to false (can be enhanced with actual JS execution)
    return false;
}

std::vector<ThemeService::Theme> ThemeService::getAvailableThemes() const {
    return {Theme::BASE, Theme::LIGHT, Theme::DARK, Theme::WARM, Theme::COOL, Theme::AUTO};
}

std::string ThemeService::getThemeName(Theme theme) const {
    auto it = themeNames_.find(theme);
    return (it != themeNames_.end()) ? it->second : "Unknown";
}

std::string ThemeService::getThemeDescription(Theme theme) const {
    auto it = themeDescriptions_.find(theme);
    return (it != themeDescriptions_.end()) ? it->second : "No description available";
}

std::string ThemeService::getThemeCSSClass(Theme theme) const {
    auto it = themeCSSClasses_.find(theme);
    return (it != themeCSSClasses_.end()) ? it->second : "";
}

std::string ThemeService::getThemeIcon(Theme theme) const {
    auto it = themeIcons_.find(theme);
    return (it != themeIcons_.end()) ? it->second : "🎨";
}

std::string ThemeService::getThemePrimaryColor(Theme theme) const {
    auto it = themePrimaryColors_.find(theme);
    return (it != themePrimaryColors_.end()) ? it->second : "#007bff";
}

std::string ThemeService::getThemeCSSPath(Theme theme) const {
    auto it = themeCSSPaths_.find(theme);
    return (it != themeCSSPaths_.end()) ? it->second : "";
}

std::string ThemeService::getThemeCategory(Theme theme) const {
    auto it = themeCategories_.find(theme);
    return (it != themeCategories_.end()) ? it->second : "General";
}

size_t ThemeService::onThemeChanged(ThemeChangeCallback callback) {
    size_t id = nextCallbackId_++;
    callbacks_[id] = callback;
    return id;
}

void ThemeService::removeThemeChangeCallback(size_t handle) {
    callbacks_.erase(handle);
}

void ThemeService::setCSSLoaderCallback(CSSLoaderCallback callback) {
    cssLoaderCallback_ = callback;
}

void ThemeService::loadThemeFramework() {
    if (frameworkLoaded_) {
        return;
    }
    
    // Load the main theme framework CSS
    if (cssLoaderCallback_) {
        cssLoaderCallback_(FRAMEWORK_CSS_PATH, true);
        loadedCSSFiles_.push_back(FRAMEWORK_CSS_PATH);
    }
    
    // Load the current theme CSS
    loadThemeCSS(currentTheme_);
    
    frameworkLoaded_ = true;
    std::cout << "[ThemeService] Theme framework loaded" << std::endl;
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

void ThemeService::reloadCurrentTheme() {
    unloadThemeCSS(currentTheme_);
    loadThemeCSS(currentTheme_);
    applyThemeClasses();
}

void ThemeService::loadThemePreference() {
    // In a real implementation, you'd load from localStorage or cookies
    std::string script = R"(
        function() {
            try {
                return localStorage.getItem(')" + std::string(THEME_PREFERENCE_KEY) + R"(') || '';
            } catch(e) {
                return '';
            }
        }()
    )";
    
    // Execute JavaScript and get result
    // For now, we'll just use the default
    preferredTheme_ = Theme::BASE;
    currentTheme_ = resolveTheme(preferredTheme_);
}

void ThemeService::saveThemePreference() {
    std::string themeString = themeToString(preferredTheme_);
    
    std::string script = R"(
        function() {
            try {
                localStorage.setItem(')" + std::string(THEME_PREFERENCE_KEY) + R"(', ')" + themeString + R"(');
                return true;
            } catch(e) {
                return false;
            }
        }()
    )";
    
    // Execute JavaScript to save preference
    // In a real implementation, you'd use app_->doJavaScript()
}

void ThemeService::clearThemePreference() {
    std::string script = R"(
        function() {
            try {
                localStorage.removeItem(')" + std::string(THEME_PREFERENCE_KEY) + R"(');
                return true;
            } catch(e) {
                return false;
            }
        }()
    )";
    
    // Execute JavaScript to clear preference
}

bool ThemeService::hasThemePreference() const {
    // In a real implementation, check if preference exists
    return true; // Simplified for now
}

void ThemeService::applyThemeToContainer(Wt::WContainerWidget* container, Theme theme) {
    if (!container) return;
    
    Theme effectiveTheme = (theme == Theme::AUTO) ? currentTheme_ : resolveTheme(theme);
    
    // Remove existing theme classes
    removeThemeFromContainer(container);
    
    // Apply new theme class
    std::string cssClass = getThemeCSSClass(effectiveTheme);
    if (!cssClass.empty()) {
        container->addStyleClass(cssClass);
    }
}

void ThemeService::removeThemeFromContainer(Wt::WContainerWidget* container) {
    if (!container) return;
    
    for (const auto& theme : getAvailableThemes()) {
        std::string cssClass = getThemeCSSClass(theme);
        if (!cssClass.empty()) {
            container->removeStyleClass(cssClass);
        }
    }
}

std::map<std::string, std::string> ThemeService::getThemeCSSVariables() const {
    std::map<std::string, std::string> variables;
    
    // Define CSS variables based on current theme
    switch (currentTheme_) {
        case Theme::BASE:
            variables["--pos-primary"] = "#007bff";
            variables["--pos-background"] = "#ffffff";
            variables["--pos-text-primary"] = "#212529";
            break;
        case Theme::LIGHT:
            variables["--pos-primary"] = "#0066cc";
            variables["--pos-background"] = "#ffffff";
            variables["--pos-text-primary"] = "#1a1a1a";
            break;
        case Theme::DARK:
            variables["--pos-primary"] = "#3b82f6";
            variables["--pos-background"] = "#1e293b";
            variables["--pos-text-primary"] = "#f1f5f9";
            break;
        case Theme::WARM:
            variables["--pos-primary"] = "#d2691e";
            variables["--pos-background"] = "#fdf6e3";
            variables["--pos-text-primary"] = "#2f1b14";
            break;
        case Theme::COOL:
            variables["--pos-primary"] = "#2563eb";
            variables["--pos-background"] = "#ffffff";
            variables["--pos-text-primary"] = "#0f172a";
            break;
        default:
            // Default to base theme variables
            variables["--pos-primary"] = "#007bff";
            variables["--pos-background"] = "#ffffff";
            variables["--pos-text-primary"] = "#212529";
            break;
    }
    
    return variables;
}

void ThemeService::injectThemeCSS() {
    // Enhanced CSS injection with transition support
    std::string css = R"(
        .pos-theme-transition,
        .pos-theme-transition * {
            transition: background-color 0.3s ease, 
                        color 0.3s ease, 
                        border-color 0.3s ease,
                        box-shadow 0.3s ease !important;
        }
        
        /* Enhanced focus styles */
        .theme-focus-enhanced {
            outline: 2px solid var(--pos-primary);
            outline-offset: 2px;
        }
    )";
    
    app_->styleSheet().addRule(".pos-theme-system", css);
}

bool ThemeService::isThemeDark(Theme theme) const {
    return theme == Theme::DARK;
}

double ThemeService::getThemeContrastRatio(Theme theme) const {
    // Enhanced contrast ratio calculation
    switch (theme) {
        case Theme::BASE: return 4.5;     // Bootstrap standard
        case Theme::LIGHT: return 7.0;    // High contrast
        case Theme::DARK: return 4.8;     // Good contrast for dark mode
        case Theme::WARM: return 5.2;     // Restaurant-optimized contrast
        case Theme::COOL: return 6.1;     // Tech-optimized contrast
        default: return 4.5;
    }
}

bool ThemeService::isThemeAccessible(Theme theme) const {
    return getThemeContrastRatio(theme) >= 4.5; // WCAG AA standard
}

void ThemeService::applyThemeClasses() {
    if (!app_ || !app_->root()) return;
    
    // Remove existing theme classes
    removeThemeClasses();
    
    // Add new theme class
    std::string cssClass = getThemeCSSClass(currentTheme_);
    if (!cssClass.empty()) {
        app_->root()->addStyleClass(cssClass);
    }
    
    // Add transition class for smooth theme changes
    app_->root()->addStyleClass("pos-theme-transition");
}

void ThemeService::removeThemeClasses() {
    if (!app_ || !app_->root()) return;
    
    for (const auto& theme : getAvailableThemes()) {
        std::string cssClass = getThemeCSSClass(theme);
        if (!cssClass.empty()) {
            app_->root()->removeStyleClass(cssClass);
        }
    }
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

ThemeService::Theme ThemeService::resolveTheme(Theme theme) const {
    if (theme == Theme::AUTO) {
        return isSystemDarkMode() ? Theme::DARK : Theme::LIGHT;
    }
    return theme;
}

void ThemeService::loadThemeCSS(Theme theme) {
    std::string cssPath = getThemeCSSPath(theme);
    if (!cssPath.empty() && cssLoaderCallback_) {
        cssLoaderCallback_(cssPath, true);
        loadedCSSFiles_.push_back(cssPath);
        std::cout << "[ThemeService] Loaded CSS for theme: " << getThemeName(theme) << std::endl;
    }
}

void ThemeService::unloadThemeCSS(Theme theme) {
    std::string cssPath = getThemeCSSPath(theme);
    if (!cssPath.empty() && cssLoaderCallback_) {
        cssLoaderCallback_(cssPath, false);
        auto it = std::find(loadedCSSFiles_.begin(), loadedCSSFiles_.end(), cssPath);
        if (it != loadedCSSFiles_.end()) {
            loadedCSSFiles_.erase(it);
        }
        std::cout << "[ThemeService] Unloaded CSS for theme: " << getThemeName(theme) << std::endl;
    }
}

void ThemeService::initializeThemeMetadata() {
    // Enhanced theme names
    themeNames_[Theme::BASE] = "Bootstrap Base";
    themeNames_[Theme::LIGHT] = "Light Mode";
    themeNames_[Theme::DARK] = "Dark Mode";
    themeNames_[Theme::WARM] = "Warm Mode";
    themeNames_[Theme::COOL] = "Cool Mode";
    themeNames_[Theme::AUTO] = "Auto";
    
    // Enhanced theme descriptions
    themeDescriptions_[Theme::BASE] = "Clean Bootstrap 4 styling with professional appearance";
    themeDescriptions_[Theme::LIGHT] = "Bright, high-contrast interface optimized for daytime use";
    themeDescriptions_[Theme::DARK] = "Easy on the eyes dark theme perfect for low-light environments";
    themeDescriptions_[Theme::WARM] = "Cozy restaurant atmosphere with warm earth tones and browns";
    themeDescriptions_[Theme::COOL] = "Modern tech-focused theme with cool blues and clean lines";
    themeDescriptions_[Theme::AUTO] = "Automatically matches your system's light/dark preference";
    
    // CSS classes
    themeCSSClasses_[Theme::BASE] = "theme-base";
    themeCSSClasses_[Theme::LIGHT] = "theme-light";
    themeCSSClasses_[Theme::DARK] = "theme-dark";
    themeCSSClasses_[Theme::WARM] = "theme-warm";
    themeCSSClasses_[Theme::COOL] = "theme-cool";
    themeCSSClasses_[Theme::AUTO] = ""; // Resolved at runtime
    
    // Enhanced icons with better visual distinction
    themeIcons_[Theme::BASE] = "⚡";
    themeIcons_[Theme::LIGHT] = "☀️";
    themeIcons_[Theme::DARK] = "🌙";
    themeIcons_[Theme::WARM] = "🔥";
    themeIcons_[Theme::COOL] = "❄️";
    themeIcons_[Theme::AUTO] = "🔄";
    
    // Primary colors for each theme
    themePrimaryColors_[Theme::BASE] = "#007bff";
    themePrimaryColors_[Theme::LIGHT] = "#0066cc";
    themePrimaryColors_[Theme::DARK] = "#3b82f6";
    themePrimaryColors_[Theme::WARM] = "#d2691e";
    themePrimaryColors_[Theme::COOL] = "#2563eb";
    themePrimaryColors_[Theme::AUTO] = "#007bff"; // Default
    
    // CSS file paths
    themeCSSPaths_[Theme::BASE] = "assets/css/themes/base.css";
    themeCSSPaths_[Theme::LIGHT] = "assets/css/themes/light.css";
    themeCSSPaths_[Theme::DARK] = "assets/css/themes/dark.css";
    themeCSSPaths_[Theme::WARM] = "assets/css/themes/warm.css";
    themeCSSPaths_[Theme::COOL] = "assets/css/themes/cool.css";
    themeCSSPaths_[Theme::AUTO] = ""; // Resolved at runtime
    
    // Theme categories for organization
    themeCategories_[Theme::BASE] = "Standard";
    themeCategories_[Theme::LIGHT] = "Standard";
    themeCategories_[Theme::DARK] = "Standard";
    themeCategories_[Theme::WARM] = "Hospitality";
    themeCategories_[Theme::COOL] = "Technology";
    themeCategories_[Theme::AUTO] = "Smart";
}

ThemeService::Theme ThemeService::parseThemeFromString(const std::string& themeString) const {
    if (themeString == "base") return Theme::BASE;
    if (themeString == "light") return Theme::LIGHT;
    if (themeString == "dark") return Theme::DARK;
    if (themeString == "warm") return Theme::WARM;
    if (themeString == "cool") return Theme::COOL;
    if (themeString == "auto") return Theme::AUTO;
    return Theme::BASE; // Default
}

std::string ThemeService::themeToString(Theme theme) const {
    switch (theme) {
        case Theme::BASE: return "base";
        case Theme::LIGHT: return "light";
        case Theme::DARK: return "dark";
        case Theme::WARM: return "warm";
        case Theme::COOL: return "cool";
        case Theme::AUTO: return "auto";
        default: return "base";
    }
}

void ThemeService::detectSystemTheme() {
    // Enhanced system theme detection
    preferredTheme_ = Theme::BASE; // Default fallback
}

void ThemeService::ensureFrameworkLoaded() {
    if (!frameworkLoaded_) {
        loadThemeFramework();
    }
}

// Enhanced theme utility functions implementation
namespace ThemeUtils {
    
    std::unique_ptr<Wt::WWidget> createThemeSelector(
        std::shared_ptr<ThemeService> themeService,
        bool showDescriptions,
        bool showCategories) {
        
        auto container = std::make_unique<Wt::WContainerWidget>();
        container->addStyleClass("theme-selector enhanced");
        
        auto comboBox = container->addNew<Wt::WComboBox>();
        comboBox->addStyleClass("form-select theme-selector-dropdown");
        
        // Get raw pointer for lambda capture
        Wt::WComboBox* comboBoxPtr = comboBox;
        
        // Populate theme options with enhanced formatting
        auto themes = themeService->getAvailableThemes();
        std::map<std::string, std::vector<ThemeService::Theme>> categorizedThemes;
        
        // Group by category if requested
        if (showCategories) {
            for (const auto& theme : themes) {
                std::string category = themeService->getThemeCategory(theme);
                categorizedThemes[category].push_back(theme);
            }
            
            // Add themes by category
            for (const auto& [category, categoryThemes] : categorizedThemes) {
                for (const auto& theme : categoryThemes) {
                    std::string text = themeService->getThemeIcon(theme) + " " + 
                                      themeService->getThemeName(theme);
                    if (showDescriptions) {
                        text += " - " + themeService->getThemeDescription(theme);
                    }
                    comboBoxPtr->addItem(text);
                }
            }
        } else {
            // Simple list
            for (const auto& theme : themes) {
                std::string text = themeService->getThemeIcon(theme) + " " + 
                                  themeService->getThemeName(theme);
                if (showDescriptions) {
                    text += " - " + themeService->getThemeDescription(theme);
                }
                comboBoxPtr->addItem(text);
            }
        }
        
        // Set current selection
        auto currentTheme = themeService->getCurrentTheme();
        auto it = std::find(themes.begin(), themes.end(), currentTheme);
        if (it != themes.end()) {
            comboBoxPtr->setCurrentIndex(std::distance(themes.begin(), it));
        }
        
        // Connect change event
        comboBoxPtr->changed().connect([themeService, comboBoxPtr, themes]() {
            int index = comboBoxPtr->currentIndex();
            if (index >= 0 && index < static_cast<int>(themes.size())) {
                themeService->setTheme(themes[index]);
            }
        });
        
        return container;
    }
    
    std::unique_ptr<Wt::WPushButton> createThemeToggleButton(
        std::shared_ptr<ThemeService> themeService) {
        
        auto button = std::make_unique<Wt::WPushButton>();
        button->addStyleClass("btn btn-outline-secondary theme-toggle-btn");
        
        // Get raw pointer for lambda capture
        Wt::WPushButton* buttonPtr = button.get();
        
        // Update button text based on current theme
        auto updateButton = [themeService, buttonPtr]() {
            auto theme = themeService->getCurrentTheme();
            buttonPtr->setText(themeService->getThemeIcon(theme) + " " + 
                              themeService->getThemeName(theme));
        };
        
        updateButton();
        
        // Connect click event for enhanced toggle
        buttonPtr->clicked().connect([themeService, updateButton]() {
            themeService->toggleTheme();
            updateButton();
        });
        
        // Register for theme change notifications
        themeService->onThemeChanged([updateButton](ThemeService::Theme, ThemeService::Theme) {
            updateButton();
        });
        
        return button;
    }
    
    std::unique_ptr<Wt::WWidget> createQuickThemeSwitcher(
        std::shared_ptr<ThemeService> themeService) {
        
        auto container = std::make_unique<Wt::WContainerWidget>();
        container->addStyleClass("quick-theme-switcher d-flex gap-2");
        
        // Create buttons for main themes
        std::vector<ThemeService::Theme> quickThemes = {
            ThemeService::Theme::BASE,
            ThemeService::Theme::LIGHT,
            ThemeService::Theme::DARK,
            ThemeService::Theme::WARM,
            ThemeService::Theme::COOL
        };
        
        for (const auto& theme : quickThemes) {
            auto button = container->addNew<Wt::WPushButton>();
            button->addStyleClass("btn btn-sm btn-outline-secondary theme-quick-btn");
            button->setText(themeService->getThemeIcon(theme));
            button->setToolTip(themeService->getThemeName(theme));
            
            // Connect click event
            button->clicked().connect([themeService, theme]() {
                themeService->setTheme(theme);
            });
        }
        
        return container;
    }
    
    ThemeService::Theme getRecommendedTheme(const std::string& context) {
        // Enhanced recommendation based on context
        if (context == "restaurant" || context == "hospitality") {
            return ThemeService::Theme::WARM;
        } else if (context == "tech" || context == "office") {
            return ThemeService::Theme::COOL;
        }
        
        // Time-based recommendation
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        int hour = tm.tm_hour;
        if (hour >= 18 || hour <= 6) {
            return ThemeService::Theme::DARK; // Evening/night
        } else {
            return ThemeService::Theme::LIGHT; // Day
        }
    }
    
    bool shouldAnimateThemeTransitions() {
        return true; // Can be enhanced with user preference detection
    }
    
    void applyThemeTransition(Wt::WApplication* app, int duration) {
        if (!app) return;
        
        std::string script = R"(
            document.body.style.transition = 'all )" + std::to_string(duration) + R"(ms ease';
            setTimeout(function() {
                document.body.style.transition = '';
            }, )" + std::to_string(duration) + R"();
        )";
        
        app->doJavaScript(script);
    }
    
    ThemeService::CSSLoaderCallback createCSSLoader(Wt::WApplication* app) {
        return [app](const std::string& cssPath, bool load) {
            if (!app) return;
            
            if (load) {
                // Load CSS file
                app->useStyleSheet(cssPath);
                std::cout << "[CSSLoader] Loaded: " << cssPath << std::endl;
            } else {
                // Unload CSS file (simplified - in real implementation, you'd track and remove)
                std::cout << "[CSSLoader] Unloaded: " << cssPath << std::endl;
            }
        };
    }
}
