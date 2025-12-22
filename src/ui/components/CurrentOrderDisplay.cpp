// ============================================================================
// Clean CurrentOrderDisplay.cpp - Minimal Borders, Streamlined Design
// Replace your existing CurrentOrderDisplay.cpp with this cleaner version
// ============================================================================

#include "../../../include/ui/components/CurrentOrderDisplay.hpp"
#include "../../../include/utils/UIStyleHelper.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WSpinBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WMessageBox.h>
#include <Wt/WBreak.h>
#include <iostream>
#include <iomanip>
#include <sstream>

CurrentOrderDisplay::CurrentOrderDisplay(std::shared_ptr<POSService> posService,
                                         std::shared_ptr<EventManager> eventManager)
    : Wt::WContainerWidget()
    , posService_(posService)
    , eventManager_(eventManager)
    , isDestroying_(false)
    , editable_(true)
    , headerContainer_(nullptr)
    , tableNumberText_(nullptr)
    , orderIdText_(nullptr)
    , itemsTable_(nullptr)
    , summaryContainer_(nullptr)
    , subtotalText_(nullptr)
    , taxText_(nullptr)
    , totalText_(nullptr)
    , itemCountText_(nullptr)
    , actionsContainer_(nullptr)
    , completeOrderButton_(nullptr)
    , cancelOrderButton_(nullptr)
{
    if (!posService_ || !eventManager_) {
        throw std::invalid_argument("CurrentOrderDisplay requires valid POSService and EventManager");
    }
    
    // CLEAN: Simple container class - no excessive styling
    addStyleClass("pos-current-order-container h-100");
    
    initializeUI();
    setupEventListeners();
    refresh();
    
    std::cout << "✓ CurrentOrderDisplay initialized with clean design" << std::endl;
}

CurrentOrderDisplay::~CurrentOrderDisplay() {
    std::cout << "[CurrentOrderDisplay] Destructor called - setting destruction flag" << std::endl;
    
    isDestroying_ = true;
    
    try {
        if (eventManager_) {
            for (auto handle : eventSubscriptions_) {
                eventManager_->unsubscribe(handle);
            }
            eventSubscriptions_.clear();
        }
        
        std::cout << "[CurrentOrderDisplay] Cleanup completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[CurrentOrderDisplay] Error during destruction: " << e.what() << std::endl;
    }
}

void CurrentOrderDisplay::initializeUI() {
    // Main container with flex column layout - full height
    addStyleClass("pos-current-order-container");
    setAttributeValue("style",
        "display: flex !important; flex-direction: column !important; "
        "height: 100% !important; overflow: hidden !important;");

    // HEADER - fixed at top
    auto header = createOrderHeader();
    addWidget(std::move(header));

    // SCROLLABLE TABLE CONTAINER - fixed behavior, scrolls when content overflows
    auto tableContainer = addNew<Wt::WContainerWidget>();
    tableContainer->addStyleClass("order-items-scroll-container");
    // No flex: 1 here - let CSS handle it

    itemsTable_ = tableContainer->addNew<Wt::WTable>();
    itemsTable_->addStyleClass("pos-current-order-table");
    // Remove Bootstrap table classes that may interfere

    // Initialize table headers
    initializeTableHeaders();

    // SUMMARY FOOTER - pinned at bottom, always visible
    summaryContainer_ = addNew<Wt::WContainerWidget>();
    summaryContainer_->addStyleClass("order-summary-footer");

    // Create summary content directly in the container
    createOrderSummaryContent();

    std::cout << "✓ CurrentOrderDisplay UI initialized with scrollable layout" << std::endl;
}

