#ifndef UICOMPONENTFACTORY_H
#define UICOMPONENTFACTORY_H

#include "../../services/POSService.hpp"
#include "../../services/ThemeService.hpp"
#include "../../services/NotificationService.hpp"
#include "../../core/ConfigurationManager.hpp"
#include "../../events/EventManager.hpp"

#include "../components/OrderEntryPanel.hpp"
#include "../components/OrderStatusPanel.hpp"
#include "../components/MenuDisplay.hpp"
#include "../components/CurrentOrderDisplay.hpp"
#include "../components/ActiveOrdersDisplay.hpp"
#include "../components/KitchenStatusDisplay.hpp"
#include "../components/ThemeSelector.hpp"

#include "../dialogs/PaymentDialog.hpp"
#include "../dialogs/CategoryPopover.hpp"
#include "../dialogs/ThemeSelectionDialog.hpp"

#include <memory>
#include <string>
#include <unordered_map>

/**
 * @file UIComponentFactory.hpp
 * @brief Factory for creating UI components with proper dependency injection
 * 
 * This factory centralizes the creation of UI components, ensuring proper
 * dependency injection and consistent configuration across the application.
 * It implements the Factory pattern for UI component creation.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class UIComponentFactory
 * @brief Factory for creating UI components with dependency injection
 * 
 * The UIComponentFactory provides centralized creation of UI components
 * with proper dependency injection. It ensures all components receive
 * the correct service instances and configuration settings.
 */
