//============================================================================
// RestaurantPOSApp Enhanced Implementation
//============================================================================

#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WText.h>
#include <iostream>

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env)
    : WApplication(env)
    , currentMode_(POS_MODE)
    , mainContainer_(nullptr)
    , mainLayout_(nullptr)
    , commonHeader_(nullptr)
    , commonFooter_(nullptr)
    , modeContainer_(nullptr)
    , posModeContainer_(nullptr)
    , kitchenModeContainer_(nullptr)
{
    logApplicationStart();
    
    // Initialize services first
    initializeServices();
    initializeComponentFactory();
    
    // Setup theme and styling
    initializeThemeService();
    setupBootstrapTheme();
    
    // Setup main application layout
    setupMainLayout();
    createCommonComponents();
    createModeContainers();
    
    // Setup event handling and real-time updates
    setupEventListeners();
    setupRealTimeUpdates();
    
    // Start in POS mode
    switchMode(POS_MODE);
    
    addCustomCSS();
}

void RestaurantPOSApp::initializeServices() {
    eventManager_ = std::make_shared<EventManager>();
    posService_ = std::make_shared<POSService>(eventManager_);
    configManager_ = std::make_shared<ConfigurationManager>();
    
    // Initialize POS service menu
    posService_->initializeMenu();
    
    std::cout << "✓ Core services initialized" << std::endl;
}

void RestaurantPOSApp::initializeComponentFactory() {
    componentFactory_ = std::make_unique<UIComponentFactory>(
        posService_, eventManager_, configManager_);
    
    std::cout << "✓ Component factory initialized" << std::endl;
}

void RestaurantPOSApp::initializeThemeService() {
    themeService_ = std::make_shared<ThemeService>(this);
    componentFactory_->registerThemeService(themeService_);
    
    std::cout << "✓ Theme service initialized" << std::endl;
}

void RestaurantPOSApp::setupBootstrapTheme() {
    auto theme = std::make_shared<Wt::WBootstrap5Theme>();
    setTheme(theme);
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/bootstrap/5.3.0/css/bootstrap.min.css");
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css");
}

void RestaurantPOSApp::setupMainLayout() {
    setTitle("Restaurant POS System - Enhanced Mode Switching");
    
    // Create main container
    mainContainer_ = root();
    mainContainer_->addStyleClass("d-flex flex-column min-vh-100");
    
    // Create main layout
    mainLayout_ = mainContainer_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    
    std::cout << "✓ Main layout configured" << std::endl;
}

void RestaurantPOSApp::createCommonComponents() {
    // Create common header
    commonHeader_ = mainLayout_->addWidget(
        std::make_unique<CommonHeader>(
            themeService_, 
            eventManager_,
            [this](ModeSelector::Mode mode) {
                OperatingMode appMode = (mode == ModeSelector::POS_MODE) ? 
                    POS_MODE : KITCHEN_MODE;
                switchMode(appMode);
            }
        )
    );
    commonHeader_->addStyleClass("bg-primary text-white shadow-sm");
    
    // Create mode container (will hold mode-specific content)
    modeContainer_ = mainLayout_->addWidget(std::make_unique<Wt::WContainerWidget>());
    modeContainer_->addStyleClass("flex-grow-1 d-flex");
    modeContainer_->setId("mode-container");
    
    // Create common footer
    commonFooter_ = mainLayout_->addWidget(
        std::make_unique<CommonFooter>(posService_, eventManager_)
    );
    commonFooter_->addStyleClass("bg-light border-top mt-auto");
    
    std::cout << "✓ Common components created" << std::endl;
}

void RestaurantPOSApp::createModeContainers() {
    // Create POS mode container
    posModeContainer_ = modeContainer_->addWidget(
        std::make_unique<POSModeContainer>(posService_, eventManager_)
    );
    posModeContainer_->addStyleClass("w-100");
    posModeContainer_->hide(); // Initially hidden
    
    // Create Kitchen mode container
    kitchenModeContainer_ = modeContainer_->addWidget(
        std::make_unique<KitchenModeContainer>(posService_, eventManager_)
    );
    kitchenModeContainer_->addStyleClass("w-100");
    kitchenModeContainer_->hide(); // Initially hidden
    
    std::cout << "✓ Mode containers created" << std::endl;
}

void RestaurantPOSApp::switchMode(OperatingMode mode) {
    if (currentMode_ == mode) {
        return; // Already in this mode
    }
    
    OperatingMode oldMode = currentMode_;
    currentMode_ = mode;
    
    // Hide all mode containers
    hideModeContainers();
    
    // Show the appropriate mode container
    switch (mode) {
        case POS_MODE:
            showPOSMode();
            break;
        case KITCHEN_MODE:
            showKitchenMode();
            break;
    }
    
    // Update header mode selector
    ModeSelector::Mode selectorMode = (mode == POS_MODE) ? 
        ModeSelector::POS_MODE : ModeSelector::KITCHEN_MODE;
    commonHeader_->setCurrentMode(selectorMode);
    
    // Apply mode-specific styling
    applyModeSpecificStyling();
    
    // Notify mode change
    onModeChanged(mode);
    
    logModeSwitch(mode);
}