std::unique_ptr<Wt::WWidget> CurrentOrderDisplay::createOrderHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();

    // COMPACT: Single-line header with reduced padding
    header->addStyleClass("pos-section-header d-flex justify-content-between align-items-center");
    header->setAttributeValue("style",
        "padding: 8px 12px !important; flex-shrink: 0; "
        "background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); "
        "border-radius: 6px 6px 0 0;");

    // Title with compact styling
    auto titleText = header->addNew<Wt::WText>("📋 Current Order");
    titleText->addStyleClass("mb-0 fw-bold");
    titleText->setAttributeValue("style", "font-size: 1rem; color: white;");

    // Order info container - horizontal layout for compact display
    auto orderInfoContainer = header->addNew<Wt::WContainerWidget>();
    orderInfoContainer->addStyleClass("d-flex align-items-center gap-2");

    // Order ID text
    orderIdText_ = orderInfoContainer->addNew<Wt::WText>("No active order");
    orderIdText_->setAttributeValue("style", "color: rgba(255,255,255,0.8); font-size: 0.75rem;");

    // Table identifier badge
    tableNumberText_ = orderInfoContainer->addNew<Wt::WText>("No table");
    tableNumberText_->addStyleClass("badge bg-light text-dark px-2 py-1 rounded-pill");
    tableNumberText_->setAttributeValue("style", "font-size: 0.7rem;");

    return std::move(header);
}

void CurrentOrderDisplay::initializeTableHeaders() {
    if (!itemsTable_) {
        return;
    }

    // Create table headers - styling handled by CSS (.pos-current-order-table tr:first-child)
    itemsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    itemsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Price"));
    itemsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Qty"));
    itemsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));
    itemsTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));

    // Center align columns (except Item column)
    for (int col = 1; col < itemsTable_->columnCount(); ++col) {
        itemsTable_->elementAt(0, col)->addStyleClass("text-center");
    }

    std::cout << "✓ Table headers initialized" << std::endl;
}

void CurrentOrderDisplay::createOrderSummaryContent() {
    if (!summaryContainer_) return;

    // Main summary container with flex column layout
    summaryContainer_->setAttributeValue("style",
        "display: flex; flex-direction: column; gap: 8px; "
        "padding: 10px 12px !important; background: #f8f9fa; "
        "border-top: 2px solid #dee2e6;");

    // TOP ROW: Summary info - items, subtotal, tax, total
    auto summaryRow = summaryContainer_->addNew<Wt::WContainerWidget>();
    summaryRow->setAttributeValue("style",
        "display: flex; justify-content: space-between; align-items: center;");

    // Left side - items, subtotal, tax in a compact row
    auto leftInfo = summaryRow->addNew<Wt::WContainerWidget>();
    leftInfo->setAttributeValue("style",
        "display: flex; gap: 16px; font-size: 0.8rem;");

    // Item count
    auto itemCountContainer = leftInfo->addNew<Wt::WContainerWidget>();
    auto itemLabel = itemCountContainer->addNew<Wt::WText>("Items: ");
    itemLabel->addStyleClass("text-muted");
    itemCountText_ = itemCountContainer->addNew<Wt::WText>("0");
    itemCountText_->addStyleClass("fw-bold");

    // Subtotal
    auto subtotalContainer = leftInfo->addNew<Wt::WContainerWidget>();
    auto subtotalLabel = subtotalContainer->addNew<Wt::WText>("Subtotal: ");
    subtotalLabel->addStyleClass("text-muted");
    subtotalText_ = subtotalContainer->addNew<Wt::WText>("$0.00");
    subtotalText_->addStyleClass("fw-bold");

    // Tax
    auto taxContainer = leftInfo->addNew<Wt::WContainerWidget>();
    auto taxLabel = taxContainer->addNew<Wt::WText>("Tax: ");
    taxLabel->addStyleClass("text-muted");
    taxText_ = taxContainer->addNew<Wt::WText>("$0.00");
    taxText_->addStyleClass("fw-bold");

    // Right side - total with emphasis
    auto totalContainer = summaryRow->addNew<Wt::WContainerWidget>();
    totalContainer->setAttributeValue("style",
        "display: flex; align-items: center; gap: 8px;");

    auto totalLabel = totalContainer->addNew<Wt::WText>("TOTAL:");
    totalLabel->setAttributeValue("style",
        "font-size: 1rem; font-weight: bold; color: #198754;");

    totalText_ = totalContainer->addNew<Wt::WText>("$0.00");
    totalText_->setAttributeValue("style",
        "font-size: 1.2rem; font-weight: bold; color: #198754;");

    // BOTTOM ROW: Action buttons
    actionsContainer_ = summaryContainer_->addNew<Wt::WContainerWidget>();
    actionsContainer_->setAttributeValue("style",
        "display: flex; justify-content: flex-end; gap: 8px; padding-top: 8px; "
        "border-top: 1px solid #dee2e6;");

    // Cancel Order button
    cancelOrderButton_ = actionsContainer_->addNew<Wt::WPushButton>("Cancel Order");
    cancelOrderButton_->setAttributeValue("style",
        "padding: 6px 16px; font-size: 0.85rem; font-weight: 500; "
        "background: #dc3545; color: white; border: none; border-radius: 4px; "
        "cursor: pointer;");
    cancelOrderButton_->setToolTip("Cancel this order");
    cancelOrderButton_->clicked().connect([this]() {
        onCancelOrderClicked();
    });

    // Complete Order button (Send to Kitchen)
    completeOrderButton_ = actionsContainer_->addNew<Wt::WPushButton>("Complete Order →");
    completeOrderButton_->setAttributeValue("style",
        "padding: 6px 20px; font-size: 0.85rem; font-weight: 600; "
        "background: #198754; color: white; border: none; border-radius: 4px; "
        "cursor: pointer;");
    completeOrderButton_->setToolTip("Send order to kitchen for preparation");
    completeOrderButton_->clicked().connect([this]() {
        onCompleteOrderClicked();
    });

    // Initial button state
    updateActionButtons();
}

