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
    , newOrderButton_(nullptr)
    , sendToKitchenButton_(nullptr)
    , processPaymentButton_(nullptr)
    , menuDisplay_(nullptr)
    , currentOrderDisplay_(nullptr)
{
    if (!posService_ || !eventManager_) {
        throw std::invalid_argument("OrderEntryPanel requires valid POSService and EventManager");
    }
    
    addStyleClass("order-entry-panel");
    
    // Initialize UI and event handling
    initializeUI();
    setupEventListeners();
    setupEventHandlers();
    
    // Initial refresh
    refresh();
    
    std::cout << "[OrderEntryPanel] Initialized successfully" << std::endl;
}

void OrderEntryPanel::refresh() {
    updateOrderActionButtons();
    updateTableStatus();
    refreshAvailableIdentifiers();
    
    // Note: MenuDisplay and CurrentOrderDisplay will be added later
    // For now, focus on getting the basic structure working
}

void OrderEntryPanel::setTableIdentifier(const std::string& tableIdentifier) {
    if (!tableIdentifierCombo_) return;
    
    // Find the identifier in the combo box and select it
    for (int i = 0; i < tableIdentifierCombo_->count(); ++i) {
        std::string itemText = tableIdentifierCombo_->itemText(i).toUTF8();
        std::string cleanIdentifier = extractTableIdentifierFromDisplayText(itemText);
        if (cleanIdentifier == tableIdentifier) {
            tableIdentifierCombo_->setCurrentIndex(i);
            onTableIdentifierChanged();
            break;
        }
    }
}

std::string OrderEntryPanel::getTableIdentifier() const {
    return getSelectedTableIdentifier();
}

void OrderEntryPanel::setTableNumber(int tableNumber) {
    // Legacy compatibility
    std::string tableIdentifier = "table " + std::to_string(tableNumber);
    setTableIdentifier(tableIdentifier);
}

int OrderEntryPanel::getTableNumber() const {
    std::string identifier = getTableIdentifier();
    if (identifier.find("table") == 0) {
        try {
            std::string numberStr = identifier.substr(6); // Remove "table "
            return std::stoi(numberStr);
        } catch (const std::exception&) {
            return 0;
        }
    }
    return 0; // Non-table orders return 0
}

void OrderEntryPanel::setOrderEntryEnabled(bool enabled) {
    if (newOrderButton_) newOrderButton_->setEnabled(enabled);
    if (sendToKitchenButton_) sendToKitchenButton_->setEnabled(enabled);
    if (processPaymentButton_) processPaymentButton_->setEnabled(enabled);
    if (tableIdentifierCombo_) tableIdentifierCombo_->setEnabled(enabled);
}

void OrderEntryPanel::setAvailableTableIdentifiers(const std::vector<std::string>& identifiers) {
    availableTableIdentifiers_ = identifiers;
    populateTableIdentifierCombo();
}

std::string OrderEntryPanel::getSelectedTableIdentifier() const {
    if (!tableIdentifierCombo_ || tableIdentifierCombo_->currentIndex() <= 0) {
        return "";
    }
    
    std::string displayText = tableIdentifierCombo_->currentText().toUTF8();
    return extractTableIdentifierFromDisplayText(displayText);
}

void OrderEntryPanel::initializeUI() {
    // Create main layout
    auto mainLayout = std::make_unique<Wt::WVBoxLayout>();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create table selection section directly
    tableSelectionGroup_ = addNew<Wt::WGroupBox>("Table Selection");
    tableSelectionGroup_->addStyleClass("table-selection-group");
    
    // Table identifier label and combo
    tableIdentifierLabel_ = tableSelectionGroup_->addNew<Wt::WLabel>("Select Table/Location:");
    tableIdentifierLabel_->addStyleClass("form-label");
    
    createTableIdentifierCombo();
    
    // Table status text
    tableStatusText_ = tableSelectionGroup_->addNew<Wt::WText>("Select a table/location");
    tableStatusText_->addStyleClass("table-status-text text-muted small");
    
    // Create order actions section directly
    auto actionsContainer = addNew<Wt::WContainerWidget>();
    actionsContainer->addStyleClass("order-actions-section");
    
    // New order button
    newOrderButton_ = actionsContainer->addNew<Wt::WPushButton>("Start New Order");
    newOrderButton_->addStyleClass("btn btn-success btn-lg mb-2");
    newOrderButton_->setEnabled(false);
    
    // Send to kitchen button
    sendToKitchenButton_ = actionsContainer->addNew<Wt::WPushButton>("Send to Kitchen");
    sendToKitchenButton_->addStyleClass("btn btn-primary mb-2");
    sendToKitchenButton_->setEnabled(false);
    
    // Process payment button
    processPaymentButton_ = actionsContainer->addNew<Wt::WPushButton>("Process Payment");
    processPaymentButton_->addStyleClass("btn btn-warning");
    processPaymentButton_->setEnabled(false);
    
    // TODO: Add MenuDisplay and CurrentOrderDisplay later
    // For now, create placeholder texts
    auto menuPlaceholder = addNew<Wt::WText>("Menu Display will be added here");
    menuPlaceholder->addStyleClass("text-muted p-3 border");
    
    auto orderPlaceholder = addNew<Wt::WText>("Current Order Display will be added here");
    orderPlaceholder->addStyleClass("text-muted p-3 border");
    
    // Apply styling
    applyTableSelectionStyling();
}

