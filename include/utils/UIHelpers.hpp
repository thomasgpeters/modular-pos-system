#ifndef UIHELPERS_H
#define UIHELPERS_H

#include <Wt/WContainerWidget.h>
#include <Wt/WWidget.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>

#include <memory>
#include <string>
#include <vector>

/**
 * @file UIHelpers.hpp
 * @brief UI utility functions and helpers for the Restaurant POS System
 * 
 * This file contains utility functions for common UI operations,
 * styling, and component creation patterns.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @namespace UIHelpers
 * @brief Utility functions for UI operations
 */
namespace UIHelpers {
    
    // =================================================================
    // Container and Layout Helpers
    // =================================================================
    
    /**
     * @brief Creates a card container with header and body
     * @param title Card title
     * @param bodyContent Content for the card body
     * @param headerActions Optional actions for the header
     * @return Unique pointer to the card container
     */
    std::unique_ptr<Wt::WContainerWidget> createCard(
        const std::string& title,
        std::unique_ptr<Wt::WWidget> bodyContent,
        std::unique_ptr<Wt::WWidget> headerActions = nullptr);
    
    /**
     * @brief Creates a panel with header and collapsible body
     * @param title Panel title
     * @param bodyContent Content for the panel body
     * @param isCollapsed Initial collapsed state
     * @return Unique pointer to the panel container
     */
    std::unique_ptr<Wt::WContainerWidget> createCollapsiblePanel(
        const std::string& title,
        std::unique_ptr<Wt::WWidget> bodyContent,
        bool isCollapsed = false);
    
    /**
     * @brief Creates a responsive grid container
     * @param columns Number of columns
     * @param items Items to add to the grid
     * @return Unique pointer to the grid container
     */
    std::unique_ptr<Wt::WContainerWidget> createResponsiveGrid(
        int columns,
        std::vector<std::unique_ptr<Wt::WWidget>> items);
    
    /**
     * @brief Creates a split panel with left and right sections
     * @param leftContent Content for the left section
     * @param rightContent Content for the right section
     * @param leftWidthPercentage Width percentage for left section (default: 50)
     * @return Unique pointer to the split panel container
     */
    std::unique_ptr<Wt::WContainerWidget> createSplitPanel(
        std::unique_ptr<Wt::WWidget> leftContent,
        std::unique_ptr<Wt::WWidget> rightContent,
        int leftWidthPercentage = 50);
    
    // =================================================================
    // Table Helpers
    // =================================================================
    
    /**
     * @brief Creates a styled data table with headers
     * @param headers Column headers
     * @param striped Whether to use striped rows
     * @param hover Whether to enable hover effects
     * @return Unique pointer to the table
     */
    std::unique_ptr<Wt::WTable> createStyledTable(
        const std::vector<std::string>& headers,
        bool striped = true,
        bool hover = true);
    
    /**
     * @brief Adds a data row to a table
     * @param table Table to add row to
     * @param rowData Data for each column
     * @param isEven Whether this is an even-numbered row
     */
    void addTableRow(Wt::WTable* table,
                    const std::vector<std::string>& rowData,
                    bool isEven = false);
    
    /**
     * @brief Adds an action button to a table cell
     * @param table Table containing the cell
     * @param row Row index
     * @param col Column index
     * @param buttonText Button text
     * @param buttonClass CSS class for the button
     * @param clickHandler Function to call when button is clicked
     */
    void addTableActionButton(Wt::WTable* table,
                             int row, int col,
                             const std::string& buttonText,
                             const std::string& buttonClass,
                             std::function<void()> clickHandler);
    
    // =================================================================
    // Dialog Helpers
    // =================================================================
    
    /**
     * @brief Creates a confirmation dialog
     * @param title Dialog title
     * @param message Confirmation message
     * @param confirmText Text for confirm button (default: "Yes")
     * @param cancelText Text for cancel button (default: "No")
     * @return Unique pointer to the dialog
     */
    std::unique_ptr<Wt::WDialog> createConfirmationDialog(
        const std::string& title,
        const std::string& message,
        const std::string& confirmText = "Yes",
        const std::string& cancelText = "No");
    
    /**
     * @brief Creates an information dialog
     * @param title Dialog title
     * @param message Information message
     * @param okText Text for OK button (default: "OK")
     * @return Unique pointer to the dialog
     */
    std::unique_ptr<Wt::WDialog> createInfoDialog(
        const std::string& title,
        const std::string& message,
        const std::string& okText = "OK");
    
    /**
     * @brief Creates an error dialog
     * @param title Dialog title
     * @param errorMessage Error message
     * @param okText Text for OK button (default: "OK")
     * @return Unique pointer to the dialog
     */
    std::unique_ptr<Wt::WDialog> createErrorDialog(
        const std::string& title,
        const std::string& errorMessage,
        const std::string& okText = "OK");
    
    // =================================================================
    // Styling Helpers
    // =================================================================
    
    /**
     * @brief Applies consistent button styling
     * @param button Button to style
     * @param variant Button variant ("primary", "secondary", "success", etc.)
     * @param size Button size ("sm", "lg", or empty for normal)
     */
    void styleButton(Wt::WPushButton* button,
                    const std::string& variant = "primary",
                    const std::string& size = "");
    
