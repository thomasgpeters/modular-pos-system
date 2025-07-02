#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WCssDecorationStyle.h>
#include <iostream>

// Complete working constructor
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
    
    try {
        // Setup meta tags
        setupMetaTags();
        
        // Initialize services
        initializeServices();
        
        // Setup theming with Bootstrap 4
        setupBootstrapTheme();
        addCustomCSS();
        
        // Setup UI
        setupMainLayout();
        setupNewOrderControls();
        setupStatusControls();
        
        // Setup events and updates
        setupEventListeners();
        setupRealTimeUpdates();
        
        // Initial UI updates
        updateCurrentOrderStatus();
        updateSystemStatus();
        
        applyComponentStyling();
        
        std::cout << "[SUCCESS] Restaurant POS Application initialized successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Application initialization failed: " << e.what() << std::endl;
        if (statusText_) {
            statusText_->setText("❌ Initialization Error: " + std::string(e.what()));
        }
    }
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

// Simplified setupMainLayout to avoid potential HTML parsing issues
void RestaurantPOSApp::setupMainLayout() {
    setTitle("Restaurant POS System");
    
    // Create simple container structure
    mainContainer_ = root()->addNew<Wt::WContainerWidget>();
    mainContainer_->addStyleClass("pos-main-container container-fluid");
    
    // Simple header without complex nesting
    auto headerContainer = mainContainer_->addNew<Wt::WContainerWidget>();
    headerContainer->addStyleClass("pos-header text-white p-3 mb-4");
    
    auto headerTitle = headerContainer->addNew<Wt::WText>("Restaurant POS System");
    headerTitle->addStyleClass("h2 mb-0");
    
    // Simple two-column layout
    auto mainRow = mainContainer_->addNew<Wt::WContainerWidget>();
    mainRow->addStyleClass("row");
    
    // Left column for order controls
    auto leftCol = mainRow->addNew<Wt::WContainerWidget>();
    leftCol->addStyleClass("col-md-6 pe-3");
    orderControlsContainer_ = leftCol;
    
    // Right column for status
    auto rightCol = mainRow->addNew<Wt::WContainerWidget>();
    rightCol->addStyleClass("col-md-6 ps-3");
    statusControlsContainer_ = rightCol;
    
    // Simple status bar
    auto statusContainer = mainContainer_->addNew<Wt::WContainerWidget>();
    statusContainer->addStyleClass("pos-status-bar p-2 mt-4");
    
    statusText_ = statusContainer->addNew<Wt::WText>("System Ready");
    statusText_->addStyleClass("mb-0");
    
    std::cout << "[DEBUG] Simplified layout created" << std::endl;
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
    std::cout << "[DEBUG] Setting up status controls with Bootstrap 4" << std::endl;
    
    // Current Order Status Section (using div instead of WGroupBox)
    auto currentOrderGroup = statusControlsContainer_->addNew<Wt::WContainerWidget>();
    currentOrderGroup->addStyleClass("pos-current-order-group");
    
    auto currentOrderTitle = currentOrderGroup->addNew<Wt::WText>("📋 Current Order");
    currentOrderTitle->addStyleClass("h4 text-info mb-3");
    
    currentOrderStatusText_ = currentOrderGroup->addNew<Wt::WText>("No active order");
    currentOrderStatusText_->addStyleClass("text-muted");
    
    // System Status Section
    auto systemStatusGroup = statusControlsContainer_->addNew<Wt::WContainerWidget>();
    systemStatusGroup->addStyleClass("pos-system-status-group");
    
    auto systemStatusTitle = systemStatusGroup->addNew<Wt::WText>("📊 System Status");
    systemStatusTitle->addStyleClass("h4 text-purple mb-3");
    
    systemStatusText_ = systemStatusGroup->addNew<Wt::WText>("🔄 Loading system status...");
    systemStatusText_->addStyleClass("small font-monospace");
    
    // Refresh Button (Bootstrap 4 classes)
    auto refreshContainer = statusControlsContainer_->addNew<Wt::WContainerWidget>();
    refreshContainer->addStyleClass("mt-3");
    
    refreshButton_ = refreshContainer->addNew<Wt::WPushButton>("🔄 Refresh Status");
    refreshButton_->addStyleClass("btn btn-outline-primary btn-block");
    
    refreshButton_->clicked().connect([this] {
        onRefreshButtonClicked();
    });
    
    std::cout << "[DEBUG] Status controls setup completed" << std::endl;
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

// Safe timer setup
void RestaurantPOSApp::setupRealTimeUpdates() {
    try {
        updateTimer_ = root()->addChild(std::make_unique<Wt::WTimer>());
        updateTimer_->setInterval(std::chrono::seconds(10)); // 10 second updates
        
        updateTimer_->timeout().connect([this] {
            try {
                onPeriodicUpdate();
            } catch (...) {
                // Silently handle timer update errors
            }
        });
        
        updateTimer_->start();
        
    } catch (...) {
        // Continue without timer if setup fails
    }
}

// Bootstrap 4 setup
void RestaurantPOSApp::setupBootstrapTheme() {
    // Bootstrap 4.6.2 - stable and Wt-compatible
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/bootstrap/4.6.2/css/bootstrap.min.css");
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css");
    require("https://cdnjs.cloudflare.com/ajax/libs/bootstrap/4.6.2/js/bootstrap.bundle.min.js");
}

// Complete CSS (includes config file CSS + Bootstrap 4 + custom)
void RestaurantPOSApp::addCustomCSS() {
    // CSS from config file
    styleSheet().addRule(".pos-application", "padding: 20px;");
    styleSheet().addRule(".pos-header", 
        "background-color: #f8f9fa; padding: 1rem; margin-bottom: 1rem; "
        "border-radius: 0.375rem; border: 1px solid #dee2e6;");
    styleSheet().addRule(".status-card", 
        "background-color: #ffffff; border: 1px solid #dee2e6; "
        "border-radius: 0.375rem; padding: 1rem; margin-bottom: 1rem;");
    styleSheet().addRule(".menu-item", 
        "padding: 0.5rem; border-bottom: 1px solid #eee;");
    styleSheet().addRule(".menu-item:last-child", "border-bottom: none;");
    
    // Bootstrap 4 compatible custom CSS
    styleSheet().addRule("body", 
        "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;");
    styleSheet().addRule(".pos-main-container", 
        "min-height: 100vh; background-color: #f8f9fa; padding: 15px;");
    styleSheet().addRule(".pos-new-order-group", 
        "border: 2px solid #28a745; background-color: #f8fff9; "
        "border-radius: 8px; padding: 15px; margin-bottom: 20px;");
    styleSheet().addRule(".pos-current-order-group", 
        "border: 2px solid #17a2b8; background-color: #f0fdff; "
        "border-radius: 8px; padding: 15px; margin-bottom: 20px;");
    styleSheet().addRule(".pos-system-status-group", 
        "border: 2px solid #6f42c1; background-color: #faf9ff; "
        "border-radius: 8px; padding: 15px; margin-bottom: 20px;");
    styleSheet().addRule(".pos-table-combo", 
        "font-size: 1.1rem; padding: 10px; width: 100%;");
    styleSheet().addRule(".pos-new-order-btn", 
        "font-size: 1.2rem; padding: 15px; width: 100%; "
        "border-radius: 8px; margin-top: 10px;");
    styleSheet().addRule(".pos-status-bar", 
        "background-color: #e9ecef; border: 1px solid #ced4da; "
        "border-radius: 6px; padding: 10px; margin-top: 20px;");
    styleSheet().addRule(".order-status-active", "color: #28a745; font-weight: bold;");
    styleSheet().addRule(".order-status-inactive", "color: #6c757d;");
}

// Meta tags setup
void RestaurantPOSApp::setupMetaTags() {
    addMetaHeader("viewport", "width=device-width, initial-scale=1.0");
    addMetaHeader("description", "Restaurant POS System - Modular Architecture");
}

void RestaurantPOSApp::applyComponentStyling() {
    // Apply dark mode support
    styleSheet().addRule("@media (prefers-color-scheme: dark)", 
        ".pos-main-container { background-color: #212529; color: #ffffff; }");
}

// Clean table combo population
void RestaurantPOSApp::populateTableIdentifierCombo() {
    tableIdentifierCombo_->clear();
    
    // Default option
    tableIdentifierCombo_->addItem("-- Select Table/Location --");
    
    // Table numbers with clean prefixes
    for (int i = 1; i <= 6; ++i) {
        std::string tableId = "table " + std::to_string(i);
        std::string displayText = "[TABLE] " + tableId;
        tableIdentifierCombo_->addItem(displayText);
    }
    
    // Special locations
    tableIdentifierCombo_->addItem("[WALKIN] walk-in");
    tableIdentifierCombo_->addItem("[DELIVERY] grubhub");
    tableIdentifierCombo_->addItem("[DELIVERY] ubereats");
    
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

// Restore the full updateSystemStatus method with safe implementation
void RestaurantPOSApp::updateSystemStatus() {
    if (!systemStatusText_ || !posService_) {
        return;
    }
    
    try {
        std::string statusText = formatSystemStatus();
        systemStatusText_->setText(statusText);
        
    } catch (const std::exception& e) {
        systemStatusText_->setText("❌ Status Error: " + std::string(e.what()));
    } catch (...) {
        systemStatusText_->setText("❌ Unknown Status Error");
    }
}

void RestaurantPOSApp::updateOrderControls() {
    // Update table combo based on current availability
    onTableIdentifierChanged();
}

// Fixed table identifier extraction
std::string RestaurantPOSApp::getCurrentTableIdentifier() const {
    if (tableIdentifierCombo_->currentIndex() <= 0) {
        return "";
    }
    
    std::string displayText = tableIdentifierCombo_->currentText().toUTF8();
    
    // Remove prefix like "[TABLE] " or "[DELIVERY] " 
    size_t bracketEnd = displayText.find("] ");
    if (bracketEnd != std::string::npos) {
        return displayText.substr(bracketEnd + 2); // Skip "] "
    }
    
    // Fallback - return as-is if no bracket format
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

void RestaurantPOSApp::updateCurrentOrderStatus() {
    std::cout << "[DEBUG] updateCurrentOrderStatus() started" << std::endl;
    
    // Check if currentOrderStatusText_ is null
    if (!currentOrderStatusText_) {
        std::cerr << "[ERROR] currentOrderStatusText_ is null!" << std::endl;
        throw std::runtime_error("currentOrderStatusText_ widget is null");
    }
    std::cout << "[DEBUG] currentOrderStatusText_ is valid" << std::endl;
    
    // Check if posService_ is null
    if (!posService_) {
        std::cerr << "[ERROR] posService_ is null!" << std::endl;
        throw std::runtime_error("posService_ is null");
    }
    std::cout << "[DEBUG] posService_ is valid" << std::endl;
    
    std::cout << "[DEBUG] About to call getCurrentOrder()" << std::endl;
    auto currentOrder = posService_->getCurrentOrder();
    std::cout << "[DEBUG] getCurrentOrder() completed" << std::endl;
    
    if (currentOrder) {
        std::cout << "[DEBUG] Current order exists, formatting status..." << std::endl;
        std::string statusText = formatOrderStatus(currentOrder);
        std::cout << "[DEBUG] Status formatted: " << statusText << std::endl;
        
        std::cout << "[DEBUG] Setting text..." << std::endl;
        currentOrderStatusText_->setText(statusText);
        std::cout << "[DEBUG] Text set successfully" << std::endl;
        
        std::cout << "[DEBUG] Updating style classes..." << std::endl;
        currentOrderStatusText_->removeStyleClass("text-muted");
        currentOrderStatusText_->addStyleClass("order-status-active");
        std::cout << "[DEBUG] Style classes updated" << std::endl;
    } else {
        std::cout << "[DEBUG] No current order, setting default text..." << std::endl;
        currentOrderStatusText_->setText("No active order");
        std::cout << "[DEBUG] Default text set" << std::endl;
        
        std::cout << "[DEBUG] Updating style classes for no order..." << std::endl;
        currentOrderStatusText_->removeStyleClass("order-status-active");
        currentOrderStatusText_->addStyleClass("text-muted");
        std::cout << "[DEBUG] Style classes updated for no order" << std::endl;
    }
    
    std::cout << "[DEBUG] updateCurrentOrderStatus() completed successfully" << std::endl;
}

// Also add debugging to formatOrderStatus to check if that's where the crash occurs
std::string RestaurantPOSApp::formatOrderStatus(std::shared_ptr<Order> order) const {
    std::cout << "[DEBUG] formatOrderStatus() started" << std::endl;
    
    if (!order) {
        std::cout << "[DEBUG] Order is null, returning default" << std::endl;
        return "No active order";
    }
    
    std::cout << "[DEBUG] Getting table identifier..." << std::endl;
    std::string tableId = order->getTableIdentifier();
    std::cout << "[DEBUG] Table identifier: " << tableId << std::endl;
    
    std::cout << "[DEBUG] Getting order type icon..." << std::endl;
    std::string icon = getOrderTypeIcon(tableId);
    std::cout << "[DEBUG] Icon: " << icon << std::endl;
    
    std::cout << "[DEBUG] Getting order type..." << std::endl;
    std::string type = order->getOrderType();
    std::cout << "[DEBUG] Order type: " << type << std::endl;
    
    std::cout << "[DEBUG] Getting order status..." << std::endl;
    std::string status = Order::statusToString(order->getStatus());
    std::cout << "[DEBUG] Status: " << status << std::endl;
    
    std::cout << "[DEBUG] Getting order ID..." << std::endl;
    int orderId = order->getOrderId();
    std::cout << "[DEBUG] Order ID: " << orderId << std::endl;
    
    std::cout << "[DEBUG] Getting items..." << std::endl;
    auto items = order->getItems();
    std::cout << "[DEBUG] Items count: " << items.size() << std::endl;
    
    std::cout << "[DEBUG] Getting total..." << std::endl;
    double total = order->getTotal();
    std::cout << "[DEBUG] Total: " << total << std::endl;
    
    std::cout << "[DEBUG] Building status string..." << std::endl;
    std::string result = icon + " Order #" + std::to_string(orderId) + 
           " (" + tableId + ") - " + status +
           " | Items: " + std::to_string(items.size()) +
           " | Total: $" + std::to_string(total);
    
    std::cout << "[DEBUG] formatOrderStatus() completed: " << result << std::endl;
    return result;
}

// Safe implementation of formatSystemStatus
std::string RestaurantPOSApp::formatSystemStatus() const {
    if (!posService_) {
        return "⚠️ POS Service not available";
    }
    
    try {
        // Get active orders safely
        auto activeOrders = posService_->getActiveOrders();
        int totalActive = static_cast<int>(activeOrders.size());
        
        // Count by type
        int dineInCount = 0, deliveryCount = 0, walkInCount = 0;
        
        for (const auto& order : activeOrders) {
            if (order) {
                try {
                    if (order->isDineIn()) dineInCount++;
                    else if (order->isDelivery()) deliveryCount++;
                    else if (order->isWalkIn()) walkInCount++;
                } catch (...) {
                    // Skip problematic orders
                }
            }
        }
        
        // Build status safely
        std::string result = "📊 Active Orders: " + std::to_string(totalActive) + "\n";
        result += "🪑 Dine-In: " + std::to_string(dineInCount) + " | ";
        result += "🚗 Delivery: " + std::to_string(deliveryCount) + " | ";
        result += "🚶 Walk-In: " + std::to_string(walkInCount) + "\n";
        
        // Add kitchen status if available
        try {
            auto kitchenTickets = posService_->getKitchenTickets();
            result += "🍳 Kitchen Queue: " + std::to_string(kitchenTickets.size()) + " | ";
            
            int waitTime = posService_->getEstimatedWaitTime();
            result += "⏱️ Est. Wait: " + std::to_string(waitTime) + " min";
        } catch (...) {
            result += "🍳 Kitchen: Available";
        }
        
        return result;
        
    } catch (const std::exception& e) {
        return "❌ Error: " + std::string(e.what());
    } catch (...) {
        return "❌ Unknown system error";
    }
}

// Clean icon implementation without complex Unicode that might cause parsing issues
std::string RestaurantPOSApp::getOrderTypeIcon(const std::string& tableIdentifier) const {
    // Use simple ASCII or well-supported Unicode
    if (tableIdentifier.find("table") == 0) {
        return "[TABLE]"; // Simple ASCII for dine-in
    } else if (tableIdentifier == "grubhub" || tableIdentifier == "ubereats") {
        return "[DELIVERY]"; // Simple ASCII for delivery
    } else if (tableIdentifier == "walk-in") {
        return "[WALKIN]"; // Simple ASCII for walk-in
    }
    return "[ORDER]"; // Default
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

