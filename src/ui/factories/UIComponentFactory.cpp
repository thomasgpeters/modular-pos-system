#include "../../../include/ui/factories/UIComponentFactory.hpp"

#include <iostream>

UIComponentFactory::UIComponentFactory(std::shared_ptr<POSService> posService,
                                     std::shared_ptr<EventManager> eventManager,
                                     std::shared_ptr<ConfigurationManager> configManager)
    : posService_(posService), eventManager_(eventManager), configManager_(configManager),
      initialized_(false) {
    
    initializeDefaults();
    validateDependencies();
    initialized_ = true;
    
    std::cout << "UIComponentFactory initialized successfully" << std::endl;
}

void UIComponentFactory::initializeDefaults() {
    defaultShowPreviews_ = true;
    defaultShowDescriptions_ = true;
    defaultMaxThemes_ = 10;
    defaultTipSuggestions_ = {0.15, 0.18, 0.20, 0.25};
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

// ============================================================================
// Template Method Implementations - FIXED: Add missing template implementations
// ============================================================================

template<typename T>
void UIComponentFactory::configureComponent(T* component) {
    if (!component) return;
    
    // Apply common configuration to all components
    applyComponentConfiguration(component);
    
    std::cout << "Component configured with factory settings" << std::endl;
}

template<typename T>
void UIComponentFactory::applyComponentConfiguration(T* component) {
    if (!component) return;
    
    // Apply theme-related configuration if ThemeService is available
    if (themeService_) {
        // Components that support theming can be configured here
        // This is a placeholder for future theme integration
    }
    
    // Apply other configuration from ConfigurationManager
    if (configManager_) {
        // Apply configuration-based settings
        // This is extensible for future configuration options
    }
}

// Explicit template instantiations for the components we use
template void UIComponentFactory::configureComponent<OrderEntryPanel>(OrderEntryPanel* component);
template void UIComponentFactory::configureComponent<OrderStatusPanel>(OrderStatusPanel* component);
template void UIComponentFactory::configureComponent<MenuDisplay>(MenuDisplay* component);
template void UIComponentFactory::configureComponent<CurrentOrderDisplay>(CurrentOrderDisplay* component);
template void UIComponentFactory::configureComponent<ActiveOrdersDisplay>(ActiveOrdersDisplay* component);
template void UIComponentFactory::configureComponent<KitchenStatusDisplay>(KitchenStatusDisplay* component);
template void UIComponentFactory::configureComponent<ThemeSelector>(ThemeSelector* component);

// ============================================================================
// Component Creation Methods (Currently Implemented)
// ============================================================================

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

std::unique_ptr<ThemeSelector> UIComponentFactory::createThemeSelector() {
    logComponentCreation("ThemeSelector");
    
    if (!themeService_) {
        std::cerr << "UIComponentFactory: ThemeService not registered, cannot create ThemeSelector" << std::endl;
        return nullptr;
    }
    
    // FIXED: Use correct constructor signature - ThemeSelector(themeService, displayMode)
    auto component = std::make_unique<ThemeSelector>(themeService_, ThemeSelector::DisplayMode::DROPDOWN);
    configureComponent(component.get());
    
    return component;
}

// ============================================================================
// Component Creation Methods (Now Implemented)
// ============================================================================

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

// ============================================================================
// Dialog Creation Methods - FIXED: Remove placeholder classes and implement properly
// ============================================================================

std::unique_ptr<PaymentDialog> UIComponentFactory::createPaymentDialog(
    std::shared_ptr<Order> order,
    std::function<void(bool success)> callback) {
    
    logComponentCreation("PaymentDialog");
    
    // Convert the simple bool callback to PaymentProcessor::PaymentCallback
    PaymentProcessor::PaymentResult dummyResult; // We need this for the lambda
    auto paymentCallback = [callback](const PaymentProcessor::PaymentResult& result) {
        if (callback) {
            callback(result.success);
        }
    };
    
    auto dialog = std::make_unique<PaymentDialog>(order, eventManager_, paymentCallback);
    
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
        } else {
            dialog->setTipSuggestions(defaultTipSuggestions_);
        }
    }
    
    return dialog;
}

