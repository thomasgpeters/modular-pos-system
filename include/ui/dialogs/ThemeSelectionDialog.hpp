//=============================================================================

#ifndef THEMESELECTIONDIALOG_H
#define THEMESELECTIONDIALOG_H

#include "../../services/ThemeService.hpp"
#include "../../events/EventManager.hpp"

#include <Wt/WDialog.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WGridLayout.h>

#include <memory>
#include <vector>
#include <functional>

/**
 * @file ThemeSelectionDialog.hpp
 * @brief Theme selection dialog for UI customization
 * 
 * This dialog allows users to preview and select different
 * themes for the POS system interface.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class ThemeSelectionDialog
 * @brief Dialog for selecting and applying UI themes
 * 
 * The ThemeSelectionDialog provides an interface for users to
 * browse available themes, see previews, and apply their selection.
 */
class ThemeSelectionDialog : public Wt::WDialog {
public:
    /**
     * @brief Theme selection callback type
     */
    using ThemeSelectionCallback = std::function<void(const std::string& themeId)>;
    
    /**
     * @brief Constructs a theme selection dialog
     * @param themeService Theme service for theme management
     * @param eventManager Event manager for notifications
     * @param callback Callback function for theme selection
     */
    ThemeSelectionDialog(std::shared_ptr<ThemeService> themeService,
                        std::shared_ptr<EventManager> eventManager,
                        ThemeSelectionCallback callback = nullptr);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ThemeSelectionDialog() = default;
    
    /**
     * @brief Shows the theme selection dialog
     */
    void showDialog();
    
    /**
     * @brief Refreshes the available themes list
     */
    void refreshThemes();
    
    /**
     * @brief Sets whether to show theme previews
     * @param enabled True to show color previews
     */
    void setShowPreviews(bool enabled);

protected:
    /**
     * @brief Creates the dialog content
     */
    void createDialogContent();
    
    /**
     * @brief Creates the themes grid
     * @return Container widget with theme selection
     */
    std::unique_ptr<Wt::WContainerWidget> createThemesGrid();
    
    /**
     * @brief Creates a theme card
     * @param theme Theme information
     * @return Container widget representing the theme
     */
    std::unique_ptr<Wt::WContainerWidget> createThemeCard(const ThemeService::ThemeInfo& theme);
    
    /**
     * @brief Creates color preview for a theme
     * @param theme Theme information
     * @return Container widget with color swatches
     */
    std::unique_ptr<Wt::WContainerWidget> createColorPreview(const ThemeService::ThemeInfo& theme);
    
    /**
     * @brief Creates the action buttons section
     * @return Container widget with action buttons
     */
    std::unique_ptr<Wt::WContainerWidget> createActionButtons();
    
    /**
     * @brief Handles theme selection changes
     */
    void onThemeSelectionChanged();
    
    /**
     * @brief Applies the selected theme
     */
    void applySelectedTheme();
    
    /**
     * @brief Gets the currently selected theme ID
     * @return Selected theme ID, or empty string if none
     */
    std::string getSelectedThemeId() const;

private:
    // Dependencies
    std::shared_ptr<ThemeService> themeService_;
    std::shared_ptr<EventManager> eventManager_;
    ThemeSelectionCallback selectionCallback_;
    
    // Configuration
    bool showPreviews_;
    
    // UI components
    Wt::WContainerWidget* themesContainer_;
    Wt::WButtonGroup* themeButtonGroup_;
    std::vector<Wt::WRadioButton*> themeButtons_;
    
    Wt::WPushButton* applyButton_;
    Wt::WPushButton* cancelButton_;
    Wt::WPushButton* previewButton_;
    
    // Theme data
    std::vector<ThemeService::ThemeInfo> availableThemes_;
    std::string originalThemeId_;
    std::string selectedThemeId_;
    
    // Helper methods
    void setupEventHandlers();
    void updateApplyButton();
    void previewTheme();
    void restoreOriginalTheme();
};

#endif // THEMESELECTIONDIALOG_H