    /**
     * @brief Applies consistent text styling
     * @param text Text widget to style
     * @param variant Text variant ("muted", "primary", "success", etc.)
     * @param size Text size ("small", "large", or empty for normal)
     */
    void styleText(Wt::WText* text,
                  const std::string& variant = "",
                  const std::string& size = "");
    
    /**
     * @brief Applies status badge styling
     * @param text Text widget to style as badge
     * @param status Status type ("success", "warning", "danger", "info")
     */
    void styleBadge(Wt::WText* text, const std::string& status);
    
    /**
     * @brief Applies card styling to a container
     * @param container Container to style
     * @param shadow Whether to add shadow effect
     */
    void styleCard(Wt::WContainerWidget* container, bool shadow = true);
    
    // =================================================================
    // Icon and Visual Helpers
    // =================================================================
    
    /**
     * @brief Creates a text icon (emoji or symbol)
     * @param icon Icon character or emoji
     * @param tooltip Optional tooltip text
     * @return Unique pointer to the icon text widget
     */
    std::unique_ptr<Wt::WText> createIcon(
        const std::string& icon,
        const std::string& tooltip = "");
    
    /**
     * @brief Creates a status indicator with icon and text
     * @param status Status text
     * @param icon Status icon
     * @param colorClass CSS color class
     * @return Unique pointer to the status container
     */
    std::unique_ptr<Wt::WContainerWidget> createStatusIndicator(
        const std::string& status,
        const std::string& icon,
        const std::string& colorClass);
    
    /**
     * @brief Creates a metric display with value and label
     * @param value Metric value
     * @param label Metric label
     * @param valueClass CSS class for the value
     * @return Unique pointer to the metric container
     */
    std::unique_ptr<Wt::WContainerWidget> createMetricDisplay(
        const std::string& value,
        const std::string& label,
        const std::string& valueClass = "");
    
    // =================================================================
    // Animation and Effects Helpers
    // =================================================================
    
    /**
     * @brief Adds fade-in animation to a widget
     * @param widget Widget to animate
     * @param durationMs Animation duration in milliseconds
     */
    void addFadeInAnimation(Wt::WWidget* widget, int durationMs = 300);
    
    /**
     * @brief Adds fade-out animation to a widget
     * @param widget Widget to animate
     * @param durationMs Animation duration in milliseconds
     */
    void addFadeOutAnimation(Wt::WWidget* widget, int durationMs = 300);
    
    /**
     * @brief Adds slide-in animation to a widget
     * @param widget Widget to animate
     * @param direction Slide direction ("left", "right", "up", "down")
     * @param durationMs Animation duration in milliseconds
     */
    void addSlideInAnimation(Wt::WWidget* widget,
                            const std::string& direction = "left",
                            int durationMs = 300);
    
    // =================================================================
    // Responsive Design Helpers
    // =================================================================
    
    /**
     * @brief Adds responsive classes to a widget
     * @param widget Widget to make responsive
     * @param breakpoints Map of breakpoint to class names
     */
    void addResponsiveClasses(Wt::WWidget* widget,
                             const std::map<std::string, std::string>& breakpoints);
    
    /**
     * @brief Creates a responsive container that adapts to screen size
     * @param content Content to make responsive
     * @param mobileClass CSS class for mobile screens
     * @param desktopClass CSS class for desktop screens
     * @return Unique pointer to the responsive container
     */
    std::unique_ptr<Wt::WContainerWidget> createResponsiveContainer(
        std::unique_ptr<Wt::WWidget> content,
        const std::string& mobileClass = "",
        const std::string& desktopClass = "");
    
    // =================================================================
    // Form Helpers
    // =================================================================
    
    /**
     * @brief Creates a form group with label and input
     * @param labelText Label text
     * @param input Input widget
     * @param helpText Optional help text
     * @return Unique pointer to the form group container
     */
    std::unique_ptr<Wt::WContainerWidget> createFormGroup(
        const std::string& labelText,
        std::unique_ptr<Wt::WWidget> input,
        const std::string& helpText = "");
    
    /**
     * @brief Creates a button group container
     * @param buttons Vector of buttons to group
     * @param vertical Whether to arrange buttons vertically
     * @return Unique pointer to the button group container
     */
    std::unique_ptr<Wt::WContainerWidget> createButtonGroup(
        std::vector<std::unique_ptr<Wt::WPushButton>> buttons,
        bool vertical = false);
    
    // =================================================================
    // Utility Functions
    // =================================================================
    
    /**
     * @brief Generates a unique CSS class name
     * @param prefix Class name prefix
     * @return Unique CSS class name
     */
    std::string generateUniqueClassName(const std::string& prefix = "ui");
    
    /**
     * @brief Safely removes a CSS class from a widget
     * @param widget Widget to remove class from
     * @param className Class name to remove
     */
    void safeRemoveStyleClass(Wt::WWidget* widget, const std::string& className);
    
    /**
     * @brief Toggles a CSS class on a widget
     * @param widget Widget to toggle class on
     * @param className Class name to toggle
     * @param force Optional force state (true = add, false = remove)
     */
    void toggleStyleClass(Wt::WWidget* widget,
                         const std::string& className,
                         bool force = false);
    
    /**
     * @brief Centers a widget within its parent container
     * @param widget Widget to center
     * @param horizontal Whether to center horizontally
     * @param vertical Whether to center vertically
     */
    void centerWidget(Wt::WWidget* widget,
                     bool horizontal = true,
                     bool vertical = true);
}

#endif // UIHELPERS_H
