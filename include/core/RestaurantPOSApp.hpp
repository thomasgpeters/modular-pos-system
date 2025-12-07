#ifndef RESTAURANTPOSAPP_HPP
#define RESTAURANTPOSAPP_HPP

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WTimer.h>
#include <memory>

// Include all necessary headers
#include "../services/POSService.hpp"
#include "../services/EnhancedPOSService.hpp"
#include "../services/ThemeService.hpp"
#include "../services/NotificationService.hpp"
#include "../services/LLMQueryService.hpp"
#include "../events/EventManager.hpp"
#include "../events/POSEvents.hpp"
#include "../core/ConfigurationManager.hpp"
#include "../api/APIServiceFactory.hpp"
#include "../utils/Logging.hpp"
#include "../utils/LoggingUtils.hpp"

// UI Components
#include "../ui/components/CommonHeader.hpp"
#include "../ui/components/CommonFooter.hpp"
#include "../ui/components/ModeSelector.hpp"
#include "../ui/containers/POSModeContainer.hpp"
#include "../ui/containers/KitchenModeContainer.hpp"
#include "../ui/factories/UIComponentFactory.hpp"

/**
 * @file RestaurantPOSApp.hpp
 * @brief Main application class for the Restaurant POS System
 * 
 * This class serves as the main entry point for the Restaurant POS application,
 * managing the overall application lifecycle, theme system, and mode switching
 * between POS and Kitchen displays.
 * 
 * @author Restaurant POS Team
 * @version 2.1.0 - Enhanced CSS Loading and Theme Management
 */

/**
 * @class RestaurantPOSApp
 * @brief Main application class that extends Wt::WApplication
 * 
 * RestaurantPOSApp is the central controller for the Restaurant POS system,
 * managing UI modes, theme switching, service coordination, and event handling.
 * It provides a modular architecture with decoupled CSS theme system.
 */
class RestaurantPOSApp : public Wt::WApplication {
public:
    /**
     * @enum OperatingMode
     * @brief Defines the operating modes for the POS system
     */
    enum OperatingMode {
        POS_MODE,      ///< Point of Sale mode for order taking
        KITCHEN_MODE   ///< Kitchen display mode for order preparation
    };
    
    /**
     * @brief Constructs the main application
     * @param env Wt environment containing request information
     */
    explicit RestaurantPOSApp(const Wt::WEnvironment& env);
    
    /**
     * @brief Virtual destructor to ensure proper cleanup
     */
    virtual ~RestaurantPOSApp();
    
    /**
     * @brief Switches the application between operating modes
     * @param mode The operating mode to switch to
     */
    void switchMode(OperatingMode mode);
    
    /**
     * @brief Gets the current operating mode
     * @return Current OperatingMode
     */
    OperatingMode getCurrentMode() const { return currentMode_; }
    
    /**
     * @brief Shows the POS mode interface
     * @deprecated Use switchMode(POS_MODE) instead
     */
    void showPOSMode();
    
    /**
     * @brief Shows the Kitchen mode interface
     * @deprecated Use switchMode(KITCHEN_MODE) instead
     */
    void showKitchenMode();

private:
    // =================================================================
    // Core Services and Management
    // =================================================================
    
    /**
     * @brief Initializes all core services
     * 
     * Sets up event manager, configuration manager, POS service,
     * theme service, and API connections in the correct order.
     */
    void initializeServices();
    
    /**
     * @brief Initializes the UI component factory
     * 
     * Creates the factory for generating UI components and registers
     * the theme service with it for proper styling.
     */
    void initializeComponentFactory();
    
    /**
     * @brief Initializes the theme service and applies default theme
     *
     * Loads theme preferences from storage and applies the current
     * theme to the application interface.
     */
    void initializeThemeService();

    /**
     * @brief Initializes the LLM Query Service for business demographics
     *
     * Sets up the LLM service based on configuration settings,
     * enabling AI-powered business demographic queries based on geolocation.
     */
    void initializeLLMService();
    
    // =================================================================
    // CSS Loading and Theme Management - NEW METHODS
    // =================================================================
    
    /**
     * @brief Sets up Bootstrap theme and loads all CSS
     * 
     * Configures the Bootstrap 5 theme and loads framework, component,
     * and theme CSS files in the correct order for proper styling.
     */
    void setupBootstrapTheme();
    
    /**
     * @brief Loads framework CSS files
     * 
     * Loads base CSS files including Bootstrap CDN, base.css, 
     * bootstrap-custom.css, pos-layout.css, typography.css, and utilities.css.
     */
    void loadFrameworkCSS();
    
    /**
     * @brief Loads component-specific CSS files
     * 
     * Loads CSS files for POS components, menu components, order components,
     * kitchen components, payment components, modals, buttons, tables, and forms.
     */
    void loadComponentCSS();
    
    /**
     * @brief Loads theme CSS files
     * 
     * Loads all available theme CSS files including light, dark, colorful,
     * restaurant, and high-contrast themes.
     */
    void loadThemeCSS();
    
    /**
     * @brief Applies the current theme to DOM elements
     * 
     * Removes existing theme classes and applies the current theme class
     * to the root container and main application container.
     */
    void applyCurrentTheme();
    
    /**
     * @brief Adds custom CSS (legacy method)
     * @deprecated Functionality moved to setupBootstrapTheme() and related methods
     */
    void addCustomCSS();
    
    // =================================================================
    // UI Layout and Component Management
    // =================================================================
    
