#ifndef RESTAURANTPOSAPP_H
#define RESTAURANTPOSAPP_H

#include "../services/POSService.hpp"
#include "../services/ThemeService.hpp"
#include "../services/NotificationService.hpp"
#include "../core/ConfigurationManager.hpp"
#include "../events/EventManager.hpp"
#include "../ui/components/OrderEntryPanel.hpp"
#include "../ui/components/OrderStatusPanel.hpp"
#include "../ui/components/ThemeSelector.hpp"
#include "../ui/factories/UIComponentFactory.hpp"

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTimer.h>
#include <Wt/WEnvironment.h>

#include <memory>

/**
 * @file RestaurantPOSApp.hpp (Modular Version)
 * @brief Simplified main application orchestrator
 * 
 * This is a much simpler version of the RestaurantPOSApp that acts as an
 * orchestrator for modular components rather than handling all UI logic itself.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class RestaurantPOSApp
 * @brief Main application orchestrator for the modular POS system
 * 
 * The RestaurantPOSApp now acts as a lightweight orchestrator that:
 * - Initializes all services and components
 * - Sets up the main layout
 * - Coordinates high-level application flow
 * - Delegates specific functionality to specialized components
 */
class RestaurantPOSApp : public Wt::WApplication {
public:
    /**
     * @brief Constructs the main POS application
     * @param env Wt web environment containing request information
     */
    RestaurantPOSApp(const Wt::WEnvironment& env);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~RestaurantPOSApp() = default;

protected:
    /**
     * @brief Initializes all services and components
     */
    void initializeServices();
    
    /**
     * @brief Sets up the main application layout
     */
    void setupMainLayout();
    
    /**
     * @brief Sets up application-level event listeners
     */
    void setupGlobalEventListeners();
    
    /**
     * @brief Sets up real-time updates
     */
    void setupRealTimeUpdates();
    
    /**
     * @brief Creates the application header
     * @return Header widget
     */
    std::unique_ptr<Wt::WWidget> createApplicationHeader();

private:
    // =================================================================
    // Services (Business Logic Layer)
    // =================================================================
    std::shared_ptr<ConfigurationManager> configManager_;
    std::shared_ptr<EventManager> eventManager_;
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<ThemeService> themeService_;
    std::shared_ptr<NotificationService> notificationService_;
    
    // =================================================================
    // UI Components (Presentation Layer)
    // =================================================================
    std::unique_ptr<UIComponentFactory> componentFactory_;
    std::unique_ptr<OrderEntryPanel> orderEntryPanel_;
    std::unique_ptr<OrderStatusPanel> orderStatusPanel_;
    std::unique_ptr<ThemeSelector> themeSelector_;
    
    // =================================================================
    // Application Infrastructure
    // =================================================================
    Wt::WTimer* updateTimer_;
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Configuration
    static constexpr int DEFAULT_UPDATE_INTERVAL_SECONDS = 5;
    
    // =================================================================
    // Event Handlers
    // =================================================================
    void handleGlobalError(const std::any& eventData);
    void handleSystemNotification(const std::any& eventData);
    void handleConfigurationChanged(const std::any& eventData);
    void handleThemeChanged(const std::any& eventData);
    
    // =================================================================
    // Real-time Update Handlers
    // =================================================================
    void performPeriodicUpdate();
    void refreshAllComponents();
    
    // =================================================================
    // Utility Methods
    // =================================================================
    void logApplicationStart();
    void logApplicationShutdown();
    void handleApplicationError(const std::string& error, bool critical = false);
};

/**
 * @brief Application factory function
 * @param env Wt web environment
 * @return Unique pointer to the created application
 */
std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env);

#endif // RESTAURANTPOSAPP_H

//=============================================================================
// IMPLEMENTATION FILE: RestaurantPOSApp.cpp
//=============================================================================

#include "RestaurantPOSApp.hpp"
#include "../events/POSEvents.hpp"

#include <Wt/WBootstrapTheme.h>
#include <Wt/WText.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>

#include <iostream>
#include <exception>

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env) 
    : Wt::WApplication(env), updateTimer_(nullptr) {
    
    try {
        logApplicationStart();
        
        // Set basic application properties
        setTitle("Restaurant POS System - Modular Architecture");
        
        // Set up Bootstrap theme as base
        auto bootstrapTheme = std::make_shared<Wt::WBootstrapTheme>();
        bootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
        setTheme(bootstrapTheme);
        
        // Initialize all services and components
        initializeServices();
        
        // Set up the main UI layout
        setupMainLayout();
        
        // Set up global event listeners
        setupGlobalEventListeners();
        
        // Set up real-time updates
        setupRealTimeUpdates();
        
        std::cout << "✓ Restaurant POS Application initialized successfully" << std::endl;
        
    } catch (const std::exception& e) {
        handleApplicationError("Failed to initialize application: " + std::string(e.what()), true);
    } catch (...) {
        handleApplicationError("Unknown error during application initialization", true);
    }
}

