#include "../../../include/ui/components/KitchenStatusDisplay.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGridLayout.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

KitchenStatusDisplay::KitchenStatusDisplay(std::shared_ptr<POSService> posService,
                                          std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , showDetailedMetrics_(true)
    , statusHeaderText_(nullptr)
    , queueSizeText_(nullptr)
    , estimatedWaitText_(nullptr)
    , kitchenLoadText_(nullptr)
    , kitchenLoadBar_(nullptr)
    , metricsContainer_(nullptr)
    , queueContainer_(nullptr) {
    
    initializeUI();
    setupEventListeners();
    refresh();
}

void KitchenStatusDisplay::initializeUI() {
    addStyleClass("kitchen-status-display card");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Status header
    auto header = createStatusHeader();
    layout->addWidget(std::move(header));
    
    // Kitchen metrics
    auto metrics = createKitchenMetrics();
    layout->addWidget(std::move(metrics));
    
    // Queue status
    auto queue = createQueueStatus();
    layout->addWidget(std::move(queue));
    
    setLayout(std::move(layout));
    
    std::cout << "✓ KitchenStatusDisplay UI initialized" << std::endl;
}

void KitchenStatusDisplay::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "Warning: EventManager not available for KitchenStatusDisplay" << std::endl;
        return;
    }
    
    // Subscribe to kitchen-related events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_SENT_TO_KITCHEN,
            [this](const std::any& data) { handleOrderSentToKitchen(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::KITCHEN_STATUS_CHANGED,
            [this](const std::any& data) { handleKitchenStatusChanged(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::KITCHEN_QUEUE_UPDATED,
            [this](const std::any& data) { handleKitchenQueueUpdated(data); }));
    
    std::cout << "✓ KitchenStatusDisplay event listeners setup complete" << std::endl;
}


// Also fix the createStatusHeader method if it has similar issues:
std::unique_ptr<Wt::WWidget> KitchenStatusDisplay::createStatusHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("card-header kitchen-status-header");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Status icon and title
    auto titleContainer = std::make_unique<Wt::WContainerWidget>();
    auto titleLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto iconText = std::make_unique<Wt::WText>(getKitchenStatusIcon());
    iconText->addStyleClass("kitchen-status-icon me-2");
    titleLayout->addWidget(std::move(iconText));
    
    // FIXED: Create and add status header text properly
    statusHeaderText_ = titleLayout->addWidget(std::make_unique<Wt::WText>("Kitchen Status"));
    statusHeaderText_->addStyleClass("h6 mb-0");
    
    titleContainer->setLayout(std::move(titleLayout));
    layout->addWidget(std::move(titleContainer), 1);
    
    // Kitchen load indicator - FIXED: Create properly
    auto progressBar = std::make_unique<Wt::WProgressBar>();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setWidth(80);
    progressBar->addStyleClass("kitchen-load-bar");
    kitchenLoadBar_ = progressBar.get();  // Store pointer before moving
    layout->addWidget(std::move(progressBar));
    
    header->setLayout(std::move(layout));
    return std::move(header);
}

// Fix for KitchenStatusDisplay.cpp - createKitchenMetrics method

