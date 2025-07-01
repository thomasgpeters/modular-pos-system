#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrapTheme.h>
#include <Wt/WLabel.h>
#include <Wt/WCssTheme.h>

#include <iostream>
#include <exception>

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env) 
    : Wt::WApplication(env), mainContainer_(nullptr), statusText_(nullptr), 
      testButton_(nullptr), updateTimer_(nullptr) {
    
    try {
        logApplicationStart();
        
        // Set basic application properties
        setTitle("Restaurant POS System - Modular Architecture");
        
        // FIXED: Properly configure Bootstrap theme with CSS loading
        setupBootstrapTheme();
        
        // FIXED: Add custom CSS for better styling
        addCustomCSS();
        
        // Initialize services
        initializeServices();
        
        // Set up the UI
        setupMainLayout();
        
        // Set up event listeners
        setupEventListeners();
        
        // Set up real-time updates
        setupRealTimeUpdates();
        
        updateStatus("Application initialized successfully");
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize application: " << e.what() << std::endl;
        updateStatus("Failed to initialize: " + std::string(e.what()));
    }
}

void RestaurantPOSApp::setupBootstrapTheme() {
    // Set up Bootstrap 3 theme with proper CSS loading
    auto bootstrapTheme = std::make_shared<Wt::WBootstrapTheme>();
    bootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
    
    // Enable responsive features
    bootstrapTheme->setResponsive(true);
    
    setTheme(bootstrapTheme);
    
    // Add viewport meta tag for mobile responsiveness
    addMetaHeader("viewport", "width=device-width, initial-scale=1.0");
    
    std::cout << "✅ Bootstrap theme configured" << std::endl;
}

void RestaurantPOSApp::addCustomCSS() {
    // Add custom CSS for POS styling
    useStyleSheet(Wt::WLink("https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css"));
    
    // Add custom inline CSS
    styleSheet().addRule(".pos-application", 
        "padding: 20px; background-color: #f8f9fa; min-height: 100vh;");
    
    styleSheet().addRule(".pos-header", 
        "background-color: #ffffff; padding: 20px; margin-bottom: 20px; "
        "border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);");
    
    styleSheet().addRule(".status-card", 
        "background-color: #ffffff; border: 1px solid #dee2e6; "
        "border-radius: 8px; padding: 15px; margin-bottom: 15px;");
    
    styleSheet().addRule(".menu-item", 
        "padding: 10px; border-bottom: 1px solid #eee; "
        "background-color: #ffffff; margin-bottom: 5px; border-radius: 4px;");
    
    styleSheet().addRule(".menu-item:hover", 
        "background-color: #f8f9fa; cursor: pointer;");
    
    styleSheet().addRule(".order-summary", 
        "background-color: #e3f2fd; padding: 15px; border-radius: 8px; "
        "border-left: 4px solid #2196f3;");
    
    styleSheet().addRule(".btn-pos", 
        "margin: 5px; padding: 10px 20px; font-weight: bold;");
    
    styleSheet().addRule(".system-status", 
        "background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); "
        "color: white; padding: 15px; border-radius: 8px; margin-bottom: 20px;");
    
    styleSheet().addRule(".menu-section", 
        "background-color: #ffffff; border-radius: 8px; padding: 20px; "
        "box-shadow: 0 2px 8px rgba(0,0,0,0.1); margin-bottom: 20px;");
    
    std::cout << "✅ Custom CSS added" << std::endl;
}

void RestaurantPOSApp::initializeServices() {
    std::cout << "Initializing services..." << std::endl;
    
    // Initialize event manager
    eventManager_ = std::make_shared<EventManager>();
    
    // Initialize core business service
    posService_ = std::make_shared<POSService>(eventManager_);
    
    // Initialize the menu
    posService_->initializeMenu();
    
    std::cout << "✅ All services initialized" << std::endl;
}

