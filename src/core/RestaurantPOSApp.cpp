//============================================================================
// src/core/RestaurantPOSApp.cpp - REQUIRED Implementation
//============================================================================

#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WEnvironment.h>
#include <iostream>

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
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
    
    // Initialize component factory
    initializeComponentFactory();
    
    // Set up the main application layout
    setupMainLayout();
    
    // Create common components (header/footer)
    createCommonComponents();
    
    // Create mode-specific containers
    createModeContainers();
    
    // Set up event listeners
    setupEventListeners();
    
    // Set up real-time updates
    setupRealTimeUpdates();
    
    // Apply initial theme
    initializeThemeService();
    
    // Start in POS mode
    switchMode(POS_MODE);
    
    std::cout << "✓ RestaurantPOSApp initialized successfully" << std::endl;
}

void RestaurantPOSApp::initializeServices() {
    // Create event manager first (other services depend on it)
    eventManager_ = std::make_shared<EventManager>();
    
    // Create configuration manager
    configManager_ = std::make_shared<ConfigurationManager>();
    configManager_->initialize();
    
    // Create POS service
    posService_ = std::make_shared<POSService>(eventManager_);
    posService_->initializeMenu();
    
    // Create theme service
    themeService_ = std::make_shared<ThemeService>(this);
    
    std::cout << "✓ Core services initialized" << std::endl;
}

void RestaurantPOSApp::initializeComponentFactory() {
    componentFactory_ = std::make_unique<UIComponentFactory>(
        posService_, eventManager_, configManager_);
    
    // Register theme service with factory
    componentFactory_->registerThemeService(themeService_);
    
    std::cout << "✓ Component factory initialized" << std::endl;
}

void RestaurantPOSApp::setupMainLayout() {
    setTitle("Restaurant POS System");
    
    // Set Bootstrap theme
    setupBootstrapTheme();
    
    // Create main container
    mainContainer_ = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
    mainContainer_->setStyleClass("h-100");
    
    // Create main layout
    mainLayout_ = mainContainer_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);
    
    std::cout << "✓ Main layout initialized" << std::endl;
}

void RestaurantPOSApp::createCommonComponents() {
    // Create header with mode switching capability
    auto modeChangeCallback = [this](ModeSelector::Mode mode) {
        OperatingMode newMode = (mode == ModeSelector::POS_MODE) ? POS_MODE : KITCHEN_MODE;
        switchMode(newMode);
    };
    
    commonHeader_ = mainLayout_->addWidget(
        std::make_unique<CommonHeader>(themeService_, eventManager_, modeChangeCallback));
    
    // Create mode container (will be populated based on current mode)
    modeContainer_ = mainLayout_->addWidget(std::make_unique<Wt::WContainerWidget>(), 1);
    modeContainer_->setStyleClass("flex-grow-1");
    
    // Create footer
    commonFooter_ = mainLayout_->addWidget(
        std::make_unique<CommonFooter>(posService_, eventManager_));
    
    std::cout << "✓ Common components created" << std::endl;
}

void RestaurantPOSApp::createModeContainers() {
    // Create POS mode container
    posModeContainer_ = new POSModeContainer(posService_, eventManager_);
    posModeContainer_->hide(); // Initially hidden
    
    // Create Kitchen mode container
    kitchenModeContainer_ = new KitchenModeContainer(posService_, eventManager_);
    kitchenModeContainer_->hide(); // Initially hidden
    
    std::cout << "✓ Mode containers created" << std::endl;
}

void RestaurantPOSApp::setupEventListeners() {
    // Listen for theme changes
    if (themeService_) {
        themeService_->onThemeChanged([this](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
            onThemeChanged(oldTheme, newTheme);
        });
    }
    
    std::cout << "✓ Event listeners set up" << std::endl;
}

void RestaurantPOSApp::setupRealTimeUpdates() {
    // Create timer for periodic updates
    updateTimer_ = std::make_unique<Wt::WTimer>();
    updateTimer_->setInterval(std::chrono::seconds(5)); // Update every 5 seconds
    updateTimer_->timeout().connect(this, &RestaurantPOSApp::onPeriodicUpdate);
    updateTimer_->start();
    
    std::cout << "✓ Real-time updates enabled" << std::endl;
}

