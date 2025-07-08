//============================================================================
// src/core/RestaurantPOSApp.cpp - Enhanced to ensure POS mode loads by default
//============================================================================

#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WEnvironment.h>
#include <iostream>

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , currentMode_(POS_MODE)  // Start with POS mode as default
    , isDestroying_(false)
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
    
    // ENHANCED: Ensure POS mode is loaded and visible by default
    ensurePOSModeDefault();
    
    std::cout << "✓ RestaurantPOSApp initialized successfully in POS mode" << std::endl;
}

RestaurantPOSApp::~RestaurantPOSApp() {
    std::cout << "[RestaurantPOSApp] Destructor called" << std::endl;
    
    // Set flag to prevent further operations
    isDestroying_ = true;
    
    try {
        // CRITICAL: Stop the update timer first
        if (updateTimer_) {
            updateTimer_->stop();
            updateTimer_.reset();
        }
        
        // CRITICAL: Remove widgets from containers before destruction
        if (modeContainer_) {
            if (posModeContainer_) {
                modeContainer_->removeWidget(posModeContainer_);
            }
            if (kitchenModeContainer_) {
                modeContainer_->removeWidget(kitchenModeContainer_);
            }
        }
        
        std::cout << "[RestaurantPOSApp] Cleanup completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[RestaurantPOSApp] Error during destruction: " << e.what() << std::endl;
    }
}

void RestaurantPOSApp::createModeContainers() {
    std::cout << "[RestaurantPOSApp] Creating mode containers..." << std::endl;
    
    try {
        // Ensure services exist
        if (!posService_ || !eventManager_) {
            throw std::runtime_error("Services not initialized before creating mode containers");
        }
        
        // Ensure mode container exists
        if (!modeContainer_) {
            throw std::runtime_error("Mode container not initialized");
        }
        
        std::cout << "[RestaurantPOSApp] Creating POS mode container..." << std::endl;
        
        // Create POS mode container as child
        posModeContainer_ = modeContainer_->addNew<POSModeContainer>(posService_, eventManager_);
        if (!posModeContainer_) {
            throw std::runtime_error("Failed to create POS mode container");
        }
        
        // Add identification for debugging
        posModeContainer_->setId("pos-mode-container");
        posModeContainer_->addStyleClass("mode-container pos-mode-active");
        
        std::cout << "[RestaurantPOSApp] Creating Kitchen mode container..." << std::endl;
        
        kitchenModeContainer_ = modeContainer_->addNew<KitchenModeContainer>(posService_, eventManager_);
        if (!kitchenModeContainer_) {
            throw std::runtime_error("Failed to create Kitchen mode container");
        }
        
        // Add identification for debugging
        kitchenModeContainer_->setId("kitchen-mode-container");
        kitchenModeContainer_->addStyleClass("mode-container kitchen-mode-inactive");
        
        // ENHANCED: Set initial visibility state clearly
        // POS mode should be visible by default, Kitchen mode hidden
        posModeContainer_->show();  // Make POS visible from start
        kitchenModeContainer_->hide();  // Hide kitchen mode
        
        std::cout << "[RestaurantPOSApp] ✓ Mode containers created successfully" << std::endl;
        std::cout << "[RestaurantPOSApp] ✓ POS mode container is visible by default" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[RestaurantPOSApp] CRITICAL ERROR creating mode containers: " << e.what() << std::endl;
        throw;
    }
}

// NEW METHOD: Ensure POS mode is properly loaded and visible
void RestaurantPOSApp::ensurePOSModeDefault() {
    std::cout << "[RestaurantPOSApp] Ensuring POS mode is loaded by default..." << std::endl;
    
    try {
        // Verify containers exist
        if (!posModeContainer_ || !kitchenModeContainer_) {
            std::cerr << "[RestaurantPOSApp] ERROR: Mode containers not created!" << std::endl;
            return;
        }
        
        // Force POS mode to be the active mode
        currentMode_ = POS_MODE;
        
        // Ensure visibility states are correct
        posModeContainer_->show();
        kitchenModeContainer_->hide();
        
        // Update header to reflect POS mode
        if (commonHeader_) {
            commonHeader_->setCurrentMode(ModeSelector::POS_MODE);
        }
        
        // Refresh POS mode container to ensure it loads content
        if (posModeContainer_) {
            posModeContainer_->refresh();
        }
        
        // Add visual confirmation
        if (modeContainer_) {
            modeContainer_->addStyleClass("pos-mode-active");
            modeContainer_->removeStyleClass("kitchen-mode-active");
        }
        
        std::cout << "[RestaurantPOSApp] ✓ POS mode is now active and visible" << std::endl;
        
        // Optional: Add a welcome message to confirm POS mode is loaded
        if (eventManager_) {
            auto welcomeEvent = POSEvents::createNotificationData(
                "Welcome to Restaurant POS System - POS Mode Ready",
                "success",
                3000
            );
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED, welcomeEvent);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[RestaurantPOSApp] ERROR ensuring POS mode default: " << e.what() << std::endl;
    }
}

