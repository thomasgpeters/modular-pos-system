#ifndef RESTAURANTPOSAPP_ENHANCED_H
#define RESTAURANTPOSAPP_ENHANCED_H

#include "../services/POSService.hpp"
#include "../services/ThemeService.hpp"
#include "../events/EventManager.hpp"
#include "../core/ConfigurationManager.hpp"
#include "../ui/factories/UIComponentFactory.hpp"

// Mode-specific containers
#include "../ui/containers/POSModeContainer.hpp"
#include "../ui/containers/KitchenModeContainer.hpp"

// RestaurantPOSApp common-components
#include "../ui/components/ModeSelector.hpp"
#include "../ui/components/CommonHeader.hpp"
#include "../ui/components/CommonFooter.hpp"

#include "../events/POSEvents.hpp"

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WTimer.h>

#include <memory>

// ============================================================================
// SUMMARY OF CRITICAL CHANGES
// ============================================================================

/*
KEY FIXES APPLIED:

1. WIDGET LIFECYCLE MANAGEMENT:
   - Use removeWidget() instead of clear() to avoid deleting widgets
   - Verify widget operations succeed before proceeding
   - Add proper destruction cleanup

2. SAFETY CHECKS:
   - Validate all pointers before use
   - Check widget indices to verify add/remove operations
   - Add destruction flag to prevent operations during cleanup

3. ERROR RECOVERY:
   - Emergency recovery to POS mode if switching fails
   - Graceful error handling without crashing

4. DEBUGGING:
   - Added widget state debugging method
   - Comprehensive logging of widget operations

The crash was caused by using clear() which DELETES child widgets, 
then trying to add those deleted widgets to containers later.
Using removeWidget() preserves the widgets for reuse.
*/


/**
 * @file RestaurantPOSApp_Enhanced.hpp
 * @brief Enhanced main application with POS/Kitchen mode switching
 * 
 * This enhanced version provides:
 * - POS Mode: Order creation and management
 * - Kitchen Mode: Order preparation and kitchen status
 * - Common header with mode switching and theme controls
 * - Intuitive workflow for restaurant operations
 * 
 * @author Restaurant POS Team
 * @version 3.0.0 - Enhanced with mode switching
 */

/**
 * @class RestaurantPOSApp
 * @brief Enhanced main application with dual operating modes
 */
class RestaurantPOSApp : public Wt::WApplication {
public:
    /**
     * @enum OperatingMode
     * @brief Application operating modes
     */
    enum OperatingMode {
        POS_MODE,       ///< Point of Sale mode for order taking
        KITCHEN_MODE    ///< Kitchen mode for order preparation
    };
    
    /**
     * @brief Constructs the enhanced POS application
     * @param env Wt web environment
     */
    RestaurantPOSApp(const Wt::WEnvironment& env);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~RestaurantPOSApp();
    
    /**
     * @brief Switches to the specified operating mode
     * @param mode Mode to switch to
     */
    void switchMode(OperatingMode mode);
    
    /**
     * @brief Gets the current operating mode
     * @return Current operating mode
     */
    OperatingMode getCurrentMode() const { return currentMode_; }

private:
    // Add this flag to prevent operations during destruction
    bool isDestroying_;

    // Core services
    std::shared_ptr<EventManager> eventManager_;
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<ThemeService> themeService_;
    std::shared_ptr<ConfigurationManager> configManager_;
    std::unique_ptr<UIComponentFactory> componentFactory_;
    
    // Application state
    OperatingMode currentMode_;
    
    // Main layout structure
    Wt::WContainerWidget* mainContainer_;
    Wt::WVBoxLayout* mainLayout_;
    
    // Common components
    CommonHeader* commonHeader_;
    CommonFooter* commonFooter_;
    Wt::WContainerWidget* modeContainer_;
    
    // Mode-specific containers
    POSModeContainer* posModeContainer_;
    KitchenModeContainer* kitchenModeContainer_;
    
    // Real-time updates
    std::unique_ptr<Wt::WTimer> updateTimer_;
    
    // Initialization methods
    void initializeServices();
    void initializeComponentFactory();
    void setupMainLayout();
    void createCommonComponents();
    void createModeContainers();
    void setupEventListeners();
    void setupRealTimeUpdates();
    
    // Mode switching
    void showPOSMode();
    void showKitchenMode();
    void hideModeContainers();
    
    // FIXED: Method signature for onModeChanged
    void onModeChanged(OperatingMode newMode);
    
    // Theme and styling
    void initializeThemeService();
    void setupBootstrapTheme();
    void addCustomCSS();
    void applyModeSpecificStyling();
    
    // Event handlers
    void onPeriodicUpdate();
    void onThemeChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme);
    
    // Utility methods
    void logApplicationStart();
    void debugWidgetState();
    void logModeSwitch(OperatingMode mode);
    std::string getModeDisplayName(OperatingMode mode) const;
};

/**
 * @brief Application factory function
 */
std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env);

#endif // RESTAURANTPOSAPP_ENHANCED_H