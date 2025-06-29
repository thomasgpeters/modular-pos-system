#include "../include/RestaurantPOSApp.hpp"

#include <Wt/WBootstrapTheme.h>
#include <Wt/WCssTheme.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPanel.h>
#include <Wt/WTabWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WTextArea.h>
#include <Wt/WMessageBox.h>
#include <Wt/WGroupBox.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WDialog.h>
#include <Wt/WLink.h>

#include <sstream>
#include <iomanip>

/**
 * @file RestaurantPOSApp.cpp
 * @brief Implementation of the RestaurantPOSApp class with theme selection
 */

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env) : Wt::WApplication(env) {
    setTitle("Restaurant POS System - Three-Legged Foundation");
    
    // Initialize theme system
    initializeThemes();
    
    // Initialize core components
    orderManager_ = std::make_unique<OrderManager>();
    paymentProcessor_ = std::make_unique<PaymentProcessor>();
    kitchenInterface_ = std::make_unique<KitchenInterface>();
    
    // Initialize sample menu items
    initializeSampleMenu();
    
    // Build user interface
    buildMainInterface();
    
    // Set up real-time updates
    setupRealTimeUpdates();
}

void RestaurantPOSApp::initializeThemes() {
    // Initialize available themes
    availableThemes_ = {
        {"bootstrap", "Bootstrap (Default)", "Modern responsive Bootstrap theme"},
        {"bootstrap-dark", "Bootstrap Dark", "Dark mode Bootstrap theme"},
        {"classic", "Classic", "Clean classic theme"},
        {"professional", "Professional", "Professional business theme"},
        {"colorful", "Colorful", "Vibrant colorful theme"}
    };
    
    // Set default theme
    currentTheme_ = "bootstrap";
    applyTheme(currentTheme_);
}

void RestaurantPOSApp::applyTheme(const std::string& themeName) {
    if (themeName == "bootstrap" || themeName == "bootstrap-dark") {
        auto bootstrapTheme = std::make_shared<Wt::WBootstrapTheme>();
        
        if (themeName == "bootstrap-dark") {
            // Add dark theme CSS
            bootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
            useStyleSheet(Wt::WLink("https://stackpath.bootstrapcdn.com/bootswatch/3.4.1/darkly/bootstrap.min.css"));
        } else {
            bootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
        }
        
        setTheme(bootstrapTheme);
        
    } else {
        // Use custom CSS theme
        auto cssTheme = std::make_shared<Wt::WCssTheme>("polished");
        setTheme(cssTheme);
        
        // Apply custom theme-specific CSS
        if (themeName == "classic") {
            useStyleSheet(createClassicThemeCSS());
        } else if (themeName == "professional") {
            useStyleSheet(createProfessionalThemeCSS());
        } else if (themeName == "colorful") {
            useStyleSheet(createColorfulThemeCSS());
        }
    }
    
    currentTheme_ = themeName;
    
    // Add common custom styles for POS system
    useStyleSheet(createPOSCustomCSS());
    
    // Refresh the UI to apply new theme
    if (root()->children().size() > 0) {
        triggerUpdate();
    }
}

Wt::WLink RestaurantPOSApp::createClassicThemeCSS() {
    // Create classic theme CSS inline
    std::string css = R"(
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            background-color: #f8f9fa; 
        }
        .container-fluid { 
            max-width: 1400px; 
            margin: 0 auto; 
        }
        .table { 
            background-color: white; 
            border: 1px solid #dee2e6; 
        }
        .table th { 
            background-color: #e9ecef; 
            border-bottom: 2px solid #dee2e6; 
        }
        .btn { 
            border-radius: 4px; 
            font-weight: 500; 
        }
        .btn-primary { 
            background-color: #007bff; 
            border-color: #007bff; 
        }
        .btn-success { 
            background-color: #28a745; 
            border-color: #28a745; 
        }
        .border { 
            border-color: #dee2e6 !important; 
        }
        .bg-light { 
            background-color: #f8f9fa !important; 
        }
        .text-primary { 
            color: #007bff !important; 
        }
    )";
    
    // In a real application, you'd save this to a file or serve it dynamically
    return Wt::WLink(Wt::WLink::Url, "data:text/css;base64," + Wt::Utils::base64Encode(css));
}

