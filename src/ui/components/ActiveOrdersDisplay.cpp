//============================================================================
// Enhanced ActiveOrdersDisplay.cpp - Clean Fix Without Manual Timers
//============================================================================

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
    
    addStyleClass("pos-active-orders-container h-100");
    setStyleClass("pos-active-orders-container h-100");
    
    initializeUI();
    setupEventListeners();
    
    // Load initial data from API
    loadOrdersFromAPI();
    
    std::cout << "✓ ActiveOrdersDisplay initialized with clean timer management" << std::endl;
}

ActiveOrdersDisplay::~ActiveOrdersDisplay() {
    std::cout << "[ActiveOrdersDisplay] Destructor called - cleaning up event subscriptions" << std::endl;
    
    try {
        // Unsubscribe from all events to prevent callbacks after destruction
        if (eventManager_) {
            for (auto handle : eventSubscriptions_) {
                eventManager_->unsubscribe(handle);
            }
            eventSubscriptions_.clear();
            std::cout << "[ActiveOrdersDisplay] Event subscriptions cleared" << std::endl;
        }
        
        // Clear service references to help prevent crashes
        posService_.reset();
        eventManager_.reset();
        
        std::cout << "[ActiveOrdersDisplay] Cleanup completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Error during destruction: " << e.what() << std::endl;
    }
}

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
    
    // Show loading state initially
    showLoadingState();
    
    std::cout << "✓ ActiveOrdersDisplay UI initialized" << std::endl;
}

