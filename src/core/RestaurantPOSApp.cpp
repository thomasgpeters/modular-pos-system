#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WCssDecorationStyle.h>
#include <iostream>

// Enhanced constructor with ThemeService integration
RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , eventManager_(nullptr)
    , posService_(nullptr)
    , themeService_(nullptr)
    , mainContainer_(nullptr)
    , orderControlsContainer_(nullptr)
    , statusText_(nullptr)
    , updateTimer_(nullptr)
    , headerContainer_(nullptr)
    , themeControlsContainer_(nullptr)
    , themeSelector_(nullptr)
    , themeToggleButton_(nullptr)
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
        
        // Initialize services (including theme service)
        initializeServices();
        
        // Setup theming with Bootstrap 4 and theme system
        setupBootstrapTheme();
        addCustomCSS();
        addThemeSpecificCSS();
        
        // Setup UI with enhanced header
        setupMainLayout();
        setupHeaderWithThemeControls();
        setupNewOrderControls();
        setupStatusControls();
        
        // Setup events and updates
        setupEventListeners();
        setupThemeEventHandlers();
        setupRealTimeUpdates();
        
        // Initial UI updates
        updateCurrentOrderStatus();
        updateSystemStatus();
        updateThemeControls();
        
        applyComponentStyling();
        applyThemeToComponents();
        
        std::cout << "[SUCCESS] Restaurant POS Application with Theme System initialized successfully!" << std::endl;
        
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
    
    // Initialize theme service
    initializeThemeService();
    
    std::cout << "[RestaurantPOSApp] All services initialized successfully" << std::endl;
}

void RestaurantPOSApp::initializeThemeService() {
    try {
        // Create theme service with this application instance
        themeService_ = std::make_shared<ThemeService>(this);
        
        // Load user's preferred theme
        themeService_->loadThemePreference();
        
        std::cout << "[ThemeService] Initialized with theme: " 
                  << themeService_->getThemeName(themeService_->getCurrentTheme()) << std::endl;
                  
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] ThemeService initialization failed: " << e.what() << std::endl;
        // Continue without theme service if it fails
        themeService_ = nullptr;
    }
}

void RestaurantPOSApp::setupMainLayout() {
    setTitle("Restaurant POS System");
    
    // Create main container with theme support
    mainContainer_ = root()->addNew<Wt::WContainerWidget>();
    mainContainer_->addStyleClass("pos-main-container container-fluid");
    
    // Apply initial theme to main container
    if (themeService_) {
        themeService_->applyThemeToContainer(mainContainer_);
    }
    
    std::cout << "[DEBUG] Main layout created with theme support" << std::endl;
}

void RestaurantPOSApp::setupHeaderWithThemeControls() {
    // Create enhanced header container
    headerContainer_ = mainContainer_->addNew<Wt::WContainerWidget>();
    headerContainer_->addStyleClass("pos-header row align-items-center p-3 mb-4");
    
    // Left side - Application title
    auto titleContainer = headerContainer_->addNew<Wt::WContainerWidget>();
    titleContainer->addStyleClass("col-md-8");
    
    auto headerTitle = titleContainer->addNew<Wt::WText>("🍽️ Restaurant POS System");
    headerTitle->addStyleClass("h2 mb-0 pos-app-title");
    
    // Right side - Theme controls
    themeControlsContainer_ = headerContainer_->addNew<Wt::WContainerWidget>();
    themeControlsContainer_->addStyleClass("col-md-4 text-right");
    
    setupThemeControls();
    
    // Two-column layout for main content
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
    
    // Status bar
    auto statusContainer = mainContainer_->addNew<Wt::WContainerWidget>();
    statusContainer->addStyleClass("pos-status-bar p-2 mt-4");
    
    statusText_ = statusContainer->addNew<Wt::WText>("System Ready");
    statusText_->addStyleClass("mb-0");
    
    std::cout << "[DEBUG] Enhanced header with theme controls created" << std::endl;
}

void RestaurantPOSApp::setupThemeControls() {
    if (!themeService_) {
        std::cout << "[WARNING] ThemeService not available, skipping theme controls" << std::endl;
        return;
    }
    
    // Create theme controls container
    auto controlsGroup = themeControlsContainer_->addNew<Wt::WContainerWidget>();
    controlsGroup->addStyleClass("btn-group");
    
    // Theme selector dropdown
    createThemeSelector();
    
    // Theme toggle button
    createThemeToggleButton();
    
    std::cout << "[DEBUG] Theme controls created" << std::endl;
}