Wt::WLink RestaurantPOSApp::createProfessionalThemeCSS() {
    std::string css = R"(
        body { 
            font-family: 'Roboto', 'Arial', sans-serif; 
            background-color: #f5f5f5; 
            color: #333; 
        }
        .container-fluid { 
            max-width: 1400px; 
            margin: 0 auto; 
        }
        .table { 
            background-color: white; 
            border: none; 
            box-shadow: 0 2px 4px rgba(0,0,0,0.1); 
        }
        .table th { 
            background-color: #2c3e50; 
            color: white; 
            border: none; 
            font-weight: 600; 
        }
        .table td { 
            border-top: 1px solid #ecf0f1; 
        }
        .btn { 
            border-radius: 3px; 
            font-weight: 500; 
            text-transform: uppercase; 
            letter-spacing: 0.5px; 
        }
        .btn-primary { 
            background-color: #3498db; 
            border-color: #3498db; 
        }
        .btn-success { 
            background-color: #2ecc71; 
            border-color: #2ecc71; 
        }
        .btn-warning { 
            background-color: #f39c12; 
            border-color: #f39c12; 
        }
        .border { 
            border-color: #bdc3c7 !important; 
        }
        .bg-light { 
            background-color: #ecf0f1 !important; 
        }
        .text-primary { 
            color: #2c3e50 !important; 
        }
        .h2, .h4 { 
            color: #2c3e50; 
            font-weight: 300; 
        }
    )";
    
    return Wt::WLink(Wt::WLink::Url, "data:text/css;base64," + Wt::Utils::base64Encode(css));
}