    /**
     * @brief Sets up the main application layout
     * 
     * Creates the main container, applies initial styling, and sets up
     * the primary VBox layout structure.
     */
    void setupMainLayout();
    
    /**
     * @brief Creates common UI components (header/footer)
     * 
     * Creates the common header with mode switching and theme selection,
     * the mode container for dynamic content, and the common footer.
     */
    void createCommonComponents();
    
    /**
     * @brief Creates mode-specific containers
     * 
     * Creates both POS mode and Kitchen mode containers, sets up their
     * initial visibility states, and configures container relationships.
     */
    void createModeContainers();
    
    /**
     * @brief Ensures POS mode is properly loaded by default
     * 
     * Verifies that POS mode is visible and active on application startup,
     * applies proper styling, and refreshes the container content.
     */
    void ensurePOSModeDefault();
    
    /**
     * @brief Enforces layout constraints with inline styles
     * 
     * Applies explicit size constraints to header, content, and footer
     * using both Wt layout properties and inline CSS styles as backup.
     */
    void enforceLayoutConstraints();
    
    /**
     * @brief Hides all mode containers
     * 
     * Helper method to hide both POS and Kitchen mode containers
     * during mode transitions.
     */
    void hideModeContainers();
    
    /**
     * @brief Applies mode-specific styling to the application
     * 
     * Updates CSS classes and page title based on the current operating mode.
     */
    void applyModeSpecificStyling();
    
    // =================================================================
    // Event Handling and Real-time Updates
    // =================================================================
    
    /**
     * @brief Sets up event listeners for the application
     * 
     * Configures listeners for theme changes, mode switches, and other
     * application-wide events.
     */
    void setupEventListeners();
    
    /**
     * @brief Sets up real-time update timers
     * 
     * Configures periodic updates for data refresh while preserving
     * UI component state to maintain responsiveness.
     */
    void setupRealTimeUpdates();
    
    /**
     * @brief Handles periodic application updates
     * 
     * Performs smart data-only updates without recreating UI components
     * to maintain performance and user experience.
     */
    void onPeriodicUpdate();
    
    /**
     * @brief Handles mode change events
     * @param newMode The new operating mode that was switched to
     * 
     * Called after a successful mode switch to perform any mode-specific
     * initialization or cleanup.
     */
    void onModeChanged(OperatingMode newMode);
    
    /**
     * @brief Handles theme change events
     * @param oldTheme The previous theme
     * @param newTheme The new theme that was applied
     * 
     * Publishes theme change events and updates the UI to reflect
     * the new theme selection.
     */
    void onThemeChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme);
    
    // =================================================================
    // Logging and Debugging
    // =================================================================
    
    /**
     * @brief Logs application startup information
     * 
     * Outputs startup banner and configuration information to the log
     * for debugging and monitoring purposes.
     */
    void logApplicationStart();
    
    /**
     * @brief Logs mode switch operations
     * @param mode The mode that was switched to
     * 
     * Records mode switch events in the application log with timing
     * and status information.
     */
    void logModeSwitch(OperatingMode mode);
    
    /**
     * @brief Gets display name for an operating mode
     * @param mode The operating mode
     * @return Human-readable name for the mode
     */
    std::string getModeDisplayName(OperatingMode mode) const;
    
    /**
     * @brief Outputs debug information about widget states
     * 
     * Dumps current widget hierarchy, visibility states, and container
     * relationships to the debug log for troubleshooting.
     */
    void debugWidgetState();
    
    // =================================================================
    // Member Variables
    // =================================================================
    
    // Core Services
    Logger& logger_;                                           ///< Reference to application logger
    std::shared_ptr<EventManager> eventManager_;              ///< Event management system
    std::shared_ptr<ConfigurationManager> configManager_;     ///< Configuration management
    std::shared_ptr<POSService> posService_;                  ///< POS service (may be enhanced)
    std::shared_ptr<ThemeService> themeService_;              ///< Theme management service
    std::shared_ptr<LLMQueryService> llmQueryService_;        ///< LLM query service for business demographics
    
    // UI Management
    std::unique_ptr<UIComponentFactory> componentFactory_;    ///< Factory for UI components
    
    // Application State
    bool isDestroying_;                                        ///< Flag to prevent operations during destruction
    OperatingMode currentMode_;                                ///< Current operating mode
    
    // Layout Components
    Wt::WContainerWidget* mainContainer_;                     ///< Main application container
    Wt::WVBoxLayout* mainLayout_;                             ///< Primary layout manager
    
    // Common UI Components
    CommonHeader* commonHeader_;                               ///< Application header with navigation
    CommonFooter* commonFooter_;                              ///< Application footer with status
    
    // Mode Containers
    Wt::WContainerWidget* modeContainer_;                     ///< Container for mode-specific content
    POSModeContainer* posModeContainer_;                      ///< POS mode interface container
    KitchenModeContainer* kitchenModeContainer_;              ///< Kitchen mode interface container
    
    // Real-time Updates
    std::unique_ptr<Wt::WTimer> updateTimer_;                 ///< Timer for periodic updates
};

// =================================================================
// Application Factory Function
// =================================================================

/**
 * @brief Creates a new instance of the Restaurant POS application
 * @param env Wt environment containing request information
 * @return Unique pointer to the new application instance
 * 
 * This function serves as the application factory for the Wt framework,
 * creating new application instances for each user session.
 */
std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env);

#endif // RESTAURANTPOSAPP_HPP
