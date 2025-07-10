#include "../../../include/ui/components/OrderStatusPanel.hpp"
#include "../../../include/utils/UIStyleHelper.hpp" // ADDED: Include styling helper

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
    
    // FIXED: Apply consistent container styling matching ActiveOrdersDisplay
    addStyleClass("pos-order-status-container h-100");
    setStyleClass("pos-order-status-container h-100");
    
    initializeUI();
    setupEventListeners();
    setupAutoRefresh();
    refresh();
    
    std::cout << "✓ OrderStatusPanel initialized with consistent styling" << std::endl;
}

void OrderStatusPanel::initializeUI() {
    // FIXED: Create main container with consistent styling
    auto mainContainer = addNew<Wt::WContainerWidget>();
    mainContainer->addStyleClass("pos-status-main h-100 bg-light");
    
    // Create panel header with consistent styling
    auto header = createPanelHeader();
    mainContainer->addWidget(std::move(header));
    
    // Create status summary with consistent styling
    auto summary = createStatusSummary();
    mainContainer->addWidget(std::move(summary));
    
    // Create component containers with consistent spacing
    // FIXED: Create Active Orders section with consistent header styling
    auto activeOrdersHeader = mainContainer->addNew<Wt::WContainerWidget>();
    activeOrdersHeader->addStyleClass("pos-section-header bg-primary text-white p-3 mx-3 mt-3 mb-0 rounded-top d-flex justify-content-between align-items-center");
    
    auto activeOrdersTitle = activeOrdersHeader->addNew<Wt::WText>("📋 Active Orders");
    activeOrdersTitle->addStyleClass("h5 mb-0 fw-bold text-white");
    
    auto activeOrdersBadge = activeOrdersHeader->addNew<Wt::WText>("Loading...");
    activeOrdersBadge->addStyleClass("badge bg-info text-dark px-3 py-2 rounded-pill");
    
    // Create active orders display directly with consistent container styling
    auto activeOrdersContainer = mainContainer->addNew<Wt::WContainerWidget>();
    activeOrdersContainer->addStyleClass("pos-table-wrapper px-3 pb-3 bg-white mx-3 mb-3 border-start border-end border-bottom rounded-bottom shadow-sm");
    
    try {
        activeOrdersDisplay_ = activeOrdersContainer->addNew<ActiveOrdersDisplay>(posService_, eventManager_);
        activeOrdersDisplay_->addStyleClass("pos-embedded-component");
        std::cout << "✓ ActiveOrdersDisplay created with consistent styling" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to create ActiveOrdersDisplay: " << e.what() << std::endl;
        
        // FIXED: Create fallback content with consistent styling
        auto fallbackText = activeOrdersContainer->addNew<Wt::WText>("❌ Unable to load active orders");
        fallbackText->addStyleClass("text-center text-danger p-4");
        activeOrdersDisplay_ = nullptr;
    }
    
    // FIXED: Create Kitchen Status section with consistent header styling
    auto kitchenStatusHeader = mainContainer->addNew<Wt::WContainerWidget>();
    kitchenStatusHeader->addStyleClass("pos-section-header bg-primary text-white p-3 mx-3 mt-3 mb-0 rounded-top d-flex justify-content-between align-items-center");
    
    auto kitchenStatusTitle = kitchenStatusHeader->addNew<Wt::WText>("🍳 Kitchen Status");
    kitchenStatusTitle->addStyleClass("h5 mb-0 fw-bold text-white");
    
    auto kitchenStatusBadge = kitchenStatusHeader->addNew<Wt::WText>("Loading...");
    kitchenStatusBadge->addStyleClass("badge bg-info text-dark px-3 py-2 rounded-pill");
    
    // Create kitchen status display directly with consistent container styling
    auto kitchenStatusContainer = mainContainer->addNew<Wt::WContainerWidget>();
    kitchenStatusContainer->addStyleClass("pos-table-wrapper px-3 pb-3 bg-white mx-3 mb-3 border-start border-end border-bottom rounded-bottom shadow-sm");
    
    try {
        kitchenStatusDisplay_ = kitchenStatusContainer->addNew<KitchenStatusDisplay>(posService_, eventManager_);
        kitchenStatusDisplay_->addStyleClass("pos-embedded-component");
        std::cout << "✓ KitchenStatusDisplay created with consistent styling" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to create KitchenStatusDisplay: " << e.what() << std::endl;
        
        // FIXED: Create fallback content with consistent styling
        auto fallbackText = kitchenStatusContainer->addNew<Wt::WText>("❌ Unable to load kitchen status");
        fallbackText->addStyleClass("text-center text-danger p-4");
        kitchenStatusDisplay_ = nullptr;
    }
    
    std::cout << "✓ OrderStatusPanel UI initialized with consistent layout" << std::endl;
}