void RestaurantPOSApp::switchMode(OperatingMode mode) {
    std::cout << "[RestaurantPOSApp] Switching to mode: " << getModeDisplayName(mode) << std::endl;
    
    try {
        // Don't switch if already in the requested mode
        if (currentMode_ == mode) {
            std::cout << "[RestaurantPOSApp] Already in " << getModeDisplayName(mode) << ", skipping switch" << std::endl;
            return;
        }
        
        // Validate containers exist
        if (!modeContainer_) {
            std::cerr << "[RestaurantPOSApp] FATAL: modeContainer_ is null!" << std::endl;
            return;
        }
        
        if (!posModeContainer_ || !kitchenModeContainer_) {
            std::cerr << "[RestaurantPOSApp] FATAL: Mode containers are null!" << std::endl;
            return;
        }
        
        // ENHANCED: Clear previous mode styling
        modeContainer_->removeStyleClass("pos-mode-active kitchen-mode-active");
        posModeContainer_->removeStyleClass("pos-mode-active pos-mode-inactive");
        kitchenModeContainer_->removeStyleClass("kitchen-mode-active kitchen-mode-inactive");
        
        // Hide both containers first
        posModeContainer_->hide();
        kitchenModeContainer_->hide();
        
        // Update current mode
        currentMode_ = mode;
        
        // Show the appropriate container and apply styling
        if (mode == POS_MODE) {
            std::cout << "[RestaurantPOSApp] Activating POS mode container" << std::endl;
            
            // Show and style POS container
            posModeContainer_->show();
            posModeContainer_->addStyleClass("pos-mode-active");
            kitchenModeContainer_->addStyleClass("kitchen-mode-inactive");
            modeContainer_->addStyleClass("pos-mode-active");
            
            // Refresh the container
            try {
                posModeContainer_->refresh();
                std::cout << "[RestaurantPOSApp] ✓ POS mode container refreshed" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[RestaurantPOSApp] Error refreshing POS container: " << e.what() << std::endl;
            }
            
        } else if (mode == KITCHEN_MODE) {
            std::cout << "[RestaurantPOSApp] Activating Kitchen mode container" << std::endl;
            
            // Show and style Kitchen container
            kitchenModeContainer_->show();
            kitchenModeContainer_->addStyleClass("kitchen-mode-active");
            posModeContainer_->addStyleClass("pos-mode-inactive");
            modeContainer_->addStyleClass("kitchen-mode-active");
            
            // Refresh the container
            try {
                kitchenModeContainer_->refresh();
                std::cout << "[RestaurantPOSApp] ✓ Kitchen mode container refreshed" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[RestaurantPOSApp] Error refreshing Kitchen container: " << e.what() << std::endl;
            }
        }
        
        // Update header
        if (commonHeader_) {
            try {
                commonHeader_->setCurrentMode(mode == POS_MODE ? ModeSelector::POS_MODE : ModeSelector::KITCHEN_MODE);
            } catch (const std::exception& e) {
                std::cerr << "[RestaurantPOSApp] ERROR updating header: " << e.what() << std::endl;
            }
        }
        
        // Apply mode-specific styling to main container
        applyModeSpecificStyling();
        
        // Trigger callbacks safely
        try {
            onModeChanged(mode);
        } catch (const std::exception& e) {
            std::cerr << "[RestaurantPOSApp] ERROR in mode change callback: " << e.what() << std::endl;
        }
        
        // Log success
        logModeSwitch(mode);
        std::cout << "[RestaurantPOSApp] ✓ Successfully switched to " << getModeDisplayName(mode) << std::endl;
        
        // Show confirmation notification
        if (eventManager_) {
            auto modeChangeEvent = POSEvents::createNotificationData(
                "Switched to " + getModeDisplayName(mode) + " mode",
                "info",
                2000
            );
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED, modeChangeEvent);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[RestaurantPOSApp] CRITICAL ERROR switching modes: " << e.what() << std::endl;
        
        // Emergency recovery to POS mode
        if (posModeContainer_) {
            kitchenModeContainer_->hide();
            posModeContainer_->show();
            currentMode_ = POS_MODE;
            std::cout << "[RestaurantPOSApp] Emergency recovery: Restored to POS mode" << std::endl;
        }
    }
}

void RestaurantPOSApp::applyModeSpecificStyling() {
    // Apply different styling based on current mode
    if (mainContainer_) {
        mainContainer_->removeStyleClass("pos-mode kitchen-mode");
        
        if (currentMode_ == POS_MODE) {
            mainContainer_->addStyleClass("pos-mode");
            setTitle("Restaurant POS System - Point of Sale");
        } else {
            mainContainer_->addStyleClass("kitchen-mode");
            setTitle("Restaurant POS System - Kitchen Display");
        }
    }
}

