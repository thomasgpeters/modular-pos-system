#include "../../../include/ui/components/OrderEntryPanel.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WBorderLayout.h>
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
    
    // Refresh child components
    if (menuDisplay_) {
        menuDisplay_->refresh();
    }
    
    if (currentOrderDisplay_) {
        currentOrderDisplay_->refresh();
    }
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
    
    // Enable/disable menu selection
    if (menuDisplay_) {
        menuDisplay_->setSelectionEnabled(enabled);
    }
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
    // Create main layout using border layout for better organization
    auto mainLayout = std::make_unique<Wt::WBorderLayout>();
    
    // Top section: Table selection and order controls
    auto topSection = std::make_unique<Wt::WContainerWidget>();
    topSection->addStyleClass("order-entry-top-section");
    
    setupTableSelectionSection(topSection.get());
    setupOrderActionsSection(topSection.get());
    
    mainLayout->addWidget(std::move(topSection), Wt::LayoutPosition::North);
    
    // Center section: Menu and current order displays
    auto centerSection = std::make_unique<Wt::WContainerWidget>();
    centerSection->addStyleClass("order-entry-center-section");
    
    setupMenuAndOrderDisplays(centerSection.get());
    
    mainLayout->addWidget(std::move(centerSection), Wt::LayoutPosition::Center);
    
    setLayout(std::move(mainLayout));
    
    // Apply styling
    applyTableSelectionStyling();
    
    std::cout << "[OrderEntryPanel] UI initialized with complete layout" << std::endl;
}

void OrderEntryPanel::setupTableSelectionSection(Wt::WContainerWidget* parent) {
    // Create table selection group
    tableSelectionGroup_ = parent->addNew<Wt::WGroupBox>("Table Selection");
    tableSelectionGroup_->addStyleClass("table-selection-group");
    
    auto groupLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // Table identifier label and combo
    auto selectorContainer = std::make_unique<Wt::WContainerWidget>();
    selectorContainer->addStyleClass("table-selector-container");
    
    tableIdentifierLabel_ = selectorContainer->addNew<Wt::WLabel>("Select Table/Location:");
    tableIdentifierLabel_->addStyleClass("form-label");
    
    createTableIdentifierCombo(selectorContainer.get());
    
    groupLayout->addWidget(std::move(selectorContainer));
    
    // Table status text
    tableStatusText_ = tableSelectionGroup_->addNew<Wt::WText>("Select a table/location");
    tableStatusText_->addStyleClass("table-status-text text-muted small");
    
    tableSelectionGroup_->setLayout(std::move(groupLayout));
}

void OrderEntryPanel::setupOrderActionsSection(Wt::WContainerWidget* parent) {
    // Create order actions container
    auto actionsContainer = parent->addNew<Wt::WContainerWidget>();
    actionsContainer->addStyleClass("order-actions-section mt-3");
    
    auto actionsLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // New order button
    newOrderButton_ = actionsContainer->addNew<Wt::WPushButton>("🆕 Start New Order");
    newOrderButton_->addStyleClass("btn btn-success btn-lg me-2");
    newOrderButton_->setEnabled(false);
    actionsLayout->addWidget(std::unique_ptr<Wt::WPushButton>(newOrderButton_));
    
    // Send to kitchen button
    sendToKitchenButton_ = actionsContainer->addNew<Wt::WPushButton>("🍳 Send to Kitchen");
    sendToKitchenButton_->addStyleClass("btn btn-primary btn-lg me-2");
    sendToKitchenButton_->setEnabled(false);
    actionsLayout->addWidget(std::unique_ptr<Wt::WPushButton>(sendToKitchenButton_));
    
    // Process payment button
    processPaymentButton_ = actionsContainer->addNew<Wt::WPushButton>("💳 Process Payment");
    processPaymentButton_->addStyleClass("btn btn-warning btn-lg");
    processPaymentButton_->setEnabled(false);
    actionsLayout->addWidget(std::unique_ptr<Wt::WPushButton>(processPaymentButton_));
    
    actionsContainer->setLayout(std::move(actionsLayout));
}

