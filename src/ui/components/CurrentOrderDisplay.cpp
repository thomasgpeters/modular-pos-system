#include "../../../include/ui/components/CurrentOrderDisplay.hpp"

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
    , isDestroying_(false)  // Add destruction flag
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
{
    if (!posService_ || !eventManager_) {
        throw std::invalid_argument("CurrentOrderDisplay requires valid POSService and EventManager");
    }
    
    setStyleClass("current-order-display p-3 bg-white border rounded shadow-sm");
    
    initializeUI();
    setupEventListeners();
    refresh();
    
    std::cout << "[CurrentOrderDisplay] Initialized" << std::endl;
}

CurrentOrderDisplay::~CurrentOrderDisplay() {
    std::cout << "[CurrentOrderDisplay] Destructor called - setting destruction flag" << std::endl;
    
    // Set destruction flag to prevent further widget access
    isDestroying_ = true;
    
    try {
        // Unsubscribe from all events
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
    // Use simple layout instead of complex initialization
    initializeUI_Simple();
}

void CurrentOrderDisplay::initializeUI_Simple() {
    // Main container with vertical layout
    auto layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(15);
    
    // Header section
    auto headerWidget = createOrderHeader();
    headerContainer_ = static_cast<Wt::WContainerWidget*>(layout->addWidget(std::move(headerWidget)));
    
    // Items table
    itemsTable_ = layout->addWidget(std::make_unique<Wt::WTable>());
    itemsTable_->setStyleClass("table table-striped table-hover w-100");
    
    // Initialize table headers
    itemsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    itemsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Price"));
    itemsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Qty"));
    itemsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));
    itemsTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));
    
    // Style headers
    for (int col = 0; col < 5; ++col) {
        auto headerCell = itemsTable_->elementAt(0, col);
        headerCell->setStyleClass("bg-success text-white fw-bold text-center p-2");
    }
    
    // Summary section
    auto summaryWidget = createOrderSummary();
    summaryContainer_ = static_cast<Wt::WContainerWidget*>(layout->addWidget(std::move(summaryWidget)));
    
    applyTableStyling();
}

std::unique_ptr<Wt::WWidget> CurrentOrderDisplay::createOrderHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->setStyleClass("d-flex justify-content-between align-items-center p-3 bg-light border rounded");
    
    // Left side: Order info
    auto leftSide = header->addNew<Wt::WContainerWidget>();
    leftSide->setStyleClass("d-flex flex-column");
    
    auto titleText = leftSide->addNew<Wt::WText>("📋 Current Order");
    titleText->setStyleClass("h5 text-primary mb-1");
    
    orderIdText_ = leftSide->addNew<Wt::WText>("No active order");
    orderIdText_->setStyleClass("text-muted small");
    
    // Right side: Table info
    auto rightSide = header->addNew<Wt::WContainerWidget>();
    rightSide->setStyleClass("text-end");
    
    tableNumberText_ = rightSide->addNew<Wt::WText>("No table selected");
    tableNumberText_->setStyleClass("badge bg-secondary");
    
    return std::move(header);
}

