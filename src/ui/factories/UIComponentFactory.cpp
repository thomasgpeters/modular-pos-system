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
// Component Creation Methods
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

// ============================================================================
// Dialog Creation Methods - FIXED: Exact signature matching
// ============================================================================

// FIXED: Remove ALL default parameter values from implementation signatures

// Basic PaymentDialog creation - NO DEFAULT VALUES
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
        } else {
            dialog->setTipSuggestions(defaultTipSuggestions_);
        }
    }
    
    return dialog;
}

// Extended PaymentDialog creation - NO DEFAULT VALUES  
std::unique_ptr<PaymentDialog> UIComponentFactory::createPaymentDialog(
    std::shared_ptr<Order> order,
    PaymentDialog::PaymentCallback callback,
    bool allowSplitPayments,
    const std::vector<double>& suggestedTips) {
    
    logComponentCreation("PaymentDialog (Extended)");
    
    // Create dialog directly to avoid recursion
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
    }
    
    // Apply extended configuration
    dialog->setSplitPaymentEnabled(allowSplitPayments);
    dialog->setTipSuggestions(suggestedTips);
    
    return dialog;
}

// Basic CategoryPopover creation - NO DEFAULT VALUES
std::unique_ptr<CategoryPopover> UIComponentFactory::createCategoryPopover(
    MenuItem::Category category,
    const std::vector<std::shared_ptr<MenuItem>>& items,
    CategoryPopover::ItemSelectionCallback callback) {
    
    logComponentCreation("CategoryPopover");
    
    auto popover = std::make_unique<CategoryPopover>(category, items, eventManager_, callback);
    
    // Configure from settings
    if (configManager_) {
        // Configure based on UI settings
        bool showDescriptions = configManager_->getValue<bool>("ui.menu.show.descriptions", defaultShowDescriptions_);
        popover->setShowDescriptions(showDescriptions);
        
        int maxColumns = configManager_->getValue<int>("ui.menu.popover.columns", 3);
        popover->setMaxColumns(maxColumns);
    }
    
    return popover;
}

// Extended CategoryPopover creation - NO DEFAULT VALUES
std::unique_ptr<CategoryPopover> UIComponentFactory::createCategoryPopover(
    MenuItem::Category category,
    const std::vector<std::shared_ptr<MenuItem>>& items,
    CategoryPopover::ItemSelectionCallback callback,
    int maxColumns,
    bool showDescriptions) {
    
    logComponentCreation("CategoryPopover (Extended)");
    
    // Create popover directly
    auto popover = std::make_unique<CategoryPopover>(category, items, eventManager_, callback);
    
    // Apply extended configuration
    popover->setMaxColumns(maxColumns);
    popover->setShowDescriptions(showDescriptions);
    
    return popover;
}

// Basic ThemeSelectionDialog creation (string callback) - NO DEFAULT VALUES
std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createThemeSelectionDialog(
    std::function<void(const std::string&)> callback) {
    
    logComponentCreation("ThemeSelectionDialog");
    
    // Convert the string callback to ThemeSelectionCallback
    ThemeSelectionDialog::ThemeSelectionCallback themeCallback = nullptr;
    
    if (callback) {
        themeCallback = [callback](const ThemeSelectionDialog::ThemeInfo& theme) {
            callback(theme.id);
        };
    }
    
    auto dialog = std::make_unique<ThemeSelectionDialog>(eventManager_, themeCallback);
    
    // Apply default configuration
    dialog->setShowPreviews(defaultShowPreviews_);
    dialog->setShowDescriptions(defaultShowDescriptions_);
    dialog->setMaxThemes(defaultMaxThemes_);
    
    return dialog;
}

// ThemeSelectionDialog creation with ThemeSelectionCallback
std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createThemeSelectionDialog(
    ThemeSelectionDialog::ThemeSelectionCallback callback) {
    
    logComponentCreation("ThemeSelectionDialog (ThemeSelectionCallback)");
    
    auto dialog = std::make_unique<ThemeSelectionDialog>(eventManager_, callback);
    
    // Apply default configuration
    dialog->setShowPreviews(defaultShowPreviews_);
    dialog->setShowDescriptions(defaultShowDescriptions_);
    dialog->setMaxThemes(defaultMaxThemes_);
    
    return dialog;
}

// Extended ThemeSelectionDialog creation - NO DEFAULT VALUES
std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createThemeSelectionDialog(
    ThemeSelectionDialog::ThemeSelectionCallback callback,
    bool showPreviews,
    bool showDescriptions,
    int maxThemes) {
    
    logComponentCreation("ThemeSelectionDialog (Extended)");
    
    auto dialog = std::make_unique<ThemeSelectionDialog>(
        eventManager_, callback, showPreviews, showDescriptions, maxThemes);
    
    configureThemeDialog(dialog.get(), showPreviews, showDescriptions, maxThemes);
    
    return dialog;
}

// Configured ThemeSelectionDialog creation
std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createConfiguredThemeSelectionDialog(
    ThemeSelectionDialog::ThemeSelectionCallback callback,
    const std::map<std::string, std::any>& config) {
    
    logComponentCreation("ThemeSelectionDialog (Configured)");
    
    auto dialog = std::make_unique<ThemeSelectionDialog>(eventManager_, callback, config);
    
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

// FIXED: configureThemeDialog - NO DEFAULT VALUES in implementation
void UIComponentFactory::configureThemeDialog(ThemeSelectionDialog* dialog, 
                                             bool showPreviews,
                                             bool showDescriptions, 
                                             int maxThemes) {
    if (!dialog) return;
    
    dialog->setShowPreviews(showPreviews);
    dialog->setShowDescriptions(showDescriptions);
    dialog->setMaxThemes(maxThemes);
    
    // Apply additional configuration from config manager if available
    if (configManager_) {
        int previewWidth = configManager_->getValue<int>("ui.theme.preview.width", 200);
        int previewHeight = configManager_->getValue<int>("ui.theme.preview.height", 150);
        dialog->setPreviewSize(previewWidth, previewHeight);
        
        bool livePreview = configManager_->getValue<bool>("ui.theme.live.preview", true);
        dialog->setLivePreviewEnabled(livePreview);
    }
}