std::unique_ptr<PaymentDialog> UIComponentFactory::createPaymentDialog(
    std::shared_ptr<Order> order,
    std::function<void(bool success)> callback,
    bool allowSplitPayments,
    const std::vector<double>& suggestedTips) {
    
    logComponentCreation("PaymentDialog (Extended)");
    
    // Convert the simple bool callback to PaymentProcessor::PaymentCallback
    auto paymentCallback = [callback](const PaymentProcessor::PaymentResult& result) {
        if (callback) {
            callback(result.success);
        }
    };
    
    auto dialog = std::make_unique<PaymentDialog>(order, eventManager_, paymentCallback);
    
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
    }
    
    // Apply extended configuration
    dialog->setSplitPaymentEnabled(allowSplitPayments);
    dialog->setTipSuggestions(suggestedTips);
    
    return dialog;
}

std::unique_ptr<CategoryPopover> UIComponentFactory::createCategoryPopover(
    MenuItem::Category category,
    const std::vector<std::shared_ptr<MenuItem>>& items,
    std::function<void(int itemId)> callback) {
    
    logComponentCreation("CategoryPopover");
    
    // Convert the int callback to CategoryPopover::ItemSelectionCallback
    auto itemCallback = [callback](std::shared_ptr<MenuItem> item) {
        if (callback && item) {
            callback(item->getId()); // Assuming MenuItem has getId() method
        }
    };
    
    auto popover = std::make_unique<CategoryPopover>(category, items, eventManager_, itemCallback);
    
    // Configure from settings
    if (configManager_) {
        bool showDescriptions = configManager_->getValue<bool>("ui.menu.show.descriptions", defaultShowDescriptions_);
        popover->setShowDescriptions(showDescriptions);
        
        int maxColumns = configManager_->getValue<int>("ui.menu.popover.columns", 3);
        popover->setMaxColumns(maxColumns);
    }
    
    return popover;
}

std::unique_ptr<CategoryPopover> UIComponentFactory::createCategoryPopover(
    MenuItem::Category category,
    const std::vector<std::shared_ptr<MenuItem>>& items,
    std::function<void(int itemId)> callback,
    int maxColumns,
    bool showDescriptions) {
    
    logComponentCreation("CategoryPopover (Extended)");
    
    // Convert the int callback to CategoryPopover::ItemSelectionCallback
    auto itemCallback = [callback](std::shared_ptr<MenuItem> item) {
        if (callback && item) {
            callback(item->getId());
        }
    };
    
    auto popover = std::make_unique<CategoryPopover>(category, items, eventManager_, itemCallback);
    
    // Apply extended configuration
    popover->setMaxColumns(maxColumns);
    popover->setShowDescriptions(showDescriptions);
    
    return popover;
}

std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createThemeSelectionDialog(
    std::function<void(const std::string&)> callback) {
    
    logComponentCreation("ThemeSelectionDialog");
    
    // Convert the string callback to ThemeSelectionDialog::ThemeSelectionCallback
    auto themeCallback = [callback](const ThemeSelectionDialog::ThemeInfo& theme) {
        if (callback) {
            callback(theme.id);
        }
    };
    
    auto dialog = std::make_unique<ThemeSelectionDialog>(eventManager_, themeCallback);
    
    // Apply default configuration
    dialog->setShowPreviews(defaultShowPreviews_);
    dialog->setShowDescriptions(defaultShowDescriptions_);
    dialog->setMaxThemes(defaultMaxThemes_);
    
    return dialog;
}

std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createThemeSelectionDialog(
    std::function<void(ThemeService::Theme theme)> callback) {
    
    logComponentCreation("ThemeSelectionDialog (ThemeService callback)");
    
    // Convert ThemeService::Theme callback to ThemeSelectionDialog callback
    auto themeCallback = [callback](const ThemeSelectionDialog::ThemeInfo& theme) {
        if (callback) {
            // Convert theme.id string to ThemeService::Theme enum
            ThemeService::Theme themeEnum = ThemeService::Theme::LIGHT; // Default
            if (theme.id == "light") themeEnum = ThemeService::Theme::LIGHT;
            else if (theme.id == "dark") themeEnum = ThemeService::Theme::DARK;
            else if (theme.id == "colorful") themeEnum = ThemeService::Theme::COLORFUL;
            else if (theme.id == "base") themeEnum = ThemeService::Theme::BASE;
            else if (theme.id == "auto") themeEnum = ThemeService::Theme::AUTO;
            
            callback(themeEnum);
        }
    };
    
    auto dialog = std::make_unique<ThemeSelectionDialog>(eventManager_, themeCallback);
    
    // Apply default configuration
    dialog->setShowPreviews(defaultShowPreviews_);
    dialog->setShowDescriptions(defaultShowDescriptions_);
    dialog->setMaxThemes(defaultMaxThemes_);
    
    return dialog;
}

