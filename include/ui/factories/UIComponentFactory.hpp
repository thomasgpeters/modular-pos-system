#ifndef UICOMPONENTFACTORY_H
#define UICOMPONENTFACTORY_H

#include "../../services/POSService.hpp"
#include "../../services/ThemeService.hpp"
#include "../../core/ConfigurationManager.hpp"
#include "../../services/NotificationService.hpp"
#include "../../events/EventManager.hpp"
#include "../../MenuItem.hpp"
#include "../../Order.hpp"
#include "../../PaymentProcessor.hpp"

#include "../components/OrderEntryPanel.hpp"
#include "../components/OrderStatusPanel.hpp"
#include "../components/ThemeSelector.hpp"
#include "../components/MenuDisplay.hpp"
#include "../components/CurrentOrderDisplay.hpp"
#include "../components/ActiveOrdersDisplay.hpp"
#include "../components/KitchenStatusDisplay.hpp"
#include "../dialogs/PaymentDialog.hpp"
#include "../dialogs/CategoryPopover.hpp"
#include "../dialogs/ThemeSelectionDialog.hpp"

// Forward declarations for components
class MenuDisplay;
class CurrentOrderDisplay;
class ActiveOrdersDisplay;
class KitchenStatusDisplay;
class PaymentDialog;
class CategoryPopover;
class ThemeSelectionDialog;

#include <memory>
#include <map>
#include <vector>
#include <any>
#include <functional>

/**
 * @file UIComponentFactory.hpp
 * @brief Factory for creating UI components with dependency injection
 * 
 * This factory centralizes the creation of UI components and handles
 * dependency injection, making the system more modular and testable.
 * Updated version with proper method signatures and implementations.
 * 
 * @author Restaurant POS Team
 * @version 2.2.0
 */

/**
 * @class UIComponentFactory
 * @brief Factory for creating and configuring UI components
 * 
 * The UIComponentFactory provides centralized creation of UI components
 * with proper dependency injection. This promotes loose coupling and
 * makes the system easier to test and maintain.
 */
