#include "UIComponentFactory.hpp"

#include <iostream>
#include <algorithm>
#include <sstream>

UIComponentFactory::UIComponentFactory(std::shared_ptr<POSService> posService,
                                      std::shared_ptr<EventManager> eventManager,
                                      std::shared_ptr<ConfigurationManager> configManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , configManager_(configManager)
    , themeService_(nullptr)
    , notificationService_(nullptr) {
    
    initializeDefaultStyling();
    initializeFeatureFlags();
    setupComponentDefaults();
    
    std::cout << "✓ UIComponentFactory initialized" << std::endl;
}

void UIComponentFactory::setThemeService(std::shared_ptr<ThemeService> themeService) {
    themeService_ = themeService;
    std::cout << "ThemeService set in UIComponentFactory" << std::endl;
}

void UIComponentFactory::setNotificationService(std::shared_ptr<NotificationService> notificationService) {
    notificationService_ = notificationService;
    std::cout << "NotificationService set in UIComponentFactory" << std::endl;
}

// =================================================================
// Main Component Creation
// =================================================================

std::unique_ptr<OrderEntryPanel> UIComponentFactory::createOrderEntryPanel() {
    if (!validateServices()) {
        std::cerr << "Error: Required services not available for OrderEntryPanel creation" << std::endl;
        return nullptr;
    }
    
    auto component = std::make_unique<OrderEntryPanel>(posService_, eventManager_);
    auto componentPtr = component.get();
    
    // Apply default styling
    applyDefaultStyling(componentPtr, COMPONENT_ORDER_ENTRY);
    
    // Register component
    std::string componentId = generateComponentId(COMPONENT_ORDER_ENTRY);
    registerComponent(componentId, componentPtr);
    
    logComponentCreation(COMPONENT_ORDER_ENTRY, componentId);
    
    return component;
}

std::unique_ptr<OrderStatusPanel> UIComponentFactory::createOrderStatusPanel() {
    if (!validateServices()) {
        std::cerr << "Error: Required services not available for OrderStatusPanel creation" << std::endl;
        return nullptr;
    }
    
    // Note: OrderStatusPanel implementation would be created here
    // For now, we'll create a placeholder
    auto component = std::make_unique<OrderStatusPanel>(posService_, eventManager_);
    auto componentPtr = component.get();
    
    // Apply default styling
    applyDefaultStyling(componentPtr, COMPONENT_ORDER_STATUS);
    
    // Register component
    std::string componentId = generateComponentId(COMPONENT_ORDER_STATUS);
    registerComponent(componentId, componentPtr);
    
    logComponentCreation(COMPONENT_ORDER_STATUS, componentId);
    
    return component;
}

// =================================================================
// Individual Component Creation
// =================================================================

std::unique_ptr<MenuDisplay> UIComponentFactory::createMenuDisplay() {
    if (!validateServices()) {
        std::cerr << "Error: Required services not available for MenuDisplay creation" << std::endl;
        return nullptr;
    }
    
    auto component = std::make_unique<MenuDisplay>(posService_, eventManager_);
    auto componentPtr = component.get();
    
    // Configure component based on settings
    configureMenuDisplay(componentPtr);
    
    // Apply default styling
    applyDefaultStyling(componentPtr, COMPONENT_MENU_DISPLAY);
    
    // Register component
    std::string componentId = generateComponentId(COMPONENT_MENU_DISPLAY);
    registerComponent(componentId, componentPtr);
    
    logComponentCreation(COMPONENT_MENU_DISPLAY, componentId);
    
    return component;
}

std::unique_ptr<CurrentOrderDisplay> UIComponentFactory::createCurrentOrderDisplay() {
    if (!validateServices()) {
        std::cerr << "Error: Required services not available for CurrentOrderDisplay creation" << std::endl;
        return nullptr;
    }
    
    auto component = std::make_unique<CurrentOrderDisplay>(posService_, eventManager_);
    auto componentPtr = component.get();
    
    // Configure component based on settings
    configureCurrentOrderDisplay(componentPtr);
    
    // Apply default styling
    applyDefaultStyling(componentPtr, COMPONENT_CURRENT_ORDER);
    
    // Register component
    std::string componentId = generateComponentId(COMPONENT_CURRENT_ORDER);
    registerComponent(componentId, componentPtr);
    
    logComponentCreation(COMPONENT_CURRENT_ORDER, componentId);
    
    return component;
}

std::unique_ptr<ActiveOrdersDisplay> UIComponentFactory::createActiveOrdersDisplay() {
    if (!validateServices()) {
        std::cerr << "Error: Required services not available for ActiveOrdersDisplay creation" << std::endl;
        return nullptr;
    }
    
    // Note: ActiveOrdersDisplay implementation would be created here
    // For now, we'll create a placeholder
    auto component = std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_);
    auto componentPtr = component.get();
    
    // Apply default styling
    applyDefaultStyling(componentPtr, COMPONENT_ACTIVE_ORDERS);
    
    // Register component
    std::string componentId = generateComponentId(COMPONENT_ACTIVE_ORDERS);
    registerComponent(componentId, componentPtr);
    
    logComponentCreation(COMPONENT_ACTIVE_ORDERS, componentId);
    
    return component;
}

