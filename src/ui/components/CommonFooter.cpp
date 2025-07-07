//============================================================================
// Fixed CommonFooter.cpp - Corrected addWidget() Usage
//============================================================================

#include "../../../include/ui/components/CommonFooter.hpp"

#include <iomanip>
#include <sstream>

CommonFooter::CommonFooter(std::shared_ptr<POSService> posService,
                          std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , activeOrdersText_(nullptr)
    , kitchenQueueText_(nullptr)
    , systemStatusText_(nullptr)
    , versionText_(nullptr)
{
    initializeUI();
    setupEventListeners();
    updateStatus();
}

void CommonFooter::initializeUI() {
    addStyleClass("common-footer p-2");
    
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(15, 5, 15, 5);
    
    createStatusSection();
    createSystemSection();
}

void CommonFooter::createStatusSection() {
    // Create status container and add to layout
    auto statusWidget = std::make_unique<Wt::WContainerWidget>();
    auto statusContainer = statusWidget.get();
    layout()->addWidget(std::move(statusWidget));
    
    statusContainer->addStyleClass("d-flex gap-4");
    
    // Active orders count
    activeOrdersText_ = statusContainer->addWidget(
        std::make_unique<Wt::WText>("📋 Active Orders: 0")
    );
    activeOrdersText_->addStyleClass("text-muted small");
    
    // Kitchen queue count
    kitchenQueueText_ = statusContainer->addWidget(
        std::make_unique<Wt::WText>("👨‍🍳 Kitchen Queue: 0")
    );
    kitchenQueueText_->addStyleClass("text-muted small");
    
    // System status
    systemStatusText_ = statusContainer->addWidget(
        std::make_unique<Wt::WText>("✅ System: Online")
    );
    systemStatusText_->addStyleClass("text-success small");
}

void CommonFooter::createSystemSection() {
    // Create system container and add to layout
    auto systemWidget = std::make_unique<Wt::WContainerWidget>();
    auto systemContainer = systemWidget.get();
    layout()->addWidget(std::move(systemWidget));
    
    systemContainer->addStyleClass("ms-auto");
    
    // Version info
    versionText_ = systemContainer->addWidget(
        std::make_unique<Wt::WText>("POS v3.0.0 Enhanced")
    );
    versionText_->addStyleClass("text-muted small");
}

void CommonFooter::setupEventListeners() {
    if (!eventManager_) return;
    
    // Listen for order status changes
    eventSubscriptions_.push_back(
        eventManager_->subscribe("ORDER_CREATED", [this](const std::any& data) {
            handleOrderStatusChanged(data);
        })
    );
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe("ORDER_COMPLETED", [this](const std::any& data) {
            handleOrderStatusChanged(data);
        })
    );
    
    // Listen for kitchen status changes
    eventSubscriptions_.push_back(
        eventManager_->subscribe("KITCHEN_STATUS_CHANGED", [this](const std::any& data) {
            handleKitchenStatusChanged(data);
        })
    );
}

void CommonFooter::updateStatus() {
    if (!posService_) return;
    
    try {
        // Update active orders count
        auto activeOrders = posService_->getActiveOrders();
        if (activeOrdersText_) {
            activeOrdersText_->setText("📋 Active Orders: " + std::to_string(activeOrders.size()));
        }
        
        // Update kitchen queue count
        auto kitchenTickets = posService_->getKitchenTickets();
        if (kitchenQueueText_) {
            kitchenQueueText_->setText("👨‍🍳 Kitchen Queue: " + std::to_string(kitchenTickets.size()));
        }
        
        // Update system status based on current load
        if (systemStatusText_) {
            if (activeOrders.size() > 10) {
                systemStatusText_->setText("⚠️ System: Busy");
                systemStatusText_->setStyleClass("text-warning small");
            } else if (activeOrders.size() > 5) {
                systemStatusText_->setText("🟡 System: Moderate");
                systemStatusText_->setStyleClass("text-info small");
            } else {
                systemStatusText_->setText("✅ System: Online");
                systemStatusText_->setStyleClass("text-success small");
            }
        }
    } catch (const std::exception& e) {
        // Handle any service errors gracefully
        if (systemStatusText_) {
            systemStatusText_->setText("❌ System: Error");
            systemStatusText_->setStyleClass("text-danger small");
        }
    }
}

void CommonFooter::handleOrderStatusChanged(const std::any& eventData) {
    updateStatus();
}

void CommonFooter::handleKitchenStatusChanged(const std::any& eventData) {
    updateStatus();
}


