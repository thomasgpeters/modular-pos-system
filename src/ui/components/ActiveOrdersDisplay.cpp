// ============================================================================
// Complete ActiveOrdersDisplay.cpp - All Methods Implemented
// This should resolve the vtable error by implementing every method
// ============================================================================

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
    : Wt::WContainerWidget()  
    , posService_(posService)
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
    
    addStyleClass("pos-active-orders-container h-100");
    
    initializeUI();
    setupEventListeners();
    loadOrdersFromAPI();
    
    std::cout << "✓ ActiveOrdersDisplay initialized with clean design" << std::endl;

    // Force visibility and proper sizing
    show();
    setHidden(false);
    
    // Add some debug styling to make it visible
    addStyleClass("border border-primary"); // Temporary - to see if it's there
    
    // Force minimum height so it's visible even when empty
    setMinimumSize(300, 200);
    
    std::cout << "✓ ActiveOrdersDisplay initialized with forced visibility" << std::endl;
}

// REQUIRED: Virtual destructor implementation (declared in header)
ActiveOrdersDisplay::~ActiveOrdersDisplay() {
    std::cout << "[ActiveOrdersDisplay] Destructor called - cleaning up" << std::endl;
    
    try {
        // Cleanup event subscriptions
        if (eventManager_) {
            for (auto handle : eventSubscriptions_) {
                eventManager_->unsubscribe(handle);
            }
            eventSubscriptions_.clear();
        }
        
        // Clear UI component pointers (Wt will handle actual widget destruction)
        headerText_ = nullptr;
        orderCountText_ = nullptr;
        ordersTable_ = nullptr;
        
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error during destruction: " << e.what() << std::endl;
    }
}

// ============================================================================
// UI INITIALIZATION METHODS
// ============================================================================

// ============================================================================
// And in your initializeUI() method, make sure the main container is visible:
// ============================================================================

void ActiveOrdersDisplay::initializeUI() {
    auto mainContainer = addNew<Wt::WContainerWidget>();
    mainContainer->addStyleClass("pos-orders-main h-100");
    
    // FORCE VISIBILITY
    mainContainer->show();
    mainContainer->setHidden(false);
    
    if (showHeader_) {
        auto header = createDisplayHeader();
        mainContainer->addWidget(std::move(header));
    }
    
    ordersTable_ = mainContainer->addNew<Wt::WTable>();
    ordersTable_->addStyleClass("table pos-orders-table w-100");
    ordersTable_->setWidth(Wt::WLength("100%"));
    
    // FORCE TABLE VISIBILITY
    ordersTable_->show();
    ordersTable_->setHidden(false);

    initializeTableHeaders();
    showLoadingState();
    
    std::cout << "✓ ActiveOrdersDisplay UI initialized with forced visibility" << std::endl;
}

std::unique_ptr<Wt::WWidget> ActiveOrdersDisplay::createDisplayHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("pos-section-header d-flex justify-content-between align-items-center");
    
    headerText_ = header->addNew<Wt::WText>("📋 Active Orders");
    headerText_->addStyleClass("h4 mb-0 fw-bold");
    
    orderCountText_ = header->addNew<Wt::WText>("Loading...");
    orderCountText_->addStyleClass("badge bg-info px-3 py-2 rounded-pill");
    
    return std::move(header);
}

void ActiveOrdersDisplay::initializeTableHeaders() {
    if (!ordersTable_) {
        return;
    }
    
    ordersTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Order #"));
    ordersTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Table/Location"));
    ordersTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Status"));
    ordersTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Items"));
    ordersTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Total"));
    ordersTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Time"));
    ordersTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Actions"));
    
    for (int col = 0; col < ordersTable_->columnCount(); ++col) {
        auto headerCell = ordersTable_->elementAt(0, col);
        headerCell->addStyleClass("pos-table-header");
        
        auto headerText = dynamic_cast<Wt::WText*>(headerCell->widget(0));
        if (headerText) {
            headerText->addStyleClass("fw-bold");
        }
    }
}

