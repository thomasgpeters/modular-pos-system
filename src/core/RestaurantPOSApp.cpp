//============================================================================
// src/core/RestaurantPOSApp.cpp - FIXED CSS Loading and Theme Integration
//============================================================================

#include "../../include/core/RestaurantPOSApp.hpp"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WEnvironment.h>
#include <iostream>
#include <sstream>

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , logger_(Logger::getInstance())
    , isDestroying_(false)
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
    
    // CRITICAL: Load CSS and theme FIRST before any UI creation
    setupBootstrapTheme();
    
    // Initialize services
    initializeServices();
    
    // Initialize component factory
    initializeComponentFactory();
    
    // FIXED: Apply theme immediately after services are ready
    initializeThemeService();
    
    // Set up the main application layout
    setupMainLayout();
    
    // Create common components (header/footer)
    createCommonComponents();
    
    // CRITICAL: Enforce layout constraints after creation
    enforceLayoutConstraints();
    
    // Create mode-specific containers
    createModeContainers();
    
    // Set up event listeners
    setupEventListeners();
    
    // Set up real-time updates
    setupRealTimeUpdates();
    
    // ENHANCED: Ensure POS mode is loaded and visible by default
    ensurePOSModeDefault();
    
    logger_.info("✓ RestaurantPOSApp initialized successfully in POS mode with styling");
    doJavaScript("setTimeout(function(){var s=document.createElement('style');s.innerHTML='body>*:not(.Wt-domRoot):not(.pos-app-container){display:none!important;position:absolute!important;left:-9999px!important;}';document.head.appendChild(s);Array.from(document.body.childNodes).forEach(function(n){if(n.nodeType===3&&n.textContent.trim().length>100&&(n.textContent.includes('CDATA')||n.textContent.includes('window.')||n.textContent.includes('function')))n.remove();});},50);");
    useStyleSheet(Wt::WLink("/assets/css/hide-cdata.css"));
}

void RestaurantPOSApp::setupBootstrapTheme() {
    logger_.info("[RestaurantPOSApp] Setting up Bootstrap theme and CSS...");
    
    // Set Bootstrap 5 theme
    auto theme = std::make_shared<Wt::WBootstrap5Theme>();
    setTheme(theme);
    
    // CRITICAL: Load all CSS in correct order
    loadFrameworkCSS();
    loadComponentCSS();
    loadThemeCSS();
    
    logger_.info("✓ Bootstrap theme and CSS framework loaded");
}

void RestaurantPOSApp::loadFrameworkCSS() {
    logger_.info("[RestaurantPOSApp] Loading framework CSS files...");

    // Bootstrap from CDN
    useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/bootstrap/5.3.0/css/bootstrap.min.css");

    // Framework CSS files (matching actual files in assets/css/)
    useStyleSheet("/assets/css/base.css");
    useStyleSheet("/assets/css/bootstrap-custom.css");
    useStyleSheet("/assets/css/main.css");
    useStyleSheet("/assets/css/typography.css");
    useStyleSheet("/assets/css/utilities.css");
    useStyleSheet("/assets/css/responsive.css");

    logger_.info("✓ Framework CSS loaded");
}

void RestaurantPOSApp::loadComponentCSS() {
    logger_.info("[RestaurantPOSApp] Loading component CSS files...");
    
    // Component CSS files (matching CMakeLists.txt structure)
    useStyleSheet("/assets/css/components/pos-components.css");
    useStyleSheet("/assets/css/components/menu-components.css");
    useStyleSheet("/assets/css/components/order-components.css");
    useStyleSheet("/assets/css/components/kitchen-components.css");
    useStyleSheet("/assets/css/components/payment-components.css");
    useStyleSheet("/assets/css/components/modal-components.css");
    useStyleSheet("/assets/css/components/button-components.css");
    useStyleSheet("/assets/css/components/table-components.css");
    useStyleSheet("/assets/css/components/form-components.css");
    
    logger_.info("✓ Component CSS loaded");
}

