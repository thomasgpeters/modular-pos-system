#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WCssDecorationStyle.h>
#include <iostream>

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , eventManager_(nullptr)
    , posService_(nullptr)
    , themeService_(nullptr)
    , configManager_(nullptr)
    , componentFactory_(nullptr)
    , mainContainer_(nullptr)
    , headerContainer_(nullptr)
    , contentContainer_(nullptr)
    , statusBarContainer_(nullptr)
    , themeControlsContainer_(nullptr)
    , themeSelector_(nullptr)
    , themeToggleButton_(nullptr)
    , orderEntryPanel_(nullptr)
    , menuDisplay_(nullptr)
    , currentOrderDisplay_(nullptr)
    , orderStatusPanel_(nullptr)
    , statusText_(nullptr)
    , updateTimer_(nullptr)
{
    logApplicationStart();
    
    try {
        // Setup meta tags and theming first
        setupMetaTags();
        setupBootstrapTheme();
        addCustomCSS();
        addThemeSpecificCSS();
        
        // Initialize services in proper order
        initializeServices();
        initializeComponentFactory();
        
        // Setup main layout structure
        setupMainLayout();
        setupHeaderWithThemeControls();
        
        // Create all components using factory
        createAllComponents();
        
        // Setup content layout with components
        setupContentLayout();
        setupStatusBar();
        
        // Setup events and updates
        setupEventListeners();
        setupThemeEventHandlers();
        setupRealTimeUpdates();
        
        // Apply styling and theming
        applyComponentStyling();
        applyThemeToComponents();
        
        // Validate everything is set up correctly
        validateComponents();
        
        std::cout << "[SUCCESS] Restaurant POS Application with Flat Architecture initialized!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Application initialization failed: " << e.what() << std::endl;
        if (statusText_) {
            statusText_->setText("❌ Initialization Error: " + std::string(e.what()));
        }
    }
}

void RestaurantPOSApp::initializeServices() {
    std::cout << "[INIT] Initializing core services..." << std::endl;
    
    // Create event manager first
    eventManager_ = std::make_shared<EventManager>();
    
    // Create configuration manager
    configManager_ = std::make_shared<ConfigurationManager>();
    configManager_->initialize();
    
    // Create POS service with event manager
    posService_ = std::make_shared<POSService>(eventManager_);
    posService_->initializeMenu();
    
    // Initialize theme service
    initializeThemeService();
    
    std::cout << "[INIT] Core services initialized successfully" << std::endl;
}

void RestaurantPOSApp::initializeComponentFactory() {
    std::cout << "[INIT] Creating UI component factory..." << std::endl;
    
    if (!posService_ || !eventManager_ || !configManager_) {
        throw std::runtime_error("Cannot create UIComponentFactory: missing required services");
    }
    
    // Create component factory with all dependencies
    componentFactory_ = std::make_unique<UIComponentFactory>(
        posService_, eventManager_, configManager_);
    
    // Register optional services
    if (themeService_) {
        componentFactory_->registerThemeService(themeService_);
    }
    
    std::cout << "[INIT] UI component factory created successfully" << std::endl;
}

void RestaurantPOSApp::initializeThemeService() {
    try {
        themeService_ = std::make_shared<ThemeService>(this);
        themeService_->loadThemePreference();
        
        std::cout << "[THEME] Initialized with theme: " 
                  << themeService_->getThemeName(themeService_->getCurrentTheme()) << std::endl;
                  
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] ThemeService initialization failed: " << e.what() << std::endl;
        themeService_ = nullptr;
    }
}

void RestaurantPOSApp::setupMainLayout() {
    setTitle("Restaurant POS System - Flat Architecture");
    
    // Create main container with simple vertical layout
    mainContainer_ = root()->addNew<Wt::WContainerWidget>();
    mainContainer_->addStyleClass("pos-main-container container-fluid");
    
    // Header section (direct child)
    headerContainer_ = mainContainer_->addNew<Wt::WContainerWidget>();
    headerContainer_->addStyleClass("pos-header-section");
    
    // Content section (direct child)
    contentContainer_ = mainContainer_->addNew<Wt::WContainerWidget>();
    contentContainer_->addStyleClass("pos-content-section");
    
    // Status bar section (direct child)
    statusBarContainer_ = mainContainer_->addNew<Wt::WContainerWidget>();
    statusBarContainer_->addStyleClass("pos-status-section");
    
    std::cout << "[LAYOUT] Simple main layout structure created" << std::endl;
}