std::unique_ptr<Wt::WWidget> KitchenStatusDisplay::createKitchenMetrics() {
    auto metrics = std::make_unique<Wt::WContainerWidget>();
    metrics->addStyleClass("kitchen-metrics card-body");
    metricsContainer_ = metrics.get();
    
    auto layout = std::make_unique<Wt::WGridLayout>();
    
    // Queue size metric
    auto queueSizeLabel = std::make_unique<Wt::WText>("Queue Size:");
    queueSizeLabel->addStyleClass("metric-label small text-muted");
    layout->addWidget(std::move(queueSizeLabel), 0, 0);
    
    // FIXED: Create and add queue size text directly to layout
    auto queueSizeWidget = std::make_unique<Wt::WText>("0");
    queueSizeWidget->addStyleClass("metric-value font-weight-bold");
    queueSizeText_ = queueSizeWidget.get();  // Store pointer before moving
    layout->addWidget(std::move(queueSizeWidget), 0, 1);
    
    // Estimated wait time metric
    auto waitTimeLabel = std::make_unique<Wt::WText>("Est. Wait:");
    waitTimeLabel->addStyleClass("metric-label small text-muted");
    layout->addWidget(std::move(waitTimeLabel), 1, 0);
    
    // FIXED: Create and add estimated wait text directly to layout
    auto estimatedWaitWidget = std::make_unique<Wt::WText>("0 min");
    estimatedWaitWidget->addStyleClass("metric-value font-weight-bold");
    estimatedWaitText_ = estimatedWaitWidget.get();  // Store pointer before moving
    layout->addWidget(std::move(estimatedWaitWidget), 1, 1);
    
    // Kitchen load metric
    auto loadLabel = std::make_unique<Wt::WText>("Load:");
    loadLabel->addStyleClass("metric-label small text-muted");
    layout->addWidget(std::move(loadLabel), 2, 0);
    
    // FIXED: Create and add kitchen load text directly to layout
    auto kitchenLoadWidget = std::make_unique<Wt::WText>("Normal");
    kitchenLoadWidget->addStyleClass("metric-value font-weight-bold");
    kitchenLoadText_ = kitchenLoadWidget.get();  // Store pointer before moving
    layout->addWidget(std::move(kitchenLoadWidget), 2, 1);
    
    // Set column stretches
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    
    metrics->setLayout(std::move(layout));
    return std::move(metrics);
}

std::unique_ptr<Wt::WWidget> KitchenStatusDisplay::createQueueStatus() {
    auto queue = std::make_unique<Wt::WContainerWidget>();
    queue->addStyleClass("queue-status card-footer bg-light");
    queueContainer_ = queue.get();
    
    // This will be populated by updateQueueStatus()
    
    return std::move(queue);
}

void KitchenStatusDisplay::refresh() {
    if (!posService_) {
        std::cerr << "Error: POSService not available for kitchen status refresh" << std::endl;
        return;
    }
    
    updateAllStatus();
    
    std::cout << "KitchenStatusDisplay refreshed" << std::endl;
}

void KitchenStatusDisplay::setShowDetailedMetrics(bool showDetailed) {
    showDetailedMetrics_ = showDetailed;
    
    if (metricsContainer_) {
        if (showDetailed) {
            metricsContainer_->show();
        } else {
            metricsContainer_->hide();
        }
    }
    
    std::cout << "Kitchen detailed metrics " << (showDetailed ? "shown" : "hidden") << std::endl;
}

bool KitchenStatusDisplay::getShowDetailedMetrics() const {
    return showDetailedMetrics_;
}

void KitchenStatusDisplay::updateAllStatus() {
    updateKitchenMetrics();
    updateQueueStatus();
    updateStatusColors();
}

void KitchenStatusDisplay::updateKitchenMetrics() {
    if (!posService_) {
        return;
    }
    
    // Update queue size
    auto kitchenTickets = posService_->getKitchenTickets();
    int queueSize = static_cast<int>(kitchenTickets.size());
    
    if (queueSizeText_) {
        queueSizeText_->setText(std::to_string(queueSize));
    }
    
    // Update estimated wait time
    int estimatedWait = posService_->getEstimatedWaitTime();
    if (estimatedWaitText_) {
        estimatedWaitText_->setText(formatWaitTime(estimatedWait));
    }
    
    // Update kitchen load
    int loadPercentage = calculateKitchenLoad();
    if (kitchenLoadBar_) {
        kitchenLoadBar_->setValue(loadPercentage);
    }
    
    if (kitchenLoadText_) {
        kitchenLoadText_->setText(getKitchenStatusText());
    }
    
    // Update header text
    if (statusHeaderText_) {
        statusHeaderText_->setText("Kitchen Status - " + getKitchenStatusText());
    }
}