void RestaurantPOSApp::loadThemeCSS() {
    logger_.info("[RestaurantPOSApp] Loading theme CSS files...");

    // Theme CSS files (matching actual files in assets/css/themes/)
    useStyleSheet("/assets/css/themes/light-theme.css");
    useStyleSheet("/assets/css/themes/dark-theme.css");
    useStyleSheet("/assets/css/themes/colorful-theme.css");
    useStyleSheet("/assets/css/themes/restaurant-theme.css");
    useStyleSheet("/assets/css/themes/high-contrast.css");
    useStyleSheet("/assets/css/themes/theme-light.css");
    useStyleSheet("/assets/css/themes/theme-dark.css");
    useStyleSheet("/assets/css/themes/theme-warm.css");
    useStyleSheet("/assets/css/themes/theme-cool.css");

    logger_.info("✓ Theme CSS loaded");
}

void RestaurantPOSApp::addCustomCSS() {
    // DEPRECATED: This method is replaced by the new CSS loading methods
    // Keeping for backward compatibility but functionality moved to setup methods
    logger_.debug("[RestaurantPOSApp] addCustomCSS() called - using new CSS loading system");
}

void RestaurantPOSApp::initializeThemeService() {
    logger_.info("[RestaurantPOSApp] Initializing theme service...");
    
    if (themeService_) {
        // Load theme preference from storage
        themeService_->loadThemePreference();
        
        // Apply the current theme immediately
        applyCurrentTheme();
        
        logger_.info("✓ Theme service initialized and theme applied");
    } else {
        logger_.error("[RestaurantPOSApp] ERROR: Theme service is null!");
    }
}

void RestaurantPOSApp::applyCurrentTheme() {
    if (!themeService_) {
        logger_.error("[RestaurantPOSApp] Cannot apply theme - theme service is null");
        return;
    }
    
    auto currentTheme = themeService_->getCurrentTheme();
    std::string themeClass = themeService_->getThemeCSSClass(currentTheme);
    std::string themeName = themeService_->getThemeName(currentTheme);
    
    logger_.info(std::string("[RestaurantPOSApp] Applying theme: ") + themeName + " (class: " + themeClass + ")");
    
    // Apply theme class to body element
    if (root()) {
        // Remove any existing theme classes
        root()->removeStyleClass("theme-light theme-dark theme-colorful theme-restaurant theme-base");
        
        // Add the current theme class
        root()->addStyleClass(themeClass);
        
        // Also add to main container if it exists
        if (mainContainer_) {
            mainContainer_->removeStyleClass("theme-light theme-dark theme-colorful theme-restaurant theme-base");
            mainContainer_->addStyleClass(themeClass);
        }
        
        logger_.info(std::string("✓ Theme applied: ") + themeName);
    }
}

// ============================================================================
// C++ Layout Fix - Replace these methods in RestaurantPOSApp.cpp
// ============================================================================

// 2. REPLACE the setupMainLayout method in RestaurantPOSApp.cpp:
void RestaurantPOSApp::setupMainLayout() {
    setTitle("Restaurant POS System - Point of Sale");
    
    // CRITICAL: Set viewport and ensure proper sizing
    addMetaHeader(Wt::MetaHeaderType::Meta, "viewport", "width=device-width, initial-scale=1");
    
    // Create main container with explicit height constraints
    mainContainer_ = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
    mainContainer_->setStyleClass("h-100 pos-app-container");
    mainContainer_->setId("main-app-container");
    
    // FORCE MAIN CONTAINER VISIBILITY
    mainContainer_->show();
    mainContainer_->setHidden(false);
    mainContainer_->setHeight(Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
    mainContainer_->setMaximumSize(Wt::WLength::Auto, Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
    mainContainer_->setAttributeValue("style", 
        "display: block !important; visibility: visible !important; "
        "height: 100vh !important; width: 100% !important; "
        "background-color: #ffffff !important;");
    
    // Create main layout with explicit spacing and margins
    mainLayout_ = mainContainer_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);
    
    logger_.info("✓ Main layout initialized with forced visibility and height constraints");
}

void RestaurantPOSApp::onThemeChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
    logger_.info("[RestaurantPOSApp] Theme change event received");
    
    // Apply the new theme immediately
    applyCurrentTheme();
    
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
    
    // Show notification
    if (eventManager_) {
        auto notification = POSEvents::createNotificationData(
            "Theme changed to " + themeService_->getThemeName(newTheme),
            "info",
            2000
        );
        eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED, notification);
    }
}

