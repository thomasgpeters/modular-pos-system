#include "../../../include/ui/components/ActiveOrdersDisplay.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WTableCell.h>
#include <Wt/WMessageBox.h>
#include <Wt/WApplication.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

ActiveOrdersDisplay::ActiveOrdersDisplay(std::shared_ptr<POSService> posService,
                                        std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , maxOrdersToDisplay_(DEFAULT_MAX_ORDERS)
    , showCompletedOrders_(false)
    , headerText_(nullptr)
    , orderCountText_(nullptr)
    , ordersTable_(nullptr) {
    
    initializeUI();
    setupEventListeners();
    refresh();
}

void ActiveOrdersDisplay::initializeUI() {
    addStyleClass("active-orders-display card");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Display header
    auto header = createDisplayHeader();
    layout->addWidget(std::move(header));
    
    // FIXED: Orders table - create and add properly
    createOrdersTable();
    // The table is already added to 'this' container in createOrdersTable()
    // No need to add it to layout again
    
    setLayout(std::move(layout));
    
    std::cout << "✓ ActiveOrdersDisplay UI initialized" << std::endl;
}

void ActiveOrdersDisplay::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "Warning: EventManager not available for ActiveOrdersDisplay" << std::endl;
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

// Fix for ActiveOrdersDisplay.cpp - createDisplayHeader method

std::unique_ptr<Wt::WWidget> ActiveOrdersDisplay::createDisplayHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("card-header active-orders-header");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Title section
    auto titleContainer = std::make_unique<Wt::WContainerWidget>();
    auto titleLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // FIXED: Create and add header text properly
    headerText_ = titleLayout->addWidget(std::make_unique<Wt::WText>("Active Orders"));
    headerText_->addStyleClass("h5 mb-1");
    
    // FIXED: Create and add order count text properly
    orderCountText_ = titleLayout->addWidget(std::make_unique<Wt::WText>(""));
    orderCountText_->addStyleClass("small text-muted");
    
    titleContainer->setLayout(std::move(titleLayout));
    layout->addWidget(std::move(titleContainer), 1);
    
    header->setLayout(std::move(layout));
    return std::move(header);
}

void ActiveOrdersDisplay::createOrdersTable() {
    // FIXED: Create table and add it to the main container, not to a layout
    ordersTable_ = addWidget(std::make_unique<Wt::WTable>());
    ordersTable_->addStyleClass("table table-sm table-hover active-orders-table");
    ordersTable_->setHeaderCount(1);
    
    // Set up table headers
    ordersTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Order #"));
    ordersTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Table"));
    ordersTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Items"));
    ordersTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));
    ordersTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Status"));
    ordersTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Time"));
    ordersTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Actions"));
    
    applyTableStyling();
    
    std::cout << "✓ Active orders table created" << std::endl;
}

void ActiveOrdersDisplay::refresh() {
    if (!posService_) {
        std::cerr << "Error: POSService not available for orders refresh" << std::endl;
        return;
    }
    
    updateOrdersTable();
    updateOrderCount();
    
    std::cout << "ActiveOrdersDisplay refreshed" << std::endl;
}

void ActiveOrdersDisplay::setMaxOrdersToDisplay(int maxOrders) {
    maxOrdersToDisplay_ = maxOrders;
    refresh();
    std::cout << "Max orders to display set to: " << maxOrders << std::endl;
}

int ActiveOrdersDisplay::getMaxOrdersToDisplay() const {
    return maxOrdersToDisplay_;
}

void ActiveOrdersDisplay::setShowCompletedOrders(bool showCompleted) {
    showCompletedOrders_ = showCompleted;
    refresh();
    std::cout << "Show completed orders: " << (showCompleted ? "enabled" : "disabled") << std::endl;
}

bool ActiveOrdersDisplay::getShowCompletedOrders() const {
    return showCompletedOrders_;
}

void ActiveOrdersDisplay::updateOrdersTable() {
    if (!ordersTable_) {
        return;
    }
    
    // Clear existing rows (except header)
    while (ordersTable_->rowCount() > 1) {
        ordersTable_->removeRow(1);
    }
    
    auto orders = getDisplayOrders();
    
    if (orders.empty()) {
        showEmptyOrdersMessage();
        return;
    }
    
    hideEmptyOrdersMessage();
    
    // Add order rows
    int row = 1;
    for (const auto& order : orders) {
        addOrderRow(order, row);
        row++;
        
        // Respect max orders limit
        if (maxOrdersToDisplay_ > 0 && row > maxOrdersToDisplay_) {
            break;
        }
    }
    
    std::cout << "Orders table updated with " << (row - 1) << " orders" << std::endl;
}