void RestaurantPOSApp::createAllComponents() {
    std::cout << "[COMPONENTS] Creating all components using factory..." << std::endl;
    
    if (!componentFactory_) {
        throw std::runtime_error("ComponentFactory not initialized");
    }
    
    // Create each component separately using the factory
    createOrderEntryPanel();
    createMenuDisplay();
    createCurrentOrderDisplay();
    createOrderStatusPanel();
    
    std::cout << "[COMPONENTS] All components created successfully" << std::endl;
}

void RestaurantPOSApp::createOrderEntryPanel() {
    std::cout << "[COMPONENT] Creating OrderEntryPanel..." << std::endl;
    
    auto orderEntryComponent = componentFactory_->createOrderEntryPanel();
    if (!orderEntryComponent) {
        throw std::runtime_error("Failed to create OrderEntryPanel");
    }
    
    // Add to content container and store pointer
    orderEntryPanel_ = contentContainer_->addWidget(std::move(orderEntryComponent));
    orderEntryPanel_->addStyleClass("order-entry-section");
    
    std::cout << "[COMPONENT] OrderEntryPanel created successfully" << std::endl;
}

void RestaurantPOSApp::createMenuDisplay() {
    std::cout << "[COMPONENT] Creating MenuDisplay..." << std::endl;
    
    auto menuComponent = componentFactory_->createMenuDisplay();
    if (!menuComponent) {
        throw std::runtime_error("Failed to create MenuDisplay");
    }
    
    // Add to content container and store pointer
    menuDisplay_ = contentContainer_->addWidget(std::move(menuComponent));
    menuDisplay_->addStyleClass("menu-display-section");
    
    std::cout << "[COMPONENT] MenuDisplay created successfully" << std::endl;
}

void RestaurantPOSApp::createCurrentOrderDisplay() {
    std::cout << "[COMPONENT] Creating CurrentOrderDisplay..." << std::endl;
    
    auto currentOrderComponent = componentFactory_->createCurrentOrderDisplay();
    if (!currentOrderComponent) {
        throw std::runtime_error("Failed to create CurrentOrderDisplay");
    }
    
    // Add to content container and store pointer
    currentOrderDisplay_ = contentContainer_->addWidget(std::move(currentOrderComponent));
    currentOrderDisplay_->addStyleClass("current-order-section");
    
    std::cout << "[COMPONENT] CurrentOrderDisplay created successfully" << std::endl;
}

void RestaurantPOSApp::createOrderStatusPanel() {
    std::cout << "[COMPONENT] Creating OrderStatusPanel..." << std::endl;
    
    auto statusPanelComponent = componentFactory_->createOrderStatusPanel();
    if (!statusPanelComponent) {
        throw std::runtime_error("Failed to create OrderStatusPanel");
    }
    
    // Add to content container and store pointer
    orderStatusPanel_ = contentContainer_->addWidget(std::move(statusPanelComponent));
    orderStatusPanel_->addStyleClass("order-status-section");
    
    std::cout << "[COMPONENT] OrderStatusPanel created successfully" << std::endl;
}

void RestaurantPOSApp::setupHeaderWithThemeControls() {
    headerContainer_->addStyleClass("pos-header row align-items-center p-3 mb-4");
    
    // Left side - Application title (direct child)
    auto titleContainer = headerContainer_->addNew<Wt::WContainerWidget>();
    titleContainer->addStyleClass("col-md-8");
    
    auto headerTitle = titleContainer->addNew<Wt::WText>("🍽️ Restaurant POS System - Flat Architecture");
    headerTitle->addStyleClass("h2 mb-0 pos-app-title");
    
    // Right side - Theme controls (direct child)
    themeControlsContainer_ = headerContainer_->addNew<Wt::WContainerWidget>();
    themeControlsContainer_->addStyleClass("col-md-4 text-right");
    
    setupThemeControls();
    
    std::cout << "[HEADER] Simple header with theme controls created" << std::endl;
}