class UIComponentFactory {
public:
    /**
     * @brief Constructs the UI component factory
     * @param posService Shared POS service instance
     * @param eventManager Shared event manager instance
     * @param configManager Shared configuration manager instance
     */
    UIComponentFactory(std::shared_ptr<POSService> posService,
                      std::shared_ptr<EventManager> eventManager,
                      std::shared_ptr<ConfigurationManager> configManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~UIComponentFactory() = default;
    
    /**
     * @brief Sets the theme service instance
     * @param themeService Shared theme service instance
     */
    void setThemeService(std::shared_ptr<ThemeService> themeService);
    
    /**
     * @brief Sets the notification service instance
     * @param notificationService Shared notification service instance
     */
    void setNotificationService(std::shared_ptr<NotificationService> notificationService);
    
    // =================================================================
    // Main Component Creation
    // =================================================================
    
    /**
     * @brief Creates an order entry panel
     * @return Unique pointer to the created panel
     */
    std::unique_ptr<OrderEntryPanel> createOrderEntryPanel();
    
    /**
     * @brief Creates an order status panel
     * @return Unique pointer to the created panel
     */
    std::unique_ptr<OrderStatusPanel> createOrderStatusPanel();
    
    // =================================================================
    // Individual Component Creation
    // =================================================================
    
    /**
     * @brief Creates a menu display component
     * @return Unique pointer to the created component
     */
    std::unique_ptr<MenuDisplay> createMenuDisplay();
    
    /**
     * @brief Creates a current order display component
     * @return Unique pointer to the created component
     */
    std::unique_ptr<CurrentOrderDisplay> createCurrentOrderDisplay();
    
    /**
     * @brief Creates an active orders display component
     * @return Unique pointer to the created component
     */
    std::unique_ptr<ActiveOrdersDisplay> createActiveOrdersDisplay();
    
    /**
     * @brief Creates a kitchen status display component
     * @return Unique pointer to the created component
     */
    std::unique_ptr<KitchenStatusDisplay> createKitchenStatusDisplay();
    
    /**
     * @brief Creates a theme selector component
     * @param mode Display mode for the theme selector
     * @return Unique pointer to the created component
     */
    std::unique_ptr<ThemeSelector> createThemeSelector(
        ThemeSelector::DisplayMode mode = ThemeSelector::COMPACT);
    
    // =================================================================
    // Dialog Creation
    // =================================================================
    
    /**
     * @brief Creates a payment dialog
     * @param order Order to process payment for
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<PaymentDialog> createPaymentDialog(std::shared_ptr<Order> order);
    
    /**
     * @brief Creates a category popover dialog
     * @param category Category to display
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<CategoryPopover> createCategoryPopover(MenuItem::Category category);
    
    /**
     * @brief Creates a theme selection dialog
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<ThemeSelectionDialog> createThemeSelectionDialog();
    
    // =================================================================
    // Configuration and Customization
    // =================================================================
    
    /**
     * @brief Sets default styling for created components
     * @param componentType Type of component to style
     * @param styleClasses CSS classes to apply
     */
    void setDefaultStyling(const std::string& componentType, 
                          const std::vector<std::string>& styleClasses);
    
    /**
     * @brief Gets default styling for a component type
     * @param componentType Type of component
     * @return Vector of CSS classes
     */
    std::vector<std::string> getDefaultStyling(const std::string& componentType) const;
    
    /**
     * @brief Enables or disables component features
     * @param featureName Name of the feature
     * @param enabled True to enable, false to disable
     */
    void setFeatureEnabled(const std::string& featureName, bool enabled);
    
    /**
     * @brief Checks if a feature is enabled
     * @param featureName Name of the feature
     * @return True if enabled, false otherwise
     */
    bool isFeatureEnabled(const std::string& featureName) const;
    
    // =================================================================
    // Component Registry and Management
    // =================================================================
    
    /**
     * @brief Registers a created component for lifecycle management
     * @param componentId Unique identifier for the component
     * @param component Pointer to the component
     */
    void registerComponent(const std::string& componentId, Wt::WWidget* component);
    
    /**
     * @brief Unregisters a component
     * @param componentId Unique identifier for the component
     */
    void unregisterComponent(const std::string& componentId);
    
    /**
     * @brief Gets a registered component by ID
     * @param componentId Unique identifier for the component
     * @return Pointer to the component, or nullptr if not found
     */
    Wt::WWidget* getComponent(const std::string& componentId) const;
    
    /**
     * @brief Gets all registered component IDs
     * @return Vector of component IDs
     */
    std::vector<std::string> getRegisteredComponentIds() const;
    
    /**
     * @brief Refreshes all registered components
     */
    void refreshAllComponents();

protected:
    /**
     * @brief Applies default styling to a component
     * @param component Component to style
     * @param componentType Type of component for styling lookup
     */
    void applyDefaultStyling(Wt::WWidget* component, const std::string& componentType);
    
    /**
     * @brief Validates that required services are available
     * @return True if all required services are available
     */
    bool validateServices() const;
    
    /**
     * @brief Logs component creation
     * @param componentType Type of component created
     * @param componentId Optional component ID
     */
    void logComponentCreation(const std::string& componentType, 
                             const std::string& componentId = "") const;

private:
    // Service dependencies
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    std::shared_ptr<ConfigurationManager> configManager_;
    std::shared_ptr<ThemeService> themeService_;
    std::shared_ptr<NotificationService> notificationService_;
    
    // Component registry
    std::unordered_map<std::string, Wt::WWidget*> registeredComponents_;
    
    // Default styling configuration
    std::unordered_map<std::string, std::vector<std::string>> defaultStyling_;
    
    // Feature flags
    std::unordered_map<std::string, bool> featureFlags_;
    
    // Component creation counters for unique IDs
    mutable std::unordered_map<std::string, int> componentCounters_;
    
    // Helper methods
    std::string generateComponentId(const std::string& componentType) const;
    void initializeDefaultStyling();
    void initializeFeatureFlags();
    void setupComponentDefaults();
    
    // Component configuration methods
    void configureMenuDisplay(MenuDisplay* menuDisplay);
    void configureCurrentOrderDisplay(CurrentOrderDisplay* currentOrderDisplay);
    void configureThemeSelector(ThemeSelector* themeSelector);
    
    // Constants
    static constexpr const char* COMPONENT_ORDER_ENTRY = "order_entry_panel";
    static constexpr const char* COMPONENT_ORDER_STATUS = "order_status_panel";
    static constexpr const char* COMPONENT_MENU_DISPLAY = "menu_display";
    static constexpr const char* COMPONENT_CURRENT_ORDER = "current_order_display";
    static constexpr const char* COMPONENT_ACTIVE_ORDERS = "active_orders_display";
    static constexpr const char* COMPONENT_KITCHEN_STATUS = "kitchen_status_display";
    static constexpr const char* COMPONENT_THEME_SELECTOR = "theme_selector";
    static constexpr const char* DIALOG_PAYMENT = "payment_dialog";
    static constexpr const char* DIALOG_CATEGORY = "category_popover";
    static constexpr const char* DIALOG_THEME_SELECTION = "theme_selection_dialog";
};

#endif // UICOMPONENTFACTORY_H
