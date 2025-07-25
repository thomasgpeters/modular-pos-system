/* ============================================================================
   Updated ThemeService.hpp - Enhanced with WARM and COOL themes
   
   This file extends the existing ThemeService to include the new theme modes
   while maintaining backward compatibility.
   
   File: include/services/ThemeService.hpp
   ============================================================================ */

#ifndef THEMESERVICE_H
#define THEMESERVICE_H

#include "../utils/CSSLoader.hpp"

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

/**
 * @file ThemeService.hpp
 * @brief Enhanced modular theme management service for the Restaurant POS System
 * 
 * This service provides a centralized way to manage themes across the application,
 * supporting Bootstrap base, Light, Dark, Warm, and Cool modes. Integrates with the CSS
 * theme framework for consistent styling.
 * 
 * @author Restaurant POS Team
 * @version 2.2.0 - Enhanced with Warm and Cool themes
 */

/**
 * @class ThemeService
 * @brief Enhanced service for managing application themes and user preferences
 * 
 * The ThemeService handles theme switching, persistence, and provides
 * a clean interface for components to react to theme changes.
 */
class ThemeService {
public:
    /**
     * @enum Theme
     * @brief Available theme options - Enhanced with new themes
     */
    enum class Theme {
        BASE,       ///< Bootstrap base theme (clean, professional)
        LIGHT,      ///< Light theme (bright, high contrast)
        DARK,       ///< Dark theme (easy on eyes)
        WARM,       ///< Warm theme (restaurant-friendly earth tones)
        COOL,       ///< Cool theme (modern, tech-focused)
        AUTO        ///< Automatic theme based on system preference
    };
    
    /**
     * @brief Theme change callback type
     */
    using ThemeChangeCallback = std::function<void(Theme oldTheme, Theme newTheme)>;
    
    /**
     * @brief CSS loader callback type for dynamic CSS loading
     */
    using CSSLoaderCallback = std::function<void(const std::string& cssPath, bool load)>;
    
    /**
     * @brief Constructs the theme service
     * @param app Wt application instance for DOM manipulation
     */
    explicit ThemeService(Wt::WApplication* app);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ThemeService() = default;
    
    // =================================================================
    // Theme Management
    // =================================================================
    
    /**
     * @brief Sets the current theme
     * @param theme Theme to activate
     * @param savePreference Whether to save the preference (default: true)
     */
    void setTheme(Theme theme, bool savePreference = true);
    
    /**
     * @brief Gets the current active theme
     * @return Current theme
     */
    Theme getCurrentTheme() const { return currentTheme_; }
    
    /**
     * @brief Gets the user's preferred theme
     * @return Preferred theme (may differ from current if auto mode is active)
     */
    Theme getPreferredTheme() const { return preferredTheme_; }
    
    /**
     * @brief Toggles between light and dark themes
     */
    void toggleTheme();
    
    /**
     * @brief Cycles through all available themes
     */
    void cycleTheme();
    
    /**
     * @brief Sets theme based on system preference
     */
    void setAutoTheme();
    
    /**
     * @brief Checks if system dark mode is preferred
     * @return True if system prefers dark mode
     */
    bool isSystemDarkMode() const;
    
    // =================================================================
    // Theme Information
    // =================================================================
    
    /**
     * @brief Gets all available themes
     * @return Vector of theme options
     */
    std::vector<Theme> getAvailableThemes() const;
    
    /**
     * @brief Gets theme display name
     * @param theme Theme to get name for
     * @return Human-readable theme name
     */
    std::string getThemeName(Theme theme) const;
    
    /**
     * @brief Gets theme description
     * @param theme Theme to get description for
     * @return Theme description
     */
    std::string getThemeDescription(Theme theme) const;
    
    /**
     * @brief Gets theme CSS class name
     * @param theme Theme to get class for
     * @return CSS class name
     */
    std::string getThemeCSSClass(Theme theme) const;
    
    /**
     * @brief Gets theme icon
     * @param theme Theme to get icon for
     * @return Unicode icon or emoji
     */
    std::string getThemeIcon(Theme theme) const;
    
    /**
     * @brief Gets theme primary color (hex)
     * @param theme Theme to get color for
     * @return Primary color in hex format
     */
    std::string getThemePrimaryColor(Theme theme) const;
    