void RestaurantPOSApp::setupContentLayout() {
    // Simple approach: widgets are already added to contentContainer_
    // Just apply styling classes for spacing
    
    if (orderEntryPanel_) {
        orderEntryPanel_->addStyleClass("mb-4");
    }
    
    if (menuDisplay_) {
        menuDisplay_->addStyleClass("mb-4");
    }
    
    if (currentOrderDisplay_) {
        currentOrderDisplay_->addStyleClass("mb-4");
    }
    
    if (orderStatusPanel_) {
        orderStatusPanel_->addStyleClass("mt-2");
    }
    
    std::cout << "[LAYOUT] Content layout styling applied" << std::endl;
}

void RestaurantPOSApp::setupStatusBar() {
    statusBarContainer_->addStyleClass("pos-status-bar p-2 mt-2 bg-light border-top");
    
    // Status text (direct child)
    statusText_ = statusBarContainer_->addNew<Wt::WText>("System Ready - Flat Architecture");
    statusText_->addStyleClass("mb-0 text-muted");
    
    // Component count info (direct child)
    auto componentInfo = statusBarContainer_->addNew<Wt::WText>(" | 4 components loaded");
    componentInfo->addStyleClass("small text-info");
    
    std::cout << "[STATUS] Simple status bar created" << std::endl;
}

void RestaurantPOSApp::setupThemeControls() {
    if (!themeService_) {
        std::cout << "[WARNING] ThemeService not available, skipping theme controls" << std::endl;
        return;
    }
    
    // Theme selector dropdown (direct child)
    createThemeSelector();
    
    // Theme toggle button (direct child)
    createThemeToggleButton();
    
    std::cout << "[THEME] Simple theme controls created" << std::endl;
}

void RestaurantPOSApp::createThemeSelector() {
    if (!themeService_) return;
    
    themeSelector_ = themeControlsContainer_->addNew<Wt::WComboBox>();
    themeSelector_->addStyleClass("form-select pos-theme-selector me-2");
    
    auto themes = themeService_->getAvailableThemes();
    for (const auto& theme : themes) {
        std::string text = themeService_->getThemeIcon(theme) + " " + 
                          themeService_->getThemeName(theme);
        themeSelector_->addItem(text);
    }
    
    auto currentTheme = themeService_->getCurrentTheme();
    auto it = std::find(themes.begin(), themes.end(), currentTheme);
    if (it != themes.end()) {
        themeSelector_->setCurrentIndex(std::distance(themes.begin(), it));
    }
}

void RestaurantPOSApp::createThemeToggleButton() {
    if (!themeService_) return;
    
    themeToggleButton_ = themeControlsContainer_->addNew<Wt::WPushButton>();
    themeToggleButton_->addStyleClass("btn btn-outline-secondary pos-theme-toggle");
    
    updateThemeControls();
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
    
    std::cout << "[EVENTS] Event listeners setup complete" << std::endl;
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
    
    std::cout << "[THEME] Theme event handlers setup complete" << std::endl;
}

void RestaurantPOSApp::setupRealTimeUpdates() {
    try {
        updateTimer_ = root()->addChild(std::make_unique<Wt::WTimer>());
        updateTimer_->setInterval(std::chrono::seconds(30)); // Less frequent updates
        
        updateTimer_->timeout().connect([this] {
            try {
                onPeriodicUpdate();
            } catch (...) {
                // Silently handle timer update errors
            }
        });
        
        updateTimer_->start();
        
        std::cout << "[TIMER] Real-time updates setup complete" << std::endl;
        
    } catch (...) {
        std::cout << "[WARNING] Could not setup auto-refresh timer" << std::endl;
    }
}

// Event handlers
void RestaurantPOSApp::onPeriodicUpdate() {
    // Refresh all components
    if (orderEntryPanel_) orderEntryPanel_->refresh();
    if (menuDisplay_) menuDisplay_->refresh();
    if (currentOrderDisplay_) currentOrderDisplay_->refresh();
    if (orderStatusPanel_) orderStatusPanel_->refresh();
    
    updateStatus("Auto-refresh completed");
}

