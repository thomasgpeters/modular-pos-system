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
    , currentTheme_(Theme::LIGHT)
    , preferredTheme_(Theme::LIGHT)
    , nextCallbackId_(1)
{
    if (!app_) {
        throw std::invalid_argument("ThemeService requires a valid WApplication instance");
    }
    
    initializeThemeMetadata();
    loadThemePreference();
    injectThemeCSS();
    
    std::cout << "[ThemeService] Initialized with theme: " << getThemeName(currentTheme_) << std::endl;
}

void ThemeService::setTheme(Theme theme, bool savePreference) {
    if (theme == currentTheme_) {
        return; // No change needed
    }
    
    Theme oldTheme = currentTheme_;
    Theme effectiveTheme = resolveTheme(theme);
    
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
    Theme newTheme = (currentTheme_ == Theme::LIGHT) ? Theme::DARK : Theme::LIGHT;
    setTheme(newTheme);
}

void ThemeService::cycleTheme() {
    std::vector<Theme> themes = {Theme::LIGHT, Theme::DARK, Theme::COLORFUL, Theme::BASE};
    
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
    // Note: In a real implementation, you'd use JavaScript to detect this
    // For now, we'll use a simple heuristic
    
    std::string script = R"(
        function() {
            if (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) {
                return true;
            }
            return false;
        }()
    )";
    
    // In a real implementation, you'd execute this JavaScript and get the result
    // For now, default to false
    return false;
}

std::vector<ThemeService::Theme> ThemeService::getAvailableThemes() const {
    return {Theme::LIGHT, Theme::DARK, Theme::COLORFUL, Theme::BASE, Theme::AUTO};
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
    return (it != themePrimaryColors_.end()) ? it->second : "#0d6efd";
}

size_t ThemeService::onThemeChanged(ThemeChangeCallback callback) {
    size_t id = nextCallbackId_++;
    callbacks_[id] = callback;
    return id;
}

void ThemeService::removeThemeChangeCallback(size_t handle) {
    callbacks_.erase(handle);
}

void ThemeService::loadThemePreference() {
    // In a real implementation, you'd load from localStorage or cookies
    // For now, we'll use the default theme
    
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
    preferredTheme_ = Theme::LIGHT;
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
        case Theme::LIGHT:
            variables["--pos-primary"] = "#0d6efd";
            variables["--pos-background"] = "#ffffff";
            variables["--pos-text-primary"] = "#212529";
            break;
        case Theme::DARK:
            variables["--pos-primary"] = "#6ea8fe";
            variables["--pos-background"] = "#212529";
            variables["--pos-text-primary"] = "#f8f9fa";
            break;
        case Theme::COLORFUL:
            variables["--pos-primary"] = "#8b5cf6";
            variables["--pos-background"] = "linear-gradient(135deg, #667eea 0%, #764ba2 100%)";
            variables["--pos-text-primary"] = "#1f2937";
            break;
        case Theme::BASE:
            variables["--pos-primary"] = "#495057";
            variables["--pos-background"] = "#ffffff";
            variables["--pos-text-primary"] = "#212529";
            break;
        default:
            // Default to light theme variables
            variables["--pos-primary"] = "#0d6efd";
            variables["--pos-background"] = "#ffffff";
            variables["--pos-text-primary"] = "#212529";
            break;
    }
    
    return variables;
}

void ThemeService::injectThemeCSS() {
    // The main CSS is loaded from the external CSS file
    // Here we just add some dynamic CSS rules
    
    std::string css = R"(
        .pos-theme-transition * {
            transition: background-color 0.3s ease, 
                        color 0.3s ease, 
                        border-color 0.3s ease,
                        box-shadow 0.3s ease !important;
        }
    )";
    
    app_->styleSheet().addRule(".pos-theme-transition *", css);
}

bool ThemeService::isThemeDark(Theme theme) const {
    return theme == Theme::DARK;
}

double ThemeService::getThemeContrastRatio(Theme theme) const {
    // Simplified contrast ratio calculation
    switch (theme) {
        case Theme::LIGHT: return 4.5;  // Good contrast
        case Theme::DARK: return 4.2;   // Good contrast  
        case Theme::COLORFUL: return 3.8; // Moderate contrast
        case Theme::BASE: return 4.1;   // Good contrast
        default: return 4.0;
    }
}

