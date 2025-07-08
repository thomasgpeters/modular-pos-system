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
    , isDestroying_(false)  // ADD THIS
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

// Add proper destructor:
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
        
        // CRITICAL: Clear event subscriptions if any exist
        // (This prevents callbacks to destroyed objects)
        
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

// 2. FIXED createModeContainers() - PROPER INITIAL SETUP
// ============================================================================

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
        
        // FIXED: Create containers as children (they stay in the container)
        posModeContainer_ = modeContainer_->addNew<POSModeContainer>(posService_, eventManager_);
        if (!posModeContainer_) {
            throw std::runtime_error("Failed to create POS mode container");
        }
        
        std::cout << "[RestaurantPOSApp] Creating Kitchen mode container..." << std::endl;
        
        kitchenModeContainer_ = modeContainer_->addNew<KitchenModeContainer>(posService_, eventManager_);
        if (!kitchenModeContainer_) {
            throw std::runtime_error("Failed to create Kitchen mode container");
        }
        
        // FIXED: Initially hide both containers instead of removing them
        posModeContainer_->hide();
        kitchenModeContainer_->hide();
        
        std::cout << "[RestaurantPOSApp] ✓ Mode containers created successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[RestaurantPOSApp] CRITICAL ERROR creating mode containers: " << e.what() << std::endl;
        throw;
    }
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

// ============================================================================
// CRITICAL FIX FOR EXC_BAD_ACCESS in widgetAdded()
// ============================================================================

// The crash happens because widgets are being deleted and then accessed.
// Root cause: Using clear() instead of removeWidget() when switching modes.

// 1. FIXED RestaurantPOSApp.cpp - SAFE MODE SWITCHING
// ============================================================================

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
        
        // FIXED: Use show/hide instead of add/remove for modern Wt
        // This avoids the unique_ptr issues while being safe
        
        // Hide both containers
        posModeContainer_->hide();
        kitchenModeContainer_->hide();
        
        // Update current mode
        currentMode_ = mode;
        
        // Show the appropriate container
        if (mode == POS_MODE) {
            std::cout << "[RestaurantPOSApp] Showing POS mode container" << std::endl;
            posModeContainer_->show();
            
            // Refresh the container
            try {
                posModeContainer_->refresh();
            } catch (const std::exception& e) {
                std::cerr << "[RestaurantPOSApp] Error refreshing POS container: " << e.what() << std::endl;
            }
            
        } else if (mode == KITCHEN_MODE) {
            std::cout << "[RestaurantPOSApp] Showing Kitchen mode container" << std::endl;
            kitchenModeContainer_->show();
            
            // Refresh the container
            try {
                kitchenModeContainer_->refresh();
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
        
        // Trigger callbacks safely
        try {
            onModeChanged(mode);
        } catch (const std::exception& e) {
            std::cerr << "[RestaurantPOSApp] ERROR in mode change callback: " << e.what() << std::endl;
        }
        
        logModeSwitch(mode);
        std::cout << "[RestaurantPOSApp] ✓ Successfully switched to " << getModeDisplayName(mode) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[RestaurantPOSApp] CRITICAL ERROR switching modes: " << e.what() << std::endl;
        
        // Emergency recovery
        if (posModeContainer_) {
            kitchenModeContainer_->hide();
            posModeContainer_->show();
            currentMode_ = POS_MODE;
        }
    }
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
    // Check if we're being destroyed
    if (isDestroying_) {
        return;
    }
    
    // Log the mode change
    std::cout << "🔄 Mode Switch: " << getModeDisplayName(newMode) << std::endl;
    
    // Apply any mode-specific styling or behavior here
    if (newMode == POS_MODE) {
        std::cout << "🔄 Mode Switch: Point of Sale" << std::endl;
    } else if (newMode == KITCHEN_MODE) {
        std::cout << "🔄 Mode Switch: Kitchen Display" << std::endl;
    }
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

// 5. DEBUGGING HELPER - Add this method for troubleshooting
// ============================================================================
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
    }
    
    std::cout << "  - currentMode_: " << static_cast<int>(currentMode_) << std::endl;
    std::cout << "  - isDestroying_: " << (isDestroying_ ? "TRUE" : "FALSE") << std::endl;
}

//============================================================================
// REQUIRED: Application Factory Function
//============================================================================

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
