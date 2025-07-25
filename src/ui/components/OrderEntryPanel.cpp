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

// Complete simplified initializeUI() method:
void OrderEntryPanel::initializeUI() {
    setupTableSelectionSection();
    setupOrderActionsSection();
    
    std::cout << "[OrderEntryPanel] UI initialized (simplified version)" << std::endl;
}

// Replace setupTableSelectionSection() method:
void OrderEntryPanel::setupTableSelectionSection() {
    // FIXED: Use regular container instead of WGroupBox to remove border and header
    auto tableSelectionContainer = addNew<Wt::WContainerWidget>();
    tableSelectionContainer->setStyleClass("mb-4"); // Just margin, no border
    
    tableIdentifierLabel_ = tableSelectionContainer->addNew<Wt::WLabel>("Select Table/Location:");
    tableIdentifierLabel_->setStyleClass("form-label fw-bold mb-2");
    
    createTableIdentifierCombo(tableSelectionContainer);
    
    tableStatusText_ = tableSelectionContainer->addNew<Wt::WText>("Select a table to start");
    tableStatusText_->setStyleClass("text-muted small mt-2");
    
    // Store reference for styling methods (since we no longer have tableSelectionGroup_)
    tableSelectionGroup_ = nullptr; // Clear the pointer since we're not using a group box
}

// Replace setupOrderActionsSection() method:
void OrderEntryPanel::setupOrderActionsSection() {
    actionsContainer_ = addNew<Wt::WContainerWidget>();
    actionsContainer_->setStyleClass("mt-4 d-flex justify-content-center");
    
    // FIXED: Only create the "Start New Order" button
    newOrderButton_ = actionsContainer_->addNew<Wt::WPushButton>("🆕 Start New Order");
    newOrderButton_->setStyleClass("btn btn-success btn-lg px-4 py-2");
    newOrderButton_->setEnabled(false);
    
    // REMOVED: sendToKitchenButton_ and processPaymentButton_
    // These will be handled in the Order Details Display later
    sendToKitchenButton_ = nullptr;
    processPaymentButton_ = nullptr;
}

void OrderEntryPanel::createTableIdentifierCombo(Wt::WContainerWidget* parent) {
    tableIdentifierCombo_ = parent->addNew<Wt::WComboBox>();
    tableIdentifierCombo_->setStyleClass("form-select mb-2");
    
    populateTableIdentifierCombo();
}

// Update setupEventHandlers() method to remove handlers for deleted buttons:
void OrderEntryPanel::setupEventHandlers() {
    // Connect handler for the Start New Order button
    if (newOrderButton_) {
        newOrderButton_->clicked().connect([this]() {
            std::cout << "[OrderEntryPanel] New Order button clicked!" << std::endl;
            onNewOrderClicked();
        });
    }
    
    // Connect handler for table selection changes
    if (tableIdentifierCombo_) {
        tableIdentifierCombo_->changed().connect([this]() {
            std::cout << "[OrderEntryPanel] Table selection changed!" << std::endl;
            onTableIdentifierChanged();
        });
    }
    
    // REMOVED: Event handlers for sendToKitchenButton_ and processPaymentButton_
    
    std::cout << "[OrderEntryPanel] Event handlers connected (simplified)" << std::endl;
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

// ============================================================================
// Enhanced OrderEntryPanel::onNewOrderClicked - Better Mode Switch Integration
// Replace the existing onNewOrderClicked method with this enhanced version
// ============================================================================

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
    
    try {
        // STEP 1: Create the order
        auto order = posService_->createOrder(identifier);
        if (!order) {
            showOrderValidationMessage("Failed to create order", "error");
            std::cout << "[OrderEntryPanel] ❌ Failed to create order" << std::endl;
            return;
        }
        
        std::cout << "[OrderEntryPanel] ✓ Order #" << order->getOrderId() << " created" << std::endl;
        
        // STEP 2: Set as current order (this triggers CURRENT_ORDER_CHANGED event)
        posService_->setCurrentOrder(order);
        
        // STEP 3: Show success message
        showOrderValidationMessage(
            "Order #" + std::to_string(order->getOrderId()) + " created - switching to edit mode...", 
            "success"
        );
        
        // STEP 4: Update button state immediately
        updateOrderActionButtons();
        
        std::cout << "[OrderEntryPanel] ✓ Order created and set as current - mode switch should trigger" << std::endl;
        
        // The POSModeContainer will automatically switch to edit mode via the event system
        
    } catch (const std::exception& e) {
        std::cerr << "[OrderEntryPanel] Exception creating order: " << e.what() << std::endl;
        showOrderValidationMessage("Error creating order: " + std::string(e.what()), "error");
    }
}