void OrderEntryPanel::setupMenuAndOrderDisplays(Wt::WContainerWidget* parent) {
    // Create horizontal layout for menu and current order
    auto displayLayout = std::make_unique<Wt::WHBoxLayout>();
    displayLayout->setContentsMargins(0, 0, 0, 0);
    
    // Left side: Menu display
    auto menuContainer = std::make_unique<Wt::WContainerWidget>();
    menuContainer->addStyleClass("menu-display-container");
    
    // Create menu display component
    menuDisplay_ = menuContainer->addNew<MenuDisplay>(posService_, eventManager_);
    menuDisplay_->setSelectionEnabled(false); // Initially disabled until order is created
    
    displayLayout->addWidget(std::move(menuContainer), 2); // 2/3 of width
    
    // Right side: Current order display
    auto orderContainer = std::make_unique<Wt::WContainerWidget>();
    orderContainer->addStyleClass("current-order-container");
    
    // Create current order display component
    currentOrderDisplay_ = orderContainer->addNew<CurrentOrderDisplay>(posService_, eventManager_);
    currentOrderDisplay_->setEditable(true);
    
    displayLayout->addWidget(std::move(orderContainer), 1); // 1/3 of width
    
    parent->setLayout(std::move(displayLayout));
    
    std::cout << "[OrderEntryPanel] Menu and order displays setup complete" << std::endl;
}

