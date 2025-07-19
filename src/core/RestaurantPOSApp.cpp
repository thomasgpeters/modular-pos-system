//============================================================================
// src/core/RestaurantPOSApp.cpp - Updated to use APIServiceFactory with Logger Integration
//============================================================================

#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WEnvironment.h>
#include <iostream>
#include <sstream>

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , logger_(Logger::getInstance())  // FIXED: Initialize reference in initialization list
    , isDestroying_(false)      // Initialize in declaration order
    , currentMode_(POS_MODE)    // Start with POS mode as default
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
    
    logger_.info("✓ RestaurantPOSApp initialized successfully in POS mode");
}

void RestaurantPOSApp::initializeServices() {
    logger_.info("[RestaurantPOSApp] Initializing services...");
    
    // Create event manager first (other services depend on it)
    eventManager_ = std::make_shared<EventManager>();
    
    // Create configuration manager and initialize with API settings
    configManager_ = std::make_shared<ConfigurationManager>();
    configManager_->initialize();
    
    // ENABLE API for testing (you can also set this via config file or environment)
    configManager_->setValue<bool>("api.enabled", true);
    configManager_->setValue<std::string>("api.base_url", "http://localhost:5656/api");
    configManager_->setValue<std::string>("api.auth_token", "");  // Add token if needed
    configManager_->setValue<int>("api.timeout", 30);
    configManager_->setValue<bool>("api.enable_caching", true);
    configManager_->setValue<bool>("api.debug_mode", true);  // Enable debug for testing
    
    logger_.info("[RestaurantPOSApp] API configuration:");
    
    // CLEANED UP: Using LoggingUtils for cleaner boolean logging
    LOG_CONFIG_BOOL(logger_, info, "API Enabled", configManager_->getValue<bool>("api.enabled", false));
    LOG_CONFIG_STRING(logger_, info, "API Base URL", configManager_->getValue<std::string>("api.base_url", "not set"));
    LOG_CONFIG_BOOL(logger_, info, "Debug Mode", configManager_->getValue<bool>("api.debug_mode", false));
    
    // FIXED: Use APIServiceFactory to create appropriate POS service
    posService_ = APIServiceFactory::createPOSService(eventManager_, configManager_);
    
    if (!posService_) {
        LOG_COMPONENT_ERROR(logger_, "RestaurantPOSApp", "service creation", "Failed to create POS service");
        throw std::runtime_error("Failed to create POS service");
    }
    
    // Check if we got an enhanced service
    auto enhancedService = std::dynamic_pointer_cast<EnhancedPOSService>(posService_);
    if (enhancedService) {
        LOG_CONFIG_BOOL(logger_, info, "EnhancedPOSService connected", enhancedService->isConnected());
        logger_.info("[RestaurantPOSApp] ✓ EnhancedPOSService created successfully");
    } else {
        logger_.info("[RestaurantPOSApp] ✓ Standard POSService created (local data)");
    }
    
    // Initialize menu
    posService_->initializeMenu();
    
    // Create theme service
    themeService_ = std::make_shared<ThemeService>(this);
    
    LOG_OPERATION_STATUS(logger_, "Core services initialization", true);
}

// Rest of the implementation with logger integration
RestaurantPOSApp::~RestaurantPOSApp() {
    logger_.info("[RestaurantPOSApp] Destructor called");
    
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
        
        logger_.info("[RestaurantPOSApp] Cleanup completed");
        
    } catch (const std::exception& e) {
        logger_.error(std::string("[RestaurantPOSApp] Error during destruction: ") + e.what());
    }
}

void RestaurantPOSApp::createModeContainers() {
    logger_.info("[RestaurantPOSApp] Creating mode containers...");
    
    try {
        // Ensure services exist
        if (!posService_ || !eventManager_) {
            throw std::runtime_error("Services not initialized before creating mode containers");
        }
        
        // Ensure mode container exists
        if (!modeContainer_) {
            throw std::runtime_error("Mode container not initialized");
        }
        
        logger_.info("[RestaurantPOSApp] Creating POS mode container...");
        
        // Create POS mode container as child
        posModeContainer_ = modeContainer_->addNew<POSModeContainer>(posService_, eventManager_);
        if (!posModeContainer_) {
            throw std::runtime_error("Failed to create POS mode container");
        }
        
        // Add identification for debugging
        posModeContainer_->setId("pos-mode-container");
        posModeContainer_->addStyleClass("mode-container pos-mode-active");
        
        logger_.info("[RestaurantPOSApp] Creating Kitchen mode container...");
        
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
        
        logger_.info("[RestaurantPOSApp] ✓ Mode containers created successfully");
        logger_.info("[RestaurantPOSApp] ✓ POS mode container is visible by default");
        
    } catch (const std::exception& e) {
        logger_.error(std::string("[RestaurantPOSApp] CRITICAL ERROR creating mode containers: ") + e.what());
        throw;
    }
}

