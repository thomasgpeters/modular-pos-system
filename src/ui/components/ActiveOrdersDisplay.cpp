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

// Helper method to get consistent badge variants for order status
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

// Rest of the methods remain the same, but with consistent styling applied...
// (Event handlers, business logic methods, etc. remain unchanged)