void ActiveOrdersDisplay::loadOrdersFromAPI() {
    // CRITICAL: Add safety checks before any dynamic_cast operations
    if (!posService_) {
        std::cout << "[ActiveOrdersDisplay] ERROR: posService_ is null - cannot load orders" << std::endl;
        displayLocalOrders();
        return;
    }
    
    // CRITICAL: Check if the widget is still valid
    if (!parent()) {
        std::cout << "[ActiveOrdersDisplay] WARNING: Widget has no parent - likely being destroyed, skipping API load" << std::endl;
        return;
    }
    
    try {
        // Try to cast to EnhancedPOSService for API access - with safety
        auto enhancedService = std::dynamic_pointer_cast<EnhancedPOSService>(posService_);
        
        if (enhancedService && enhancedService->isConnected()) {
            std::cout << "[ActiveOrdersDisplay] Loading orders from API..." << std::endl;
            
            // Use async API call with safety checks (no manual timers!)
            enhancedService->getActiveOrdersAsync([this](std::vector<std::shared_ptr<Order>> orders, bool success) {
                // CRITICAL: Check if widget is still valid before doing anything
                if (!parent()) {
                    std::cout << "[ActiveOrdersDisplay] Async callback: Widget destroyed, ignoring result" << std::endl;
                    return;
                }
                
                if (!posService_) {
                    std::cout << "[ActiveOrdersDisplay] Async callback: Service destroyed, ignoring result" << std::endl;
                    return;
                }
                
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
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Exception during API load: " << e.what() << std::endl;
        displayLocalOrders();
    }
}

void ActiveOrdersDisplay::displayAPIOrders(const std::vector<std::shared_ptr<Order>>& orders) {
    // CRITICAL: Safety check before modifying UI
    if (!ordersTable_ || !parent()) {
        std::cout << "[ActiveOrdersDisplay] displayAPIOrders: Widget invalid, skipping" << std::endl;
        return;
    }
    
    try {
        clearLoadingState();
        
        // Clear existing rows (except header)
        while (ordersTable_->rowCount() > 1) {
            ordersTable_->removeRow(1);
        }
        
        if (orders.empty()) {
            showEmptyOrdersMessage();
            updateOrderCountDirect(0);
            return;
        }
        
        auto filteredOrders = filterOrders(orders);
        
        // Add order rows
        for (size_t i = 0; i < filteredOrders.size(); ++i) {
            if (maxOrdersToDisplay_ > 0 && i >= static_cast<size_t>(maxOrdersToDisplay_)) {
                break;
            }
            addOrderRow(filteredOrders[i], static_cast<int>(i + 1));
        }
        
        updateOrderCountDirect(static_cast<int>(filteredOrders.size()));
        std::cout << "[ActiveOrdersDisplay] Displayed " << filteredOrders.size() << " orders from API" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Exception in displayAPIOrders: " << e.what() << std::endl;
    }
}

void ActiveOrdersDisplay::displayLocalOrders() {
    // CRITICAL: Safety check before accessing posService_
    if (!posService_ || !ordersTable_ || !parent()) {
        std::cout << "[ActiveOrdersDisplay] displayLocalOrders: Service or widget invalid, showing empty" << std::endl;
        clearLoadingState();
        if (ordersTable_ && ordersTable_->rowCount() <= 1) {
            showEmptyOrdersMessage();
        }
        updateOrderCountDirect(0);
        return;
    }
    
    try {
        auto orders = getDisplayOrders();
        
        clearLoadingState();
        
        // Clear existing rows (except header)
        while (ordersTable_->rowCount() > 1) {
            ordersTable_->removeRow(1);
        }
        
        if (orders.empty()) {
            showEmptyOrdersMessage();
            updateOrderCountDirect(0);
            return;
        }
        
        // Add order rows
        for (size_t i = 0; i < orders.size(); ++i) {
            if (maxOrdersToDisplay_ > 0 && i >= static_cast<size_t>(maxOrdersToDisplay_)) {
                break;
            }
            addOrderRow(orders[i], static_cast<int>(i + 1));
        }
        
        updateOrderCountDirect(static_cast<int>(orders.size()));
        std::cout << "[ActiveOrdersDisplay] Displayed " << orders.size() << " orders from local data" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Exception in displayLocalOrders: " << e.what() << std::endl;
        clearLoadingState();
        updateOrderCountDirect(0);
    }
}

void ActiveOrdersDisplay::refresh() {
    std::cout << "[ActiveOrdersDisplay] Refreshing data..." << std::endl;
    
    // CRITICAL: Add safety checks before refreshing
    if (!parent()) {
        std::cout << "[ActiveOrdersDisplay] WARNING: Refresh called on destroyed widget - ignoring" << std::endl;
        return;
    }
    
    if (!posService_) {
        std::cout << "[ActiveOrdersDisplay] WARNING: No POS service available - showing empty" << std::endl;
        clearLoadingState();
        showEmptyOrdersMessage();
        updateOrderCountDirect(0);
        return;
    }
    
    loadOrdersFromAPI();
}

// Event handlers - SIMPLIFIED WITHOUT TIMERS
void ActiveOrdersDisplay::handleOrderCreated(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Order created event received" << std::endl;
    
    // Safety checks
    if (!parent() || !posService_) {
        std::cout << "[ActiveOrdersDisplay] Widget/service invalid - ignoring order created event" << std::endl;
        return;
    }
    
    // Directly refresh - no timer needed
    try {
        loadOrdersFromAPI();
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Exception during refresh: " << e.what() << std::endl;
    }
}

void ActiveOrdersDisplay::handleOrderModified(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Order modified event received" << std::endl;
    
    if (!parent() || !posService_) {
        std::cout << "[ActiveOrdersDisplay] Widget/service invalid - ignoring order modified event" << std::endl;
        return;
    }
    
    loadOrdersFromAPI();
}

void ActiveOrdersDisplay::handleOrderCompleted(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Order completed event received" << std::endl;
    
    if (!parent() || !posService_) {
        std::cout << "[ActiveOrdersDisplay] Widget/service invalid - ignoring order completed event" << std::endl;
        return;
    }
    
    loadOrdersFromAPI();
}

void ActiveOrdersDisplay::handleOrderCancelled(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Order cancelled event received" << std::endl;
    
    if (!parent() || !posService_) {
        std::cout << "[ActiveOrdersDisplay] Widget/service invalid - ignoring order cancelled event" << std::endl;
        return;
    }
    
    loadOrdersFromAPI();
}

void ActiveOrdersDisplay::handleKitchenStatusChanged(const std::any& eventData) {
    std::cout << "[ActiveOrdersDisplay] Kitchen status changed" << std::endl;
    
    if (!parent() || !posService_) {
        std::cout << "[ActiveOrdersDisplay] Widget/service invalid - ignoring kitchen status event" << std::endl;
        return;
    }
    
    loadOrdersFromAPI();
}

// Action handlers - CLEAN API INTEGRATION
void ActiveOrdersDisplay::onCompleteOrderClicked(int orderId) {
    if (!posService_ || !parent()) {
        std::cout << "[ActiveOrdersDisplay] No service available or widget invalid for completing order #" << orderId << std::endl;
        return;
    }
    
    try {
        auto enhancedService = std::dynamic_pointer_cast<EnhancedPOSService>(posService_);
        
        if (enhancedService && enhancedService->isConnected()) {
            std::cout << "[ActiveOrdersDisplay] Completing order #" << orderId << " via API..." << std::endl;
            
            enhancedService->updateOrderStatusAsync(orderId, Order::SERVED, [this, orderId](bool success) {
                // Check if widget is still valid
                if (!parent() || !posService_) {
                    std::cout << "[ActiveOrdersDisplay] Widget destroyed during async operation" << std::endl;
                    return;
                }
                
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
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Exception completing order #" << orderId << ": " << e.what() << std::endl;
    }
}

void ActiveOrdersDisplay::onCancelOrderClicked(int orderId) {
    if (!posService_ || !parent()) {
        std::cout << "[ActiveOrdersDisplay] No service available or widget invalid for cancelling order #" << orderId << std::endl;
        return;
    }
    
    try {
        auto enhancedService = std::dynamic_pointer_cast<EnhancedPOSService>(posService_);
        
        if (enhancedService && enhancedService->isConnected()) {
            std::cout << "[ActiveOrdersDisplay] Cancelling order #" << orderId << " via API..." << std::endl;
            
            enhancedService->cancelOrderAsync(orderId, [this, orderId](bool success) {
                // Check if widget is still valid
                if (!parent() || !posService_) {
                    std::cout << "[ActiveOrdersDisplay] Widget destroyed during async operation" << std::endl;
                    return;
                }
                
                if (success) {
                    std::cout << "[ActiveOrdersDisplay] Order #" << orderId << " cancelled successfully" << std::endl;
                    loadOrdersFromAPI();
                } else {
                    std::cout << "[ActiveOrdersDisplay] Failed to cancel order #" << orderId << std::endl;
                }
            });
        } else {
            // Fall back to local service
            bool success = posService_->cancelOrder(orderId);
            if (success) {
                std::cout << "[ActiveOrdersDisplay] Order #" << orderId << " cancelled locally" << std::endl;
                refresh();
            } else {
                std::cout << "[ActiveOrdersDisplay] Failed to cancel order #" << orderId << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Exception cancelling order #" << orderId << ": " << e.what() << std::endl;
    }
}

// All other helper methods remain the same...
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

void ActiveOrdersDisplay::showLoadingState() {
    if (!ordersTable_) return;
    
    int row = ordersTable_->rowCount();
    auto loadingContainer = std::make_unique<Wt::WContainerWidget>();
    loadingContainer->addStyleClass("text-center py-4 bg-white");
    
    auto loadingText = loadingContainer->addNew<Wt::WText>("🔄 Loading orders from API...");
    loadingText->addStyleClass("h5 mb-2 text-primary");
    
    auto cell = ordersTable_->elementAt(row, 0);
    cell->setColumnSpan(7);
    cell->addWidget(std::move(loadingContainer));
    cell->addStyleClass("border-0");
}

void ActiveOrdersDisplay::clearLoadingState() {
    if (!ordersTable_) return;
    
    while (ordersTable_->rowCount() > 1) {
        ordersTable_->removeRow(1);
    }
}

void ActiveOrdersDisplay::updateOrderCountDirect(int count) {
    if (!orderCountText_ || !orderCountText_->parent()) {
        return;
    }
    
    try {
        std::string countText = std::to_string(count) + " orders";
        orderCountText_->setText(countText);
        
        orderCountText_->removeStyleClass("bg-secondary");
        orderCountText_->removeStyleClass("bg-warning");
        orderCountText_->removeStyleClass("bg-danger");
        orderCountText_->removeStyleClass("bg-info");
        
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

void ActiveOrdersDisplay::addOrderRow(std::shared_ptr<Order> order, int row) {
    if (!order || !ordersTable_ || !parent()) {
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

// All remaining helper methods stay exactly the same...

void ActiveOrdersDisplay::initializeTableHeaders() {
    if (!ordersTable_) return;
    
    ordersTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Order #"));
    ordersTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Table/Location"));
    ordersTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Status"));
    ordersTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Items"));
    ordersTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Total"));
    ordersTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Time"));
    ordersTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Actions"));
    
    for (int col = 0; col < ordersTable_->columnCount(); ++col) {
        auto headerCell = ordersTable_->elementAt(0, col);
        headerCell->addStyleClass("pos-table-header bg-primary text-white text-center p-2 fw-bold border-0");
        
        auto headerText = dynamic_cast<Wt::WText*>(headerCell->widget(0));
        if (headerText) {
            headerText->addStyleClass("text-white fw-bold");
        }
    }
}

std::unique_ptr<Wt::WWidget> ActiveOrdersDisplay::createDisplayHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("pos-section-header bg-primary text-white p-3 mx-3 mt-3 mb-0 rounded-top d-flex justify-content-between align-items-center");
    
    headerText_ = header->addNew<Wt::WText>("📋 Active Orders");
    headerText_->addStyleClass("h4 mb-0 fw-bold text-white");
    
    orderCountText_ = header->addNew<Wt::WText>("Loading...");
    orderCountText_->addStyleClass("badge bg-info text-dark px-3 py-2 rounded-pill");
    
    return std::move(header);
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

// All remaining helper methods (formatting, styling, etc.)

void ActiveOrdersDisplay::applyRowStyling(int row, bool isEven) {
    if (!ordersTable_ || row == 0) return;
    
    for (int col = 0; col < ordersTable_->columnCount(); ++col) {
        auto cell = ordersTable_->elementAt(row, col);
        cell->addStyleClass("pos-table-cell p-2 align-middle border-0");
        
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
    if (!ordersTable_) return;
    
    int row = ordersTable_->rowCount();
    auto messageContainer = std::make_unique<Wt::WContainerWidget>();
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

std::vector<std::shared_ptr<Order>> ActiveOrdersDisplay::getDisplayOrders() const {
    if (!posService_) return {};
    
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[ActiveOrdersDisplay] Exception getting display orders: " << e.what() << std::endl;
        return {};
    }
}

void ActiveOrdersDisplay::onViewOrderClicked(int orderId) {
    std::cout << "[ActiveOrdersDisplay] View order #" << orderId << " (not implemented)" << std::endl;
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

void ActiveOrdersDisplay::hideEmptyOrdersMessage() {
    // Handled by refresh() and updateOrdersTable()
}

void ActiveOrdersDisplay::applyTableStyling() {
    if (ordersTable_) {
        ordersTable_->addStyleClass("table w-100 mb-0");
    }
}

void ActiveOrdersDisplay::applyCurrentTheme() {
    refresh();
}

void ActiveOrdersDisplay::updateOrderCount() {
    auto orders = getDisplayOrders();
    updateOrderCountDirect(static_cast<int>(orders.size()));
}
