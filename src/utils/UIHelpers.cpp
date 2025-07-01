//=============================================================================
// UIHelpers.cpp
//=============================================================================

#include "../../include/utils/UIHelpers.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WTableCell.h>
#include <Wt/WLabel.h>

namespace UIHelpers {

// =================================================================
// Widget Creation Utilities
// =================================================================

std::unique_ptr<Wt::WContainerWidget> createContainer(const std::string& cssClass) {
    auto container = std::make_unique<Wt::WContainerWidget>();
    if (!cssClass.empty()) {
        container->addStyleClass(cssClass);
    }
    return container;
}

std::unique_ptr<Wt::WText> createText(const std::string& text, const std::string& cssClass) {
    auto textWidget = std::make_unique<Wt::WText>(text);
    if (!cssClass.empty()) {
        textWidget->addStyleClass(cssClass);
    }
    return textWidget;
}

std::unique_ptr<Wt::WPushButton> createButton(const std::string& text, const std::string& cssClass) {
    auto button = std::make_unique<Wt::WPushButton>(text);
    button->addStyleClass(cssClass);
    return button;
}

std::unique_ptr<Wt::WLineEdit> createInput(const std::string& placeholder, const std::string& cssClass) {
    auto input = std::make_unique<Wt::WLineEdit>();
    if (!placeholder.empty()) {
        input->setPlaceholderText(placeholder);
    }
    input->addStyleClass(cssClass);
    return input;
}

std::unique_ptr<Wt::WComboBox> createComboBox(const std::vector<std::string>& options, const std::string& cssClass) {
    auto comboBox = std::make_unique<Wt::WComboBox>();
    
    for (const auto& option : options) {
        comboBox->addItem(option);
    }
    
    comboBox->addStyleClass(cssClass);
    return comboBox;
}

std::unique_ptr<Wt::WSpinBox> createSpinBox(int min, int max, int value, const std::string& cssClass) {
    auto spinBox = std::make_unique<Wt::WSpinBox>();
    spinBox->setMinimum(min);
    spinBox->setMaximum(max);
    spinBox->setValue(value);
    spinBox->addStyleClass(cssClass);
    return spinBox;
}

std::unique_ptr<Wt::WDoubleSpinBox> createDoubleSpinBox(double min, double max, double value, int decimals, const std::string& cssClass) {
    auto doubleSpinBox = std::make_unique<Wt::WDoubleSpinBox>();
    doubleSpinBox->setMinimum(min);
    doubleSpinBox->setMaximum(max);
    doubleSpinBox->setValue(value);
    doubleSpinBox->setDecimals(decimals);
    doubleSpinBox->addStyleClass(cssClass);
    return doubleSpinBox;
}

std::unique_ptr<Wt::WCheckBox> createCheckBox(const std::string& text, bool checked, const std::string& cssClass) {
    auto checkBox = std::make_unique<Wt::WCheckBox>(text);
    checkBox->setChecked(checked);
    checkBox->addStyleClass(cssClass);
    return checkBox;
}

// =================================================================
// Table Utilities
// =================================================================

std::unique_ptr<Wt::WTable> createTable(const std::string& cssClass) {
    auto table = std::make_unique<Wt::WTable>();
    table->addStyleClass(cssClass);
    return table;
}

void addTableHeader(Wt::WTable* table, const std::vector<std::string>& headers) {
    if (!table || headers.empty()) return;
    
    for (size_t i = 0; i < headers.size(); ++i) {
        auto cell = table->elementAt(0, static_cast<int>(i));
        cell->addNew<Wt::WText>(headers[i]);
        cell->addStyleClass("table-header");
    }
}

int addTableRow(Wt::WTable* table, const std::vector<std::string>& data, int rowIndex) {
    if (!table || data.empty()) return -1;
    
    int actualRowIndex = rowIndex;
    if (rowIndex == -1) {
        actualRowIndex = table->rowCount();
    }
    
    for (size_t i = 0; i < data.size(); ++i) {
        auto cell = table->elementAt(actualRowIndex, static_cast<int>(i));
        cell->addNew<Wt::WText>(data[i]);
    }
    
    return actualRowIndex;
}

void clearTableData(Wt::WTable* table, int keepHeaderRows) {
    if (!table) return;
    
    int rowCount = table->rowCount();
    for (int i = rowCount - 1; i >= keepHeaderRows; --i) {
        table->removeRow(i);
    }
}

void setTableCell(Wt::WTable* table, int row, int column, const std::string& content, const std::string& cssClass) {
    if (!table) return;
    
    auto cell = table->elementAt(row, column);
    cell->clear();
    auto text = cell->addNew<Wt::WText>(content);
    
    if (!cssClass.empty()) {
        text->addStyleClass(cssClass);
    }
}

void setTableCellWidget(Wt::WTable* table, int row, int column, std::unique_ptr<Wt::WWidget> widget) {
    if (!table || !widget) return;
    
    auto cell = table->elementAt(row, column);
    cell->clear();
    cell->addWidget(std::move(widget));
}

// =================================================================
// Layout Utilities
// =================================================================

std::unique_ptr<Wt::WContainerWidget> createHorizontalContainer(const std::string& cssClass) {
    auto container = createContainer(cssClass);
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    container->setLayout(std::move(layout));
    return container;
}

std::unique_ptr<Wt::WContainerWidget> createVerticalContainer(const std::string& cssClass) {
    auto container = createContainer(cssClass);
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    container->setLayout(std::move(layout));
    return container;
}

std::unique_ptr<Wt::WContainerWidget> createCard(const std::string& title, const std::string& cssClass) {
    auto card = createContainer(cssClass);
    
    if (!title.empty()) {
        auto cardHeader = card->addNew<Wt::WContainerWidget>();
        cardHeader->addStyleClass("card-header");
        cardHeader->addNew<Wt::WText>(title)->addStyleClass("card-title");
        
        auto cardBody = card->addNew<Wt::WContainerWidget>();
        cardBody->addStyleClass("card-body");
    }
    
    return card;
}

std::unique_ptr<Wt::WContainerWidget> createFormGroup(const std::string& label, const std::string& cssClass) {
    auto formGroup = createContainer(cssClass);
    
    if (!label.empty()) {
        auto labelWidget = formGroup->addNew<Wt::WLabel>(label);
        labelWidget->addStyleClass("form-label");
    }
    
    return formGroup;
}

// =================================================================
// Styling Utilities
// =================================================================

void styleButton(Wt::WPushButton* button, const std::string& variant, const std::string& size) {
    if (!button) return;
    
    button->removeStyleClass("btn-primary");
    button->removeStyleClass("btn-secondary");
    button->removeStyleClass("btn-success");
    button->removeStyleClass("btn-danger");
    button->removeStyleClass("btn-warning");
    button->removeStyleClass("btn-info");
    
    button->addStyleClass("btn");
    button->addStyleClass("btn-" + variant);
    
    if (!size.empty()) {
        button->addStyleClass("btn-" + size);
    }
}

void styleTable(Wt::WTable* table, bool responsive, bool striped, bool hover) {
    if (!table) return;
    
    table->addStyleClass("table");
    
    if (striped) {
        table->addStyleClass("table-striped");
    }
    
    if (hover) {
        table->addStyleClass("table-hover");
    }
    
    if (responsive) {
        // Note: In a real implementation, you'd wrap the table in a responsive container
        table->addStyleClass("table-responsive");
    }
}

void styleFormControl(Wt::WWidget* widget, const std::string& size) {
    if (!widget) return;
    
    widget->addStyleClass("form-control");
    
    if (!size.empty()) {
        widget->addStyleClass("form-control-" + size);
    }
}

void styleAlert(Wt::WContainerWidget* container, const std::string& type, bool dismissible) {
    if (!container) return;
    
    container->addStyleClass("alert");
    container->addStyleClass("alert-" + type);
    
    if (dismissible) {
        container->addStyleClass("alert-dismissible");
    }
}

void styleBadge(Wt::WText* text, const std::string& variant) {
    if (!text) return;
    
    text->addStyleClass("badge");
    text->addStyleClass("badge-" + variant);
}

// =================================================================
// Animation and Effects
// =================================================================

void addFadeInAnimation(Wt::WWidget* widget, int duration) {
    if (!widget) return;
    
    widget->addStyleClass("fade-in");
    widget->setAttributeValue("style", 
        widget->attributeValue("style").toUTF8() + 
        " animation: fadeIn " + std::to_string(duration) + "ms ease-in-out;");
}

void addSlideInAnimation(Wt::WWidget* widget, const std::string& direction, int duration) {
    if (!widget) return;
    
    widget->addStyleClass("slide-in-" + direction);
    widget->setAttributeValue("style", 
        widget->attributeValue("style").toUTF8() + 
        " animation: slideIn" + direction + " " + std::to_string(duration) + "ms ease-in-out;");
}

void addPulseAnimation(Wt::WWidget* widget, int duration) {
    if (!widget) return;
    
    widget->addStyleClass("pulse");
    widget->setAttributeValue("style", 
        widget->attributeValue("style").toUTF8() + 
        " animation: pulse " + std::to_string(duration) + "ms infinite;");
}

// =================================================================
// Responsive Utilities
// =================================================================

void applyResponsiveVisibility(Wt::WWidget* widget, bool hideOnMobile, bool hideOnTablet) {
    if (!widget) return;
    
    if (hideOnMobile) {
        widget->addStyleClass("d-none d-md-block");
    }
    
    if (hideOnTablet) {
        widget->addStyleClass("d-none d-lg-block");
    }
}

void applyResponsiveText(Wt::WText* text, const std::string& mobileSize, const std::string& desktopSize) {
    if (!text) return;
    
    text->addStyleClass("text-" + mobileSize + " text-md-" + desktopSize);
}

// =================================================================
// Icon Utilities
// =================================================================

std::unique_ptr<Wt::WText> createIcon(const std::string& iconName, const std::string& cssClass) {
    auto icon = createText(iconName, cssClass);
    icon->addStyleClass("icon");
    return icon;
}

void addButtonIcon(Wt::WPushButton* button, const std::string& iconName, const std::string& position) {
    if (!button) return;
    
    std::string currentText = button->text().toUTF8();
    
    if (position == "left") {
        button->setText(iconName + " " + currentText);
    } else {
        button->setText(currentText + " " + iconName);
    }
}

// =================================================================
// Validation Utilities
// =================================================================

void applyValidationState(Wt::WWidget* widget, const std::string& state, const std::string& message) {
    if (!widget) return;
    
    // Clear existing validation classes
    clearValidationState(widget);
    
    if (state == "valid") {
        widget->addStyleClass("is-valid");
    } else if (state == "invalid") {
        widget->addStyleClass("is-invalid");
    }
    
    // Note: In a real implementation, you'd also create/update feedback elements
}

void clearValidationState(Wt::WWidget* widget) {
    if (!widget) return;
    
    widget->removeStyleClass("is-valid");
    widget->removeStyleClass("is-invalid");
}

} // namespace UIHelpers
