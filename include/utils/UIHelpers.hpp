#ifndef UIHELPERS_H
#define UIHELPERS_H

#include "../ui/factories/UIComponentFactory.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WSpinBox.h>
#include <Wt/WDoubleSpinBox.h>
#include <Wt/WCheckBox.h>

#include <string>
#include <vector>
#include <memory>

/**
 * @file UIHelpers.hpp
 * @brief Utility functions for UI component creation and styling
 * 
 * This header provides a comprehensive set of utility functions for
 * creating, styling, and managing UI components consistently across
 * the POS system.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @namespace UIHelpers
 * @brief Contains utility functions for UI operations
 */
namespace UIHelpers {
    
    // =================================================================
    // Widget Creation Utilities
    // =================================================================
    
    /**
     * @brief Creates a styled container widget
     * @param cssClass CSS class to apply
     * @return Unique pointer to the container
     */
    std::unique_ptr<Wt::WContainerWidget> createContainer(const std::string& cssClass = "");
    
    /**
     * @brief Creates a styled text widget
     * @param text Text content
     * @param cssClass CSS class to apply
     * @return Unique pointer to the text widget
     */
    std::unique_ptr<Wt::WText> createText(const std::string& text, const std::string& cssClass = "");
    
    /**
     * @brief Creates a styled button
     * @param text Button text
     * @param cssClass CSS class to apply
     * @return Unique pointer to the button
     */
    std::unique_ptr<Wt::WPushButton> createButton(const std::string& text, const std::string& cssClass = "btn btn-primary");
    
    /**
     * @brief Creates a styled input field
     * @param placeholder Placeholder text
     * @param cssClass CSS class to apply
     * @return Unique pointer to the input field
     */
    std::unique_ptr<Wt::WLineEdit> createInput(const std::string& placeholder = "", const std::string& cssClass = "form-control");
    
    /**
     * @brief Creates a styled combo box
     * @param options List of options
     * @param cssClass CSS class to apply
     * @return Unique pointer to the combo box
     */
    std::unique_ptr<Wt::WComboBox> createComboBox(const std::vector<std::string>& options = {}, const std::string& cssClass = "form-control");
    
    /**
     * @brief Creates a styled spin box
     * @param min Minimum value
     * @param max Maximum value
     * @param value Initial value
     * @param cssClass CSS class to apply
     * @return Unique pointer to the spin box
     */
    std::unique_ptr<Wt::WSpinBox> createSpinBox(int min = 0, int max = 100, int value = 0, const std::string& cssClass = "form-control");
    
    /**
     * @brief Creates a styled double spin box
     * @param min Minimum value
     * @param max Maximum value
     * @param value Initial value
     * @param decimals Number of decimal places
     * @param cssClass CSS class to apply
     * @return Unique pointer to the double spin box
     */
    std::unique_ptr<Wt::WDoubleSpinBox> createDoubleSpinBox(double min = 0.0, double max = 999.99, double value = 0.0, int decimals = 2, const std::string& cssClass = "form-control");
    
    /**
     * @brief Creates a styled checkbox
     * @param text Label text
     * @param checked Initial checked state
     * @param cssClass CSS class to apply
     * @return Unique pointer to the checkbox
     */
    std::unique_ptr<Wt::WCheckBox> createCheckBox(const std::string& text = "", bool checked = false, const std::string& cssClass = "form-check-input");
    
    // =================================================================
    // Table Utilities
    // =================================================================
    
    /**
     * @brief Creates a styled table widget
     * @param cssClass CSS class to apply
     * @return Unique pointer to the table
     */
    std::unique_ptr<Wt::WTable> createTable(const std::string& cssClass = "table table-striped");
    
    /**
     * @brief Adds a header row to a table
     * @param table Table to add header to
     * @param headers List of header texts
     */
    void addTableHeader(Wt::WTable* table, const std::vector<std::string>& headers);
    
    /**
     * @brief Adds a data row to a table
     * @param table Table to add row to
     * @param data List of cell data
     * @param rowIndex Row index (use -1 to append)
     * @return Row index of the added row
     */
    int addTableRow(Wt::WTable* table, const std::vector<std::string>& data, int rowIndex = -1);
    
    /**
     * @brief Clears all rows from a table except header
     * @param table Table to clear
     * @param keepHeaderRows Number of header rows to keep
     */
    void clearTableData(Wt::WTable* table, int keepHeaderRows = 1);
    
    /**
     * @brief Sets table cell content
     * @param table Table widget
     * @param row Row index
     * @param column Column index
     * @param content Cell content
     * @param cssClass CSS class for the cell
     */
    void setTableCell(Wt::WTable* table, int row, int column, const std::string& content, const std::string& cssClass = "");
    
    /**
     * @brief Sets table cell widget
     * @param table Table widget
     * @param row Row index
     * @param column Column index
     * @param widget Widget to place in cell
     */
    void setTableCellWidget(Wt::WTable* table, int row, int column, std::unique_ptr<Wt::WWidget> widget);
    
    // =================================================================
    // Layout Utilities
    // =================================================================
    