void CurrentOrderDisplay::updateOrderItemsTable() {
    if (!itemsTable_) return;
    
    // Clear existing rows (except header)
    while (itemsTable_->rowCount() > 1) {
        itemsTable_->removeRow(1);
    }
    
    auto currentOrder = getCurrentOrder();
    if (!currentOrder) {
        showEmptyOrderMessage();
        return;
    }
    
    const auto& items = currentOrder->getItems();
    if (items.empty()) {
        showEmptyOrderMessage();
        return;
    }
    
    // Add item rows
    for (size_t i = 0; i < items.size(); ++i) {
        addOrderItemRow(items[i], i);
    }
    
    hideEmptyOrderMessage();
    updateOrderSummary();
}

void CurrentOrderDisplay::addOrderItemRow(const OrderItem& item, size_t index) {
    if (!itemsTable_) return;
    
    int row = static_cast<int>(index + 1); // +1 for header row
    
    try {
        // Item name with special instructions - COMPACT
        auto nameContainer = std::make_unique<Wt::WContainerWidget>();
        auto nameText = nameContainer->addNew<Wt::WText>(item.getMenuItem().getName());
        nameText->addStyleClass("fw-bold text-primary");
        nameText->setAttributeValue("style", "font-size: 0.85rem;");

        // Show special instructions if any - inline for compactness
        if (!item.getSpecialInstructions().empty()) {
            auto instructionsText = nameContainer->addNew<Wt::WText>(" - " + item.getSpecialInstructions());
            instructionsText->setAttributeValue("style",
                "font-size: 0.7rem; color: #6c757d; font-style: italic;");
        }

        itemsTable_->elementAt(row, 0)->addWidget(std::move(nameContainer));

        // Unit price - COMPACT
        auto priceText = std::make_unique<Wt::WText>(formatCurrency(item.getMenuItem().getPrice()));
        priceText->addStyleClass("fw-medium text-success");
        priceText->setAttributeValue("style", "font-size: 0.8rem;");
        itemsTable_->elementAt(row, 1)->addWidget(std::move(priceText));
        
        // Quantity controls (editable spinner + buttons) - COMPACT
        auto qtyContainer = std::make_unique<Wt::WContainerWidget>();
        qtyContainer->setStyleClass("d-flex align-items-center justify-content-center gap-1");

        if (editable_) {
            // Decrease button - compact size
            auto decreaseBtn = qtyContainer->addNew<Wt::WPushButton>("-");
            UIStyleHelper::styleButton(decreaseBtn, "outline-secondary", "sm");
            decreaseBtn->setAttributeValue("style",
                "width: 24px !important; height: 24px !important; padding: 0 !important; "
                "font-size: 0.9rem; line-height: 1;");

            // Quantity display/spinner - compact
            auto qtySpinner = qtyContainer->addNew<Wt::WSpinBox>();
            qtySpinner->setRange(1, 99);
            qtySpinner->setValue(item.getQuantity());
            UIStyleHelper::styleFormControl(qtySpinner, "sm");
            qtySpinner->addStyleClass("text-center");
            qtySpinner->setAttributeValue("style",
                "width: 45px !important; height: 24px !important; padding: 2px 4px !important; "
                "font-size: 0.8rem;");
            
            // Increase button - compact size
            auto increaseBtn = qtyContainer->addNew<Wt::WPushButton>("+");
            UIStyleHelper::styleButton(increaseBtn, "outline-secondary", "sm");
            increaseBtn->setAttributeValue("style",
                "width: 24px !important; height: 24px !important; padding: 0 !important; "
                "font-size: 0.9rem; line-height: 1;");
            
            // Connect quantity change handlers
            decreaseBtn->clicked().connect([this, index, qtySpinner]() {
                int newQty = qtySpinner->value() - 1;
                if (newQty >= 1) {
                    qtySpinner->setValue(newQty);
                    onQuantityChanged(index, newQty);
                }
            });
            
            increaseBtn->clicked().connect([this, index, qtySpinner]() {
                int newQty = qtySpinner->value() + 1;
                if (newQty <= 99) {
                    qtySpinner->setValue(newQty);
                    onQuantityChanged(index, newQty);
                }
            });
            
            qtySpinner->valueChanged().connect([this, index, qtySpinner]() {
                onQuantityChanged(index, qtySpinner->value());
            });
            
        } else {
            // Read-only quantity
            auto qtyText = qtyContainer->addNew<Wt::WText>(std::to_string(item.getQuantity()));
            qtyText->setStyleClass("fw-bold");
        }
        
        itemsTable_->elementAt(row, 2)->addWidget(std::move(qtyContainer));
        
        // Total price for this item - COMPACT
        auto totalText = std::make_unique<Wt::WText>(formatCurrency(item.getTotalPrice()));
        totalText->setStyleClass("fw-bold text-success");
        totalText->setAttributeValue("style", "font-size: 0.8rem;");
        itemsTable_->elementAt(row, 3)->addWidget(std::move(totalText));
        
        // Actions (remove button) - COMPACT
        auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
        actionsContainer->setStyleClass("d-flex justify-content-center gap-1");

        if (editable_) {
            // Remove item button - compact with icon only
            auto removeBtn = actionsContainer->addNew<Wt::WPushButton>("🗑️");
            UIStyleHelper::styleButton(removeBtn, "outline-danger", "sm");
            removeBtn->setToolTip("Remove item");
            removeBtn->setAttributeValue("style",
                "padding: 2px 6px !important; font-size: 0.75rem;");
            removeBtn->clicked().connect([this, index]() {
                onRemoveItemClicked(index);
            });

            // Quick duplicate button - compact
            auto duplicateBtn = actionsContainer->addNew<Wt::WPushButton>("+1");
            UIStyleHelper::styleButton(duplicateBtn, "outline-info", "sm");
            duplicateBtn->setToolTip("Add another of this item");
            duplicateBtn->setAttributeValue("style",
                "padding: 2px 6px !important; font-size: 0.75rem;");
            duplicateBtn->clicked().connect([this, index]() {
                auto currentOrder = getCurrentOrder();
                if (currentOrder && index < currentOrder->getItems().size()) {
                    const auto& orderItem = currentOrder->getItems()[index];
                    posService_->addItemToCurrentOrder(orderItem.getMenuItem(), 1, orderItem.getSpecialInstructions());
                }
            });
        }
        
        itemsTable_->elementAt(row, 4)->addWidget(std::move(actionsContainer));
        
        // CLEAN: Simple row styling without excessive borders
        applyRowStyling(row, (row % 2) == 0);
        
    } catch (const std::exception& e) {
        std::cerr << "[CurrentOrderDisplay] Error adding order item row: " << e.what() << std::endl;
    }
}

