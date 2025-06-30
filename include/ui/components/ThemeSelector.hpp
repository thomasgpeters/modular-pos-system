#ifndef THEMESELECTOR_H
#define THEMESELECTOR_H

#include "../../services/ThemeService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

#include <memory>
#include <vector>

/**
 * @file ThemeSelector.hpp
 * @brief Theme selection component for the Restaurant POS System
 * 
 * This component provides a user interface for selecting and applying
 * different visual themes to the application.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class ThemeSelector
 * @brief UI component for theme selection and management
 * 
 * The ThemeSelector provides a simple interface for users to choose
 * from available themes and apply them to the application. It displays
 * theme previews and handles theme switching through the ThemeService.
 */
class ThemeSelector : public Wt::WContainerWidget {
public:
    /**
     * @enum DisplayMode
     * @brief Display modes for the theme selector
     */
    enum DisplayMode {
        COMPACT,    ///< Simple dropdown selector
        DETAILED,   ///< Dropdown with preview and description
        BUTTON_ONLY ///< Just a button that opens a dialog
    };
    
    /**
     * @brief Constructs the theme selector
     * @param themeService Shared theme service for theme operations
     * @param eventManager Shared event manager for component communication
     * @param mode Display mode for the selector (default: COMPACT)
     */
    ThemeSelector(std::shared_ptr<ThemeService> themeService,
                 std::shared_ptr<EventManager> eventManager,
                 DisplayMode mode = COMPACT);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ThemeSelector() = default;
    
    /**
     * @brief Refreshes the theme selector from the service
     */
    void refresh();
    
    /**
     * @brief Sets the display mode
     * @param mode Display mode to use
     */
    void setDisplayMode(DisplayMode mode);
    
    /**
     * @brief Gets the current display mode
     * @return Current display mode
     */
    DisplayMode getDisplayMode() const;
    
    /**
     * @brief Sets whether the selector is enabled
     * @param enabled True to enable theme switching
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Checks if the selector is enabled
     * @return True if enabled
     */
    bool isEnabled() const;
    
    /**
     * @brief Gets the currently selected theme ID
     * @return Current theme ID
     */
    std::string getCurrentThemeId() const;
    
    /**
     * @brief Sets the current theme by ID
     * @param themeId Theme ID to select
     * @return True if theme was found and selected
     */
    bool setCurrentTheme(const std::string& themeId);

protected:
    /**
     * @brief Initializes the UI components
     */
    void initializeUI();
    
    /**
     * @brief Sets up event listeners
     */
    void setupEventListeners();
    
    /**
     * @brief Builds the compact mode UI
     */
    void buildCompactMode();
    
    /**
     * @brief Builds the detailed mode UI
     */
    void buildDetailedMode();
    
    /**
     * @brief Builds the button-only mode UI
     */
    void buildButtonOnlyMode();
    
    /**
     * @brief Updates the theme options in the dropdown
     */
    void updateThemeOptions();
    
    /**
     * @brief Updates the theme preview display
     */
    void updateThemePreview();
    
    /**
     * @brief Shows the theme selection dialog
     */
    void showThemeSelectionDialog();

private:
    // Services and dependencies
    std::shared_ptr<ThemeService> themeService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // Configuration
    DisplayMode displayMode_;
    bool enabled_;
    
    // UI components
    Wt::WComboBox* themeComboBox_;
    Wt::WPushButton* themeButton_;
    Wt::WText* themeDescriptionText_;
    Wt::WContainerWidget* themePreviewContainer_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers
    void handleThemeChanged(const std::any& eventData);
    void handleThemeListUpdated(const std::any& eventData);
    
    // UI action handlers
    void onThemeSelectionChanged();
    void onThemeButtonClicked();
    void onApplyThemeClicked(const std::string& themeId);
    
    // Helper methods
    void populateThemeComboBox();
    void selectCurrentThemeInComboBox();
    std::string getThemeDisplayText(const ThemeService::ThemeInfo& theme) const;
    std::unique_ptr<Wt::WWidget> createThemePreview(const ThemeService::ThemeInfo& theme);
    void clearUI();
    void rebuildUI();
    
    // Constants
    static constexpr int PREVIEW_COLOR_SIZE = 20;
};

#endif // THEMESELECTOR_H
