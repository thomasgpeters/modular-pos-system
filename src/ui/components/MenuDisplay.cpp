#include "../../../include/ui/components/MenuDisplay.hpp"

#include <Wt/WApplication.h>
#include <Wt/WLabel.h>
#include <Wt/WTableCell.h>
#include <Wt/WDialog.h>
#include <Wt/WTextArea.h>
#include <Wt/WMessageBox.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

MenuDisplay::MenuDisplay(std::shared_ptr<POSService> posService,
                        std::shared_ptr<EventManager> eventManager,
                        std::shared_ptr<ThemeService> themeService)
    : posService_(posService)
    , eventManager_(eventManager)
    , themeService_(themeService)
    , currentCategory_(ALL_CATEGORIES)
    , selectionEnabled_(true)
    , menuGroup_(nullptr)
    , categoryCombo_(nullptr)
    , itemCountText_(nullptr)
    , itemsTable_(nullptr)
    , headerContainer_(nullptr) {
    
    if (!posService_) {
        throw std::invalid_argument("MenuDisplay requires valid POSService");
    }
    
    addStyleClass("menu-display");
    
    initializeUI();
    setupEventListeners();
    loadMenuItems();
    refresh();
    
    std::cout << "[MenuDisplay] Initialized with " << menuItems_.size() << " menu items" << std::endl;
}

void MenuDisplay::initializeUI() {
    // Create main menu group
    menuGroup_ = addNew<Wt::WGroupBox>("Menu");
    menuGroup_->addStyleClass("menu-group");
    
    // Create layout for menu group
    auto groupLayout = std::make_unique<Wt::WVBoxLayout>();
    groupLayout->setContentsMargins(10, 10, 10, 10);
    
    // Create header
    auto header = createMenuHeader();
    groupLayout->addWidget(std::move(header));
    
    // Create menu items table
    createMenuItemsTable();
    
    // Add table to layout
    auto tableContainer = std::make_unique<Wt::WContainerWidget>();
    tableContainer->addStyleClass("menu-table-container");
    tableContainer->addWidget(std::unique_ptr<Wt::WTable>(itemsTable_));
    groupLayout->addWidget(std::move(tableContainer), 1);
    
    menuGroup_->setLayout(std::move(groupLayout));
    
    // Apply initial styling
    applyTheme();
    applyTableStyling();
    applyHeaderStyling();
    
    std::cout << "[MenuDisplay] UI initialized" << std::endl;
}

void MenuDisplay::setupEventListeners() {
    if (!eventManager_) {
        std::cout << "[MenuDisplay] No EventManager available" << std::endl;
        return;
    }
    
    // Subscribe to menu updates
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::MENU_UPDATED,
            [this](const std::any& data) { handleMenuUpdated(data); }));
    
    // Subscribe to current order changes
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { handleCurrentOrderChanged(data); }));
    
    // Subscribe to order events that affect menu availability
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED,
            [this](const std::any& data) { handleCurrentOrderChanged(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_COMPLETED,
            [this](const std::any& data) { handleCurrentOrderChanged(data); }));
    
    // Subscribe to theme changes if theme service is available
    if (themeService_) {
        themeService_->onThemeChanged([this](auto oldTheme, auto newTheme) {
            handleThemeChanged(std::any{});
        });
    }
    
    std::cout << "[MenuDisplay] Event listeners setup complete" << std::endl;
}

std::unique_ptr<Wt::WWidget> MenuDisplay::createMenuHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("menu-header");
    headerContainer_ = header.get();
    
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    headerLayout->setContentsMargins(0, 0, 0, 10);
    
    // Category filter section
    auto categoryContainer = std::make_unique<Wt::WContainerWidget>();
    categoryContainer->addStyleClass("category-filter");
    
    auto categoryLabel = categoryContainer->addNew<Wt::WLabel>("Category:");
    categoryLabel->addStyleClass("form-label me-2");
    
    categoryCombo_ = categoryContainer->addNew<Wt::WComboBox>();
    categoryCombo_->addStyleClass("form-select category-combo");
    categoryCombo_->changed().connect([this] { onCategoryChanged(); });
    
    headerLayout->addWidget(std::move(categoryContainer));
    
    // Spacer
    headerLayout->addStretch(1);
    
    // Item count section
    auto countContainer = std::make_unique<Wt::WContainerWidget>();
    countContainer->addStyleClass("item-count");
    
    itemCountText_ = countContainer->addNew<Wt::WText>("0 items");
    itemCountText_->addStyleClass("badge bg-secondary");
    
    headerLayout->addWidget(std::move(countContainer));
    
    header->setLayout(std::move(headerLayout));
    return std::move(header);
}

