#ifndef THEMESERVICE_H
#define THEMESERVICE_H

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

/**
 * @file ThemeService.hpp
 * @brief Modular theme management service for the Restaurant POS System
 * 
 * This service provides a centralized way to manage themes across the application,
 * supporting light, dark, colorful, and base modes. Integrates with the CSS
 * theme framework for consistent styling.
 * 
 * @author Restaurant POS Team
 * @version 2.1.0
 */

/**
 * @class ThemeService
 * @brief Service for managing application themes and user preferences
 * 
 * The ThemeService handles theme switching, persistence, and provides
 * a clean interface for components to react to theme changes.
 */
class ThemeService {
public:
    /**
     * @enum Theme
     * @brief Available theme options
     */
    enum class Theme {
        LIGHT,      ///< Light theme (default)
        DARK,       ///< Dark theme
        COLORFUL,   ///< Colorful theme with vibrant colors
        BASE,       ///< Minimal base theme
        AUTO        ///< Automatic theme based on system preference
    };
    
    /**
     * @brief Theme change callback type
     */
    using ThemeChangeCallback = std::function<void(Theme oldTheme, Theme newTheme)>;
    
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

private:
    Wt::WApplication* app_;                                 ///< Application instance
    Theme currentTheme_;                                    ///< Current active theme
    Theme preferredTheme_;                                  ///< User's preferred theme
    std::map<size_t, ThemeChangeCallback> callbacks_;      ///< Theme change callbacks
    size_t nextCallbackId_;                                 ///< Next callback ID
    
    // Theme metadata
    std::map<Theme, std::string> themeNames_;
    std::map<Theme, std::string> themeDescriptions_;
    std::map<Theme, std::string> themeCSSClasses_;
    std::map<Theme, std::string> themeIcons_;
    std::map<Theme, std::string> themePrimaryColors_;
    
    // Helper methods
    void initializeThemeMetadata();
    Theme parseThemeFromString(const std::string& themeString) const;
    std::string themeToString(Theme theme) const;
    void detectSystemTheme();
    
    // Storage key for preferences
    static constexpr const char* THEME_PREFERENCE_KEY = "pos_theme_preference";
};

/**
 * @brief Theme management utility functions
 */
namespace ThemeUtils {
    
    /**
     * @brief Creates a theme selector widget
     * @param themeService Theme service instance
     * @param showDescriptions Whether to show theme descriptions
     * @return Theme selector widget
     */
    std::unique_ptr<Wt::WWidget> createThemeSelector(
        std::shared_ptr<ThemeService> themeService,
        bool showDescriptions = true);
    
    /**
     * @brief Creates a theme toggle button
     * @param themeService Theme service instance
     * @return Theme toggle button
     */
    std::unique_ptr<Wt::WPushButton> createThemeToggleButton(
        std::shared_ptr<ThemeService> themeService);
    
    /**
     * @brief Gets recommended theme based on time of day
     * @return Recommended theme
     */
    ThemeService::Theme getRecommendedTheme();
    
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
}

#endif // THEMESERVICE_H