void KitchenStatusDisplay::updateQueueStatus() {
    if (!queueContainer_ || !posService_) {
        return;
    }
    
    queueContainer_->clear();
    
    auto kitchenTickets = posService_->getKitchenTickets();
    
    if (kitchenTickets.empty()) {
        auto emptyMessage = std::make_unique<Wt::WText>("Kitchen queue is empty");
        emptyMessage->addStyleClass("text-muted small text-center");
        queueContainer_->addWidget(std::move(emptyMessage));
        return;
    }
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Add queue summary
    auto summaryText = std::make_unique<Wt::WText>(
        "Current Tickets: " + std::to_string(kitchenTickets.size()));
    summaryText->addStyleClass("small font-weight-bold mb-1");
    layout->addWidget(std::move(summaryText));
    
    // Add recent tickets (limit to 3 for space)
    int displayCount = std::min(3, static_cast<int>(kitchenTickets.size()));
    for (int i = 0; i < displayCount; ++i) {
        const auto& ticket = kitchenTickets[i];
        
        auto ticketContainer = std::make_unique<Wt::WContainerWidget>();
        ticketContainer->addStyleClass("ticket-item small d-flex justify-content-between");
        
        auto ticketInfo = std::make_unique<Wt::WText>(
            "Order #" + std::to_string(ticket.orderId) + 
            " (Table " + std::to_string(ticket.tableNumber) + ")");
        ticketInfo->addStyleClass("ticket-info");
        ticketContainer->addWidget(std::move(ticketInfo));
        
        auto statusText = std::make_unique<Wt::WText>(formatKitchenTicketStatus(ticket.status));
        statusText->addStyleClass("ticket-status text-muted");
        ticketContainer->addWidget(std::move(statusText));
        
        layout->addWidget(std::move(ticketContainer));
    }
    
    // Show "and X more" if there are additional tickets
    if (kitchenTickets.size() > displayCount) {
        auto moreText = std::make_unique<Wt::WText>(
            "... and " + std::to_string(kitchenTickets.size() - displayCount) + " more");
        moreText->addStyleClass("small text-muted font-italic");
        layout->addWidget(std::move(moreText));
    }
    
    queueContainer_->setLayout(std::move(layout));
}

// =================================================================
// Event Handlers
// =================================================================

void KitchenStatusDisplay::handleOrderSentToKitchen(const std::any& eventData) {
    std::cout << "Order sent to kitchen event received in KitchenStatusDisplay" << std::endl;
    refresh();
}

void KitchenStatusDisplay::handleKitchenStatusChanged(const std::any& eventData) {
    std::cout << "Kitchen status changed event received in KitchenStatusDisplay" << std::endl;
    refresh();
}

void KitchenStatusDisplay::handleKitchenQueueUpdated(const std::any& eventData) {
    std::cout << "Kitchen queue updated event received in KitchenStatusDisplay" << std::endl;
    updateQueueStatus();
}

// =================================================================
// Helper Methods
// =================================================================

std::string KitchenStatusDisplay::getKitchenStatusText() const {
    if (isKitchenOverloaded()) {
        return "Overloaded";
    } else if (isKitchenBusy()) {
        return "Busy";
    } else {
        return "Normal";
    }
}

std::string KitchenStatusDisplay::getKitchenStatusIcon() const {
    if (isKitchenOverloaded()) {
        return "🔴";
    } else if (isKitchenBusy()) {
        return "🟡";
    } else {
        return "🟢";
    }
}

std::string KitchenStatusDisplay::getKitchenStatusColor() const {
    if (isKitchenOverloaded()) {
        return "text-danger";
    } else if (isKitchenBusy()) {
        return "text-warning";
    } else {
        return "text-success";
    }
}

int KitchenStatusDisplay::calculateKitchenLoad() const {
    if (!posService_) {
        return 0;
    }
    
    auto kitchenTickets = posService_->getKitchenTickets();
    int queueSize = static_cast<int>(kitchenTickets.size());
    
    // Calculate load percentage based on queue size
    // 0-5 orders = 0-50%, 5-10 orders = 50-100%, >10 orders = 100%
    if (queueSize <= BUSY_THRESHOLD) {
        return (queueSize * 50) / BUSY_THRESHOLD;
    } else if (queueSize <= OVERLOADED_THRESHOLD) {
        return 50 + ((queueSize - BUSY_THRESHOLD) * 50) / (OVERLOADED_THRESHOLD - BUSY_THRESHOLD);
    } else {
        return 100;
    }
}