void RestaurantPOSApp::switchMode(OperatingMode mode) {
    if (mode == currentMode_) {
        return; // Already in this mode
    }
    
    OperatingMode oldMode = currentMode_;
    currentMode_ = mode;
    
    // Hide all mode containers first
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
    
    // Update header to reflect current mode
    if (commonHeader_) {
        ModeSelector::Mode headerMode = (mode == POS_MODE) ? 
            ModeSelector::POS_MODE : ModeSelector::KITCHEN_MODE;
        commonHeader_->setCurrentMode(headerMode);
    }
    
    // Apply mode-specific styling
    applyModeSpecificStyling();
    
    // Log the mode change
    logModeSwitch(mode);
    
    // Notify of mode change
    onModeChanged(mode);
}

void RestaurantPOSApp::showPOSMode() {
    modeContainer_->clear();
    modeContainer_->addWidget(std::unique_ptr<Wt::WWidget>(posModeContainer_));
    posModeContainer_->show();
    posModeContainer_->refresh();
    
    std::cout << "✓ Switched to POS Mode" << std::endl;
}

void RestaurantPOSApp::showKitchenMode() {
    modeContainer_->clear();
    modeContainer_->addWidget(std::unique_ptr<Wt::WWidget>(kitchenModeContainer_));
    kitchenModeContainer_->show();
    kitchenModeContainer_->refresh();
    
    std::cout << "✓ Switched to Kitchen Mode" << std::endl;
}

void RestaurantPOSApp::hideModeContainers() {
    if (posModeContainer_) {
        posModeContainer_->hide();
    }
    if (kitchenModeContainer_) {
        kitchenModeContainer_->hide();
    }
}

void RestaurantPOSApp::onModeChanged(OperatingMode newMode) {
    // Publish mode change event
    auto modeChangeData = POSEvents::createNotificationData(
        "Switched to " + getModeDisplayName(newMode) + " mode",
        "info"
    );
    eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED, modeChangeData);
}

void RestaurantPOSApp::initializeThemeService() {
    if (themeService_) {
        themeService_->loadThemePreference();
        themeService_->loadThemeFramework();
    }
}

void RestaurantPOSApp::setupBootstrapTheme() {
    auto theme = std::make_shared<Wt::WBootstrap5Theme>();
    setTheme(theme);
    
    // Add custom CSS
    addCustomCSS();
}

void RestaurantPOSApp::addCustomCSS() {
    // Add custom CSS for the POS system
    useStyleSheet("assets/css/pos-system.css");
    
    // Add responsive utilities
    useStyleSheet("assets/css/responsive.css");
}

void RestaurantPOSApp::applyModeSpecificStyling() {
    // Apply different styling based on current mode
    if (mainContainer_) {
        mainContainer_->removeStyleClass("pos-mode kitchen-mode");
        
        if (currentMode_ == POS_MODE) {
            mainContainer_->addStyleClass("pos-mode");
        } else {
            mainContainer_->addStyleClass("kitchen-mode");
        }
    }
}

void RestaurantPOSApp::onPeriodicUpdate() {
    // Update footer status
    if (commonFooter_) {
        commonFooter_->updateStatus();
    }
    
    // Refresh current mode container
    if (currentMode_ == POS_MODE && posModeContainer_) {
        posModeContainer_->refresh();
    } else if (currentMode_ == KITCHEN_MODE && kitchenModeContainer_) {
        kitchenModeContainer_->refresh();
    }
}

void RestaurantPOSApp::onThemeChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
    // Publish theme change event
    auto themeEventData = POSEvents::createThemeChangedData(
        themeService_->getThemeCSSClass(newTheme),
        themeService_->getThemeName(newTheme),
        themeService_->getThemeCSSClass(oldTheme)
    );
    eventManager_->publish(POSEvents::THEME_CHANGED, themeEventData);
    
    std::cout << "✓ Theme changed from " << themeService_->getThemeName(oldTheme) 
              << " to " << themeService_->getThemeName(newTheme) << std::endl;
}

void RestaurantPOSApp::logApplicationStart() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "🍽️  RESTAURANT POS SYSTEM STARTING" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void RestaurantPOSApp::logModeSwitch(OperatingMode mode) {
    std::cout << "🔄 Mode Switch: " << getModeDisplayName(mode) << std::endl;
}

std::string RestaurantPOSApp::getModeDisplayName(OperatingMode mode) const {
    switch (mode) {
        case POS_MODE:     return "Point of Sale";
        case KITCHEN_MODE: return "Kitchen Display";
        default:          return "Unknown";
    }
}

//============================================================================
// REQUIRED: Application Factory Function
//============================================================================

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