void RestaurantPOSApp::onOrderCreated(std::shared_ptr<Order> order) {
    if (order) {
        updateStatus("✅ Order #" + std::to_string(order->getOrderId()) + " created");
    }
}

void RestaurantPOSApp::onOrderModified(std::shared_ptr<Order> order) {
    if (order) {
        updateStatus("📝 Order #" + std::to_string(order->getOrderId()) + " modified");
    }
}

void RestaurantPOSApp::onCurrentOrderChanged() {
    updateStatus("🔄 Current order changed");
}

// Theme event handlers
void RestaurantPOSApp::onThemeChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
    std::cout << "[THEME] Changed from " << themeService_->getThemeName(oldTheme) 
              << " to " << themeService_->getThemeName(newTheme) << std::endl;
    
    applyThemeTransition();
    updateThemeControls();
    applyThemeToComponents();
    
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
    
    if (themeToggleButton_) {
        std::string buttonText = themeService_->getThemeIcon(currentTheme) + " " + 
                               themeService_->getThemeName(currentTheme);
        themeToggleButton_->setText(buttonText);
    }
    
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
    
    // Apply theme to main containers
    themeService_->applyThemeToContainer(mainContainer_);
    if (headerContainer_) themeService_->applyThemeToContainer(headerContainer_);
    if (contentContainer_) themeService_->applyThemeToContainer(contentContainer_);
    if (statusBarContainer_) themeService_->applyThemeToContainer(statusBarContainer_);
    
    // Individual components handle their own theming through their event subscriptions
}

void RestaurantPOSApp::applyThemeTransition() {
    if (themeService_) {
        ThemeUtils::applyThemeTransition(this, 300);
    }
}

// CSS and styling methods (same as before)
void RestaurantPOSApp::setupBootstrapTheme() {
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/bootstrap/4.6.2/css/bootstrap.min.css");
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css");
    require("https://cdnjs.cloudflare.com/ajax/libs/bootstrap/4.6.2/js/bootstrap.bundle.min.js");
}

void RestaurantPOSApp::addCustomCSS() {
    styleSheet().addRule(".pos-application", "padding: 20px;");
    styleSheet().addRule(".pos-header-section", 
        "background-color: var(--pos-header-bg, #f8f9fa); "
        "color: var(--pos-header-text, #212529); "
        "border-radius: 0.375rem; border: 1px solid var(--pos-border-color, #dee2e6);");
    
    styleSheet().addRule(".pos-content-section", 
        "background-color: var(--pos-main-bg, #ffffff); "
        "color: var(--pos-main-text, #212529); "
        "padding: 1rem; min-height: 400px;");
    
    styleSheet().addRule(".pos-status-section", 
        "background-color: var(--pos-card-bg, #f8f9fa); "
        "color: var(--pos-card-text, #6c757d); "
        "border: 1px solid var(--pos-border-color, #dee2e6);");
    
    // Component section styling - simplified for vertical stack
    styleSheet().addRule(".order-entry-section", 
        "border: 2px solid #28a745; border-radius: 8px; padding: 15px; "
        "background-color: var(--pos-card-bg, #f8fff9);");
    
    styleSheet().addRule(".menu-display-section", 
        "border: 2px solid #007bff; border-radius: 8px; padding: 15px; "
        "background-color: var(--pos-card-bg, #f0f8ff);");
    
    styleSheet().addRule(".current-order-section", 
        "border: 2px solid #17a2b8; border-radius: 8px; padding: 15px; "
        "background-color: var(--pos-card-bg, #f0fdff);");
    
    styleSheet().addRule(".order-status-section", 
        "border: 2px solid #6f42c1; border-radius: 8px; padding: 15px; "
        "background-color: var(--pos-card-bg, #faf9ff);");
    
    // OrderEntryPanel specific styling to replace layouts
    styleSheet().addRule(".table-selection-group", "padding: 20px;");
    styleSheet().addRule(".order-actions-section", "padding: 20px; text-align: center;");
    styleSheet().addRule(".table-identifier-combo", "width: 100%; max-width: 400px;");
    styleSheet().addRule(".action-buttons-container", "margin-top: 15px;");
    
    // Responsive design
    styleSheet().addRule("@media (min-width: 768px)", 
        ".pos-content-section { display: flex; flex-wrap: wrap; gap: 1rem; }");
    
    styleSheet().addRule("@media (min-width: 768px)", 
        ".order-entry-section { flex: 1 1 100%; }");
    
    styleSheet().addRule("@media (min-width: 768px)", 
        ".menu-display-section { flex: 2 1 60%; }");
    
    styleSheet().addRule("@media (min-width: 768px)", 
        ".current-order-section { flex: 1 1 35%; }");
    
    styleSheet().addRule("@media (min-width: 768px)", 
        ".order-status-section { flex: 1 1 100%; }");
}

