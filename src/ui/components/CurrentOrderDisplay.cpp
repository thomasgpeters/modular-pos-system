#include "../../../include/ui/components/CurrentOrderDisplay.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WTableCell.h>
#include <Wt/WLineEdit.h>
#include <Wt/WApplication.h>

#include <iostream>
#include <iomanip>
#include <sstream>

CurrentOrderDisplay::CurrentOrderDisplay(std::shared_ptr<POSService> posService,
                                        std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , editable_(true)
    , headerContainer_(nullptr)
    , tableNumberText_(nullptr)
    , orderIdText_(nullptr)
    , itemsTable_(nullptr)
    , summaryContainer_(nullptr)
    , subtotalText_(nullptr)
    , taxText_(nullptr)
    , totalText_(nullptr)
    , itemCountText_(nullptr) {
    
    initializeUI();
    setupEventListeners();
    refresh();
}

void CurrentOrderDisplay::initializeUI() {
    addStyleClass("current-order-display card");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Order header
    auto header = createOrderHeader();
    layout->addWidget(std::move(header));
    
    // Order items table
    createOrderItemsTable();
    layout->addWidget(itemsTable_);
    
    // Order summary
    auto summary = createOrderSummary();
    layout->addWidget(std::move(summary));
    
    setLayout(std::move(layout));
    
    std::cout << "✓ CurrentOrderDisplay UI initialized" << std::endl;
}

void CurrentOrderDisplay::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "Warning: EventManager not available for CurrentOrderDisplay" << std::endl;
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
        eventManager_->subscribe(POSEvents::ORDER_ITEM_ADDED,
            [this](const std::any& data) { handleOrderModified(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_ITEM_REMOVED,
            [this](const std::any& data) { handleOrderModified(data); }));
    
    std::cout << "✓ CurrentOrderDisplay event listeners setup complete" << std::endl;
}

std::unique_ptr<Wt::WWidget> CurrentOrderDisplay::createOrderHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("card-header order-header");
    headerContainer_ = header.get();
    
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Order title
    auto titleContainer = std::make_unique<Wt::WContainerWidget>();
    auto titleLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto title = std::make_unique<Wt::WText>("Current Order");
    title->addStyleClass("h5 mb-1");
    titleLayout->addWidget(std::move(title));
    
    // Order details container
    auto detailsContainer = std::make_unique<Wt::WContainerWidget>();
    auto detailsLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Table number
    auto tableLabel = std::make_unique<Wt::WText>("Table: ");
    tableLabel->addStyleClass("text-muted small");
    detailsLayout->addWidget(std::move(tableLabel));
    
    tableNumberText_ = detailsContainer->addWidget(std::make_unique<Wt::WText>("--"));
    tableNumberText_->addStyleClass("font-weight-bold small");
    detailsLayout->addWidget(tableNumberText_);
    
    // Separator
    auto separator = std::make_unique<Wt::WText>(" | ");
    separator->addStyleClass("text-muted small");
    detailsLayout->addWidget(std::move(separator));
    
    // Order ID
    auto orderLabel = std::make_unique<Wt::WText>("Order #");
    orderLabel->addStyleClass("text-muted small");
    detailsLayout->addWidget(std::move(orderLabel));
    
    orderIdText_ = detailsContainer->addWidget(std::make_unique<Wt::WText>("--"));
    orderIdText_->addStyleClass("font-weight-bold small");
    detailsLayout->addWidget(orderIdText_);
    
    detailsContainer->setLayout(std::move(detailsLayout));
    titleLayout->addWidget(std::move(detailsContainer));
    
    titleContainer->setLayout(std::move(titleLayout));
    headerLayout->addWidget(std::move(titleContainer), 1);
    
    // Item count (right side)
    itemCountText_ = header->addWidget(std::make_unique<Wt::WText>("0 items"));
    itemCountText_->addStyleClass("badge badge-secondary");
    headerLayout->addWidget(itemCountText_);
    
    header->setLayout(std::move(headerLayout));
    return std::move(header);
}

void CurrentOrderDisplay::createOrderItemsTable() {
    itemsTable_ = addWidget(std::make_unique<Wt::WTable>());
    itemsTable_->addStyleClass("table table-sm order-items-table");
    itemsTable_->setHeaderCount(1);
    
    // Set up table headers
    itemsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    itemsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Qty"));
    itemsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Price"));
    itemsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));
    
    if (editable_) {
        itemsTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));
    }
    
    applyTableStyling();
    
    std::cout << "✓ Order items table created" << std::endl;
}