void CurrentOrderDisplay::applyRowStyling(int row, bool isEven) {
    if (!itemsTable_ || row == 0) { // Don't style header row
        return;
    }

    for (int col = 0; col < itemsTable_->columnCount(); ++col) {
        auto cell = itemsTable_->elementAt(row, col);

        // Use CSS class for fixed row height styling (defined in base.css)
        cell->addStyleClass("pos-table-cell");

        // Alternating row colors
        if (isEven) {
            cell->addStyleClass("bg-white");
        } else {
            cell->addStyleClass("bg-light");
        }

        // Column-specific alignment
        if (col >= 1) { // Price, Qty, Total, Actions - center aligned
            cell->addStyleClass("text-center");
        }
    }
}

void CurrentOrderDisplay::updateOrderSummary() {
    auto currentOrder = getCurrentOrder();
    
    if (!currentOrder) {
        // Clear summary
        if (itemCountText_) itemCountText_->setText("0");
        if (subtotalText_) subtotalText_->setText("$0.00");
        if (taxText_) taxText_->setText("$0.00");
        if (totalText_) totalText_->setText("$0.00");
        
        // Update header to show no order
        if (orderIdText_) {
            orderIdText_->setText("No active order");
            orderIdText_->setStyleClass("text-white-50 small");
        }
        
        if (tableNumberText_) {
            tableNumberText_->setText("No table selected");
            tableNumberText_->setStyleClass("badge bg-secondary px-3 py-1 rounded-pill");
        }
        return;
    }
    
    // Update all summary fields
    const auto& items = currentOrder->getItems();
    if (itemCountText_) itemCountText_->setText(std::to_string(items.size()));
    if (subtotalText_) subtotalText_->setText(formatCurrency(currentOrder->getSubtotal()));
    if (taxText_) taxText_->setText(formatCurrency(currentOrder->getTax()));
    if (totalText_) totalText_->setText(formatCurrency(currentOrder->getTotal()));
    
    // Update header with order info
    if (orderIdText_) {
        orderIdText_->setText("Order #" + std::to_string(currentOrder->getOrderId()));
        orderIdText_->setStyleClass("text-white fw-bold small");
    }
    
    if (tableNumberText_) {
        std::string tableInfo = currentOrder->getTableIdentifier();
        if (currentOrder->isDineIn()) {
            tableInfo = "🪑 " + tableInfo;
            tableNumberText_->setStyleClass("badge bg-info px-3 py-1 rounded-pill");
        } else if (currentOrder->isDelivery()) {
            tableInfo = "🚗 " + tableInfo;
            tableNumberText_->setStyleClass("badge bg-warning px-3 py-1 rounded-pill");
        } else if (currentOrder->isWalkIn()) {
            tableInfo = "🚶 " + tableInfo;
            tableNumberText_->setStyleClass("badge bg-success px-3 py-1 rounded-pill");
        }
        tableNumberText_->setText(tableInfo);
    }
}

