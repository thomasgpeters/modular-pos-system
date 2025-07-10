#include "../../../include/ui/components/ActiveOrdersDisplay.hpp"

#include <Wt/WApplication.h>
#include <Wt/WLabel.h>
#include <Wt/WTableCell.h>
#include <Wt/WGroupBox.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <chrono>

ActiveOrdersDisplay::ActiveOrdersDisplay(std::shared_ptr<POSService> posService,
                                        std::shared_ptr<EventManager> eventManager,
                                         bool showHeader)
    : posService_(posService)
    , eventManager_(eventManager)
    , maxOrdersToDisplay_(DEFAULT_MAX_ORDERS)
    , showCompletedOrders_(false)
    , headerText_(nullptr)
    , orderCountText_(nullptr)
    , ordersTable_(nullptr) 
    , showHeader_(showHeader)
{
    
    if (!posService_) {
        throw std::invalid_argument("ActiveOrdersDisplay requires valid POSService");
    }
    
    // FIXED: Remove extra padding/margins and borders
    addStyleClass("pos-active-orders-container h-100");
    setStyleClass("pos-active-orders-container h-100"); // Remove default container styling
    
    initializeUI();
    setupEventListeners();
    refresh();
    
    std::cout << "✓ ActiveOrdersDisplay initialized with clean styling" << std::endl;
}

// Replace the initializeUI() method with this corrected version:

void ActiveOrdersDisplay::initializeUI() {
    // Create main container without excessive padding/borders
    auto mainContainer = addNew<Wt::WContainerWidget>();
    mainContainer->addStyleClass("pos-orders-main h-100 bg-light");
    
    // Only create header if showHeader_ is true
    if (showHeader_) {
        auto header = createDisplayHeader();
        mainContainer->addWidget(std::move(header));
    }
    
    // Create table container with minimal spacing
    auto tableContainer = mainContainer->addNew<Wt::WContainerWidget>();
    tableContainer->addStyleClass("pos-table-wrapper px-3 pb-3");
    
    // Create table without borders and excessive spacing
    ordersTable_ = tableContainer->addNew<Wt::WTable>();
    ordersTable_->addStyleClass("table pos-orders-table w-100 mb-0");
    ordersTable_->setWidth(Wt::WLength("100%"));

    // Initialize table headers
    initializeTableHeaders();
    
    std::cout << "✓ ActiveOrdersDisplay UI initialized with clean layout" << std::endl;
}

void ActiveOrdersDisplay::initializeTableHeaders() {
    if (!ordersTable_) {
        return;
    }
    
    // Create table headers
    ordersTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Order #"));
    ordersTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Table/Location"));
    ordersTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Status"));
    ordersTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Items"));
    ordersTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Total"));
    ordersTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Time"));
    ordersTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Actions"));
    
    // FIXED: Apply proper header styling with white text on blue background
    for (int col = 0; col < ordersTable_->columnCount(); ++col) {
        auto headerCell = ordersTable_->elementAt(0, col);
        // FIXED: Use white text on blue background, remove borders
        headerCell->addStyleClass("pos-table-header bg-primary text-white text-center p-2 fw-bold border-0");
        
        auto headerText = dynamic_cast<Wt::WText*>(headerCell->widget(0));
        if (headerText) {
            headerText->addStyleClass("text-white fw-bold"); // Ensure white text
        }
    }
    
    std::cout << "✓ Table headers styled with white text on blue background" << std::endl;
}

void ActiveOrdersDisplay::createHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->setStyleClass("active-orders-header mb-3");
    
    headerText_ = header->addNew<Wt::WText>("📋 Active Orders");
    headerText_->setStyleClass("h4 text-primary mb-2");
    
    // Add order count badge
    // Count badge with better styling
    orderCountText_ = header->addNew<Wt::WText>("0 orders");
    orderCountText_->addStyleClass("badge bg-info text-dark px-3 py-2 rounded-pill");
}

std::unique_ptr<Wt::WWidget> ActiveOrdersDisplay::createDisplayHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    
    // FIXED: Header with proper spacing and no bottom border
    header->addStyleClass("pos-section-header bg-primary text-white p-3 mx-3 mt-3 mb-0 rounded-top d-flex justify-content-between align-items-center");
    
    // Title with white text
    headerText_ = header->addNew<Wt::WText>("📋 Active Orders");
    headerText_->addStyleClass("h4 mb-0 fw-bold text-white");
    
    // Count badge with better styling
    orderCountText_ = header->addNew<Wt::WText>("0 orders");
    orderCountText_->addStyleClass("badge bg-info text-dark px-3 py-2 rounded-pill");
    
    return std::move(header);
}

