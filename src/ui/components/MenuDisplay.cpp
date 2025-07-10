#include "../../../include/ui/components/MenuDisplay.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WSpinBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WMessageBox.h>
#include <Wt/WLabel.h>
#include <Wt/WBreak.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

MenuDisplay::MenuDisplay(std::shared_ptr<POSService> posService,
                        std::shared_ptr<EventManager> eventManager,
                        std::shared_ptr<ThemeService> themeService)
    : Wt::WContainerWidget()
    , posService_(posService)
    , eventManager_(eventManager)
    , themeService_(themeService)
    , isDestroying_(false)  // Add destruction flag
    , currentCategory_("")
    , selectionEnabled_(true)
    , categoryCombo_(nullptr)
    , itemCountText_(nullptr)
    , itemsTable_(nullptr)
    , headerContainer_(nullptr)
{
    if (!posService_) {
        throw std::invalid_argument("MenuDisplay requires valid POSService");
    }
    
    setStyleClass("menu-display p-3 bg-white border rounded shadow-sm");
    
    initializeUI();
    setupEventListeners();
    loadMenuItems();
    refresh();
    
    std::cout << "[MenuDisplay] Initialized with " << menuItems_.size() << " menu items" << std::endl;
}

MenuDisplay::~MenuDisplay() {
    std::cout << "[MenuDisplay] Destructor called - setting destruction flag" << std::endl;
    
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
        
        std::cout << "[MenuDisplay] Cleanup completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[MenuDisplay] Error during destruction: " << e.what() << std::endl;
    }
}

void MenuDisplay::initializeUI() {
    // FIXED: Remove WGroupBox to eliminate inner border, use simple container structure
    // Add heading without border (like OrderEntryPanel and KitchenStatusDisplay)
    auto headingText = addNew<Wt::WText>("🍽️ Restaurant Menu");
    headingText->addStyleClass("h4 text-primary mb-3");
    
    // Create header with category filter (no border)
    auto header = createMenuHeader();
    addWidget(std::move(header));
    
    // Create table container (no additional border)
    auto tableContainer = addNew<Wt::WContainerWidget>();
    tableContainer->setStyleClass("mt-3");
    
    // Create menu items table
    itemsTable_ = tableContainer->addNew<Wt::WTable>();
    UIStyleHelper::styleTable(itemsTable_, "menu");
    
    initializeTableHeaders();
    
    std::cout << "[MenuDisplay] UI initialized without inner border" << std::endl;
}

void MenuDisplay::initializeTableHeaders() {
    if (!itemsTable_) return;
    
    // Create table headers - simplified columns
    itemsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    itemsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Price"));
    itemsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Category"));
    itemsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Add to Order"));
    
    // Style headers
    for (int col = 0; col < 4; ++col) {
        auto headerCell = itemsTable_->elementAt(0, col);
        headerCell->setStyleClass("bg-primary text-white fw-bold text-center p-2");
    }
}

std::unique_ptr<Wt::WWidget> MenuDisplay::createMenuHeader() {
    // Create header container
    auto headerContainer = std::make_unique<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(headerContainer.get(), "between", "center");
    
    // Store raw pointer for later reference
    headerContainer_ = headerContainer.get();
    
    // Left side: Category filter
    auto leftSide = headerContainer->addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(leftSide, "start", "center");
    
    auto categoryLabel = leftSide->addNew<Wt::WLabel>("Category: ");
    categoryLabel->setStyleClass("form-label fw-bold me-2");
    
    categoryCombo_ = leftSide->addNew<Wt::WComboBox>();
    UIStyleHelper::styleComboBox(categoryCombo_);
    categoryCombo_->setWidth(200);
    populateCategoryCombo();
    
    // Connect category change handler
    categoryCombo_->changed().connect([this]() {
        onCategoryChanged();
    });
    
    // Right side: Item count and current order status
    auto rightSide = headerContainer->addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(rightSide, "end", "center");
    
    itemCountText_ = rightSide->addNew<Wt::WText>("0 items");
    UIStyleHelper::styleBadge(itemCountText_, "info");
    
    // Current order indicator
    auto orderStatusText = rightSide->addNew<Wt::WText>();
    auto currentOrder = posService_->getCurrentOrder();
    if (currentOrder) {
        int itemCount = currentOrder->getItems().size();
        orderStatusText->setText("Current Order: " + std::to_string(itemCount) + " items");
        orderStatusText->setStyleClass("badge bg-success ms-2");
    } else {
        orderStatusText->setText("No Active Order");
        orderStatusText->setStyleClass("badge bg-secondary ms-2");
    }
    
    return std::move(headerContainer);
}

