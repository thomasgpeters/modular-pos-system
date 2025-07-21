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
    // CLEAN: Single main container without excessive wrappers
    auto mainContainer = addNew<Wt::WContainerWidget>();
    mainContainer->addStyleClass("pos-current-order-main h-100");
    
    // Create header
    auto header = createOrderHeader();
    mainContainer->addWidget(std::move(header));
    
    // CLEAN: Direct table creation without wrapper containers
    itemsTable_ = mainContainer->addNew<Wt::WTable>();
    itemsTable_->addStyleClass("table pos-current-order-table w-100");
    itemsTable_->setWidth(Wt::WLength("100%"));
    
    // Initialize table headers
    initializeTableHeaders(); 
    
    // CLEAN: Create summary container directly
    summaryContainer_ = mainContainer->addNew<Wt::WContainerWidget>();
    summaryContainer_->addStyleClass("pos-order-summary");
    
    // Create summary content directly in the container
    createOrderSummaryContent();
    
    std::cout << "✓ CurrentOrderDisplay UI initialized with clean layout" << std::endl;
}

std::unique_ptr<Wt::WWidget> CurrentOrderDisplay::createOrderHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    
    // CLEAN: Simple header without excessive border classes
    header->addStyleClass("pos-section-header d-flex justify-content-between align-items-center");
    
    // Title with clean styling
    auto titleText = header->addNew<Wt::WText>("📋 Current Order");
    titleText->addStyleClass("h4 mb-0 fw-bold");
    
    // Order info container
    auto orderInfoContainer = header->addNew<Wt::WContainerWidget>();
    orderInfoContainer->addStyleClass("d-flex flex-column align-items-end");
    
    // Table identifier badge
    tableNumberText_ = orderInfoContainer->addNew<Wt::WText>("No table selected");
    tableNumberText_->addStyleClass("badge bg-info px-3 py-1 rounded-pill mb-1");
    
    // Order ID text
    orderIdText_ = orderInfoContainer->addNew<Wt::WText>("No active order");
    orderIdText_->addStyleClass("text-white-50 small");
    
    return std::move(header);
}

void CurrentOrderDisplay::initializeTableHeaders() {
    if (!itemsTable_) {
        return;
    }
    
    // Create table headers
    itemsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    itemsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Price"));
    itemsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Qty"));
    itemsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));
    itemsTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));
    
    // CLEAN: Simple header styling without excessive border classes
    for (int col = 0; col < itemsTable_->columnCount(); ++col) {
        auto headerCell = itemsTable_->elementAt(0, col);
        headerCell->addStyleClass("pos-table-header");
        
        auto headerText = dynamic_cast<Wt::WText*>(headerCell->widget(0));
        if (headerText) {
            headerText->addStyleClass("fw-bold");
        }
    }
    
    std::cout << "✓ Table headers styled with clean design" << std::endl;
}