void CurrentOrderDisplay::showEmptyOrderMessage() {
    if (!itemsTable_) return;
    
    // Add empty message row
    auto emptyRow = itemsTable_->elementAt(1, 0);
    emptyRow->setColumnSpan(5);
    
    // CLEAN: Simple empty message without excessive containers
    auto emptyContainer = emptyRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    emptyContainer->addStyleClass("text-center py-5");
    
    auto emptyText = emptyContainer->addNew<Wt::WText>("🛒 No items in order yet");
    emptyText->addStyleClass("h5 mb-2 text-muted");
    
    auto subText = emptyContainer->addNew<Wt::WText>("Add items from the menu to get started");
    subText->addStyleClass("text-muted small");
}

// Keep all existing business logic methods unchanged...

void CurrentOrderDisplay::onQuantityChanged(size_t itemIndex, int newQuantity) {
    std::cout << "[CurrentOrderDisplay] Quantity changed for item " << itemIndex 
              << " to " << newQuantity << std::endl;
    
    if (!posService_) {
        std::cerr << "[CurrentOrderDisplay] No POS service available" << std::endl;
        return;
    }
    
    if (newQuantity <= 0) {
        onRemoveItemClicked(itemIndex);
        return;
    }
    
    try {
        bool success = posService_->updateCurrentOrderItemQuantity(itemIndex, newQuantity);
        if (success) {
            std::cout << "[CurrentOrderDisplay] ✓ Quantity updated successfully" << std::endl;
        } else {
            std::cerr << "[CurrentOrderDisplay] ✗ Failed to update quantity" << std::endl;
            refresh();
        }
    } catch (const std::exception& e) {
        std::cerr << "[CurrentOrderDisplay] Exception updating quantity: " << e.what() << std::endl;
        refresh();
    }
}

