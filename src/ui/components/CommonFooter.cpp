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
    // Dark footer spanning full width
    setAttributeValue("style",
        "background: #1a1a2e !important; width: 100% !important; "
        "padding: 8px 15px; margin: 0; box-sizing: border-box;");
    addStyleClass("common-footer");

    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);

    createStatusSection();
    createSystemSection();
}

void CommonFooter::createStatusSection() {
    // Create status container and add to layout
    auto statusWidget = std::make_unique<Wt::WContainerWidget>();
    auto statusContainer = statusWidget.get();
    layout()->addWidget(std::move(statusWidget));

    statusContainer->setAttributeValue("style", "display: flex; gap: 20px;");

    // Active orders count
    activeOrdersText_ = statusContainer->addWidget(
        std::make_unique<Wt::WText>("📋 Active Orders: 0")
    );
    activeOrdersText_->setAttributeValue("style", "color: rgba(255,255,255,0.9); font-size: 0.85rem;");

    // Kitchen queue count
    kitchenQueueText_ = statusContainer->addWidget(
        std::make_unique<Wt::WText>("👨‍🍳 Kitchen Queue: 0")
    );
    kitchenQueueText_->setAttributeValue("style", "color: rgba(255,255,255,0.9); font-size: 0.85rem;");

    // System status
    systemStatusText_ = statusContainer->addWidget(
        std::make_unique<Wt::WText>("✅ System: Online")
    );
    systemStatusText_->setAttributeValue("style", "color: #4ade80; font-size: 0.85rem;");
}

void CommonFooter::createSystemSection() {
    // Create system container and add to layout
    auto systemWidget = std::make_unique<Wt::WContainerWidget>();
    auto systemContainer = systemWidget.get();
    layout()->addWidget(std::move(systemWidget));

    systemContainer->setAttributeValue("style", "margin-left: auto;");

    // Version info
    versionText_ = systemContainer->addWidget(
        std::make_unique<Wt::WText>("POS v3.0.0-beta.1")
    );
    versionText_->setAttributeValue("style", "color: rgba(255,255,255,0.6); font-size: 0.8rem;");
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

        // Update system status based on current load (colors for dark background)
        if (systemStatusText_) {
            if (activeOrders.size() > 10) {
                systemStatusText_->setText("⚠️ System: Busy");
                systemStatusText_->setAttributeValue("style", "color: #fbbf24; font-size: 0.85rem;");
            } else if (activeOrders.size() > 5) {
                systemStatusText_->setText("🟡 System: Moderate");
                systemStatusText_->setAttributeValue("style", "color: #60a5fa; font-size: 0.85rem;");
            } else {
                systemStatusText_->setText("✅ System: Online");
                systemStatusText_->setAttributeValue("style", "color: #4ade80; font-size: 0.85rem;");
            }
        }
    } catch (const std::exception& e) {
        // Handle any service errors gracefully
        if (systemStatusText_) {
            systemStatusText_->setText("❌ System: Error");
            systemStatusText_->setAttributeValue("style", "color: #f87171; font-size: 0.85rem;");
        }
    }
}

void CommonFooter::handleOrderStatusChanged(const std::any& eventData) {
    updateStatus();
}

void CommonFooter::handleKitchenStatusChanged(const std::any& eventData) {
    updateStatus();
}