void OrderEntryPanel::setupEventListeners() {
    // For now, simplified event handling
    std::cout << "[OrderEntryPanel] Event listeners setup (simplified)" << std::endl;
}

void OrderEntryPanel::setupEventHandlers() {
    // Table identifier combo change
    if (tableIdentifierCombo_) {
        tableIdentifierCombo_->changed().connect([this] {
            onTableIdentifierChanged();
        });
    }
    
    // Button click handlers
    if (newOrderButton_) {
        newOrderButton_->clicked().connect([this] {
            onNewOrderClicked();
        });
    }
    
    if (sendToKitchenButton_) {
        sendToKitchenButton_->clicked().connect([this] {
            onSendToKitchenClicked();
        });
    }
    
    if (processPaymentButton_) {
        processPaymentButton_->clicked().connect([this] {
            onProcessPaymentClicked();
        });
    }
}

void OrderEntryPanel::createTableIdentifierCombo() {
    tableIdentifierCombo_ = tableSelectionGroup_->addNew<Wt::WComboBox>();
    tableIdentifierCombo_->addStyleClass("form-select table-identifier-combo");
    populateTableIdentifierCombo();
}

void OrderEntryPanel::populateTableIdentifierCombo() {
    if (!tableIdentifierCombo_) return;
    
    tableIdentifierCombo_->clear();
    tableIdentifierCombo_->addItem("-- Select Table/Location --");
    
    // Use available identifiers if set, otherwise use defaults
    std::vector<std::string> identifiers = availableTableIdentifiers_.empty() 
        ? getDefaultTableIdentifiers() 
        : availableTableIdentifiers_;
    
    for (const auto& identifier : identifiers) {
        std::string displayText = formatTableIdentifierForDisplay(identifier);
        tableIdentifierCombo_->addItem(displayText);
    }
    
    tableIdentifierCombo_->setCurrentIndex(0);
}

void OrderEntryPanel::handleOrderCreated(const std::any& eventData) {
    refresh();
    updateOrderActionButtons();
}

void OrderEntryPanel::handleOrderModified(const std::any& eventData) {
    updateOrderActionButtons();
}

void OrderEntryPanel::handleCurrentOrderChanged(const std::any& eventData) {
    updateOrderActionButtons();
}

void OrderEntryPanel::onNewOrderClicked() {
    if (!validateTableIdentifierSelection()) {
        showOrderValidationError("Please select a valid table/location");
        return;
    }
    
    std::string tableIdentifier = getSelectedTableIdentifier();
    createNewOrder(tableIdentifier);
}

void OrderEntryPanel::onSendToKitchenClicked() {
    if (!validateCurrentOrder()) {
        showOrderValidationError("No valid order to send to kitchen");
        return;
    }
    
    sendCurrentOrderToKitchen();
}

void OrderEntryPanel::onProcessPaymentClicked() {
    if (!validateCurrentOrder()) {
        showOrderValidationError("No valid order to process payment for");
        return;
    }
    
    std::cout << "[OrderEntryPanel] Process payment clicked" << std::endl;
}

void OrderEntryPanel::onTableIdentifierChanged() {
    updateTableStatus();
    updateOrderActionButtons();
}

void OrderEntryPanel::createNewOrder(const std::string& tableIdentifier) {
    try {
        auto order = posService_->createOrder(tableIdentifier);
        if (order) {
            posService_->setCurrentOrder(order);
            std::cout << "[OrderEntryPanel] Created new order for " << tableIdentifier << std::endl;
        } else {
            showOrderValidationError("Failed to create order for " + tableIdentifier);
        }
    } catch (const std::exception& e) {
        showOrderValidationError("Error creating order: " + std::string(e.what()));
    }
}

void OrderEntryPanel::createNewOrder(int tableNumber) {
    // Legacy compatibility
    std::string tableIdentifier = "table " + std::to_string(tableNumber);
    createNewOrder(tableIdentifier);
}

void OrderEntryPanel::sendCurrentOrderToKitchen() {
    try {
        bool success = posService_->sendCurrentOrderToKitchen();
        if (success) {
            std::cout << "[OrderEntryPanel] Order sent to kitchen successfully" << std::endl;
        } else {
            showOrderValidationError("Failed to send order to kitchen");
        }
    } catch (const std::exception& e) {
        showOrderValidationError("Error sending to kitchen: " + std::string(e.what()));
    }
}

void OrderEntryPanel::updateOrderActionButtons() {
    bool hasCurrentOrder = this->hasCurrentOrder();
    bool validTableSelection = validateTableIdentifierSelection();
    
    // New order button
    if (newOrderButton_) {
        newOrderButton_->setEnabled(validTableSelection && !hasCurrentOrder);
    }
    
    // Send to kitchen button
    if (sendToKitchenButton_) {
        sendToKitchenButton_->setEnabled(hasCurrentOrder);
    }
    
    // Process payment button
    if (processPaymentButton_) {
        processPaymentButton_->setEnabled(hasCurrentOrder);
    }
}

