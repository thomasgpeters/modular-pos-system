#include "../../../include/ui/components/MenuDisplay.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WSpinBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
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
    , currentCategory_("")
    , selectionEnabled_(true)
    , menuGroup_(nullptr)
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
    loadMenuItems(); // CRITICAL: Load menu items from service
    refresh();
    
    std::cout << "[MenuDisplay] Initialized with " << menuItems_.size() << " menu items" << std::endl;
}

void MenuDisplay::initializeUI() {
    // Create main group container
    menuGroup_ = addNew<Wt::WGroupBox>("🍽️ Restaurant Menu");
    menuGroup_->setStyleClass("border border-primary rounded");
    
    // Create header with category filter - FIXED: Use addWidget instead of unique_ptr
    auto header = createMenuHeader();
    menuGroup_->addWidget(std::move(header));
    
    // Create table container
    auto tableContainer = menuGroup_->addNew<Wt::WContainerWidget>();
    tableContainer->setStyleClass("mt-3");
    
    // Create menu items table
    itemsTable_ = tableContainer->addNew<Wt::WTable>();
    itemsTable_->setStyleClass("table table-striped table-hover w-100");
    
    initializeTableHeaders();
    
    std::cout << "[MenuDisplay] UI initialized" << std::endl;
}

void MenuDisplay::initializeTableHeaders() {
    if (!itemsTable_) return;
    
    // Create table headers
    itemsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    itemsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Price"));
    itemsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Category"));
    itemsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Status"));
    itemsTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));
    
    // Style headers
    for (int col = 0; col < 5; ++col) {
        auto headerCell = itemsTable_->elementAt(0, col);
        headerCell->setStyleClass("bg-primary text-white fw-bold text-center p-2");
    }
}

// FIXED: Correct pointer management for createMenuHeader
std::unique_ptr<Wt::WWidget> MenuDisplay::createMenuHeader() {
    // Create header container as local unique_ptr
    auto headerContainer = std::make_unique<Wt::WContainerWidget>();
    headerContainer->setStyleClass("d-flex justify-content-between align-items-center mb-3");
    
    // Store raw pointer to the container for later reference (FIXED: assign to member)
    headerContainer_ = headerContainer.get();
    
    // Category filter - addNew returns raw pointer, which is correct
    categoryCombo_ = headerContainer->addNew<Wt::WComboBox>();
    categoryCombo_->setStyleClass("form-select me-3");
    populateCategoryCombo();
    
    // Connect category change handler
    categoryCombo_->changed().connect([this]() {
        onCategoryChanged();
    });
    
    // Item count display - addNew returns raw pointer, which is correct
    itemCountText_ = headerContainer->addNew<Wt::WText>("0 items");
    itemCountText_->setStyleClass("badge bg-info");
    
    // Return the unique_ptr (this transfers ownership to the caller)
    return std::move(headerContainer);
}

void MenuDisplay::loadMenuItems() {
    // CRITICAL: Actually load items from the service
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
        
        // Add to categories list if not already present
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
        emptyRow->setColumnSpan(5);
        emptyRow->addWidget(std::make_unique<Wt::WText>("No items in this category"));
        emptyRow->setStyleClass("text-center text-muted p-4");
        return;
    }
    
    // Add menu item rows
    for (size_t i = 0; i < filteredItems.size(); ++i) {
        addMenuItemRow(filteredItems[i], i);
    }
    
    updateItemCount();
}