std::unique_ptr<KitchenStatusDisplay> UIComponentFactory::createKitchenStatusDisplay() {
    if (!validateServices()) {
        std::cerr << "Error: Required services not available for KitchenStatusDisplay creation" << std::endl;
        return nullptr;
    }
    
    // Note: KitchenStatusDisplay implementation would be created here
    // For now, we'll create a placeholder
    auto component = std::make_unique<KitchenStatusDisplay>(posService_, eventManager_);
    auto componentPtr = component.get();
    
    // Apply default styling
    applyDefaultStyling(componentPtr, COMPONENT_KITCHEN_STATUS);
    
    // Register component
    std::string componentId = generateComponentId(COMPONENT_KITCHEN_STATUS);
    registerComponent(componentId, componentPtr);
    
    logComponentCreation(COMPONENT_KITCHEN_STATUS, componentId);
    
    return component;
}

std::unique_ptr<ThemeSelector> UIComponentFactory::createThemeSelector(ThemeSelector::DisplayMode mode) {
    if (!themeService_) {
        std::cerr << "Error: ThemeService not available for ThemeSelector creation" << std::endl;
        return nullptr;
    }
    
    auto component = std::make_unique<ThemeSelector>(themeService_, eventManager_, mode);
    auto componentPtr = component.get();
    
    // Configure component based on settings
    configureThemeSelector(componentPtr);
    
    // Apply default styling
    applyDefaultStyling(componentPtr, COMPONENT_THEME_SELECTOR);
    
    // Register component
    std::string componentId = generateComponentId(COMPONENT_THEME_SELECTOR);
    registerComponent(componentId, componentPtr);
    
    logComponentCreation(COMPONENT_THEME_SELECTOR, componentId);
    
    return component;
}

// =================================================================
// Dialog Creation
// =================================================================

std::unique_ptr<PaymentDialog> UIComponentFactory::createPaymentDialog(std::shared_ptr<Order> order) {
    if (!validateServices()) {
        std::cerr << "Error: Required services not available for PaymentDialog creation" << std::endl;
        return nullptr;
    }
    
    // Note: PaymentDialog implementation would be created here
    // For now, we'll create a placeholder
    auto component = std::make_unique<PaymentDialog>(posService_, eventManager_, order);
    auto componentPtr = component.get();
    
    // Apply default styling
    applyDefaultStyling(componentPtr, DIALOG_PAYMENT);
    
    logComponentCreation(DIALOG_PAYMENT);
    
    return component;
}

std::unique_ptr<CategoryPopover> UIComponentFactory::createCategoryPopover(MenuItem::Category category) {
    if (!validateServices()) {
        std::cerr << "Error: Required services not available for CategoryPopover creation" << std::endl;
        return nullptr;
    }
    
    // Note: CategoryPopover implementation would be created here
    // For now, we'll create a placeholder
    auto component = std::make_unique<CategoryPopover>(posService_, eventManager_, category);
    auto componentPtr = component.get();
    
    // Apply default styling
    applyDefaultStyling(componentPtr, DIALOG_CATEGORY);
    
    logComponentCreation(DIALOG_CATEGORY);
    
    return component;
}

std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createThemeSelectionDialog() {
    if (!themeService_) {
        std::cerr << "Error: ThemeService not available for ThemeSelectionDialog creation" << std::endl;
        return nullptr;
    }
    
    // Note: ThemeSelectionDialog implementation would be created here
    // For now, we'll create a placeholder
    auto component = std::make_unique<ThemeSelectionDialog>(themeService_, eventManager_);
    auto componentPtr = component.get();
    
    // Apply default styling
    applyDefaultStyling(componentPtr, DIALOG_THEME_SELECTION);
    
    logComponentCreation(DIALOG_THEME_SELECTION);
    
    return component;
}

