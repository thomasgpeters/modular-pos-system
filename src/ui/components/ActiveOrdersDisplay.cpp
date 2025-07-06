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
                                        std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , maxOrdersToDisplay_(DEFAULT_MAX_ORDERS)
    , showCompletedOrders_(false)
    , headerText_(nullptr)
    , orderCountText_(nullptr)
    , ordersTable_(nullptr) {
    
    if (!posService_) {
        throw std::invalid_argument("ActiveOrdersDisplay requires valid POSService");
    }
    
    addStyleClass("active-orders-display");
    
    initializeUI();
    setupEventListeners();
    refresh();
    
    std::cout << "✓ ActiveOrdersDisplay initialized successfully" << std::endl;
}

void ActiveOrdersDisplay::initializeUI() {
    // Create main group container
    auto ordersGroup = addNew<Wt::WGroupBox>("Active Orders");
    ordersGroup->addStyleClass("active-orders-group");
    
    // Create header
    auto header = createDisplayHeader();
    ordersGroup->addWidget(std::move(header));
    
    // Create table container and table
    auto tableContainer = ordersGroup->addNew<Wt::WContainerWidget>();
    tableContainer->addStyleClass("orders-table-container");
    
    // Create table directly in its container
    ordersTable_ = tableContainer->addNew<Wt::WTable>();
    ordersTable_->addStyleClass("table table-striped table-hover orders-table");
    ordersTable_->setWidth(Wt::WLength("100%"));
    
    // Initialize table headers
    initializeTableHeaders();
    
    // Apply styling
    applyTableStyling();
    
    std::cout << "✓ ActiveOrdersDisplay UI initialized" << std::endl;
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
    
    // Style headers
    for (int col = 0; col < ordersTable_->columnCount(); ++col) {
        auto headerCell = ordersTable_->elementAt(0, col);
        headerCell->addStyleClass("table-header bg-primary text-white");
    }
    
    std::cout << "✓ Active orders table created" << std::endl;
}

std::unique_ptr<Wt::WWidget> ActiveOrdersDisplay::createDisplayHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("active-orders-header mb-3");
    
    // Create header layout using containers instead of layouts
    auto headerRow = header->addNew<Wt::WContainerWidget>();
    headerRow->addStyleClass("d-flex justify-content-between align-items-center");
    
    // Title section
    headerText_ = headerRow->addNew<Wt::WText>("📋 Active Orders");
    headerText_->addStyleClass("h5 mb-0");
    
    // Count section
    orderCountText_ = headerRow->addNew<Wt::WText>("0 orders");
    orderCountText_->addStyleClass("badge bg-secondary");
    
    return std::move(header);
}