Wt::WLink RestaurantPOSApp::createColorfulThemeCSS() {
    std::string css = R"(
        body { 
            font-family: 'Poppins', 'Arial', sans-serif; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); 
            color: #333; 
            min-height: 100vh; 
        }
        .container-fluid { 
            max-width: 1400px; 
            margin: 0 auto; 
            background-color: rgba(255,255,255,0.95); 
            border-radius: 15px; 
            padding: 20px; 
            margin-top: 20px; 
            box-shadow: 0 10px 30px rgba(0,0,0,0.2); 
        }
        .table { 
            background-color: white; 
            border: none; 
            border-radius: 10px; 
            overflow: hidden; 
            box-shadow: 0 5px 15px rgba(0,0,0,0.1); 
        }
        .table th { 
            background: linear-gradient(45deg, #ff6b6b, #ee5a52); 
            color: white; 
            border: none; 
            font-weight: 600; 
        }
        .table td { 
            border-top: 1px solid #f8f9fa; 
        }
        .btn { 
            border-radius: 25px; 
            font-weight: 600; 
            transition: all 0.3s ease; 
            border: none; 
        }
        .btn:hover { 
            transform: translateY(-2px); 
            box-shadow: 0 5px 15px rgba(0,0,0,0.2); 
        }
        .btn-primary { 
            background: linear-gradient(45deg, #667eea, #764ba2); 
        }
        .btn-success { 
            background: linear-gradient(45deg, #56ab2f, #a8e6cf); 
        }
        .btn-warning { 
            background: linear-gradient(45deg, #f093fb, #f5576c); 
        }
        .border { 
            border-color: #e9ecef !important; 
            border-radius: 10px !important; 
        }
        .bg-light { 
            background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%) !important; 
            border-radius: 10px; 
        }
        .text-primary { 
            color: #667eea !important; 
        }
        .h2, .h4 { 
            color: #667eea; 
            font-weight: 600; 
        }
    )";
    
    return Wt::WLink(Wt::WLink::Url, "data:text/css;base64," + Wt::Utils::base64Encode(css));
}

Wt::WLink RestaurantPOSApp::createPOSCustomCSS() {
    std::string css = R"(
        .pos-header {
            padding: 15px 0;
            margin-bottom: 20px;
            border-bottom: 2px solid #dee2e6;
        }
        .pos-theme-selector {
            position: absolute;
            top: 20px;
            right: 20px;
            z-index: 1000;
        }
        .theme-button {
            padding: 8px 15px;
            margin: 0 5px;
            border-radius: 20px;
            cursor: pointer;
            transition: all 0.3s ease;
        }
        .theme-button:hover {
            transform: scale(1.05);
        }
        .theme-dialog {
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        .theme-preview {
            width: 100%;
            height: 60px;
            border-radius: 8px;
            margin-bottom: 10px;
            cursor: pointer;
            transition: all 0.3s ease;
        }
        .theme-preview:hover {
            transform: scale(1.02);
            box-shadow: 0 5px 15px rgba(0,0,0,0.2);
        }
        .theme-preview.bootstrap {
            background: linear-gradient(45deg, #007bff, #0056b3);
        }
        .theme-preview.bootstrap-dark {
            background: linear-gradient(45deg, #343a40, #1a1e21);
        }
        .theme-preview.classic {
            background: linear-gradient(45deg, #f8f9fa, #e9ecef);
            border: 2px solid #dee2e6;
        }
        .theme-preview.professional {
            background: linear-gradient(45deg, #2c3e50, #3498db);
        }
        .theme-preview.colorful {
            background: linear-gradient(45deg, #667eea, #764ba2);
        }
        .notification {
            position: fixed;
            top: 20px;
            right: 20px;
            z-index: 9999;
            padding: 15px 20px;
            border-radius: 8px;
            color: white;
            font-weight: 500;
            animation: slideIn 0.3s ease;
        }
        @keyframes slideIn {
            from { transform: translateX(100%); }
            to { transform: translateX(0); }
        }
        .notification.success { background-color: #28a745; }
        .notification.info { background-color: #17a2b8; }
        .notification.warning { background-color: #ffc107; color: #333; }
        .notification.error { background-color: #dc3545; }
    )";
    
    return Wt::WLink(Wt::WLink::Url, "data:text/css;base64," + Wt::Utils::base64Encode(css));
}

void RestaurantPOSApp::showThemeSelector() {
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Choose Theme"));
    dialog->addStyleClass("theme-dialog");
    dialog->setWidth("500px");
    dialog->setResizable(false);
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Header
    auto header = std::make_unique<Wt::WText>("Select Your Preferred Theme");
    header->addStyleClass("h4 mb-4 text-center");
    layout->addWidget(std::move(header));
    
    // Theme options
    auto buttonGroup = std::make_shared<Wt::WButtonGroup>();
    
    for (const auto& theme : availableThemes_) {
        auto themeContainer = std::make_unique<Wt::WContainerWidget>();
        themeContainer->addStyleClass("mb-3 p-3 border rounded");
        
        auto themeLayout = std::make_unique<Wt::WHBoxLayout>();
        
        // Theme preview
        auto preview = std::make_unique<Wt::WContainerWidget>();
        preview->addStyleClass("theme-preview " + theme.id);
        preview->setWidth("80px");
        preview->setHeight("60px");
        themeLayout->addWidget(std::move(preview));
        
        // Theme info
        auto infoContainer = std::make_unique<Wt::WContainerWidget>();
        auto infoLayout = std::make_unique<Wt::WVBoxLayout>();
        
        auto radio = std::make_unique<Wt::WRadioButton>(theme.name);
        radio->addStyleClass("h6 mb-1");
        if (theme.id == currentTheme_) {
            radio->setChecked(true);
        }
        buttonGroup->addButton(radio.get());
        
        auto description = std::make_unique<Wt::WText>(theme.description);
        description->addStyleClass("text-muted small");
        
        infoLayout->addWidget(std::move(radio));
        infoLayout->addWidget(std::move(description));
        infoContainer->setLayout(std::move(infoLayout));
        
        themeLayout->addWidget(std::move(infoContainer), 1);
        themeContainer->setLayout(std::move(themeLayout));
        
        layout->addWidget(std::move(themeContainer));
    }
    
    // Buttons
    auto buttonContainer = std::make_unique<Wt::WContainerWidget>();
    buttonContainer->addStyleClass("mt-4 text-center");
    auto buttonLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto applyBtn = std::make_unique<Wt::WPushButton>("Apply Theme");
    applyBtn->addStyleClass("btn btn-primary me-2");
    
    auto cancelBtn = std::make_unique<Wt::WPushButton>("Cancel");
    cancelBtn->addStyleClass("btn btn-secondary");
    
    // Event handlers
    applyBtn->clicked().connect([this, buttonGroup, dialog]() {
        int selectedIndex = buttonGroup->checkedId();
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(availableThemes_.size())) {
            const auto& selectedTheme = availableThemes_[selectedIndex];
            applyTheme(selectedTheme.id);
            showNotification("Theme changed to " + selectedTheme.name, "success");
        }
        dialog->accept();
    });
    
    cancelBtn->clicked().connect([dialog]() {
        dialog->reject();
    });
    
    buttonLayout->addWidget(std::move(applyBtn));
    buttonLayout->addWidget(std::move(cancelBtn));
    buttonContainer->setLayout(std::move(buttonLayout));
    
    layout->addWidget(std::move(buttonContainer));
    
    dialog->contents()->setLayout(std::move(layout));
    dialog->show();
}

void RestaurantPOSApp::buildMainInterface() {
    root()->addStyleClass("container-fluid");
    
    // Add theme selector button
    auto themeSelector = std::make_unique<Wt::WContainerWidget>();
    themeSelector->addStyleClass("pos-theme-selector");
    
    auto themeBtn = std::make_unique<Wt::WPushButton>("🎨 Themes");
    themeBtn->addStyleClass("btn btn-outline-secondary btn-sm theme-button");
    themeBtn->clicked().connect([this]() { showThemeSelector(); });
    themeSelector->addWidget(std::move(themeBtn));
    root()->addWidget(std::move(themeSelector));
    
    // Add header with current theme indicator
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("pos-header");
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto title = std::make_unique<Wt::WText>("Restaurant POS System");
    title->addStyleClass("h1 text-primary");
    headerLayout->addWidget(std::move(title));
    
    auto themeIndicator = std::make_unique<Wt::WText>();
    themeIndicator->addStyleClass("h6 text-muted");
    updateThemeIndicator(themeIndicator.get());
    themeIndicator_ = themeIndicator.get();
    headerLayout->addWidget(std::move(themeIndicator));
    
    header->setLayout(std::move(headerLayout));
    root()->addWidget(std::move(header));
    
    // Create main layout
    auto mainLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Left panel: Menu and order entry (60% width)
    auto leftPanel = createOrderEntryPanel();
    mainLayout->addWidget(std::move(leftPanel), 3);
    
    // Right panel: Active orders and kitchen status (40% width)
    auto rightPanel = createOrderStatusPanel();
    mainLayout->addWidget(std::move(rightPanel), 2);
    
    auto mainContainer = std::make_unique<Wt::WContainerWidget>();
    mainContainer->setLayout(std::move(mainLayout));
    root()->addWidget(std::move(mainContainer));
}

void RestaurantPOSApp::updateThemeIndicator(Wt::WText* indicator) {
    if (indicator) {
        auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
            [this](const ThemeInfo& theme) { return theme.id == currentTheme_; });
        
        if (it != availableThemes_.end()) {
            indicator->setText("Current Theme: " + it->name);
        }
    }
}

void RestaurantPOSApp::triggerUpdate() {
    // Update theme indicator
    updateThemeIndicator(themeIndicator_);
    
    // Refresh all tables to apply new theme
    if (menuTable_) buildMenuTable();
    if (currentOrderTable_) updateCurrentOrderTable();
    if (activeOrdersTable_) updateActiveOrdersTable();
    if (kitchenStatusTable_) updateKitchenStatusTable();
}

// ... rest of the existing methods remain the same ...

void RestaurantPOSApp::initializeSampleMenu() {
    // Appetizers
    menuItems_.push_back(std::make_shared<MenuItem>(1, "Caesar Salad", 12.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(2, "Buffalo Wings", 14.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(3, "Calamari Rings", 13.99, MenuItem::APPETIZER));
    
    // Main Courses
    menuItems_.push_back(std::make_shared<MenuItem>(4, "Grilled Salmon", 24.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(5, "Ribeye Steak", 32.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(6, "Chicken Parmesan", 19.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(7, "Pasta Primavera", 16.99, MenuItem::MAIN_COURSE));
    
    // Desserts
    menuItems_.push_back(std::make_shared<MenuItem>(8, "Chocolate Cake", 8.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(9, "Tiramisu", 9.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(10, "Ice Cream Sundae", 6.99, MenuItem::DESSERT));
    
    // Beverages
    menuItems_.push_back(std::make_shared<MenuItem>(11, "House Wine", 7.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(12, "Craft Beer", 5.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(13, "Soft Drink", 2.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(14, "Fresh Juice", 4.99, MenuItem::BEVERAGE));
    
    // Daily Special
    menuItems_.push_back(std::make_shared<MenuItem>(15, "Chef's Special", 28.99, MenuItem::SPECIAL));
}

// ... include all other existing methods from the original file ...

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