void MenuDisplay::createMenuItemsTable() {
    itemsTable_ = menuGroup_->addNew<Wt::WTable>();
    itemsTable_->addStyleClass("table table-striped table-hover menu-items-table");
    itemsTable_->setWidth(Wt::WLength("100%"));
    
    // Create table headers
    itemsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    itemsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Description"));
    itemsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Price"));
    
    if (selectionEnabled_) {
        itemsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Add to Order"));
    }
    
    // Style headers
    for (int col = 0; col < itemsTable_->columnCount(); ++col) {
        auto headerCell = itemsTable_->elementAt(0, col);
        headerCell->addStyleClass("table-header bg-primary text-white");
    }
    
    std::cout << "[MenuDisplay] Menu items table created" << std::endl;
}

void MenuDisplay::refresh() {
    if (!posService_) {
        std::cerr << "[MenuDisplay] POSService not available for refresh" << std::endl;
        return;
    }
    
    loadMenuItems();
    populateCategoryCombo();
    updateMenuItemsTable();
    updateItemCount();
    
    std::cout << "[MenuDisplay] Refreshed with " << menuItems_.size() << " items" << std::endl;
}

void MenuDisplay::loadMenuItems() {
    if (!posService_) {
        return;
    }
    
    try {
        menuItems_ = posService_->getMenuItems();
        organizeItemsByCategory();
        
        std::cout << "[MenuDisplay] Loaded " << menuItems_.size() << " menu items" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[MenuDisplay] Error loading menu items: " << e.what() << std::endl;
        menuItems_.clear();
    }
}

void MenuDisplay::organizeItemsByCategory() {
    categories_.clear();
    itemsByCategory_.clear();
    
    // Add "All Categories" option
    categories_.push_back(ALL_CATEGORIES);
    itemsByCategory_[ALL_CATEGORIES] = menuItems_;
    
    // Organize items by category
    for (const auto& item : menuItems_) {
        if (!item) continue; // Skip null pointers
        
        std::string category = MenuItem::categoryToString(item->getCategory());
        if (category.empty()) {
            category = "Other";
        }
        
        // Add category if not already present
        if (std::find(categories_.begin(), categories_.end(), category) == categories_.end()) {
            categories_.push_back(category);
        }
        
        itemsByCategory_[category].push_back(item);
    }
    
    // Sort categories alphabetically (except "All Categories" which stays first)
    std::sort(categories_.begin() + 1, categories_.end());
    
    std::cout << "[MenuDisplay] Organized items into " << (categories_.size() - 1) << " categories" << std::endl;
}

void MenuDisplay::populateCategoryCombo() {
    if (!categoryCombo_) {
        return;
    }
    
    categoryCombo_->clear();
    
    for (const auto& category : categories_) {
        categoryCombo_->addItem(category);
    }
    
    // Set current selection
    auto it = std::find(categories_.begin(), categories_.end(), currentCategory_);
    if (it != categories_.end()) {
        categoryCombo_->setCurrentIndex(std::distance(categories_.begin(), it));
    }
}

void MenuDisplay::updateMenuItemsTable() {
    if (!itemsTable_) {
        return;
    }
    
    // Clear existing rows (except header)
    while (itemsTable_->rowCount() > 1) {
        itemsTable_->removeRow(1);
    }
    
    // Get filtered items
    auto filteredItems = getFilteredItems();
    
    // Add menu item rows
    for (size_t i = 0; i < filteredItems.size(); ++i) {
        addMenuItemRow(filteredItems[i], i);
    }
    
    // Apply table styling
    applyTableStyling();
    
    std::cout << "[MenuDisplay] Updated table with " << filteredItems.size() << " items" << std::endl;
}

