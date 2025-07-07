//============================================================================
// src/ui/containers/KitchenModeContainer.cpp - FIXED (Uses modeTitle_)
//============================================================================

#include "../../../include/ui/containers/KitchenModeContainer.hpp"

#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>

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
    setStyleClass("kitchen-mode-container");
    
    // Create main layout
    auto layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    // Create and add mode title - THIS FIXES THE WARNING
    modeTitle_ = layout->addWidget(std::make_unique<Wt::WText>("🍳 Kitchen Mode"));
    modeTitle_->setStyleClass("h2 text-center mb-3");
    
    // Create main content layout
    auto contentContainer = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto contentLayout = contentContainer->setLayout(std::make_unique<Wt::WHBoxLayout>());
    contentLayout->setSpacing(20);
    
    setupLayout();
}

void KitchenModeContainer::setupLayout() {
    auto contentContainer = static_cast<Wt::WContainerWidget*>(layout()->itemAt(1)->widget());
    auto contentLayout = static_cast<Wt::WHBoxLayout*>(contentContainer->layout());
    
    createLeftPanel();
    createRightPanel();
    
    // Add panels to layout
    contentLayout->addWidget(std::unique_ptr<Wt::WWidget>(leftPanel_), 2); // 2/3 width
    contentLayout->addWidget(std::unique_ptr<Wt::WWidget>(rightPanel_), 1); // 1/3 width
}

void KitchenModeContainer::createLeftPanel() {
    leftPanel_ = new Wt::WContainerWidget();
    leftPanel_->setStyleClass("kitchen-left-panel card");
    
    auto layout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(15, 15, 15, 15);
    
    // Add title for active orders
    auto title = layout->addWidget(std::make_unique<Wt::WText>("📋 Active Orders"));
    title->setStyleClass("h4 mb-3");
    
    // Create active orders display for kitchen view
    activeOrdersDisplay_ = layout->addWidget(
        std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_));
    
    // Configure for kitchen mode
    activeOrdersDisplay_->setMaxOrdersToDisplay(0); // Show all orders
    activeOrdersDisplay_->setShowCompletedOrders(false); // Hide completed orders
}

void KitchenModeContainer::createRightPanel() {
    rightPanel_ = new Wt::WContainerWidget();
    rightPanel_->setStyleClass("kitchen-right-panel card");
    
    auto layout = rightPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(15, 15, 15, 15);
    
    // Add title for kitchen status
    auto title = layout->addWidget(std::make_unique<Wt::WText>("⏱️ Kitchen Status"));
    title->setStyleClass("h4 mb-3");
    
    // Create kitchen status display
    kitchenStatusDisplay_ = layout->addWidget(
        std::make_unique<KitchenStatusDisplay>(posService_, eventManager_));
    
    // Configure for detailed metrics
    kitchenStatusDisplay_->setShowDetailedMetrics(true);
}

void KitchenModeContainer::setupEventListeners() {
    // Subscribe to kitchen-related events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::KITCHEN_STATUS_CHANGED,
            [this](const std::any& data) { handleKitchenStatusChanged(data); })
    );
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_STATUS_CHANGED,
            [this](const std::any& data) { handleOrderStatusChanged(data); })
    );
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_SENT_TO_KITCHEN,
            [this](const std::any& data) { handleOrderStatusChanged(data); })
    );
}

void KitchenModeContainer::refresh() {
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
    if (kitchenStatusDisplay_) {
        kitchenStatusDisplay_->refresh();
    }
    
    // Update mode title with current status
    if (modeTitle_) {
        auto queueSize = posService_->getKitchenTickets().size();
        std::string titleText = "🍳 Kitchen Mode";
        if (queueSize > 0) {
            titleText += " (" + std::to_string(queueSize) + " orders in queue)";
        }
        modeTitle_->setText(titleText);
    }
}

void KitchenModeContainer::viewOrderDetails(std::shared_ptr<Order> order) {
    if (!order) return;
    
    // For now, just refresh to highlight the order
    // In a full implementation, you might open a detailed view
    refresh();
    
    // Publish notification about viewing order details
    auto notificationData = POSEvents::createNotificationData(
        "Viewing details for Order #" + std::to_string(order->getOrderId()),
        "info"
    );
    eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED, notificationData);
}

void KitchenModeContainer::handleKitchenStatusChanged(const std::any& eventData) {
    // Refresh kitchen status display
    if (kitchenStatusDisplay_) {
        kitchenStatusDisplay_->refresh();
    }
    
    // Update title with current queue status
    refresh();
}

void KitchenModeContainer::handleOrderStatusChanged(const std::any& eventData) {
    // Refresh both displays when order status changes
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
    if (kitchenStatusDisplay_) {
        kitchenStatusDisplay_->refresh();
    }
    
    // Update title
    refresh();
}
