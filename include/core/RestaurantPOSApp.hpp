#ifndef RESTAURANTPOSAPP_H
#define RESTAURANTPOSAPP_H

#include "../services/POSService.hpp"
#include "../services/ThemeService.hpp"
#include "../events/EventManager.hpp"

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
 * @file RestaurantPOSApp.hpp (Enhanced with Theme Management)
 * @brief Main application with integrated theme switching capability
 * 
 * @author Restaurant POS Team
 * @version 2.2.0 - Updated with ThemeService integration
 */

/**
 * @class RestaurantPOSApp
 * @brief Main application orchestrator with enhanced theming support
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
    
    // UI components
    Wt::WContainerWidget* mainContainer_;
    Wt::WContainerWidget* orderControlsContainer_;
    Wt::WText* statusText_;

    // Change this line - use raw pointer since it's managed by the widget tree now
    Wt::WTimer* updateTimer_;
    
    // Header and theme controls
    Wt::WContainerWidget* headerContainer_;
    Wt::WContainerWidget* themeControlsContainer_;
    Wt::WComboBox* themeSelector_;
    Wt::WPushButton* themeToggleButton_;
    
    // New order controls
    Wt::WGroupBox* newOrderGroup_;
    Wt::WComboBox* tableIdentifierCombo_;
    Wt::WPushButton* newOrderButton_;
    Wt::WText* currentOrderStatusText_;
    
    // Application status controls
    Wt::WContainerWidget* statusControlsContainer_;
    Wt::WPushButton* refreshButton_;
    Wt::WText* systemStatusText_;
    
    // Initialization methods
    void initializeServices();
    void setupMainLayout();
    void setupHeaderWithThemeControls();
    void setupNewOrderControls();
    void setupStatusControls();
    void setupEventListeners();
    void setupRealTimeUpdates();
    
    // Theme management methods
    void initializeThemeService();
    void setupThemeControls();
    void setupThemeEventHandlers();
    void onThemeChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme);
    void onThemeToggleClicked();
    void onThemeSelectorChanged();
    void updateThemeControls();
    
    // CSS and styling methods
    void setupBootstrapTheme();
    void addCustomCSS();
    void addThemeSpecificCSS();
    void setupMetaTags();
    void applyComponentStyling();
    void applyThemeToComponents();
    
    // Event handlers
    void onNewOrderButtonClicked();
    void onTableIdentifierChanged();
    void onRefreshButtonClicked();
    void onPeriodicUpdate();
    
    // Order management event handlers
    void onOrderCreated(std::shared_ptr<Order> order);
    void onOrderModified(std::shared_ptr<Order> order);
    void onCurrentOrderChanged();
    
    // UI update methods
    void updateCurrentOrderStatus();
    void updateSystemStatus();
    void updateOrderControls();
    
    // Helper methods
    void populateTableIdentifierCombo();
    void logApplicationStart();
    void updateStatus(const std::string& message);
    std::string getCurrentTableIdentifier() const;
    bool isValidTableSelection() const;
    void showOrderCreationStatus(bool success, const std::string& tableIdentifier);
    void resetOrderControls();
    
    // Validation methods
    bool validateNewOrderInput() const;
    void showValidationError(const std::string& message);
    
    // Status formatting methods
    std::string formatOrderStatus(std::shared_ptr<Order> order) const;
    std::string formatSystemStatus() const;
    std::string getOrderTypeIcon(const std::string& tableIdentifier) const;
    
    // Table identifier management
    std::vector<std::string> getAvailableTableIdentifiers() const;
    std::string formatTableIdentifierDisplay(const std::string& identifier) const;
    bool isTableIdentifierAvailable(const std::string& identifier) const;
    
    // Theme utility methods
    void createThemeSelector();
    void createThemeToggleButton();
    std::string getCurrentThemeDisplayName() const;
    void applyThemeTransition();
};

/**
 * @brief Application factory function
 * @param env Wt web environment
 * @return Unique pointer to the created application
 */
std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env);

#endif // RESTAURANTPOSAPP_H
