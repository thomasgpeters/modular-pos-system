//============================================================================
// Fixed UIComponentFactory.cpp - Minimal Implementation
//============================================================================

//============================================================================
// src/ui/factories/UIComponentFactory.cpp - CORRECTED (Move implementation here)
//============================================================================

#include "../../../include/ui/factories/UIComponentFactory.hpp"

#include <iostream>

UIComponentFactory::UIComponentFactory(std::shared_ptr<POSService> posService,
                                      std::shared_ptr<EventManager> eventManager,
                                      std::shared_ptr<ConfigurationManager> configManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , configManager_(configManager)
    , themeService_(nullptr)
    , initialized_(false)
{
    initializeDefaults();
    validateDependencies();
    initialized_ = true;
}

void UIComponentFactory::initializeDefaults() {
    defaultTipSuggestions_ = {0.15, 0.18, 0.20, 0.22};
}

void UIComponentFactory::validateDependencies() const {
    if (!posService_) {
        throw std::invalid_argument("POSService is required");
    }
    if (!eventManager_) {
        throw std::invalid_argument("EventManager is required");
    }
    if (!configManager_) {
        throw std::invalid_argument("ConfigurationManager is required");
    }
}

void UIComponentFactory::logComponentCreation(const std::string& componentName) const {
    std::cout << "✓ Created " << componentName << std::endl;
}

// =================================================================
// Component Creation Methods (Implement only what you have)
// =================================================================

std::unique_ptr<OrderEntryPanel> UIComponentFactory::createOrderEntryPanel() {
    if (!initialized_) return nullptr;
    
    auto component = std::make_unique<OrderEntryPanel>(posService_, eventManager_);
    logComponentCreation("OrderEntryPanel");
    return component;
}

std::unique_ptr<MenuDisplay> UIComponentFactory::createMenuDisplay() {
    if (!initialized_) return nullptr;
    
    auto component = std::make_unique<MenuDisplay>(posService_, eventManager_, themeService_);
    logComponentCreation("MenuDisplay");
    return component;
}

std::unique_ptr<CurrentOrderDisplay> UIComponentFactory::createCurrentOrderDisplay() {
    if (!initialized_) return nullptr;
    
    auto component = std::make_unique<CurrentOrderDisplay>(posService_, eventManager_);
    logComponentCreation("CurrentOrderDisplay");
    return component;
}

std::unique_ptr<ActiveOrdersDisplay> UIComponentFactory::createActiveOrdersDisplay() {
    if (!initialized_) return nullptr;
    
    auto component = std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_);
    logComponentCreation("ActiveOrdersDisplay");
    return component;
}

std::unique_ptr<KitchenStatusDisplay> UIComponentFactory::createKitchenStatusDisplay() {
    if (!initialized_) return nullptr;
    
    auto component = std::make_unique<KitchenStatusDisplay>(posService_, eventManager_);
    logComponentCreation("KitchenStatusDisplay");
    return component;
}

// =================================================================
// Stub Implementations for Missing Components
// =================================================================

std::unique_ptr<OrderStatusPanel> UIComponentFactory::createOrderStatusPanel() {
    std::cout << "⚠ OrderStatusPanel not implemented yet - returning nullptr" << std::endl;
    return nullptr;
}

std::unique_ptr<ThemeSelector> UIComponentFactory::createThemeSelector() {
    std::cout << "⚠ ThemeSelector not implemented yet - returning nullptr" << std::endl;
    return nullptr;
}

std::unique_ptr<PaymentDialog> UIComponentFactory::createPaymentDialog(
    std::shared_ptr<Order> order,
    std::function<void(bool success)> callback) {
    
    std::cout << "⚠ PaymentDialog not implemented yet - returning nullptr" << std::endl;
    if (order) {
        std::cout << "  Order: #" << order->getOrderId() << std::endl;
    }
    return nullptr;
}

std::unique_ptr<PaymentDialog> UIComponentFactory::createPaymentDialog(
    std::shared_ptr<Order> order,
    std::function<void(bool success)> callback,
    bool allowSplitPayments,
    const std::vector<double>& suggestedTips) {
    
    std::cout << "⚠ PaymentDialog (extended) not implemented yet - returning nullptr" << std::endl;
    if (order) {
        std::cout << "  Order: #" << order->getOrderId() << std::endl;
    }
    std::cout << "  Split payments: " << (allowSplitPayments ? "enabled" : "disabled") << std::endl;
    return nullptr;
}

std::unique_ptr<CategoryPopover> UIComponentFactory::createCategoryPopover(
    MenuItem::Category category,
    const std::vector<std::shared_ptr<MenuItem>>& items,
    std::function<void(int itemId)> callback) {
    
    std::cout << "⚠ CategoryPopover not implemented yet - returning nullptr" << std::endl;
    std::cout << "  Category: " << static_cast<int>(category) << std::endl;
    std::cout << "  Items: " << items.size() << std::endl;
    return nullptr;
}

std::unique_ptr<ThemeSelectionDialog> UIComponentFactory::createThemeSelectionDialog(
    std::function<void(const std::string&)> callback) {
    
    std::cout << "⚠ ThemeSelectionDialog not implemented yet - returning nullptr" << std::endl;
    return nullptr;
}

// =================================================================
// Service Registration
// =================================================================

void UIComponentFactory::registerThemeService(std::shared_ptr<ThemeService> themeService) {
    themeService_ = themeService;
    std::cout << "✓ ThemeService registered with UIComponentFactory" << std::endl;
}