void RestaurantPOSApp::createThemeSelector() {
    if (!themeService_) return;
    
    themeSelector_ = themeControlsContainer_->addNew<Wt::WComboBox>();
    themeSelector_->addStyleClass("form-select pos-theme-selector");
    
    // Populate theme options
    auto themes = themeService_->getAvailableThemes();
    for (const auto& theme : themes) {
        std::string text = themeService_->getThemeIcon(theme) + " " + 
                          themeService_->getThemeName(theme);
        themeSelector_->addItem(text);
    }
    
    // Set current selection
    auto currentTheme = themeService_->getCurrentTheme();
    auto it = std::find(themes.begin(), themes.end(), currentTheme);
    if (it != themes.end()) {
        themeSelector_->setCurrentIndex(std::distance(themes.begin(), it));
    }
}

void RestaurantPOSApp::createThemeToggleButton() {
    if (!themeService_) return;
    
    themeToggleButton_ = themeControlsContainer_->addNew<Wt::WPushButton>();
    themeToggleButton_->addStyleClass("btn btn-outline-secondary ms-2 pos-theme-toggle");
    
    // Update button text
    updateThemeControls();
}

void RestaurantPOSApp::setupThemeEventHandlers() {
    if (!themeService_) return;
    
    // Register for theme change notifications
    themeService_->onThemeChanged([this](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
        onThemeChanged(oldTheme, newTheme);
    });
    
    // Connect theme selector change event
    if (themeSelector_) {
        themeSelector_->changed().connect([this] {
            onThemeSelectorChanged();
        });
    }
    
    // Connect theme toggle button
    if (themeToggleButton_) {
        themeToggleButton_->clicked().connect([this] {
            onThemeToggleClicked();
        });
    }
    
    std::cout << "[DEBUG] Theme event handlers setup completed" << std::endl;
}

void RestaurantPOSApp::onThemeChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
    std::cout << "[THEME] Changed from " << themeService_->getThemeName(oldTheme) 
              << " to " << themeService_->getThemeName(newTheme) << std::endl;
    
    // Apply theme transition effect
    applyThemeTransition();
    
    // Update theme controls
    updateThemeControls();
    
    // Apply theme to all components
    applyThemeToComponents();
    
    // Update status
    updateStatus("Theme changed to " + themeService_->getThemeName(newTheme));
}

void RestaurantPOSApp::onThemeToggleClicked() {
    if (themeService_) {
        themeService_->toggleTheme();
    }
}

void RestaurantPOSApp::onThemeSelectorChanged() {
    if (!themeService_ || !themeSelector_) return;
    
    int index = themeSelector_->currentIndex();
    auto themes = themeService_->getAvailableThemes();
    if (index >= 0 && index < static_cast<int>(themes.size())) {
        themeService_->setTheme(themes[index]);
    }
}

void RestaurantPOSApp::updateThemeControls() {
    if (!themeService_) return;
    
    auto currentTheme = themeService_->getCurrentTheme();
    
    // Update toggle button
    if (themeToggleButton_) {
        std::string buttonText = themeService_->getThemeIcon(currentTheme) + " " + 
                               themeService_->getThemeName(currentTheme);
        themeToggleButton_->setText(buttonText);
    }
    
    // Update selector
    if (themeSelector_) {
        auto themes = themeService_->getAvailableThemes();
        auto it = std::find(themes.begin(), themes.end(), currentTheme);
        if (it != themes.end()) {
            themeSelector_->setCurrentIndex(std::distance(themes.begin(), it));
        }
    }
}

void RestaurantPOSApp::applyThemeToComponents() {
    if (!themeService_) return;
    
    // Apply theme to main container
    themeService_->applyThemeToContainer(mainContainer_);
    
    // Apply theme to header
    if (headerContainer_) {
        themeService_->applyThemeToContainer(headerContainer_);
    }
    
    // Apply theme to order controls container
    if (orderControlsContainer_) {
        themeService_->applyThemeToContainer(orderControlsContainer_);
    }
    
    // Apply theme to status controls container
    if (statusControlsContainer_) {
        themeService_->applyThemeToContainer(statusControlsContainer_);
    }
}