void MenuDisplay::addMenuItemRow(const std::shared_ptr<MenuItem>& item, size_t index) {
    if (!item) return; // Skip null pointers
    
    int row = itemsTable_->rowCount();
    
    // Item name
    auto nameContainer = std::make_unique<Wt::WContainerWidget>();
    auto nameText = nameContainer->addNew<Wt::WText>(item->getName());
    nameText->addStyleClass("menu-item-name fw-bold");
    
    // Add category badge if showing all categories
    if (currentCategory_ == ALL_CATEGORIES) {
        std::string categoryName = MenuItem::categoryToString(item->getCategory());
        if (!categoryName.empty()) {
            auto categoryBadge = nameContainer->addNew<Wt::WText>(" " + categoryName);
            categoryBadge->addStyleClass("badge bg-info ms-2");
        }
    }
    
    itemsTable_->elementAt(row, 0)->addWidget(std::move(nameContainer));
    
    // Description
    auto descText = std::make_unique<Wt::WText>(formatItemDescription(item));
    descText->addStyleClass("menu-item-description text-muted");
    itemsTable_->elementAt(row, 1)->addWidget(std::move(descText));
    
    // Price
    auto priceText = std::make_unique<Wt::WText>(formatCurrency(item->getPrice()));
    priceText->addStyleClass("menu-item-price fw-bold text-success");
    itemsTable_->elementAt(row, 2)->addWidget(std::move(priceText));
    
    // Add to order button (if selection enabled)
    if (selectionEnabled_) {
        auto addButton = std::make_unique<Wt::WPushButton>("Add");
        addButton->addStyleClass("btn btn-primary btn-sm");
        addButton->setToolTip("Add " + item->getName() + " to order");
        
        addButton->clicked().connect([this, item]() {
            showAddToOrderDialog(item);
        });
        
        itemsTable_->elementAt(row, 3)->addWidget(std::move(addButton));
    }
    
    // Apply row styling
    applyItemRowStyling(row, item);
    updateRowStyling(row, (index % 2) == 0);
}

void MenuDisplay::showAddToOrderDialog(const std::shared_ptr<MenuItem>& item) {
    if (!item || !canAddToOrder()) {
        showMessage("No active order. Please start a new order first.", "warning");
        return;
    }
    
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Add to Order"));
    dialog->addStyleClass("add-to-order-dialog");
    dialog->setModal(true);
    dialog->setResizable(false);
    
    auto contents = std::make_unique<Wt::WContainerWidget>();
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Item info
    auto itemInfo = std::make_unique<Wt::WContainerWidget>();
    itemInfo->addStyleClass("item-info mb-3");
    
    auto itemTitle = itemInfo->addNew<Wt::WText>(item->getName());
    itemTitle->addStyleClass("h5 mb-1");
    
    auto itemPrice = itemInfo->addNew<Wt::WText>(formatCurrency(item->getPrice()));
    itemPrice->addStyleClass("text-success fw-bold");
    
    // Note: MenuItem doesn't have getDescription() method, so we'll skip it for now
    // If you want to add description support, you'll need to add it to MenuItem class
    
    layout->addWidget(std::move(itemInfo));
    
    // Quantity section
    auto quantityContainer = std::make_unique<Wt::WContainerWidget>();
    quantityContainer->addStyleClass("quantity-section mb-3");
    
    auto quantityLabel = quantityContainer->addNew<Wt::WLabel>("Quantity:");
    quantityLabel->addStyleClass("form-label");
    
    auto quantitySpinBox = quantityContainer->addNew<Wt::WSpinBox>();
    quantitySpinBox->setRange(1, MAX_QUANTITY);
    quantitySpinBox->setValue(1);
    quantitySpinBox->addStyleClass("form-control");
    
    layout->addWidget(std::move(quantityContainer));
    
    // Special instructions section
    auto instructionsContainer = std::make_unique<Wt::WContainerWidget>();
    instructionsContainer->addStyleClass("instructions-section mb-3");
    
    auto instructionsLabel = instructionsContainer->addNew<Wt::WLabel>("Special Instructions:");
    instructionsLabel->addStyleClass("form-label");
    
    auto instructionsTextArea = instructionsContainer->addNew<Wt::WTextArea>();
    instructionsTextArea->setPlaceholderText("Optional special instructions...");
    instructionsTextArea->addStyleClass("form-control");
    instructionsTextArea->setRows(3);
    
    layout->addWidget(std::move(instructionsContainer));
    
    contents->setLayout(std::move(layout));
    dialog->contents()->addWidget(std::move(contents));
    
    // Dialog buttons
    auto buttonContainer = std::make_unique<Wt::WContainerWidget>();
    buttonContainer->addStyleClass("dialog-buttons");
    
    auto buttonLayout = std::make_unique<Wt::WHBoxLayout>();
    buttonLayout->addStretch(1);
    
    auto cancelButton = std::make_unique<Wt::WPushButton>("Cancel");
    cancelButton->addStyleClass("btn btn-secondary me-2");
    cancelButton->clicked().connect([dialog]() {
        dialog->reject();
    });
    buttonLayout->addWidget(std::move(cancelButton));
    
    auto addButton = std::make_unique<Wt::WPushButton>("Add to Order");
    addButton->addStyleClass("btn btn-primary");
    addButton->clicked().connect([this, dialog, item, quantitySpinBox, instructionsTextArea]() {
        int quantity = quantitySpinBox->value();
        std::string instructions = instructionsTextArea->text().toUTF8();
        
        if (validateQuantity(quantity) && validateSpecialInstructions(instructions)) {
            addItemToCurrentOrder(*item, quantity, instructions);
            dialog->accept();
        }
    });
    buttonLayout->addWidget(std::move(addButton));
    
    buttonContainer->setLayout(std::move(buttonLayout));
    dialog->footer()->addWidget(std::move(buttonContainer));
    
    dialog->show();
}