std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createThemeSelectionDialog(
    std::function<void(ThemeService::Theme theme)> callback,
    bool showPreviews,
    bool showDescriptions,
    int maxThemes) {
    
    logComponentCreation("ThemeSelectionDialog (Extended)");
    
    // Convert ThemeService::Theme callback to ThemeSelectionDialog callback
    auto themeCallback = [callback](const ThemeSelectionDialog::ThemeInfo& theme) {
        if (callback) {
            // Convert theme.id string to ThemeService::Theme enum
            ThemeService::Theme themeEnum = ThemeService::Theme::LIGHT;
            if (theme.id == "light") themeEnum = ThemeService::Theme::LIGHT;
            else if (theme.id == "dark") themeEnum = ThemeService::Theme::DARK;
            else if (theme.id == "colorful") themeEnum = ThemeService::Theme::COLORFUL;
            else if (theme.id == "base") themeEnum = ThemeService::Theme::BASE;
            else if (theme.id == "auto") themeEnum = ThemeService::Theme::AUTO;
            
            callback(themeEnum);
        }
    };
    
    auto dialog = std::make_unique<ThemeSelectionDialog>(
        eventManager_, themeCallback, showPreviews, showDescriptions, maxThemes);
    
    configureThemeDialog(dialog.get(), showPreviews, showDescriptions, maxThemes);
    
    return dialog;
}

std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createConfiguredThemeSelectionDialog(
    std::function<void(ThemeService::Theme theme)> callback,
    const std::map<std::string, std::any>& config) {
    
    logComponentCreation("ThemeSelectionDialog (Configured)");
    
    // Convert ThemeService::Theme callback to ThemeSelectionDialog callback
    auto themeCallback = [callback](const ThemeSelectionDialog::ThemeInfo& theme) {
        if (callback) {
            // Convert theme.id string to ThemeService::Theme enum
            ThemeService::Theme themeEnum = ThemeService::Theme::LIGHT;
            if (theme.id == "light") themeEnum = ThemeService::Theme::LIGHT;
            else if (theme.id == "dark") themeEnum = ThemeService::Theme::DARK;
            else if (theme.id == "colorful") themeEnum = ThemeService::Theme::COLORFUL;
            else if (theme.id == "base") themeEnum = ThemeService::Theme::BASE;
            else if (theme.id == "auto") themeEnum = ThemeService::Theme::AUTO;
            
            callback(themeEnum);
        }
    };
    
    auto dialog = std::make_unique<ThemeSelectionDialog>(eventManager_, themeCallback, config);
    
    return dialog;
}

// ============================================================================
// Service Registration
// ============================================================================

void UIComponentFactory::registerThemeService(std::shared_ptr<ThemeService> themeService) {
    themeService_ = themeService;
    std::cout << "UIComponentFactory: ThemeService registered" << std::endl;
}

void UIComponentFactory::registerNotificationService(std::shared_ptr<NotificationService> notificationService) {
    notificationService_ = notificationService;
    std::cout << "UIComponentFactory: NotificationService registered" << std::endl;
}

// ============================================================================
// Protected Helper Methods
// ============================================================================

void UIComponentFactory::configureThemeDialog(void* dialog, 
                                             bool showPreviews,
                                             bool showDescriptions, 
                                             int maxThemes) {
    auto* themeDialog = static_cast<ThemeSelectionDialog*>(dialog);
    if (!themeDialog) return;
    
    themeDialog->setShowPreviews(showPreviews);
    themeDialog->setShowDescriptions(showDescriptions);
    themeDialog->setMaxThemes(maxThemes);
    
    // Apply additional configuration from config manager if available
    if (configManager_) {
        int previewWidth = configManager_->getValue<int>("ui.theme.preview.width", 200);
        int previewHeight = configManager_->getValue<int>("ui.theme.preview.height", 150);
        themeDialog->setPreviewSize(previewWidth, previewHeight);
        
        bool livePreview = configManager_->getValue<bool>("ui.theme.live.preview", true);
        themeDialog->setLivePreviewEnabled(livePreview);
    }
}