void RestaurantPOSApp::addThemeSpecificCSS() {
    // Light theme CSS variables
    styleSheet().addRule(".theme-light", 
        "--pos-main-bg: #f8f9fa; --pos-main-text: #212529; "
        "--pos-header-bg: #ffffff; --pos-header-text: #495057; "
        "--pos-card-bg: #ffffff; --pos-card-text: #212529; "
        "--pos-border-color: #dee2e6;");
    
    // Dark theme CSS variables
    styleSheet().addRule(".theme-dark", 
        "--pos-main-bg: #212529; --pos-main-text: #f8f9fa; "
        "--pos-header-bg: #343a40; --pos-header-text: #f8f9fa; "
        "--pos-card-bg: #495057; --pos-card-text: #f8f9fa; "
        "--pos-border-color: #6c757d;");
    
    // Base theme CSS variables
    styleSheet().addRule(".theme-base", 
        "--pos-main-bg: #ffffff; --pos-main-text: #495057; "
        "--pos-header-bg: #f8f9fa; --pos-header-text: #495057; "
        "--pos-card-bg: #ffffff; --pos-card-text: #495057; "
        "--pos-border-color: #ced4da;");
}

void RestaurantPOSApp::setupMetaTags() {
    addMetaHeader("viewport", "width=device-width, initial-scale=1.0");
    addMetaHeader("description", "Restaurant POS System - Flat Component Architecture");
}

void RestaurantPOSApp::applyComponentStyling() {
    styleSheet().addRule(".pos-theme-selector", "min-width: 150px;");
    styleSheet().addRule(".pos-theme-toggle", "min-width: 120px;");
    styleSheet().addRule(".pos-app-title", "color: var(--pos-title-color, #495057);");
    
    // Component transition support
    styleSheet().addRule(".pos-theme-transition", 
        "transition: background-color 0.3s ease, color 0.3s ease, "
        "border-color 0.3s ease, box-shadow 0.3s ease !important;");
}

// Utility methods
void RestaurantPOSApp::validateComponents() const {
    std::vector<std::string> missing;
    
    if (!orderEntryPanel_) missing.push_back("OrderEntryPanel");
    if (!menuDisplay_) missing.push_back("MenuDisplay");
    if (!currentOrderDisplay_) missing.push_back("CurrentOrderDisplay");
    if (!orderStatusPanel_) missing.push_back("OrderStatusPanel");
    
    if (!missing.empty()) {
        std::string errorMsg = "Missing components: ";
        for (size_t i = 0; i < missing.size(); ++i) {
            if (i > 0) errorMsg += ", ";
            errorMsg += missing[i];
        }
        throw std::runtime_error(errorMsg);
    }
    
    std::cout << "[VALIDATION] All components validated successfully" << std::endl;
}

void RestaurantPOSApp::logApplicationStart() {
    std::cout << "=== Restaurant POS Application Starting (Flat Architecture) ===" << std::endl;
    std::cout << "Version: 2.3.0 - Flattened Component Hierarchy" << std::endl;
    std::cout << "Architecture: Individual components created via UIComponentFactory" << std::endl;
}

void RestaurantPOSApp::updateStatus(const std::string& message) {
    if (statusText_) {
        statusText_->setText(message);
    }
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