void CurrentOrderDisplay::onRemoveItemClicked(size_t itemIndex) {
    std::cout << "[CurrentOrderDisplay] Remove item clicked for index " << itemIndex << std::endl;
    
    if (!posService_) {
        std::cerr << "[CurrentOrderDisplay] No POS service available" << std::endl;
        return;
    }
    
    try {
        bool success = posService_->removeItemFromCurrentOrder(itemIndex);
        if (success) {
            std::cout << "[CurrentOrderDisplay] ✓ Item removed successfully" << std::endl;
        } else {
            std::cerr << "[CurrentOrderDisplay] ✗ Failed to remove item" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CurrentOrderDisplay] Exception removing item: " << e.what() << std::endl;
    }
}

void CurrentOrderDisplay::hideEmptyOrderMessage() {
    // The empty message is automatically hidden when rows are added
}

void CurrentOrderDisplay::onCompleteOrderClicked() {
    std::cout << "[CurrentOrderDisplay] Complete Order button clicked" << std::endl;

    if (!posService_) {
        std::cerr << "[CurrentOrderDisplay] No POS service available" << std::endl;
        return;
    }

    auto currentOrder = getCurrentOrder();
    if (!currentOrder) {
        std::cout << "[CurrentOrderDisplay] No current order to complete" << std::endl;
        return;
    }

    if (currentOrder->getItems().empty()) {
        std::cout << "[CurrentOrderDisplay] Cannot complete empty order" << std::endl;
        return;
    }

    try {
        // Send to kitchen
        bool success = posService_->sendCurrentOrderToKitchen();
        if (success) {
            std::cout << "[CurrentOrderDisplay] ✓ Order #" << currentOrder->getOrderId()
                      << " sent to kitchen successfully" << std::endl;

            // Publish event for UI updates
            if (eventManager_) {
                eventManager_->publish(POSEvents::ORDER_SENT_TO_KITCHEN,
                    std::any(currentOrder->getOrderId()), "CurrentOrderDisplay");
            }

            // Clear current order after sending to kitchen
            posService_->clearCurrentOrder();
        } else {
            std::cerr << "[CurrentOrderDisplay] ✗ Failed to send order to kitchen" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CurrentOrderDisplay] Exception sending to kitchen: " << e.what() << std::endl;
    }
}

void CurrentOrderDisplay::onCancelOrderClicked() {
    std::cout << "[CurrentOrderDisplay] Cancel Order button clicked" << std::endl;

    if (!posService_) {
        std::cerr << "[CurrentOrderDisplay] No POS service available" << std::endl;
        return;
    }

    auto currentOrder = getCurrentOrder();
    if (!currentOrder) {
        std::cout << "[CurrentOrderDisplay] No current order to cancel" << std::endl;
        return;
    }

    try {
        int orderId = currentOrder->getOrderId();

        // Cancel the order
        bool success = posService_->cancelCurrentOrder();
        if (success) {
            std::cout << "[CurrentOrderDisplay] ✓ Order #" << orderId << " cancelled" << std::endl;

            // Publish event
            if (eventManager_) {
                eventManager_->publish(POSEvents::ORDER_CANCELLED,
                    std::any(orderId), "CurrentOrderDisplay");
            }
        } else {
            std::cerr << "[CurrentOrderDisplay] ✗ Failed to cancel order" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CurrentOrderDisplay] Exception cancelling order: " << e.what() << std::endl;
    }
}

