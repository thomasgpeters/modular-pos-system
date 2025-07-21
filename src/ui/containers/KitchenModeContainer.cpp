// ============================================================================
// Clean KitchenModeContainer.cpp - Minimal Borders, Streamlined Layout
// Replace your existing KitchenModeContainer.cpp with this cleaner version
// ============================================================================

#include "../../../include/ui/containers/KitchenModeContainer.hpp"
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <iostream>

KitchenModeContainer::KitchenModeContainer(std::shared_ptr<POSService> posService,
                                          std::shared_ptr<EventManager> eventManager)
    : Wt::WContainerWidget()
    , posService_(posService)
    , eventManager_(eventManager)
    , leftPanel_(nullptr)
    , rightPanel_(nullptr)
    , activeOrdersDisplay_(nullptr)
    , kitchenStatusDisplay_(nullptr)
{
    if (!posService_ || !eventManager_) {
        throw std::invalid_argument("KitchenModeContainer requires valid POSService and EventManager");
    }
    
    // CLEAN: Simple container class - no excessive styling
    setStyleClass("kitchen-mode-container h-100");
    
    std::cout << "[KitchenModeContainer] Initializing with clean design..." << std::endl;
    
    initializeUI();
    setupEventListeners();
    
    std::cout << "[KitchenModeContainer] Initialized successfully with clean layout" << std::endl;
}

void KitchenModeContainer::initializeUI() {
    setupLayout();
    createLeftPanel();
    createRightPanel();
}

void KitchenModeContainer::setupLayout() {
    // Create main horizontal layout
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0); // CLEAN: No margins
    layout->setSpacing(0); // CLEAN: No spacing between panels
    
    // Create panels first, then set stretch factors
    leftPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    // CLEAN: Set layout proportions to match POSModeContainer: 
    // Left panel (ActiveOrdersDisplay) 30%, Right panel (Kitchen Status) 70%
    layout->setStretchFactor(leftPanel_, 3);  // 30% - same as POS mode
    layout->setStretchFactor(rightPanel_, 7); // 70% - gives more space to kitchen status
    
    // CLEAN: Simple panel classes
    leftPanel_->setStyleClass("kitchen-left-panel");
    rightPanel_->setStyleClass("kitchen-right-panel");
    
    std::cout << "[KitchenModeContainer] Layout setup complete with clean proportions" << std::endl;
}

void KitchenModeContainer::createLeftPanel() {
    if (!leftPanel_) {
        std::cerr << "[KitchenModeContainer] Error: leftPanel_ is null" << std::endl;
        return;
    }
    
    // CLEAN: Simple layout without excessive margins/padding
    auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    
    // CLEAN: Create ActiveOrdersDisplay directly with showHeader=true for consistency
    try {
        activeOrdersDisplay_ = leftLayout->addWidget(
            std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_, true) // showHeader=true for consistency
        );
        activeOrdersDisplay_->setStyleClass("kitchen-active-orders flex-fill");
        
        std::cout << "[KitchenModeContainer] ✓ ActiveOrdersDisplay created with clean styling" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[KitchenModeContainer] ✗ Error creating ActiveOrdersDisplay: " << e.what() << std::endl;
        activeOrdersDisplay_ = nullptr;
    }
}

void KitchenModeContainer::createRightPanel() {
    if (!rightPanel_) {
        std::cerr << "[KitchenModeContainer] Error: rightPanel_ is null" << std::endl;
        return;
    }
    
    // CLEAN: Simple layout without excessive styling
    auto rightLayout = rightPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    
    // Create Kitchen Status Display directly
    try {
        kitchenStatusDisplay_ = rightLayout->addWidget(
            std::make_unique<KitchenStatusDisplay>(posService_, eventManager_)
        );
        kitchenStatusDisplay_->setStyleClass("kitchen-status-display");
        
        std::cout << "[KitchenModeContainer] ✓ KitchenStatusDisplay created with clean styling" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[KitchenModeContainer] ✗ Error creating KitchenStatusDisplay: " << e.what() << std::endl;
        kitchenStatusDisplay_ = nullptr;
    }
}

void KitchenModeContainer::setupEventListeners() {
    if (!eventManager_) {
        std::cout << "[KitchenModeContainer] No EventManager available for event listeners" << std::endl;
        return;
    }
    
    // Subscribe to kitchen-related events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::KITCHEN_STATUS_CHANGED,
            [this](const std::any& data) { handleKitchenStatusChanged(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_SENT_TO_KITCHEN,
            [this](const std::any& data) { handleOrderStatusChanged(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_COMPLETED,
            [this](const std::any& data) { handleOrderStatusChanged(data); }));
    
    std::cout << "[KitchenModeContainer] Event listeners setup complete" << std::endl;
}

void KitchenModeContainer::refresh() {
    std::cout << "[KitchenModeContainer] Refreshing components..." << std::endl;
    
    try {
        // Refresh active orders display
        if (activeOrdersDisplay_) {
            activeOrdersDisplay_->refresh();
        }
        
        // Refresh kitchen status display
        if (kitchenStatusDisplay_) {
            kitchenStatusDisplay_->refresh();
        }
        
        std::cout << "[KitchenModeContainer] Refresh completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[KitchenModeContainer] Error during refresh: " << e.what() << std::endl;
    }
}

void KitchenModeContainer::viewOrderDetails(std::shared_ptr<Order> order) {
    if (!order) {
        std::cout << "[KitchenModeContainer] Cannot view details for null order" << std::endl;
        return;
    }
    
    std::cout << "[KitchenModeContainer] Viewing details for order #" << order->getOrderId() << std::endl;
    
    // TODO: Implement order details view for kitchen
    // This could open a modal or update a details panel
}

// Event handlers
void KitchenModeContainer::handleKitchenStatusChanged(const std::any& eventData) {
    std::cout << "[KitchenModeContainer] Kitchen status changed" << std::endl;
    
    // Refresh kitchen status display
    if (kitchenStatusDisplay_) {
        kitchenStatusDisplay_->refresh();
    }
}

void KitchenModeContainer::handleOrderStatusChanged(const std::any& eventData) {
    std::cout << "[KitchenModeContainer] Order status changed" << std::endl;
    
    // Refresh both displays when order status changes
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
    
    if (kitchenStatusDisplay_) {
        kitchenStatusDisplay_->refresh();
    }
}