void MenuDisplay::loadMenuItems() {
    if (!posService_) {
        std::cerr << "[MenuDisplay] No POS service available" << std::endl;
        return;
    }
    
    menuItems_ = posService_->getMenuItems();
    organizeItemsByCategory();
    
    std::cout << "[MenuDisplay] Loaded " << menuItems_.size() << " menu items from service" << std::endl;
}

void MenuDisplay::organizeItemsByCategory() {
    itemsByCategory_.clear();
    categories_.clear();
    
    for (const auto& item : menuItems_) {
        if (!item) continue;
        
        std::string categoryName = MenuItem::categoryToString(item->getCategory());
        itemsByCategory_[categoryName].push_back(item);
        
        if (std::find(categories_.begin(), categories_.end(), categoryName) == categories_.end()) {
            categories_.push_back(categoryName);
        }
    }
    
    std::cout << "[MenuDisplay] Organized into " << categories_.size() << " categories" << std::endl;
}

void MenuDisplay::populateCategoryCombo() {
    if (!categoryCombo_) return;
    
    categoryCombo_->clear();
    categoryCombo_->addItem(ALL_CATEGORIES);
    
    for (const auto& category : categories_) {
        categoryCombo_->addItem(category);
    }
    
    categoryCombo_->setCurrentIndex(0);
}

void MenuDisplay::updateMenuItemsTable() {
    if (!itemsTable_) return;
    
    // Clear existing rows (except header)
    while (itemsTable_->rowCount() > 1) {
        itemsTable_->removeRow(1);
    }
    
    auto filteredItems = getFilteredItems();
    
    if (filteredItems.empty()) {
        // Show empty message
        auto emptyRow = itemsTable_->elementAt(1, 0);
        emptyRow->setColumnSpan(4);
        emptyRow->addWidget(std::make_unique<Wt::WText>("No items in this category"));
        emptyRow->setStyleClass("text-center text-muted p-4");
        return;
    }
    
    // Add menu item rows
    for (size_t i = 0; i < filteredItems.size(); ++i) {
        addMenuItemRow(filteredItems[i], i);
    }
    
    updateItemCount();
    
    // The order status will be updated when the event system triggers refresh
}

