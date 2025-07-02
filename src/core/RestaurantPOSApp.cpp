#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WCssDecorationStyle.h>
#include <iostream>

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , eventManager_(nullptr)
    , posService_(nullptr)
    , mainContainer_(nullptr)
    , orderControlsContainer_(nullptr)
    , statusText_(nullptr)
    , updateTimer_(nullptr)
    , newOrderGroup_(nullptr)
    , tableIdentifierCombo_(nullptr)
    , newOrderButton_(nullptr)
    , currentOrderStatusText_(nullptr)
    , statusControlsContainer_(nullptr)
    , refreshButton_(nullptr)
    , systemStatusText_(nullptr)
{
    logApplicationStart();
    
    // Initialize services first
    initializeServices();
    
    // Setup CSS and theming
    setupBootstrapTheme();
    addCustomCSS();
    
    // Setup the main UI layout
    setupMainLayout();
    setupNewOrderControls();
    setupStatusControls();
    
    // Setup event handling and real-time updates
    setupEventListeners();
    setupRealTimeUpdates();
    
    // Initial UI update
    updateCurrentOrderStatus();
    updateSystemStatus();
    
    // Apply final styling
    applyComponentStyling();
}

void RestaurantPOSApp::initializeServices() {
    // Create event manager
    eventManager_ = std::make_shared<EventManager>();
    
    // Create POS service with event manager
    posService_ = std::make_shared<POSService>(eventManager_);
    
    // Initialize the menu
    posService_->initializeMenu();
    
    std::cout << "[RestaurantPOSApp] Services initialized successfully" << std::endl;
}

void RestaurantPOSApp::setupMainLayout() {
    setTitle("Restaurant POS System - Enhanced Order Management");
    
    // Create main container
    mainContainer_ = root()->addNew<Wt::WContainerWidget>();
    mainContainer_->addStyleClass("container-fluid pos-main-container");
    
    // Create main layout
    auto mainLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // Header section
    auto headerContainer = std::make_unique<Wt::WContainerWidget>();
    headerContainer->addStyleClass("pos-header bg-primary text-white p-3 mb-4");
    
    auto headerTitle = headerContainer->addNew<Wt::WText>("Restaurant POS System");
    headerTitle->addStyleClass("h2 mb-0");
    
    mainLayout->addWidget(std::move(headerContainer));
    
    // Content area container
    auto contentContainer = std::make_unique<Wt::WContainerWidget>();
    contentContainer->addStyleClass("row");
    
    // Order controls column (left side)
    orderControlsContainer_ = contentContainer->addNew<Wt::WContainerWidget>();
    orderControlsContainer_->addStyleClass("col-md-6 pos-order-controls");
    
    // Status column (right side)
    statusControlsContainer_ = contentContainer->addNew<Wt::WContainerWidget>();
    statusControlsContainer_->addStyleClass("col-md-6 pos-status-section");
    
    mainLayout->addWidget(std::move(contentContainer));
    
    // Status bar at bottom
    statusText_ = mainContainer_->addNew<Wt::WText>("System Ready");
    statusText_->addStyleClass("pos-status-bar bg-light p-2 mt-4 border-top");
    
    mainContainer_->setLayout(std::move(mainLayout));
}

void RestaurantPOSApp::setupNewOrderControls() {
    // Create new order group box
    newOrderGroup_ = orderControlsContainer_->addNew<Wt::WGroupBox>();
    newOrderGroup_->setTitle("Create New Order");
    newOrderGroup_->addStyleClass("pos-new-order-group mb-4");
    
    auto groupLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // Table identifier selection
    auto tableSelectionContainer = std::make_unique<Wt::WContainerWidget>();
    tableSelectionContainer->addStyleClass("mb-3");
    
    auto tableLabel = tableSelectionContainer->addNew<Wt::WLabel>("Select Table/Location:");
    tableLabel->addStyleClass("form-label");
    
    tableIdentifierCombo_ = tableSelectionContainer->addNew<Wt::WComboBox>();
    tableIdentifierCombo_->addStyleClass("form-select pos-table-combo");
    populateTableIdentifierCombo();
    
    // Connect table identifier change event
    tableIdentifierCombo_->changed().connect([this] {
        onTableIdentifierChanged();
    });
    
    groupLayout->addWidget(std::move(tableSelectionContainer));
    
    // New order button
    auto buttonContainer = std::make_unique<Wt::WContainerWidget>();
    buttonContainer->addStyleClass("d-grid mb-3");
    
    newOrderButton_ = buttonContainer->addNew<Wt::WPushButton>("Start New Order");
    newOrderButton_->addStyleClass("btn btn-success btn-lg pos-new-order-btn");
    newOrderButton_->setEnabled(false); // Disabled until valid selection
    
    // Connect new order button click
    newOrderButton_->clicked().connect([this] {
        onNewOrderButtonClicked();
    });
    
    groupLayout->addWidget(std::move(buttonContainer));
    
    // Current order status
    currentOrderStatusText_ = newOrderGroup_->addNew<Wt::WText>("No active order");
    currentOrderStatusText_->addStyleClass("pos-current-order-status text-muted");
    
    newOrderGroup_->setLayout(std::move(groupLayout));
}