std::string KitchenStatusDisplay::formatWaitTime(int minutes) const {
    if (minutes <= 0) {
        return "0 min";
    } else if (minutes == 1) {
        return "1 min";
    } else if (minutes < 60) {
        return std::to_string(minutes) + " min";
    } else {
        int hours = minutes / 60;
        int remainingMinutes = minutes % 60;
        if (remainingMinutes == 0) {
            return std::to_string(hours) + "h";
        } else {
            return std::to_string(hours) + "h " + std::to_string(remainingMinutes) + "m";
        }
    }
}

std::string KitchenStatusDisplay::formatKitchenMetrics() const {
    if (!posService_) {
        return "";
    }
    
    auto kitchenStatus = posService_->getKitchenQueueStatus();
    
    std::ostringstream oss;
    oss << "Queue: " << queueSizeText_->text().toUTF8()
        << ", Wait: " << estimatedWaitText_->text().toUTF8()
        << ", Load: " << kitchenLoadText_->text().toUTF8();
    
    return oss.str();
}

void KitchenStatusDisplay::updateStatusColors() {
    std::string colorClass = getKitchenStatusColor();
    
    if (statusHeaderText_) {
        statusHeaderText_->removeStyleClass("text-success");
        statusHeaderText_->removeStyleClass("text-warning");
        statusHeaderText_->removeStyleClass("text-danger");
        statusHeaderText_->addStyleClass(colorClass);
    }
    
    if (kitchenLoadText_) {
        kitchenLoadText_->removeStyleClass("text-success");
        kitchenLoadText_->removeStyleClass("text-warning");
        kitchenLoadText_->removeStyleClass("text-danger");
        kitchenLoadText_->addStyleClass(colorClass);
    }
    
    if (kitchenLoadBar_) {
        kitchenLoadBar_->removeStyleClass("progress-bar-success");
        kitchenLoadBar_->removeStyleClass("progress-bar-warning");
        kitchenLoadBar_->removeStyleClass("progress-bar-danger");
        
        if (isKitchenOverloaded()) {
            kitchenLoadBar_->addStyleClass("progress-bar-danger");
        } else if (isKitchenBusy()) {
            kitchenLoadBar_->addStyleClass("progress-bar-warning");
        } else {
            kitchenLoadBar_->addStyleClass("progress-bar-success");
        }
    }
}

bool KitchenStatusDisplay::isKitchenBusy() const {
    if (!posService_) {
        return false;
    }
    
    auto kitchenTickets = posService_->getKitchenTickets();
    return kitchenTickets.size() >= BUSY_THRESHOLD;
}

bool KitchenStatusDisplay::isKitchenOverloaded() const {
    if (!posService_) {
        return false;
    }
    
    auto kitchenTickets = posService_->getKitchenTickets();
    return kitchenTickets.size() >= OVERLOADED_THRESHOLD;
}

int KitchenStatusDisplay::getAveragePreparationTime() const {
    // This would calculate the average preparation time based on historical data
    // For now, return a reasonable default
    return 15; // 15 minutes average
}

// Fix for KitchenStatusDisplay.cpp - formatKitchenTicketStatus method

std::string KitchenStatusDisplay::formatKitchenTicketStatus(KitchenInterface::KitchenStatus status) const {
    switch (status) {
        case KitchenInterface::ORDER_RECEIVED:    return "Received";
        case KitchenInterface::PREP_STARTED:      return "Preparing";      // FIXED: was IN_PREPARATION
        case KitchenInterface::READY_FOR_PICKUP:  return "Ready";          // FIXED: was READY_TO_SERVE
        case KitchenInterface::SERVED:            return "Served";
        default:                                  return "Unknown";
    }
}