// Rest of the methods remain the same but with enhanced logging...

void RestaurantPOSApp::initializeServices() {
    logger_.info("[RestaurantPOSApp] Initializing services...");
    
    // Create event manager first (other services depend on it)
    eventManager_ = std::make_shared<EventManager>();
    
    // Create configuration manager and initialize with API settings
    configManager_ = std::make_shared<ConfigurationManager>();
    configManager_->initialize();
    
    // ENABLE API for testing
    configManager_->setValue<bool>("api.enabled", true);
    configManager_->setValue<std::string>("api.base_url", "http://localhost:5656/api");
    configManager_->setValue<std::string>("api.auth_token", "");
    configManager_->setValue<int>("api.timeout", 30);
    configManager_->setValue<bool>("api.enable_caching", true);
    configManager_->setValue<bool>("api.debug_mode", true);
    
    logger_.info("[RestaurantPOSApp] API configuration:");
    
    LOG_CONFIG_BOOL(logger_, info, "API Enabled", configManager_->getValue<bool>("api.enabled", false));
    LOG_CONFIG_STRING(logger_, info, "API Base URL", configManager_->getValue<std::string>("api.base_url", "not set"));
    LOG_CONFIG_BOOL(logger_, info, "Debug Mode", configManager_->getValue<bool>("api.debug_mode", false));
    
    // Create POS service
    posService_ = APIServiceFactory::createPOSService(eventManager_, configManager_);
    
    if (!posService_) {
        LOG_COMPONENT_ERROR(logger_, "RestaurantPOSApp", "service creation", "Failed to create POS service");
        throw std::runtime_error("Failed to create POS service");
    }
    
    // Check service type
    auto enhancedService = std::dynamic_pointer_cast<EnhancedPOSService>(posService_);
    if (enhancedService) {
        LOG_CONFIG_BOOL(logger_, info, "EnhancedPOSService connected", enhancedService->isConnected());
        logger_.info("[RestaurantPOSApp] ✓ EnhancedPOSService created successfully");
    } else {
        logger_.info("[RestaurantPOSApp] ✓ Standard POSService created (local data)");
    }
    
    // Initialize menu
    posService_->initializeMenu();
    
    // Create theme service AFTER event manager is ready
    themeService_ = std::make_shared<ThemeService>(this);

    // Initialize LLM Query Service for business demographics
    initializeLLMService();

    LOG_OPERATION_STATUS(logger_, "Core services initialization", true);
}