void RestaurantPOSApp::setupStatusControls() {
    // System status section
    auto statusGroup = statusControlsContainer_->addNew<Wt::WGroupBox>();
    statusGroup->setTitle("System Status");
    statusGroup->addStyleClass("pos-status-group mb-4");
    
    auto statusLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // System status text
    systemStatusText_ = statusGroup->addNew<Wt::WText>();
    systemStatusText_->addStyleClass("pos-system-status mb-3");
    
    // Refresh button
    auto refreshContainer = std::make_unique<Wt::WContainerWidget>();
    refreshContainer->addStyleClass("d-grid");
    
    refreshButton_ = refreshContainer->addNew<Wt::WPushButton>("Refresh Status");
    refreshButton_->addStyleClass("btn btn-outline-primary pos-refresh-btn");
    
    refreshButton_->clicked().connect([this] {
        onRefreshButtonClicked();
    });
    
    statusLayout->addWidget(std::move(refreshContainer));
    statusGroup->setLayout(std::move(statusLayout));
}

void RestaurantPOSApp::setupEventListeners() {
    // Register for order creation events
    posService_->onOrderCreated([this](std::shared_ptr<Order> order) {
        onOrderCreated(order);
    });
    
    // Register for order modification events
    posService_->onOrderModified([this](std::shared_ptr<Order> order) {
        onOrderModified(order);
    });
}

void RestaurantPOSApp::setupRealTimeUpdates() {
    // Create and start update timer
    updateTimer_ = std::make_unique<Wt::WTimer>();
    updateTimer_->setInterval(std::chrono::seconds(5)); // Update every 5 seconds
    updateTimer_->timeout().connect([this] {
        onPeriodicUpdate();
    });
    updateTimer_->start();
}

void RestaurantPOSApp::setupBootstrapTheme() {
    // Set Bootstrap 5 theme
    setTheme(std::make_shared<Wt::WBootstrap5Theme>());
    
    // Use external Bootstrap CSS
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/bootstrap/5.3.0/css/bootstrap.min.css");
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/bootstrap-icons/1.10.0/font/bootstrap-icons.min.css");
}

void RestaurantPOSApp::addCustomCSS() {
    styleSheet().addRule(".pos-main-container", "min-height: 100vh; background-color: #f8f9fa;");
    styleSheet().addRule(".pos-header", "box-shadow: 0 2px 4px rgba(0,0,0,0.1);");
    styleSheet().addRule(".pos-new-order-group", "border: 2px solid #198754; box-shadow: 0 4px 6px rgba(0,0,0,0.1);");
    styleSheet().addRule(".pos-table-combo", "font-size: 1.1rem; padding: 0.75rem;");
    styleSheet().addRule(".pos-new-order-btn", "font-size: 1.2rem; padding: 1rem; box-shadow: 0 3px 6px rgba(25,135,84,0.3);");
    styleSheet().addRule(".pos-new-order-btn:hover", "transform: translateY(-1px); box-shadow: 0 4px 8px rgba(25,135,84,0.4);");
    styleSheet().addRule(".pos-current-order-status", "font-style: italic; border-left: 3px solid #6c757d; padding-left: 0.75rem;");
    styleSheet().addRule(".pos-status-group", "border: 1px solid #dee2e6;");
    styleSheet().addRule(".pos-system-status", "font-family: 'Courier New', monospace; background-color: #f8f9fa; padding: 1rem; border-radius: 0.375rem;");
    styleSheet().addRule(".pos-status-bar", "font-size: 0.9rem; color: #6c757d;");
    
    // Table identifier specific styles
    styleSheet().addRule(".table-identifier-dine-in", "color: #0d6efd; font-weight: 500;");
    styleSheet().addRule(".table-identifier-delivery", "color: #fd7e14; font-weight: 500;");
    styleSheet().addRule(".table-identifier-walk-in", "color: #20c997; font-weight: 500;");
    
    // Order status styles
    styleSheet().addRule(".order-status-active", "color: #198754; font-weight: bold;");
    styleSheet().addRule(".order-status-inactive", "color: #6c757d;");
}