void RestaurantPOSApp::onModeChanged(OperatingMode newMode) {
    // Check if we're being destroyed
    if (isDestroying_) {
        return;
    }
    
    // Log the mode change with enhanced detail
    std::cout << "🔄 Mode Change Complete: " << getModeDisplayName(newMode) << std::endl;
    
    // Apply mode-specific behavior
    if (newMode == POS_MODE) {
        std::cout << "📋 POS Mode: Ready for order taking and management" << std::endl;
        
        // Ensure POS components are ready
        if (posModeContainer_) {
            // Any POS-specific initialization can go here
        }
        
    } else if (newMode == KITCHEN_MODE) {
        std::cout << "👨‍🍳 Kitchen Mode: Ready for order preparation tracking" << std::endl;
        
        // Ensure Kitchen components are ready
        if (kitchenModeContainer_) {
            // Any Kitchen-specific initialization can go here
        }
    }
}

// ... (rest of the implementation remains the same)

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
    setTitle("Restaurant POS System - Point of Sale");  // Default title shows POS mode
    
    // Set Bootstrap theme
    setupBootstrapTheme();
    
    // Create main container
    mainContainer_ = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
    mainContainer_->setStyleClass("h-100 pos-app-container");
    
    // Create main layout
    mainLayout_ = mainContainer_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);
    
    std::cout << "✓ Main layout initialized for POS mode" << std::endl;
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
    modeContainer_->setStyleClass("flex-grow-1 mode-container-wrapper");
    modeContainer_->setId("main-mode-container");
    
    // Create footer
    commonFooter_ = mainLayout_->addWidget(
        std::make_unique<CommonFooter>(posService_, eventManager_));
    
    std::cout << "✓ Common components created" << std::endl;
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
    
    // Add mode-specific CSS
    useStyleSheet("assets/css/mode-styles.css");
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
    std::cout << "📋 Default Mode: Point of Sale (POS)" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void RestaurantPOSApp::logModeSwitch(OperatingMode mode) {
    std::cout << "🔄 Mode Switch Complete: " << getModeDisplayName(mode) << std::endl;
}

std::string RestaurantPOSApp::getModeDisplayName(OperatingMode mode) const {
    switch (mode) {
        case POS_MODE:     return "Point of Sale";
        case KITCHEN_MODE: return "Kitchen Display";
        default:          return "Unknown";
    }
}

void RestaurantPOSApp::debugWidgetState() {
    std::cout << "[DEBUG] Widget State Check:" << std::endl;
    std::cout << "  - modeContainer_: " << (modeContainer_ ? "VALID" : "NULL") << std::endl;
    std::cout << "  - posModeContainer_: " << (posModeContainer_ ? "VALID" : "NULL") << std::endl;
    std::cout << "  - kitchenModeContainer_: " << (kitchenModeContainer_ ? "VALID" : "NULL") << std::endl;
    
    if (modeContainer_) {
        std::cout << "  - modeContainer children: " << modeContainer_->children().size() << std::endl;
        
        // Check if our containers are in the mode container
        int posIndex = modeContainer_->indexOf(posModeContainer_);
        int kitchenIndex = modeContainer_->indexOf(kitchenModeContainer_);
        
        std::cout << "  - POS container index: " << posIndex << std::endl;
        std::cout << "  - Kitchen container index: " << kitchenIndex << std::endl;
        
        if (posModeContainer_) {
            std::cout << "  - POS container visible: " << (posModeContainer_->isVisible() ? "YES" : "NO") << std::endl;
        }
        if (kitchenModeContainer_) {
            std::cout << "  - Kitchen container visible: " << (kitchenModeContainer_->isVisible() ? "YES" : "NO") << std::endl;
        }
    }
    
    std::cout << "  - currentMode_: " << static_cast<int>(currentMode_) << " (" << getModeDisplayName(currentMode_) << ")" << std::endl;
    std::cout << "  - isDestroying_: " << (isDestroying_ ? "TRUE" : "FALSE") << std::endl;
}

// Cleanup methods remain the same...
void RestaurantPOSApp::showPOSMode() {
    // This method is kept for compatibility but the main switching is handled by switchMode()
    switchMode(POS_MODE);
}

void RestaurantPOSApp::showKitchenMode() {
    // This method is kept for compatibility but the main switching is handled by switchMode()
    switchMode(KITCHEN_MODE);
}

void RestaurantPOSApp::hideModeContainers() {
    if (posModeContainer_) {
        posModeContainer_->hide();
    }
    if (kitchenModeContainer_) {
        kitchenModeContainer_->hide();
    }
}

//============================================================================
// REQUIRED: Application Factory Function
//============================================================================

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