void RestaurantPOSApp::applyThemeTransition() {
    if (!themeService_) return;
    
    // Apply smooth transition effect
    ThemeUtils::applyThemeTransition(this, 300);
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
    std::cout << "[DEBUG] Setting up status controls with Bootstrap 4 and theme support" << std::endl;
    
    // Current Order Status Section
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
    
    // Refresh Button
    auto refreshContainer = statusControlsContainer_->addNew<Wt::WContainerWidget>();
    refreshContainer->addStyleClass("mt-3");
    
    refreshButton_ = refreshContainer->addNew<Wt::WPushButton>("🔄 Refresh Status");
    refreshButton_->addStyleClass("btn btn-outline-primary btn-block");
    
    refreshButton_->clicked().connect([this] {
        onRefreshButtonClicked();
    });
    
    std::cout << "[DEBUG] Status controls setup completed with theme support" << std::endl;
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
    try {
        updateTimer_ = root()->addChild(std::make_unique<Wt::WTimer>());
        updateTimer_->setInterval(std::chrono::seconds(10));
        
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

void RestaurantPOSApp::setupBootstrapTheme() {
    // Bootstrap 4.6.2 - stable and Wt-compatible
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/bootstrap/4.6.2/css/bootstrap.min.css");
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css");
    require("https://cdnjs.cloudflare.com/ajax/libs/bootstrap/4.6.2/js/bootstrap.bundle.min.js");
}

void RestaurantPOSApp::addCustomCSS() {
    // Base CSS rules
    styleSheet().addRule(".pos-application", "padding: 20px;");
    styleSheet().addRule(".pos-header", 
        "background-color: var(--pos-header-bg, #f8f9fa); "
        "color: var(--pos-header-text, #212529); "
        "padding: 1rem; margin-bottom: 1rem; "
        "border-radius: 0.375rem; border: 1px solid var(--pos-border-color, #dee2e6);");
    
    styleSheet().addRule(".status-card", 
        "background-color: var(--pos-card-bg, #ffffff); "
        "color: var(--pos-card-text, #212529); "
        "border: 1px solid var(--pos-border-color, #dee2e6); "
        "border-radius: 0.375rem; padding: 1rem; margin-bottom: 1rem;");
    
    styleSheet().addRule(".menu-item", 
        "padding: 0.5rem; border-bottom: 1px solid var(--pos-border-light, #eee);");
    styleSheet().addRule(".menu-item:last-child", "border-bottom: none;");
    
    // Bootstrap 4 compatible custom CSS with theme variables
    styleSheet().addRule("body", 
        "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; "
        "background-color: var(--pos-body-bg, #ffffff); "
        "color: var(--pos-body-text, #212529);");
    
    styleSheet().addRule(".pos-main-container", 
        "min-height: 100vh; "
        "background-color: var(--pos-main-bg, #f8f9fa); "
        "color: var(--pos-main-text, #212529); "
        "padding: 15px;");
    
    // Theme selector specific styles
    styleSheet().addRule(".pos-theme-selector", 
        "min-width: 150px; margin-right: 10px;");
    styleSheet().addRule(".pos-theme-toggle", 
        "min-width: 120px;");
    styleSheet().addRule(".pos-app-title", 
        "color: var(--pos-title-color, #495057);");
}

void RestaurantPOSApp::addThemeSpecificCSS() {
    // Light theme CSS variables
    styleSheet().addRule(".theme-light", 
        "--pos-main-bg: #f8f9fa; "
        "--pos-main-text: #212529; "
        "--pos-header-bg: #ffffff; "
        "--pos-header-text: #495057; "
        "--pos-card-bg: #ffffff; "
        "--pos-card-text: #212529; "
        "--pos-border-color: #dee2e6; "
        "--pos-border-light: #eee; "
        "--pos-title-color: #495057; "
        "--pos-body-bg: #ffffff; "
        "--pos-body-text: #212529;");
    
    // Dark theme CSS variables
    styleSheet().addRule(".theme-dark", 
        "--pos-main-bg: #212529; "
        "--pos-main-text: #f8f9fa; "
        "--pos-header-bg: #343a40; "
        "--pos-header-text: #f8f9fa; "
        "--pos-card-bg: #495057; "
        "--pos-card-text: #f8f9fa; "
        "--pos-border-color: #6c757d; "
        "--pos-border-light: #6c757d; "
        "--pos-title-color: #f8f9fa; "
        "--pos-body-bg: #212529; "
        "--pos-body-text: #f8f9fa;");
    
    // Colorful theme CSS variables
    styleSheet().addRule(".theme-colorful", 
        "--pos-main-bg: linear-gradient(135deg, #667eea 0%, #764ba2 100%); "
        "--pos-main-text: #ffffff; "
        "--pos-header-bg: rgba(255, 255, 255, 0.9); "
        "--pos-header-text: #495057; "
        "--pos-card-bg: rgba(255, 255, 255, 0.95); "
        "--pos-card-text: #495057; "
        "--pos-border-color: rgba(255, 255, 255, 0.3); "
        "--pos-border-light: rgba(255, 255, 255, 0.2); "
        "--pos-title-color: #6f42c1; "
        "--pos-body-bg: #667eea; "
        "--pos-body-text: #ffffff;");
    
    // Base theme CSS variables
    styleSheet().addRule(".theme-base", 
        "--pos-main-bg: #ffffff; "
        "--pos-main-text: #495057; "
        "--pos-header-bg: #f8f9fa; "
        "--pos-header-text: #495057; "
        "--pos-card-bg: #ffffff; "
        "--pos-card-text: #495057; "
        "--pos-border-color: #ced4da; "
        "--pos-border-light: #dee2e6; "
        "--pos-title-color: #495057; "
        "--pos-body-bg: #ffffff; "
        "--pos-body-text: #495057;");
    
    // Enhanced component styles with theme support
    styleSheet().addRule(".pos-new-order-group", 
        "border: 2px solid #28a745; "
        "background-color: var(--pos-card-bg, #f8fff9); "
        "color: var(--pos-card-text, #212529); "
        "border-radius: 8px; padding: 15px; margin-bottom: 20px;");
    
    styleSheet().addRule(".pos-current-order-group", 
        "border: 2px solid #17a2b8; "
        "background-color: var(--pos-card-bg, #f0fdff); "
        "color: var(--pos-card-text, #212529); "
        "border-radius: 8px; padding: 15px; margin-bottom: 20px;");
    
    styleSheet().addRule(".pos-system-status-group", 
        "border: 2px solid #6f42c1; "
        "background-color: var(--pos-card-bg, #faf9ff); "
        "color: var(--pos-card-text, #212529); "
        "border-radius: 8px; padding: 15px; margin-bottom: 20px;");
}

void RestaurantPOSApp::setupMetaTags() {
    addMetaHeader("viewport", "width=device-width, initial-scale=1.0");
    addMetaHeader("description", "Restaurant POS System - Modular Architecture with Theme Support");
}

void RestaurantPOSApp::applyComponentStyling() {
    // Apply enhanced styling with theme support
    styleSheet().addRule(".pos-table-combo", 
        "font-size: 1.1rem; padding: 10px; width: 100%; "
        "background-color: var(--pos-card-bg, #ffffff); "
        "color: var(--pos-card-text, #212529); "
        "border-color: var(--pos-border-color, #ced4da);");
    
    styleSheet().addRule(".pos-new-order-btn", 
        "font-size: 1.2rem; padding: 15px; width: 100%; "
        "border-radius: 8px; margin-top: 10px;");
    
    styleSheet().addRule(".pos-status-bar", 
        "background-color: var(--pos-card-bg, #e9ecef); "
        "color: var(--pos-card-text, #495057); "
        "border: 1px solid var(--pos-border-color, #ced4da); "
        "border-radius: 6px; padding: 10px; margin-top: 20px;");
    
    styleSheet().addRule(".order-status-active", "color: #28a745; font-weight: bold;");
    styleSheet().addRule(".order-status-inactive", "color: var(--pos-card-text, #6c757d);");
    
    // Theme transition support
    styleSheet().addRule(".pos-theme-transition", 
        "transition: background-color 0.3s ease, color 0.3s ease, "
        "border-color 0.3s ease, box-shadow 0.3s ease !important;");
}

// [Rest of the methods remain the same as in the original file]
// Including all the event handlers, status methods, etc.
// I'll include the key ones that might need theme awareness:

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
        auto newOrder = posService_->createOrder(tableIdentifier);
        
        if (newOrder) {
            posService_->setCurrentOrder(newOrder);
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
    populateTableIdentifierCombo();
    updateStatus("Status refreshed");
}

void RestaurantPOSApp::onPeriodicUpdate() {
    updateCurrentOrderStatus();
    updateSystemStatus();
}

void RestaurantPOSApp::onOrderCreated(std::shared_ptr<Order> order) {
    updateCurrentOrderStatus();
    updateSystemStatus();
    resetOrderControls();
}

void RestaurantPOSApp::onOrderModified(std::shared_ptr<Order> order) {
    updateCurrentOrderStatus();
}

void RestaurantPOSApp::onCurrentOrderChanged() {
    updateCurrentOrderStatus();
    updateOrderControls();
}

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
    onTableIdentifierChanged();
}

std::string RestaurantPOSApp::getCurrentTableIdentifier() const {
    if (tableIdentifierCombo_->currentIndex() <= 0) {
        return "";
    }
    
    std::string displayText = tableIdentifierCombo_->currentText().toUTF8();
    
    size_t bracketEnd = displayText.find("] ");
    if (bracketEnd != std::string::npos) {
        return displayText.substr(bracketEnd + 2);
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

void RestaurantPOSApp::updateCurrentOrderStatus() {
    std::cout << "[DEBUG] updateCurrentOrderStatus() started" << std::endl;
    
    if (!currentOrderStatusText_) {
        std::cerr << "[ERROR] currentOrderStatusText_ is null!" << std::endl;
        throw std::runtime_error("currentOrderStatusText_ widget is null");
    }
    
    if (!posService_) {
        std::cerr << "[ERROR] posService_ is null!" << std::endl;
        throw std::runtime_error("posService_ is null");
    }
    
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
    
    std::cout << "[DEBUG] updateCurrentOrderStatus() completed successfully" << std::endl;
}

std::string RestaurantPOSApp::formatOrderStatus(std::shared_ptr<Order> order) const {
    if (!order) {
        return "No active order";
    }
    
    std::string tableId = order->getTableIdentifier();
    std::string icon = getOrderTypeIcon(tableId);
    std::string status = Order::statusToString(order->getStatus());
    int orderId = order->getOrderId();
    auto items = order->getItems();
    double total = order->getTotal();
    
    return icon + " Order #" + std::to_string(orderId) + 
           " (" + tableId + ") - " + status +
           " | Items: " + std::to_string(items.size()) +
           " | Total: $" + std::to_string(total);
}

std::string RestaurantPOSApp::formatSystemStatus() const {
    if (!posService_) {
        return "⚠️ POS Service not available";
    }
    
    try {
        auto activeOrders = posService_->getActiveOrders();
        int totalActive = static_cast<int>(activeOrders.size());
        
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
        
        std::string result = "📊 Active Orders: " + std::to_string(totalActive) + "\n";
        result += "🪑 Dine-In: " + std::to_string(dineInCount) + " | ";
        result += "🚗 Delivery: " + std::to_string(deliveryCount) + " | ";
        result += "🚶 Walk-In: " + std::to_string(walkInCount) + "\n";
        
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

std::string RestaurantPOSApp::getOrderTypeIcon(const std::string& tableIdentifier) const {
    if (tableIdentifier.find("table") == 0) {
        return "[TABLE]";
    } else if (tableIdentifier == "grubhub" || tableIdentifier == "ubereats") {
        return "[DELIVERY]";
    } else if (tableIdentifier == "walk-in") {
        return "[WALKIN]";
    }
    return "[ORDER]";
}

bool RestaurantPOSApp::isTableIdentifierAvailable(const std::string& identifier) const {
    return !posService_->isTableIdentifierInUse(identifier);
}

void RestaurantPOSApp::logApplicationStart() {
    std::cout << "=== Restaurant POS Application Starting ===" << std::endl;
    std::cout << "Version: 2.2.0 - Enhanced with Theme Management" << std::endl;
    std::cout << "Features: String-based table identifiers, delivery support, theme switching" << std::endl;
}

void RestaurantPOSApp::updateStatus(const std::string& message) {
    statusText_->setText(message);
    std::cout << "[POS] " << message << std::endl;
}

std::string RestaurantPOSApp::getCurrentThemeDisplayName() const {
    if (themeService_) {
        return themeService_->getThemeName(themeService_->getCurrentTheme());
    }
    return "Default";
}

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