void ActiveOrdersDisplay::setupEventListeners() {
    if (!eventManager_) {
        std::cout << "[ActiveOrdersDisplay] No EventManager available" << std::endl;
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
        eventManager_->subscribe(POSEvents::ORDER_COMPLETED,
            [this](const std::any& data) { handleOrderCompleted(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CANCELLED,
            [this](const std::any& data) { handleOrderCancelled(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::KITCHEN_STATUS_CHANGED,
            [this](const std::any& data) { handleKitchenStatusChanged(data); }));
    
    std::cout << "✓ ActiveOrdersDisplay event listeners setup complete" << std::endl;
}

void ActiveOrdersDisplay::refresh() {
    if (!posService_) {
        std::cerr << "[ActiveOrdersDisplay] POSService not available for refresh" << std::endl;
        return;
    }
    
    try {
        updateOrdersTable();
        updateOrderCount();
        std::cout << "[ActiveOrdersDisplay] Refreshed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Refresh failed: " << e.what() << std::endl;
    }
}

void ActiveOrdersDisplay::updateOrdersTable() {
    if (!ordersTable_) {
        return;
    }
    
    // Clear existing rows (except header)
    while (ordersTable_->rowCount() > 1) {
        ordersTable_->removeRow(1);
    }
    
    // Get orders to display
    auto orders = getDisplayOrders();
    
    if (orders.empty()) {
        showEmptyOrdersMessage();
        return;
    }
    
    hideEmptyOrdersMessage();
    
    // Add order rows
    for (size_t i = 0; i < orders.size(); ++i) {
        addOrderRow(orders[i], static_cast<int>(i + 1));
    }
    
    // Apply table styling
    applyTableStyling();
    
    std::cout << "[ActiveOrdersDisplay] Updated table with " << orders.size() << " orders" << std::endl;
}

void ActiveOrdersDisplay::addOrderRow(std::shared_ptr<Order> order, int row) {
    if (!order || !ordersTable_) {
        return;
    }
    
    try {
        // Order ID
        auto orderIdText = std::make_unique<Wt::WText>(formatOrderId(order->getOrderId()));
        orderIdText->addStyleClass("order-id fw-bold");
        ordersTable_->elementAt(row, 0)->addWidget(std::move(orderIdText));
        
        // Table/Location
        auto tableText = std::make_unique<Wt::WText>(order->getTableIdentifier());
        tableText->addStyleClass("table-identifier");
        ordersTable_->elementAt(row, 1)->addWidget(std::move(tableText));
        
        // Status with badge
        auto statusText = std::make_unique<Wt::WText>(formatOrderStatus(order->getStatus()));
        statusText->addStyleClass("badge " + getStatusBadgeClass(order->getStatus()));
        ordersTable_->elementAt(row, 2)->addWidget(std::move(statusText));
        
        // Items count
        auto itemsText = std::make_unique<Wt::WText>(std::to_string(order->getItems().size()) + " items");
        itemsText->addStyleClass("items-count text-muted");
        ordersTable_->elementAt(row, 3)->addWidget(std::move(itemsText));
        
        // Total
        auto totalText = std::make_unique<Wt::WText>(formatCurrency(order->getTotal()));
        totalText->addStyleClass("order-total fw-bold text-success");
        ordersTable_->elementAt(row, 4)->addWidget(std::move(totalText));
        
        // Time
        auto timeText = std::make_unique<Wt::WText>(formatOrderTime(order));
        timeText->addStyleClass("order-time text-muted small");
        ordersTable_->elementAt(row, 5)->addWidget(std::move(timeText));
        
        // Actions
        auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
        actionsContainer->addStyleClass("order-actions");
        
        // View button
        auto viewButton = actionsContainer->addNew<Wt::WPushButton>("View");
        viewButton->addStyleClass("btn btn-outline-primary btn-sm me-1");
        viewButton->clicked().connect([this, order]() {
            onViewOrderClicked(order->getOrderId());
        });
        
        // Complete button (only for appropriate statuses)
        if (order->getStatus() == Order::READY) {
            auto completeButton = actionsContainer->addNew<Wt::WPushButton>("Complete");
            completeButton->addStyleClass("btn btn-success btn-sm me-1");
            completeButton->clicked().connect([this, order]() {
                onCompleteOrderClicked(order->getOrderId());
            });
        }
        
        // Cancel button (only for pending orders)
        if (order->getStatus() == Order::PENDING || order->getStatus() == Order::SENT_TO_KITCHEN) {
            auto cancelButton = actionsContainer->addNew<Wt::WPushButton>("Cancel");
            cancelButton->addStyleClass("btn btn-outline-danger btn-sm");
            cancelButton->clicked().connect([this, order]() {
                onCancelOrderClicked(order->getOrderId());
            });
        }
        
        ordersTable_->elementAt(row, 6)->addWidget(std::move(actionsContainer));
        
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error adding order row: " << e.what() << std::endl;
    }
}

// Event handlers
void ActiveOrdersDisplay::handleOrderCreated(const std::any& eventData) {
    refresh();
    std::cout << "[ActiveOrdersDisplay] Order created event handled" << std::endl;
}

void ActiveOrdersDisplay::handleOrderModified(const std::any& eventData) {
    refresh();
    std::cout << "[ActiveOrdersDisplay] Order modified event handled" << std::endl;
}

void ActiveOrdersDisplay::handleOrderCompleted(const std::any& eventData) {
    refresh();
    std::cout << "[ActiveOrdersDisplay] Order completed event handled" << std::endl;
}

void ActiveOrdersDisplay::handleOrderCancelled(const std::any& eventData) {
    refresh();
    std::cout << "[ActiveOrdersDisplay] Order cancelled event handled" << std::endl;
}

void ActiveOrdersDisplay::handleKitchenStatusChanged(const std::any& eventData) {
    refresh();
    std::cout << "[ActiveOrdersDisplay] Kitchen status changed event handled" << std::endl;
}

// UI action handlers
void ActiveOrdersDisplay::onViewOrderClicked(int orderId) {
    std::cout << "[ActiveOrdersDisplay] View order clicked: " << orderId << std::endl;
    // TODO: Implement order detail view
}

void ActiveOrdersDisplay::onCompleteOrderClicked(int orderId) {
    if (!posService_) {
        return;
    }
    
    try {
        // Update order status to served
        bool success = posService_->getOrderById(orderId) != nullptr;
        if (success) {
            auto order = posService_->getOrderById(orderId);
            if (order) {
                order->setStatus(Order::SERVED);
                refresh();
                std::cout << "[ActiveOrdersDisplay] Order " << orderId << " completed" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error completing order: " << e.what() << std::endl;
    }
}

void ActiveOrdersDisplay::onCancelOrderClicked(int orderId) {
    if (!posService_) {
        return;
    }
    
    try {
        bool success = posService_->cancelOrder(orderId);
        if (success) {
            refresh();
            std::cout << "[ActiveOrdersDisplay] Order " << orderId << " cancelled" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error cancelling order: " << e.what() << std::endl;
    }
}

// Helper methods
std::vector<std::shared_ptr<Order>> ActiveOrdersDisplay::getDisplayOrders() const {
    if (!posService_) {
        return {};
    }
    
    try {
        auto orders = posService_->getActiveOrders();
        
        // Filter completed orders if not showing them
        if (!showCompletedOrders_) {
            orders.erase(std::remove_if(orders.begin(), orders.end(),
                [](const std::shared_ptr<Order>& order) {
                    return order && order->getStatus() == Order::SERVED;
                }), orders.end());
        }
        
        // Limit to max orders
        if (maxOrdersToDisplay_ > 0 && orders.size() > static_cast<size_t>(maxOrdersToDisplay_)) {
            orders.resize(maxOrdersToDisplay_);
        }
        
        return orders;
        
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error getting display orders: " << e.what() << std::endl;
        return {};
    }
}

std::string ActiveOrdersDisplay::formatOrderId(int orderId) const {
    return "#" + std::to_string(orderId);
}

std::string ActiveOrdersDisplay::formatOrderStatus(Order::Status status) const {
    return Order::statusToString(status);
}

std::string ActiveOrdersDisplay::formatOrderTime(const std::shared_ptr<Order>& order) const {
    if (!order) {
        return "Unknown";
    }
    
    auto timestamp = order->getTimestamp();
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%H:%M");
    return oss.str();
}

std::string ActiveOrdersDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << "$" << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}

std::string ActiveOrdersDisplay::getStatusBadgeClass(Order::Status status) const {
    switch (status) {
        case Order::PENDING:         return "bg-secondary";
        case Order::SENT_TO_KITCHEN: return "bg-primary";
        case Order::PREPARING:       return "bg-warning";
        case Order::READY:           return "bg-info";
        case Order::SERVED:          return "bg-success";
        case Order::CANCELLED:       return "bg-danger";
        default:                     return "bg-light";
    }
}

void ActiveOrdersDisplay::showEmptyOrdersMessage() {
    if (!ordersTable_) {
        return;
    }
    
    int row = ordersTable_->rowCount();
    auto messageText = std::make_unique<Wt::WText>("No active orders");
    messageText->addStyleClass("text-muted text-center p-3");
    
    auto cell = ordersTable_->elementAt(row, 0);
    cell->setColumnSpan(7); // Span all columns
    cell->addWidget(std::move(messageText));
}

void ActiveOrdersDisplay::hideEmptyOrdersMessage() {
    // Empty message will be removed when table is cleared
}

void ActiveOrdersDisplay::applyTableStyling() {
    if (!ordersTable_) {
        return;
    }
    
    // Set column widths
    if (ordersTable_->rowCount() > 0) {
        ordersTable_->elementAt(0, 0)->setWidth(Wt::WLength("10%")); // Order #
        ordersTable_->elementAt(0, 1)->setWidth(Wt::WLength("15%")); // Table
        ordersTable_->elementAt(0, 2)->setWidth(Wt::WLength("15%")); // Status
        ordersTable_->elementAt(0, 3)->setWidth(Wt::WLength("10%")); // Items
        ordersTable_->elementAt(0, 4)->setWidth(Wt::WLength("12%")); // Total
        ordersTable_->elementAt(0, 5)->setWidth(Wt::WLength("10%")); // Time
        ordersTable_->elementAt(0, 6)->setWidth(Wt::WLength("28%")); // Actions
    }
}

void ActiveOrdersDisplay::updateOrderCount() {
    if (!orderCountText_) {
        return;
    }
    
    auto orders = getDisplayOrders();
    std::string countText = std::to_string(orders.size()) + " orders";
    orderCountText_->setText(countText);
}

// Configuration methods
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
