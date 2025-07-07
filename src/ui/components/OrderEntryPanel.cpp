#include "../../../include/ui/components/OrderEntryPanel.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <iostream>

OrderEntryPanel::OrderEntryPanel(std::shared_ptr<POSService> posService,
                                 std::shared_ptr<EventManager> eventManager)
    : Wt::WContainerWidget()
    , posService_(posService)
    , eventManager_(eventManager)
    , tableSelectionGroup_(nullptr)
    , tableIdentifierLabel_(nullptr)
    , tableIdentifierCombo_(nullptr)
    , tableStatusText_(nullptr)
    , actionsContainer_(nullptr)
    , newOrderButton_(nullptr)
    , sendToKitchenButton_(nullptr)
    , processPaymentButton_(nullptr)
{
    if (!posService_ || !eventManager_) {
        throw std::invalid_argument("OrderEntryPanel requires valid POSService and EventManager");
    }
    
    // Apply consistent container styling
    UIStyleHelper::styleContainer(this, "panel");
    addStyleClass("order-entry-panel");
    
    // Initialize UI and event handling
    initializeUI();
    setupEventListeners();
    setupEventHandlers();
    
    // Initial refresh
    refresh();
    
    std::cout << "[OrderEntryPanel] Initialized with consistent styling" << std::endl;
}

void OrderEntryPanel::initializeUI() {
    // Apply fade-in animation
    UIStyleHelper::addFadeIn(this);
    
    // Setup sections with consistent styling
    setupTableSelectionSection();
    setupOrderActionsSection();
    
    std::cout << "[OrderEntryPanel] UI initialized with consistent styling" << std::endl;
}

void OrderEntryPanel::setupTableSelectionSection() {
    // Create table selection group with consistent styling
    tableSelectionGroup_ = addNew<Wt::WGroupBox>("🏷️ Table/Location Selection");
    UIStyleHelper::styleGroupBox(tableSelectionGroup_, "success");
    
    // Create form container with consistent layout
    auto formContainer = tableSelectionGroup_->addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexColumn(formContainer);
    
    // Table identifier label with consistent styling
    tableIdentifierLabel_ = formContainer->addNew<Wt::WLabel>("Select Table/Location:");
    tableIdentifierLabel_->addStyleClass("form-label fw-bold text-dark");
    UIStyleHelper::styleHeading(tableIdentifierLabel_, 6);
    tableIdentifierLabel_->removeStyleClass("mb-3"); // Reduce margin for form labels
    tableIdentifierLabel_->addStyleClass("mb-2");
    
    // Create combo box with consistent styling
    createTableIdentifierCombo(formContainer);
    
    // Table status text with consistent styling
    tableStatusText_ = formContainer->addNew<Wt::WText>("Select a table/location to start");
    UIStyleHelper::styleStatusText(tableStatusText_, "info");
    tableStatusText_->addStyleClass("pos-status-message mt-2 p-2 bg-light border rounded");
    
    std::cout << "[OrderEntryPanel] Table selection section styled consistently" << std::endl;
}

void OrderEntryPanel::setupOrderActionsSection() {
    // Create order actions container with consistent styling
    actionsContainer_ = addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleContainer(actionsContainer_, "content");
    
    // Create actions header
    auto actionsHeader = actionsContainer_->addNew<Wt::WText>("⚡ Order Actions");
    UIStyleHelper::styleHeading(actionsHeader, 5, "primary");
    
    // Create button container with consistent layout
    auto buttonContainer = actionsContainer_->addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(buttonContainer, "start", "center");
    buttonContainer->addStyleClass("pos-action-buttons-container mt-3");
    
    // New order button with consistent styling
    newOrderButton_ = buttonContainer->addNew<Wt::WPushButton>("🆕 Start New Order");
    UIStyleHelper::styleButton(newOrderButton_, "success", "lg");
    newOrderButton_->setEnabled(false);
    newOrderButton_->addStyleClass("pos-new-order-btn");
    UIStyleHelper::addHoverEffect(newOrderButton_, "lift");
    
    // Send to kitchen button with consistent styling
    sendToKitchenButton_ = buttonContainer->addNew<Wt::WPushButton>("🍳 Send to Kitchen");
    UIStyleHelper::styleButton(sendToKitchenButton_, "primary", "lg");
    sendToKitchenButton_->setEnabled(false);
    sendToKitchenButton_->addStyleClass("pos-kitchen-btn");
    UIStyleHelper::addHoverEffect(sendToKitchenButton_, "lift");
    
    // Process payment button with consistent styling
    processPaymentButton_ = buttonContainer->addNew<Wt::WPushButton>("💳 Process Payment");
    UIStyleHelper::styleButton(processPaymentButton_, "warning", "lg");
    processPaymentButton_->setEnabled(false);
    processPaymentButton_->addStyleClass("pos-payment-btn");
    UIStyleHelper::addHoverEffect(processPaymentButton_, "lift");
    
    // Add responsive spacing for mobile
    UIStyleHelper::addResponsiveSpacing(buttonContainer);
    
    std::cout << "[OrderEntryPanel] Order actions section styled consistently" << std::endl;
}

