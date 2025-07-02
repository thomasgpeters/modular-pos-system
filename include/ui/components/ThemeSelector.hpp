#ifndef THEMESELECTOR_H
#define THEMESELECTOR_H

#include "../../services/ThemeService.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WGroupBox.h>

#include <memory>
#include <vector>

/**
 * @file ThemeSelector.hpp
 * @brief Theme selection UI component for the Restaurant POS System
 * 
 * This component provides a user interface for selecting and switching
 * between different application themes (Light, Dark, Colorful, Base).
 * Simplified version that focuses on dropdown selection for maximum compatibility.
 * 
 * @author Restaurant POS Team
 * @version 2.1.0
 */

/**
 * @class ThemeSelector
 * @brief UI component for theme selection and management
 * 
 * The ThemeSelector provides an intuitive dropdown interface for users to select
 * from available themes. This simplified version focuses on compatibility and
 * reliability over advanced features.
 */
class ThemeSelector : public Wt::WContainerWidget {
public:
    /**
     * @enum DisplayMode
     * @brief Different display modes for the theme selector
     * Note: Only DROPDOWN is fully implemented in this version
     */
    enum class DisplayMode {
        DROPDOWN,       ///< Dropdown/combo box selection (primary mode)
        RADIO_BUTTONS,  ///< Radio button selection (fallback to dropdown)
        TOGGLE_BUTTON,  ///< Simple toggle button (fallback to dropdown)
        GRID_CARDS      ///< Grid of theme preview cards (fallback to dropdown)
    };
    
    /**
     * @brief Theme selection callback type
     */
    using ThemeSelectionCallback = std::function<void(ThemeService::Theme selectedTheme)>;
    
    /**
     * @brief Constructs the theme selector
     * @param themeService Shared theme service for theme management
     * @param displayMode Display mode for the selector (all modes use dropdown in this version)
     */
    explicit ThemeSelector(std::shared_ptr<ThemeService> themeService,
                          DisplayMode displayMode = DisplayMode::DROPDOWN);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ThemeSelector() = default;
    
    /**
     * @brief Sets the display mode for the theme selector
     * @param mode New display mode (all modes use dropdown for compatibility)
     */
    void setDisplayMode(DisplayMode mode);
    
    /**
     * @brief Gets the current display mode
     * @return Current display mode
     */
    DisplayMode getDisplayMode() const { return displayMode_; }
    
    /**
     * @brief Sets whether to show theme descriptions
     * @param show True to show descriptions
     */
    void setShowDescriptions(bool show);
    
    /**
     * @brief Checks if descriptions are shown
     * @return True if descriptions are shown
     */
    bool getShowDescriptions() const { return showDescriptions_; }
    
    /**
     * @brief Sets whether to show theme previews
     * @param show True to show previews (not implemented in simplified version)
     */
    void setShowPreviews(bool show);
    
    /**
     * @brief Checks if previews are shown
     * @return True if previews are shown
     */
    bool getShowPreviews() const { return showPreviews_; }
    
    /**
     * @brief Sets whether to allow auto theme option
     * @param allow True to allow auto theme
     */
    void setAllowAutoTheme(bool allow);
    
    /**
     * @brief Checks if auto theme is allowed
     * @return True if auto theme is allowed
     */
    bool getAllowAutoTheme() const { return allowAutoTheme_; }
    
    /**
     * @brief Sets a callback for theme selection events
     * @param callback Function to call when theme is selected
     */
    void setThemeSelectionCallback(ThemeSelectionCallback callback);
    
    /**
     * @brief Gets the currently selected theme
     * @return Currently selected theme
     */
    ThemeService::Theme getSelectedTheme() const;
    
    /**
     * @brief Sets the selected theme
     * @param theme Theme to select
     */
    void setSelectedTheme(ThemeService::Theme theme);
    
    /**
     * @brief Refreshes the selector to match current theme service state
     */
    void refresh();
    