void RestaurantPOSApp::setupMainLayout() {
    std::cout << "Setting up main layout..." << std::endl;
    
    // Create main container with proper CSS classes
    mainContainer_ = root()->addNew<Wt::WContainerWidget>();
    mainContainer_->setStyleClass("pos-application container-fluid");
    
    // Create layout
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Add system status header
    auto statusHeader = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusHeader->setStyleClass("system-status");
    
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Title section
    auto titleContainer = headerLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto title = titleContainer->addNew<Wt::WText>("🍽️ Restaurant POS System");
    title->setStyleClass("h2");
    
    auto subtitle = titleContainer->addNew<Wt::WText>("Modular Architecture: Service Layer • Event System • Business Logic");
    subtitle->setStyleClass("small");
    
    // Status section
    statusText_ = headerLayout->addWidget(std::make_unique<Wt::WText>("Initializing..."));
    statusText_->setStyleClass("h4 text-right");
    
    statusHeader->setLayout(std::move(headerLayout));
    
    // Add main content area
    auto contentRow = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentRow->setStyleClass("row");
    
    // Left column - Menu and Test
    auto leftCol = contentRow->addNew<Wt::WContainerWidget>();
    leftCol->setStyleClass("col-md-8");
    
    // Test button section
    auto testSection = leftCol->addNew<Wt::WContainerWidget>();
    testSection->setStyleClass("status-card text-center");
    
    testButton_ = testSection->addNew<Wt::WPushButton>("Test Order Creation");
    testButton_->setStyleClass("btn btn-primary btn-lg btn-pos");
    testButton_->clicked().connect(this, &RestaurantPOSApp::onTestButtonClicked);
    
    // Menu display section
    auto menuSection = leftCol->addNew<Wt::WContainerWidget>();
    menuSection->setStyleClass("menu-section");
    
    auto menuHeader = menuSection->addNew<Wt::WText>("📋 Available Menu Items");
    menuHeader->setStyleClass("h3 text-primary");
    
    auto menuGrid = menuSection->addNew<Wt::WContainerWidget>();
    menuGrid->setStyleClass("row");
    
    // Display menu items in a nice grid
    const auto& menuItems = posService_->getMenuItems();
    int itemCount = 0;
    for (const auto& item : menuItems) {
        if (itemCount % 2 == 0 && itemCount > 0) {
            // Start new row every 2 items
            menuGrid = menuSection->addNew<Wt::WContainerWidget>();
            menuGrid->setStyleClass("row");
        }
        
        auto itemCol = menuGrid->addNew<Wt::WContainerWidget>();
        itemCol->setStyleClass("col-md-6");
        
        auto itemCard = itemCol->addNew<Wt::WContainerWidget>();
        itemCard->setStyleClass("menu-item");
        
        // Item header with name and price
        auto itemHeader = itemCard->addNew<Wt::WContainerWidget>();
        auto itemName = itemHeader->addNew<Wt::WText>(item->getName());
        itemName->setStyleClass("h5 pull-left");
        
        auto itemPrice = itemHeader->addNew<Wt::WText>("$" + std::to_string(item->getPrice()).substr(0, 5));
        itemPrice->setStyleClass("h5 text-success pull-right");
        
        itemCard->addNew<Wt::WText>("<div class='clearfix'></div>", Wt::TextFormat::UnsafeXHTML);
        
        // Item category
        auto itemCategory = itemCard->addNew<Wt::WText>(MenuItem::categoryToString(item->getCategory()));
        itemCategory->setStyleClass("text-muted small");
        
        itemCount++;
    }
    
    // Right column - Order Status
    auto rightCol = contentRow->addNew<Wt::WContainerWidget>();
    rightCol->setStyleClass("col-md-4");
    
    // Order summary section
    auto orderSection = rightCol->addNew<Wt::WContainerWidget>();
    orderSection->setStyleClass("order-summary");
    
    auto orderHeader = orderSection->addNew<Wt::WText>("📊 System Status");
    orderHeader->setStyleClass("h4");
    
    auto activeOrdersText = orderSection->addNew<Wt::WText>("Active Orders: Loading...");
    activeOrdersText->setStyleClass("lead");
    
    auto transactionsText = orderSection->addNew<Wt::WText>("Transactions: Loading...");
    transactionsText->setStyleClass("lead");
    
    // Set layout
    mainContainer_->setLayout(std::move(layout));
    
    std::cout << "✅ Enhanced layout setup complete" << std::endl;
}