void CurrentOrderDisplay::updateActionButtons() {
    if (!completeOrderButton_ || !cancelOrderButton_) return;

    auto currentOrder = getCurrentOrder();
    bool hasOrder = currentOrder != nullptr;
    bool hasItems = hasOrder && !currentOrder->getItems().empty();

    // Complete Order button - only enabled if order has items
    completeOrderButton_->setEnabled(hasItems);
    if (hasItems) {
        completeOrderButton_->setAttributeValue("style",
            "padding: 6px 20px; font-size: 0.85rem; font-weight: 600; "
            "background: #198754; color: white; border: none; border-radius: 4px; "
            "cursor: pointer;");
    } else {
        completeOrderButton_->setAttributeValue("style",
            "padding: 6px 20px; font-size: 0.85rem; font-weight: 600; "
            "background: #6c757d; color: white; border: none; border-radius: 4px; "
            "cursor: not-allowed; opacity: 0.6;");
    }

    // Cancel Order button - enabled if order exists
    cancelOrderButton_->setEnabled(hasOrder);
    if (hasOrder) {
        cancelOrderButton_->setAttributeValue("style",
            "padding: 6px 16px; font-size: 0.85rem; font-weight: 500; "
            "background: #dc3545; color: white; border: none; border-radius: 4px; "
            "cursor: pointer;");
    } else {
        cancelOrderButton_->setAttributeValue("style",
            "padding: 6px 16px; font-size: 0.85rem; font-weight: 500; "
            "background: #6c757d; color: white; border: none; border-radius: 4px; "
            "cursor: not-allowed; opacity: 0.6;");
    }
}

std::shared_ptr<Order> CurrentOrderDisplay::getCurrentOrder() const {
    return posService_ ? posService_->getCurrentOrder() : nullptr;
}

bool CurrentOrderDisplay::hasCurrentOrder() const {
    return getCurrentOrder() != nullptr;
}

std::string CurrentOrderDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "$" << amount;
    return oss.str();
}

// EVENT HANDLERS
void CurrentOrderDisplay::setupEventListeners() {
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
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { handleCurrentOrderChanged(data); })
    );
}

void CurrentOrderDisplay::handleOrderCreated(const std::any& eventData) {
    std::cout << "[CurrentOrderDisplay] Order created event received" << std::endl;
    refresh();
}

void CurrentOrderDisplay::handleOrderModified(const std::any& eventData) {
    std::cout << "[CurrentOrderDisplay] Order modified event received" << std::endl;
    refresh();
}

void CurrentOrderDisplay::handleCurrentOrderChanged(const std::any& eventData) {
    std::cout << "[CurrentOrderDisplay] Current order changed event received" << std::endl;
    refresh();
}

// PUBLIC INTERFACE
void CurrentOrderDisplay::refresh() {
    updateOrderItemsTable();
    updateActionButtons();
    std::cout << "[CurrentOrderDisplay] Refreshed" << std::endl;
}

void CurrentOrderDisplay::clearOrder() {
    updateOrderItemsTable();
    updateOrderSummary();
    updateActionButtons();
    std::cout << "[CurrentOrderDisplay] Order cleared" << std::endl;
}

void CurrentOrderDisplay::setEditable(bool editable) {
    editable_ = editable;
    updateOrderItemsTable();
}

bool CurrentOrderDisplay::isEditable() const {
    return editable_;
}

void CurrentOrderDisplay::validateOrderDisplay() {
    auto currentOrder = getCurrentOrder();
    if (!currentOrder) {
        clearOrder();
    } else {
        updateOrderItemsTable();
        updateOrderSummary();
    }
}

void CurrentOrderDisplay::applyTableStyling() {
    if (itemsTable_) {
        itemsTable_->setWidth(Wt::WLength("100%"));
        itemsTable_->addStyleClass("table-responsive");
    }
}

void CurrentOrderDisplay::applySummaryStyling() {
    // Summary styling is handled in createOrderSummaryContent
}

void CurrentOrderDisplay::updateRowStyling(int row, bool isEven) {
    // Row styling is handled in addOrderItemRow
}

// Additional helper methods for interface compliance
std::string CurrentOrderDisplay::formatItemName(const OrderItem& item) const {
    return item.getMenuItem().getName();
}

std::string CurrentOrderDisplay::formatItemPrice(const OrderItem& item) const {
    return formatCurrency(item.getTotalPrice());
}

void CurrentOrderDisplay::onSpecialInstructionsChanged(size_t itemIndex, const std::string& instructions) {
    std::cout << "[CurrentOrderDisplay] Special instructions changed for item " << itemIndex 
              << ": " << instructions << std::endl;
}