void OrderEntryPanel::createTableIdentifierCombo(Wt::WContainerWidget* parent) {
    // Create combo as child of the specified parent
    tableIdentifierCombo_ = parent->addNew<Wt::WComboBox>();
    UIStyleHelper::styleComboBox(tableIdentifierCombo_, "md");
    tableIdentifierCombo_->addStyleClass("pos-table-selector");
    
    // Add hover effect
    UIStyleHelper::addHoverEffect(tableIdentifierCombo_, "glow");
    
    populateTableIdentifierCombo();
}

void OrderEntryPanel::updateOrderActionButtons() {
    bool hasCurrentOrder = this->hasCurrentOrder();
    bool validTableSelection = validateTableIdentifierSelection();
    bool hasOrderItems = hasCurrentOrder && hasOrderWithItems();
    
    // New order button styling based on state
    if (newOrderButton_) {
        newOrderButton_->setEnabled(validTableSelection && !hasCurrentOrder);
        
        if (hasCurrentOrder) {
            newOrderButton_->setText("🆕 Order In Progress");
            UIStyleHelper::styleButton(newOrderButton_, "outline-success", "lg");
        } else {
            newOrderButton_->setText("🆕 Start New Order");
            UIStyleHelper::styleButton(newOrderButton_, "success", "lg");
        }
    }
    
    // Send to kitchen button styling based on state
    if (sendToKitchenButton_) {
        sendToKitchenButton_->setEnabled(hasOrderItems);
        
        if (hasOrderItems) {
            auto currentOrder = posService_->getCurrentOrder();
            int itemCount = currentOrder->getItems().size();
            sendToKitchenButton_->setText("🍳 Send to Kitchen (" + std::to_string(itemCount) + " items)");
            UIStyleHelper::styleButton(sendToKitchenButton_, "primary", "lg");
            // Add pulsing animation for ready orders
            sendToKitchenButton_->addStyleClass("pos-pulse-animation");
        } else {
            sendToKitchenButton_->setText("🍳 Send to Kitchen");
            UIStyleHelper::styleButton(sendToKitchenButton_, "outline-primary", "lg");
            sendToKitchenButton_->removeStyleClass("pos-pulse-animation");
        }
    }
    
    // Process payment button styling based on state
    if (processPaymentButton_) {
        processPaymentButton_->setEnabled(hasOrderItems);
        
        if (hasOrderItems) {
            auto currentOrder = posService_->getCurrentOrder();
            double total = currentOrder->getTotal();
            processPaymentButton_->setText("💳 Payment ($" + std::to_string((int)total) + ")");
            UIStyleHelper::styleButton(processPaymentButton_, "warning", "lg");
        } else {
            processPaymentButton_->setText("💳 Process Payment");
            UIStyleHelper::styleButton(processPaymentButton_, "outline-warning", "lg");
        }
    }
    
    std::cout << "[OrderEntryPanel] Button states updated with consistent styling" << std::endl;
}

