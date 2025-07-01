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

// Fix 2: Complete corrected initializeUI method
void CurrentOrderDisplay::initializeUI() {
    addStyleClass("current-order-display");
    
    // Header section
    auto headerContainer = std::make_unique<Wt::WContainerWidget>();
    headerContainer->addStyleClass("order-header mb-3");
    
    // Order info
    auto orderInfo = std::make_unique<Wt::WText>("Current Order");
    orderInfo->addStyleClass("h5");
    headerContainer->addWidget(std::move(orderInfo));
    
    // Table number text
    auto tableText = std::make_unique<Wt::WText>("Table: --");
    tableNumberText_ = tableText.get();
    headerContainer->addWidget(std::move(tableText));
    
    // Order ID text  
    auto orderText = std::make_unique<Wt::WText>("Order: New");
    orderIdText_ = orderText.get();
    headerContainer->addWidget(std::move(orderText));
    
    headerContainer_ = headerContainer.get();
    addWidget(std::move(headerContainer));
    
    // Items table section
    createOrderItemsTable();
    
    // Summary section
    auto summaryContainer = std::make_unique<Wt::WContainerWidget>();
    summaryContainer->addStyleClass("order-summary mt-3 p-3 border");
    
    auto summaryTitle = std::make_unique<Wt::WText>("Order Total");
    summaryTitle->addStyleClass("h6");
    summaryContainer->addWidget(std::move(summaryTitle));
    
    // Subtotal
    auto subtotalText = std::make_unique<Wt::WText>("Subtotal: $0.00");
    subtotalText_ = subtotalText.get();
    summaryContainer->addWidget(std::move(subtotalText));
    
    // Tax
    auto taxText = std::make_unique<Wt::WText>("Tax: $0.00");  
    taxText_ = taxText.get();
    summaryContainer->addWidget(std::move(taxText));
    
    // Total
    auto totalText = std::make_unique<Wt::WText>("Total: $0.00");
    totalText->addStyleClass("fw-bold");
    totalText_ = totalText.get();
    summaryContainer->addWidget(std::move(totalText));
    
    // Item count
    auto itemCountText = std::make_unique<Wt::WText>("0 items");
    itemCountText_ = itemCountText.get();
    summaryContainer->addWidget(std::move(itemCountText));
    
    summaryContainer_ = summaryContainer.get();
    addWidget(std::move(summaryContainer));
}

// Fix 5: Alternative simple approach - add directly to container
void CurrentOrderDisplay::initializeUI_Simple() {
    addStyleClass("current-order-display");
    
    // Create header directly in container
    auto header = createOrderHeader();
    addWidget(std::move(header));
    
    // Create table directly in container
    auto table = std::make_unique<Wt::WTable>();
    table->addStyleClass("table table-striped order-items-table");
    itemsTable_ = table.get();
    addWidget(std::move(table));
    
    // Create summary directly in container
    auto summary = createOrderSummary();
    addWidget(std::move(summary));
    
    // Apply styling
    applyTableStyling();
    applySummaryStyling();
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

// Fix 3: Correct createOrderHeader implementation
std::unique_ptr<Wt::WWidget> CurrentOrderDisplay::createOrderHeader() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("order-header");
    
    auto title = std::make_unique<Wt::WText>("Current Order");
    title->addStyleClass("h4");
    container->addWidget(std::move(title));
    
    return container;
}


// CORRECT way - Option A: Create widget and store pointer
void CurrentOrderDisplay::createOrderItemsTable() {
    // Create table container
    auto tableContainer = std::make_unique<Wt::WContainerWidget>();
    tableContainer->addStyleClass("table-container");
    
    // Create table
    auto table = std::make_unique<Wt::WTable>();
    table->addStyleClass("table table-striped");
    
    // Add header row
    table->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    table->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Quantity"));
    table->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Price"));
    table->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));
    if (editable_) {
        table->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));
    }
    
    // Store pointer for later access
    itemsTable_ = table.get();
    
    // Add table to container
    tableContainer->addWidget(std::move(table));
    
    // Add container to this widget
    addWidget(std::move(tableContainer));
}

