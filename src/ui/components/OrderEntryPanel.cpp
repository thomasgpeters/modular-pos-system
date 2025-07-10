#include "../../../include/ui/components/OrderEntryPanel.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WBreak.h>
#include <iostream>

OrderEntryPanel::OrderEntryPanel(std::shared_ptr<POSService> posService,
                                 std::shared_ptr<EventManager> eventManager)
    : Wt::WContainerWidget()
    , posService_(posService)
    , eventManager_(eventManager)
    , isDestroying_(false)
    , tableSelectionGroup_(nullptr)
    , tableIdentifierLabel_(nullptr)
    , tableIdentifierCombo_(nullptr)
    , tableStatusText_(nullptr)
    , actionsContainer_(nullptr)
    , newOrderButton_(nullptr)
    , sendToKitchenButton_(nullptr)
    , processPaymentButton_(nullptr)
    , availableTableIdentifiers_()
    , eventSubscriptions_()
{
    if (!posService_ || !eventManager_) {
        throw std::invalid_argument("OrderEntryPanel requires valid POSService and EventManager");
    }
    
    setStyleClass("order-entry-panel p-3 bg-white border rounded shadow-sm");
    
    initializeUI();
    setupEventHandlers(); // FIRST setup handlers
    setupEventListeners(); // THEN setup listeners
    
    refresh();
    
    std::cout << "[OrderEntryPanel] Initialized and ready" << std::endl;
}

OrderEntryPanel::~OrderEntryPanel() {
    std::cout << "[OrderEntryPanel] Destructor called - setting destruction flag" << std::endl;
    
    // Set destruction flag to prevent further widget access
    isDestroying_ = true;
    
    try {
        // Unsubscribe from all events
        if (eventManager_) {
            for (auto handle : eventSubscriptions_) {
                eventManager_->unsubscribe(handle);
            }
            eventSubscriptions_.clear();
        }
        
        std::cout << "[OrderEntryPanel] Cleanup completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[OrderEntryPanel] Error during destruction: " << e.what() << std::endl;
    }
}

void OrderEntryPanel::initializeUI() {
    setupTableSelectionSection();
    setupOrderActionsSection();
}

void OrderEntryPanel::setupTableSelectionSection() {
    tableSelectionGroup_ = addNew<Wt::WGroupBox>("Table/Location Selection");
    tableSelectionGroup_->setStyleClass("mb-3 border border-success rounded");
    
    auto formContainer = tableSelectionGroup_->addNew<Wt::WContainerWidget>();
    
    tableIdentifierLabel_ = formContainer->addNew<Wt::WLabel>("Select Table/Location:");
    tableIdentifierLabel_->setStyleClass("form-label fw-bold");
    
    createTableIdentifierCombo(formContainer);
    
    tableStatusText_ = formContainer->addNew<Wt::WText>("Select a table to start");
    tableStatusText_->setStyleClass("text-muted small mt-2");
}

void OrderEntryPanel::setupOrderActionsSection() {
    actionsContainer_ = addNew<Wt::WContainerWidget>();
    actionsContainer_->setStyleClass("mt-3");
    
    auto header = actionsContainer_->addNew<Wt::WText>("Order Actions");
    header->setStyleClass("h5 text-primary mb-3");
    
    auto buttonContainer = actionsContainer_->addNew<Wt::WContainerWidget>();
    buttonContainer->setStyleClass("d-flex gap-2 flex-wrap");
    
    // Create buttons with ACTUAL click handlers
    newOrderButton_ = buttonContainer->addNew<Wt::WPushButton>("🆕 Start New Order");
    newOrderButton_->setStyleClass("btn btn-success");
    newOrderButton_->setEnabled(false);
    
    sendToKitchenButton_ = buttonContainer->addNew<Wt::WPushButton>("🍳 Send to Kitchen");
    sendToKitchenButton_->setStyleClass("btn btn-primary");
    sendToKitchenButton_->setEnabled(false);
    
    processPaymentButton_ = buttonContainer->addNew<Wt::WPushButton>("💳 Process Payment");
    processPaymentButton_->setStyleClass("btn btn-warning");
    processPaymentButton_->setEnabled(false);
}