void OrderEntryPanel::showOrderValidationMessage(const std::string& message, const std::string& type) {
    std::cout << "[OrderEntryPanel] " << type << ": " << message << std::endl;
    
    if (tableStatusText_) {
        std::string icon;
        
        if (type == "success") {
            icon = "✅";
            UIStyleHelper::styleStatusText(tableStatusText_, "success");
            tableStatusText_->removeStyleClass("bg-light");
            tableStatusText_->addStyleClass("bg-success-subtle border-success");
        } else if (type == "warning") {
            icon = "⚠️";
            UIStyleHelper::styleStatusText(tableStatusText_, "warning");
            tableStatusText_->removeStyleClass("bg-light");
            tableStatusText_->addStyleClass("bg-warning-subtle border-warning");
        } else if (type == "error") {
            icon = "❌";
            UIStyleHelper::styleStatusText(tableStatusText_, "error");
            tableStatusText_->removeStyleClass("bg-light");
            tableStatusText_->addStyleClass("bg-danger-subtle border-danger");
        } else {
            icon = "ℹ️";
            UIStyleHelper::styleStatusText(tableStatusText_, "info");
            tableStatusText_->removeStyleClass("bg-success-subtle bg-warning-subtle bg-danger-subtle border-success border-warning border-danger");
            tableStatusText_->addStyleClass("bg-light border");
        }
        
        tableStatusText_->setText(icon + " " + message);
        
        // Add animation for important messages
        if (type == "success" || type == "error") {
            UIStyleHelper::addFadeIn(tableStatusText_);
        }
    }
}

void OrderEntryPanel::updateTableStatus() {
    if (!tableStatusText_) return;
    
    std::string identifier = getSelectedTableIdentifier();
    if (identifier.empty()) {
        showOrderValidationMessage("Select a table/location to start ordering", "info");
        return;
    }
    
    bool isAvailable = isTableIdentifierAvailable(identifier);
    if (isAvailable) {
        if (hasCurrentOrder()) {
            auto currentOrder = posService_->getCurrentOrder();
            int itemCount = currentOrder->getItems().size();
            showOrderValidationMessage(
                "Order #" + std::to_string(currentOrder->getOrderId()) + 
                " active (" + std::to_string(itemCount) + " items)", 
                "success"
            );
        } else {
            showOrderValidationMessage(
                formatTableIdentifier(identifier) + " is available", 
                "success"
            );
        }
    } else {
        showOrderValidationMessage(
            formatTableIdentifier(identifier) + " is currently in use", 
            "warning"
        );
    }
}

void OrderEntryPanel::populateTableIdentifierCombo() {
    if (!tableIdentifierCombo_) return;
    
    tableIdentifierCombo_->clear();
    
    // Add placeholder with styling
    auto placeholderItem = tableIdentifierCombo_->addItem("-- Select Table/Location --");
    
    // Use available identifiers if set, otherwise use defaults
    std::vector<std::string> identifiers = availableTableIdentifiers_.empty() 
        ? getDefaultTableIdentifiers() 
        : availableTableIdentifiers_;
    
    for (const auto& identifier : identifiers) {
        std::string displayText = formatTableIdentifierForDisplay(identifier);
        tableIdentifierCombo_->addItem(displayText);
    }
    
    tableIdentifierCombo_->setCurrentIndex(0);
    
    // Style the combo box based on availability
    updateTableIdentifierStyling();
}

void OrderEntryPanel::updateTableIdentifierStyling() {
    std::string identifier = getSelectedTableIdentifier();
    
    if (tableIdentifierCombo_) {
        // Remove all theme classes
        tableIdentifierCombo_->removeStyleClass("border-success border-warning border-danger");
        
        if (identifier.empty()) {
            // Default styling for placeholder
            tableIdentifierCombo_->addStyleClass("border-secondary");
        } else if (identifier.find("table") == 0) {
            // Dine-in table styling
            tableIdentifierCombo_->addStyleClass("border-success");
        } else if (identifier == "grubhub" || identifier == "ubereats") {
            // Delivery service styling
            tableIdentifierCombo_->addStyleClass("border-warning");
        } else if (identifier == "walk-in") {
            // Walk-in styling
            tableIdentifierCombo_->addStyleClass("border-info");
        }
    }
}

// Apply consistent styling to table selection section
void OrderEntryPanel::applyTableSelectionStyling() {
    if (tableSelectionGroup_) {
        UIStyleHelper::addHoverEffect(tableSelectionGroup_, "shadow");
    }
    
    if (tableIdentifierCombo_) {
        updateTableIdentifierStyling();
    }
}

// Rest of the methods remain the same but with consistent error/success messaging
// (Event handlers, business logic methods, etc. with consistent showOrderValidationMessage calls)