    /**
     * @brief Gets theme CSS file path
     * @param theme Theme to get CSS path for
     * @return CSS file path
     */
    std::string getThemeCSSPath(Theme theme) const;
    
    /**
     * @brief Gets theme category (for grouping in UI)
     * @param theme Theme to get category for
     * @return Theme category string
     */
    std::string getThemeCategory(Theme theme) const;
    
    // =================================================================
    // Event Handling
    // =================================================================
    
    /**
     * @brief Registers a callback for theme changes
     * @param callback Function to call when theme changes
     * @return Subscription handle for unregistering
     */
    size_t onThemeChanged(ThemeChangeCallback callback);
    
    /**
     * @brief Unregisters a theme change callback
     * @param handle Subscription handle from onThemeChanged
     */
    void removeThemeChangeCallback(size_t handle);
    
    // =================================================================
    // CSS Loading and Management
    // =================================================================
    
    /**
     * @brief Sets a CSS loader callback for dynamic CSS loading
     * @param callback Function to call for loading/unloading CSS files
     */
    void setCSSLoaderCallback(CSSLoaderCallback callback);
    
    /**
     * @brief Loads the CSS framework and current theme
     */
    void loadThemeFramework();
    
    /**
     * @brief Unloads all theme CSS files
     */
    void unloadAllThemes();
    
    /**
     * @brief Reloads the current theme CSS
     */
    void reloadCurrentTheme();
    
    // =================================================================
    // Persistence
    // =================================================================
    
    /**
     * @brief Loads theme preference from storage
     */
    void loadThemePreference();
    
    /**
     * @brief Saves current theme preference to storage
     */
    void saveThemePreference();
    
    /**
     * @brief Clears saved theme preference
     */
    void clearThemePreference();
    
    /**
     * @brief Checks if theme preference is saved
     * @return True if preference is saved
     */
    bool hasThemePreference() const;
    
    // =================================================================
    // Utility Methods
    // =================================================================
    
    /**
     * @brief Applies theme to a specific container
     * @param container Container to apply theme to
     * @param theme Theme to apply (uses current if not specified)
     */
    void applyThemeToContainer(Wt::WContainerWidget* container, 
                              Theme theme = Theme::AUTO);
    
    /**
     * @brief Removes theme classes from a container
     * @param container Container to remove themes from
     */
    void removeThemeFromContainer(Wt::WContainerWidget* container);
    
    /**
     * @brief Gets CSS variables for current theme
     * @return Map of CSS variable names to values
     */
    std::map<std::string, std::string> getThemeCSSVariables() const;
    
    /**
     * @brief Injects custom CSS for theme support
     */
    void injectThemeCSS();
    
    /**
     * @brief Checks if theme supports dark mode
     * @param theme Theme to check
     * @return True if theme is dark
     */
    bool isThemeDark(Theme theme) const;
    
    /**
     * @brief Gets contrast ratio for theme
     * @param theme Theme to check
     * @return Contrast ratio (higher = better accessibility)
     */
    double getThemeContrastRatio(Theme theme) const;
    
    /**
     * @brief Validates theme choice for accessibility
     * @param theme Theme to validate
     * @return True if theme meets accessibility standards
     */
    bool isThemeAccessible(Theme theme) const;

protected:
    /**
     * @brief Applies theme classes to the application root
     */
    void applyThemeClasses();
    
    /**
     * @brief Removes all theme classes from application root
     */
    void removeThemeClasses();
    
    /**
     * @brief Notifies all registered callbacks of theme change
     * @param oldTheme Previous theme
     * @param newTheme New theme
     */
    void notifyThemeChange(Theme oldTheme, Theme newTheme);
    
    /**
     * @brief Determines effective theme (resolves AUTO mode)
     * @param theme Theme to resolve
     * @return Effective theme
     */
    Theme resolveTheme(Theme theme) const;
    
    /**
     * @brief Loads a specific theme's CSS file
     * @param theme Theme to load
     */
    void loadThemeCSS(Theme theme);
    
    /**
     * @brief Unloads a specific theme's CSS file
     * @param theme Theme to unload
     */
    void unloadThemeCSS(Theme theme);
    