void OrderEntryPanel::createTableIdentifierCombo(Wt::WContainerWidget* parent) {
    tableIdentifierCombo_ = parent->addNew<Wt::WComboBox>();
    tableIdentifierCombo_->setStyleClass("form-select mb-2");
    
    populateTableIdentifierCombo();
}

void OrderEntryPanel::setupEventHandlers() {
    // CRITICAL: Actually connect button signals to slots
    if (newOrderButton_) {
        newOrderButton_->clicked().connect([this]() {
            std::cout << "[OrderEntryPanel] New Order button clicked!" << std::endl;
            onNewOrderClicked();
        });
    }
    
    if (sendToKitchenButton_) {
        sendToKitchenButton_->clicked().connect([this]() {
            std::cout << "[OrderEntryPanel] Send to Kitchen button clicked!" << std::endl;
            onSendToKitchenClicked();
        });
    }
    
    if (processPaymentButton_) {
        processPaymentButton_->clicked().connect([this]() {
            std::cout << "[OrderEntryPanel] Process Payment button clicked!" << std::endl;
            onProcessPaymentClicked();
        });
    }
    
    if (tableIdentifierCombo_) {
        tableIdentifierCombo_->changed().connect([this]() {
            std::cout << "[OrderEntryPanel] Table selection changed!" << std::endl;
            onTableIdentifierChanged();
        });
    }
    
    std::cout << "[OrderEntryPanel] Event handlers connected" << std::endl;
}

void OrderEntryPanel::setupEventListeners() {
    if (!eventManager_) return;
    
    // Listen for order events and update UI accordingly
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED, 
            [this](const std::any& data) { 
                if (isDestroying_) return;
                std::cout << "[OrderEntryPanel] Order created event received" << std::endl;
                handleOrderCreated(data); 
            })
    );
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { 
                if (isDestroying_) return;
                std::cout << "[OrderEntryPanel] Current order changed event received" << std::endl;
                handleCurrentOrderChanged(data); 
            })
    );
}

// ACTUAL WORKING BUTTON HANDLERS
void OrderEntryPanel::onNewOrderClicked() {
    if (isDestroying_) return;
    
    std::string identifier = getSelectedTableIdentifier();
    
    if (identifier.empty()) {
        showOrderValidationMessage("Please select a table/location first", "error");
        return;
    }
    
    if (hasCurrentOrder()) {
        showOrderValidationMessage("An order is already in progress", "warning");
        return;
    }
    
    std::cout << "[OrderEntryPanel] Creating new order for: " << identifier << std::endl;
    
    // ACTUALLY CREATE THE ORDER
    auto order = posService_->createOrder(identifier);
    if (order) {
        posService_->setCurrentOrder(order);
        showOrderValidationMessage("New order #" + std::to_string(order->getOrderId()) + " created", "success");
        std::cout << "[OrderEntryPanel] Order created successfully: #" << order->getOrderId() << std::endl;
    } else {
        showOrderValidationMessage("Failed to create order", "error");
        std::cout << "[OrderEntryPanel] Failed to create order" << std::endl;
    }
    
    updateOrderActionButtons();
}

void OrderEntryPanel::onSendToKitchenClicked() {
    if (isDestroying_) return;
    
    if (!hasCurrentOrder()) {
        showOrderValidationMessage("No current order to send", "error");
        return;
    }
    
    auto currentOrder = posService_->getCurrentOrder();
    if (!currentOrder || currentOrder->getItems().empty()) {
        showOrderValidationMessage("Cannot send empty order to kitchen", "error");
        return;
    }
    
    std::cout << "[OrderEntryPanel] Sending order to kitchen..." << std::endl;
    
    // ACTUALLY SEND TO KITCHEN
    bool success = posService_->sendCurrentOrderToKitchen();
    if (success) {
        showOrderValidationMessage("Order sent to kitchen successfully", "success");
        std::cout << "[OrderEntryPanel] Order sent to kitchen" << std::endl;
    } else {
        showOrderValidationMessage("Failed to send order to kitchen", "error");
        std::cout << "[OrderEntryPanel] Failed to send to kitchen" << std::endl;
    }
    
    updateOrderActionButtons();
}