void ActiveOrdersDisplay::addOrderRow(std::shared_ptr<Order> order, int row) {
    if (!order || !ordersTable_) {
        return;
    }
    
    try {
        // Order ID
        auto orderIdText = std::make_unique<Wt::WText>(formatOrderId(order->getOrderId()));
        orderIdText->addStyleClass("fw-bold text-primary");
        ordersTable_->elementAt(row, 0)->addWidget(std::move(orderIdText));
        
        // Table/Location
        auto tableText = std::make_unique<Wt::WText>(order->getTableIdentifier());
        tableText->addStyleClass("fw-medium");
        ordersTable_->elementAt(row, 1)->addWidget(std::move(tableText));
        
        // Status badge
        auto statusText = std::make_unique<Wt::WText>(formatOrderStatus(order->getStatus()));
        UIStyleHelper::styleBadge(statusText.get(), getStatusBadgeVariant(order->getStatus()));
        ordersTable_->elementAt(row, 2)->addWidget(std::move(statusText));
        
        // Items count
        auto itemsText = std::make_unique<Wt::WText>(std::to_string(order->getItems().size()) + " items");
        itemsText->addStyleClass("text-muted small");
        ordersTable_->elementAt(row, 3)->addWidget(std::move(itemsText));
        
        // Total
        auto totalText = std::make_unique<Wt::WText>(formatCurrency(order->getTotal()));
        totalText->addStyleClass("fw-bold text-success");
        ordersTable_->elementAt(row, 4)->addWidget(std::move(totalText));
        
        // Time
        auto timeText = std::make_unique<Wt::WText>(formatOrderTime(order));
        timeText->addStyleClass("text-muted small");
        ordersTable_->elementAt(row, 5)->addWidget(std::move(timeText));
        
        // Actions
        auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
        actionsContainer->addStyleClass("d-flex gap-1");
        
        // View button
        auto viewButton = actionsContainer->addNew<Wt::WPushButton>("👁️ View");
        UIStyleHelper::styleButton(viewButton, "outline-primary", "sm");
        viewButton->clicked().connect([this, order]() {
            onViewOrderClicked(order->getOrderId());
        });
        
        // Complete button (only for appropriate statuses)
        if (order->getStatus() == Order::READY) {
            auto completeButton = actionsContainer->addNew<Wt::WPushButton>("✅ Complete");
            UIStyleHelper::styleButton(completeButton, "success", "sm");
            completeButton->clicked().connect([this, order]() {
                onCompleteOrderClicked(order->getOrderId());
            });
        }
        
        // Cancel button (only for pending orders)
        if (order->getStatus() == Order::PENDING || order->getStatus() == Order::SENT_TO_KITCHEN) {
            auto cancelButton = actionsContainer->addNew<Wt::WPushButton>("❌ Cancel");
            UIStyleHelper::styleButton(cancelButton, "outline-danger", "sm");
            cancelButton->clicked().connect([this, order]() {
                onCancelOrderClicked(order->getOrderId());
            });
        }
        
        ordersTable_->elementAt(row, 6)->addWidget(std::move(actionsContainer));
        
        // FIXED: Apply clean row styling without borders
        applyRowStyling(row, (row % 2) == 0);
        
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error adding order row: " << e.what() << std::endl;
    }
}

void ActiveOrdersDisplay::applyRowStyling(int row, bool isEven) {
    if (!ordersTable_ || row == 0) { // Don't style header row
        return;
    }
    
    for (int col = 0; col < ordersTable_->columnCount(); ++col) {
        auto cell = ordersTable_->elementAt(row, col);
        
        // FIXED: Clean cell styling without borders
        cell->addStyleClass("pos-table-cell p-2 align-middle border-0");
        
        // FIXED: Subtle alternating row colors
        if (isEven) {
            cell->addStyleClass("bg-white");
        } else {
            cell->addStyleClass("bg-light");
        }
        
        // Column-specific alignment
        if (col == 0 || col == 2 || col == 3 || col == 4 || col == 5) {
            cell->addStyleClass("text-center");
        } else if (col == 6) {
            cell->addStyleClass("text-start");
        }
    }
}

