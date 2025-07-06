#ifndef RESTAURANTPOSAPP_H
#define RESTAURANTPOSAPP_H

#include "../services/POSService.hpp"
#include "../services/ThemeService.hpp"
#include "../events/EventManager.hpp"
#include "../core/ConfigurationManager.hpp"
#include "../ui/factories/UIComponentFactory.hpp"

// Include all the component headers
#include "../ui/components/OrderEntryPanel.hpp"
#include "../ui/components/OrderStatusPanel.hpp"
#include "../ui/components/MenuDisplay.hpp"
#include "../ui/components/CurrentOrderDisplay.hpp"

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTimer.h>
#include <Wt/WEnvironment.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>

#include <memory>

/**
 * @file RestaurantPOSApp.hpp (Flat Component Architecture)
 * @brief Main application with flattened component hierarchy
 * 
 * @author Restaurant POS Team
 * @version 2.3.0 - Flattened component architecture
 */

/**
 * @class RestaurantPOSApp
 * @brief Main application with individual components created via factory
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
    std::shared_ptr<ThemeService> themeService_;
    std::shared_ptr<ConfigurationManager> configManager_;
    
    // UI Component Factory
    std::unique_ptr<UIComponentFactory> componentFactory_;
    
    // Main UI containers
    Wt::WContainerWidget* mainContainer_;
    Wt::WContainerWidget* headerContainer_;
    Wt::WContainerWidget* contentContainer_;
    Wt::WContainerWidget* statusBarContainer_;
    
    // Header components
    Wt::WContainerWidget* themeControlsContainer_;
    Wt::WComboBox* themeSelector_;
    Wt::WPushButton* themeToggleButton_;
    
    // Individual components (raw pointers - Wt manages lifetime)
    OrderEntryPanel* orderEntryPanel_;
    MenuDisplay* menuDisplay_;
    CurrentOrderDisplay* currentOrderDisplay_;
    OrderStatusPanel* orderStatusPanel_;
    
    // Status and timer
    Wt::WText* statusText_;
    Wt::WTimer* updateTimer_;
    
    // Initialization methods
    void initializeServices();
    void initializeComponentFactory();
    void setupMainLayout();
    void createAllComponents();
    void setupHeaderWithThemeControls();
    void setupContentLayout();
    void setupStatusBar();
    void setupEventListeners();
    void setupRealTimeUpdates();
    
    // Component creation methods
    void createOrderEntryPanel();
    void createMenuDisplay();
    void createCurrentOrderDisplay();
    void createOrderStatusPanel();
    
    // Theme management methods
    void initializeThemeService();
    void setupThemeControls();
    void setupThemeEventHandlers();
    void onThemeChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme);
    void onThemeToggleClicked();
    void onThemeSelectorChanged();
    void updateThemeControls();
    void createThemeSelector();
    void createThemeToggleButton();
    
    // CSS and styling methods
    void setupBootstrapTheme();
    void addCustomCSS();
    void addThemeSpecificCSS();
    void setupMetaTags();
    void applyComponentStyling();
    void applyThemeToComponents();
    void applyThemeTransition();
    
    // Event handlers
    void onPeriodicUpdate();
    void onOrderCreated(std::shared_ptr<Order> order);
    void onOrderModified(std::shared_ptr<Order> order);
    void onCurrentOrderChanged();
    
    // Utility methods
    void logApplicationStart();
    void updateStatus(const std::string& message);
    std::string getCurrentThemeDisplayName() const;
    void validateComponents() const;
};

/**
 * @brief Application factory function
 * @param env Wt web environment
 * @return Unique pointer to the created application
 */
std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env);

#endif // RESTAURANTPOSAPP_H