// Fix 4: Correct createOrderSummary implementation
std::unique_ptr<Wt::WWidget> CurrentOrderDisplay::createOrderSummary() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("order-summary mt-3 p-3 border rounded");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Summary title
    auto titleText = std::make_unique<Wt::WText>("Order Summary");
    titleText->addStyleClass("h6 mb-2");
    layout->addWidget(std::move(titleText));
    
    // Subtotal row
    auto subtotalContainer = std::make_unique<Wt::WContainerWidget>();
    auto subtotalLayout = std::make_unique<Wt::WHBoxLayout>();
    
    subtotalLayout->addWidget(std::make_unique<Wt::WText>("Subtotal:"));
    
    auto subtotalText = std::make_unique<Wt::WText>("$0.00");
    subtotalText->addStyleClass("text-end");
    subtotalText_ = subtotalText.get();
    subtotalLayout->addWidget(std::move(subtotalText));
    
    subtotalContainer->setLayout(std::move(subtotalLayout));
    layout->addWidget(std::move(subtotalContainer));
    
    // Tax row
    auto taxContainer = std::make_unique<Wt::WContainerWidget>();
    auto taxLayout = std::make_unique<Wt::WHBoxLayout>();
    
    taxLayout->addWidget(std::make_unique<Wt::WText>("Tax:"));
    
    auto taxText = std::make_unique<Wt::WText>("$0.00");
    taxText->addStyleClass("text-end");
    taxText_ = taxText.get();
    taxLayout->addWidget(std::move(taxText));
    
    taxContainer->setLayout(std::move(taxLayout));
    layout->addWidget(std::move(taxContainer));
    
    // Total row
    auto totalContainer = std::make_unique<Wt::WContainerWidget>();
    totalContainer->addStyleClass("border-top pt-2 mt-2");
    auto totalLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto totalLabel = std::make_unique<Wt::WText>("Total:");
    totalLabel->addStyleClass("fw-bold");
    totalLayout->addWidget(std::move(totalLabel));
    
    auto totalText = std::make_unique<Wt::WText>("$0.00");
    totalText->addStyleClass("text-end fw-bold h5");
    totalText_ = totalText.get();
    totalLayout->addWidget(std::move(totalText));
    
    totalContainer->setLayout(std::move(totalLayout));
    layout->addWidget(std::move(totalContainer));
    
    container->setLayout(std::move(layout));
    return container;
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
        // Update header information - FIXED: use getOrderId() instead of getId()
        tableNumberText_->setText(std::to_string(currentOrder->getTableNumber()));
        orderIdText_->setText(std::to_string(currentOrder->getOrderId()));
        
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
        itemsTable_->removeRow(1);
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

// FIXED: Changed Order::OrderItem to OrderItem and used getter methods
void CurrentOrderDisplay::addOrderItemRow(const OrderItem& item, size_t index) {
    int row = itemsTable_->rowCount();
    
    // Item name and description
    auto itemContainer = std::make_unique<Wt::WContainerWidget>();
    auto itemLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // FIXED: Use getMenuItem().getName() instead of menuItem->getName()
    auto nameText = std::make_unique<Wt::WText>(item.getMenuItem().getName());
    nameText->addStyleClass("font-weight-bold");
    itemLayout->addWidget(std::move(nameText));
    
    // FIXED: Use getSpecialInstructions() instead of specialInstructions
    if (!item.getSpecialInstructions().empty()) {
        auto instructionsText = std::make_unique<Wt::WText>("Note: " + item.getSpecialInstructions());
        instructionsText->addStyleClass("text-muted small font-italic");
        itemLayout->addWidget(std::move(instructionsText));
    }
    
    itemContainer->setLayout(std::move(itemLayout));
    itemsTable_->elementAt(row, 0)->addWidget(std::move(itemContainer));
    
    // Quantity
    if (editable_) {
        auto quantitySpinBox = std::make_unique<Wt::WSpinBox>();
        quantitySpinBox->setRange(1, 99);
        // FIXED: Use getQuantity() instead of quantity
        quantitySpinBox->setValue(item.getQuantity());
        quantitySpinBox->addStyleClass("form-control-sm");
        quantitySpinBox->valueChanged().connect([this, index](int newQuantity) {
            onQuantityChanged(index, newQuantity);
        });
        itemsTable_->elementAt(row, 1)->addWidget(std::move(quantitySpinBox));
    } else {
        // FIXED: Use getQuantity() instead of quantity
        auto quantityText = std::make_unique<Wt::WText>(std::to_string(item.getQuantity()));
        quantityText->addStyleClass("text-center");
        itemsTable_->elementAt(row, 1)->addWidget(std::move(quantityText));
    }
    
    // Unit price
    // FIXED: Use getMenuItem().getPrice() instead of menuItem->getPrice()
    auto priceText = std::make_unique<Wt::WText>(formatCurrency(item.getMenuItem().getPrice()));
    itemsTable_->elementAt(row, 2)->addWidget(std::move(priceText));
    
    // Line total
    // FIXED: Use getter methods instead of direct member access
    double lineTotal = item.getMenuItem().getPrice() * item.getQuantity();
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

// FIXED: Changed Order::OrderItem to OrderItem and used getter methods
std::string CurrentOrderDisplay::formatItemName(const OrderItem& item) const {
    return item.getMenuItem().getName();
}

// FIXED: Changed Order::OrderItem to OrderItem and used getter methods
std::string CurrentOrderDisplay::formatItemPrice(const OrderItem& item) const {
    return formatCurrency(item.getMenuItem().getPrice());
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
            // FIXED: Use getOrderId() instead of getId()
            orderIdText_->setText(std::to_string(currentOrder->getOrderId()));
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