// =================================================================
// Configuration and Customization
// =================================================================

void UIComponentFactory::setDefaultStyling(const std::string& componentType, 
                                          const std::vector<std::string>& styleClasses) {
    defaultStyling_[componentType] = styleClasses;
    std::cout << "Default styling set for " << componentType << std::endl;
}

std::vector<std::string> UIComponentFactory::getDefaultStyling(const std::string& componentType) const {
    auto it = defaultStyling_.find(componentType);
    if (it != defaultStyling_.end()) {
        return it->second;
    }
    return {};
}

void UIComponentFactory::setFeatureEnabled(const std::string& featureName, bool enabled) {
    featureFlags_[featureName] = enabled;
    std::cout << "Feature '" << featureName << "' " << (enabled ? "enabled" : "disabled") << std::endl;
}

bool UIComponentFactory::isFeatureEnabled(const std::string& featureName) const {
    auto it = featureFlags_.find(featureName);
    if (it != featureFlags_.end()) {
        return it->second;
    }
    
    // Check configuration manager for feature flags
    if (configManager_) {
        return configManager_->isFeatureEnabled(featureName);
    }
    
    return false;
}

// =================================================================
// Component Registry and Management
// =================================================================

void UIComponentFactory::registerComponent(const std::string& componentId, Wt::WWidget* component) {
    registeredComponents_[componentId] = component;
    std::cout << "Component registered: " << componentId << std::endl;
}

void UIComponentFactory::unregisterComponent(const std::string& componentId) {
    auto it = registeredComponents_.find(componentId);
    if (it != registeredComponents_.end()) {
        registeredComponents_.erase(it);
        std::cout << "Component unregistered: " << componentId << std::endl;
    }
}

Wt::WWidget* UIComponentFactory::getComponent(const std::string& componentId) const {
    auto it = registeredComponents_.find(componentId);
    return (it != registeredComponents_.end()) ? it->second : nullptr;
}

std::vector<std::string> UIComponentFactory::getRegisteredComponentIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : registeredComponents_) {
        ids.push_back(pair.first);
    }
    return ids;
}

void UIComponentFactory::refreshAllComponents() {
    std::cout << "Refreshing all registered components..." << std::endl;
    
    for (const auto& pair : registeredComponents_) {
        auto component = pair.second;
        if (!component) continue;
        
        // Try to cast to known component types and call their refresh methods
        if (auto orderEntry = dynamic_cast<OrderEntryPanel*>(component)) {
            orderEntry->refresh();
        } else if (auto orderStatus = dynamic_cast<OrderStatusPanel*>(component)) {
            orderStatus->refresh();
        } else if (auto menuDisplay = dynamic_cast<MenuDisplay*>(component)) {
            menuDisplay->refresh();
        } else if (auto currentOrder = dynamic_cast<CurrentOrderDisplay*>(component)) {
            currentOrder->refresh();
        } else if (auto activeOrders = dynamic_cast<ActiveOrdersDisplay*>(component)) {
            activeOrders->refresh();
        } else if (auto kitchenStatus = dynamic_cast<KitchenStatusDisplay*>(component)) {
            kitchenStatus->refresh();
        } else if (auto themeSelector = dynamic_cast<ThemeSelector*>(component)) {
            themeSelector->refresh();
        }
    }
    
    std::cout << "✓ All components refreshed" << std::endl;
}

// =================================================================
// Protected Helper Methods
// =================================================================

void UIComponentFactory::applyDefaultStyling(Wt::WWidget* component, const std::string& componentType) {
    if (!component) return;
    
    auto styleClasses = getDefaultStyling(componentType);
    for (const auto& styleClass : styleClasses) {
        component->addStyleClass(styleClass);
    }
    
    if (!styleClasses.empty()) {
        std::cout << "Applied default styling to " << componentType << std::endl;
    }
}

bool UIComponentFactory::validateServices() const {
    if (!posService_) {
        std::cerr << "POSService is required but not available" << std::endl;
        return false;
    }
    
    if (!eventManager_) {
        std::cerr << "EventManager is required but not available" << std::endl;
        return false;
    }
    
    return true;
}

void UIComponentFactory::logComponentCreation(const std::string& componentType, 
                                             const std::string& componentId) const {
    if (componentId.empty()) {
        std::cout << "✓ Created " << componentType << std::endl;
    } else {
        std::cout << "✓ Created " << componentType << " (" << componentId << ")" << std::endl;
    }
}