void MenuDisplay::addItemToCurrentOrder(const MenuItem& item, int quantity, const std::string& instructions) {
    if (!posService_ || !canAddToOrder()) {
        showMessage("Cannot add item to order", "error");
        return;
    }
    
    try {
        // Use the enhanced POSService method that supports quantity and instructions
        bool success = posService_->addItemToCurrentOrder(item, quantity, instructions);
        
        if (success) {
            std::string message = "Added " + std::to_string(quantity) + "x " + item.getName() + " to order";
            if (!instructions.empty()) {
                message += " (Note: " + instructions + ")";
            }
            showMessage(message, "success");
        } else {
            showMessage("Failed to add " + item.getName() + " to order", "error");
        }
    } catch (const std::exception& e) {
        showMessage("Error adding item: " + std::string(e.what()), "error");
    }
}

std::vector<std::shared_ptr<MenuItem>> MenuDisplay::getFilteredItems() const {
    if (currentCategory_ == ALL_CATEGORIES) {
        return menuItems_;
    }
    
    auto it = itemsByCategory_.find(currentCategory_);
    if (it != itemsByCategory_.end()) {
        return it->second;
    }
    
    return std::vector<std::shared_ptr<MenuItem>>();
}

void MenuDisplay::updateItemCount() {
    if (!itemCountText_) {
        return;
    }
    
    auto filteredItems = getFilteredItems();
    std::string countText = std::to_string(filteredItems.size()) + " items";
    
    if (currentCategory_ != ALL_CATEGORIES) {
        countText += " in " + currentCategory_;
    }
    
    itemCountText_->setText(countText);
}

// Event handlers
void MenuDisplay::handleMenuUpdated(const std::any& eventData) {
    std::cout << "[MenuDisplay] Menu updated event received" << std::endl;
    refresh();
}

void MenuDisplay::handleCurrentOrderChanged(const std::any& eventData) {
    // Update UI state based on current order availability
    bool hasOrder = canAddToOrder();
    std::cout << "[MenuDisplay] Current order changed, has order: " << (hasOrder ? "yes" : "no") << std::endl;
}

void MenuDisplay::handleThemeChanged(const std::any& eventData) {
    std::cout << "[MenuDisplay] Theme changed event received" << std::endl;
    applyTheme();
}

void MenuDisplay::onCategoryChanged() {
    if (!categoryCombo_) {
        return;
    }
    
    int index = categoryCombo_->currentIndex();
    if (index >= 0 && index < static_cast<int>(categories_.size())) {
        currentCategory_ = categories_[index];
        updateMenuItemsTable();
        updateItemCount();
        
        std::cout << "[MenuDisplay] Category changed to: " << currentCategory_ << std::endl;
    }
}

// Public interface methods
void MenuDisplay::setCategory(const std::string& category) {
    currentCategory_ = category;
    
    if (categoryCombo_) {
        auto it = std::find(categories_.begin(), categories_.end(), category);
        if (it != categories_.end()) {
            categoryCombo_->setCurrentIndex(std::distance(categories_.begin(), it));
        }
    }
    
    updateMenuItemsTable();
    updateItemCount();
}

std::string MenuDisplay::getCurrentCategory() const {
    return currentCategory_;
}

void MenuDisplay::setSelectionEnabled(bool enabled) {
    if (selectionEnabled_ != enabled) {
        selectionEnabled_ = enabled;
        refresh(); // Rebuild table with/without selection controls
    }
}