std::unique_ptr<Wt::WWidget> OrderStatusPanel::createPanelHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    
    // FIXED: Header with consistent styling matching ActiveOrdersDisplay
    header->addStyleClass("pos-section-header bg-primary text-white p-3 mx-3 mt-3 mb-0 rounded-top d-flex justify-content-between align-items-center");
    
    // Left side: Title and subtitle
    auto leftSide = header->addNew<Wt::WContainerWidget>();
    leftSide->addStyleClass("d-flex flex-column");
    
    statusTitleText_ = leftSide->addNew<Wt::WText>("📊 Order Status Dashboard");
    statusTitleText_->addStyleClass("h4 mb-0 fw-bold text-white");
    
    lastUpdateText_ = leftSide->addNew<Wt::WText>("Initializing...");
    lastUpdateText_->addStyleClass("text-white-50 small");
    
    // Right side: Auto-refresh indicator
    auto rightSide = header->addNew<Wt::WContainerWidget>();
    rightSide->addStyleClass("text-end");
    
    auto refreshBadge = rightSide->addNew<Wt::WText>("🔄 Auto-refresh");
    refreshBadge->addStyleClass("badge bg-info text-dark px-3 py-2 rounded-pill");
    
    return std::move(header);
}

std::unique_ptr<Wt::WWidget> OrderStatusPanel::createStatusSummary() {
    auto summary = std::make_unique<Wt::WContainerWidget>();
    
    // FIXED: Summary container with consistent styling
    summary->addStyleClass("pos-status-summary px-3 py-3 bg-white mx-3 mb-3 border-start border-end border-bottom rounded-bottom shadow-sm");
    statusSummaryContainer_ = summary.get();
    
    // FIXED: Create summary header
    auto summaryHeader = summary->addNew<Wt::WText>("📈 System Overview");
    summaryHeader->addStyleClass("h5 text-success mb-3 fw-bold");
    
    // FIXED: Create status metrics grid
    auto metricsGrid = summary->addNew<Wt::WContainerWidget>();
    metricsGrid->addStyleClass("row g-3");
    
    // Active Orders metric
    auto activeOrdersCol = metricsGrid->addNew<Wt::WContainerWidget>();
    activeOrdersCol->addStyleClass("col-md-4");
    
    auto activeOrdersCard = activeOrdersCol->addNew<Wt::WContainerWidget>();
    activeOrdersCard->addStyleClass("pos-metric-card p-3 bg-light border rounded text-center");
    
    auto activeOrdersIcon = activeOrdersCard->addNew<Wt::WText>("📋");
    activeOrdersIcon->addStyleClass("display-6 mb-2");
    
    auto activeOrdersTitle = activeOrdersCard->addNew<Wt::WText>("Active Orders");
    activeOrdersTitle->addStyleClass("h6 text-muted mb-1");
    
    auto activeOrdersValue = activeOrdersCard->addNew<Wt::WText>("0");
    activeOrdersValue->addStyleClass("h4 fw-bold text-primary mb-1");
    
    auto activeOrdersSubtitle = activeOrdersCard->addNew<Wt::WText>("orders waiting");
    activeOrdersSubtitle->addStyleClass("small text-muted");
    
    // Kitchen Queue metric
    auto kitchenQueueCol = metricsGrid->addNew<Wt::WContainerWidget>();
    kitchenQueueCol->addStyleClass("col-md-4");
    
    auto kitchenQueueCard = kitchenQueueCol->addNew<Wt::WContainerWidget>();
    kitchenQueueCard->addStyleClass("pos-metric-card p-3 bg-light border rounded text-center");
    
    auto kitchenQueueIcon = kitchenQueueCard->addNew<Wt::WText>("🍳");
    kitchenQueueIcon->addStyleClass("display-6 mb-2");
    
    auto kitchenQueueTitle = kitchenQueueCard->addNew<Wt::WText>("Kitchen Queue");
    kitchenQueueTitle->addStyleClass("h6 text-muted mb-1");
    
    auto kitchenQueueValue = kitchenQueueCard->addNew<Wt::WText>("0");
    kitchenQueueValue->addStyleClass("h4 fw-bold text-primary mb-1");
    
    auto kitchenQueueSubtitle = kitchenQueueCard->addNew<Wt::WText>("items preparing");
    kitchenQueueSubtitle->addStyleClass("small text-muted");
    
    // Wait Time metric
    auto waitTimeCol = metricsGrid->addNew<Wt::WContainerWidget>();
    waitTimeCol->addStyleClass("col-md-4");
    
    auto waitTimeCard = waitTimeCol->addNew<Wt::WContainerWidget>();
    waitTimeCard->addStyleClass("pos-metric-card p-3 bg-light border rounded text-center");
    
    auto waitTimeIcon = waitTimeCard->addNew<Wt::WText>("⏱️");
    waitTimeIcon->addStyleClass("display-6 mb-2");
    
    auto waitTimeTitle = waitTimeCard->addNew<Wt::WText>("Est. Wait");
    waitTimeTitle->addStyleClass("h6 text-muted mb-1");
    
    auto waitTimeValue = waitTimeCard->addNew<Wt::WText>("0 min");
    waitTimeValue->addStyleClass("h4 fw-bold text-primary mb-1");
    
    auto waitTimeSubtitle = waitTimeCard->addNew<Wt::WText>("current estimate");
    waitTimeSubtitle->addStyleClass("small text-muted");
    
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
    
    std::cout << "✓ OrderStatusPanel event listeners setup complete" << std::endl;
}

