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
#include <map>
#include <any>

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
     * @brief Theme selection callback type (alias for compatibility)
     */
    using ThemeSelectionCallback = ThemeChangeCallback;
    
    /**
     * @brief String callback type for simple theme ID callbacks
     */
    using StringCallback = std::function<void(const std::string&)>;
    
    // =================================================================
    // CONSTRUCTORS - Multiple overloads to handle all creation patterns
    // =================================================================
    
    /**
     * @brief Default constructor
     */
    ThemeSelectionDialog();
    
    /**
     * @brief Constructor with event manager only
     * @param eventManager Event manager for notifications
     */
    explicit ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Constructor with event manager and ThemeChangeCallback
     * @param eventManager Event manager for notifications
     * @param callback Callback function for theme changes
     */
    ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                        ThemeChangeCallback callback);
    
        
    /**
     * @brief Constructor with event manager and string callback
     * @param eventManager Event manager for notifications
     * @param callback String callback function (theme ID only)
     */
    ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                        StringCallback callback);
    
    /**
     * @brief Constructor with configuration options
     * @param eventManager Event manager for notifications
     * @param callback Callback function for theme changes
     * @param showPreviews Whether to show theme previews
     */
    ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                        ThemeChangeCallback callback,
                        bool showPreviews);
    
    /**
     * @brief Constructor with full configuration
     * @param eventManager Event manager for notifications
     * @param callback Callback function for theme changes
     * @param showPreviews Whether to show theme previews
     * @param showDescriptions Whether to show descriptions
     */
    ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                        ThemeChangeCallback callback,
                        bool showPreviews,
                        bool showDescriptions);
    
    /**
     * @brief Constructor with extended configuration
     * @param eventManager Event manager for notifications
     * @param callback Callback function for theme changes
     * @param showPreviews Whether to show theme previews
     * @param showDescriptions Whether to show descriptions
     * @param maxThemes Maximum number of themes to display
     */
    ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                        ThemeChangeCallback callback,
                        bool showPreviews,
                        bool showDescriptions,
                        int maxThemes);
    
    /**
     * @brief Constructor with configuration map
     * @param eventManager Event manager for notifications
     * @param callback Callback function for theme changes
     * @param config Configuration options map
     */
    ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                        ThemeChangeCallback callback,
                        const std::map<std::string, std::any>& config);
    
        
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
    
    // =================================================================
    // Configuration Methods
    // =================================================================
    
    /**
     * @brief Sets whether to show theme previews
     * @param showPreviews True to show previews, false to hide
     */
    void setShowPreviews(bool showPreviews) { showPreviews_ = showPreviews; }
    
    /**
     * @brief Gets whether previews are shown
     * @return True if previews are shown
     */
    bool getShowPreviews() const { return showPreviews_; }
    
    /**
     * @brief Sets the dialog size
     * @param width Dialog width
     * @param height Dialog height
     */
    void setDialogSize(int width, int height) {
        resize(Wt::WLength(width), Wt::WLength(height));
    }
    
    /**
     * @brief Sets whether the dialog is modal
     * @param modal True for modal dialog
     */
    void setDialogModal(bool modal) { 
        if (modal) {
            setModal(true);
        }
    }
    
    /**
     * @brief Sets whether the dialog is resizable
     * @param resizable True to allow resizing
     */
    void setDialogResizable(bool resizable) { 
        setResizable(resizable); 
    }
    
    /**
     * @brief Sets the dialog title
     * @param title Dialog title
     */
    void setDialogTitle(const std::string& title) { setWindowTitle(title); }
    
    /**
     * @brief Enables or disables live preview mode
     * @param enabled True to enable live preview
     */
    void setLivePreviewEnabled(bool enabled) { livePreviewEnabled_ = enabled; }
    
    /**
     * @brief Gets whether live preview is enabled
     * @return True if live preview is enabled
     */
    bool isLivePreviewEnabled() const { return livePreviewEnabled_; }
    
    /**
     * @brief Sets the maximum number of themes to display
     * @param maxThemes Maximum number of themes
     */
    void setMaxThemes(int maxThemes) { maxThemes_ = maxThemes; }
    
    /**
     * @brief Gets the maximum number of themes displayed
     * @return Maximum themes
     */
    int getMaxThemes() const { return maxThemes_; }
    
    /**
     * @brief Sets whether to show theme descriptions
     * @param showDescriptions True to show descriptions
     */
    void setShowDescriptions(bool showDescriptions) { showDescriptions_ = showDescriptions; }
    
    /**
     * @brief Gets whether descriptions are shown
     * @return True if descriptions are shown
     */
    bool getShowDescriptions() const { return showDescriptions_; }
    
    /**
     * @brief Sets the preview size
     * @param width Preview width
     * @param height Preview height
     */
    void setPreviewSize(int width, int height) {
        previewWidth_ = width;
        previewHeight_ = height;
    }
    
    /**
     * @brief Gets the preview width
     * @return Preview width
     */
    int getPreviewWidth() const { return previewWidth_; }
    
    /**
     * @brief Gets the preview height
     * @return Preview height
     */
    int getPreviewHeight() const { return previewHeight_; }
    
    /**
     * @brief Sets configuration from a map
     * @param config Configuration map
     */
    void setConfiguration(const std::map<std::string, std::any>& config);

protected:
    // ... (same protected methods as before)
    void createDialogContent();
    void createThemePanel();
    void createGeneralPanel();
    void createDisplayPanel();
    std::unique_ptr<Wt::WContainerWidget> createThemeCard(const ThemeInfo& theme);
    std::unique_ptr<Wt::WContainerWidget> createActionButtons();
    void setupEventHandlers();
    void showThemePanel();
    void showGeneralPanel();
    void showDisplayPanel();
    void onThemeSelectionChanged();
    void onThemeSelected(const std::string& themeId);
    void togglePreviewMode();
    void resetToDefaults();
    void applyChanges();
    void applyTheme(const std::string& themeId);
    void loadThemeConfiguration();
    void loadDefaultThemes();
    void loadCurrentTheme();
    void savePreferences();

private:
    /**
     * @brief Initialize dialog with default values
     */
    void initializeDefaults();
    
    /**
     * @brief Convert string callback to ThemeChangeCallback
     * @param stringCallback String callback to convert
     * @return ThemeChangeCallback
     */
    ThemeChangeCallback convertStringCallback(StringCallback stringCallback);

    // Core components
    std::shared_ptr<EventManager> eventManager_;
    ThemeChangeCallback themeChangeCallback_;
    
    // Theme management
    std::vector<ThemeInfo> availableThemes_;
    std::string currentThemeId_;
    std::string selectedThemeId_;
    std::string originalThemeId_;
    bool previewMode_;
    
    // Configuration options
    bool showPreviews_;
    bool livePreviewEnabled_;
    bool showDescriptions_;
    int maxThemes_;
    int previewWidth_;
    int previewHeight_;
    
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