    /**
     * @brief Enables or disables the theme selector
     * @param enabled True to enable, false to disable
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Checks if the selector is enabled
     * @return True if enabled
     */
    bool isEnabled() const { return enabled_; }

protected:
    /**
     * @brief Initializes the UI components
     */
    void initializeUI();
    
    /**
     * @brief Creates the dropdown selector
     */
    void createDropdownSelector();
    
    /**
     * @brief Creates the radio button selector (fallback to dropdown)
     */
    void createRadioButtonSelector();
    
    /**
     * @brief Creates the toggle button selector (fallback to dropdown)
     */
    void createToggleButtonSelector();
    
    /**
     * @brief Creates the grid card selector (fallback to dropdown)
     */
    void createGridCardSelector();
    
    /**
     * @brief Clears the current selector UI
     */
    void clearSelector();
    
    /**
     * @brief Updates the selector to reflect current theme
     */
    void updateSelector();
    
    /**
     * @brief Sets up event listeners for theme service
     */
    void setupEventListeners();

private:
    // Core dependencies
    std::shared_ptr<ThemeService> themeService_;
    
    // Configuration
    DisplayMode displayMode_;
    bool showDescriptions_;
    bool showPreviews_;
    bool allowAutoTheme_;
    bool enabled_;
    
    // UI components
    Wt::WComboBox* themeComboBox_;
    Wt::WPushButton* toggleButton_;
    Wt::WContainerWidget* gridContainer_;
    Wt::WText* currentThemeText_;
    
    // Event handling
    ThemeSelectionCallback selectionCallback_;
    size_t themeChangeSubscription_;
    
    // Event handlers
    void onThemeSelected(ThemeService::Theme theme);
    void onThemeServiceChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme);
    
    // Helper methods
    std::string getThemeDisplayText(ThemeService::Theme theme) const;
    std::string getThemePreviewHTML(ThemeService::Theme theme) const;
    void populateDropdown();
    void populateRadioButtons();
    void updateToggleButton();
    void populateGridCards();
    void applyThemeSelectorStyling();
    void updateEnabledState();
    
    // Theme card creation (not implemented in simplified version)
    std::unique_ptr<Wt::WWidget> createThemeCard(ThemeService::Theme theme);
    void styleThemeCard(Wt::WContainerWidget* card, ThemeService::Theme theme);
    
    // Validation
    bool isValidTheme(ThemeService::Theme theme) const;
    std::vector<ThemeService::Theme> getSelectableThemes() const;
    
    // Radio button management (for future use - not implemented)
    void clearRadioButtonSelection();
    void setRadioButtonSelection(int index);
    int getSelectedRadioButtonIndex() const;
};

/**
 * @brief Theme selector utility functions
 */
namespace ThemeSelectorUtils {
    
    /**
     * @brief Creates a simple theme dropdown
     * @param themeService Theme service instance
     * @return Configured theme selector
     */
    std::unique_ptr<ThemeSelector> createSimpleDropdown(std::shared_ptr<ThemeService> themeService);
    
    /**
     * @brief Creates a theme toggle button (returns dropdown in simplified version)
     * @param themeService Theme service instance
     * @return Configured theme selector
     */
    std::unique_ptr<ThemeSelector> createToggleButton(std::shared_ptr<ThemeService> themeService);
    
    /**
     * @brief Creates a theme grid with previews (returns dropdown in simplified version)
     * @param themeService Theme service instance
     * @return Configured theme selector
     */
    std::unique_ptr<ThemeSelector> createThemeGrid(std::shared_ptr<ThemeService> themeService);
    
    /**
     * @brief Creates a compact theme selector for toolbars
     * @param themeService Theme service instance
     * @return Configured theme selector
     */
    std::unique_ptr<ThemeSelector> createCompactSelector(std::shared_ptr<ThemeService> themeService);
}

#endif // THEMESELECTOR_H