std::unique_ptr<Wt::WWidget> CurrentOrderDisplay::createOrderSummary() {
    auto summary = std::make_unique<Wt::WContainerWidget>();
    summary->addStyleClass("card-footer order-summary");
    summaryContainer_ = summary.get();
    
    auto summaryLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // Subtotal row
    auto subtotalRow = std::make_unique<Wt::WContainerWidget>();
    auto subtotalLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto subtotalLabel = std::make_unique<Wt::WText>("Subtotal:");
    subtotalLabel->addStyleClass("text-muted");
    subtotalLayout->addWidget(std::move(subtotalLabel), 1);
    
    subtotalText_ = subtotalRow->addWidget(std::make_unique<Wt::WText>("$0.00"));
    subtotalText_->addStyleClass("font-weight-bold");
    subtotalLayout->addWidget(subtotalText_);
    
    subtotalRow->setLayout(std::move(subtotalLayout));
    summaryLayout->addWidget(std::move(subtotalRow));
    
    // Tax row
    auto taxRow = std::make_unique<Wt::WContainerWidget>();
    auto taxLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto taxLabel = std::make_unique<Wt::WText>("Tax:");
    taxLabel->addStyleClass("text-muted");
    taxLayout->addWidget(std::move(taxLabel), 1);
    
    taxText_ = taxRow->addWidget(std::make_unique<Wt::WText>("$0.00"));
    taxText_->addStyleClass("font-weight-bold");
    taxLayout->addWidget(taxText_);
    
    taxRow->setLayout(std::move(taxLayout));
    summaryLayout->addWidget(std::move(taxRow));
    
    // Total row
    auto totalRow = std::make_unique<Wt::WContainerWidget>();
    totalRow->addStyleClass("border-top pt-2 mt-2");
    auto totalLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto totalLabel = std::make_unique<Wt::WText>("Total:");
    totalLabel->addStyleClass("h6 mb-0");
    totalLayout->addWidget(std::move(totalLabel), 1);
    
    totalText_ = totalRow->addWidget(std::make_unique<Wt::WText>("$0.00"));
    totalText_->addStyleClass("h5 text-primary font-weight-bold mb-0");
    totalLayout->addWidget(totalText_);
    
    totalRow->setLayout(std::move(totalLayout));
    summaryLayout->addWidget(std::move(totalRow));
    
    summary->setLayout(std::move(summaryLayout));
    return std::move(summary);
}

void CurrentOrderDisplay::refresh() {
    if (!posService_) {
        std::cerr << "Error: POSService not available for order refresh" << std::endl;
        return;
    }
    
    updateOrderItemsTable();
    updateOrderSummary();
    
    auto currentOrder = getCurrentOrder();
    if (currentOrder) {
        // Update header information
        tableNumberText_->setText(std::to_string(currentOrder->getTableNumber()));
        orderIdText_->setText(std::to_string(currentOrder->getId()));
        
        hideEmptyOrderMessage();
    } else {
        // Clear header information
        tableNumberText_->setText("--");
        orderIdText_->setText("--");
        
        showEmptyOrderMessage();
    }
    
    validateOrderDisplay();
}

void CurrentOrderDisplay::updateOrderItemsTable() {
    if (!itemsTable_) {
        return;
    }
    
    // Clear existing rows (except header)
    while (itemsTable_->rowCount() > 1) {
        itemsTable_->deleteRow(1);
    }
    
    auto currentOrder = getCurrentOrder();
    if (!currentOrder) {
        return;
    }
    
    const auto& items = currentOrder->getItems();
    
    for (size_t i = 0; i < items.size(); ++i) {
        addOrderItemRow(items[i], i);
    }
    
    // Update item count
    itemCountText_->setText(std::to_string(items.size()) + " items");
    
    std::cout << "Order items table updated with " << items.size() << " items" << std::endl;
}

