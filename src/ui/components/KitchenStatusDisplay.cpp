#include "../../../include/ui/components/KitchenStatusDisplay.hpp"

#include <Wt/WApplication.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>
#include <iostream>
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
    
    if (!posService_) {
        throw std::invalid_argument("KitchenStatusDisplay requires valid POSService");
    }
    
    // FIXED: Apply consistent container styling matching OrderEntryPanel
    setStyleClass("kitchen-status-display p-3 bg-white rounded shadow-sm");
    
    initializeUI();
    setupEventListeners();
    refresh();
    
    std::cout << "[KitchenStatusDisplay] Initialized with consistent styling" << std::endl;
}

void KitchenStatusDisplay::initializeUI() {
    // FIXED: Add "Kitchen Status" heading outside sections (like "Order Management" heading)
    auto headingText = addNew<Wt::WText>("👨‍🍳 Kitchen Status");
    headingText->addStyleClass("h4 text-primary mb-0");
    
    // Create kitchen metrics section (keep inner sections as requested)
    if (showDetailedMetrics_) {
        auto metrics = createKitchenMetrics();
        addWidget(std::move(metrics));
    }
    
    // Create queue status section (keep inner sections as requested)
    auto queue = createQueueStatus();
    addWidget(std::move(queue));
    
    // Apply initial styling
    updateStatusColors();
    
    std::cout << "[KitchenStatusDisplay] UI initialized with clean layout" << std::endl;
}

