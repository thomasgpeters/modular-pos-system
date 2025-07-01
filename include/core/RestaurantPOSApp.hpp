#ifndef RESTAURANTPOSAPP_H
#define RESTAURANTPOSAPP_H

#include "../services/POSService.hpp"
#include "../events/EventManager.hpp"

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTimer.h>
#include <Wt/WEnvironment.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>

#include <memory>

/**
 * @file RestaurantPOSApp.hpp (Enhanced with CSS)
 * @brief Main application with proper styling and Bootstrap integration
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class RestaurantPOSApp
 * @brief Main application orchestrator with enhanced UI styling
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

private:
    // Core services
    std::shared_ptr<EventManager> eventManager_;
    std::shared_ptr<POSService> posService_;
    
    // UI components
    Wt::WContainerWidget* mainContainer_;
    Wt::WText* statusText_;
    Wt::WPushButton* testButton_;
    Wt::WTimer* updateTimer_;
    
    // Initialization methods
    void initializeServices();
    void setupMainLayout();
    void setupEventListeners();
    void setupRealTimeUpdates();
    
    // CSS and styling methods - ADDED
    void setupBootstrapTheme();
    void addCustomCSS();
    
    // Event handlers
    void onTestButtonClicked();
    void onPeriodicUpdate();
    
    // Utility methods
    void logApplicationStart();
    void updateStatus(const std::string& message);
};

/**
 * @brief Application factory function
 * @param env Wt web environment
 * @return Unique pointer to the created application
 */
std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env);

#endif // RESTAURANTPOSAPP_H