    /**
     * @brief Creates a horizontal flex container
     * @param cssClass Additional CSS class
     * @return Unique pointer to the container
     */
    std::unique_ptr<Wt::WContainerWidget> createHorizontalContainer(const std::string& cssClass = "d-flex");
    
    /**
     * @brief Creates a vertical flex container
     * @param cssClass Additional CSS class
     * @return Unique pointer to the container
     */
    std::unique_ptr<Wt::WContainerWidget> createVerticalContainer(const std::string& cssClass = "d-flex flex-column");
    
    /**
     * @brief Creates a card container
     * @param title Card title
     * @param cssClass Additional CSS class
     * @return Unique pointer to the card container
     */
    std::unique_ptr<Wt::WContainerWidget> createCard(const std::string& title = "", const std::string& cssClass = "card");
    
    /**
     * @brief Creates a form group container
     * @param label Label text
     * @param cssClass Additional CSS class
     * @return Unique pointer to the form group
     */
    std::unique_ptr<Wt::WContainerWidget> createFormGroup(const std::string& label = "", const std::string& cssClass = "form-group");
    
    // =================================================================
    // Styling Utilities
    // =================================================================
    
    /**
     * @brief Applies Bootstrap button styling
     * @param button Button to style
     * @param variant Button variant (primary, secondary, success, etc.)
     * @param size Button size (sm, lg, etc.)
     */
    void styleButton(Wt::WPushButton* button, const std::string& variant = "primary", const std::string& size = "");
    
    /**
     * @brief Applies responsive table classes
     * @param table Table to style
     * @param responsive Enable responsive wrapper
     * @param striped Enable striped rows
     * @param hover Enable hover effect
     */
    void styleTable(Wt::WTable* table, bool responsive = true, bool striped = true, bool hover = true);
    
    /**
     * @brief Applies form control styling
     * @param widget Widget to style
     * @param size Input size (sm, lg, etc.)
     */
    void styleFormControl(Wt::WWidget* widget, const std::string& size = "");
    
    /**
     * @brief Applies alert styling
     * @param container Container to style as alert
     * @param type Alert type (success, warning, danger, info)
     * @param dismissible Make alert dismissible
     */
    void styleAlert(Wt::WContainerWidget* container, const std::string& type = "info", bool dismissible = false);
    
    /**
     * @brief Applies badge styling
     * @param text Text widget to style as badge
     * @param variant Badge variant (primary, secondary, success, etc.)
     */
    void styleBadge(Wt::WText* text, const std::string& variant = "primary");
    
    // =================================================================
    // Animation and Effects
    // =================================================================
    
    /**
     * @brief Adds fade-in animation to a widget
     * @param widget Widget to animate
     * @param duration Animation duration in milliseconds
     */
    void addFadeInAnimation(Wt::WWidget* widget, int duration = 300);
    
    /**
     * @brief Adds slide-in animation to a widget
     * @param widget Widget to animate
     * @param direction Slide direction (left, right, up, down)
     * @param duration Animation duration in milliseconds
     */
    void addSlideInAnimation(Wt::WWidget* widget, const std::string& direction = "left", int duration = 300);
    
    /**
     * @brief Adds pulse animation to a widget
     * @param widget Widget to animate
     * @param duration Animation duration in milliseconds
     */
    void addPulseAnimation(Wt::WWidget* widget, int duration = 1000);
    
    // =================================================================
    // Responsive Utilities
    // =================================================================
    
    /**
     * @brief Applies responsive visibility classes
     * @param widget Widget to make responsive
     * @param hideOnMobile Hide on mobile devices
     * @param hideOnTablet Hide on tablet devices
     */
    void applyResponsiveVisibility(Wt::WWidget* widget, bool hideOnMobile = false, bool hideOnTablet = false);
    
    /**
     * @brief Applies responsive text sizing
     * @param text Text widget to make responsive
     * @param mobileSize Text size on mobile
     * @param desktopSize Text size on desktop
     */
    void applyResponsiveText(Wt::WText* text, const std::string& mobileSize = "sm", const std::string& desktopSize = "md");
    
    // =================================================================
    // Icon Utilities
    // =================================================================
    
    /**
     * @brief Creates an icon text widget
     * @param iconName Icon name or emoji
     * @param cssClass CSS class for styling
     * @return Unique pointer to the icon widget
     */
    std::unique_ptr<Wt::WText> createIcon(const std::string& iconName, const std::string& cssClass = "icon");
    
    /**
     * @brief Adds an icon to a button
     * @param button Button to add icon to
     * @param iconName Icon name or emoji
     * @param position Icon position (left, right)
     */
    void addButtonIcon(Wt::WPushButton* button, const std::string& iconName, const std::string& position = "left");
    
    // =================================================================
    // Validation Utilities
    // =================================================================
    
    /**
     * @brief Applies validation state to a form control
     * @param widget Widget to style
     * @param state Validation state (valid, invalid, neutral)
     * @param message Validation message
     */
    void applyValidationState(Wt::WWidget* widget, const std::string& state, const std::string& message = "");
    
    /**
     * @brief Clears validation state from a widget
     * @param widget Widget to clear validation from
     */
    void clearValidationState(Wt::WWidget* widget);
}

#endif // UIHELPERS_H