#include "../../../include/ui/factories/UIComponentFactory.hpp"

#include <iostream>

UIComponentFactory::UIComponentFactory(std::shared_ptr<POSService> posService,
                                     std::shared_ptr<EventManager> eventManager,
                                     std::shared_ptr<ConfigurationManager> configManager)
    : posService_(posService), eventManager_(eventManager), configManager_(configManager),
      initialized_(false) {
    
    validateDependencies();
    initialized_ = true;
    
    std::cout << "UIComponentFactory initialized successfully" << std::endl;
}

void UIComponentFactory::validateDependencies() const {
    if (!posService_) {
        throw std::runtime_error("UIComponentFactory: POSService is required");
    }
    
    if (!eventManager_) {
        throw std::runtime_error("UIComponentFactory: EventManager is required");
    }
    
    if (!configManager_) {
        throw std::runtime_error("UIComponentFactory: ConfigurationManager is required");
    }
}

void UIComponentFactory::logComponentCreation(const std::string& componentName) const {
    std::cout << "UIComponentFactory: Creating " << componentName << std::endl;
}

// Component creation methods
std::unique_ptr<OrderEntryPanel> UIComponentFactory::createOrderEntryPanel() {
    logComponentCreation("OrderEntryPanel");
    
    auto component = std::make_unique<OrderEntryPanel>(posService_, eventManager_);
    configureComponent(component.get());
    
    return component;
}

std::unique_ptr<OrderStatusPanel> UIComponentFactory::createOrderStatusPanel() {
    logComponentCreation("OrderStatusPanel");
    
    auto component = std::make_unique<OrderStatusPanel>(posService_, eventManager_);
    configureComponent(component.get());
    
    return component;
}

std::unique_ptr<MenuDisplay> UIComponentFactory::createMenuDisplay() {
    logComponentCreation("MenuDisplay");
    
    auto component = std::make_unique<MenuDisplay>(posService_, eventManager_);
    configureComponent(component.get());
    
    return component;
}

std::unique_ptr<CurrentOrderDisplay> UIComponentFactory::createCurrentOrderDisplay() {
    logComponentCreation("CurrentOrderDisplay");
    
    auto component = std::make_unique<CurrentOrderDisplay>(posService_, eventManager_);
    configureComponent(component.get());
    
    return component;
}

std::unique_ptr<ActiveOrdersDisplay> UIComponentFactory::createActiveOrdersDisplay() {
    logComponentCreation("ActiveOrdersDisplay");
    
    auto component = std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_);
    configureComponent(component.get());
    
    return component;
}

std::unique_ptr<KitchenStatusDisplay> UIComponentFactory::createKitchenStatusDisplay() {
    logComponentCreation("KitchenStatusDisplay");
    
    auto component = std::make_unique<KitchenStatusDisplay>(posService_, eventManager_);
    configureComponent(component.get());
    
    return component;
}

std::unique_ptr<ThemeSelector> UIComponentFactory::createThemeSelector() {
    logComponentCreation("ThemeSelector");
    
    if (!themeService_) {
        std::cerr << "UIComponentFactory: ThemeService not registered, creating basic ThemeSelector" << std::endl;
    }
    
    auto component = std::make_unique<ThemeSelector>(themeService_, eventManager_);
    configureComponent(component.get());
    
    return component;
}

// Dialog creation methods
std::unique_ptr<PaymentDialog> UIComponentFactory::createPaymentDialog(
    std::shared_ptr<Order> order,
    PaymentDialog::PaymentCallback callback) {
    
    logComponentCreation("PaymentDialog");
    
    auto dialog = std::make_unique<PaymentDialog>(order, eventManager_, callback);
    
    // Configure payment methods from configuration
    if (configManager_) {
        auto enabledMethods = configManager_->getEnabledPaymentMethods();
        std::vector<PaymentProcessor::PaymentMethod> methods;
        
        for (const auto& methodStr : enabledMethods) {
            if (methodStr == "cash") methods.push_back(PaymentProcessor::CASH);
            else if (methodStr == "credit_card") methods.push_back(PaymentProcessor::CREDIT_CARD);
            else if (methodStr == "debit_card") methods.push_back(PaymentProcessor::DEBIT_CARD);
            else if (methodStr == "mobile_pay") methods.push_back(PaymentProcessor::MOBILE_PAY);
            else if (methodStr == "gift_card") methods.push_back(PaymentProcessor::GIFT_CARD);
        }
        
        if (!methods.empty()) {
            dialog->setAvailablePaymentMethods(methods);
        }
        
        // Configure tip suggestions
        auto tipSuggestions = configManager_->getTipSuggestions();
        if (!tipSuggestions.empty()) {
            dialog->setTipSuggestions(tipSuggestions);
        }
    }
    
    return dialog;
}

std::unique_ptr<CategoryPopover> UIComponentFactory::createCategoryPopover(
    MenuItem::Category category,
    const std::vector<std::shared_ptr<MenuItem>>& items,
    CategoryPopover::ItemSelectionCallback callback) {
    
    logComponentCreation("CategoryPopover");
    
    auto popover = std::make_unique<CategoryPopover>(category, items, eventManager_, callback);
    
    // Configure from settings
    if (configManager_) {
        // Configure based on UI settings
        bool showDescriptions = configManager_->getValue<bool>("ui.menu.show.descriptions", true);
        popover->setShowDescriptions(showDescriptions);
        
        int maxColumns = configManager_->getValue<int>("ui.menu.popover.columns", 3);
        popover->setMaxColumns(maxColumns);
    }
    
    return popover;
}

std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createThemeSelectionDialog(
    std::function<void(const std::string&)> callback) {
    
    // Convert the string callback to ThemeSelectionCallback
    ThemeSelectionDialog::ThemeSelectionCallback themeCallback = nullptr;
    
    if (callback) {
        themeCallback = [callback](const ThemeSelectionDialog::ThemeInfo& theme) {
            callback(theme.id);
        };
    }
    
    return std::make_unique<ThemeSelectionDialog>(eventManager_, themeCallback);
}

// Service registration
void UIComponentFactory::registerThemeService(std::shared_ptr<ThemeService> themeService) {
    themeService_ = themeService;
    std::cout << "UIComponentFactory: ThemeService registered" << std::endl;
}

void UIComponentFactory::registerNotificationService(std::shared_ptr<NotificationService> notificationService) {
    notificationService_ = notificationService;
    std::cout << "UIComponentFactory: NotificationService registered" << std::endl;
}
