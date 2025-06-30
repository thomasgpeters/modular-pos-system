
#ifndef THEMESERVICE_H
#define THEMESERVICE_H

#include "../events/EventManager.hpp"
#include <Wt/WApplication.h>
#include <Wt/WLink.h>

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

/**
 * @file ThemeService.hpp
 * @brief Centralized theme management service
 * 
 * This service handles all theme-related operations including loading,
 * switching, and managing theme configurations.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class ThemeService
 * @brief Service for managing UI themes
 * 
 * The ThemeService provides centralized theme management, allowing
 * dynamic theme switching and theme persistence across sessions.
 */
class ThemeService {
public:
    /**
     * @struct ThemeInfo
     * @brief Information about a theme
     */
    struct ThemeInfo {
        std::string id;                          ///< Unique theme identifier
        std::string name;                        ///< Display name
        std::string description;                 ///< Theme description
        std::string cssFile;                     ///< Path to CSS file
        std::string externalCss;                 ///< External CSS URL (optional)
        std::vector<std::string> previewColors; ///< Colors for preview
        bool isDefault;                          ///< Whether this is the default theme
        bool isEnabled;                          ///< Whether theme is enabled
        
        ThemeInfo() : isDefault(false), isEnabled(true) {}
        
        ThemeInfo(const std::string& id, const std::string& name,
                 const std::string& description, const std::string& cssFile,
                 const std::string& externalCss = "",
                 const std::vector<std::string>& previewColors = {},
                 bool isDefault = false, bool isEnabled = true)
            : id(id), name(name), description(description), cssFile(cssFile),
              externalCss(externalCss), previewColors(previewColors),
              isDefault(isDefault), isEnabled(isEnabled) {}
    };
    
    /**
     * @brief Constructs the theme service
     * @param eventManager Shared event manager for notifications
     * @param application Wt application instance for theme application
     */
    ThemeService(std::shared_ptr<EventManager> eventManager,
                Wt::WApplication* application);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ThemeService() = default;
    
    /**
     * @brief Initializes the theme service
     * Loads available themes and applies the default theme
     */
    void initialize();
    
    /**
     * @brief Gets all available themes
     * @return Vector of theme information
     */
    const std::vector<ThemeInfo>& getAvailableThemes() const;
    
    /**
     * @brief Gets enabled themes only
     * @return Vector of enabled theme information
     */
    std::vector<ThemeInfo> getEnabledThemes() const;
    
    /**
     * @brief Gets theme information by ID
     * @param themeId Theme ID to look up
     * @return Pointer to theme info, or nullptr if not found
     */
    const ThemeInfo* getThemeById(const std::string& themeId) const;
    
    /**
     * @brief Gets the currently active theme ID
     * @return Current theme ID
     */
    const std::string& getCurrentThemeId() const;
    
    /**
     * @brief Gets the currently active theme information
     * @return Pointer to current theme info, or nullptr if none set
     */
    const ThemeInfo* getCurrentTheme() const;
    
    /**
     * @brief Applies a theme by ID
     * @param themeId ID of the theme to apply
     * @return True if theme was found and applied, false otherwise
     */
    bool applyTheme(const std::string& themeId);
    
    /**
     * @brief Applies the default theme
     * @return True if default theme was applied, false if none found
     */
    bool applyDefaultTheme();
    
    /**
     * @brief Reloads themes from configuration
     * Useful for development or when themes are added dynamically
     */
    void reloadThemes();
    
    /**
     * @brief Adds a new theme
     * @param theme Theme information to add
     * @return True if theme was added, false if ID already exists
     */
    bool addTheme(const ThemeInfo& theme);
    
    /**
     * @brief Removes a theme by ID
     * @param themeId ID of theme to remove
     * @return True if theme was found and removed, false otherwise
     */
    bool removeTheme(const std::string& themeId);
    
    /**
     * @brief Enables or disables a theme
     * @param themeId ID of theme to modify
     * @param enabled True to enable, false to disable
     * @return True if theme was found and modified, false otherwise
     */
    bool setThemeEnabled(const std::string& themeId, bool enabled);
    
    /**
     * @brief Gets user's preferred theme from storage
     * @return Preferred theme ID, or empty string if none set
     */
    std::string getUserPreferredTheme() const;
    
    /**
     * @brief Sets user's preferred theme in storage
     * @param themeId Theme ID to set as preferred
     */
    void setUserPreferredTheme(const std::string& themeId);
    
    /**
     * @brief Clears user's theme preference
     */
    void clearUserPreferredTheme();

protected:
    /**
     * @brief Loads hardcoded themes
     * Fallback method when configuration file is not available
     */
    void loadHardcodedThemes();
    
    /**
     * @brief Loads themes from configuration file
     * @return True if configuration was loaded, false otherwise
     */
    bool loadThemesFromConfiguration();
    
    /**
     * @brief Applies CSS files for a theme
     * @param theme Theme to apply
     * @return True if CSS was successfully applied
     */
    bool applyCSSFiles(const ThemeInfo& theme);
    
    /**
     * @brief Removes previously applied CSS files
     */
    void removePreviousCSSFiles();

private:
    // Dependencies
    std::shared_ptr<EventManager> eventManager_;
    Wt::WApplication* application_;
    
    // Theme data
    std::vector<ThemeInfo> availableThemes_;
    std::string currentThemeId_;
    std::string previousThemeId_;
    
    // Theme directories
    std::string themeDirectory_;
    std::string customThemeDirectory_;
    
    // Applied CSS links for cleanup
    std::vector<Wt::WLink> appliedCSSLinks_;
    
    // Configuration
    bool allowUserThemes_;
    bool persistUserPreference_;
    
    // Helper methods
    void initializeDefaultThemes();
    void notifyThemeChanged(const std::string& newThemeId, const std::string& oldThemeId);
    bool validateTheme(const ThemeInfo& theme) const;
    std::string sanitizeThemeId(const std::string& themeId) const;
    
    // Storage methods
    void saveThemePreference(const std::string& themeId);
    std::string loadThemePreference() const;
};

#endif // THEMESERVICE_H