void OrderStatusPanel::setupAutoRefresh() {
    if (!autoRefreshEnabled_ || autoRefreshInterval_ <= 0) {
        return;
    }
    
    try {
        refreshTimer_ = std::make_unique<Wt::WTimer>();
        refreshTimer_->setInterval(std::chrono::seconds(autoRefreshInterval_));
        refreshTimer_->timeout().connect([this] { onAutoRefreshTimer(); });
        refreshTimer_->start();
        
        std::cout << "✓ Auto-refresh setup: " << autoRefreshInterval_ << " seconds" << std::endl;
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
        
        std::cout << "✓ OrderStatusPanel refreshed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[OrderStatusPanel] Refresh failed: " << e.what() << std::endl;
    }
}

void OrderStatusPanel::updateStatusSummary() {
    if (!statusSummaryContainer_ || !posService_) {
        return;
    }
    
    try {
        // Update the metric cards with current data
        int activeCount = getActiveOrderCount();
        int queueSize = getKitchenQueueSize();
        int waitTime = posService_->getEstimatedWaitTime();
        
        // Find and update metric cards (simplified approach)
        auto widgets = statusSummaryContainer_->children();
        if (widgets.size() >= 2) {
            auto metricsGrid = dynamic_cast<Wt::WContainerWidget*>(widgets[1]);
            if (metricsGrid) {
                auto columns = metricsGrid->children();
                
                // Update Active Orders (first column)
                if (columns.size() > 0) {
                    auto card = dynamic_cast<Wt::WContainerWidget*>(columns[0]);
                    if (card) {
                        auto cardWidgets = card->children();
                        if (cardWidgets.size() > 0) {
                            auto cardContainer = dynamic_cast<Wt::WContainerWidget*>(cardWidgets[0]);
                            if (cardContainer) {
                                auto cardChildren = cardContainer->children();
                                if (cardChildren.size() >= 3) {
                                    auto valueWidget = dynamic_cast<Wt::WText*>(cardChildren[2]);
                                    if (valueWidget) {
                                        valueWidget->setText(std::to_string(activeCount));
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Update Kitchen Queue (second column)
                if (columns.size() > 1) {
                    auto card = dynamic_cast<Wt::WContainerWidget*>(columns[1]);
                    if (card) {
                        auto cardWidgets = card->children();
                        if (cardWidgets.size() > 0) {
                            auto cardContainer = dynamic_cast<Wt::WContainerWidget*>(cardWidgets[0]);
                            if (cardContainer) {
                                auto cardChildren = cardContainer->children();
                                if (cardChildren.size() >= 3) {
                                    auto valueWidget = dynamic_cast<Wt::WText*>(cardChildren[2]);
                                    if (valueWidget) {
                                        valueWidget->setText(std::to_string(queueSize));
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Update Wait Time (third column)
                if (columns.size() > 2) {
                    auto card = dynamic_cast<Wt::WContainerWidget*>(columns[2]);
                    if (card) {
                        auto cardWidgets = card->children();
                        if (cardWidgets.size() > 0) {
                            auto cardContainer = dynamic_cast<Wt::WContainerWidget*>(cardWidgets[0]);
                            if (cardContainer) {
                                auto cardChildren = cardContainer->children();
                                if (cardChildren.size() >= 3) {
                                    auto valueWidget = dynamic_cast<Wt::WText*>(cardChildren[2]);
                                    if (valueWidget) {
                                        valueWidget->setText(std::to_string(waitTime) + " min");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
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
    std::cout << "✓ OrderStatusPanel: Order created event handled" << std::endl;
}

void OrderStatusPanel::handleOrderModified(const std::any& eventData) {
    updateStatusSummary();
    std::cout << "✓ OrderStatusPanel: Order modified event handled" << std::endl;
}

void OrderStatusPanel::handleOrderSentToKitchen(const std::any& eventData) {
    updateStatusSummary();
    std::cout << "✓ OrderStatusPanel: Order sent to kitchen event handled" << std::endl;
}

void OrderStatusPanel::handleOrderCompleted(const std::any& eventData) {
    updateStatusSummary();
    std::cout << "✓ OrderStatusPanel: Order completed event handled" << std::endl;
}

void OrderStatusPanel::handleKitchenStatusChanged(const std::any& eventData) {
    updateStatusSummary();
    std::cout << "✓ OrderStatusPanel: Kitchen status changed event handled" << std::endl;
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
        
        std::string summary = "📊 Active: " + std::to_string(activeCount);
        summary += " | 🍳 Queue: " + std::to_string(queueSize);
        summary += " | ⏱️ Wait: " + std::to_string(waitTime) + "m";
        
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

bool OrderStatusPanel::isAutoRefreshEnabled() const {
    return autoRefreshEnabled_;
}