void OrderEntryPanel::onProcessPaymentClicked() {
    if (isDestroying_) return;
    
    showOrderValidationMessage("Payment processing not yet implemented", "warning");
    std::cout << "[OrderEntryPanel] Payment processing clicked (placeholder)" << std::endl;
}

void OrderEntryPanel::onTableIdentifierChanged() {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Table identifier changed" << std::endl;
    updateTableStatus();
    updateOrderActionButtons();
}

// EVENT HANDLERS THAT ACTUALLY UPDATE THE UI
void OrderEntryPanel::handleOrderCreated(const std::any& eventData) {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Handling order created event" << std::endl;
    updateOrderActionButtons();
    updateTableStatus();
}

void OrderEntryPanel::handleCurrentOrderChanged(const std::any& eventData) {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Handling current order changed event" << std::endl;
    updateOrderActionButtons();
    updateTableStatus();
}

void OrderEntryPanel::handleOrderModified(const std::any& eventData) {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Handling order modified event" << std::endl;
    updateOrderActionButtons();
}

// ACTUAL WORKING HELPER METHODS
void OrderEntryPanel::updateOrderActionButtons() {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Updating button states..." << std::endl;
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    bool validTableSelection = validateTableIdentifierSelection();
    bool hasOrderItems = hasCurrentOrder && hasOrderWithItems();
    
    std::cout << "[OrderEntryPanel] State: hasCurrentOrder=" << hasCurrentOrder 
              << ", validTableSelection=" << validTableSelection 
              << ", hasOrderItems=" << hasOrderItems << std::endl;
    
    // New order button
    if (newOrderButton_) {
        newOrderButton_->setEnabled(validTableSelection && !hasCurrentOrder);
        if (hasCurrentOrder) {
            newOrderButton_->setText("🆕 Order In Progress");
            newOrderButton_->setStyleClass("btn btn-outline-success");
        } else {
            newOrderButton_->setText("🆕 Start New Order");
            newOrderButton_->setStyleClass("btn btn-success");
        }
    }
    
    // Send to kitchen button
    if (sendToKitchenButton_) {
        sendToKitchenButton_->setEnabled(hasOrderItems);
        if (hasOrderItems) {
            auto currentOrder = posService_->getCurrentOrder();
            int itemCount = currentOrder->getItems().size();
            sendToKitchenButton_->setText("🍳 Send to Kitchen (" + std::to_string(itemCount) + " items)");
            sendToKitchenButton_->setStyleClass("btn btn-primary");
        } else {
            sendToKitchenButton_->setText("🍳 Send to Kitchen");
            sendToKitchenButton_->setStyleClass("btn btn-outline-primary");
        }
    }
    
    // Process payment button
    if (processPaymentButton_) {
        processPaymentButton_->setEnabled(hasOrderItems);
        if (hasOrderItems) {
            auto currentOrder = posService_->getCurrentOrder();
            double total = currentOrder->getTotal();
            processPaymentButton_->setText("💳 Payment ($" + std::to_string((int)total) + ")");
            processPaymentButton_->setStyleClass("btn btn-warning");
        } else {
            processPaymentButton_->setText("💳 Process Payment");
            processPaymentButton_->setStyleClass("btn btn-outline-warning");
        }
    }
}

void OrderEntryPanel::showOrderValidationMessage(const std::string& message, const std::string& type) {
    if (isDestroying_ || !tableStatusText_) return;
    
    std::string icon;
    std::string cssClass;
    
    if (type == "success") {
        icon = "✅";
        cssClass = "text-success fw-bold";
    } else if (type == "warning") {
        icon = "⚠️";
        cssClass = "text-warning fw-bold";
    } else if (type == "error") {
        icon = "❌";
        cssClass = "text-danger fw-bold";
    } else {
        icon = "ℹ️";
        cssClass = "text-info";
    }
    
    tableStatusText_->setText(icon + " " + message);
    tableStatusText_->setStyleClass(cssClass + " small mt-2");
    
    std::cout << "[OrderEntryPanel] " << type << ": " << message << std::endl;
}