class UIComponentFactory {
public:
    /**
     * @brief Constructs the UI component factory
     * @param posService POS service for business logic
     * @param eventManager Event manager for component communication
     * @param configManager Configuration manager for settings
     */
    UIComponentFactory(std::shared_ptr<POSService> posService,
                      std::shared_ptr<EventManager> eventManager,
                      std::shared_ptr<ConfigurationManager> configManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~UIComponentFactory() = default;
    
    // =================================================================
    // Component Creation Methods (Currently Implemented)
    // =================================================================
    
    /**
     * @brief Creates an order entry panel
     * @return Unique pointer to the created component
     */
    std::unique_ptr<OrderEntryPanel> createOrderEntryPanel();
    
    /**
     * @brief Creates an order status panel
     * @return Unique pointer to the created component
     */
    std::unique_ptr<OrderStatusPanel> createOrderStatusPanel();
    
    /**
     * @brief Creates a theme selector component
     * @return Unique pointer to the created component (nullptr if no ThemeService)
     */
    std::unique_ptr<ThemeSelector> createThemeSelector();
    
    // =================================================================
    // Component Creation Methods (Now Implemented)
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
    
    // =================================================================
    // Dialog Creation Methods - FIXED: Corrected method signatures
    // =================================================================
    
    /**
     * @brief Creates a payment dialog with simple success callback
     * @param order Order to process payment for
     * @param callback Simple success/failure callback
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<PaymentDialog> createPaymentDialog(
        std::shared_ptr<Order> order,
        std::function<void(bool success)> callback = nullptr);
    
    /**
     * @brief Creates a payment dialog with extended configuration
     * @param order Order to process payment for
     * @param callback Simple success/failure callback
     * @param allowSplitPayments Whether to allow split payments
     * @param suggestedTips Suggested tip percentages
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<PaymentDialog> createPaymentDialog(
        std::shared_ptr<Order> order,
        std::function<void(bool success)> callback,
        bool allowSplitPayments,
        const std::vector<double>& suggestedTips = {0.15, 0.18, 0.20});
    
    /**
     * @brief Creates a category popover with item ID callback
     * @param category Menu category to display
     * @param items Menu items in the category
     * @param callback Item selection callback (receives item ID)
     * @return Unique pointer to the created popover
     */
    std::unique_ptr<CategoryPopover> createCategoryPopover(
        MenuItem::Category category,
        const std::vector<std::shared_ptr<MenuItem>>& items,
        std::function<void(int itemId)> callback = nullptr);
    
    /**
     * @brief Creates a category popover with extended configuration
     * @param category Menu category to display
     * @param items Menu items in the category
     * @param callback Item selection callback (receives item ID)
     * @param maxColumns Maximum columns for item display
     * @param showDescriptions Whether to show item descriptions
     * @return Unique pointer to the created popover
     */
    std::unique_ptr<CategoryPopover> createCategoryPopover(
        MenuItem::Category category,
        const std::vector<std::shared_ptr<MenuItem>>& items,
        std::function<void(int itemId)> callback,
        int maxColumns,
        bool showDescriptions = true);
    
    /**
     * @brief Creates a theme selection dialog with string callback
     * @param callback Theme selection callback (receives theme ID string)
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<ThemeSelectionDialog> createThemeSelectionDialog(
        std::function<void(const std::string&)> callback = nullptr);
    
    /**
     * @brief Creates a theme selection dialog with ThemeService enum callback
     * @param callback Theme selection callback (receives ThemeService::Theme enum)
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<ThemeSelectionDialog> createThemeSelectionDialog(
        std::function<void(ThemeService::Theme theme)> callback);
    
    /**
     * @brief Creates a theme selection dialog with configuration options
     * @param callback Theme selection callback (receives ThemeService::Theme enum)
     * @param showPreviews Whether to show theme previews
     * @param showDescriptions Whether to show theme descriptions
     * @param maxThemes Maximum number of themes to display
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<ThemeSelectionDialog> createThemeSelectionDialog(
        std::function<void(ThemeService::Theme theme)> callback,
        bool showPreviews,
        bool showDescriptions = true,
        int maxThemes = 10);
    
    /**
     * @brief Creates a configured theme selection dialog
     * @param callback Theme selection callback (receives ThemeService::Theme enum)
     * @param config Configuration options for the dialog
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<ThemeSelectionDialog> createConfiguredThemeSelectionDialog(
        std::function<void(ThemeService::Theme theme)> callback,
        const std::map<std::string, std::any>& config);
    
    // =================================================================
    // Service Registration
    // =================================================================
    
    /**
     * @brief Registers the theme service
     * @param themeService Theme service instance
     */
    void registerThemeService(std::shared_ptr<ThemeService> themeService);
    
    /**
     * @brief Registers the notification service
     * @param notificationService Notification service instance
     */
    void registerNotificationService(std::shared_ptr<NotificationService> notificationService);
    
    /**
     * @brief Gets the current theme service
     * @return Shared pointer to theme service (may be null)
     */
    std::shared_ptr<ThemeService> getThemeService() const { return themeService_; }
    
    /**
     * @brief Gets the current notification service
     * @return Shared pointer to notification service (may be null)
     */
    std::shared_ptr<NotificationService> getNotificationService() const { return notificationService_; }
    
    /**
     * @brief Gets the POS service
     * @return Shared pointer to POS service
     */
    std::shared_ptr<POSService> getPOSService() const { return posService_; }
    
    /**
     * @brief Gets the event manager
     * @return Shared pointer to event manager
     */
    std::shared_ptr<EventManager> getEventManager() const { return eventManager_; }
    
    /**
     * @brief Gets the configuration manager
     * @return Shared pointer to configuration manager
     */
    std::shared_ptr<ConfigurationManager> getConfigurationManager() const { return configManager_; }

protected:
    /**
     * @brief Configures a component with common dependencies
     * @tparam T Component type
     * @param component Component to configure
     */
    template<typename T>
    void configureComponent(T* component);
    
    /**
     * @brief Applies configuration settings to a component
     * @tparam T Component type
     * @param component Component to configure
     */
    template<typename T>
    void applyComponentConfiguration(T* component);
    
    /**
     * @brief Applies theme configuration to a dialog
     * @param dialog Dialog to configure (as void* for flexibility)
     * @param showPreviews Whether to show previews
     * @param showDescriptions Whether to show descriptions
     * @param maxThemes Maximum themes to display
     */
    void configureThemeDialog(void* dialog, 
                             bool showPreviews = true,
                             bool showDescriptions = true, 
                             int maxThemes = 10);

private:
    // Core services
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    std::shared_ptr<ConfigurationManager> configManager_;
    
    // Optional services (registered separately)
    std::shared_ptr<ThemeService> themeService_;
    std::shared_ptr<NotificationService> notificationService_;
    
    // Factory state
    bool initialized_;
    
    // Default configuration values
    bool defaultShowPreviews_;
    bool defaultShowDescriptions_;
    int defaultMaxThemes_;
    std::vector<double> defaultTipSuggestions_;
    
    // Helper methods
    void validateDependencies() const;
    void logComponentCreation(const std::string& componentName) const;
    void initializeDefaults();
};

#endif // UICOMPONENTFACTORY_H
