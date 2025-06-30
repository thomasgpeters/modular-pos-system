#include "../../../include/ui/components/OrderStatusPanel.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WBorderLayout.h>
#include <Wt/WApplication.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

OrderStatusPanel::OrderStatusPanel(std::shared_ptr<POSService> posService,
                                  std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , autoRefreshInterval_(DEFAULT_REFRESH_INTERVAL)
    , autoRefreshEnabled_(true)
    , statusTitleText_(nullptr)
    , lastUpdateText_(nullptr)
    , statusSummaryContainer_(nullptr)
    , refreshTimer_(nullptr) {
    
    initializeUI();
    setupEventListeners();
    setupAutoRefresh();
    refresh();
}

void OrderStatusPanel::initializeUI() {
    addStyleClass("order-status-panel");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Panel header
    auto header = createPanelHeader();
    layout->addWidget(std::move(header));
    
    // Status summary
    auto summary = createStatusSummary();
    layout->addWidget(std::move(summary));
    
    // Active orders display
    activeOrdersDisplay_ = std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_);
    layout->addWidget(std::move(activeOrdersDisplay_), 1);
    
    // Kitchen status display
    kitchenStatusDisplay_ = std::make_unique<KitchenStatusDisplay>(posService_, eventManager_);
    layout->addWidget(std::move(kitchenStatusDisplay_));
    
    setLayout(std::move(layout));
    
    std::cout << "✓ OrderStatusPanel UI initialized" << std::endl;
}

void OrderStatusPanel::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "Warning: EventManager not available for OrderStatusPanel" << std::endl;
        return;
    }
    
    // Subscribe to order events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED,
            [this](const std::any& data) { handleOrderCreated(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_MODIFIED,
            [this](const std::any& data) { handleOrderModified(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_SENT_TO_KITCHEN,
            [this](const std::any& data) { handleOrderSentToKitchen(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_COMPLETED,
            [this](const std::any& data) { handleOrderCompleted(data); }));
    
    // Subscribe to kitchen events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::KITCHEN_STATUS_CHANGED,
            [this](const std::any& data) { handleKitchenStatusChanged(data); }));
    
    std::cout << "✓ OrderStatusPanel event listeners setup complete" << std::endl;
}

void OrderStatusPanel::setupAutoRefresh() {
    if (!Wt::WApplication::instance()) {
        std::cerr << "Warning: No WApplication instance for auto-refresh timer" << std::endl;
        return;
    }
    
    refreshTimer_ = addWidget(std::make_unique<Wt::WTimer>());
    refreshTimer_->setInterval(std::chrono::seconds(autoRefreshInterval_));
    refreshTimer_->timeout().connect([this] { onAutoRefreshTimer(); });
    
    if (autoRefreshEnabled_) {
        refreshTimer_->start();
    }
    
    std::cout << "✓ Auto-refresh timer setup (interval: " << autoRefreshInterval_ << "s)" << std::endl;
}

std::unique_ptr<Wt::WWidget> OrderStatusPanel::createPanelHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("order-status-header bg-primary text-white p-3");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Title section
    auto titleContainer = std::make_unique<Wt::WContainerWidget>();
    auto titleLayout = std::make_unique<Wt::WVBoxLayout>();
    
    statusTitleText_ = titleContainer->addWidget(std::make_unique<Wt::WText>("Order Status"));
    statusTitleText_->addStyleClass("h4 mb-1 text-white");
    titleLayout->addWidget(statusTitleText_);
    
    lastUpdateText_ = titleContainer->addWidget(std::make_unique<Wt::WText>(""));
    lastUpdateText_->addStyleClass("small text-light");
    titleLayout->addWidget(lastUpdateText_);
    
    titleContainer->setLayout(std::move(titleLayout));
    layout->addWidget(std::move(titleContainer), 1);
    
    // Refresh indicator
    auto refreshIndicator = std::make_unique<Wt::WText>("🔄");
    refreshIndicator->addStyleClass("refresh-indicator");
    refreshIndicator->setToolTip("Auto-refresh enabled");
    layout->addWidget(std::move(refreshIndicator));
    
    header->setLayout(std::move(layout));
    return std::move(header);
}

std::unique_ptr<Wt::WWidget> OrderStatusPanel::createStatusSummary() {
    auto summary = std::make_unique<Wt::WContainerWidget>();
    summary->addStyleClass("status-summary bg-light p-2 border-bottom");
    statusSummaryContainer_ = summary.get();
    
    updateStatusSummary();
    
    return std::move(summary);
}

void OrderStatusPanel::refresh() {
    if (!posService_) {
        std::cerr << "Error: POSService not available for status refresh" << std::endl;
        return;
    }
    
    // Refresh child components
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
    
    if (kitchenStatusDisplay_) {
        kitchenStatusDisplay_->refresh();
    }
    
    // Update summary and last update time
    updateStatusSummary();
    updateLastUpdateTime();
    
    std::cout << "OrderStatusPanel refreshed" << std::endl;
}

void OrderStatusPanel::setAutoRefreshInterval(int intervalSeconds) {
    autoRefreshInterval_ = intervalSeconds;
    
    if (refreshTimer_) {
        refreshTimer_->setInterval(std::chrono::seconds(intervalSeconds));
    }
    
    std::cout << "Auto-refresh interval set to " << intervalSeconds << " seconds" << std::endl;
}

int OrderStatusPanel::getAutoRefreshInterval() const {
    return autoRefreshInterval_;
}

void OrderStatusPanel::setAutoRefreshEnabled(bool enabled) {
    autoRefreshEnabled_ = enabled;
    
    if (refreshTimer_) {
        if (enabled) {
            refreshTimer_->start();
        } else {
            refreshTimer_->stop();
        }
    }
    
    std::cout << "Auto-refresh " << (enabled ? "enabled" : "disabled") << std::endl;
}