void MenuDisplay::addMenuItemRow(const std::shared_ptr<MenuItem>& item, size_t index) {
    if (!item || !itemsTable_) return;
    
    int row = static_cast<int>(index + 1); // +1 for header row
    
    try {
        // Item name
        auto nameText = std::make_unique<Wt::WText>(item->getName());
        nameText->setStyleClass("fw-bold text-dark");
        itemsTable_->elementAt(row, 0)->addWidget(std::move(nameText));
        
        // Price
        auto priceText = std::make_unique<Wt::WText>(formatCurrency(item->getPrice()));
        priceText->setStyleClass("fw-bold text-success");
        itemsTable_->elementAt(row, 1)->addWidget(std::move(priceText));
        
        // Category
        auto categoryText = std::make_unique<Wt::WText>(MenuItem::categoryToString(item->getCategory()));
        categoryText->setStyleClass("badge bg-secondary");
        itemsTable_->elementAt(row, 2)->addWidget(std::move(categoryText));
        
        // Status
        auto statusText = std::make_unique<Wt::WText>(item->isAvailable() ? "Available" : "Unavailable");
        statusText->setStyleClass(item->isAvailable() ? "badge bg-success" : "badge bg-danger");
        itemsTable_->elementAt(row, 3)->addWidget(std::move(statusText));
        
        // Actions
        auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
        actionsContainer->setStyleClass("d-flex gap-1");
        
        if (selectionEnabled_ && item->isAvailable() && canAddToOrder()) {
            // Quick add button (quantity 1)
            auto quickAddBtn = actionsContainer->addNew<Wt::WPushButton>("+ Add");
            quickAddBtn->setStyleClass("btn btn-success btn-sm");
            quickAddBtn->clicked().connect([this, item]() {
                std::cout << "[MenuDisplay] Quick add clicked for: " << item->getName() << std::endl;
                addItemToCurrentOrder(*item, 1, "");
            });
            
            // Custom add button (with dialog)
            auto customAddBtn = actionsContainer->addNew<Wt::WPushButton>("⚙️ Custom");
            customAddBtn->setStyleClass("btn btn-outline-primary btn-sm");
            customAddBtn->clicked().connect([this, item]() {
                std::cout << "[MenuDisplay] Custom add clicked for: " << item->getName() << std::endl;
                showAddToOrderDialog(item);
            });
        } else {
            auto disabledBtn = actionsContainer->addNew<Wt::WPushButton>("Unavailable");
            disabledBtn->setStyleClass("btn btn-outline-secondary btn-sm");
            disabledBtn->setEnabled(false);
        }
        
        itemsTable_->elementAt(row, 4)->addWidget(std::move(actionsContainer));
        
        // Apply row styling
        for (int col = 0; col < 5; ++col) {
            auto cell = itemsTable_->elementAt(row, col);
            cell->setStyleClass("p-2 align-middle");
            if (col == 1 || col == 2 || col == 3) { // Price, Category, Status
                cell->addStyleClass("text-center");
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[MenuDisplay] Error adding menu item row: " << e.what() << std::endl;
    }
}

// CRITICAL: The actual add to order functionality
void MenuDisplay::addItemToCurrentOrder(const MenuItem& item, int quantity, const std::string& instructions) {
    std::cout << "[MenuDisplay] Adding to order: " << item.getName() 
              << " (qty: " << quantity << ", instructions: '" << instructions << "')" << std::endl;
    
    if (!posService_) {
        showMessage("Service not available", "error");
        return;
    }
    
    if (!canAddToOrder()) {
        showMessage("No current order. Please start a new order first.", "warning");
        return;
    }
    
    // ACTUALLY ADD THE ITEM TO THE ORDER
    bool success = posService_->addItemToCurrentOrder(item, quantity, instructions);
    
    if (success) {
        showMessage(
            "Added " + std::to_string(quantity) + "x " + item.getName() + " to order", 
            "success"
        );
        std::cout << "[MenuDisplay] Successfully added item to current order" << std::endl;
    } else {
        showMessage("Failed to add item to order", "error");
        std::cout << "[MenuDisplay] Failed to add item to current order" << std::endl;
    }
}

void MenuDisplay::showAddToOrderDialog(const std::shared_ptr<MenuItem>& item) {
    if (!item) return;
    
    std::cout << "[MenuDisplay] Showing add to order dialog for: " << item->getName() << std::endl;
    
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Add " + item->getName() + " to Order"));
    dialog->setModal(true);
    dialog->resize(400, 300);
    
    auto content = dialog->contents();
    
    // Quantity selection
    content->addNew<Wt::WText>("Quantity:")->setStyleClass("form-label fw-bold");
    auto quantitySpinBox = content->addNew<Wt::WSpinBox>();
    quantitySpinBox->setRange(1, MAX_QUANTITY);
    quantitySpinBox->setValue(1);
    quantitySpinBox->setStyleClass("form-control mb-3");
    
    // Special instructions
    content->addNew<Wt::WText>("Special Instructions:")->setStyleClass("form-label fw-bold");
    auto instructionsEdit = content->addNew<Wt::WLineEdit>();
    instructionsEdit->setPlaceholderText("Any special requests...");
    instructionsEdit->setStyleClass("form-control mb-3");
    
    // Buttons
    auto buttonContainer = content->addNew<Wt::WContainerWidget>();
    buttonContainer->setStyleClass("d-flex gap-2 justify-content-end");
    
    auto addButton = buttonContainer->addNew<Wt::WPushButton>("Add to Order");
    addButton->setStyleClass("btn btn-success");
    
    auto cancelButton = buttonContainer->addNew<Wt::WPushButton>("Cancel");
    cancelButton->setStyleClass("btn btn-secondary");
    
    // ACTUALLY CONNECT THE HANDLERS
    addButton->clicked().connect([this, item, quantitySpinBox, instructionsEdit, dialog]() {
        int quantity = quantitySpinBox->value();
        std::string instructions = instructionsEdit->text().toUTF8();
        
        std::cout << "[MenuDisplay] Dialog add button clicked - qty: " << quantity 
                  << ", instructions: '" << instructions << "'" << std::endl;
        
        addItemToCurrentOrder(*item, quantity, instructions);
        removeChild(dialog);
    });
    
    cancelButton->clicked().connect([this, dialog]() {
        std::cout << "[MenuDisplay] Dialog cancelled" << std::endl;
        removeChild(dialog);
    });
    
    dialog->show();
}

bool MenuDisplay::canAddToOrder() const {
    bool result = posService_ && posService_->getCurrentOrder() != nullptr;
    std::cout << "[MenuDisplay] canAddToOrder: " << result << std::endl;
    return result;
}

void MenuDisplay::showMessage(const std::string& message, const std::string& type) {
    std::cout << "[MenuDisplay] " << type << ": " << message << std::endl;
    
    // Create a temporary message box
    Wt::StandardButton button = Wt::StandardButton::Ok;
    
    if (type == "error") {
        Wt::WMessageBox::show("Error", message, button);
    } else if (type == "warning") {
        Wt::WMessageBox::show("Warning", message, button);
    } else if (type == "success") {
        Wt::WMessageBox::show("Success", message, button);
    } else {
        Wt::WMessageBox::show("Info", message, button);
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
    
    std::cout << "[MenuDisplay] Category changed to: " << currentCategory_ << std::endl;
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
    itemCountText_->setText(countText);
}

std::string MenuDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "$" << amount;
    return oss.str();
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
}

void MenuDisplay::handleMenuUpdated(const std::any& eventData) {
    std::cout << "[MenuDisplay] Menu updated event received" << std::endl;
    loadMenuItems();
    refresh();
}

void MenuDisplay::handleCurrentOrderChanged(const std::any& eventData) {
    std::cout << "[MenuDisplay] Current order changed event received" << std::endl;
    updateMenuItemsTable(); // Refresh to update button states
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

// UNUSED HANDLER PLACEHOLDERS
void MenuDisplay::onAddToOrderClicked(const std::shared_ptr<MenuItem>& item, int quantity, const std::string& instructions) {
    addItemToCurrentOrder(*item, quantity, instructions);
}

void MenuDisplay::onItemRowClicked(const std::shared_ptr<MenuItem>& item) {
    // Could be used for item details view
}