void RestaurantPOSApp::initializeServices() {
    std::cout << "Initializing services..." << std::endl;
    
    // Initialize configuration manager first
    configManager_ = std::make_shared<ConfigurationManager>();
    configManager_->initialize();
    
    // Initialize event manager
    eventManager_ = std::make_shared<EventManager>();
    
    // Initialize core business service
    posService_ = std::make_shared<POSService>(eventManager_);
    
    // Initialize theme service
    themeService_ = std::make_shared<ThemeService>(eventManager_, this);
    themeService_->initialize();
    
    // Initialize notification service
    notificationService_ = std::make_shared<NotificationService>(eventManager_);
    
    // Initialize UI component factory
    componentFactory_ = std::make_unique<UIComponentFactory>(
        posService_, eventManager_, configManager_);
    
    std::cout << "✓ All services initialized" << std::endl;
}

void RestaurantPOSApp::setupMainLayout() {
    std::cout << "Setting up main layout..." << std::endl;
    
    // Set container styling
    root()->addStyleClass("container-fluid pos-application");
    
    // Create main layout
    auto mainLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // Add application header
    auto header = createApplicationHeader();
    mainLayout->addWidget(std::move(header));
    
    // Create main content area with horizontal layout
    auto contentContainer = std::make_unique<Wt::WContainerWidget>();
    auto contentLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Create order entry panel (left side - 60% width)
    orderEntryPanel_ = componentFactory_->createOrderEntryPanel();
    contentLayout->addWidget(std::move(orderEntryPanel_), 3);
    
    // Create order status panel (right side - 40% width)
    orderStatusPanel_ = componentFactory_->createOrderStatusPanel();
    contentLayout->addWidget(std::move(orderStatusPanel_), 2);
    
    contentContainer->setLayout(std::move(contentLayout));
    mainLayout->addWidget(std::move(contentContainer), 1);
    
    root()->setLayout(std::move(mainLayout));
    
    std::cout << "✓ Main layout setup complete" << std::endl;
}

std::unique_ptr<Wt::WWidget> RestaurantPOSApp::createApplicationHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("pos-header");
    
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Title section
    auto titleContainer = std::make_unique<Wt::WContainerWidget>();
    auto titleLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto title = std::make_unique<Wt::WText>("🍽️ Restaurant POS System");
    title->addStyleClass("h1 text-primary mb-0");
    titleLayout->addWidget(std::move(title));
    
    auto subtitle = std::make_unique<Wt::WText>(
        "Modular Architecture: Service Layer • UI Components • Event System");
    subtitle->addStyleClass("h6 text-muted");
    titleLayout->addWidget(std::move(subtitle));
    
    titleContainer->setLayout(std::move(titleLayout));
    headerLayout->addWidget(std::move(titleContainer), 1);
    
    // Theme selector section
    themeSelector_ = componentFactory_->createThemeSelector();
    headerLayout->addWidget(std::move(themeSelector_));
    
    header->setLayout(std::move(headerLayout));
    return std::move(header);
}