void CurrentOrderDisplay::createOrderSummaryContent() {
    if (!summaryContainer_) return;
    
    // Title
    auto titleText = summaryContainer_->addNew<Wt::WText>("💰 Order Summary");
    titleText->addStyleClass("h5 text-success mb-3 fw-bold");
    
    // CLEAN: Simple summary grid without excessive containers
    auto summaryGrid = summaryContainer_->addNew<Wt::WContainerWidget>();
    summaryGrid->addStyleClass("d-flex flex-column gap-2");
    
    // Item count row
    auto itemCountRow = summaryGrid->addNew<Wt::WContainerWidget>();
    itemCountRow->addStyleClass("d-flex justify-content-between align-items-center");
    auto itemLabel = itemCountRow->addNew<Wt::WText>("Items:");
    itemLabel->addStyleClass("fw-medium text-muted");
    itemCountText_ = itemCountRow->addNew<Wt::WText>("0");
    itemCountText_->addStyleClass("fw-bold");
    
    // Subtotal row
    auto subtotalRow = summaryGrid->addNew<Wt::WContainerWidget>();
    subtotalRow->addStyleClass("d-flex justify-content-between align-items-center");
    auto subtotalLabel = subtotalRow->addNew<Wt::WText>("Subtotal:");
    subtotalLabel->addStyleClass("fw-medium text-muted");
    subtotalText_ = subtotalRow->addNew<Wt::WText>("$0.00");
    subtotalText_->addStyleClass("fw-bold");
    
    // Tax row
    auto taxRow = summaryGrid->addNew<Wt::WContainerWidget>();
    taxRow->addStyleClass("d-flex justify-content-between align-items-center");
    auto taxLabel = taxRow->addNew<Wt::WText>("Tax:");
    taxLabel->addStyleClass("fw-medium text-muted");
    taxText_ = taxRow->addNew<Wt::WText>("$0.00");
    taxText_->addStyleClass("fw-bold");
    
    // CLEAN: Simple separator without excessive borders
    auto separator = summaryGrid->addNew<Wt::WContainerWidget>();
    separator->addStyleClass("border-top my-2");
    
    // Total row with emphasis
    auto totalRow = summaryGrid->addNew<Wt::WContainerWidget>();
    totalRow->addStyleClass("d-flex justify-content-between align-items-center");
    auto totalLabel = totalRow->addNew<Wt::WText>("TOTAL:");
    totalLabel->addStyleClass("h5 fw-bold text-success mb-0");
    totalText_ = totalRow->addNew<Wt::WText>("$0.00");
    totalText_->addStyleClass("h5 fw-bold text-success mb-0");
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
        nameText->addStyleClass("fw-bold text-primary");
        
        // Show special instructions if any
        if (!item.getSpecialInstructions().empty()) {
            nameContainer->addNew<Wt::WBreak>();
            auto instructionsText = nameContainer->addNew<Wt::WText>("📝 " + item.getSpecialInstructions());
            instructionsText->addStyleClass("small text-muted fst-italic");
        }
        
        itemsTable_->elementAt(row, 0)->addWidget(std::move(nameContainer));
        
        // Unit price
        auto priceText = std::make_unique<Wt::WText>(formatCurrency(item.getMenuItem().getPrice()));
        priceText->addStyleClass("fw-medium text-success");
        itemsTable_->elementAt(row, 1)->addWidget(std::move(priceText));
        
        // Quantity controls (editable spinner + buttons)
        auto qtyContainer = std::make_unique<Wt::WContainerWidget>();
        qtyContainer->setStyleClass("d-flex align-items-center justify-content-center gap-1");
        
        if (editable_) {
            // Decrease button with clean styling
            auto decreaseBtn = qtyContainer->addNew<Wt::WPushButton>("-");
            UIStyleHelper::styleButton(decreaseBtn, "outline-secondary", "sm");
            decreaseBtn->setWidth(30);
            
            // Quantity display/spinner
            auto qtySpinner = qtyContainer->addNew<Wt::WSpinBox>();
            qtySpinner->setRange(1, 99);
            qtySpinner->setValue(item.getQuantity());
            qtySpinner->setWidth(60);
            UIStyleHelper::styleFormControl(qtySpinner, "sm");
            qtySpinner->addStyleClass("text-center");
            
            // Increase button with clean styling
            auto increaseBtn = qtyContainer->addNew<Wt::WPushButton>("+");
            UIStyleHelper::styleButton(increaseBtn, "outline-secondary", "sm");
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
            // Remove item button with clean styling
            auto removeBtn = actionsContainer->addNew<Wt::WPushButton>("🗑️ Remove");
            UIStyleHelper::styleButton(removeBtn, "outline-danger", "sm");
            removeBtn->clicked().connect([this, index]() {
                onRemoveItemClicked(index);
            });
            
            // Quick duplicate button with clean styling
            auto duplicateBtn = actionsContainer->addNew<Wt::WPushButton>("📋 +1");
            UIStyleHelper::styleButton(duplicateBtn, "outline-info", "sm");
            duplicateBtn->setToolTip("Add another of this item");
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
        
        // CLEAN: Simple cell styling without excessive borders
        cell->addStyleClass("pos-table-cell");
        
        // CLEAN: Very subtle alternating row colors
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
    std::cout << "[CurrentOrderDisplay] Refreshed with clean styling" << std::endl;
}

void CurrentOrderDisplay::clearOrder() {
    updateOrderItemsTable();
    updateOrderSummary();
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
