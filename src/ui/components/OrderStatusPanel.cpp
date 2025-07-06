
#include "../../../include/ui/components/OrderStatusPanel.hpp"

#include <Wt/WApplication.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

OrderStatusPanel::OrderStatusPanel(std::shared_ptr<POSService> posService,
                                  std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , autoRefreshInterval_(DEFAULT_REFRESH_INTERVAL)
    , autoRefreshEnabled_(true)
    , statusTitleText_(nullptr)
    , lastUpdateText_(nullptr)
    , statusSummaryContainer_(nullptr)
    , activeOrdersDisplay_(nullptr)
    , kitchenStatusDisplay_(nullptr)
    , refreshTimer_(nullptr) {
    
    if (!posService_) {
        throw std::invalid_argument("OrderStatusPanel requires valid POSService");
    }
    
    addStyleClass("order-status-panel");
    
    initializeUI();
    setupEventListeners();
    setupAutoRefresh();
    refresh();
    
    std::cout << "[OrderStatusPanel] Initialized successfully" << std::endl;
}

void OrderStatusPanel::initializeUI() {
    // Create main layout container
    auto mainContainer = addNew<Wt::WContainerWidget>();
    mainContainer->addStyleClass("order-status-main");
    
    // Create panel header
    auto header = createPanelHeader();
    mainContainer->addWidget(std::move(header));
    
    // Create status summary
    auto summary = createStatusSummary();
    mainContainer->addWidget(std::move(summary));
    
    // Create active orders display directly in main container
    try {
        activeOrdersDisplay_ = mainContainer->addNew<ActiveOrdersDisplay>(posService_, eventManager_);
        activeOrdersDisplay_->addStyleClass("order-status-active-orders");
        std::cout << "✓ ActiveOrdersDisplay created and added" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to create ActiveOrdersDisplay: " << e.what() << std::endl;
        activeOrdersDisplay_ = nullptr;
    }
    
    // Create kitchen status display directly in main container
    try {
        kitchenStatusDisplay_ = mainContainer->addNew<KitchenStatusDisplay>(posService_, eventManager_);
        kitchenStatusDisplay_->addStyleClass("order-status-kitchen");
        std::cout << "✓ KitchenStatusDisplay created and added" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to create KitchenStatusDisplay: " << e.what() << std::endl;
        kitchenStatusDisplay_ = nullptr;
    }
    
    std::cout << "[OrderStatusPanel] UI initialized" << std::endl;
}

std::unique_ptr<Wt::WWidget> OrderStatusPanel::createPanelHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("order-status-header mb-3");
    
    // Title
    statusTitleText_ = header->addNew<Wt::WText>("📊 Order Status Dashboard");
    statusTitleText_->addStyleClass("h4 mb-2");
    
    // Last update time
    lastUpdateText_ = header->addNew<Wt::WText>("Last updated: Loading...");
    lastUpdateText_->addStyleClass("text-muted small");
    
    return std::move(header);
}

std::unique_ptr<Wt::WWidget> OrderStatusPanel::createStatusSummary() {
    auto summary = std::make_unique<Wt::WContainerWidget>();
    summary->addStyleClass("order-status-summary mb-3 p-3 border rounded");
    statusSummaryContainer_ = summary.get();
    
    // Summary will be populated by updateStatusSummary()
    auto summaryText = summary->addNew<Wt::WText>("Loading status summary...");
    summaryText->addStyleClass("text-muted");
    
    return std::move(summary);
}