// Fix the addOrderRow method to use correct enum values
void ActiveOrdersDisplay::addOrderRow(std::shared_ptr<Order> order, int row) {
    if (!order) return;
    
    // Order ID - FIXED: use getOrderId()
    auto orderIdText = std::make_unique<Wt::WText>("#" + std::to_string(order->getOrderId()));
    orderIdText->addStyleClass("font-weight-bold");
    ordersTable_->elementAt(row, 0)->addWidget(std::move(orderIdText));
    
    // Table number
    auto tableText = std::make_unique<Wt::WText>(std::to_string(order->getTableNumber()));
    ordersTable_->elementAt(row, 1)->addWidget(std::move(tableText));
    
    // Item count
    auto itemCountText = std::make_unique<Wt::WText>(std::to_string(order->getItems().size()));
    ordersTable_->elementAt(row, 2)->addWidget(std::move(itemCountText));
    
    // Total amount
    auto totalText = std::make_unique<Wt::WText>(formatCurrency(order->getTotal()));
    totalText->addStyleClass("font-weight-bold");
    ordersTable_->elementAt(row, 3)->addWidget(std::move(totalText));
    
    // Status badge
    auto statusBadge = std::make_unique<Wt::WText>(formatOrderStatus(order->getStatus()));
    statusBadge->addStyleClass("badge " + getStatusBadgeClass(order->getStatus()));
    ordersTable_->elementAt(row, 4)->addWidget(std::move(statusBadge));
    
    // Time
    auto timeText = std::make_unique<Wt::WText>(formatOrderTime(order));
    timeText->addStyleClass("small text-muted");
    ordersTable_->elementAt(row, 5)->addWidget(std::move(timeText));
    
    // Actions - FIXED: Use correct enum values
    auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
    auto actionsLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // View button
    auto viewButton = std::make_unique<Wt::WPushButton>("👁");
    viewButton->addStyleClass("btn btn-sm btn-outline-info me-1");
    viewButton->setToolTip("View order details");
    viewButton->clicked().connect([this, orderId = order->getOrderId()]() {  // FIXED: getOrderId()
        onViewOrderClicked(orderId);
    });
    actionsLayout->addWidget(std::move(viewButton));
    
    // Complete button (only for non-served orders)
    if (order->getStatus() != Order::Status::SERVED &&     // FIXED: was COMPLETED
        order->getStatus() != Order::Status::CANCELLED) {
        auto completeButton = std::make_unique<Wt::WPushButton>("✓");
        completeButton->addStyleClass("btn btn-sm btn-outline-success me-1");
        completeButton->setToolTip("Mark as served");       // FIXED: text
        completeButton->clicked().connect([this, orderId = order->getOrderId()]() {  // FIXED: getOrderId()
            onCompleteOrderClicked(orderId);
        });
        actionsLayout->addWidget(std::move(completeButton));
    }
    
    // Cancel button (only for pending/in-kitchen orders)
    if (order->getStatus() == Order::Status::PENDING || 
        order->getStatus() == Order::Status::SENT_TO_KITCHEN) {
        auto cancelButton = std::make_unique<Wt::WPushButton>("✕");
        cancelButton->addStyleClass("btn btn-sm btn-outline-danger");
        cancelButton->setToolTip("Cancel order");
        cancelButton->clicked().connect([this, orderId = order->getOrderId()]() {  // FIXED: getOrderId()
            onCancelOrderClicked(orderId);
        });
        actionsLayout->addWidget(std::move(cancelButton));
    }
    
    actionsContainer->setLayout(std::move(actionsLayout));
    ordersTable_->elementAt(row, 6)->addWidget(std::move(actionsContainer));
    
    // Apply row styling
    std::string rowClass = ((row % 2) == 0) ? "table-row-even" : "table-row-odd";
    for (int col = 0; col < 7; ++col) {
        auto cell = ordersTable_->elementAt(row, col);
        cell->addStyleClass("order-cell " + rowClass);
    }
}

// =================================================================
// Event Handlers
// =================================================================

void ActiveOrdersDisplay::handleOrderCreated(const std::any& eventData) {
    std::cout << "Order created event received in ActiveOrdersDisplay" << std::endl;
    refresh();
}