void OrderEntryPanel::onTableIdentifierChanged() {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Table identifier changed" << std::endl;
    updateTableStatus();
    updateOrderActionButtons();
}

void OrderEntryPanel::handleOrderCreated(const std::any& eventData) {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Order created event received" << std::endl;
    updateOrderActionButtons();
    updateTableStatus();
}

// ============================================================================
// Enhanced event handlers for better integration
// ============================================================================

void OrderEntryPanel::handleCurrentOrderChanged(const std::any& eventData) {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Current order changed event received" << std::endl;
    
    bool hasOrder = hasCurrentOrder();
    std::cout << "[OrderEntryPanel] hasCurrentOrder: " << hasOrder << std::endl;
    
    if (hasOrder) {
        auto order = posService_->getCurrentOrder();
        if (order) {
            std::cout << "[OrderEntryPanel] Order #" << order->getOrderId() 
                      << " is now current - UI will switch to edit mode" << std::endl;
        }
    } else {
        std::cout << "[OrderEntryPanel] No current order - remaining in entry mode" << std::endl;
    }
    
    updateOrderActionButtons();
    updateTableStatus();
}

void OrderEntryPanel::handleOrderModified(const std::any& eventData) {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Handling order modified event" << std::endl;
    updateOrderActionButtons();
}

// ============================================================================
// Enhanced updateOrderActionButtons method
// ============================================================================

void OrderEntryPanel::updateOrderActionButtons() {
    if (isDestroying_) return;
    
    std::cout << "[OrderEntryPanel] Updating button state..." << std::endl;
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    bool validTableSelection = validateTableIdentifierSelection();
    
    std::cout << "[OrderEntryPanel] State: hasCurrentOrder=" << hasCurrentOrder 
              << ", validTableSelection=" << validTableSelection << std::endl;
    
    if (newOrderButton_) {
        if (hasCurrentOrder) {
            // Order exists - button shows status
            auto order = posService_->getCurrentOrder();
            if (order) {
                newOrderButton_->setText("✅ Order #" + std::to_string(order->getOrderId()) + " Active");
                newOrderButton_->setStyleClass("btn btn-success btn-lg px-4 py-2");
                newOrderButton_->setEnabled(false);
                
                // Add a subtitle showing table
                if (tableStatusText_) {
                    tableStatusText_->setText("📝 Switch to Edit Mode to add items");
                    tableStatusText_->setStyleClass("text-success fw-bold small mt-2");
                }
            }
        } else {
            // No order - button allows creation
            newOrderButton_->setText("🆕 Start New Order");
            newOrderButton_->setStyleClass("btn btn-success btn-lg px-4 py-2");
            newOrderButton_->setEnabled(validTableSelection);
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

// ============================================================================
// Enhanced table status method
// ============================================================================

void OrderEntryPanel::updateTableStatus() {
    if (isDestroying_) return;
    
    std::string identifier = getSelectedTableIdentifier();
    if (identifier.empty()) {
        showOrderValidationMessage("Select a table/location to start ordering", "info");
        return;
    }
    
    if (hasCurrentOrder()) {
        auto currentOrder = posService_->getCurrentOrder();
        if (currentOrder) {
            int itemCount = currentOrder->getItems().size();
            showOrderValidationMessage(
                "Order #" + std::to_string(currentOrder->getOrderId()) + 
                " active (" + std::to_string(itemCount) + " items) - Edit Mode Available", 
                "success"
            );
        }
    } else {
        showOrderValidationMessage(
            formatTableIdentifier(identifier) + " selected - ready to create order", 
            "success"
        );
    }
}

// ============================================================================
// Additional helper for smooth integration
// ============================================================================

void OrderEntryPanel::refresh() {
    if (isDestroying_) return;
    
    // Only refresh if we're still in order entry mode
    // (i.e., no current order exists)
    if (!hasCurrentOrder()) {
        std::cout << "[OrderEntryPanel] Refreshing in entry mode" << std::endl;
        updateOrderActionButtons();
        updateTableStatus();
    } else {
        std::cout << "[OrderEntryPanel] Has current order - likely switching modes" << std::endl;
        // Just update the button to show the current state
        updateOrderActionButtons();
    }
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

// Update applyTableSelectionStyling() method since we no longer have a group box:
void OrderEntryPanel::applyTableSelectionStyling() {
    // FIXED: No longer using a group box, so apply styling to the combo box directly
    if (tableIdentifierCombo_) {
        UIStyleHelper::applyComboBoxStyle(tableIdentifierCombo_, "table-selector");
    }
    
    // No group box styling needed anymore
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