void ActiveOrdersDisplay::showEmptyOrdersMessage() {
    if (!ordersTable_) {
        return;
    }
    
    int row = ordersTable_->rowCount();
    auto messageContainer = std::make_unique<Wt::WContainerWidget>();
    
    // FIXED: Clean empty state styling
    messageContainer->addStyleClass("text-center py-5 bg-white");
    
    auto messageText = messageContainer->addNew<Wt::WText>("📝 No active orders");
    messageText->addStyleClass("h5 mb-2 text-muted");
    
    auto subText = messageContainer->addNew<Wt::WText>("Orders will appear here when created");
    subText->addStyleClass("text-muted small");
    
    auto cell = ordersTable_->elementAt(row, 0);
    cell->setColumnSpan(7);
    cell->addWidget(std::move(messageContainer));
    cell->addStyleClass("border-0");
}

// Fix for ActiveOrdersDisplay.cpp - updateOrderCount() method
// Replace the problematic updateOrderCount() method with this corrected version:

// Replace the updateOrderCount() method in ActiveOrdersDisplay.cpp with this:

// Also add safety checks to updateOrderCount():
void ActiveOrdersDisplay::updateOrderCount() {
    // Add safety check - only update if we have a valid orderCountText widget
    if (!orderCountText_ || !orderCountText_->parent()) {
        std::cout << "[ActiveOrdersDisplay] orderCountText_ is null or orphaned - skipping update" << std::endl;
        return;
    }
    
    try {
        auto orders = getDisplayOrders();
        std::string countText = std::to_string(orders.size()) + " orders";
        orderCountText_->setText(countText);
        
        // Remove CSS classes individually with safety checks
        std::cout << "[ActiveOrdersDisplay] updateOrderCount() removing bg-secondary." << std::endl;
        orderCountText_->removeStyleClass("bg-secondary");

        std::cout << "[ActiveOrdersDisplay] updateOrderCount() removing bg-warning." << std::endl;
        orderCountText_->removeStyleClass("bg-warning");

        std::cout << "[ActiveOrdersDisplay] updateOrderCount() removing bg-danger." << std::endl;
        orderCountText_->removeStyleClass("bg-danger");
        
        std::cout << "[ActiveOrdersDisplay] updateOrderCount() removing bg-info." << std::endl;
        orderCountText_->removeStyleClass("bg-info");
        
        // Apply new badge styling based on count
        if (orders.size() == 0) {
            UIStyleHelper::styleBadge(orderCountText_, "secondary");
        } else if (orders.size() > 10) {
            UIStyleHelper::styleBadge(orderCountText_, "danger");
        } else if (orders.size() > 5) {
            UIStyleHelper::styleBadge(orderCountText_, "warning");
        } else {
            UIStyleHelper::styleBadge(orderCountText_, "info");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error in updateOrderCount: " << e.what() << std::endl;
    }
}

std::string ActiveOrdersDisplay::getStatusBadgeVariant(Order::Status status) const {
    switch (status) {
        case Order::PENDING:         return "secondary";
        case Order::SENT_TO_KITCHEN: return "primary";
        case Order::PREPARING:       return "warning";
        case Order::READY:           return "info";
        case Order::SERVED:          return "success";
        case Order::CANCELLED:       return "danger";
        default:                     return "secondary";
    }
}

// =================================================================
// EXISTING METHOD IMPLEMENTATIONS (unchanged)
// =================================================================

void ActiveOrdersDisplay::refresh() {
    updateOrdersTable();
    updateOrderCount();
}

void ActiveOrdersDisplay::setMaxOrdersToDisplay(int maxOrders) {
    maxOrdersToDisplay_ = maxOrders;
    refresh();
}

int ActiveOrdersDisplay::getMaxOrdersToDisplay() const {
    return maxOrdersToDisplay_;
}

void ActiveOrdersDisplay::setShowCompletedOrders(bool showCompleted) {
    showCompletedOrders_ = showCompleted;
    refresh();
}

bool ActiveOrdersDisplay::getShowCompletedOrders() const {
    return showCompletedOrders_;
}

void ActiveOrdersDisplay::setupEventListeners() {
    if (!eventManager_) return;
    
    // Subscribe to order events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED, 
            [this](const std::any& data) { handleOrderCreated(data); })
    );
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_MODIFIED,
            [this](const std::any& data) { handleOrderModified(data); })
    );
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_COMPLETED,
            [this](const std::any& data) { handleOrderCompleted(data); })
    );
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CANCELLED,
            [this](const std::any& data) { handleOrderCancelled(data); })
    );
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::KITCHEN_STATUS_CHANGED,
            [this](const std::any& data) { handleKitchenStatusChanged(data); })
    );
}