void RestaurantPOSApp::initializeLLMService() {
    logger_.info("[RestaurantPOSApp] Initializing LLM Query Service...");

    // Check if LLM is enabled in configuration
    if (!configManager_->isLLMEnabled()) {
        logger_.info("[RestaurantPOSApp] LLM service is disabled in configuration");
        return;
    }

    try {
        // Create the LLM query service
        llmQueryService_ = std::make_shared<LLMQueryService>(eventManager_);

        // Get configuration values
        std::string provider = configManager_->getLLMProvider();
        std::string apiKey = configManager_->getLLMApiKey();
        std::string model = configManager_->getLLMModel();
        std::string baseUrl = configManager_->getLLMBaseUrl();
        int timeout = configManager_->getLLMTimeout();
        bool debugMode = configManager_->isLLMDebugMode();

        // Log configuration (mask API key for security)
        logger_.info("[RestaurantPOSApp] LLM Configuration:");
        logger_.info(std::string("  Provider: ") + provider);
        logger_.info(std::string("  Model: ") + model);
        logger_.info(std::string("  Base URL: ") + (baseUrl.empty() ? "(default)" : baseUrl));
        logger_.info(std::string("  Timeout: ") + std::to_string(timeout) + "s");
        logger_.info(std::string("  Debug Mode: ") + (debugMode ? "enabled" : "disabled"));
        logger_.info(std::string("  API Key: ") + (apiKey.empty() ? "(not set)" : "********"));

        // Check if API key is available
        if (apiKey.empty()) {
            logger_.info("[RestaurantPOSApp] LLM API key not configured - service will not be functional");
            logger_.info("[RestaurantPOSApp] Set the LLM_API_KEY environment variable or configure in pos_config.xml");
            return;
        }

        // Convert provider string to enum
        LLMQueryService::LLMProvider llmProvider = LLMQueryService::stringToProvider(provider);

        // Initialize the service
        bool initialized = llmQueryService_->initialize(apiKey, llmProvider, baseUrl);

        if (initialized) {
            // Apply additional configuration
            if (!model.empty()) {
                llmQueryService_->setModel(model);
            }
            llmQueryService_->setTimeout(timeout);
            llmQueryService_->setDebugMode(debugMode);

            logger_.info("[RestaurantPOSApp] ✓ LLM Query Service initialized successfully");

            // Log default geolocation settings
            double defaultRadius = configManager_->getLLMDefaultRadius();
            double maxRadius = configManager_->getLLMMaxRadius();
            logger_.info(std::string("  Default Search Radius: ") + std::to_string(defaultRadius) + " km");
            logger_.info(std::string("  Maximum Search Radius: ") + std::to_string(maxRadius) + " km");

        } else {
            logger_.error("[RestaurantPOSApp] Failed to initialize LLM Query Service");
            llmQueryService_.reset();
        }

    } catch (const std::exception& e) {
        logger_.error(std::string("[RestaurantPOSApp] Error initializing LLM service: ") + e.what());
        llmQueryService_.reset();
    }
}

void RestaurantPOSApp::initializeComponentFactory() {
    componentFactory_ = std::make_unique<UIComponentFactory>(
        posService_, eventManager_, configManager_);
    
    // Register theme service with factory
    componentFactory_->registerThemeService(themeService_);
    
    logger_.info("✓ Component factory initialized");
}

// 3. REPLACE the createCommonComponents method in RestaurantPOSApp.cpp:
void RestaurantPOSApp::createCommonComponents() {
    logger_.info("[RestaurantPOSApp] Creating common components with forced visibility...");
    
    // Create header with mode switching capability
    auto modeChangeCallback = [this](ModeSelector::Mode mode) {
        OperatingMode newMode = (mode == ModeSelector::POS_MODE) ? POS_MODE : KITCHEN_MODE;
        switchMode(newMode);
    };
    
    // CRITICAL: Create header with explicit size constraints
    commonHeader_ = mainLayout_->addWidget(
        std::make_unique<CommonHeader>(themeService_, eventManager_, modeChangeCallback));
    
    // FORCE header size constraints and visibility
    commonHeader_->setHeight(Wt::WLength(60, Wt::LengthUnit::Pixel));
    commonHeader_->setMinimumSize(Wt::WLength::Auto, Wt::WLength(60, Wt::LengthUnit::Pixel));
    commonHeader_->setMaximumSize(Wt::WLength::Auto, Wt::WLength(60, Wt::LengthUnit::Pixel));
    commonHeader_->show();
    commonHeader_->setHidden(false);
    
    // CRITICAL: Set layout stretch factor to 0 (don't expand)
    mainLayout_->setStretchFactor(commonHeader_, 0);
    
    // Create mode container with explicit sizing and visibility
    modeContainer_ = mainLayout_->addWidget(std::make_unique<Wt::WContainerWidget>(), 1);
    modeContainer_->setStyleClass("flex-grow-1 mode-container-wrapper");
    modeContainer_->setId("main-mode-container");
    
    // FORCE MODE CONTAINER VISIBILITY AND SIZE
    modeContainer_->show();
    modeContainer_->setHidden(false);
    modeContainer_->setMinimumSize(Wt::WLength::Auto, Wt::WLength(400, Wt::LengthUnit::Pixel));
    modeContainer_->setAttributeValue("style",
        "display: block !important; visibility: visible !important; "
        "height: calc(100vh - 100px) !important; width: 100% !important; "
        "background-color: #f8f9fa !important; padding: 10px;");
    
    // CRITICAL: Set mode container to expand and fill remaining space
    mainLayout_->setStretchFactor(modeContainer_, 1);
    
    // Create footer with size constraints
    commonFooter_ = mainLayout_->addWidget(
        std::make_unique<CommonFooter>(posService_, eventManager_));
    
    // FORCE footer size constraints and visibility
    commonFooter_->setHeight(Wt::WLength(40, Wt::LengthUnit::Pixel));
    commonFooter_->setMinimumSize(Wt::WLength::Auto, Wt::WLength(40, Wt::LengthUnit::Pixel));
    commonFooter_->setMaximumSize(Wt::WLength::Auto, Wt::WLength(40, Wt::LengthUnit::Pixel));
    commonFooter_->show();
    commonFooter_->setHidden(false);
    
    // CRITICAL: Set layout stretch factor to 0 (don't expand)
    mainLayout_->setStretchFactor(commonFooter_, 0);
    
    // Debug: Check all container visibility
    logger_.info("[DEBUG] Container visibility after creation:");
    logger_.info(std::string("[DEBUG]   mainContainer_ visible: ") + 
                (mainContainer_->isVisible() ? "YES" : "NO"));
    logger_.info(std::string("[DEBUG]   commonHeader_ visible: ") + 
                (commonHeader_->isVisible() ? "YES" : "NO"));
    logger_.info(std::string("[DEBUG]   modeContainer_ visible: ") + 
                (modeContainer_->isVisible() ? "YES" : "NO"));
    logger_.info(std::string("[DEBUG]   commonFooter_ visible: ") + 
                (commonFooter_->isVisible() ? "YES" : "NO"));
    
    logger_.info("✓ Common components created with forced visibility and explicit size constraints");
}