void ActiveOrdersDisplay::setupEventListeners() {
    if (!eventManager_) return;
    
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

// ============================================================================
// API INTEGRATION METHODS
// ============================================================================

void ActiveOrdersDisplay::loadOrdersFromAPI() {
    auto enhancedService = std::dynamic_pointer_cast<EnhancedPOSService>(posService_);
    
    if (enhancedService && enhancedService->isConnected()) {
        std::cout << "[ActiveOrdersDisplay] Loading orders from API..." << std::endl;
        
        enhancedService->getActiveOrdersAsync([this](std::vector<std::shared_ptr<Order>> orders, bool success) {
            if (success) {
                std::cout << "[ActiveOrdersDisplay] Loaded " << orders.size() << " orders from API" << std::endl;
                displayAPIOrders(orders);
            } else {
                std::cout << "[ActiveOrdersDisplay] Failed to load from API, falling back to local data" << std::endl;
                displayLocalOrders();
            }
        });
    } else {
        std::cout << "[ActiveOrdersDisplay] Enhanced service not available, using local data" << std::endl;
        displayLocalOrders();
    }
}

void ActiveOrdersDisplay::displayAPIOrders(const std::vector<std::shared_ptr<Order>>& orders) {
    clearLoadingState();
    
    while (ordersTable_->rowCount() > 1) {
        ordersTable_->removeRow(1);
    }
    
    if (orders.empty()) {
        showEmptyOrdersMessage();
        updateOrderCountDirect(0);
        return;
    }
    
    auto filteredOrders = filterOrders(orders);
    
    for (size_t i = 0; i < filteredOrders.size(); ++i) {
        if (maxOrdersToDisplay_ > 0 && i >= static_cast<size_t>(maxOrdersToDisplay_)) {
            break;
        }
        addOrderRow(filteredOrders[i], static_cast<int>(i + 1));
    }
    
    updateOrderCountDirect(static_cast<int>(filteredOrders.size()));
    std::cout << "[ActiveOrdersDisplay] Displayed " << filteredOrders.size() << " orders from API" << std::endl;
}

void ActiveOrdersDisplay::displayLocalOrders() {
    auto orders = getDisplayOrders();
    
    clearLoadingState();
    
    while (ordersTable_->rowCount() > 1) {
        ordersTable_->removeRow(1);
    }
    
    if (orders.empty()) {
        showEmptyOrdersMessage();
        updateOrderCountDirect(0);
        return;
    }
    
    for (size_t i = 0; i < orders.size(); ++i) {
        if (maxOrdersToDisplay_ > 0 && i >= static_cast<size_t>(maxOrdersToDisplay_)) {
            break;
        }
        addOrderRow(orders[i], static_cast<int>(i + 1));
    }
    
    updateOrderCountDirect(static_cast<int>(orders.size()));
    std::cout << "[ActiveOrdersDisplay] Displayed " << orders.size() << " orders from local data" << std::endl;
}

std::vector<std::shared_ptr<Order>> ActiveOrdersDisplay::filterOrders(const std::vector<std::shared_ptr<Order>>& orders) const {
    std::vector<std::shared_ptr<Order>> filteredOrders;
    
    for (const auto& order : orders) {
        if (!order) continue;
        
        if (!showCompletedOrders_) {
            if (order->getStatus() == Order::SERVED || order->getStatus() == Order::CANCELLED) {
                continue;
            }
        }
        
        filteredOrders.push_back(order);
    }
    
    std::sort(filteredOrders.begin(), filteredOrders.end(),
        [](const std::shared_ptr<Order>& a, const std::shared_ptr<Order>& b) {
            return a->getTimestamp() > b->getTimestamp();
        });
    
    return filteredOrders;
}

// ============================================================================
// DISPLAY METHODS
// ============================================================================

void ActiveOrdersDisplay::addOrderRow(std::shared_ptr<Order> order, int row) {
    if (!order || !ordersTable_) {
        return;
    }
    
    try {
        // Order ID
        std::string orderIdStr;
        if (order->getOrderId() > 0) {
            orderIdStr = formatOrderId(order->getOrderId());
        } else {
            orderIdStr = "#" + std::to_string(row);
        }
        
        auto orderIdText = std::make_unique<Wt::WText>(orderIdStr);
        orderIdText->addStyleClass("fw-bold text-primary");
        ordersTable_->elementAt(row, 0)->addWidget(std::move(orderIdText));
        
        // Table/Location
        std::string tableDisplay = order->getTableIdentifier();
        if (tableDisplay.empty()) {
            tableDisplay = "Table " + std::to_string(order->getTableNumber());
        }
        
        auto tableText = std::make_unique<Wt::WText>(tableDisplay);
        tableText->addStyleClass("fw-medium");
        ordersTable_->elementAt(row, 1)->addWidget(std::move(tableText));
        
        // Status badge
        auto statusText = std::make_unique<Wt::WText>(formatOrderStatus(order->getStatus()));
        UIStyleHelper::styleBadge(statusText.get(), getStatusBadgeVariant(order->getStatus()));
        ordersTable_->elementAt(row, 2)->addWidget(std::move(statusText));
        
        // Items count
        size_t itemCount = order->getItems().size();
        auto itemsText = std::make_unique<Wt::WText>(std::to_string(itemCount) + " items");
        itemsText->addStyleClass("text-muted small");
        ordersTable_->elementAt(row, 3)->addWidget(std::move(itemsText));
        
        // Total
        double total = order->getTotal();
        if (total <= 0.0 && !order->getItems().empty()) {
            total = order->getSubtotal() + order->getTax();
        }
        
        auto totalText = std::make_unique<Wt::WText>(formatCurrency(total));
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
        
        applyRowStyling(row, (row % 2) == 0);
        
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error adding order row: " << e.what() << std::endl;
    }
}

void ActiveOrdersDisplay::applyRowStyling(int row, bool isEven) {
    if (!ordersTable_ || row == 0) { 
        return;
    }
    
    for (int col = 0; col < ordersTable_->columnCount(); ++col) {
        auto cell = ordersTable_->elementAt(row, col);
        
        cell->addStyleClass("pos-table-cell");
        
        if (isEven) {
            cell->addStyleClass("bg-white");
        } else {
            cell->addStyleClass("bg-light");
        }
        
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
    
    messageContainer->addStyleClass("text-center py-5");
    
    auto messageText = messageContainer->addNew<Wt::WText>("📝 No active orders");
    messageText->addStyleClass("h5 mb-2 text-muted");
    
    auto subText = messageContainer->addNew<Wt::WText>("Orders will appear here when created");
    subText->addStyleClass("text-muted small");
    
    auto cell = ordersTable_->elementAt(row, 0);
    cell->setColumnSpan(7);
    cell->addWidget(std::move(messageContainer));
}

void ActiveOrdersDisplay::hideEmptyOrdersMessage() {
    // Handled by refresh() and updateOrdersTable()
}

void ActiveOrdersDisplay::showLoadingState() {
    if (!ordersTable_) return;
    
    int row = ordersTable_->rowCount();
    auto loadingContainer = std::make_unique<Wt::WContainerWidget>();
    loadingContainer->addStyleClass("text-center py-4");
    
    auto loadingText = loadingContainer->addNew<Wt::WText>("🔄 Loading orders from API...");
    loadingText->addStyleClass("h5 mb-2 text-primary");
    
    auto cell = ordersTable_->elementAt(row, 0);
    cell->setColumnSpan(7);
    cell->addWidget(std::move(loadingContainer));
}

void ActiveOrdersDisplay::clearLoadingState() {
    if (!ordersTable_) return;
    
    while (ordersTable_->rowCount() > 1) {
        ordersTable_->removeRow(1);
    }
}

// ============================================================================
// PUBLIC INTERFACE METHODS
// ============================================================================

void ActiveOrdersDisplay::refresh() {
    std::cout << "[ActiveOrdersDisplay] Refreshing data..." << std::endl;
    loadOrdersFromAPI();
}

void ActiveOrdersDisplay::updateOrdersTable() {
    refresh();
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

void ActiveOrdersDisplay::applyCurrentTheme() {
    refresh();
}

void ActiveOrdersDisplay::updateOrderCount() {
    auto orders = getDisplayOrders();
    updateOrderCountDirect(static_cast<int>(orders.size()));
}

void ActiveOrdersDisplay::updateOrderCountDirect(int count) {
    if (!orderCountText_ || !orderCountText_->parent()) {
        return;
    }
    
    try {
        std::string countText = std::to_string(count) + " orders";
        orderCountText_->setText(countText);
        
        orderCountText_->removeStyleClass("bg-secondary bg-warning bg-danger bg-info");
        
        if (count == 0) {
            UIStyleHelper::styleBadge(orderCountText_, "secondary");
        } else if (count > 10) {
            UIStyleHelper::styleBadge(orderCountText_, "danger");
        } else if (count > 5) {
            UIStyleHelper::styleBadge(orderCountText_, "warning");
        } else {
            UIStyleHelper::styleBadge(orderCountText_, "info");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error updating order count: " << e.what() << std::endl;
    }
}

// ============================================================================
// COMPATIBILITY METHODS
// ============================================================================

void ActiveOrdersDisplay::createHeader() {
    if (showHeader_) {
        auto header = createDisplayHeader();
        // Header creation is handled in initializeUI()
    }
}

void ActiveOrdersDisplay::applyTableStyling() {
    if (ordersTable_) {
        ordersTable_->addStyleClass("table w-100 mb-0");
    }
}

// ============================================================================
// EVENT HANDLERS
// ============================================================================

void ActiveOrdersDisplay::handleOrderCreated(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Order created event received, refreshing from API..." << std::endl;
    loadOrdersFromAPI();
}

void ActiveOrdersDisplay::handleOrderModified(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Order modified event received, refreshing from API..." << std::endl;
    loadOrdersFromAPI();
}

void ActiveOrdersDisplay::handleOrderCompleted(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Order completed event received, refreshing from API..." << std::endl;
    loadOrdersFromAPI();
}

void ActiveOrdersDisplay::handleOrderCancelled(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Order cancelled event received, refreshing from API..." << std::endl;
    loadOrdersFromAPI();
}

void ActiveOrdersDisplay::handleKitchenStatusChanged(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Kitchen status changed, refreshing from API..." << std::endl;
    loadOrdersFromAPI();
}

// ============================================================================
// ACTION HANDLERS
// ============================================================================

void ActiveOrdersDisplay::onViewOrderClicked(int orderId) {
    std::cout << "[ActiveOrdersDisplay] View order #" << orderId << " (not implemented)" << std::endl;
}

void ActiveOrdersDisplay::onCompleteOrderClicked(int orderId) {
    auto enhancedService = std::dynamic_pointer_cast<EnhancedPOSService>(posService_);
    
    if (enhancedService && enhancedService->isConnected()) {
        std::cout << "[ActiveOrdersDisplay] Completing order #" << orderId << " via API..." << std::endl;
        
        enhancedService->updateOrderStatusAsync(orderId, Order::SERVED, [this, orderId](bool success) {
            if (success) {
                std::cout << "[ActiveOrdersDisplay] Order #" << orderId << " completed successfully" << std::endl;
                loadOrdersFromAPI();
            } else {
                std::cout << "[ActiveOrdersDisplay] Failed to complete order #" << orderId << std::endl;
            }
        });
    } else {
        std::cout << "[ActiveOrdersDisplay] Complete order #" << orderId << " (API not available)" << std::endl;
    }
}

void ActiveOrdersDisplay::onCancelOrderClicked(int orderId) {
    auto enhancedService = std::dynamic_pointer_cast<EnhancedPOSService>(posService_);
    
    if (enhancedService && enhancedService->isConnected()) {
        std::cout << "[ActiveOrdersDisplay] Cancelling order #" << orderId << " via API..." << std::endl;
        
        enhancedService->cancelOrderAsync(orderId, [this, orderId](bool success) {
            if (success) {
                std::cout << "[ActiveOrdersDisplay] Order #" << orderId << " cancelled successfully" << std::endl;
                loadOrdersFromAPI();
            } else {
                std::cout << "[ActiveOrdersDisplay] Failed to cancel order #" << orderId << std::endl;
            }
        });
    } else {
        if (posService_) {
            bool success = posService_->cancelOrder(orderId);
            if (success) {
                std::cout << "[ActiveOrdersDisplay] Order #" << orderId << " cancelled locally" << std::endl;
                refresh();
            } else {
                std::cout << "[ActiveOrdersDisplay] Failed to cancel order #" << orderId << std::endl;
            }
        }
    }
}

// ============================================================================
// HELPER METHODS
// ============================================================================

std::vector<std::shared_ptr<Order>> ActiveOrdersDisplay::getDisplayOrders() const {
    if (!posService_) return {};
    
    auto orders = posService_->getActiveOrders();
    
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
    
    std::sort(orders.begin(), orders.end(),
        [](const std::shared_ptr<Order>& a, const std::shared_ptr<Order>& b) {
            return a->getTimestamp() > b->getTimestamp();
        });
    
    return orders;
}

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