void RestaurantPOSApp::setupEventListeners() {
    std::cout << "Setting up event listeners..." << std::endl;
    
    // Subscribe to order events
    eventManager_->subscribe(POSEvents::ORDER_CREATED, 
        [this](const std::any& data) {
            updateStatus("✅ Order created successfully!");
        });
    
    eventManager_->subscribe(POSEvents::ORDER_MODIFIED,
        [this](const std::any& data) {
            updateStatus("📝 Order modified");
        });
    
    std::cout << "✅ Event listeners setup complete" << std::endl;
}

void RestaurantPOSApp::setupRealTimeUpdates() {
    std::cout << "Setting up real-time updates..." << std::endl;
    
    updateTimer_ = root()->addChild(std::make_unique<Wt::WTimer>());
    updateTimer_->setInterval(std::chrono::seconds(5));
    updateTimer_->timeout().connect(this, &RestaurantPOSApp::onPeriodicUpdate);
    updateTimer_->start();
    
    std::cout << "✅ Real-time updates configured" << std::endl;
}

void RestaurantPOSApp::onTestButtonClicked() {
    try {
        // Create a test order
        auto order = posService_->createOrder(5); // Table 5
        
        if (order) {
            // Add some items to the order
            const auto& menuItems = posService_->getMenuItems();
            if (!menuItems.empty()) {
                posService_->setCurrentOrder(order);
                posService_->addItemToCurrentOrder(menuItems[0], 2, "Extra sauce");
                if (menuItems.size() > 1) {
                    posService_->addItemToCurrentOrder(menuItems[1], 1);
                }
            }
            
            updateStatus("✅ Test order #" + std::to_string(order->getOrderId()) + 
                        " created for Table " + std::to_string(order->getTableNumber()) +
                        " | Total: $" + std::to_string(order->getTotal()).substr(0, 5));
            
            // Update button
            testButton_->setText("✅ Order Created! Create Another?");
            testButton_->setStyleClass("btn btn-success btn-lg btn-pos");
            
        } else {
            updateStatus("❌ Failed to create test order");
        }
        
    } catch (const std::exception& e) {
        updateStatus("❌ Error: " + std::string(e.what()));
    }
}

void RestaurantPOSApp::onPeriodicUpdate() {
    try {
        // Get business statistics
        auto stats = posService_->getBusinessStatistics();
        
        // Update status with current stats
        updateStatus("📊 " + std::to_string(stats["activeOrderCount"].orIfNull(0)) + 
                    " active orders • " + std::to_string(stats["totalTransactions"].orIfNull(0)) + 
                    " transactions • Revenue: $" + std::to_string(stats["totalRevenue"].orIfNull(0.0)).substr(0, 6));
        
        // Reset button if needed
        if (testButton_->text().toUTF8().find("Order Created") != std::string::npos) {
            testButton_->setText("Create Another Test Order");
            testButton_->setStyleClass("btn btn-primary btn-lg btn-pos");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error during periodic update: " << e.what() << std::endl;
    }
}

void RestaurantPOSApp::logApplicationStart() {
    std::cout << "===========================================================" << std::endl;
    std::cout << "  Restaurant POS System - Enhanced UI v2.0.0" << std::endl;
    std::cout << "===========================================================" << std::endl;
    std::cout << "Framework: Wt (C++ Web Toolkit)" << std::endl;
    std::cout << "Architecture: Modular Service-Component Design" << std::endl;
    std::cout << "UI: Bootstrap 3 + Custom CSS" << std::endl;
    std::cout << "===========================================================" << std::endl;
}

void RestaurantPOSApp::updateStatus(const std::string& message) {
    if (statusText_) {
        statusText_->setText(message);
        std::cout << "Status: " << message << std::endl;
    }
}

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}