// ============================================================================
// PARENT CONTAINER FIX - Update RestaurantPOSApp.cpp methods
// The issue is that the parent modeContainer_ is not visible/sized properly
// ============================================================================

// 1. REPLACE the createModeContainers method in RestaurantPOSApp.cpp:
void RestaurantPOSApp::createModeContainers() {
    logger_.info("[RestaurantPOSApp] Creating mode containers with forced visibility...");
    
    try {
        // Ensure services exist
        if (!posService_ || !eventManager_) {
            throw std::runtime_error("Services not initialized before creating mode containers");
        }
        
        // Ensure mode container exists and is properly sized
        if (!modeContainer_) {
            throw std::runtime_error("Mode container not initialized");
        }
        
        // FORCE MODE CONTAINER VISIBILITY
        modeContainer_->show();
        modeContainer_->setHidden(false);
        modeContainer_->setAttributeValue("style",
            "display: block !important; visibility: visible !important; "
            "height: calc(100vh - 120px) !important; width: 100% !important; "
            "background-color: #f8f9fa !important; padding: 10px;");
        
        logger_.info("[RestaurantPOSApp] Mode container forced visible");
        
        logger_.info("[RestaurantPOSApp] Creating POS mode container...");
        
        // Create POS mode container as child
        posModeContainer_ = modeContainer_->addNew<POSModeContainer>(posService_, eventManager_);
        if (!posModeContainer_) {
            throw std::runtime_error("Failed to create POS mode container");
        }
        
        // FORCE POS CONTAINER VISIBILITY
        posModeContainer_->setId("pos-mode-container");
        posModeContainer_->addStyleClass("mode-container pos-mode-active");
        posModeContainer_->show();
        posModeContainer_->setHidden(false);
        posModeContainer_->setAttributeValue("style", 
            "display: block !important; visibility: visible !important; "
            "height: 100% !important; width: 100% !important; "
            "background-color: #f8f9fa !important;");
        
        logger_.info("[RestaurantPOSApp] Creating Kitchen mode container...");
        
        kitchenModeContainer_ = modeContainer_->addNew<KitchenModeContainer>(posService_, eventManager_);
        if (!kitchenModeContainer_) {
            throw std::runtime_error("Failed to create Kitchen mode container");
        }
        
        // Kitchen container - hidden initially
        kitchenModeContainer_->setId("kitchen-mode-container");
        kitchenModeContainer_->addStyleClass("mode-container kitchen-mode-inactive");
        kitchenModeContainer_->hide(); // Hide kitchen mode initially
        
        // Set initial visibility state clearly
        logger_.info("[RestaurantPOSApp] Setting initial visibility states...");
        
        // Debug: Check hierarchy visibility
        logger_.info("[DEBUG] Visibility check:");
        logger_.info(std::string("[DEBUG]   modeContainer_ visible: ") + 
                    (modeContainer_->isVisible() ? "YES" : "NO"));
        logger_.info(std::string("[DEBUG]   posModeContainer_ visible: ") + 
                    (posModeContainer_->isVisible() ? "YES" : "NO"));
        logger_.info(std::string("[DEBUG]   kitchenModeContainer_ visible: ") + 
                    (kitchenModeContainer_->isVisible() ? "YES" : "NO"));
        
        logger_.info("[RestaurantPOSApp] ✓ Mode containers created successfully with forced visibility");
        
    } catch (const std::exception& e) {
        logger_.error(std::string("[RestaurantPOSApp] CRITICAL ERROR creating mode containers: ") + e.what());
        throw;
    }
}

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
        
        // Apply theme to ensure styling is visible
        applyCurrentTheme();
        
        logger_.info("[RestaurantPOSApp] ✓ POS mode is now active and visible with styling");
        
        // Add a welcome message to confirm POS mode is loaded
        if (eventManager_) {
            auto welcomeEvent = POSEvents::createNotificationData(
                "Welcome to Restaurant POS System - Ready with Theme Support",
                "success",
                3000
            );
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED, welcomeEvent);
        }
        
    } catch (const std::exception& e) {
        logger_.error(std::string("[RestaurantPOSApp] ERROR ensuring POS mode default: ") + e.what());
    }
}