bool OrderStatusPanel::isAutoRefreshEnabled() const {
    return autoRefreshEnabled_;
}

// =================================================================
// Event Handlers
// =================================================================

void OrderStatusPanel::handleOrderCreated(const std::any& eventData) {
    std::cout << "Order created event received in OrderStatusPanel" << std::endl;
    updateStatusSummary();
}

void OrderStatusPanel::handleOrderModified(const std::any& eventData) {
    std::cout << "Order modified event received in OrderStatusPanel" << std::endl;
    updateStatusSummary();
}

void OrderStatusPanel::handleOrderSentToKitchen(const std::any& eventData) {
    std::cout << "Order sent to kitchen event received in OrderStatusPanel" << std::endl;
    refresh(); // Full refresh for kitchen orders
}

void OrderStatusPanel::handleOrderCompleted(const std::any& eventData) {
    std::cout << "Order completed event received in OrderStatusPanel" << std::endl;
    refresh(); // Full refresh for completed orders
}

void OrderStatusPanel::handleKitchenStatusChanged(const std::any& eventData) {
    std::cout << "Kitchen status changed event received in OrderStatusPanel" << std::endl;
    updateStatusSummary();
}

// =================================================================
// Timer Handlers
// =================================================================

void OrderStatusPanel::onAutoRefreshTimer() {
    if (autoRefreshEnabled_) {
        refresh();
    }
}

// =================================================================
// Helper Methods
// =================================================================

void OrderStatusPanel::updateStatusSummary() {
    if (!statusSummaryContainer_) {
        return;
    }
    
    statusSummaryContainer_->clear();
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Active orders count
    auto activeOrdersContainer = std::make_unique<Wt::WContainerWidget>();
    activeOrdersContainer->addStyleClass("status-item text-center");
    
    auto activeOrdersLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto activeOrdersCount = std::make_unique<Wt::WText>(std::to_string(getActiveOrderCount()));
    activeOrdersCount->addStyleClass("h3 mb-0 text-primary");
    activeOrdersLayout->addWidget(std::move(activeOrdersCount));
    
    auto activeOrdersLabel = std::make_unique<Wt::WText>("Active Orders");
    activeOrdersLabel->addStyleClass("small text-muted");
    activeOrdersLayout->addWidget(std::move(activeOrdersLabel));
    
    activeOrdersContainer->setLayout(std::move(activeOrdersLayout));
    layout->addWidget(std::move(activeOrdersContainer), 1);
    
    // Kitchen queue size
    auto kitchenQueueContainer = std::make_unique<Wt::WContainerWidget>();
    kitchenQueueContainer->addStyleClass("status-item text-center border-left");
    
    auto kitchenQueueLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto kitchenQueueCount = std::make_unique<Wt::WText>(std::to_string(getKitchenQueueSize()));
    kitchenQueueCount->addStyleClass("h3 mb-0 text-warning");
    kitchenQueueLayout->addWidget(std::move(kitchenQueueCount));
    
    auto kitchenQueueLabel = std::make_unique<Wt::WText>("In Kitchen");
    kitchenQueueLabel->addStyleClass("small text-muted");
    kitchenQueueLayout->addWidget(std::move(kitchenQueueLabel));
    
    kitchenQueueContainer->setLayout(std::move(kitchenQueueLayout));
    layout->addWidget(std::move(kitchenQueueContainer), 1);
    
    // Kitchen status
    auto kitchenStatusContainer = std::make_unique<Wt::WContainerWidget>();
    kitchenStatusContainer->addStyleClass("status-item text-center border-left");
    
    auto kitchenStatusLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto kitchenStatusText = std::make_unique<Wt::WText>(getKitchenStatusSummary());
    kitchenStatusText->addStyleClass("h6 mb-0 text-success");
    kitchenStatusLayout->addWidget(std::move(kitchenStatusText));
    
    auto kitchenStatusLabel = std::make_unique<Wt::WText>("Kitchen Status");
    kitchenStatusLabel->addStyleClass("small text-muted");
    kitchenStatusLayout->addWidget(std::move(kitchenStatusLabel));
    
    kitchenStatusContainer->setLayout(std::move(kitchenStatusLayout));
    layout->addWidget(std::move(kitchenStatusContainer), 1);
    
    statusSummaryContainer_->setLayout(std::move(layout));
}

void OrderStatusPanel::updateLastUpdateTime() {
    if (lastUpdateText_) {
        lastUpdateText_->setText("Last updated: " + formatLastUpdateTime());
    }
}

std::string OrderStatusPanel::formatLastUpdateTime() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    return ss.str();
}

int OrderStatusPanel::getActiveOrderCount() const {
    if (!posService_) {
        return 0;
    }
    
    auto activeOrders = posService_->getActiveOrders();
    return static_cast<int>(activeOrders.size());
}

int OrderStatusPanel::getKitchenQueueSize() const {
    if (!posService_) {
        return 0;
    }
    
    auto kitchenTickets = posService_->getKitchenTickets();
    return static_cast<int>(kitchenTickets.size());
}

std::string OrderStatusPanel::getKitchenStatusSummary() const {
    if (!posService_) {
        return "Unknown";
    }
    
    int queueSize = getKitchenQueueSize();
    int estimatedWaitTime = posService_->getEstimatedWaitTime();
    
    if (queueSize == 0) {
        return "Ready";
    } else if (queueSize < 5) {
        return "Normal";
    } else if (queueSize < 10) {
        return "Busy";
    } else {
        return "Very Busy";
    }
}