void CurrentOrderDisplay::addOrderItemRow(const Order::OrderItem& item, size_t index) {
    int row = itemsTable_->rowCount();
    
    // Item name and description
    auto itemContainer = std::make_unique<Wt::WContainerWidget>();
    auto itemLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto nameText = std::make_unique<Wt::WText>(item.menuItem->getName());
    nameText->addStyleClass("font-weight-bold");
    itemLayout->addWidget(std::move(nameText));
    
    if (!item.specialInstructions.empty()) {
        auto instructionsText = std::make_unique<Wt::WText>("Note: " + item.specialInstructions);
        instructionsText->addStyleClass("text-muted small font-italic");
        itemLayout->addWidget(std::move(instructionsText));
    }
    
    itemContainer->setLayout(std::move(itemLayout));
    itemsTable_->elementAt(row, 0)->addWidget(std::move(itemContainer));
    
    // Quantity
    if (editable_) {
        auto quantitySpinBox = std::make_unique<Wt::WSpinBox>();
        quantitySpinBox->setRange(1, 99);
        quantitySpinBox->setValue(item.quantity);
        quantitySpinBox->addStyleClass("form-control-sm");
        quantitySpinBox->valueChanged().connect([this, index](int newQuantity) {
            onQuantityChanged(index, newQuantity);
        });
        itemsTable_->elementAt(row, 1)->addWidget(std::move(quantitySpinBox));
    } else {
        auto quantityText = std::make_unique<Wt::WText>(std::to_string(item.quantity));
        quantityText->addStyleClass("text-center");
        itemsTable_->elementAt(row, 1)->addWidget(std::move(quantityText));
    }
    
    // Unit price
    auto priceText = std::make_unique<Wt::WText>(formatCurrency(item.menuItem->getPrice()));
    itemsTable_->elementAt(row, 2)->addWidget(std::move(priceText));
    
    // Line total
    double lineTotal = item.menuItem->getPrice() * item.quantity;
    auto totalText = std::make_unique<Wt::WText>(formatCurrency(lineTotal));
    totalText->addStyleClass("font-weight-bold");
    itemsTable_->elementAt(row, 3)->addWidget(std::move(totalText));
    
    // Actions (if editable)
    if (editable_) {
        auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
        auto actionsLayout = std::make_unique<Wt::WHBoxLayout>();
        
        auto removeButton = std::make_unique<Wt::WPushButton>("×");
        removeButton->addStyleClass("btn btn-sm btn-outline-danger");
        removeButton->setToolTip("Remove item");
        removeButton->clicked().connect([this, index]() {
            onRemoveItemClicked(index);
        });
        actionsLayout->addWidget(std::move(removeButton));
        
        actionsContainer->setLayout(std::move(actionsLayout));
        itemsTable_->elementAt(row, 4)->addWidget(std::move(actionsContainer));
    }
    
    // Apply row styling
    updateRowStyling(row, (index % 2) == 0);
}

void CurrentOrderDisplay::updateOrderSummary() {
    auto currentOrder = getCurrentOrder();
    
    if (!currentOrder) {
        subtotalText_->setText("$0.00");
        taxText_->setText("$0.00");
        totalText_->setText("$0.00");
        return;
    }
    
    double subtotal = currentOrder->getSubtotal();
    double tax = currentOrder->getTax();
    double total = currentOrder->getTotal();
    
    subtotalText_->setText(formatCurrency(subtotal));
    taxText_->setText(formatCurrency(tax));
    totalText_->setText(formatCurrency(total));
    
    std::cout << "Order summary updated - Total: " << formatCurrency(total) << std::endl;
}

// =================================================================
// Event Handlers
// =================================================================

void CurrentOrderDisplay::handleOrderCreated(const std::any& eventData) {
    std::cout << "Order created event received, refreshing current order display" << std::endl;
    refresh();
}

void CurrentOrderDisplay::handleOrderModified(const std::any& eventData) {
    std::cout << "Order modified event received, refreshing current order display" << std::endl;
    refresh();
}

void CurrentOrderDisplay::handleCurrentOrderChanged(const std::any& eventData) {
    std::cout << "Current order changed event received, refreshing display" << std::endl;
    refresh();
}

// =================================================================
// UI Action Handlers
// =================================================================

void CurrentOrderDisplay::onQuantityChanged(size_t itemIndex, int newQuantity) {
    if (!posService_) {
        std::cerr << "Error: POSService not available for quantity change" << std::endl;
        return;
    }
    
    bool success = posService_->updateCurrentOrderItemQuantity(itemIndex, newQuantity);
    
    if (!success) {
        std::cerr << "Failed to update item quantity" << std::endl;
        // Refresh to revert the UI change
        refresh();
    }
}

void CurrentOrderDisplay::onRemoveItemClicked(size_t itemIndex) {
    if (!posService_) {
        std::cerr << "Error: POSService not available for item removal" << std::endl;
        return;
    }
    
    bool success = posService_->removeItemFromCurrentOrder(itemIndex);
    
    if (!success) {
        std::cerr << "Failed to remove item from order" << std::endl;
    }
}

void CurrentOrderDisplay::onSpecialInstructionsChanged(size_t itemIndex, const std::string& instructions) {
    // This would update special instructions if the Order class supports it
    // For now, we'll just log it
    std::cout << "Special instructions updated for item " << itemIndex << ": " << instructions << std::endl;
}