// Include all other methods (switchMode, setupEventListeners, etc.) with minimal changes
// but ensure they call applyCurrentTheme() when needed...

void RestaurantPOSApp::setupEventListeners() {
    // Listen for theme changes
    if (themeService_) {
        themeService_->onThemeChanged([this](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
            onThemeChanged(oldTheme, newTheme);
        });
    }
    
    logger_.info("✓ Event listeners set up");
}

void RestaurantPOSApp::setupRealTimeUpdates() {
    // Create timer for periodic updates - keep responsive 5-second interval
    updateTimer_ = std::make_unique<Wt::WTimer>();
    updateTimer_->setInterval(std::chrono::seconds(5));
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

// ... rest of the methods remain the same

RestaurantPOSApp::~RestaurantPOSApp() {
    logger_.info("[RestaurantPOSApp] Destructor called");
    
    // Set flag to prevent further operations
    isDestroying_ = true;
    
    try {
        // Stop the update timer first
        if (updateTimer_) {
            updateTimer_->stop();
            updateTimer_.reset();
        }
        
        // Remove widgets from containers before destruction
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

void RestaurantPOSApp::logApplicationStart() {
    logger_.info("\n" + std::string(60, '='));
    logger_.info("🍽️  RESTAURANT POS SYSTEM STARTING");
    logger_.info("📋 Default Mode: Point of Sale (POS)");
    logger_.info("🔗 API Integration: Enabled");
    logger_.info("🎨 Theme System: Activated");
    logger_.info(std::string(60, '='));
}

//============================================================================
// Missing Method Implementations - Add these to RestaurantPOSApp.cpp
//============================================================================

// Add this method implementation to RestaurantPOSApp.cpp
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
        
        // Clear previous mode styling
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

// Add this method implementation to RestaurantPOSApp.cpp
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

// Add this method implementation to RestaurantPOSApp.cpp
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

// Add this method implementation to RestaurantPOSApp.cpp
void RestaurantPOSApp::showPOSMode() {
    // This method is kept for compatibility but the main switching is handled by switchMode()
    switchMode(POS_MODE);
}

// Add this method implementation to RestaurantPOSApp.cpp
void RestaurantPOSApp::showKitchenMode() {
    // This method is kept for compatibility but the main switching is handled by switchMode()
    switchMode(KITCHEN_MODE);
}

// Add this method implementation to RestaurantPOSApp.cpp
void RestaurantPOSApp::hideModeContainers() {
    if (posModeContainer_) {
        posModeContainer_->hide();
    }
    if (kitchenModeContainer_) {
        kitchenModeContainer_->hide();
    }
}

//============================================================================
// Remaining Missing Method Implementations - Add these to RestaurantPOSApp.cpp
//============================================================================

// Add this method implementation to RestaurantPOSApp.cpp
void RestaurantPOSApp::logModeSwitch(OperatingMode mode) {
    std::ostringstream switchMsg;
    switchMsg << "🔄 Mode Switch Complete: " << getModeDisplayName(mode);
    logger_.info(switchMsg.str());
    
    // Log additional mode switch details
    logger_.info(std::string("[RestaurantPOSApp] Mode switch timestamp: ") + 
                std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()));
    
    // Log container visibility states for debugging
    if (posModeContainer_ && kitchenModeContainer_) {
        std::ostringstream visibilityMsg;
        visibilityMsg << "[RestaurantPOSApp] Container visibility - POS: " 
                     << (posModeContainer_->isVisible() ? "VISIBLE" : "HIDDEN")
                     << ", Kitchen: " 
                     << (kitchenModeContainer_->isVisible() ? "VISIBLE" : "HIDDEN");
        logger_.debug(visibilityMsg.str());
    }
}

// Add this method implementation to RestaurantPOSApp.cpp
std::string RestaurantPOSApp::getModeDisplayName(OperatingMode mode) const {
    switch (mode) {
        case POS_MODE:     
            return "Point of Sale";
        case KITCHEN_MODE: 
            return "Kitchen Display";
        default:          
            return "Unknown Mode";
    }
}

// OPTIONAL: Add this method if you want enhanced debugging capabilities
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

// NEW METHOD: Force layout constraints after creation
void RestaurantPOSApp::enforceLayoutConstraints() {
    logger_.info("[RestaurantPOSApp] Enforcing layout constraints...");
    
    if (commonHeader_) {
        // Double-check header constraints
        commonHeader_->setHeight(Wt::WLength(60, Wt::LengthUnit::Pixel));
        commonHeader_->setMaximumSize(Wt::WLength::Auto, Wt::WLength(60, Wt::LengthUnit::Pixel));
        
        // Add CSS class for additional styling
        commonHeader_->addStyleClass("pos-header-fixed");
        
        // Set CSS style directly as backup
        commonHeader_->setAttributeValue("style", 
            "height: 60px !important; max-height: 60px !important; min-height: 60px !important; flex: 0 0 60px !important;");
    }
    
    if (modeContainer_) {
        // Ensure content area expands
        modeContainer_->addStyleClass("pos-content-flex");
        
        // Calculate available height (viewport - header - footer)
        modeContainer_->setAttributeValue("style", 
            "flex: 1 1 auto !important; height: calc(100vh - 100px) !important; overflow: auto !important;");
    }
    
    if (commonFooter_) {
        // Double-check footer constraints
        commonFooter_->setHeight(Wt::WLength(40, Wt::LengthUnit::Pixel));
        commonFooter_->setMaximumSize(Wt::WLength::Auto, Wt::WLength(40, Wt::LengthUnit::Pixel));
        
        // Add CSS class for additional styling
        commonFooter_->addStyleClass("pos-footer-fixed");
        
        // Set CSS style directly as backup
        commonFooter_->setAttributeValue("style", 
            "height: 40px !important; max-height: 40px !important; min-height: 40px !important; flex: 0 0 40px !important;");
    }
    
    logger_.info("✓ Layout constraints enforced with inline styles");
}

//============================================================================
// REQUIRED: Application Factory Function - ADD THIS AT THE END OF RestaurantPOSApp.cpp
//============================================================================

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