bool ThemeService::isThemeAccessible(Theme theme) const {
    return getThemeContrastRatio(theme) >= 3.0; // WCAG AA minimum
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

void ThemeService::initializeThemeMetadata() {
    // Theme names
    themeNames_[Theme::LIGHT] = "Light";
    themeNames_[Theme::DARK] = "Dark";
    themeNames_[Theme::COLORFUL] = "Colorful";
    themeNames_[Theme::BASE] = "Base";
    themeNames_[Theme::AUTO] = "Auto";
    
    // Theme descriptions
    themeDescriptions_[Theme::LIGHT] = "Clean light theme with high contrast";
    themeDescriptions_[Theme::DARK] = "Easy on the eyes dark theme";
    themeDescriptions_[Theme::COLORFUL] = "Vibrant theme with gradient backgrounds";
    themeDescriptions_[Theme::BASE] = "Minimal monochromatic theme";
    themeDescriptions_[Theme::AUTO] = "Automatically matches system preference";
    
    // CSS classes
    themeCSSClasses_[Theme::LIGHT] = "theme-light";
    themeCSSClasses_[Theme::DARK] = "theme-dark";
    themeCSSClasses_[Theme::COLORFUL] = "theme-colorful";
    themeCSSClasses_[Theme::BASE] = "theme-base";
    themeCSSClasses_[Theme::AUTO] = ""; // Resolved at runtime
    
    // Icons
    themeIcons_[Theme::LIGHT] = "☀️";
    themeIcons_[Theme::DARK] = "🌙";
    themeIcons_[Theme::COLORFUL] = "🌈";
    themeIcons_[Theme::BASE] = "⚪";
    themeIcons_[Theme::AUTO] = "🔄";
    
    // Primary colors
    themePrimaryColors_[Theme::LIGHT] = "#0d6efd";
    themePrimaryColors_[Theme::DARK] = "#6ea8fe";
    themePrimaryColors_[Theme::COLORFUL] = "#8b5cf6";
    themePrimaryColors_[Theme::BASE] = "#495057";
    themePrimaryColors_[Theme::AUTO] = "#0d6efd"; // Default
}

ThemeService::Theme ThemeService::parseThemeFromString(const std::string& themeString) const {
    if (themeString == "light") return Theme::LIGHT;
    if (themeString == "dark") return Theme::DARK;
    if (themeString == "colorful") return Theme::COLORFUL;
    if (themeString == "base") return Theme::BASE;
    if (themeString == "auto") return Theme::AUTO;
    return Theme::LIGHT; // Default
}

std::string ThemeService::themeToString(Theme theme) const {
    switch (theme) {
        case Theme::LIGHT: return "light";
        case Theme::DARK: return "dark";
        case Theme::COLORFUL: return "colorful";
        case Theme::BASE: return "base";
        case Theme::AUTO: return "auto";
        default: return "light";
    }
}

void ThemeService::detectSystemTheme() {
    // In a real implementation, this would use JavaScript to detect system theme
    // For now, we'll just default to light theme
    preferredTheme_ = Theme::LIGHT;
}

// Theme utility functions implementation
namespace ThemeUtils {
    
    std::unique_ptr<Wt::WWidget> createThemeSelector(
        std::shared_ptr<ThemeService> themeService,
        bool showDescriptions) {
        
        auto container = std::make_unique<Wt::WContainerWidget>();
        container->addStyleClass("theme-selector");
        
        auto comboBox = container->addNew<Wt::WComboBox>();
        comboBox->addStyleClass("form-select");
        
        // Get raw pointer for lambda capture (Wt manages widget lifetime)
        Wt::WComboBox* comboBoxPtr = comboBox;
        
        // Populate theme options
        for (const auto& theme : themeService->getAvailableThemes()) {
            std::string text = themeService->getThemeIcon(theme) + " " + 
                              themeService->getThemeName(theme);
            if (showDescriptions) {
                text += " - " + themeService->getThemeDescription(theme);
            }
            comboBoxPtr->addItem(text);
        }
        
        // Set current selection
        auto themes = themeService->getAvailableThemes();
        auto currentTheme = themeService->getCurrentTheme();
        auto it = std::find(themes.begin(), themes.end(), currentTheme);
        if (it != themes.end()) {
            comboBoxPtr->setCurrentIndex(std::distance(themes.begin(), it));
        }
        
        // Connect change event
        comboBoxPtr->changed().connect([themeService, comboBoxPtr]() {
            int index = comboBoxPtr->currentIndex();
            auto themes = themeService->getAvailableThemes();
            if (index >= 0 && index < static_cast<int>(themes.size())) {
                themeService->setTheme(themes[index]);
            }
        });
        
        return container;
    }
    
    std::unique_ptr<Wt::WPushButton> createThemeToggleButton(
        std::shared_ptr<ThemeService> themeService) {
        
        auto button = std::make_unique<Wt::WPushButton>();
        button->addStyleClass("btn btn-outline-secondary");
        
        // Get raw pointer for lambda capture (Wt manages widget lifetime)
        Wt::WPushButton* buttonPtr = button.get();
        
        // Update button text based on current theme
        auto updateButton = [themeService, buttonPtr]() {
            auto theme = themeService->getCurrentTheme();
            buttonPtr->setText(themeService->getThemeIcon(theme) + " " + 
                              themeService->getThemeName(theme));
        };
        
        updateButton();
        
        // Connect click event
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
    
    ThemeService::Theme getRecommendedTheme() {
        // Simple heuristic based on time of day
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
        // In a real implementation, you might check user preferences or device capabilities
        return true;
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
}