void MenuDisplay::addMenuItemRow(const std::shared_ptr<MenuItem>& item, size_t index) {
    if (!item || !itemsTable_) return;
    
    int row = static_cast<int>(index + 1); // +1 for header row
    
    try {
        // Item name and description
        auto nameContainer = std::make_unique<Wt::WContainerWidget>();
        auto itemName = nameContainer->addNew<Wt::WText>(item->getName());
        itemName->setStyleClass("fw-bold text-dark");
        
        // Add a subtle description if available
        std::string description = formatItemDescription(item);
        if (!description.empty()) {
            nameContainer->addNew<Wt::WBreak>();
            auto descText = nameContainer->addNew<Wt::WText>(description);
            descText->setStyleClass("small text-muted");
        }
        
        itemsTable_->elementAt(row, 0)->addWidget(std::move(nameContainer));
        
        // Price
        auto priceText = std::make_unique<Wt::WText>(formatCurrency(item->getPrice()));
        priceText->setStyleClass("fw-bold text-success");
        itemsTable_->elementAt(row, 1)->addWidget(std::move(priceText));
        
        // Category
        auto categoryText = std::make_unique<Wt::WText>(MenuItem::categoryToString(item->getCategory()));
        UIStyleHelper::styleBadge(categoryText.get(), "secondary");
        itemsTable_->elementAt(row, 2)->addWidget(std::move(categoryText));
        
        // Add to Order Controls - SIMPLIFIED NO DIALOG
        auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
        UIStyleHelper::styleFlexRow(actionsContainer.get(), "center", "center");
        
        if (selectionEnabled_ && item->isAvailable() && canAddToOrder()) {
            // Quantity spinner (small)
            auto qtySpinner = actionsContainer->addNew<Wt::WSpinBox>();
            qtySpinner->setRange(1, 10);
            qtySpinner->setValue(1);
            qtySpinner->setWidth(60);
            qtySpinner->setStyleClass("form-control form-control-sm me-2");
            
            // Direct Add button - no dialog!
            auto addBtn = actionsContainer->addNew<Wt::WPushButton>("+ Add");
            UIStyleHelper::styleButton(addBtn, "success", "sm");
            
            // DIRECT ADD - no dialog popup
            addBtn->clicked().connect([this, item, qtySpinner]() {
                int quantity = qtySpinner->value();
                std::cout << "[MenuDisplay] Adding " << quantity << "x " << item->getName() 
                          << " directly to order" << std::endl;
                
                // Add directly to current order
                addItemToCurrentOrder(*item, quantity, "");
                
                // Note: Don't reset spinner here as the table refresh will recreate everything
            });
            
        } else {
            auto statusBtn = actionsContainer->addNew<Wt::WPushButton>();
            
            if (!item->isAvailable()) {
                statusBtn->setText("Unavailable");
                UIStyleHelper::styleButton(statusBtn, "outline-secondary", "sm");
                statusBtn->setEnabled(false);
            } else if (!canAddToOrder()) {
                statusBtn->setText("No Order");
                UIStyleHelper::styleButton(statusBtn, "outline-warning", "sm");
                statusBtn->setEnabled(false);
                statusBtn->setToolTip("Create a new order first");
            }
        }
        
        itemsTable_->elementAt(row, 3)->addWidget(std::move(actionsContainer));
        
        // Apply row styling
        bool isEven = (row % 2 == 0);
        for (int col = 0; col < 4; ++col) {
            auto cell = itemsTable_->elementAt(row, col);
            cell->setStyleClass("p-3 align-middle");
            if (col == 1 || col == 2 || col == 3) { // Price, Category, Actions
                cell->addStyleClass("text-center");
            }
            if (isEven) {
                cell->addStyleClass("bg-light");
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[MenuDisplay] Error adding menu item row: " << e.what() << std::endl;
    }
}

void MenuDisplay::addItemToCurrentOrder(const MenuItem& item, int quantity, const std::string& instructions) {
    std::cout << "[MenuDisplay] Adding to order: " << item.getName() 
              << " (qty: " << quantity << ")" << std::endl;
    
    if (!posService_) {
        showMessage("Service not available", "error");
        return;
    }
    
    if (!canAddToOrder()) {
        showMessage("No current order. Please start a new order first.", "warning");
        return;
    }
    
    // Add the item to the order
    try {
        bool success = posService_->addItemToCurrentOrder(item, quantity, instructions);
        
        if (success) {
            std::string message = "Added " + std::to_string(quantity) + "x " + item.getName();
            showMessage(message, "success");
            std::cout << "[MenuDisplay] ✓ Successfully added item to current order" << std::endl;
            
            // Refresh the table to update order status
            updateMenuItemsTable();
            
        } else {
            showMessage("Failed to add item to order", "error");
            std::cout << "[MenuDisplay] ✗ Failed to add item to current order" << std::endl;
        }
    } catch (const std::exception& e) {
        std::string error = "Error adding item: " + std::string(e.what());
        showMessage(error, "error");
        std::cout << "[MenuDisplay] ✗ Exception: " << e.what() << std::endl;
    }
}

bool MenuDisplay::canAddToOrder() const {
    bool result = posService_ && posService_->getCurrentOrder() != nullptr;
    return result;
}

void MenuDisplay::showMessage(const std::string& message, const std::string& type) {
    std::cout << "[MenuDisplay] " << type << ": " << message << std::endl;
    
    // Use event manager for notifications if available
    if (eventManager_) {
        auto notificationData = POSEvents::createNotificationData(message, type, 3000);
        eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED, notificationData);
    } else {
        // Fallback to console log only (no blocking message box)
        std::cout << "[MenuDisplay] Notification: " << message << std::endl;
    }
}

void MenuDisplay::onCategoryChanged() {
    if (!categoryCombo_) return;
    
    int index = categoryCombo_->currentIndex();
    if (index == 0) {
        currentCategory_ = "";  // All categories
    } else {
        currentCategory_ = categoryCombo_->currentText().toUTF8();
    }
    
    std::cout << "[MenuDisplay] Category changed to: " << 
                 (currentCategory_.empty() ? "All" : currentCategory_) << std::endl;
    updateMenuItemsTable();
}

std::vector<std::shared_ptr<MenuItem>> MenuDisplay::getFilteredItems() const {
    if (currentCategory_.empty()) {
        return menuItems_; // Return all items
    }
    
    auto it = itemsByCategory_.find(currentCategory_);
    if (it != itemsByCategory_.end()) {
        return it->second;
    }
    
    return {}; // Empty vector
}

void MenuDisplay::updateItemCount() {
    if (!itemCountText_) return;
    
    auto filteredItems = getFilteredItems();
    std::string countText = std::to_string(filteredItems.size()) + " items";
    if (!currentCategory_.empty()) {
        countText += " in " + currentCategory_;
    }
    itemCountText_->setText(countText);
}

std::string MenuDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "$" << amount;
    return oss.str();
}

