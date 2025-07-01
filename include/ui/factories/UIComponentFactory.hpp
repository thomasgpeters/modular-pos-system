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

/**
 * @file UIComponentFactory.hpp
 * @brief Factory for creating UI components with dependency injection
 * 
 * This factory centralizes the creation of UI components and handles
 * dependency injection, making the system more modular and testable.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
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
    // Component Creation Methods
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
     * @return Unique pointer to the created component
     */
    std::unique_ptr<ThemeSelector> createThemeSelector();
    
    // =================================================================
    // Dialog Creation Methods
    // =================================================================
    
    /**
     * @brief Creates a payment dialog
     * @param order Order to process payment for
     * @param callback Payment completion callback
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<PaymentDialog> createPaymentDialog(
        std::shared_ptr<Order> order,
        PaymentDialog::PaymentCallback callback = nullptr);
    
    /**
     * @brief Creates a category popover
     * @param category Menu category to display
     * @param items Menu items in the category
     * @param callback Item selection callback
     * @return Unique pointer to the created popover
     */
    std::unique_ptr<CategoryPopover> createCategoryPopover(
        MenuItem::Category category,
        const std::vector<std::shared_ptr<MenuItem>>& items,
        CategoryPopover::ItemSelectionCallback callback = nullptr);
    
    /**
     * @brief Creates a theme selection dialog
     * @param callback Theme selection callback
     * @return Unique pointer to the created dialog
     */
    std::unique_ptr<ThemeSelectionDialog> createThemeSelectionDialog(
        std::function<void(const std::string&)> callback = nullptr);
    
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
    
    // Helper methods
    void validateDependencies() const;
    void logComponentCreation(const std::string& componentName) const;
};

#endif // UICOMPONENTFACTORY_H