bool OrderEntryPanel::hasCurrentOrder() const {
    bool result = posService_ && posService_->getCurrentOrder() != nullptr;
    std::cout << "[OrderEntryPanel] hasCurrentOrder: " << result << std::endl;
    return result;
}

bool OrderEntryPanel::hasOrderWithItems() const {
    auto order = posService_ ? posService_->getCurrentOrder() : nullptr;
    bool result = order && !order->getItems().empty();
    std::cout << "[OrderEntryPanel] hasOrderWithItems: " << result << std::endl;
    return result;
}

bool OrderEntryPanel::validateTableIdentifierSelection() const {
    bool result = !getSelectedTableIdentifier().empty();
    std::cout << "[OrderEntryPanel] validateTableIdentifierSelection: " << result << std::endl;
    return result;
}

std::string OrderEntryPanel::getSelectedTableIdentifier() const {
    if (!tableIdentifierCombo_ || tableIdentifierCombo_->currentIndex() <= 0) {
        return "";
    }
    
    std::string displayText = tableIdentifierCombo_->currentText().toUTF8();
    return extractTableIdentifierFromDisplayText(displayText);
}

void OrderEntryPanel::populateTableIdentifierCombo() {
    if (!tableIdentifierCombo_) return;
    
    tableIdentifierCombo_->clear();
    tableIdentifierCombo_->addItem("-- Select Table/Location --");
    
    auto identifiers = getDefaultTableIdentifiers();
    for (const auto& identifier : identifiers) {
        std::string displayText = formatTableIdentifierForDisplay(identifier);
        tableIdentifierCombo_->addItem(displayText);
    }
    
    tableIdentifierCombo_->setCurrentIndex(0);
}

void OrderEntryPanel::updateTableStatus() {
    if (isDestroying_) return;
    
    std::string identifier = getSelectedTableIdentifier();
    if (identifier.empty()) {
        showOrderValidationMessage("Select a table/location to start ordering", "info");
        return;
    }
    
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
            formatTableIdentifier(identifier) + " selected", 
            "success"
        );
    }
}

void OrderEntryPanel::refresh() {
    if (isDestroying_) return;
    
    updateOrderActionButtons();
    updateTableStatus();
}

// REMAINING HELPER METHODS
std::string OrderEntryPanel::formatTableIdentifier(const std::string& identifier) const {
    if (identifier.find("table ") == 0) {
        return "Table " + identifier.substr(6);
    } else if (identifier == "walk-in") {
        return "Walk-in";
    } else if (identifier == "grubhub") {
        return "GrubHub Delivery";
    } else if (identifier == "ubereats") {
        return "UberEats Delivery";
    }
    return identifier;
}

std::string OrderEntryPanel::extractTableIdentifierFromDisplayText(const std::string& displayText) const {
    if (displayText.find("Table ") != std::string::npos) {
        size_t pos = displayText.find("Table ");
        std::string numberPart = displayText.substr(pos + 6);
        return "table " + numberPart;
    } else if (displayText.find("Walk-in") != std::string::npos) {
        return "walk-in";
    } else if (displayText.find("GrubHub") != std::string::npos) {
        return "grubhub";
    } else if (displayText.find("UberEats") != std::string::npos) {
        return "ubereats";
    }
    return "";
}

std::string OrderEntryPanel::formatTableIdentifierForDisplay(const std::string& identifier) const {
    if (identifier.find("table ") == 0) {
        return "🪑 Table " + identifier.substr(6);
    } else if (identifier == "walk-in") {
        return "🚶 Walk-in";
    } else if (identifier == "grubhub") {
        return "🚗 GrubHub Delivery";
    } else if (identifier == "ubereats") {
        return "🚗 UberEats Delivery";
    }
    return "📋 " + identifier;
}

std::vector<std::string> OrderEntryPanel::getDefaultTableIdentifiers() const {
    return {
        "table 1", "table 2", "table 3", "table 4", "table 5",
        "table 6", "table 7", "table 8", "table 9", "table 10",
        "walk-in", "grubhub", "ubereats"
    };
}

