#include "UIHelpers.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGridLayout.h>
#include <Wt/WBorderLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WTableCell.h>
#include <Wt/WMessageBox.h>
#include <Wt/WApplication.h>

#include <sstream>
#include <random>
#include <chrono>

namespace UIHelpers {

// =================================================================
// Container and Layout Helpers
// =================================================================

std::unique_ptr<Wt::WContainerWidget> createCard(
    const std::string& title,
    std::unique_ptr<Wt::WWidget> bodyContent,
    std::unique_ptr<Wt::WWidget> headerActions) {
    
    auto card = std::make_unique<Wt::WContainerWidget>();
    card->addStyleClass("card");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Card header
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("card-header");
    
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Title
    auto titleText = std::make_unique<Wt::WText>(title);
    titleText->addStyleClass("card-title h5 mb-0");
    headerLayout->addWidget(std::move(titleText), 1);
    
    // Header actions
    if (headerActions) {
        headerLayout->addWidget(std::move(headerActions));
    }
    
    header->setLayout(std::move(headerLayout));
    layout->addWidget(std::move(header));
    
    // Card body
    auto body = std::make_unique<Wt::WContainerWidget>();
    body->addStyleClass("card-body");
    body->addWidget(std::move(bodyContent));
    layout->addWidget(std::move(body), 1);
    
    card->setLayout(std::move(layout));
    return card;
}

std::unique_ptr<Wt::WContainerWidget> createCollapsiblePanel(
    const std::string& title,
    std::unique_ptr<Wt::WWidget> bodyContent,
    bool isCollapsed) {
    
    auto panel = std::make_unique<Wt::WContainerWidget>();
    panel->addStyleClass("collapsible-panel");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Panel header with toggle button
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("panel-header");
    
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto toggleButton = std::make_unique<Wt::WPushButton>(isCollapsed ? "▶" : "▼");
    toggleButton->addStyleClass("btn btn-sm btn-link toggle-button");
    headerLayout->addWidget(std::move(toggleButton));
    
    auto titleText = std::make_unique<Wt::WText>(title);
    titleText->addStyleClass("panel-title font-weight-bold");
    headerLayout->addWidget(std::move(titleText), 1);
    
    header->setLayout(std::move(headerLayout));
    layout->addWidget(std::move(header));
    
    // Panel body
    auto body = std::make_unique<Wt::WContainerWidget>();
    body->addStyleClass("panel-body");
    if (isCollapsed) {
        body->hide();
    }
    body->addWidget(std::move(bodyContent));
    layout->addWidget(std::move(body));
    
    panel->setLayout(std::move(layout));
    return panel;
}

std::unique_ptr<Wt::WContainerWidget> createResponsiveGrid(
    int columns,
    std::vector<std::unique_ptr<Wt::WWidget>> items) {
    
    auto grid = std::make_unique<Wt::WContainerWidget>();
    grid->addStyleClass("responsive-grid");
    
    auto layout = std::make_unique<Wt::WGridLayout>();
    
    int row = 0, col = 0;
    for (auto& item : items) {
        layout->addWidget(std::move(item), row, col);
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
    
    grid->setLayout(std::move(layout));
    return grid;
}

std::unique_ptr<Wt::WContainerWidget> createSplitPanel(
    std::unique_ptr<Wt::WWidget> leftContent,
    std::unique_ptr<Wt::WWidget> rightContent,
    int leftWidthPercentage) {
    
    auto splitPanel = std::make_unique<Wt::WContainerWidget>();
    splitPanel->addStyleClass("split-panel");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Left panel
    auto leftPanel = std::make_unique<Wt::WContainerWidget>();
    leftPanel->addStyleClass("split-panel-left");
    leftPanel->addWidget(std::move(leftContent));
    layout->addWidget(std::move(leftPanel), leftWidthPercentage);
    
    // Right panel
    auto rightPanel = std::make_unique<Wt::WContainerWidget>();
    rightPanel->addStyleClass("split-panel-right");
    rightPanel->addWidget(std::move(rightContent));
    layout->addWidget(std::move(rightPanel), 100 - leftWidthPercentage);
    
    splitPanel->setLayout(std::move(layout));
    return splitPanel;
}

// =================================================================
// Table Helpers
// =================================================================

std::unique_ptr<Wt::WTable> createStyledTable(
    const std::vector<std::string>& headers,
    bool striped,
    bool hover) {
    
    auto table = std::make_unique<Wt::WTable>();
    table->addStyleClass("table");
    
    if (striped) {
        table->addStyleClass("table-striped");
    }
    
    if (hover) {
        table->addStyleClass("table-hover");
    }
    
    table->setHeaderCount(1);
    
    // Add headers
    for (size_t i = 0; i < headers.size(); ++i) {
        auto headerText = std::make_unique<Wt::WText>(headers[i]);
        headerText->addStyleClass("table-header-text");
        table->elementAt(0, static_cast<int>(i))->addWidget(std::move(headerText));
        table->elementAt(0, static_cast<int>(i))->addStyleClass("table-header");
    }
    
    return table;
}

void addTableRow(Wt::WTable* table,
                const std::vector<std::string>& rowData,
                bool isEven) {
    if (!table) return;
    
    int row = table->rowCount();
    
    for (size_t col = 0; col < rowData.size(); ++col) {
        auto cellText = std::make_unique<Wt::WText>(rowData[col]);
        table->elementAt(row, static_cast<int>(col))->addWidget(std::move(cellText));
        
        std::string rowClass = isEven ? "table-row-even" : "table-row-odd";
        table->elementAt(row, static_cast<int>(col))->addStyleClass(rowClass);
    }
}

void addTableActionButton(Wt::WTable* table,
                         int row, int col,
                         const std::string& buttonText,
                         const std::string& buttonClass,
                         std::function<void()> clickHandler) {
    if (!table) return;
    
    auto button = std::make_unique<Wt::WPushButton>(buttonText);
    button->addStyleClass(buttonClass);
    
    if (clickHandler) {
        button->clicked().connect(clickHandler);
    }
    
    table->elementAt(row, col)->addWidget(std::move(button));
}

// =================================================================
// Dialog Helpers
// =================================================================

std::unique_ptr<Wt::WDialog> createConfirmationDialog(
    const std::string& title,
    const std::string& message,
    const std::string& confirmText,
    const std::string& cancelText) {
    
    auto dialog = std::make_unique<Wt::WDialog>(title);
    dialog->setModal(true);
    dialog->setResizable(false);
    dialog->addStyleClass("confirmation-dialog");
    
    // Message
    auto messageText = std::make_unique<Wt::WText>(message);
    messageText->addStyleClass("dialog-message");
    dialog->contents()->addWidget(std::move(messageText));
    
    // Buttons
    auto buttonContainer = std::make_unique<Wt::WContainerWidget>();
    auto buttonLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto confirmButton = std::make_unique<Wt::WPushButton>(confirmText);
    confirmButton->addStyleClass("btn btn-primary me-2");
    confirmButton->clicked().connect([dialog = dialog.get()]() {
        dialog->accept();
    });
    buttonLayout->addWidget(std::move(confirmButton));
    
    auto cancelButton = std::make_unique<Wt::WPushButton>(cancelText);
    cancelButton->addStyleClass("btn btn-secondary");
    cancelButton->clicked().connect([dialog = dialog.get()]() {
        dialog->reject();
    });
    buttonLayout->addWidget(std::move(cancelButton));
    
    buttonContainer->setLayout(std::move(buttonLayout));
    dialog->footer()->addWidget(std::move(buttonContainer));
    
    return dialog;
}

std::unique_ptr<Wt::WDialog> createInfoDialog(
    const std::string& title,
    const std::string& message,
    const std::string& okText) {
    
    auto dialog = std::make_unique<Wt::WDialog>(title);
    dialog->setModal(true);
    dialog->setResizable(false);
    dialog->addStyleClass("info-dialog");
    
    // Icon and message
    auto contentContainer = std::make_unique<Wt::WContainerWidget>();
    auto contentLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto icon = createIcon("ℹ️", "Information");
    icon->addStyleClass("dialog-icon me-3");
    contentLayout->addWidget(std::move(icon));
    
    auto messageText = std::make_unique<Wt::WText>(message);
    messageText->addStyleClass("dialog-message");
    contentLayout->addWidget(std::move(messageText), 1);
    
    contentContainer->setLayout(std::move(contentLayout));
    dialog->contents()->addWidget(std::move(contentContainer));
    
    // OK button
    auto okButton = std::make_unique<Wt::WPushButton>(okText);
    okButton->addStyleClass("btn btn-primary");
    okButton->clicked().connect([dialog = dialog.get()]() {
        dialog->accept();
    });
    dialog->footer()->addWidget(std::move(okButton));
    
    return dialog;
}

std::unique_ptr<Wt::WDialog> createErrorDialog(
    const std::string& title,
    const std::string& errorMessage,
    const std::string& okText) {
    
    auto dialog = std::make_unique<Wt::WDialog>(title);
    dialog->setModal(true);
    dialog->setResizable(false);
    dialog->addStyleClass("error-dialog");
    
    // Icon and message
    auto contentContainer = std::make_unique<Wt::WContainerWidget>();
    auto contentLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto icon = createIcon("❌", "Error");
    icon->addStyleClass("dialog-icon me-3");
    contentLayout->addWidget(std::move(icon));
    
    auto messageText = std::make_unique<Wt::WText>(errorMessage);
    messageText->addStyleClass("dialog-message text-danger");
    contentLayout->addWidget(std::move(messageText), 1);
    
    contentContainer->setLayout(std::move(contentLayout));
    dialog->contents()->addWidget(std::move(contentContainer));
    
    // OK button
    auto okButton = std::make_unique<Wt::WPushButton>(okText);
    okButton->addStyleClass("btn btn-danger");
    okButton->clicked().connect([dialog = dialog.get()]() {
        dialog->accept();
    });
    dialog->footer()->addWidget(std::move(okButton));
    
    return dialog;
}

// =================================================================
// Styling Helpers
// =================================================================

void styleButton(Wt::WPushButton* button,
                const std::string& variant,
                const std::string& size) {
    if (!button) return;
    
    button->addStyleClass("btn");
    
    if (!variant.empty()) {
        button->addStyleClass("btn-" + variant);
    }
    
    if (!size.empty()) {
        button->addStyleClass("btn-" + size);
    }
}

void styleText(Wt::WText* text,
              const std::string& variant,
              const std::string& size) {
    if (!text) return;
    
    if (!variant.empty()) {
        text->addStyleClass("text-" + variant);
    }
    
    if (size == "small") {
        text->addStyleClass("small");
    } else if (size == "large") {
        text->addStyleClass("h4");
    }
}

void styleBadge(Wt::WText* text, const std::string& status) {
    if (!text) return;
    
    text->addStyleClass("badge");
    if (!status.empty()) {
        text->addStyleClass("badge-" + status);
    }
}

void styleCard(Wt::WContainerWidget* container, bool shadow) {
    if (!container) return;
    
    container->addStyleClass("card");
    if (shadow) {
        container->addStyleClass("shadow");
    }
}

// =================================================================
// Icon and Visual Helpers
// =================================================================

std::unique_ptr<Wt::WText> createIcon(const std::string& icon, const std::string& tooltip) {
    auto iconWidget = std::make_unique<Wt::WText>(icon);
    iconWidget->addStyleClass("icon");
    
    if (!tooltip.empty()) {
        iconWidget->setToolTip(tooltip);
    }
    
    return iconWidget;
}

std::unique_ptr<Wt::WContainerWidget> createStatusIndicator(
    const std::string& status,
    const std::string& icon,
    const std::string& colorClass) {
    
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("status-indicator");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    auto iconWidget = createIcon(icon);
    iconWidget->addStyleClass(colorClass + " me-1");
    layout->addWidget(std::move(iconWidget));
    
    auto statusText = std::make_unique<Wt::WText>(status);
    statusText->addStyleClass(colorClass);
    layout->addWidget(std::move(statusText));
    
    container->setLayout(std::move(layout));
    return container;
}

std::unique_ptr<Wt::WContainerWidget> createMetricDisplay(
    const std::string& value,
    const std::string& label,
    const std::string& valueClass) {
    
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("metric-display text-center");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    auto valueText = std::make_unique<Wt::WText>(value);
    valueText->addStyleClass("metric-value h3 mb-0");
    if (!valueClass.empty()) {
        valueText->addStyleClass(valueClass);
    }
    layout->addWidget(std::move(valueText));
    
    auto labelText = std::make_unique<Wt::WText>(label);
    labelText->addStyleClass("metric-label small text-muted");
    layout->addWidget(std::move(labelText));
    
    container->setLayout(std::move(layout));
    return container;
}

// =================================================================
// Animation and Effects Helpers
// =================================================================

void addFadeInAnimation(Wt::WWidget* widget, int durationMs) {
    if (!widget) return;
    
    widget->addStyleClass("fade-in");
    // In a real implementation, you might set CSS animation duration
    // widget->setAttributeValue("style", "animation-duration: " + std::to_string(durationMs) + "ms");
}

void addFadeOutAnimation(Wt::WWidget* widget, int durationMs) {
    if (!widget) return;
    
    widget->addStyleClass("fade-out");
    // In a real implementation, you might set CSS animation duration
    // widget->setAttributeValue("style", "animation-duration: " + std::to_string(durationMs) + "ms");
}

void addSlideInAnimation(Wt::WWidget* widget, const std::string& direction, int durationMs) {
    if (!widget) return;
    
    widget->addStyleClass("slide-in-" + direction);
    // In a real implementation, you might set CSS animation duration
}

// =================================================================
// Responsive Design Helpers
// =================================================================

void addResponsiveClasses(Wt::WWidget* widget,
                         const std::map<std::string, std::string>& breakpoints) {
    if (!widget) return;
    
    for (const auto& breakpoint : breakpoints) {
        widget->addStyleClass(breakpoint.first + "-" + breakpoint.second);
    }
}

std::unique_ptr<Wt::WContainerWidget> createResponsiveContainer(
    std::unique_ptr<Wt::WWidget> content,
    const std::string& mobileClass,
    const std::string& desktopClass) {
    
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("responsive-container");
    
    if (!mobileClass.empty()) {
        container->addStyleClass("d-block d-md-none " + mobileClass);
    }
    
    if (!desktopClass.empty()) {
        container->addStyleClass("d-none d-md-block " + desktopClass);
    }
    
    container->addWidget(std::move(content));
    return container;
}

// =================================================================
// Form Helpers
// =================================================================

std::unique_ptr<Wt::WContainerWidget> createFormGroup(
    const std::string& labelText,
    std::unique_ptr<Wt::WWidget> input,
    const std::string& helpText) {
    
    auto formGroup = std::make_unique<Wt::WContainerWidget>();
    formGroup->addStyleClass("form-group mb-3");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Label
    auto label = std::make_unique<Wt::WLabel>(labelText);
    label->addStyleClass("form-label");
    layout->addWidget(std::move(label));
    
    // Input
    input->addStyleClass("form-control");
    layout->addWidget(std::move(input));
    
    // Help text
    if (!helpText.empty()) {
        auto help = std::make_unique<Wt::WText>(helpText);
        help->addStyleClass("form-text text-muted small");
        layout->addWidget(std::move(help));
    }
    
    formGroup->setLayout(std::move(layout));
    return formGroup;
}

std::unique_ptr<Wt::WContainerWidget> createButtonGroup(
    std::vector<std::unique_ptr<Wt::WPushButton>> buttons,
    bool vertical) {
    
    auto buttonGroup = std::make_unique<Wt::WContainerWidget>();
    buttonGroup->addStyleClass("btn-group");
    
    if (vertical) {
        buttonGroup->addStyleClass("btn-group-vertical");
        auto layout = std::make_unique<Wt::WVBoxLayout>();
        for (auto& button : buttons) {
            layout->addWidget(std::move(button));
        }
        buttonGroup->setLayout(std::move(layout));
    } else {
        auto layout = std::make_unique<Wt::WHBoxLayout>();
        for (auto& button : buttons) {
            layout->addWidget(std::move(button));
        }
        buttonGroup->setLayout(std::move(layout));
    }
    
    return buttonGroup;
}

// =================================================================
// Utility Functions
// =================================================================

std::string generateUniqueClassName(const std::string& prefix) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    std::ostringstream oss;
    oss << prefix << "-" << timestamp << "-" << dis(gen);
    return oss.str();
}

void safeRemoveStyleClass(Wt::WWidget* widget, const std::string& className) {
    if (!widget) return;
    
    widget->removeStyleClass(className);
}

void toggleStyleClass(Wt::WWidget* widget, const std::string& className, bool force) {
    if (!widget) return;
    
    if (force) {
        widget->addStyleClass(className);
    } else {
        widget->removeStyleClass(className);
    }
}

void centerWidget(Wt::WWidget* widget, bool horizontal, bool vertical) {
    if (!widget) return;
    
    if (horizontal) {
        widget->addStyleClass("mx-auto");
    }
    
    if (vertical) {
        widget->addStyleClass("d-flex align-items-center");
    }
    
    if (horizontal && vertical) {
        widget->addStyleClass("justify-content-center");
    }
}

} // namespace UIHelpers