std::unique_ptr<Wt::WWidget> CurrentOrderDisplay::createOrderSummary() {
    auto summary = std::make_unique<Wt::WContainerWidget>();
    summary->setStyleClass("p-3 bg-light border rounded");
    
    auto layout = summary->setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    
    // Title
    auto titleText = layout->addWidget(std::make_unique<Wt::WText>("💰 Order Summary"));
    titleText->setStyleClass("h6 text-success mb-2");
    
    // Summary lines
    auto summaryGrid = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    // Item count
    auto itemCountRow = summaryGrid->addNew<Wt::WContainerWidget>();
    itemCountRow->setStyleClass("d-flex justify-content-between");
    itemCountRow->addNew<Wt::WText>("Items:")->setStyleClass("fw-bold");
    itemCountText_ = itemCountRow->addNew<Wt::WText>("0");
    
    // Subtotal
    auto subtotalRow = summaryGrid->addNew<Wt::WContainerWidget>();
    subtotalRow->setStyleClass("d-flex justify-content-between");
    subtotalRow->addNew<Wt::WText>("Subtotal:")->setStyleClass("fw-bold");
    subtotalText_ = subtotalRow->addNew<Wt::WText>("$0.00");
    
    // Tax
    auto taxRow = summaryGrid->addNew<Wt::WContainerWidget>();
    taxRow->setStyleClass("d-flex justify-content-between");
    taxRow->addNew<Wt::WText>("Tax:")->setStyleClass("fw-bold");
    taxText_ = taxRow->addNew<Wt::WText>("$0.00");
    
    // Total
    auto totalRow = summaryGrid->addNew<Wt::WContainerWidget>();
    totalRow->setStyleClass("d-flex justify-content-between border-top pt-2 mt-2");
    totalRow->addNew<Wt::WText>("TOTAL:")->setStyleClass("h6 fw-bold text-success");
    totalText_ = totalRow->addNew<Wt::WText>("$0.00");
    totalText_->setStyleClass("h6 fw-bold text-success");
    
    return std::move(summary);
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
        // Item name with special instructions
        auto nameContainer = std::make_unique<Wt::WContainerWidget>();
        auto nameText = nameContainer->addNew<Wt::WText>(item.getMenuItem().getName());
        nameText->setStyleClass("fw-bold text-dark");
        
        // Show special instructions if any
        if (!item.getSpecialInstructions().empty()) {
            nameContainer->addNew<Wt::WBreak>();
            auto instructionsText = nameContainer->addNew<Wt::WText>("📝 " + item.getSpecialInstructions());
            instructionsText->setStyleClass("small text-muted fst-italic");
        }
        
        itemsTable_->elementAt(row, 0)->addWidget(std::move(nameContainer));
        
        // Unit price
        auto priceText = std::make_unique<Wt::WText>(formatCurrency(item.getMenuItem().getPrice()));
        priceText->setStyleClass("text-success");
        itemsTable_->elementAt(row, 1)->addWidget(std::move(priceText));
        
        // Quantity controls (editable spinner + buttons)
        auto qtyContainer = std::make_unique<Wt::WContainerWidget>();
        qtyContainer->setStyleClass("d-flex align-items-center justify-content-center gap-1");
        
        if (editable_) {
            // Decrease button
            auto decreaseBtn = qtyContainer->addNew<Wt::WPushButton>("-");
            decreaseBtn->setStyleClass("btn btn-outline-secondary btn-sm");
            decreaseBtn->setWidth(30);
            
            // Quantity display/spinner
            auto qtySpinner = qtyContainer->addNew<Wt::WSpinBox>();
            qtySpinner->setRange(1, 99);
            qtySpinner->setValue(item.getQuantity());
            qtySpinner->setWidth(60);
            qtySpinner->setStyleClass("form-control form-control-sm text-center");
            
            // Increase button
            auto increaseBtn = qtyContainer->addNew<Wt::WPushButton>("+");
            increaseBtn->setStyleClass("btn btn-outline-secondary btn-sm");
            increaseBtn->setWidth(30);
            
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
        
        // Total price for this item
        auto totalText = std::make_unique<Wt::WText>(formatCurrency(item.getTotalPrice()));
        totalText->setStyleClass("fw-bold text-success");
        itemsTable_->elementAt(row, 3)->addWidget(std::move(totalText));
        
        // Actions (remove button)
        auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
        actionsContainer->setStyleClass("d-flex justify-content-center gap-1");
        
        if (editable_) {
            // Remove item button
            auto removeBtn = actionsContainer->addNew<Wt::WPushButton>("🗑️ Remove");
            removeBtn->setStyleClass("btn btn-outline-danger btn-sm");
            removeBtn->clicked().connect([this, index]() {
                onRemoveItemClicked(index);
            });
            
            // Quick duplicate button
            auto duplicateBtn = actionsContainer->addNew<Wt::WPushButton>("📋 +1");
            duplicateBtn->setStyleClass("btn btn-outline-info btn-sm");
            duplicateBtn->setToolTip("Add another of this item");
            duplicateBtn->clicked().connect([this, index]() {
                auto currentOrder = getCurrentOrder();
                if (currentOrder && index < currentOrder->getItems().size()) {
                    const auto& orderItem = currentOrder->getItems()[index];
                    // Add the same item again
                    posService_->addItemToCurrentOrder(orderItem.getMenuItem(), 1, orderItem.getSpecialInstructions());
                }
            });
        }
        
        itemsTable_->elementAt(row, 4)->addWidget(std::move(actionsContainer));
        
        // Apply row styling
        bool isEven = (row % 2 == 0);
        for (int col = 0; col < 5; ++col) {
            auto cell = itemsTable_->elementAt(row, col);
            cell->setStyleClass("p-2 align-middle");
            if (col >= 1) { // Price, Qty, Total, Actions - center aligned
                cell->addStyleClass("text-center");
            }
            if (isEven) {
                cell->addStyleClass("table-light");
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[CurrentOrderDisplay] Error adding order item row: " << e.what() << std::endl;
    }
}

void CurrentOrderDisplay::onQuantityChanged(size_t itemIndex, int newQuantity) {
    std::cout << "[CurrentOrderDisplay] Quantity changed for item " << itemIndex 
              << " to " << newQuantity << std::endl;
    
    if (!posService_) {
        std::cerr << "[CurrentOrderDisplay] No POS service available" << std::endl;
        return;
    }
    
    if (newQuantity <= 0) {
        // Remove the item if quantity is 0 or less
        onRemoveItemClicked(itemIndex);
        return;
    }
    
    try {
        bool success = posService_->updateCurrentOrderItemQuantity(itemIndex, newQuantity);
        if (success) {
            std::cout << "[CurrentOrderDisplay] ✓ Quantity updated successfully" << std::endl;
            // The order modification event will trigger a refresh
        } else {
            std::cerr << "[CurrentOrderDisplay] ✗ Failed to update quantity" << std::endl;
            refresh(); // Refresh to revert changes
        }
    } catch (const std::exception& e) {
        std::cerr << "[CurrentOrderDisplay] Exception updating quantity: " << e.what() << std::endl;
        refresh(); // Refresh to revert changes
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
            // The order modification event will trigger a refresh
        } else {
            std::cerr << "[CurrentOrderDisplay] ✗ Failed to remove item" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CurrentOrderDisplay] Exception removing item: " << e.what() << std::endl;
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
        return;
    }
    
    // Update all summary fields
    const auto& items = currentOrder->getItems();
    if (itemCountText_) itemCountText_->setText(std::to_string(items.size()));
    if (subtotalText_) subtotalText_->setText(formatCurrency(currentOrder->getSubtotal()));
    if (taxText_) taxText_->setText(formatCurrency(currentOrder->getTax()));
    if (totalText_) totalText_->setText(formatCurrency(currentOrder->getTotal()));
    
    // Update header
    if (orderIdText_) {
        orderIdText_->setText("Order #" + std::to_string(currentOrder->getOrderId()));
        orderIdText_->setStyleClass("text-success small fw-bold");
    }
    
    if (tableNumberText_) {
        std::string tableInfo = currentOrder->getTableIdentifier();
        if (currentOrder->isDineIn()) {
            tableInfo = "🪑 " + tableInfo;
            tableNumberText_->setStyleClass("badge bg-primary");
        } else if (currentOrder->isDelivery()) {
            tableInfo = "🚗 " + tableInfo;
            tableNumberText_->setStyleClass("badge bg-warning text-dark");
        } else if (currentOrder->isWalkIn()) {
            tableInfo = "🚶 " + tableInfo;
            tableNumberText_->setStyleClass("badge bg-info");
        }
        tableNumberText_->setText(tableInfo);
    }
}

void CurrentOrderDisplay::showEmptyOrderMessage() {
    if (!itemsTable_) return;
    
    // Add empty message row
    auto emptyRow = itemsTable_->elementAt(1, 0);
    emptyRow->setColumnSpan(5);
    auto emptyText = emptyRow->addWidget(std::make_unique<Wt::WText>("🛒 No items in order yet"));
    emptyText->setStyleClass("text-center text-muted p-4 fst-italic");
    emptyRow->setStyleClass("bg-light");
}

void CurrentOrderDisplay::hideEmptyOrderMessage() {
    // The empty message is automatically hidden when rows are added
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
    std::cout << "[CurrentOrderDisplay] Refreshed" << std::endl;
}

void CurrentOrderDisplay::clearOrder() {
    updateOrderItemsTable();
    updateOrderSummary();
    std::cout << "[CurrentOrderDisplay] Order cleared" << std::endl;
}

void CurrentOrderDisplay::setEditable(bool editable) {
    editable_ = editable;
    updateOrderItemsTable(); // Refresh to show/hide edit controls
}

bool CurrentOrderDisplay::isEditable() const {
    return editable_;
}

void CurrentOrderDisplay::validateOrderDisplay() {
    // Ensure display is consistent with current order state
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
    // Summary styling is handled in createOrderSummary
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
    // Special instructions editing could be added in future
    std::cout << "[CurrentOrderDisplay] Special instructions changed for item " << itemIndex 
              << ": " << instructions << std::endl;
}