// NEW METHOD: Ensure POS mode is properly loaded and visible
void RestaurantPOSApp::ensurePOSModeDefault() {
    logger_.info("[RestaurantPOSApp] Ensuring POS mode is loaded by default...");
    
    try {
        // Verify containers exist
        if (!posModeContainer_ || !kitchenModeContainer_) {
            logger_.error("[RestaurantPOSApp] ERROR: Mode containers not created!");
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
        
        logger_.info("[RestaurantPOSApp] ✓ POS mode is now active and visible");
        
        // Optional: Add a welcome message to confirm POS mode is loaded
        if (eventManager_) {
            auto welcomeEvent = POSEvents::createNotificationData(
                "Welcome to Restaurant POS System - API Integration Ready",
                "success",
                3000
            );
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED, welcomeEvent);
        }
        
    } catch (const std::exception& e) {
        logger_.error(std::string("[RestaurantPOSApp] ERROR ensuring POS mode default: ") + e.what());
    }
}

void RestaurantPOSApp::switchMode(OperatingMode mode) {
    std::ostringstream modeMsg;
    modeMsg << "[RestaurantPOSApp] Switching to mode: " << getModeDisplayName(mode);
    logger_.info(modeMsg.str());
    
    try {
        // Don't switch if already in the requested mode
        if (currentMode_ == mode) {
            std::ostringstream skipMsg;
            skipMsg << "[RestaurantPOSApp] Already in " << getModeDisplayName(mode) << ", skipping switch";
            logger_.info(skipMsg.str());
            return;
        }
        
        // Validate containers exist
        if (!modeContainer_) {
            logger_.error("[RestaurantPOSApp] FATAL: modeContainer_ is null!");
            return;
        }
        
        if (!posModeContainer_ || !kitchenModeContainer_) {
            logger_.error("[RestaurantPOSApp] FATAL: Mode containers are null!");
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
            logger_.info("[RestaurantPOSApp] Activating POS mode container");
            
            // Show and style POS container
            posModeContainer_->show();
            posModeContainer_->addStyleClass("pos-mode-active");
            kitchenModeContainer_->addStyleClass("kitchen-mode-inactive");
            modeContainer_->addStyleClass("pos-mode-active");
            
            // Refresh the container
            try {
                posModeContainer_->refresh();
                logger_.info("[RestaurantPOSApp] ✓ POS mode container refreshed");
            } catch (const std::exception& e) {
                logger_.error(std::string("[RestaurantPOSApp] Error refreshing POS container: ") + e.what());
            }
            
        } else if (mode == KITCHEN_MODE) {
            logger_.info("[RestaurantPOSApp] Activating Kitchen mode container");
            
            // Show and style Kitchen container
            kitchenModeContainer_->show();
            kitchenModeContainer_->addStyleClass("kitchen-mode-active");
            posModeContainer_->addStyleClass("pos-mode-inactive");
            modeContainer_->addStyleClass("kitchen-mode-active");
            
            // Refresh the container
            try {
                kitchenModeContainer_->refresh();
                logger_.info("[RestaurantPOSApp] ✓ Kitchen mode container refreshed");
            } catch (const std::exception& e) {
                logger_.error(std::string("[RestaurantPOSApp] Error refreshing Kitchen container: ") + e.what());
            }
        }
        
        // Update header
        if (commonHeader_) {
            try {
                commonHeader_->setCurrentMode(mode == POS_MODE ? ModeSelector::POS_MODE : ModeSelector::KITCHEN_MODE);
            } catch (const std::exception& e) {
                logger_.error(std::string("[RestaurantPOSApp] ERROR updating header: ") + e.what());
            }
        }
        
        // Apply mode-specific styling to main container
        applyModeSpecificStyling();
        
        // Trigger callbacks safely
        try {
            onModeChanged(mode);
        } catch (const std::exception& e) {
            logger_.error(std::string("[RestaurantPOSApp] ERROR in mode change callback: ") + e.what());
        }
        
        // Log success
        logModeSwitch(mode);
        std::ostringstream successMsg;
        successMsg << "[RestaurantPOSApp] ✓ Successfully switched to " << getModeDisplayName(mode);
        logger_.info(successMsg.str());
        
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
        logger_.error(std::string("[RestaurantPOSApp] CRITICAL ERROR switching modes: ") + e.what());
        
        // Emergency recovery to POS mode
        if (posModeContainer_) {
            kitchenModeContainer_->hide();
            posModeContainer_->show();
            currentMode_ = POS_MODE;
            logger_.info("[RestaurantPOSApp] Emergency recovery: Restored to POS mode");
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
    std::ostringstream changeMsg;
    changeMsg << "🔄 Mode Change Complete: " << getModeDisplayName(newMode);
    logger_.info(changeMsg.str());
    
    // Apply mode-specific behavior
    if (newMode == POS_MODE) {
        logger_.info("📋 POS Mode: Ready for order taking and management");
        
        // Ensure POS components are ready
        if (posModeContainer_) {
            // Any POS-specific initialization can go here
        }
        
    } else if (newMode == KITCHEN_MODE) {
        logger_.info("👨‍🍳 Kitchen Mode: Ready for order preparation tracking");
        
        // Ensure Kitchen components are ready
        if (kitchenModeContainer_) {
            // Any Kitchen-specific initialization can go here
        }
    }
}

// Keep all other methods with logger integration...
void RestaurantPOSApp::initializeComponentFactory() {
    componentFactory_ = std::make_unique<UIComponentFactory>(
        posService_, eventManager_, configManager_);
    
    // Register theme service with factory
    componentFactory_->registerThemeService(themeService_);
    
    logger_.info("✓ Component factory initialized");
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
    
    logger_.info("✓ Main layout initialized for POS mode");
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
    
    logger_.info("✓ Common components created");
}

void RestaurantPOSApp::setupEventListeners() {
    // Listen for theme changes
    if (themeService_) {
        themeService_->onThemeChanged([this](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
            onThemeChanged(oldTheme, newTheme);
        });
    }
    
    logger_.info("✓ Event listeners set up");
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

void RestaurantPOSApp::setupRealTimeUpdates() {
    // Create timer for periodic updates - keep responsive 5-second interval
    updateTimer_ = std::make_unique<Wt::WTimer>();
    updateTimer_->setInterval(std::chrono::seconds(5)); // Keep 5 seconds for responsiveness
    updateTimer_->timeout().connect(this, &RestaurantPOSApp::onPeriodicUpdate);
    updateTimer_->start();
    
    logger_.info("✓ Real-time updates enabled (5 second interval with smart refresh)");
}

void RestaurantPOSApp::onPeriodicUpdate() {
    // Smart periodic update - only refresh data, never recreate UI components
    logger_.debug("[RestaurantPOSApp] Periodic data refresh (preserving UI state)");
    
    // Update footer status
    if (commonFooter_) {
        commonFooter_->updateStatus();
    }
    
    // Smart refresh that only updates data, never recreates UI components
    if (currentMode_ == POS_MODE && posModeContainer_) {
        posModeContainer_->refreshDataOnly(); // New method for data-only refresh
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
    
    std::ostringstream themeMsg;
    themeMsg << "✓ Theme changed from " << themeService_->getThemeName(oldTheme) 
             << " to " << themeService_->getThemeName(newTheme);
    logger_.info(themeMsg.str());
}

void RestaurantPOSApp::logApplicationStart() {
    logger_.info("\n" + std::string(60, '='));
    logger_.info("🍽️  RESTAURANT POS SYSTEM STARTING");
    logger_.info("📋 Default Mode: Point of Sale (POS)");
    logger_.info("🔗 API Integration: Enabled");
    logger_.info(std::string(60, '='));
}

void RestaurantPOSApp::logModeSwitch(OperatingMode mode) {
    std::ostringstream switchMsg;
    switchMsg << "🔄 Mode Switch Complete: " << getModeDisplayName(mode);
    logger_.info(switchMsg.str());
}

std::string RestaurantPOSApp::getModeDisplayName(OperatingMode mode) const {
    switch (mode) {
        case POS_MODE:     return "Point of Sale";
        case KITCHEN_MODE: return "Kitchen Display";
        default:          return "Unknown";
    }
}

void RestaurantPOSApp::debugWidgetState() {
    logger_.debug("[DEBUG] Widget State Check:");
    logger_.debug(std::string("  - modeContainer_: ") + (modeContainer_ ? "VALID" : "NULL"));
    logger_.debug(std::string("  - posModeContainer_: ") + (posModeContainer_ ? "VALID" : "NULL"));
    logger_.debug(std::string("  - kitchenModeContainer_: ") + (kitchenModeContainer_ ? "VALID" : "NULL"));
    
    if (modeContainer_) {
        std::ostringstream childMsg;
        childMsg << "  - modeContainer children: " << modeContainer_->children().size();
        logger_.debug(childMsg.str());
        
        // Check if our containers are in the mode container
        int posIndex = modeContainer_->indexOf(posModeContainer_);
        int kitchenIndex = modeContainer_->indexOf(kitchenModeContainer_);
        
        std::ostringstream indexMsg;
        indexMsg << "  - POS container index: " << posIndex;
        logger_.debug(indexMsg.str());
        
        indexMsg.str(""); indexMsg.clear();
        indexMsg << "  - Kitchen container index: " << kitchenIndex;
        logger_.debug(indexMsg.str());
        
        if (posModeContainer_) {
            std::string visibleMsg = std::string("  - POS container visible: ") + 
                                   (posModeContainer_->isVisible() ? "YES" : "NO");
            logger_.debug(visibleMsg);
        }
        if (kitchenModeContainer_) {
            std::string visibleMsg = std::string("  - Kitchen container visible: ") + 
                                   (kitchenModeContainer_->isVisible() ? "YES" : "NO");
            logger_.debug(visibleMsg);
        }
    }
    
    std::ostringstream modeMsg;
    modeMsg << "  - currentMode_: " << static_cast<int>(currentMode_) << " (" << getModeDisplayName(currentMode_) << ")";
    logger_.debug(modeMsg.str());
    
    std::string destroyMsg = std::string("  - isDestroying_: ") + (isDestroying_ ? "TRUE" : "FALSE");
    logger_.debug(destroyMsg);
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