void RestaurantPOSApp::applyComponentStyling() {
    // Apply dark mode support
    styleSheet().addRule("@media (prefers-color-scheme: dark)", 
        ".pos-main-container { background-color: #212529; color: #ffffff; }");
}

void RestaurantPOSApp::populateTableIdentifierCombo() {
    tableIdentifierCombo_->clear();
    
    // Add default option
    tableIdentifierCombo_->addItem("-- Select Table/Location --");
    
    // Add table numbers (1-20)
    for (int i = 1; i <= 20; ++i) {
        std::string tableId = "table " + std::to_string(i);
        tableIdentifierCombo_->addItem(getOrderTypeIcon(tableId) + " " + tableId);
    }
    
    // Add special locations
    tableIdentifierCombo_->addItem(getOrderTypeIcon("walk-in") + " walk-in");
    tableIdentifierCombo_->addItem(getOrderTypeIcon("grubhub") + " grubhub");
    tableIdentifierCombo_->addItem(getOrderTypeIcon("ubereats") + " ubereats");
    
    // Set default selection
    tableIdentifierCombo_->setCurrentIndex(0);
}

void RestaurantPOSApp::onNewOrderButtonClicked() {
    if (!validateNewOrderInput()) {
        showValidationError("Please select a valid table/location");
        return;
    }
    
    std::string tableIdentifier = getCurrentTableIdentifier();
    
    try {
        // Create new order
        auto newOrder = posService_->createOrder(tableIdentifier);
        
        if (newOrder) {
            // Set as current order
            posService_->setCurrentOrder(newOrder);
            
            // Update UI
            showOrderCreationStatus(true, tableIdentifier);
            updateCurrentOrderStatus();
            updateOrderControls();
            
            updateStatus("New order created for " + tableIdentifier + " (Order #" + std::to_string(newOrder->getOrderId()) + ")");
        } else {
            showOrderCreationStatus(false, tableIdentifier);
            updateStatus("Failed to create order for " + tableIdentifier);
        }
    } catch (const std::exception& e) {
        showOrderCreationStatus(false, tableIdentifier);
        updateStatus("Error creating order: " + std::string(e.what()));
    }
}

void RestaurantPOSApp::onTableIdentifierChanged() {
    bool isValidSelection = isValidTableSelection();
    newOrderButton_->setEnabled(isValidSelection);
    
    if (isValidSelection) {
        std::string identifier = getCurrentTableIdentifier();
        bool isAvailable = isTableIdentifierAvailable(identifier);
        
        if (!isAvailable) {
            newOrderButton_->setText("Table/Location In Use");
            newOrderButton_->setEnabled(false);
            newOrderButton_->addStyleClass("btn-warning");
            newOrderButton_->removeStyleClass("btn-success");
        } else {
            newOrderButton_->setText("Start New Order");
            newOrderButton_->setEnabled(true);
            newOrderButton_->addStyleClass("btn-success");
            newOrderButton_->removeStyleClass("btn-warning");
        }
    }
}

void RestaurantPOSApp::onRefreshButtonClicked() {
    updateCurrentOrderStatus();
    updateSystemStatus();
    populateTableIdentifierCombo(); // Refresh available tables
    updateStatus("Status refreshed");
}

void RestaurantPOSApp::onPeriodicUpdate() {
    updateCurrentOrderStatus();
    updateSystemStatus();
}

void RestaurantPOSApp::onOrderCreated(std::shared_ptr<Order> order) {
    updateCurrentOrderStatus();
    updateSystemStatus();
    // Reset order controls after successful creation
    resetOrderControls();
}

void RestaurantPOSApp::onOrderModified(std::shared_ptr<Order> order) {
    updateCurrentOrderStatus();
}

void RestaurantPOSApp::onCurrentOrderChanged() {
    updateCurrentOrderStatus();
    updateOrderControls();
}

void RestaurantPOSApp::updateCurrentOrderStatus() {
    auto currentOrder = posService_->getCurrentOrder();
    
    if (currentOrder) {
        std::string statusText = formatOrderStatus(currentOrder);
        currentOrderStatusText_->setText(statusText);
        currentOrderStatusText_->removeStyleClass("text-muted");
        currentOrderStatusText_->addStyleClass("order-status-active");
    } else {
        currentOrderStatusText_->setText("No active order");
        currentOrderStatusText_->removeStyleClass("order-status-active");
        currentOrderStatusText_->addStyleClass("text-muted");
    }
}