// =================================================================
// Private Helper Methods
// =================================================================

std::string UIComponentFactory::generateComponentId(const std::string& componentType) const {
    int& counter = componentCounters_[componentType];
    counter++;
    
    std::ostringstream oss;
    oss << componentType << "_" << counter;
    return oss.str();
}

void UIComponentFactory::initializeDefaultStyling() {
    // Order Entry Panel
    setDefaultStyling(COMPONENT_ORDER_ENTRY, {
        "order-entry-panel", "h-100"
    });
    
    // Order Status Panel
    setDefaultStyling(COMPONENT_ORDER_STATUS, {
        "order-status-panel", "h-100"
    });
    
    // Menu Display
    setDefaultStyling(COMPONENT_MENU_DISPLAY, {
        "menu-display", "h-100"
    });
    
    // Current Order Display
    setDefaultStyling(COMPONENT_CURRENT_ORDER, {
        "current-order-display", "card", "h-100"
    });
    
    // Active Orders Display
    setDefaultStyling(COMPONENT_ACTIVE_ORDERS, {
        "active-orders-display", "card"
    });
    
    // Kitchen Status Display
    setDefaultStyling(COMPONENT_KITCHEN_STATUS, {
        "kitchen-status-display", "card"
    });
    
    // Theme Selector
    setDefaultStyling(COMPONENT_THEME_SELECTOR, {
        "theme-selector"
    });
    
    // Payment Dialog
    setDefaultStyling(DIALOG_PAYMENT, {
        "payment-dialog", "modal-dialog"
    });
    
    // Category Popover
    setDefaultStyling(DIALOG_CATEGORY, {
        "category-popover", "modal-dialog"
    });
    
    // Theme Selection Dialog
    setDefaultStyling(DIALOG_THEME_SELECTION, {
        "theme-selection-dialog", "modal-dialog"
    });
    
    std::cout << "✓ Default styling initialized" << std::endl;
}

void UIComponentFactory::initializeFeatureFlags() {
    // UI Feature flags
    setFeatureEnabled("menu_categories", true);
    setFeatureEnabled("menu_descriptions", true);
    setFeatureEnabled("order_editing", true);
    setFeatureEnabled("theme_switching", true);
    setFeatureEnabled("notifications", true);
    setFeatureEnabled("category_tiles", true);
    setFeatureEnabled("payment_dialog", true);
    
    // Advanced features (might be disabled in basic installations)
    setFeatureEnabled("split_payments", false);
    setFeatureEnabled("customer_info", false);
    setFeatureEnabled("advanced_reporting", false);
    
    std::cout << "✓ Feature flags initialized" << std::endl;
}

void UIComponentFactory::setupComponentDefaults() {
    // This could load component defaults from configuration
    // For now, we'll use hardcoded sensible defaults
    
    std::cout << "✓ Component defaults setup complete" << std::endl;
}

// =================================================================
// Component Configuration Methods
// =================================================================

void UIComponentFactory::configureMenuDisplay(MenuDisplay* menuDisplay) {
    if (!menuDisplay || !configManager_) return;
    
    // Configure display mode based on settings
    bool useCategoryTiles = configManager_->getValue<bool>("ui.menu.use_category_tiles", false);
    if (useCategoryTiles && isFeatureEnabled("category_tiles")) {
        menuDisplay->setDisplayMode(MenuDisplay::CATEGORY_TILES);
    } else {
        menuDisplay->setDisplayMode(MenuDisplay::TABLE_VIEW);
    }
    
    std::cout << "MenuDisplay configured" << std::endl;
}

void UIComponentFactory::configureCurrentOrderDisplay(CurrentOrderDisplay* currentOrderDisplay) {
    if (!currentOrderDisplay) return;
    
    // Configure edit mode based on feature flags
    bool allowEditing = isFeatureEnabled("order_editing");
    currentOrderDisplay->setEditable(allowEditing);
    
    std::cout << "CurrentOrderDisplay configured" << std::endl;
}

void UIComponentFactory::configureThemeSelector(ThemeSelector* themeSelector) {
    if (!themeSelector) return;
    
    // Configure theme selector based on feature flags
    bool themeSwitchingEnabled = isFeatureEnabled("theme_switching");
    themeSelector->setEnabled(themeSwitchingEnabled);
    
    std::cout << "ThemeSelector configured" << std::endl;
}