void OrderStatusPanel::setupEventListeners() {
    if (!eventManager_) {
        std::cout << "[OrderStatusPanel] No EventManager available" << std::endl;
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
    
    std::cout << "[OrderStatusPanel] Event listeners setup complete" << std::endl;
}

void OrderStatusPanel::setupAutoRefresh() {
    if (!autoRefreshEnabled_ || autoRefreshInterval_ <= 0) {
        return;
    }
    
    try {
        // Create timer as unique_ptr (not managed by Wt widget tree)
        refreshTimer_ = std::make_unique<Wt::WTimer>();
        refreshTimer_->setInterval(std::chrono::seconds(autoRefreshInterval_));
        refreshTimer_->timeout().connect([this] { onAutoRefreshTimer(); });
        refreshTimer_->start();
        
        std::cout << "[OrderStatusPanel] Auto-refresh setup: " << autoRefreshInterval_ << " seconds" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[OrderStatusPanel] Auto-refresh setup failed: " << e.what() << std::endl;
        refreshTimer_ = nullptr;
    }
}

void OrderStatusPanel::refresh() {
    if (!posService_) {
        std::cerr << "[OrderStatusPanel] POSService not available for refresh" << std::endl;
        return;
    }
    
    try {
        // Refresh child components
        if (activeOrdersDisplay_) {
            activeOrdersDisplay_->refresh();
        }
        
        if (kitchenStatusDisplay_) {
            kitchenStatusDisplay_->refresh();
        }
        
        // Update status summary and timestamp
        updateStatusSummary();
        updateLastUpdateTime();
        
        std::cout << "[OrderStatusPanel] Refreshed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[OrderStatusPanel] Refresh failed: " << e.what() << std::endl;
    }
}

void OrderStatusPanel::updateStatusSummary() {
    if (!statusSummaryContainer_ || !posService_) {
        return;
    }
    
    try {
        // Clear existing content
        statusSummaryContainer_->clear();
        
        // Create new summary content
        auto summaryText = statusSummaryContainer_->addNew<Wt::WText>(getKitchenStatusSummary());
        summaryText->addStyleClass("small font-monospace");
        
    } catch (const std::exception& e) {
        std::cerr << "[OrderStatusPanel] Failed to update status summary: " << e.what() << std::endl;
    }
}

void OrderStatusPanel::updateLastUpdateTime() {
    if (!lastUpdateText_) {
        return;
    }
    
    try {
        std::string updateTime = formatLastUpdateTime();
        lastUpdateText_->setText("Last updated: " + updateTime);
    } catch (const std::exception& e) {
        std::cerr << "[OrderStatusPanel] Failed to update timestamp: " << e.what() << std::endl;
    }
}

// Event handlers
void OrderStatusPanel::handleOrderCreated(const std::any& eventData) {
    updateStatusSummary();
    std::cout << "[OrderStatusPanel] Order created event handled" << std::endl;
}

void OrderStatusPanel::handleOrderModified(const std::any& eventData) {
    updateStatusSummary();
    std::cout << "[OrderStatusPanel] Order modified event handled" << std::endl;
}

void OrderStatusPanel::handleOrderSentToKitchen(const std::any& eventData) {
    updateStatusSummary();
    std::cout << "[OrderStatusPanel] Order sent to kitchen event handled" << std::endl;
}

void OrderStatusPanel::handleOrderCompleted(const std::any& eventData) {
    updateStatusSummary();
    std::cout << "[OrderStatusPanel] Order completed event handled" << std::endl;
}

void OrderStatusPanel::handleKitchenStatusChanged(const std::any& eventData) {
    updateStatusSummary();
    std::cout << "[OrderStatusPanel] Kitchen status changed event handled" << std::endl;
}

void OrderStatusPanel::onAutoRefreshTimer() {
    try {
        refresh();
    } catch (const std::exception& e) {
        std::cerr << "[OrderStatusPanel] Auto-refresh timer error: " << e.what() << std::endl;
    }
}

// Helper methods
int OrderStatusPanel::getActiveOrderCount() const {
    if (!posService_) {
        return 0;
    }
    
    try {
        auto activeOrders = posService_->getActiveOrders();
        return static_cast<int>(activeOrders.size());
    } catch (const std::exception& e) {
        std::cerr << "[OrderStatusPanel] Error getting active order count: " << e.what() << std::endl;
        return 0;
    }
}

int OrderStatusPanel::getKitchenQueueSize() const {
    if (!posService_) {
        return 0;
    }
    
    try {
        auto tickets = posService_->getKitchenTickets();
        return static_cast<int>(tickets.size());
    } catch (const std::exception& e) {
        std::cerr << "[OrderStatusPanel] Error getting kitchen queue size: " << e.what() << std::endl;
        return 0;
    }
}

std::string OrderStatusPanel::getKitchenStatusSummary() const {
    if (!posService_) {
        return "⚠️ POS Service not available";
    }
    
    try {
        int activeCount = getActiveOrderCount();
        int queueSize = getKitchenQueueSize();
        int waitTime = posService_->getEstimatedWaitTime();
        
        std::string summary = "📊 Active Orders: " + std::to_string(activeCount) + " | ";
        summary += "🍳 Kitchen Queue: " + std::to_string(queueSize) + " | ";
        summary += "⏱️ Est. Wait: " + std::to_string(waitTime) + " min";
        
        return summary;
        
    } catch (const std::exception& e) {
        return "❌ Error loading status: " + std::string(e.what());
    }
}

std::string OrderStatusPanel::formatLastUpdateTime() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    return oss.str();
}

// Configuration methods
void OrderStatusPanel::setAutoRefreshInterval(int intervalSeconds) {
    autoRefreshInterval_ = intervalSeconds;
    
    if (refreshTimer_ && autoRefreshEnabled_) {
        refreshTimer_->setInterval(std::chrono::seconds(intervalSeconds));
    }
}

int OrderStatusPanel::getAutoRefreshInterval() const {
    return autoRefreshInterval_;
}

void OrderStatusPanel::setAutoRefreshEnabled(bool enabled) {
    autoRefreshEnabled_ = enabled;
    
    if (refreshTimer_) {
        if (enabled && autoRefreshInterval_ > 0) {
            refreshTimer_->start();
        } else {
            refreshTimer_->stop();
        }
    }
}