bool OrderEntryPanel::validateCurrentOrder() {
    auto currentOrder = posService_->getCurrentOrder();
    return currentOrder && !currentOrder->getItems().empty();
}

bool OrderEntryPanel::validateTableIdentifierSelection() {
    std::string identifier = getSelectedTableIdentifier();
    return !identifier.empty() && posService_->isValidTableIdentifier(identifier);
}

bool OrderEntryPanel::hasCurrentOrder() const {
    auto currentOrder = posService_->getCurrentOrder();
    return currentOrder != nullptr;
}

void OrderEntryPanel::showOrderValidationError(const std::string& message) {
    std::cerr << "[OrderEntryPanel] Validation Error: " << message << std::endl;
    
    if (tableStatusText_) {
        tableStatusText_->setText("Error: " + message);
        tableStatusText_->addStyleClass("text-danger");
    }
}

void OrderEntryPanel::updateTableStatus() {
    if (!tableStatusText_) return;
    
    std::string identifier = getSelectedTableIdentifier();
    if (identifier.empty()) {
        tableStatusText_->setText("Select a table/location");
        tableStatusText_->removeStyleClass("text-danger text-success");
        tableStatusText_->addStyleClass("text-muted");
        return;
    }
    
    bool isAvailable = isTableIdentifierAvailable(identifier);
    if (isAvailable) {
        tableStatusText_->setText("✓ " + formatTableIdentifier(identifier) + " is available");
        tableStatusText_->removeStyleClass("text-danger text-muted");
        tableStatusText_->addStyleClass("text-success");
    } else {
        tableStatusText_->setText("⚠ " + formatTableIdentifier(identifier) + " is in use");
        tableStatusText_->removeStyleClass("text-success text-muted");
        tableStatusText_->addStyleClass("text-danger");
    }
}

std::string OrderEntryPanel::formatTableIdentifier(const std::string& identifier) const {
    return identifier; // Simple format for now
}

std::string OrderEntryPanel::getTableIdentifierIcon(const std::string& identifier) const {
    if (identifier.find("table") == 0) {
        return "🪑"; // Chair icon for dine-in
    } else if (identifier == "grubhub" || identifier == "ubereats") {
        return "🚗"; // Car icon for delivery
    } else if (identifier == "walk-in") {
        return "🚶"; // Walking icon for walk-in
    }
    return "📋"; // Default clipboard icon
}

bool OrderEntryPanel::isTableIdentifierAvailable(const std::string& identifier) const {
    return !posService_->isTableIdentifierInUse(identifier);
}

void OrderEntryPanel::refreshAvailableIdentifiers() {
    // Update available identifiers based on current state
    populateTableIdentifierCombo();
}

std::string OrderEntryPanel::extractTableIdentifierFromDisplayText(const std::string& displayText) const {
    // Remove icon prefix if present (e.g., "🪑 table 5" -> "table 5")
    size_t spacePos = displayText.find(' ');
    if (spacePos != std::string::npos && spacePos < 3) {
        return displayText.substr(spacePos + 1);
    }
    return displayText;
}

std::string OrderEntryPanel::formatTableIdentifierForDisplay(const std::string& identifier) const {
    return getTableIdentifierIcon(identifier) + " " + identifier;
}

std::vector<std::string> OrderEntryPanel::getDefaultTableIdentifiers() const {
    std::vector<std::string> identifiers;
    
    // Add table numbers 1-20
    for (int i = 1; i <= 20; ++i) {
        identifiers.push_back("table " + std::to_string(i));
    }
    
    // Add special locations
    identifiers.push_back("walk-in");
    identifiers.push_back("grubhub");
    identifiers.push_back("ubereats");
    
    return identifiers;
}

void OrderEntryPanel::applyTableSelectionStyling() {
    if (tableSelectionGroup_) {
        tableSelectionGroup_->addStyleClass("border border-success");
    }
    
    if (tableIdentifierCombo_) {
        tableIdentifierCombo_->addStyleClass("table-identifier-combo");
    }
}

void OrderEntryPanel::updateTableIdentifierStyling() {
    // Apply styling based on current selection
    std::string identifier = getSelectedTableIdentifier();
    
    if (tableIdentifierCombo_) {
        tableIdentifierCombo_->removeStyleClass("table-identifier-dine-in");
        tableIdentifierCombo_->removeStyleClass("table-identifier-delivery");
        tableIdentifierCombo_->removeStyleClass("table-identifier-walk-in");
        
        if (identifier.find("table") == 0) {
            tableIdentifierCombo_->addStyleClass("table-identifier-dine-in");
        } else if (identifier == "grubhub" || identifier == "ubereats") {
            tableIdentifierCombo_->addStyleClass("table-identifier-delivery");
        } else if (identifier == "walk-in") {
            tableIdentifierCombo_->addStyleClass("table-identifier-walk-in");
        }
    }
}