void ActiveOrdersDisplay::updateOrdersTable() {
    if (!ordersTable_) return;
    
    // Clear existing rows (except header)
    while (ordersTable_->rowCount() > 1) {
        ordersTable_->removeRow(1);
    }
    
    auto orders = getDisplayOrders();
    
    if (orders.empty()) {
        showEmptyOrdersMessage();
        return;
    }
    
    // Add order rows
    for (size_t i = 0; i < orders.size(); ++i) {
        if (maxOrdersToDisplay_ > 0 && i >= static_cast<size_t>(maxOrdersToDisplay_)) {
            break;
        }
        addOrderRow(orders[i], static_cast<int>(i + 1));
    }
}

std::vector<std::shared_ptr<Order>> ActiveOrdersDisplay::getDisplayOrders() const {
    if (!posService_) return {};
    
    auto orders = posService_->getActiveOrders();
    
    // Filter orders based on configuration
    if (!showCompletedOrders_) {
        orders.erase(
            std::remove_if(orders.begin(), orders.end(),
                [](const std::shared_ptr<Order>& order) {
                    return order->getStatus() == Order::SERVED ||
                           order->getStatus() == Order::CANCELLED;
                }),
            orders.end()
        );
    }
    
    // Sort by creation time (newest first)
    std::sort(orders.begin(), orders.end(),
        [](const std::shared_ptr<Order>& a, const std::shared_ptr<Order>& b) {
            return a->getTimestamp() > b->getTimestamp();
        });
    
    return orders;
}

// Event handlers
void ActiveOrdersDisplay::handleOrderCreated(const std::any& eventData) {
    refresh();
}

void ActiveOrdersDisplay::handleOrderModified(const std::any& eventData) {
    refresh();
}

void ActiveOrdersDisplay::handleOrderCompleted(const std::any& eventData) {
    refresh();
}

void ActiveOrdersDisplay::handleOrderCancelled(const std::any& eventData) {
    refresh();
}

void ActiveOrdersDisplay::handleKitchenStatusChanged(const std::any& eventData) {
    refresh();
}

// UI action handlers
void ActiveOrdersDisplay::onViewOrderClicked(int orderId) {
    std::cout << "[ActiveOrdersDisplay] View order #" << orderId << " (not implemented)" << std::endl;
}

void ActiveOrdersDisplay::onCompleteOrderClicked(int orderId) {
    std::cout << "[ActiveOrdersDisplay] Complete order #" << orderId << " (not implemented)" << std::endl;
}

void ActiveOrdersDisplay::onCancelOrderClicked(int orderId) {
    if (posService_) {
        bool success = posService_->cancelOrder(orderId);
        if (success) {
            std::cout << "[ActiveOrdersDisplay] Order #" << orderId << " cancelled successfully" << std::endl;
            refresh();
        } else {
            std::cout << "[ActiveOrdersDisplay] Failed to cancel order #" << orderId << std::endl;
        }
    }
}

// Helper method implementations
std::string ActiveOrdersDisplay::formatOrderId(int orderId) const {
    return "#" + std::to_string(orderId);
}

std::string ActiveOrdersDisplay::formatOrderStatus(Order::Status status) const {
    return Order::statusToString(status);
}

std::string ActiveOrdersDisplay::formatOrderTime(const std::shared_ptr<Order>& order) const {
    auto now = std::chrono::system_clock::now();
    auto orderTime = order->getTimestamp();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - orderTime);
    
    int minutes = static_cast<int>(duration.count());
    
    if (minutes < 1) {
        return "Just now";
    } else if (minutes < 60) {
        return std::to_string(minutes) + "m ago";
    } else {
        int hours = minutes / 60;
        int remainingMinutes = minutes % 60;
        return std::to_string(hours) + "h " + std::to_string(remainingMinutes) + "m ago";
    }
}

std::string ActiveOrdersDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "$" << amount;
    return oss.str();
}

void ActiveOrdersDisplay::hideEmptyOrdersMessage() {
    // Handled by refresh() and updateOrdersTable()
}

void ActiveOrdersDisplay::applyTableStyling() {
    if (ordersTable_) {
        ordersTable_->addStyleClass("table w-100 mb-0");
    }
}

void ActiveOrdersDisplay::applyCurrentTheme() {
    // For now, the base theme fixes are applied directly in the styling
    refresh();
}