    // =================================================================
    // FIXED: Added Missing Method Declarations
    // =================================================================
    
    /**
     * @brief Applies inline CSS for immediate theme changes
     * @param theme Theme to apply inline CSS for
     */
    void applyInlineThemeCSS(Theme theme);
    
    /**
     * @brief Generates comprehensive CSS for a theme
     * @param theme Theme to generate CSS for
     * @return CSS string for the theme
     */
    std::string generateThemeCSS(Theme theme);
    
    /**
     * @brief Loads theme CSS asynchronously (non-blocking)
     * @param theme Theme to load CSS for
     */
    void loadThemeCSSAsync(Theme theme);
    
    /**
     * @brief Applies base theme variables and framework CSS
     */
    void applyBaseThemeVariables();

private:
    Wt::WApplication* app_;                                 ///< Application instance
    Theme currentTheme_;                                    ///< Current active theme
    Theme preferredTheme_;                                  ///< User's preferred theme
    std::map<size_t, ThemeChangeCallback> callbacks_;      ///< Theme change callbacks
    size_t nextCallbackId_;                                 ///< Next callback ID
    CSSLoaderCallback cssLoaderCallback_;                  ///< CSS loader callback
    
    // Theme metadata
    std::map<Theme, std::string> themeNames_;
    std::map<Theme, std::string> themeDescriptions_;
    std::map<Theme, std::string> themeCSSClasses_;
    std::map<Theme, std::string> themeIcons_;
    std::map<Theme, std::string> themePrimaryColors_;
    std::map<Theme, std::string> themeCSSPaths_;
    std::map<Theme, std::string> themeCategories_;
    
    // Loaded CSS tracking
    std::vector<std::string> loadedCSSFiles_;
    bool frameworkLoaded_;
    
    // Helper methods
    void initializeThemeMetadata();
    std::string detectCSSBasePath();
    Theme parseThemeFromString(const std::string& themeString) const;
    std::string themeToString(Theme theme) const;
    void detectSystemTheme();
    void ensureFrameworkLoaded();
        
    // Storage key for preferences
    static constexpr const char* THEME_PREFERENCE_KEY = "pos_theme_preference";
    static constexpr const char* FRAMEWORK_CSS_PATH = "assets/css/theme-framework.css";
};

/**
 * @brief Enhanced theme management utility functions
 */
namespace ThemeUtils {
    
    /**
     * @brief Creates a theme selector widget with enhanced features
     * @param themeService Theme service instance
     * @param showDescriptions Whether to show theme descriptions
     * @param showCategories Whether to group by categories
     * @return Theme selector widget
     */
    std::unique_ptr<Wt::WWidget> createThemeSelector(
        std::shared_ptr<ThemeService> themeService,
        bool showDescriptions = true,
        bool showCategories = false);
    
    /**
     * @brief Creates a theme toggle button
     * @param themeService Theme service instance
     * @return Theme toggle button
     */
    std::unique_ptr<Wt::WPushButton> createThemeToggleButton(
        std::shared_ptr<ThemeService> themeService);
    
    /**
     * @brief Creates a quick theme switcher with icons
     * @param themeService Theme service instance
     * @return Quick theme switcher widget
     */
    std::unique_ptr<Wt::WWidget> createQuickThemeSwitcher(
        std::shared_ptr<ThemeService> themeService);
    
    /**
     * @brief Gets recommended theme based on time of day and context
     * @param context Context hint (e.g., "restaurant", "office", "retail")
     * @return Recommended theme
     */
    ThemeService::Theme getRecommendedTheme(const std::string& context = "");
    
    /**
     * @brief Checks if theme switching should be animated
     * @return True if animations are supported/enabled
     */
    bool shouldAnimateThemeTransitions();
    
    /**
     * @brief Applies transition animation for theme changes
     * @param app Application instance
     * @param duration Animation duration in milliseconds
     */
    void applyThemeTransition(Wt::WApplication* app, int duration = 300);
    
    /**
     * @brief Creates a CSS file loader function for ThemeService
     * @param app Application instance
     * @return CSS loader callback function
     */
    ThemeService::CSSLoaderCallback createCSSLoader(Wt::WApplication* app);
}

#endif // THEMESERVICE_H