void KitchenStatusDisplay::setupEventListeners() {
    if (!eventManager_) {
        std::cout << "[KitchenStatusDisplay] No EventManager available" << std::endl;
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
    
    std::cout << "[KitchenStatusDisplay] Event listeners setup complete" << std::endl;
}

std::unique_ptr<Wt::WWidget> KitchenStatusDisplay::createStatusHeader() {
    // REMOVED: No longer needed since header is handled by parent container
    // This method is kept for interface compatibility but returns empty container
    return std::make_unique<Wt::WContainerWidget>();
}

std::unique_ptr<Wt::WWidget> KitchenStatusDisplay::createKitchenMetrics() {
    // FIXED: Create section with label and bordered container (like OrderEntryPanel structure)
    auto section = std::make_unique<Wt::WContainerWidget>();
    section->addStyleClass("mb-4");
    
    // FIXED: Add section label styled like "Select Table/Location:" 
    auto sectionLabel = section->addNew<Wt::WLabel>("Kitchen Load:");
    sectionLabel->addStyleClass("form-label fw-bold mb-2");
    
    // FIXED: Create bordered container for content (like table selection area)
    auto metricsContainer = section->addNew<Wt::WContainerWidget>();
    metricsContainer->addStyleClass("p-3 border rounded bg-white");
    metricsContainer_ = metricsContainer;
    
    // FIXED: Move kitchen status and load info here instead of separate header
    // Kitchen status text with icon
    statusHeaderText_ = metricsContainer->addNew<Wt::WText>(getKitchenStatusIcon() + " " + getKitchenStatusText());
    statusHeaderText_->addStyleClass("h6 mb-2 fw-bold");
    
    // FIXED: Kitchen load text with percentage (moved from header as requested)
    kitchenLoadText_ = metricsContainer->addNew<Wt::WText>("Load: Calculating...");
    kitchenLoadText_->addStyleClass("text-muted small mb-2");
    
    // Create kitchen load progress bar
    auto loadContainer = metricsContainer->addNew<Wt::WContainerWidget>();
    loadContainer->addStyleClass("kitchen-load-container mb-2");
    
    auto loadLabel = loadContainer->addNew<Wt::WLabel>("Progress:");
    loadLabel->addStyleClass("form-label small mb-1");
    
    kitchenLoadBar_ = loadContainer->addNew<Wt::WProgressBar>();
    kitchenLoadBar_->addStyleClass("progress kitchen-load-bar");
    kitchenLoadBar_->setRange(0, 100);
    kitchenLoadBar_->setValue(0);
    
    // Metrics text (will be populated by updateKitchenMetrics)
    if (showDetailedMetrics_) {
        auto metricsText = metricsContainer->addNew<Wt::WText>("Loading metrics...");
        metricsText->addStyleClass("kitchen-metrics-text small text-muted");
    }
    
    return std::move(section);
}

std::unique_ptr<Wt::WWidget> KitchenStatusDisplay::createQueueStatus() {
    // FIXED: Create section with label and bordered container (like OrderEntryPanel structure)
    auto section = std::make_unique<Wt::WContainerWidget>();
    section->addStyleClass("mb-4");
    
    // FIXED: Add section label styled like "Select Table/Location:"
    auto sectionLabel = section->addNew<Wt::WLabel>("Kitchen Queue:");
    sectionLabel->addStyleClass("form-label fw-bold mb-2");
    
    // FIXED: Create bordered container for content (like table selection area)
    auto queueContainer = section->addNew<Wt::WContainerWidget>();
    queueContainer->addStyleClass("p-3 border rounded bg-white");
    queueContainer_ = queueContainer;
    
    // Queue size
    queueSizeText_ = queueContainer->addNew<Wt::WText>("Queue: 0 orders");
    queueSizeText_->addStyleClass("queue-size fw-bold d-block mb-2");
    
    // Estimated wait time
    estimatedWaitText_ = queueContainer->addNew<Wt::WText>("Est. Wait: 0 min");
    estimatedWaitText_->addStyleClass("estimated-wait text-info d-block");
    
    return std::move(section);
}

void KitchenStatusDisplay::refresh() {
    if (!posService_) {
        std::cerr << "[KitchenStatusDisplay] POSService not available for refresh" << std::endl;
        return;
    }
    
    try {
        updateAllStatus();
        std::cout << "[KitchenStatusDisplay] Refreshed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[KitchenStatusDisplay] Refresh failed: " << e.what() << std::endl;
    }
}

void KitchenStatusDisplay::updateAllStatus() {
    updateKitchenMetrics();
    updateQueueStatus();
    updateStatusColors();
}

void KitchenStatusDisplay::updateKitchenMetrics() {
    if (!statusHeaderText_ || !kitchenLoadText_) {
        return;
    }
    
    try {
        // FIXED: Update status with icon (no redundant "Kitchen Status:" prefix)
        std::string statusText = getKitchenStatusIcon() + " " + getKitchenStatusText();
        statusHeaderText_->setText(statusText);
        
        // FIXED: Update load information with percentage in the load area (as requested)
        int loadPercentage = calculateKitchenLoad();
        std::string loadText = "Load: " + std::to_string(loadPercentage) + "%";
        
        if (isKitchenBusy()) {
            loadText += " (Busy)";
        } else if (isKitchenOverloaded()) {
            loadText += " (Overloaded)";
        }
        
        kitchenLoadText_->setText(loadText);
        
        // Update progress bar if it exists
        if (kitchenLoadBar_) {
            kitchenLoadBar_->setValue(loadPercentage);
        }
        
        // Update detailed metrics if container exists
        if (metricsContainer_ && showDetailedMetrics_) {
            // Find and update the metrics text
            auto children = metricsContainer_->children();
            for (auto& child : children) {
                auto textWidget = dynamic_cast<Wt::WText*>(child);
                if (textWidget && textWidget->hasStyleClass("kitchen-metrics-text")) {
                    textWidget->setText(formatKitchenMetrics());
                    break;
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[KitchenStatusDisplay] Error updating kitchen metrics: " << e.what() << std::endl;
    }
}

void KitchenStatusDisplay::updateQueueStatus() {
    if (!queueSizeText_ || !estimatedWaitText_ || !posService_) {
        return;
    }
    
    try {
        // Get current queue information
        auto tickets = posService_->getKitchenTickets();
        int queueSize = static_cast<int>(tickets.size());
        int waitTime = posService_->getEstimatedWaitTime();
        
        // Update queue size
        std::string queueText = "Queue: " + std::to_string(queueSize) + " orders";
        queueSizeText_->setText(queueText);
        
        // Update estimated wait time
        std::string waitText = "Est. Wait: " + formatWaitTime(waitTime);
        estimatedWaitText_->setText(waitText);
        
    } catch (const std::exception& e) {
        std::cerr << "[KitchenStatusDisplay] Error updating queue status: " << e.what() << std::endl;
    }
}

// Event handlers
void KitchenStatusDisplay::handleOrderSentToKitchen(const std::any& eventData) {
    updateAllStatus();
    std::cout << "[KitchenStatusDisplay] Order sent to kitchen event handled" << std::endl;
}

void KitchenStatusDisplay::handleKitchenStatusChanged(const std::any& eventData) {
    updateAllStatus();
    std::cout << "[KitchenStatusDisplay] Kitchen status changed event handled" << std::endl;
}

void KitchenStatusDisplay::handleKitchenQueueUpdated(const std::any& eventData) {
    updateQueueStatus();
    std::cout << "[KitchenStatusDisplay] Kitchen queue updated event handled" << std::endl;
}

// Helper methods
std::string KitchenStatusDisplay::getKitchenStatusText() const {
    if (!posService_) {
        return "Unavailable";
    }
    
    try {
        if (isKitchenOverloaded()) {
            return "Overloaded";
        } else if (isKitchenBusy()) {
            return "Busy";
        } else {
            return "Available";
        }
    } catch (const std::exception& e) {
        return "Error";
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
        return "danger";
    } else if (isKitchenBusy()) {
        return "warning";
    } else {
        return "success";
    }
}

int KitchenStatusDisplay::calculateKitchenLoad() const {
    if (!posService_) {
        return 0;
    }
    
    try {
        auto tickets = posService_->getKitchenTickets();
        int queueSize = static_cast<int>(tickets.size());
        
        // Calculate load as percentage of busy threshold
        int loadPercentage = std::min(100, (queueSize * 100) / std::max(1, BUSY_THRESHOLD));
        return loadPercentage;
        
    } catch (const std::exception& e) {
        return 0;
    }
}

std::string KitchenStatusDisplay::formatWaitTime(int minutes) const {
    if (minutes <= 0) {
        return "0 min";
    } else if (minutes < 60) {
        return std::to_string(minutes) + " min";
    } else {
        int hours = minutes / 60;
        int remainingMinutes = minutes % 60;
        return std::to_string(hours) + "h " + std::to_string(remainingMinutes) + "m";
    }
}

std::string KitchenStatusDisplay::formatKitchenMetrics() const {
    if (!posService_) {
        return "Metrics unavailable";
    }
    
    try {
        auto tickets = posService_->getKitchenTickets();
        int totalTickets = static_cast<int>(tickets.size());
        int avgPrepTime = getAveragePreparationTime();
        
        std::string metrics = "Active Tickets: " + std::to_string(totalTickets) + " | ";
        metrics += "Avg Prep Time: " + std::to_string(avgPrepTime) + " min";
        
        return metrics;
        
    } catch (const std::exception& e) {
        return "Error loading metrics";
    }
}

std::string KitchenStatusDisplay::formatKitchenTicketStatus(KitchenInterface::KitchenStatus status) const {
    return KitchenInterface::kitchenStatusToString(status);
}

void KitchenStatusDisplay::updateStatusColors() {
    std::string colorClass = getKitchenStatusColor();
    
    // Apply color coding to status elements
    if (statusHeaderText_) {
        statusHeaderText_->removeStyleClass("text-success text-warning text-danger");
        statusHeaderText_->addStyleClass("text-" + colorClass);
    }
    
    if (queueSizeText_) {
        queueSizeText_->removeStyleClass("text-success text-warning text-danger");
        queueSizeText_->addStyleClass("text-" + colorClass);
    }
}

// Status calculation methods
bool KitchenStatusDisplay::isKitchenBusy() const {
    if (!posService_) {
        return false;
    }
    
    try {
        auto tickets = posService_->getKitchenTickets();
        return tickets.size() >= BUSY_THRESHOLD;
    } catch (const std::exception& e) {
        return false;
    }
}

bool KitchenStatusDisplay::isKitchenOverloaded() const {
    if (!posService_) {
        return false;
    }
    
    try {
        auto tickets = posService_->getKitchenTickets();
        return tickets.size() >= OVERLOADED_THRESHOLD;
    } catch (const std::exception& e) {
        return false;
    }
}

int KitchenStatusDisplay::getAveragePreparationTime() const {
    if (!posService_) {
        return 0;
    }
    
    try {
        auto tickets = posService_->getKitchenTickets();
        if (tickets.empty()) {
            return 0;
        }
        
        int totalTime = 0;
        for (const auto& ticket : tickets) {
            totalTime += ticket.estimatedPrepTime;
        }
        
        return totalTime / static_cast<int>(tickets.size());
        
    } catch (const std::exception& e) {
        return 0;
    }
}

// Configuration methods
void KitchenStatusDisplay::setShowDetailedMetrics(bool showDetailed) {
    if (showDetailedMetrics_ != showDetailed) {
        showDetailedMetrics_ = showDetailed;
        // In a full implementation, you might want to rebuild the UI here
        // For now, just update the flag
    }
}

bool KitchenStatusDisplay::getShowDetailedMetrics() const {
    return showDetailedMetrics_;
}