void ActiveOrdersDisplay::handleOrderModified(const std::any& eventData) {
    std::cout << "Order modified event received in ActiveOrdersDisplay" << std::endl;
    refresh();
}

void ActiveOrdersDisplay::handleOrderCompleted(const std::any& eventData) {
    std::cout << "Order completed event received in ActiveOrdersDisplay" << std::endl;
    refresh();
}

void ActiveOrdersDisplay::handleOrderCancelled(const std::any& eventData) {
    std::cout << "Order cancelled event received in ActiveOrdersDisplay" << std::endl;
    refresh();
}

void ActiveOrdersDisplay::handleKitchenStatusChanged(const std::any& eventData) {
    std::cout << "Kitchen status changed event received in ActiveOrdersDisplay" << std::endl;
    refresh();
}

// =================================================================
// UI Action Handlers
// =================================================================

void ActiveOrdersDisplay::onViewOrderClicked(int orderId) {
    if (!posService_) {
        return;
    }
    
    auto order = posService_->getOrderById(orderId);
    if (!order) {
        std::cerr << "Order not found: " << orderId << std::endl;
        return;
    }
    
    // Create a simple order details dialog
    auto messageBox = addChild(std::make_unique<Wt::WMessageBox>(
        "Order Details",
        "Order #" + std::to_string(orderId) + " for Table " + 
        std::to_string(order->getTableNumber()) + "\n\n" +
        "Items: " + std::to_string(order->getItems().size()) + "\n" +
        "Total: " + formatCurrency(order->getTotal()) + "\n" +
        "Status: " + formatOrderStatus(order->getStatus()),
        Wt::Icon::Information,
        Wt::StandardButton::Ok));
    
    messageBox->buttonClicked().connect([this, messageBox](Wt::StandardButton) {
        removeChild(messageBox);
    });
    
    messageBox->show();
    
    std::cout << "Viewing order details for order #" << orderId << std::endl;
}

void ActiveOrdersDisplay::onCancelOrderClicked(int orderId) {
    if (!posService_) {
        return;
    }
    
    auto messageBox = addChild(std::make_unique<Wt::WMessageBox>(
        "Cancel Order",
        "Are you sure you want to cancel Order #" + std::to_string(orderId) + "?",
        Wt::Icon::Question,
        Wt::StandardButton::Yes | Wt::StandardButton::No));
    
    messageBox->buttonClicked().connect([this, messageBox, orderId](Wt::StandardButton button) {
        if (button == Wt::StandardButton::Yes) {
            bool success = posService_->cancelOrder(orderId);
            if (success) {
                std::cout << "Order #" << orderId << " cancelled successfully" << std::endl;
            } else {
                std::cerr << "Failed to cancel order #" << orderId << std::endl;
            }
        }
        removeChild(messageBox);
    });
    
    messageBox->show();
}

// If you have any business logic methods that need to handle order completion:
void ActiveOrdersDisplay::onCompleteOrderClicked(int orderId) {
    if (!posService_) {
        return;
    }
    
    auto messageBox = addChild(std::make_unique<Wt::WMessageBox>(
        "Mark as Served",  // FIXED: title
        "Mark Order #" + std::to_string(orderId) + " as served?",  // FIXED: text
        Wt::Icon::Question,
        Wt::StandardButton::Yes | Wt::StandardButton::No));
    
    messageBox->buttonClicked().connect([this, messageBox, orderId](Wt::StandardButton button) {
        if (button == Wt::StandardButton::Yes) {
            // Get the order and update its status
            auto order = posService_->getOrderById(orderId);
            if (order) {
                order->setStatus(Order::Status::SERVED);  // FIXED: Set to SERVED
                std::cout << "Order #" << orderId << " marked as served" << std::endl;
                refresh();  // Refresh the display
            } else {
                std::cerr << "Failed to find order #" << orderId << std::endl;
            }
        }
        removeChild(messageBox);
    });
    
    messageBox->show();
}

// =================================================================
// Helper Methods
// =================================================================