void RestaurantPOSApp::hideModeContainers() {
    posModeContainer_->hide();
    kitchenModeContainer_->hide();
}

void RestaurantPOSApp::showPOSMode() {
    posModeContainer_->show();
    posModeContainer_->refresh();
    
    // Focus on order taking workflow
    std::cout << "Switched to POS Mode - Order Taking" << std::endl;
}

void RestaurantPOSApp::showKitchenMode() {
    kitchenModeContainer_->show();
    kitchenModeContainer_->refresh();
    
    // Focus on kitchen workflow
    std::cout << "Switched to Kitchen Mode - Order Preparation" << std::endl;
}

void RestaurantPOSApp::applyModeSpecificStyling() {
    // Remove previous mode classes
    modeContainer_->removeStyleClass("pos-mode");
    modeContainer_->removeStyleClass("kitchen-mode");
    
    // Add current mode class
    switch (currentMode_) {
        case POS_MODE:
            modeContainer_->addStyleClass("pos-mode");
            break;
        case KITCHEN_MODE:
            modeContainer_->addStyleClass("kitchen-mode");
            break;
    }
}

void RestaurantPOSApp::onModeChanged(OperatingMode newMode) {
    // Publish mode change event
    eventManager_->publish("MODE_CHANGED", std::to_string(static_cast<int>(newMode)));
    
    // Update footer status
    commonFooter_->updateStatus();
}

void RestaurantPOSApp::setupEventListeners() {
    // Listen for theme changes
    if (themeService_) {
        themeService_->onThemeChanged([this](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
            onThemeChanged(oldTheme, newTheme);
        });
    }
    
    std::cout << "✓ Event listeners configured" << std::endl;
}

void RestaurantPOSApp::setupRealTimeUpdates() {
    updateTimer_ = std::make_unique<Wt::WTimer>();
    updateTimer_->timeout().connect([this]() {
        onPeriodicUpdate();
    });
    updateTimer_->setInterval(std::chrono::seconds(5));
    updateTimer_->start();
    
    std::cout << "✓ Real-time updates configured" << std::endl;
}

void RestaurantPOSApp::onPeriodicUpdate() {
    // Update current mode container
    switch (currentMode_) {
        case POS_MODE:
            if (posModeContainer_->isVisible()) {
                posModeContainer_->refresh();
            }
            break;
        case KITCHEN_MODE:
            if (kitchenModeContainer_->isVisible()) {
                kitchenModeContainer_->refresh();
            }
            break;
    }
    
    // Update footer
    commonFooter_->updateStatus();
}

void RestaurantPOSApp::onThemeChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
    std::cout << "Theme changed from " << static_cast<int>(oldTheme) 
              << " to " << static_cast<int>(newTheme) << std::endl;
    
    // Apply theme-specific styling if needed
    applyModeSpecificStyling();
}

void RestaurantPOSApp::addCustomCSS() {
    // Mode-specific styles
    styleSheet().addRule(".pos-mode", "background-color: #f8f9fa;");
    styleSheet().addRule(".kitchen-mode", "background-color: #fff3cd;");
    
    // POS Mode styles
    styleSheet().addRule(".pos-mode .order-work-area", 
        "border-left: 3px solid #007bff; padding-left: 1rem;");
    
    // Kitchen Mode styles  
    styleSheet().addRule(".kitchen-mode .kitchen-status", 
        "border-left: 3px solid #fd7e14; padding-left: 1rem;");
    
    // Common header styles
    styleSheet().addRule(".common-header", 
        "box-shadow: 0 2px 4px rgba(0,0,0,0.1);");
    
    // Mode selector styles
    styleSheet().addRule(".mode-selector .btn.active", 
        "background-color: #ffffff; color: #007bff; font-weight: bold;");
    
    std::cout << "✓ Custom CSS applied" << std::endl;
}

void RestaurantPOSApp::logApplicationStart() {
    std::cout << "\n=== Restaurant POS System Enhanced ===\n";
    std::cout << "Version: 3.0.0 - Mode Switching\n";
    std::cout << "Starting application...\n\n";
}

void RestaurantPOSApp::logModeSwitch(OperatingMode mode) {
    std::cout << "→ Mode switched to: " << getModeDisplayName(mode) << std::endl;
}

std::string RestaurantPOSApp::getModeDisplayName(OperatingMode mode) const {
    switch (mode) {
        case POS_MODE:     return "POS Mode (Order Taking)";
        case KITCHEN_MODE: return "Kitchen Mode (Order Preparation)";
        default:           return "Unknown Mode";
    }
}
