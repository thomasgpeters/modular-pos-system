//============================================================================
// include/ui/factories/UIComponentFactory.hpp - CORRECTED Header
//============================================================================

#ifndef UICOMPONENTFACTORY_H
#define UICOMPONENTFACTORY_H

#include "../../services/POSService.hpp"
#include "../../services/ThemeService.hpp"
#include "../../core/ConfigurationManager.hpp"
#include "../../events/EventManager.hpp"
#include "../../MenuItem.hpp"
#include "../../Order.hpp"
#include "../../PaymentProcessor.hpp"

// Include only the components you actually have implemented
#include "../components/OrderEntryPanel.hpp"
#include "../components/MenuDisplay.hpp"
#include "../components/CurrentOrderDisplay.hpp"
#include "../components/ActiveOrdersDisplay.hpp"
#include "../components/KitchenStatusDisplay.hpp"

// Forward declarations for components
class OrderEntryPanel;
class OrderStatusPanel;
class ThemeSelector;
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
 * @class UIComponentFactory
 * @brief Factory for creating and configuring UI components
 */
class UIComponentFactory {
public:
    UIComponentFactory(std::shared_ptr<POSService> posService,
                      std::shared_ptr<EventManager> eventManager,
                      std::shared_ptr<ConfigurationManager> configManager);
    
    virtual ~UIComponentFactory() = default;
    
    // Component creation methods
    std::unique_ptr<OrderEntryPanel> createOrderEntryPanel();
    std::unique_ptr<OrderStatusPanel> createOrderStatusPanel();
    std::unique_ptr<ThemeSelector> createThemeSelector();
    std::unique_ptr<MenuDisplay> createMenuDisplay();
    std::unique_ptr<CurrentOrderDisplay> createCurrentOrderDisplay();
    std::unique_ptr<ActiveOrdersDisplay> createActiveOrdersDisplay();
    std::unique_ptr<KitchenStatusDisplay> createKitchenStatusDisplay();
    
    // Dialog creation methods
    std::unique_ptr<PaymentDialog> createPaymentDialog(
        std::shared_ptr<Order> order,
        std::function<void(bool success)> callback = nullptr);
    
    std::unique_ptr<PaymentDialog> createPaymentDialog(
        std::shared_ptr<Order> order,
        std::function<void(bool success)> callback,
        bool allowSplitPayments,
        const std::vector<double>& suggestedTips = {0.15, 0.18, 0.20});
    
    std::unique_ptr<CategoryPopover> createCategoryPopover(
        MenuItem::Category category,
        const std::vector<std::shared_ptr<MenuItem>>& items,
        std::function<void(int itemId)> callback = nullptr);
    
    std::unique_ptr<ThemeSelectionDialog> createThemeSelectionDialog(
        std::function<void(const std::string&)> callback = nullptr);
    
    // Service registration
    void registerThemeService(std::shared_ptr<ThemeService> themeService);
    
    // Getters
    std::shared_ptr<ThemeService> getThemeService() const { return themeService_; }
    std::shared_ptr<POSService> getPOSService() const { return posService_; }
    std::shared_ptr<EventManager> getEventManager() const { return eventManager_; }
    std::shared_ptr<ConfigurationManager> getConfigurationManager() const { return configManager_; }

protected:
    // ADDED: Missing method declarations
    void logComponentCreation(const std::string& componentName) const;

private:
    // Core services
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    std::shared_ptr<ConfigurationManager> configManager_;
    
    // Optional services
    std::shared_ptr<ThemeService> themeService_;
    
    // Factory state
    bool initialized_;
    
    // Default configuration values
    std::vector<double> defaultTipSuggestions_;
    
    // Helper methods
    void validateDependencies() const;
    void initializeDefaults();
};

#endif // UICOMPONENTFACTORY_H