// Fix the getDisplayOrders method to use correct enum values
std::vector<std::shared_ptr<Order>> ActiveOrdersDisplay::getDisplayOrders() const {
    if (!posService_) {
        return {};
    }
    
    auto orders = posService_->getActiveOrders();
    
    // Filter completed/cancelled orders if not showing them
    if (!showCompletedOrders_) {
        orders.erase(std::remove_if(orders.begin(), orders.end(),
            [](const std::shared_ptr<Order>& order) {
                return order->getStatus() == Order::Status::SERVED ||      // FIXED: was COMPLETED
                       order->getStatus() == Order::Status::CANCELLED;
            }), orders.end());
    }
    
    // Sort by order ID (most recent first)
    std::sort(orders.begin(), orders.end(),
        [](const std::shared_ptr<Order>& a, const std::shared_ptr<Order>& b) {
            return a->getOrderId() > b->getOrderId();  // FIXED: use getOrderId() not getId()
        });
    
    return orders;
}

std::string ActiveOrdersDisplay::formatOrderStatus(Order::Status status) const {
    switch (status) {
        case Order::Status::PENDING:          return "Pending";
        case Order::Status::SENT_TO_KITCHEN:  return "In Kitchen";
        case Order::Status::PREPARING:        return "Preparing";      // FIXED: was IN_PREPARATION
        case Order::Status::READY:            return "Ready";          // FIXED: was READY_TO_SERVE
        case Order::Status::SERVED:           return "Served";
        case Order::Status::CANCELLED:        return "Cancelled";
        default:                              return "Unknown";
    }
}

std::string ActiveOrdersDisplay::formatOrderTime(const std::shared_ptr<Order>& order) const {
    // This would format the order creation time
    // For now, return a placeholder
    return "15:30";
}

std::string ActiveOrdersDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << "$" << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}

std::string ActiveOrdersDisplay::getStatusBadgeClass(Order::Status status) const {
    switch (status) {
        case Order::Status::PENDING:          return "badge-secondary";
        case Order::Status::SENT_TO_KITCHEN:  return "badge-primary";
        case Order::Status::PREPARING:        return "badge-warning";   // FIXED: was IN_PREPARATION
        case Order::Status::READY:            return "badge-info";      // FIXED: was READY_TO_SERVE
        case Order::Status::SERVED:           return "badge-success";
        case Order::Status::CANCELLED:        return "badge-danger";
        default:                              return "badge-light";
    }
}

void ActiveOrdersDisplay::showEmptyOrdersMessage() {
    if (!ordersTable_ || ordersTable_->rowCount() > 1) {
        return;
    }
    
    int row = ordersTable_->rowCount();
    auto emptyMessage = std::make_unique<Wt::WText>("No active orders");
    emptyMessage->addStyleClass("text-muted text-center py-3");
    
    ordersTable_->elementAt(row, 0)->addWidget(std::move(emptyMessage));
    ordersTable_->elementAt(row, 0)->setColumnSpan(7);
}

void ActiveOrdersDisplay::hideEmptyOrdersMessage() {
    // Empty message is automatically hidden when real orders are added
}

void ActiveOrdersDisplay::applyTableStyling() {
    if (!ordersTable_) {
        return;
    }
    
    // Style table headers
    for (int col = 0; col < 7; ++col) {
        auto headerCell = ordersTable_->elementAt(0, col);
        headerCell->addStyleClass("table-header bg-light font-weight-bold");
        
        // Set column widths
        switch (col) {
            case 0: // Order #
                headerCell->setWidth(Wt::WLength("12%"));
                break;
            case 1: // Table
                headerCell->setWidth(Wt::WLength("8%"));
                break;
            case 2: // Items
                headerCell->setWidth(Wt::WLength("8%"));
                break;
            case 3: // Total
                headerCell->setWidth(Wt::WLength("12%"));
                break;
            case 4: // Status
                headerCell->setWidth(Wt::WLength("15%"));
                break;
            case 5: // Time
                headerCell->setWidth(Wt::WLength("15%"));
                break;
            case 6: // Actions
                headerCell->setWidth(Wt::WLength("30%"));
                break;
        }
    }
}

void ActiveOrdersDisplay::updateOrderCount() {
    if (!orderCountText_) {
        return;
    }
    
    auto orders = getDisplayOrders();
    int totalOrders = static_cast<int>(orders.size());
    int displayedOrders = std::min(totalOrders, maxOrdersToDisplay_);
    
    std::string countText;
    if (maxOrdersToDisplay_ > 0 && totalOrders > maxOrdersToDisplay_) {
        countText = "Showing " + std::to_string(displayedOrders) + " of " + 
                   std::to_string(totalOrders) + " orders";
    } else {
        countText = std::to_string(totalOrders) + " orders";
    }
    
    orderCountText_->setText(countText);
}