// =================================================================
// Public Interface Methods
// =================================================================

std::shared_ptr<Order> CurrentOrderDisplay::getCurrentOrder() const {
    if (!posService_) {
        return nullptr;
    }
    
    return posService_->getCurrentOrder();
}

void CurrentOrderDisplay::setEditable(bool editable) {
    if (editable_ != editable) {
        editable_ = editable;
        refresh(); // Rebuild table with/without edit controls
        std::cout << "CurrentOrderDisplay editable mode set to: " << (editable ? "true" : "false") << std::endl;
    }
}

bool CurrentOrderDisplay::isEditable() const {
    return editable_;
}

void CurrentOrderDisplay::clearOrder() {
    // Clear current order through service
    if (posService_) {
        posService_->setCurrentOrder(nullptr);
    }
    
    refresh();
    std::cout << "Current order display cleared" << std::endl;
}

// =================================================================
// Helper Methods
// =================================================================

std::string CurrentOrderDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << "$" << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}

std::string CurrentOrderDisplay::formatItemName(const Order::OrderItem& item) const {
    return item.menuItem->getName();
}

std::string CurrentOrderDisplay::formatItemPrice(const Order::OrderItem& item) const {
    return formatCurrency(item.menuItem->getPrice());
}

void CurrentOrderDisplay::showEmptyOrderMessage() {
    if (!itemsTable_) {
        return;
    }
    
    // Add empty message row if not already present
    if (itemsTable_->rowCount() == 1) {
        int row = itemsTable_->rowCount();
        auto emptyMessage = std::make_unique<Wt::WText>("No items in order");
        emptyMessage->addStyleClass("text-muted text-center py-3");
        
        int colspan = editable_ ? 5 : 4;
        itemsTable_->elementAt(row, 0)->addWidget(std::move(emptyMessage));
        itemsTable_->elementAt(row, 0)->setColumnSpan(colspan);
    }
}

void CurrentOrderDisplay::hideEmptyOrderMessage() {
    // Empty message is automatically hidden when real items are added
}

bool CurrentOrderDisplay::hasCurrentOrder() const {
    auto order = getCurrentOrder();
    return order && !order->getItems().empty();
}

void CurrentOrderDisplay::validateOrderDisplay() {
    // Perform any validation or consistency checks
    auto currentOrder = getCurrentOrder();
    
    if (currentOrder) {
        // Ensure table number and order ID are displayed correctly
        if (tableNumberText_->text().toUTF8() == "--") {
            tableNumberText_->setText(std::to_string(currentOrder->getTableNumber()));
        }
        
        if (orderIdText_->text().toUTF8() == "--") {
            orderIdText_->setText(std::to_string(currentOrder->getId()));
        }
    }
}

// =================================================================
// Styling Methods
// =================================================================

void CurrentOrderDisplay::applyTableStyling() {
    if (!itemsTable_) {
        return;
    }
    
    // Style table headers
    for (int col = 0; col < itemsTable_->columnCount(); ++col) {
        auto headerCell = itemsTable_->elementAt(0, col);
        headerCell->addStyleClass("table-header bg-light font-weight-bold");
        
        // Set column widths
        switch (col) {
            case 0: // Item name
                headerCell->setWidth(Wt::WLength("40%"));
                break;
            case 1: // Quantity
                headerCell->setWidth(Wt::WLength("15%"));
                break;
            case 2: // Price
                headerCell->setWidth(Wt::WLength("20%"));
                break;
            case 3: // Total
                headerCell->setWidth(Wt::WLength("20%"));
                break;
            case 4: // Actions
                if (editable_) {
                    headerCell->setWidth(Wt::WLength("5%"));
                }
                break;
        }
    }
}

void CurrentOrderDisplay::applySummaryStyling() {
    if (summaryContainer_) {
        summaryContainer_->addStyleClass("bg-light");
    }
}

void CurrentOrderDisplay::updateRowStyling(int row, bool isEven) {
    if (!itemsTable_ || row == 0) { // Don't style header row
        return;
    }
    
    std::string rowClass = isEven ? "table-row-even" : "table-row-odd";
    
    for (int col = 0; col < itemsTable_->columnCount(); ++col) {
        auto cell = itemsTable_->elementAt(row, col);
        cell->addStyleClass("order-item-cell " + rowClass);
        
        // Center align quantity and price columns
        if (col == 1 || col == 2 || col == 3) {
            cell->addStyleClass("text-center");
        }
    }
}
