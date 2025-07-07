//============================================================================
// KitchenModeContainer Implementation
//============================================================================

#include "../../../include/ui/containers/KitchenModeContainer.hpp"

#include <iostream>

KitchenModeContainer::KitchenModeContainer(std::shared_ptr<POSService> posService,
                                          std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , leftPanel_(nullptr)
    , rightPanel_(nullptr)
    , activeOrdersDisplay_(nullptr)
    , kitchenStatusDisplay_(nullptr)
    , modeTitle_(nullptr)
{
    initializeUI();
    setupEventListeners();
}

void KitchenModeContainer::initializeUI() {
    addStyleClass("kitchen-mode-container h-100");
    setupLayout();
    createLeftPanel();
    createRightPanel();
}

void KitchenModeContainer::setupLayout() {
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    // Left panel (50% width) - Active Orders (Kitchen View)
    leftPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    leftPanel_->addStyleClass("bg-white rounded shadow-sm p-3");
    leftPanel_->setWidth(Wt::WLength(50, Wt::WLength::Unit::Percentage));
    
    // Right panel (50% width) - Kitchen Status
    rightPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightPanel_->addStyleClass("bg-white rounded shadow-sm p-3");
    rightPanel_->setWidth(Wt::WLength(50, Wt::WLength::Unit::Percentage));
}

void KitchenModeContainer::createLeftPanel() {
    auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    // Panel title with kitchen context
    auto title = leftLayout->addWidget(
        std::make_unique<Wt::WText>("👨‍🍳 Kitchen Orders")
    );
    title->addStyleClass("h5 mb-3 text-warning fw-bold");
    
    // Kitchen instructions
    auto instructions = leftLayout->addWidget(
        std::make_unique<Wt::WText>(
            "<small class='text-muted'>Click on an order to view preparation details</small>"
        )
    );
    instructions->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    instructions->addStyleClass("mb-3");
    
    // Active orders display (kitchen-focused)
    activeOrdersDisplay_ = leftLayout->addWidget(
        std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_)
    );
    
    // Configure for kitchen use
    activeOrdersDisplay_->setShowCompletedOrders(false); // Hide completed orders
    activeOrdersDisplay_->setMaxOrdersToDisplay(15);     // Show more orders
    
    std::cout << "✓ Kitchen Mode: Left panel (Kitchen Orders) created" << std::endl;
}

void KitchenModeContainer::createRightPanel() {
    auto rightLayout = rightPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    rightLayout->setContentsMargins(0, 0, 0, 0);
    
    // Panel title
    auto title = rightLayout->addWidget(
        std::make_unique<Wt::WText>("📊 Kitchen Status")
    );
    title->addStyleClass("h5 mb-3 text-warning fw-bold");
    
    // Kitchen status display
    kitchenStatusDisplay_ = rightLayout->addWidget(
        std::make_unique<KitchenStatusDisplay>(posService_, eventManager_)
    );
    
    // Configure for detailed metrics
    kitchenStatusDisplay_->setShowDetailedMetrics(true);
    
    // Quick actions section
    auto actionsContainer = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsContainer->addStyleClass("mt-4 p-3 bg-light rounded");
    
    auto actionsTitle = actionsContainer->addWidget(
        std::make_unique<Wt::WText>("⚡ Quick Actions")
    );
    actionsTitle->addStyleClass("h6 mb-3 text-dark");
    
    auto actionButtons = actionsContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    actionButtons->addStyleClass("d-flex flex-wrap gap-2");
    
    auto refreshButton = actionButtons->addWidget(
        std::make_unique<Wt::WPushButton>("🔄 Refresh Status")
    );
    refreshButton->addStyleClass("btn btn-outline-primary btn-sm");
    refreshButton->clicked().connect([this]() {
        refresh();
    });
    
    auto clearCompletedButton = actionButtons->addWidget(
        std::make_unique<Wt::WPushButton>("✅ Clear Completed")
    );
    clearCompletedButton->addStyleClass("btn btn-outline-success btn-sm");
    
    std::cout << "✓ Kitchen Mode: Right panel (Kitchen Status) created" << std::endl;
}

void KitchenModeContainer::setupEventListeners() {
    // Listen for kitchen status changes
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::KITCHEN_STATUS_CHANGED, [this](const std::any& data) {
            handleKitchenStatusChanged(data);
        })
    );
    
    // Listen for order status changes
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_STATUS_CHANGED, [this](const std::any& data) {
            handleOrderStatusChanged(data);
        })
    );
    
    std::cout << "✓ Kitchen Mode: Event listeners configured" << std::endl;
}

void KitchenModeContainer::viewOrderDetails(std::shared_ptr<Order> order) {
    if (!order) return;
    
    // Create a detailed view dialog or update the status display
    std::cout << "🔍 Viewing details for order #" << order->getOrderId() << std::endl;
    
    // Could show a modal with detailed preparation instructions
    // For now, just log the action
}

void KitchenModeContainer::refresh() {
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
    
    if (kitchenStatusDisplay_) {
        kitchenStatusDisplay_->refresh();
    }
    
    std::cout << "🔄 Kitchen Mode: Refreshed all displays" << std::endl;
}

void KitchenModeContainer::handleKitchenStatusChanged(const std::any& eventData) {
    // Refresh kitchen status display
    if (kitchenStatusDisplay_) {
        kitchenStatusDisplay_->refresh();
    }
}

void KitchenModeContainer::handleOrderStatusChanged(const std::any& eventData) {
    // Refresh active orders display
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
}