std::string MenuDisplay::formatItemDescription(const std::shared_ptr<MenuItem>& item) const {
    // Simple description based on category
    if (!item) return "";
    
    switch (item->getCategory()) {
        case MenuItem::APPETIZER:
            return "Perfect for sharing";
        case MenuItem::MAIN_COURSE:
            return "Served with side dish";
        case MenuItem::DESSERT:
            return "Sweet ending to your meal";
        case MenuItem::BEVERAGE:
            return "Refreshing drink";
        case MenuItem::SPECIAL:
            return "Chef's recommendation";
        default:
            return "";
    }
}

// EVENT HANDLERS
void MenuDisplay::setupEventListeners() {
    if (!eventManager_) return;
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::MENU_UPDATED, 
            [this](const std::any& data) { handleMenuUpdated(data); })
    );
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { handleCurrentOrderChanged(data); })
    );
    
    // Removed ORDER_MODIFIED subscription to avoid unnecessary table refreshes
}

void MenuDisplay::handleMenuUpdated(const std::any& eventData) {
    std::cout << "[MenuDisplay] Menu updated event received" << std::endl;
    loadMenuItems();
    refresh();
}

void MenuDisplay::handleCurrentOrderChanged(const std::any& eventData) {
    std::cout << "[MenuDisplay] Current order changed event received" << std::endl;
    // Only refresh when the current order itself changes (created/cleared)
    // This recreates the table with updated button states
    updateMenuItemsTable();
}

void MenuDisplay::handleThemeChanged(const std::any& eventData) {
    std::cout << "[MenuDisplay] Theme changed event received" << std::endl;
    // Apply new theme if needed
}

// PUBLIC INTERFACE METHODS
void MenuDisplay::refresh() {
    populateCategoryCombo();
    updateMenuItemsTable();
    std::cout << "[MenuDisplay] Refreshed" << std::endl;
}

void MenuDisplay::setCategory(const std::string& category) {
    currentCategory_ = category;
    
    if (categoryCombo_) {
        if (category.empty()) {
            categoryCombo_->setCurrentIndex(0);
        } else {
            for (int i = 1; i < categoryCombo_->count(); ++i) {
                if (categoryCombo_->itemText(i).toUTF8() == category) {
                    categoryCombo_->setCurrentIndex(i);
                    break;
                }
            }
        }
    }
    
    updateMenuItemsTable();
}

std::string MenuDisplay::getCurrentCategory() const {
    return currentCategory_;
}

void MenuDisplay::setSelectionEnabled(bool enabled) {
    selectionEnabled_ = enabled;
    updateMenuItemsTable(); // Refresh to update button states
}

bool MenuDisplay::isSelectionEnabled() const {
    return selectionEnabled_;
}

std::vector<std::string> MenuDisplay::getAvailableCategories() const {
    return categories_;
}

// VALIDATION METHODS
bool MenuDisplay::validateQuantity(int quantity) const {
    return quantity > 0 && quantity <= MAX_QUANTITY;
}

bool MenuDisplay::validateSpecialInstructions(const std::string& instructions) const {
    return instructions.length() <= MAX_INSTRUCTIONS_LENGTH;
}

// REMOVED: All dialog-related methods
// - showAddToOrderDialog() - completely removed
// - Dialog event handlers - removed

// PLACEHOLDER IMPLEMENTATIONS (keep for header compatibility)
void MenuDisplay::onAddToOrderClicked(const std::shared_ptr<MenuItem>& item, int quantity, const std::string& instructions) {
    addItemToCurrentOrder(*item, quantity, instructions);
}

void MenuDisplay::onItemRowClicked(const std::shared_ptr<MenuItem>& item) {
    // Could be used for item details view in the future
    std::cout << "[MenuDisplay] Item clicked: " << item->getName() << std::endl;
}

// Placeholder methods for interface compatibility
void MenuDisplay::applyTheme() {
    // Theme application logic if needed
}

void MenuDisplay::applyTableStyling() {
    // Table styling logic if needed
}

void MenuDisplay::applyHeaderStyling() {
    // Header styling logic if needed
}

void MenuDisplay::updateRowStyling(int row, bool isEven) {
    // Row styling logic if needed
}

void MenuDisplay::applyItemRowStyling(int row, const std::shared_ptr<MenuItem>& item) {
    // Item row styling logic if needed
}
