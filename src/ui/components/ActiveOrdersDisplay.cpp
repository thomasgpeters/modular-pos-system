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
    
    // Apply consistent container styling
    UIStyleHelper::styleOrderDisplay(this);
    
    initializeUI();
    setupEventListeners();
    refresh();
    
    std::cout << "✓ ActiveOrdersDisplay initialized with consistent styling" << std::endl;
}

void ActiveOrdersDisplay::initializeUI() {
    // Create main group container with consistent styling
    auto ordersGroup = addNew<Wt::WGroupBox>("📋 Active Orders");
    UIStyleHelper::styleGroupBox(ordersGroup, "primary");
    
    // Create header with consistent styling
    auto header = createDisplayHeader();
    ordersGroup->addWidget(std::move(header));
    
    // Create table container with consistent layout
    auto tableContainer = ordersGroup->addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleContainer(tableContainer, "content");
    
    // Create table with consistent styling
    ordersTable_ = tableContainer->addNew<Wt::WTable>();
    UIStyleHelper::styleTable(ordersTable_, "orders");
    
    // Initialize table headers
    initializeTableHeaders();
    
    // Add fade-in animation
    UIStyleHelper::addFadeIn(this);
    
    std::cout << "✓ ActiveOrdersDisplay UI initialized with consistent styling" << std::endl;
}

void ActiveOrdersDisplay::initializeTableHeaders() {
    if (!ordersTable_) {
        return;
    }
    
    // Create styled table headers
    ordersTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Order #"));
    ordersTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Table/Location"));
    ordersTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Status"));
    ordersTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Items"));
    ordersTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Total"));
    ordersTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Time"));
    ordersTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Actions"));
    
    // Apply consistent header styling
    for (int col = 0; col < ordersTable_->columnCount(); ++col) {
        auto headerCell = ordersTable_->elementAt(0, col);
        headerCell->addStyleClass("table-header bg-primary text-white fw-bold text-center p-2");
        auto headerText = dynamic_cast<Wt::WText*>(headerCell->widget(0));
        if (headerText) {
            UIStyleHelper::styleHeading(headerText, 6, "white");
            headerText->removeStyleClass("mb-3"); // Remove default margin for table headers
        }
    }
    
    std::cout << "✓ Active orders table headers styled consistently" << std::endl;
}

std::unique_ptr<Wt::WWidget> ActiveOrdersDisplay::createDisplayHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    UIStyleHelper::styleContainer(header.get(), "header");
    UIStyleHelper::styleFlexRow(header.get(), "between", "center");
    
    // Title with consistent styling
    headerText_ = header->addNew<Wt::WText>("📋 Active Orders");
    UIStyleHelper::styleHeading(headerText_, 4, "white");
    headerText_->removeStyleClass("mb-3"); // Remove margin in header
    
    // Count badge with consistent styling
    orderCountText_ = header->addNew<Wt::WText>("0 orders");
    UIStyleHelper::styleBadge(orderCountText_, "secondary");
    orderCountText_->addStyleClass("badge-pill fs-6");
    
    return std::move(header);
}