void RestaurantPOSApp::setupGlobalEventListeners() {
    std::cout << "Setting up global event listeners..." << std::endl;
    
    // Subscribe to global events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::SYSTEM_ERROR, 
            [this](const std::any& data) { handleGlobalError(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::NOTIFICATION_REQUESTED,
            [this](const std::any& data) { handleSystemNotification(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CONFIGURATION_CHANGED,
            [this](const std::any& data) { handleConfigurationChanged(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::THEME_CHANGED,
            [this](const std::any& data) { handleThemeChanged(data); }));
    
    std::cout << "✓ Global event listeners setup complete" << std::endl;
}

void RestaurantPOSApp::setupRealTimeUpdates() {
    std::cout << "Setting up real-time updates..." << std::endl;
    
    int updateInterval = configManager_->getUIUpdateInterval();
    if (updateInterval <= 0) {
        updateInterval = DEFAULT_UPDATE_INTERVAL_SECONDS;
    }
    
    updateTimer_ = root()->addChild(std::make_unique<Wt::WTimer>());
    updateTimer_->setInterval(std::chrono::seconds(updateInterval));
    updateTimer_->timeout().connect([this] { performPeriodicUpdate(); });
    updateTimer_->start();
    
    std::cout << "✓ Real-time updates configured (interval: " 
              << updateInterval << " seconds)" << std::endl;
}

void RestaurantPOSApp::performPeriodicUpdate() {
    try {
        // Refresh all components with latest data
        refreshAllComponents();
        
        // Publish periodic update event for components that need it
        eventManager_->publish(POSEvents::UI_REFRESH_REQUESTED);
        
    } catch (const std::exception& e) {
        handleApplicationError("Error during periodic update: " + std::string(e.what()));
    }
}

void RestaurantPOSApp::refreshAllComponents() {
    if (orderEntryPanel_) {
        orderEntryPanel_->refresh();
    }
    
    if (orderStatusPanel_) {
        orderStatusPanel_->refresh();
    }
}

void RestaurantPOSApp::handleGlobalError(const std::any& eventData) {
    try {
        auto errorData = std::any_cast<POSEvents::ErrorEventData>(eventData);
        
        std::cerr << "Global Error [" << errorData.component << "]: " 
                  << errorData.errorMessage << std::endl;
        
        if (errorData.isCritical) {
            // Handle critical errors - maybe show dialog or restart components
            std::cerr << "CRITICAL ERROR - System stability may be compromised" << std::endl;
        }
        
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error handling global error event: invalid data type" << std::endl;
    }
}

void RestaurantPOSApp::handleSystemNotification(const std::any& eventData) {
    if (notificationService_) {
        notificationService_->handleNotification(eventData);
    }
}

void RestaurantPOSApp::handleConfigurationChanged(const std::any& eventData) {
    try {
        // Reload configuration and update components
        configManager_->reload();
        
        // Update real-time timer if interval changed
        int newInterval = configManager_->getUIUpdateInterval();
        if (updateTimer_ && newInterval > 0) {
            updateTimer_->setInterval(std::chrono::seconds(newInterval));
        }
        
        std::cout << "Configuration reloaded successfully" << std::endl;
        
    } catch (const std::exception& e) {
        handleApplicationError("Error reloading configuration: " + std::string(e.what()));
    }
}

void RestaurantPOSApp::handleThemeChanged(const std::any& eventData) {
    try {
        auto themeData = std::any_cast<POSEvents::ThemeEventData>(eventData);
        
        std::cout << "Theme changed from '" << themeData.previousThemeId 
                  << "' to '" << themeData.themeId << "'" << std::endl;
        
        // Refresh all components to apply new theme
        refreshAllComponents();
        
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error handling theme change event: invalid data type" << std::endl;
    }
}

void RestaurantPOSApp::logApplicationStart() {
    std::cout << "===========================================================" << std::endl;
    std::cout << "  Restaurant POS System - Modular Architecture v2.0.0" << std::endl;
    std::cout << "===========================================================" << std::endl;
    std::cout << "Framework: Wt (C++ Web Toolkit)" << std::endl;
    std::cout << "Architecture: Modular Service-Component Design" << std::endl;
    std::cout << "Components:" << std::endl;
    std::cout << "  • Core Services: POSService, ThemeService, ConfigurationManager" << std::endl;
    std::cout << "  • UI Components: OrderEntryPanel, OrderStatusPanel, MenuDisplay" << std::endl;
    std::cout << "  • Event System: EventManager, POSEvents" << std::endl;
    std::cout << "  • Business Logic: OrderManager, PaymentProcessor, KitchenInterface" << std::endl;
    std::cout << "===========================================================" << std::endl;
}

void RestaurantPOSApp::logApplicationShutdown() {
    std::cout << "Restaurant POS Application shutting down..." << std::endl;
    
    // Clean up event subscriptions
    for (auto handle : eventSubscriptions_) {
        eventManager_->unsubscribe(handle);
    }
    eventSubscriptions_.clear();
    
    std::cout << "✓ Application shutdown complete" << std::endl;
}

void RestaurantPOSApp::handleApplicationError(const std::string& error, bool critical) {
    std::cerr << (critical ? "CRITICAL ERROR: " : "ERROR: ") << error << std::endl;
    
    // Publish error event
    eventManager_->publish(POSEvents::SYSTEM_ERROR, 
        POSEvents::createErrorEvent(error, "", "RestaurantPOSApp", critical));
    
    if (critical) {
        // For critical errors, we might want to restart the application
        // or show a critical error dialog
        std::cerr << "Application may need to be restarted" << std::endl;
    }
}

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
