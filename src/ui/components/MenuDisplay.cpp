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
    // Container with proper width constraints for left panel
    addStyleClass("menu-display-container");
    setAttributeValue("style",
        "width: 100% !important; max-width: 100% !important; "
        "overflow: hidden !important; box-sizing: border-box !important;");

    // Add heading
    auto headingText = addNew<Wt::WText>("🍽️ Restaurant Menu");
    headingText->addStyleClass("h5 text-primary mb-2");
    headingText->setAttributeValue("style", "font-size: 1rem;");

    // Create header with category filter
    auto header = createMenuHeader();
    addWidget(std::move(header));

    // Create scrollable table container
    auto tableContainer = addNew<Wt::WContainerWidget>();
    tableContainer->addStyleClass("menu-table-container");
    tableContainer->setAttributeValue("style",
        "width: 100% !important; overflow-x: hidden !important; "
        "overflow-y: auto !important; margin-top: 8px;");

    // Create menu items table with fixed layout
    itemsTable_ = tableContainer->addNew<Wt::WTable>();
    itemsTable_->addStyleClass("menu-items-table");
    itemsTable_->setAttributeValue("style",
        "width: 100% !important; table-layout: fixed !important; "
        "border-collapse: collapse !important;");

    initializeTableHeaders();

    std::cout << "[MenuDisplay] UI initialized with constrained width" << std::endl;
}

void MenuDisplay::initializeTableHeaders() {
    if (!itemsTable_) return;

    // Create table headers - compact layout for left panel
    itemsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    itemsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Price"));
    itemsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Add"));

    // Set column widths - Item gets most space, Price and Add are compact
    itemsTable_->elementAt(0, 0)->setAttributeValue("style",
        "width: 50% !important; padding: 6px 4px !important;");
    itemsTable_->elementAt(0, 1)->setAttributeValue("style",
        "width: 25% !important; padding: 6px 4px !important;");
    itemsTable_->elementAt(0, 2)->setAttributeValue("style",
        "width: 25% !important; padding: 6px 4px !important;");

    // Style headers
    for (int col = 0; col < 3; ++col) {
        auto headerCell = itemsTable_->elementAt(0, col);
        headerCell->addStyleClass("text-center");
        headerCell->setAttributeValue("style",
            headerCell->attributeValue("style").toUTF8() +
            "background: #e9ecef !important; color: #495057 !important; "
            "font-weight: 600 !important; font-size: 0.75rem !important; "
            "border-bottom: 2px solid #dee2e6 !important;");
    }
}

std::unique_ptr<Wt::WWidget> MenuDisplay::createMenuHeader() {
    // Create compact header container
    auto headerContainer = std::make_unique<Wt::WContainerWidget>();
    headerContainer->setAttributeValue("style",
        "display: flex; flex-wrap: wrap; gap: 8px; align-items: center; "
        "width: 100%; padding: 4px 0;");

    // Store raw pointer for later reference
    headerContainer_ = headerContainer.get();

    // Category filter - compact
    auto filterContainer = headerContainer->addNew<Wt::WContainerWidget>();
    filterContainer->setAttributeValue("style",
        "display: flex; align-items: center; gap: 4px;");

    auto categoryLabel = filterContainer->addNew<Wt::WText>("Filter:");
    categoryLabel->setAttributeValue("style",
        "font-size: 0.75rem; font-weight: 600; color: #666;");

    categoryCombo_ = filterContainer->addNew<Wt::WComboBox>();
    categoryCombo_->setAttributeValue("style",
        "height: 26px; font-size: 0.75rem; padding: 2px 6px; "
        "border: 1px solid #ccc; border-radius: 4px; max-width: 120px;");
    populateCategoryCombo();

    categoryCombo_->changed().connect([this]() {
        onCategoryChanged();
    });

    // Item count badge - compact
    itemCountText_ = headerContainer->addNew<Wt::WText>("0 items");
    itemCountText_->setAttributeValue("style",
        "font-size: 0.7rem; padding: 2px 8px; background: #e9ecef; "
        "border-radius: 10px; color: #666;");

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
        emptyRow->setColumnSpan(3);
        emptyRow->addWidget(std::make_unique<Wt::WText>("No items in this category"));
        emptyRow->setAttributeValue("style",
            "text-align: center; color: #999; padding: 16px; font-size: 0.8rem;");
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
        // Item name - compact, no description to save space
        auto nameText = std::make_unique<Wt::WText>(item->getName());
        nameText->setAttributeValue("style",
            "font-size: 0.8rem; font-weight: 600; color: #333; "
            "overflow: hidden; text-overflow: ellipsis; white-space: nowrap;");
        itemsTable_->elementAt(row, 0)->addWidget(std::move(nameText));

        // Price - compact
        auto priceText = std::make_unique<Wt::WText>(formatCurrency(item->getPrice()));
        priceText->setAttributeValue("style",
            "font-size: 0.8rem; font-weight: 600; color: #198754;");
        itemsTable_->elementAt(row, 1)->addWidget(std::move(priceText));

        // Add to Order Controls - compact layout
        auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
        actionsContainer->setAttributeValue("style",
            "display: flex; align-items: center; justify-content: center; gap: 2px;");

        if (selectionEnabled_ && item->isAvailable() && canAddToOrder()) {
            // Quantity spinner - very compact
            auto qtySpinner = actionsContainer->addNew<Wt::WSpinBox>();
            qtySpinner->setRange(1, 10);
            qtySpinner->setValue(1);
            qtySpinner->setAttributeValue("style",
                "width: 40px !important; height: 24px !important; "
                "padding: 2px !important; font-size: 0.75rem; text-align: center;");

            // Direct Add button - compact
            auto addBtn = actionsContainer->addNew<Wt::WPushButton>("+");
            addBtn->setAttributeValue("style",
                "width: 28px !important; height: 24px !important; "
                "padding: 0 !important; font-size: 0.9rem; font-weight: bold; "
                "background: #198754; color: white; border: none; border-radius: 4px;");
            addBtn->setToolTip("Add to order");

            addBtn->clicked().connect([this, item, qtySpinner]() {
                int quantity = qtySpinner->value();
                addItemToCurrentOrder(*item, quantity, "");
            });

        } else {
            auto statusText = actionsContainer->addNew<Wt::WText>(
                item->isAvailable() ? "—" : "N/A");
            statusText->setAttributeValue("style",
                "font-size: 0.7rem; color: #999;");
        }

        itemsTable_->elementAt(row, 2)->addWidget(std::move(actionsContainer));

        // Apply compact row styling
        bool isEven = (row % 2 == 0);
        for (int col = 0; col < 3; ++col) {
            auto cell = itemsTable_->elementAt(row, col);
            cell->setAttributeValue("style",
                std::string("padding: 4px !important; vertical-align: middle; ") +
                "border-bottom: 1px solid #eee; " +
                (col > 0 ? "text-align: center; " : "") +
                (isEven ? "background: #fafafa;" : "background: white;"));
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
