//=============================================================================
#ifndef THEMESELECTIONDIALOG_H
#define THEMESELECTIONDIALOG_H

#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"

#include <Wt/WDialog.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WSpinBox.h>

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

/**
 * @file ThemeSelectionDialog.hpp
 * @brief Theme selection and preferences dialog for the Restaurant POS System
 * 
 * This dialog allows users to select themes and configure application preferences
 * including display settings, language options, and behavior preferences.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class ThemeSelectionDialog
 * @brief Dialog for selecting themes and configuring application preferences
 * 
 * The ThemeSelectionDialog provides a comprehensive interface for users to:
 * - Select from available themes with live preview
 * - Configure general application settings
 * - Adjust display preferences
 * - Set language and regional options
 */
class ThemeSelectionDialog : public Wt::WDialog {
public:
    /**
     * @struct ThemeInfo
     * @brief Information about an available theme
     */
    struct ThemeInfo {
        std::string id;                     ///< Unique theme identifier
        std::string name;                   ///< Display name
        std::string description;            ///< Theme description
        std::string cssFile;               ///< CSS file path
        bool isDefault;                    ///< Whether this is the default theme
        std::vector<std::string> previewColors; ///< Colors for preview display
        
        ThemeInfo() : isDefault(false) {}
        
        ThemeInfo(const std::string& id_, const std::string& name_, 
                 const std::string& desc_, const std::string& css_, 
                 bool default_ = false, 
                 const std::vector<std::string>& colors_ = {})
            : id(id_), name(name_), description(desc_), cssFile(css_), 
              isDefault(default_), previewColors(colors_) {}
    };
    
    /**
     * @brief Theme change callback type
     */
    using ThemeChangeCallback = std::function<void(const ThemeInfo& theme)>;
    
    /**
     * @brief Constructs a theme selection dialog
     * @param eventManager Event manager for notifications
     * @param callback Callback function for theme changes
     */
    ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                        ThemeChangeCallback callback = nullptr);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ThemeSelectionDialog() = default;
    
    /**
     * @brief Gets the currently selected theme ID
     * @return Selected theme ID
     */
    const std::string& getSelectedThemeId() const { return selectedThemeId_; }
    
    /**
     * @brief Sets the current theme
     * @param themeId Theme ID to set as current
     */
    void setCurrentTheme(const std::string& themeId);
    
    /**
     * @brief Gets information about a specific theme
     * @param themeId Theme ID to get info for
     * @return Theme information
     */
    ThemeInfo getThemeInfo(const std::string& themeId) const;
    
    /**
     * @brief Gets all available themes
     * @return Vector of available themes
     */
    const std::vector<ThemeInfo>& getAvailableThemes() const { return availableThemes_; }
    
    /**
     * @brief Shows the theme selection dialog
     */
    void showDialog() { show(); }

protected:
    /**
     * @brief Creates the main dialog content
     */
    void createDialogContent();
    
    /**
     * @brief Creates the theme selection panel
     */
    void createThemePanel();
    
    /**
     * @brief Creates the general preferences panel
     */
    void createGeneralPanel();
    
    /**
     * @brief Creates the display preferences panel
     */
    void createDisplayPanel();
    
    /**
     * @brief Creates a theme selection card
     * @param theme Theme information
     * @return Container widget for the theme card
     */
    std::unique_ptr<Wt::WContainerWidget> createThemeCard(const ThemeInfo& theme);
    
    /**
     * @brief Creates the action buttons container
     * @return Container widget with action buttons
     */
    std::unique_ptr<Wt::WContainerWidget> createActionButtons();
    
    /**
     * @brief Sets up event handlers for UI interactions
     */
    void setupEventHandlers();
    
    /**
     * @brief Shows the theme selection panel
     */
    void showThemePanel();
    
    /**
     * @brief Shows the general preferences panel
     */
    void showGeneralPanel();
    
    /**
     * @brief Shows the display preferences panel
     */
    void showDisplayPanel();
    
    /**
     * @brief Handles theme selection change
     */
    void onThemeSelectionChanged();
    
    /**
     * @brief Handles specific theme selection
     * @param themeId Selected theme ID
     */
    void onThemeSelected(const std::string& themeId);
    
    /**
     * @brief Toggles preview mode on/off
     */
    void togglePreviewMode();
    
    /**
     * @brief Resets all settings to defaults
     */
    void resetToDefaults();
    
    /**
     * @brief Applies all changes and closes dialog
     */
    void applyChanges();
    
    /**
     * @brief Applies a specific theme
     * @param themeId Theme ID to apply
     */
    void applyTheme(const std::string& themeId);
    
    /**
     * @brief Loads theme configuration from JSON file
     */
    void loadThemeConfiguration();
    
    /**
     * @brief Loads default themes if config file unavailable
     */
    void loadDefaultThemes();
    
    /**
     * @brief Loads current theme from preferences
     */
    void loadCurrentTheme();
    
    /**
     * @brief Saves preferences to persistent storage
     */
    void savePreferences();

private:
    // Core components
    std::shared_ptr<EventManager> eventManager_;
    ThemeChangeCallback themeChangeCallback_;
    
    // Theme management
    std::vector<ThemeInfo> availableThemes_;
    std::string currentThemeId_;
    std::string selectedThemeId_;
    std::string originalThemeId_;
    bool previewMode_;
    
    // UI Components - Tab Navigation
    Wt::WContainerWidget* tabContent_;
    Wt::WPushButton* themeTab_;
    Wt::WPushButton* generalTab_;
    Wt::WPushButton* displayTab_;
    
    // UI Components - Theme Panel
    std::unique_ptr<Wt::WContainerWidget> themePanel_;
    std::unique_ptr<Wt::WButtonGroup> themeButtonGroup_;
    std::unordered_map<std::string, Wt::WRadioButton*> themeRadioButtons_;
    Wt::WText* currentThemeText_;
    
    // UI Components - General Panel
    std::unique_ptr<Wt::WContainerWidget> generalPanel_;
    Wt::WCheckBox* autoSaveCheckbox_;
    Wt::WCheckBox* notificationsCheckbox_;
    Wt::WCheckBox* soundCheckbox_;
    Wt::WComboBox* languageCombo_;
    
    // UI Components - Display Panel
    std::unique_ptr<Wt::WContainerWidget> displayPanel_;
    Wt::WSpinBox* fontSizeSpinBox_;
    Wt::WCheckBox* showDescriptionsCheckbox_;
    Wt::WCheckBox* compactModeCheckbox_;
    Wt::WCheckBox* animationsCheckbox_;
    Wt::WSpinBox* rowsPerPageSpinBox_;
    Wt::WCheckBox* stripedRowsCheckbox_;
    
    // UI Components - Action Buttons
    Wt::WPushButton* previewButton_;
    Wt::WPushButton* resetButton_;
    Wt::WPushButton* cancelButton_;
    Wt::WPushButton* applyButton_;
};

#endif // THEMESELECTIONDIALOG_H