bool MenuDisplay::isSelectionEnabled() const {
    return selectionEnabled_;
}

std::vector<std::string> MenuDisplay::getAvailableCategories() const {
    return categories_;
}

// Helper methods
bool MenuDisplay::canAddToOrder() const {
    return posService_ && posService_->getCurrentOrder() != nullptr;
}

std::string MenuDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << "$" << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}

std::string MenuDisplay::formatItemDescription(const std::shared_ptr<MenuItem>& item) const {
    if (!item) {
        return "No description available";
    }
    
    // Since MenuItem doesn't have a getDescription() method in the provided interface,
    // we'll create a basic description from the available information
    std::string description = "Category: " + MenuItem::categoryToString(item->getCategory());
    
    if (!item->isAvailable()) {
        description += " (Currently Unavailable)";
    }
    
    return description;
}

bool MenuDisplay::validateQuantity(int quantity) const {
    return quantity >= 1 && quantity <= MAX_QUANTITY;
}

bool MenuDisplay::validateSpecialInstructions(const std::string& instructions) const {
    return instructions.length() <= MAX_INSTRUCTIONS_LENGTH;
}

void MenuDisplay::showMessage(const std::string& message, const std::string& type) {
    std::cout << "[MenuDisplay] " << type << ": " << message << std::endl;
    
    // In a real implementation, you might want to show a toast notification
    // For now, just log it
}

// Styling methods
void MenuDisplay::applyTheme() {
    if (themeService_) {
        themeService_->applyThemeToContainer(this);
        if (menuGroup_) {
            themeService_->applyThemeToContainer(menuGroup_);
        }
    }
}

void MenuDisplay::applyTableStyling() {
    if (!itemsTable_) {
        return;
    }
    
    // Set column widths
    for (int col = 0; col < itemsTable_->columnCount(); ++col) {
        auto headerCell = itemsTable_->elementAt(0, col);
        switch (col) {
            case 0: // Item name
                headerCell->setWidth(Wt::WLength("25%"));
                break;
            case 1: // Description
                headerCell->setWidth(Wt::WLength("45%"));
                break;
            case 2: // Price
                headerCell->setWidth(Wt::WLength("15%"));
                break;
            case 3: // Add button
                if (selectionEnabled_) {
                    headerCell->setWidth(Wt::WLength("15%"));
                }
                break;
        }
    }
}

void MenuDisplay::applyHeaderStyling() {
    if (headerContainer_) {
        headerContainer_->addStyleClass("border-bottom pb-2 mb-3");
    }
}

void MenuDisplay::updateRowStyling(int row, bool isEven) {
    if (!itemsTable_ || row == 0) {
        return;
    }
    
    std::string rowClass = isEven ? "table-row-even" : "table-row-odd";
    
    for (int col = 0; col < itemsTable_->columnCount(); ++col) {
        auto cell = itemsTable_->elementAt(row, col);
        cell->addStyleClass("menu-item-cell " + rowClass);
        
        // Right-align price column
        if (col == 2) {
            cell->addStyleClass("text-end");
        }
        
        // Center-align button column
        if (col == 3 && selectionEnabled_) {
            cell->addStyleClass("text-center");
        }
    }
}

void MenuDisplay::applyItemRowStyling(int row, const std::shared_ptr<MenuItem>& item) {
    if (!itemsTable_ || row == 0 || !item) {
        return;
    }
    
    // Add special styling based on item properties
    // Since MenuItem interface doesn't have isPopular() or isSpecial() methods,
    // we'll use category-based styling instead
    
    for (int col = 0; col < itemsTable_->columnCount(); ++col) {
        auto cell = itemsTable_->elementAt(row, col);
        
        // Style based on category
        switch (item->getCategory()) {
            case MenuItem::SPECIAL:
                cell->addStyleClass("menu-item-special");
                break;
            case MenuItem::APPETIZER:
                cell->addStyleClass("menu-item-appetizer");
                break;
            case MenuItem::MAIN_COURSE:
                cell->addStyleClass("menu-item-main-course");
                break;
            case MenuItem::DESSERT:
                cell->addStyleClass("menu-item-dessert");
                break;
            case MenuItem::BEVERAGE:
                cell->addStyleClass("menu-item-beverage");
                break;
        }
        
        // Mark unavailable items
        if (!item->isAvailable()) {
            cell->addStyleClass("menu-item-unavailable");
        }
    }
}