void ActiveOrdersDisplay::addOrderRow(std::shared_ptr<Order> order, int row) {
    if (!order || !ordersTable_) {
        return;
    }
    
    try {
        // Order ID with consistent styling
        auto orderIdText = std::make_unique<Wt::WText>(formatOrderId(order->getOrderId()));
        orderIdText->addStyleClass("fw-bold text-primary pos-order-id");
        ordersTable_->elementAt(row, 0)->addWidget(std::move(orderIdText));
        
        // Table/Location with consistent styling
        auto tableText = std::make_unique<Wt::WText>(order->getTableIdentifier());
        tableText->addStyleClass("text-dark fw-medium pos-table-id");
        ordersTable_->elementAt(row, 1)->addWidget(std::move(tableText));
        
        // Status badge with consistent styling
        auto statusText = std::make_unique<Wt::WText>(formatOrderStatus(order->getStatus()));
        UIStyleHelper::styleBadge(statusText.get(), getStatusBadgeVariant(order->getStatus()));
        statusText->addStyleClass("pos-status-badge");
        ordersTable_->elementAt(row, 2)->addWidget(std::move(statusText));
        
        // Items count with consistent styling
        auto itemsText = std::make_unique<Wt::WText>(std::to_string(order->getItems().size()) + " items");
        itemsText->addStyleClass("text-muted small pos-item-count");
        ordersTable_->elementAt(row, 3)->addWidget(std::move(itemsText));
        
        // Total with consistent styling
        auto totalText = std::make_unique<Wt::WText>(formatCurrency(order->getTotal()));
        totalText->addStyleClass("fw-bold text-success pos-order-total");
        ordersTable_->elementAt(row, 4)->addWidget(std::move(totalText));
        
        // Time with consistent styling
        auto timeText = std::make_unique<Wt::WText>(formatOrderTime(order));
        timeText->addStyleClass("text-muted small pos-order-time");
        ordersTable_->elementAt(row, 5)->addWidget(std::move(timeText));
        
        // Actions with consistent styling
        auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
        UIStyleHelper::styleFlexRow(actionsContainer.get(), "start", "center");
        actionsContainer->addStyleClass("pos-action-buttons");
        
        // View button with consistent styling
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
        
        // Apply consistent row styling
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
        
        // Base cell styling
        cell->addStyleClass("pos-table-cell p-2 align-middle");
        
        // Alternating row colors
        if (isEven) {
            cell->addStyleClass("table-row-even");
        } else {
            cell->addStyleClass("table-row-odd");
        }
        
        // Column-specific alignment
        if (col == 0 || col == 2 || col == 3 || col == 4 || col == 5) { // Order#, Status, Items, Total, Time
            cell->addStyleClass("text-center");
        } else if (col == 6) { // Actions
            cell->addStyleClass("text-start");
        }
        
        // Add hover effect
        UIStyleHelper::addHoverEffect(cell, "shadow");
    }
}

void ActiveOrdersDisplay::showEmptyOrdersMessage() {
    if (!ordersTable_) {
        return;
    }
    
    int row = ordersTable_->rowCount();
    auto messageContainer = std::make_unique<Wt::WContainerWidget>();
    
    // Style the empty message container
    UIStyleHelper::styleContainer(messageContainer.get(), "content");
    messageContainer->addStyleClass("text-center py-5");
    
    auto messageText = messageContainer->addNew<Wt::WText>("📝 No active orders");
    UIStyleHelper::styleHeading(messageText, 5, "muted");
    
    auto subText = messageContainer->addNew<Wt::WText>("Orders will appear here when created");
    subText->addStyleClass("text-muted small");
    
    auto cell = ordersTable_->elementAt(row, 0);
    cell->setColumnSpan(7); // Span all columns
    cell->addWidget(std::move(messageContainer));
    cell->addStyleClass("pos-empty-state");
}

void ActiveOrdersDisplay::updateOrderCount() {
    if (!orderCountText_) {
        return;
    }
    
    auto orders = getDisplayOrders();
    std::string countText = std::to_string(orders.size()) + " orders";
    orderCountText_->setText(countText);
    
    // Update badge color based on count
    orderCountText_->removeStyleClass("bg-secondary bg-warning bg-danger");
    if (orders.size() == 0) {
        UIStyleHelper::styleBadge(orderCountText_, "secondary");
    } else if (orders.size() > 10) {
        UIStyleHelper::styleBadge(orderCountText_, "danger");
    } else if (orders.size() > 5) {
        UIStyleHelper::styleBadge(orderCountText_, "warning");
    } else {
        UIStyleHelper::styleBadge(orderCountText_, "info");
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
// MISSING METHOD IMPLEMENTATIONS
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

// Event handlers (placeholder implementations)
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

// UI action handlers (placeholder implementations)
void ActiveOrdersDisplay::onViewOrderClicked(int orderId) {
    std::cout << "[ActiveOrdersDisplay] View order #" << orderId << " (not implemented)" << std::endl;
}

void ActiveOrdersDisplay::onCompleteOrderClicked(int orderId) {
    std::cout << "[ActiveOrdersDisplay] Complete order #" << orderId << " (not implemented)" << std::endl;
    // TODO: Implement order completion logic
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
    // This method can be used to explicitly hide empty state
    // Currently handled by refresh() and updateOrdersTable()
}

void ActiveOrdersDisplay::applyTableStyling() {
    if (ordersTable_) {
        UIStyleHelper::styleTable(ordersTable_, "orders");
    }
}