void OrderEntryPanel::setupEventListeners() {
    if (!eventManager_) {
        std::cout << "[OrderEntryPanel] No EventManager available" << std::endl;
        return;
    }
    
    // Subscribe to order events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED,
            [this](const std::any& data) { handleOrderCreated(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_MODIFIED,
            [this](const std::any& data) { handleOrderModified(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { handleCurrentOrderChanged(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_ITEM_ADDED,
            [this](const std::any& data) { handleOrderModified(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_ITEM_REMOVED,
            [this](const std::any& data) { handleOrderModified(data); }));
    
    std::cout << "[OrderEntryPanel] Event listeners setup complete" << std::endl;
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
    
    std::cout << "[OrderEntryPanel] Event handlers setup complete" << std::endl;
}

void OrderEntryPanel::createTableIdentifierCombo(Wt::WContainerWidget* parent) {
    tableIdentifierCombo_ = parent->addNew<Wt::WComboBox>();
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

// Event handlers
void OrderEntryPanel::handleOrderCreated(const std::any& eventData) {
    std::cout << "[OrderEntryPanel] Order created event received" << std::endl;
    
    // Enable menu selection now that we have an order
    if (menuDisplay_) {
        menuDisplay_->setSelectionEnabled(true);
    }
    
    // Update button states
    updateOrderActionButtons();
    
    // Update table status
    updateTableStatus();
    
    // Show success message
    showOrderValidationMessage("✅ Order created successfully! You can now add items from the menu.", "success");
}

void OrderEntryPanel::handleOrderModified(const std::any& eventData) {
    std::cout << "[OrderEntryPanel] Order modified event received" << std::endl;
    updateOrderActionButtons();
}

void OrderEntryPanel::handleCurrentOrderChanged(const std::any& eventData) {
    std::cout << "[OrderEntryPanel] Current order changed event received" << std::endl;
    
    bool hasOrder = hasCurrentOrder();
    
    // Enable/disable menu selection based on order availability
    if (menuDisplay_) {
        menuDisplay_->setSelectionEnabled(hasOrder);
    }
    
    // Update button states
    updateOrderActionButtons();
    
    // Clear table selection if no order
    if (!hasOrder) {
        tableIdentifierCombo_->setCurrentIndex(0);
        showOrderValidationMessage("Order cleared. Select a table/location to start a new order.", "info");
    }
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
    
    auto currentOrder = posService_->getCurrentOrder();
    if (!currentOrder || currentOrder->getItems().empty()) {
        showOrderValidationError("Cannot send empty order to kitchen. Please add items first.");
        return;
    }
    
    sendCurrentOrderToKitchen();
}

void OrderEntryPanel::onProcessPaymentClicked() {
    if (!validateCurrentOrder()) {
        showOrderValidationError("No valid order to process payment for");
        return;
    }
    
    auto currentOrder = posService_->getCurrentOrder();
    if (!currentOrder || currentOrder->getItems().empty()) {
        showOrderValidationError("Cannot process payment for empty order");
        return;
    }
    
    std::cout << "[OrderEntryPanel] Process payment clicked for order: " << currentOrder->getOrderId() << std::endl;
    showOrderValidationMessage("Payment processing not yet implemented", "info");
}

void OrderEntryPanel::onTableIdentifierChanged() {
    updateTableStatus();
    updateOrderActionButtons();
}

void OrderEntryPanel::createNewOrder(const std::string& tableIdentifier) {
    try {
        // Check if table is already in use
        if (!isTableIdentifierAvailable(tableIdentifier)) {
            showOrderValidationError("Table/location is already in use");
            return;
        }
        
        auto order = posService_->createOrder(tableIdentifier);
        if (order) {
            posService_->setCurrentOrder(order);
            std::cout << "[OrderEntryPanel] Created new order #" << order->getOrderId() 
                     << " for " << tableIdentifier << std::endl;
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
        auto currentOrder = posService_->getCurrentOrder();
        if (!currentOrder) {
            showOrderValidationError("No current order to send");
            return;
        }
        
        // Validate order has items
        if (currentOrder->getItems().empty()) {
            showOrderValidationError("Cannot send empty order to kitchen");
            return;
        }
        
        bool success = posService_->sendCurrentOrderToKitchen();
        if (success) {
            showOrderValidationMessage("✅ Order #" + std::to_string(currentOrder->getOrderId()) + 
                                     " sent to kitchen successfully!", "success");
            
            // Clear current order after sending to kitchen
            posService_->setCurrentOrder(nullptr);
            
            // Disable menu selection
            if (menuDisplay_) {
                menuDisplay_->setSelectionEnabled(false);
            }
            
            // Reset table selection
            tableIdentifierCombo_->setCurrentIndex(0);
            
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
    bool hasOrderItems = hasCurrentOrder && hasOrderWithItems();
    
    // New order button - enabled when valid table selected and no current order
    if (newOrderButton_) {
        newOrderButton_->setEnabled(validTableSelection && !hasCurrentOrder);
        
        if (hasCurrentOrder) {
            newOrderButton_->setText("🆕 Order In Progress");
        } else {
            newOrderButton_->setText("🆕 Start New Order");
        }
    }
    
    // Send to kitchen button - enabled when order has items
    if (sendToKitchenButton_) {
        sendToKitchenButton_->setEnabled(hasOrderItems);
        
        if (hasOrderItems) {
            auto currentOrder = posService_->getCurrentOrder();
            int itemCount = currentOrder->getItems().size();
            sendToKitchenButton_->setText("🍳 Send to Kitchen (" + std::to_string(itemCount) + " items)");
        } else {
            sendToKitchenButton_->setText("🍳 Send to Kitchen");
        }
    }
    
    // Process payment button - enabled when order has items
    if (processPaymentButton_) {
        processPaymentButton_->setEnabled(hasOrderItems);
        
        if (hasOrderItems) {
            auto currentOrder = posService_->getCurrentOrder();
            double total = currentOrder->getTotal();
            processPaymentButton_->setText("💳 Payment ($" + std::to_string((int)total) + ")");
        } else {
            processPaymentButton_->setText("💳 Process Payment");
        }
    }
    
    std::cout << "[OrderEntryPanel] Button states updated - HasOrder: " << hasCurrentOrder 
              << ", HasItems: " << hasOrderItems << std::endl;
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

bool OrderEntryPanel::hasOrderWithItems() const {
    auto currentOrder = posService_->getCurrentOrder();
    return currentOrder && !currentOrder->getItems().empty();
}

void OrderEntryPanel::showOrderValidationError(const std::string& message) {
    std::cerr << "[OrderEntryPanel] Validation Error: " << message << std::endl;
    
    if (tableStatusText_) {
        tableStatusText_->setText("❌ " + message);
        tableStatusText_->removeStyleClass("text-success text-muted text-info");
        tableStatusText_->addStyleClass("text-danger");
    }
}

void OrderEntryPanel::showOrderValidationMessage(const std::string& message, const std::string& type) {
    std::cout << "[OrderEntryPanel] " << type << ": " << message << std::endl;
    
    if (tableStatusText_) {
        std::string icon = "ℹ️";
        std::string cssClass = "text-info";
        
        if (type == "success") {
            icon = "✅";
            cssClass = "text-success";
        } else if (type == "warning") {
            icon = "⚠️";
            cssClass = "text-warning";
        } else if (type == "error") {
            icon = "❌";
            cssClass = "text-danger";
        }
        
        tableStatusText_->setText(icon + " " + message);
        tableStatusText_->removeStyleClass("text-success text-danger text-warning text-info text-muted");
        tableStatusText_->addStyleClass(cssClass);
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
            showOrderValidationMessage("Order #" + std::to_string(currentOrder->getOrderId()) + 
                                     " active (" + std::to_string(itemCount) + " items)", "success");
        } else {
            showOrderValidationMessage(formatTableIdentifier(identifier) + " is available", "success");
        }
    } else {
        showOrderValidationMessage(formatTableIdentifier(identifier) + " is currently in use", "warning");
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