// PLACEHOLDER IMPLEMENTATIONS FOR INTERFACE COMPLIANCE
void OrderEntryPanel::setTableIdentifier(const std::string& tableIdentifier) {
    // Find and select the identifier in the combo box
    if (tableIdentifierCombo_) {
        for (int i = 0; i < tableIdentifierCombo_->count(); ++i) {
            std::string comboText = tableIdentifierCombo_->itemText(i).toUTF8();
            if (extractTableIdentifierFromDisplayText(comboText) == tableIdentifier) {
                tableIdentifierCombo_->setCurrentIndex(i);
                break;
            }
        }
    }
    updateTableStatus();
}

std::string OrderEntryPanel::getTableIdentifier() const {
    return getSelectedTableIdentifier();
}

void OrderEntryPanel::setTableNumber(int tableNumber) {
    setTableIdentifier("table " + std::to_string(tableNumber));
}

int OrderEntryPanel::getTableNumber() const {
    std::string identifier = getSelectedTableIdentifier();
    if (identifier.find("table ") == 0) {
        try {
            return std::stoi(identifier.substr(6));
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

void OrderEntryPanel::setOrderEntryEnabled(bool enabled) {
    if (tableIdentifierCombo_) tableIdentifierCombo_->setEnabled(enabled);
    if (newOrderButton_) newOrderButton_->setEnabled(enabled && validateTableIdentifierSelection());
}

void OrderEntryPanel::setAvailableTableIdentifiers(const std::vector<std::string>& identifiers) {
    availableTableIdentifiers_ = identifiers;
    populateTableIdentifierCombo();
}

bool OrderEntryPanel::validateCurrentOrder() {
    return hasCurrentOrder() && hasOrderWithItems();
}

void OrderEntryPanel::showOrderValidationError(const std::string& message) {
    showOrderValidationMessage(message, "error");
}

std::string OrderEntryPanel::getTableIdentifierIcon(const std::string& identifier) const {
    if (identifier.find("table") == 0) return "🪑";
    if (identifier == "walk-in") return "🚶";
    if (identifier == "grubhub" || identifier == "ubereats") return "🚗";
    return "📋";
}

bool OrderEntryPanel::isTableIdentifierAvailable(const std::string& identifier) const {
    return posService_ ? !posService_->isTableIdentifierInUse(identifier) : true;
}

void OrderEntryPanel::refreshAvailableIdentifiers() {
    populateTableIdentifierCombo();
}

void OrderEntryPanel::createNewOrder(const std::string& tableIdentifier) {
    if (posService_) {
        auto order = posService_->createOrder(tableIdentifier);
        if (order) {
            posService_->setCurrentOrder(order);
            showOrderValidationMessage("New order created for " + formatTableIdentifier(tableIdentifier), "success");
        }
    }
}

void OrderEntryPanel::createNewOrder(int tableNumber) {
    createNewOrder("table " + std::to_string(tableNumber));
}

void OrderEntryPanel::sendCurrentOrderToKitchen() {
    if (posService_) {
        bool success = posService_->sendCurrentOrderToKitchen();
        if (success) {
            showOrderValidationMessage("Order sent to kitchen successfully", "success");
        } else {
            showOrderValidationMessage("Failed to send order to kitchen", "error");
        }
    }
}

void OrderEntryPanel::applyTableSelectionStyling() {
    // Placeholder for styling - applying Bootstrap theme styles
    if (tableSelectionGroup_) {
        UIStyleHelper::applyGroupBoxStyle(tableSelectionGroup_, "table-selection");
    }
    
    if (tableIdentifierCombo_) {
        UIStyleHelper::applyComboBoxStyle(tableIdentifierCombo_, "table-selector");
    }
}

void OrderEntryPanel::updateTableIdentifierStyling() {
    // Dynamic styling based on selection state
    if (tableIdentifierCombo_) {
        std::string selectedId = getSelectedTableIdentifier();
        if (selectedId.empty()) {
            tableIdentifierCombo_->setStyleClass("form-select mb-2");
        } else {
            tableIdentifierCombo_->setStyleClass("form-select mb-2 border-success");
        }
    }
}