void RestaurantPOSApp::updateSystemStatus() {
    std::string statusText = formatSystemStatus();
    systemStatusText_->setText(statusText);
}

void RestaurantPOSApp::updateOrderControls() {
    // Update table combo based on current availability
    onTableIdentifierChanged();
}

std::string RestaurantPOSApp::getCurrentTableIdentifier() const {
    if (tableIdentifierCombo_->currentIndex() <= 0) {
        return "";
    }
    
    std::string displayText = tableIdentifierCombo_->currentText().toUTF8();
    
    // Remove icon prefix if present
    size_t spacePos = displayText.find(' ');
    if (spacePos != std::string::npos && spacePos < 3) {
        return displayText.substr(spacePos + 1);
    }
    
    return displayText;
}

bool RestaurantPOSApp::isValidTableSelection() const {
    return tableIdentifierCombo_->currentIndex() > 0;
}

bool RestaurantPOSApp::validateNewOrderInput() const {
    return isValidTableSelection() && !getCurrentTableIdentifier().empty();
}

void RestaurantPOSApp::showValidationError(const std::string& message) {
    updateStatus("Validation Error: " + message);
}

void RestaurantPOSApp::showOrderCreationStatus(bool success, const std::string& tableIdentifier) {
    if (success) {
        updateStatus("✓ Order created successfully for " + tableIdentifier);
    } else {
        updateStatus("✗ Failed to create order for " + tableIdentifier);
    }
}

void RestaurantPOSApp::resetOrderControls() {
    tableIdentifierCombo_->setCurrentIndex(0);
    newOrderButton_->setEnabled(false);
    newOrderButton_->setText("Start New Order");
    newOrderButton_->addStyleClass("btn-success");
    newOrderButton_->removeStyleClass("btn-warning");
}

std::string RestaurantPOSApp::formatOrderStatus(std::shared_ptr<Order> order) const {
    if (!order) return "No active order";
    
    std::string icon = getOrderTypeIcon(order->getTableIdentifier());
    std::string type = order->getOrderType();
    std::string status = Order::statusToString(order->getStatus());
    
    return icon + " Order #" + std::to_string(order->getOrderId()) + 
           " (" + order->getTableIdentifier() + ") - " + status +
           " | Items: " + std::to_string(order->getItems().size()) +
           " | Total: $" + std::to_string(order->getTotal());
}

std::string RestaurantPOSApp::formatSystemStatus() const {
    auto activeOrders = posService_->getActiveOrders();
    int totalActive = activeOrders.size();
    
    int dineInCount = 0, deliveryCount = 0, walkInCount = 0;
    
    for (const auto& order : activeOrders) {
        if (order->isDineIn()) dineInCount++;
        else if (order->isDelivery()) deliveryCount++;
        else if (order->isWalkIn()) walkInCount++;
    }
    
    return "Active Orders: " + std::to_string(totalActive) +
           " | Dine-In: " + std::to_string(dineInCount) +
           " | Delivery: " + std::to_string(deliveryCount) +
           " | Walk-In: " + std::to_string(walkInCount) +
           "\nKitchen Queue: " + std::to_string(posService_->getKitchenTickets().size()) +
           " | Est. Wait: " + std::to_string(posService_->getEstimatedWaitTime()) + " min";
}

std::string RestaurantPOSApp::getOrderTypeIcon(const std::string& tableIdentifier) const {
    if (tableIdentifier.find("table") == 0) {
        return "🪑"; // Chair icon for dine-in
    } else if (tableIdentifier == "grubhub" || tableIdentifier == "ubereats") {
        return "🚗"; // Car icon for delivery
    } else if (tableIdentifier == "walk-in") {
        return "🚶"; // Walking icon for walk-in
    }
    return "📋"; // Default clipboard icon
}

bool RestaurantPOSApp::isTableIdentifierAvailable(const std::string& identifier) const {
    return !posService_->isTableIdentifierInUse(identifier);
}

void RestaurantPOSApp::logApplicationStart() {
    std::cout << "=== Restaurant POS Application Starting ===" << std::endl;
    std::cout << "Version: 2.1.0 - Enhanced Order Management" << std::endl;
    std::cout << "Features: String-based table identifiers, delivery support" << std::endl;
}

void RestaurantPOSApp::updateStatus(const std::string& message) {
    statusText_->setText(message);
    std::cout << "[POS] " << message << std::endl;
}

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
