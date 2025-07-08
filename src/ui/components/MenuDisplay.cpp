//============================================================================
// Fixed MenuDisplay.cpp - Correct Wt Pointer Management
//============================================================================

#include "../../../include/ui/components/MenuDisplay.hpp"

#include <Wt/WApplication.h>
#include <Wt/WMessageBox.h>
#include <Wt/WDialog.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WBreak.h>

#include <iostream>

MenuDisplay::MenuDisplay(std::shared_ptr<POSService> posService,
                        std::shared_ptr<EventManager> eventManager,
                        std::shared_ptr<ThemeService> themeService)
    : posService_(posService)
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
    std::cout << "[MenuDisplay] Initializing..." << std::endl;
    
    // Apply container styling
    UIStyleHelper::styleMenuDisplay(this);
    
    initializeUI();
    loadMenuItems();
    setupEventListeners();
    
    std::cout << "[MenuDisplay] ✓ Initialized with " << menuItems_.size() << " menu items" << std::endl;
}

void MenuDisplay::initializeUI() {
    setId("menu-display");
    
    // Create main layout
    auto layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    
    // Create header
    auto headerWidget = createMenuHeader();
    layout->addWidget(std::move(headerWidget));
    
    // Create menu items table
    itemsTable_ = layout->addWidget(std::make_unique<Wt::WTable>());
    UIStyleHelper::styleTable(itemsTable_, "menu");
    itemsTable_->setId("menu-items-table");
    
    initializeTableHeaders();
    updateMenuItemsTable();
}

std::unique_ptr<Wt::WWidget> MenuDisplay::createMenuHeader() {
    // Create the container and store raw pointer for later use
    auto container = std::make_unique<Wt::WContainerWidget>();
    headerContainer_ = container.get();  // Store raw pointer
    
    UIStyleHelper::styleContainer(headerContainer_, "header");
    
    auto layout = headerContainer_->setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(15, 10, 15, 10);
    
    // Category selection
    auto categoryLabel = layout->addWidget(std::make_unique<Wt::WText>("Category: "));
    UIStyleHelper::styleHeading(categoryLabel, 5, "default");
    
    categoryCombo_ = layout->addWidget(std::make_unique<Wt::WComboBox>());
    UIStyleHelper::styleComboBox(categoryCombo_);
    categoryCombo_->activated().connect([this]() { onCategoryChanged(); });
    
    layout->addStretch(1); // Push item count to the right
    
    // Item count display
    itemCountText_ = layout->addWidget(std::make_unique<Wt::WText>());
    UIStyleHelper::styleStatusText(itemCountText_, "info");
    
    populateCategoryCombo();
    updateItemCount();
    
    return std::move(container);  // Return the unique_ptr
}

void MenuDisplay::initializeTableHeaders() {
    if (!itemsTable_) return;
    
    itemsTable_->clear();
    
    // Add headers
    int col = 0;
    auto nameHeader = itemsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Item"));
    auto categoryHeader = itemsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Category"));
    auto priceHeader = itemsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Price"));
    auto actionHeader = itemsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Action"));
    
    // Style headers
    UIStyleHelper::styleHeading(nameHeader, 6, "primary");
    UIStyleHelper::styleHeading(categoryHeader, 6, "primary");
    UIStyleHelper::styleHeading(priceHeader, 6, "primary");
    UIStyleHelper::styleHeading(actionHeader, 6, "primary");
    
    // Set header row style
    itemsTable_->rowAt(0)->setStyleClass("table-header bg-primary text-white");
}

void MenuDisplay::updateMenuItemsTable() {
    if (!itemsTable_) return;
    
    std::cout << "[MenuDisplay] Updating menu items table..." << std::endl;
    
    // Clear existing rows (keep header)
    while (itemsTable_->rowCount() > 1) {
        itemsTable_->removeRow(itemsTable_->rowCount() - 1);
    }
    
    auto filteredItems = getFilteredItems();
    
    for (size_t i = 0; i < filteredItems.size(); ++i) {
        addMenuItemRow(filteredItems[i], i);
    }
    
    updateItemCount();
    std::cout << "[MenuDisplay] ✓ Table updated with " << filteredItems.size() << " items" << std::endl;
}

void MenuDisplay::addMenuItemRow(const std::shared_ptr<MenuItem>& item, size_t index) {
    if (!itemsTable_ || !item) return;
    
    int row = itemsTable_->rowCount();
    int col = 0;
    
    // Item name
    auto nameText = itemsTable_->elementAt(row, col++)->addWidget(
        std::make_unique<Wt::WText>(item->getName()));
    nameText->setStyleClass("fw-bold");
    
    // Category
    auto categoryText = itemsTable_->elementAt(row, col++)->addWidget(
        std::make_unique<Wt::WText>(MenuItem::categoryToString(item->getCategory())));
    UIStyleHelper::styleBadge(categoryText, "secondary");
    
    // Price
    auto priceText = itemsTable_->elementAt(row, col++)->addWidget(
        std::make_unique<Wt::WText>(formatCurrency(item->getPrice())));
    priceText->setStyleClass("text-success fw-bold");
    
    // Action button
    auto addButton = itemsTable_->elementAt(row, col++)->addWidget(
        std::make_unique<Wt::WPushButton>("Add to Order"));
    
    if (selectionEnabled_ && canAddToOrder()) {
        UIStyleHelper::styleButton(addButton, "primary", "sm");
        addButton->clicked().connect([this, item]() {
            onItemRowClicked(item);
        });
    } else {
        UIStyleHelper::styleButton(addButton, "secondary", "sm");
        addButton->setEnabled(false);
        if (!canAddToOrder()) {
            addButton->setToolTip("No current order - create a new order first");
        }
    }
    
    // Apply row styling
    updateRowStyling(row, (row % 2) == 0);
}

void MenuDisplay::showAddToOrderDialog(const std::shared_ptr<MenuItem>& item) {
    std::cout << "[MenuDisplay] Showing add to order dialog for: " << item->getName() << std::endl;
    
    // Create modal dialog - Wt manages this widget
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Add Item to Order"));
    dialog->setModal(true);
    dialog->setResizable(false);
    dialog->resize(400, 300);
    
    // Style the dialog
    dialog->addStyleClass("pos-dialog");
    
    // Create dialog content container
    auto content = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto layout = content->setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);
    
    // Item information header
    auto itemHeader = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto headerLayout = itemHeader->setLayout(std::make_unique<Wt::WVBoxLayout>());
    
    auto itemName = headerLayout->addWidget(std::make_unique<Wt::WText>(item->getName()));
    UIStyleHelper::styleHeading(itemName, 4, "primary");
    
    auto itemPrice = headerLayout->addWidget(std::make_unique<Wt::WText>(
        "Price: " + formatCurrency(item->getPrice())));
    UIStyleHelper::styleStatusText(itemPrice, "success");
    
    auto itemCategory = headerLayout->addWidget(std::make_unique<Wt::WText>(
        "Category: " + MenuItem::categoryToString(item->getCategory())));
    itemCategory->setStyleClass("text-muted");
    
    // Separator
    layout->addWidget(std::make_unique<Wt::WBreak>());
    
    // Quantity selection
    auto quantityContainer = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto qtyLayout = quantityContainer->setLayout(std::make_unique<Wt::WHBoxLayout>());
    
    auto qtyLabel = qtyLayout->addWidget(std::make_unique<Wt::WLabel>("Quantity:"));
    qtyLabel->setStyleClass("form-label fw-bold");
    
    auto quantitySpinBox = qtyLayout->addWidget(std::make_unique<Wt::WSpinBox>());
    quantitySpinBox->setRange(1, MAX_QUANTITY);
    quantitySpinBox->setValue(1);
    UIStyleHelper::styleFormControl(quantitySpinBox);
    quantitySpinBox->setWidth(80);
    
    // Special instructions
    auto instructionsContainer = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto instrLayout = instructionsContainer->setLayout(std::make_unique<Wt::WVBoxLayout>());
    
    auto instrLabel = instrLayout->addWidget(std::make_unique<Wt::WLabel>("Special Instructions:"));
    instrLabel->setStyleClass("form-label fw-bold");
    
    auto instructionsEdit = instrLayout->addWidget(std::make_unique<Wt::WLineEdit>());
    instructionsEdit->setPlaceholderText("Optional: cooking preferences, allergies, etc.");
    UIStyleHelper::styleFormControl(instructionsEdit);
    
    // Total price display
    auto totalContainer = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    totalContainer->setStyleClass("bg-light p-3 rounded border");
    
    auto totalText = totalContainer->addWidget(std::make_unique<Wt::WText>(
        "Total: " + formatCurrency(item->getPrice())));
    UIStyleHelper::styleHeading(totalText, 5, "success");
    
    // Update total when quantity changes
    quantitySpinBox->valueChanged().connect([=]() {
        double total = item->getPrice() * quantitySpinBox->value();
        totalText->setText("Total: " + formatCurrency(total));
    });
    
    // Action buttons
    auto buttonContainer = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto buttonLayout = buttonContainer->setLayout(std::make_unique<Wt::WHBoxLayout>());
    buttonLayout->addStretch(1); // Push buttons to the right
    
    auto cancelButton = buttonLayout->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    UIStyleHelper::styleButton(cancelButton, "outline-secondary");
    
    auto addButton = buttonLayout->addWidget(std::make_unique<Wt::WPushButton>("Add to Order"));
    UIStyleHelper::styleButton(addButton, "success");
    
    // Button event handlers
    cancelButton->clicked().connect([=]() {
        std::cout << "[MenuDisplay] Add to order canceled" << std::endl;
        dialog->reject();
    });
    
    addButton->clicked().connect([=]() {
        int quantity = quantitySpinBox->value();
        std::string instructions = instructionsEdit->text().toUTF8();
        
        std::cout << "[MenuDisplay] Adding " << quantity << "x " << item->getName() 
                  << " with instructions: '" << instructions << "'" << std::endl;
        
        onAddToOrderClicked(item, quantity, instructions);
        dialog->accept();
    });
    
    // Handle Enter key to add item
    instructionsEdit->enterPressed().connect([=]() {
        addButton->clicked().emit(Wt::WMouseEvent());
    });
    
    quantitySpinBox->enterPressed().connect([=]() {
        addButton->clicked().emit(Wt::WMouseEvent());
    });
    
    // Focus on quantity input
    quantitySpinBox->setFocus(true);
    
    // Show the dialog
    dialog->show();
    
    std::cout << "[MenuDisplay] ✓ Add to order dialog displayed" << std::endl;
}

void MenuDisplay::onItemRowClicked(const std::shared_ptr<MenuItem>& item) {
    std::cout << "[MenuDisplay] Item clicked: " << item->getName() << std::endl;
    
    if (!canAddToOrder()) {
        std::cout << "[MenuDisplay] canAddToOrder: 0" << std::endl;
        showMessage("Please create a new order first", "warning");
        return;
    }
    
    std::cout << "[MenuDisplay] canAddToOrder: 1" << std::endl;
    
    // Show the add to order dialog
    showAddToOrderDialog(item);
}

void MenuDisplay::onAddToOrderClicked(const std::shared_ptr<MenuItem>& item, int quantity, const std::string& instructions) {
    try {
        addItemToCurrentOrder(*item, quantity, instructions);
        
        std::string message = "Added " + std::to_string(quantity) + "x " + item->getName() + " to current order";
        std::cout << "[MenuDisplay] success: " << message << std::endl;
        
        showMessage(message, "success");
        
    } catch (const std::exception& e) {
        std::string error = "Failed to add item: " + std::string(e.what());
        std::cout << "[MenuDisplay] error: " << error << std::endl;
        showMessage(error, "error");
    }
}

void MenuDisplay::addItemToCurrentOrder(const MenuItem& item, int quantity, const std::string& instructions) {
    if (!posService_) {
        throw std::runtime_error("POS service not available");
    }
    
    bool success = posService_->addItemToCurrentOrder(item, quantity, instructions);
    if (!success) {
        throw std::runtime_error("Failed to add item to order");
    }
}

bool MenuDisplay::canAddToOrder() const {
    return posService_ && posService_->getCurrentOrder() != nullptr;
}

void MenuDisplay::showMessage(const std::string& message, const std::string& type) {
    if (!eventManager_) return;
    
    auto notificationData = POSEvents::createNotificationData(message, type, 3000);
    eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED, notificationData);
}

// Category management
void MenuDisplay::onCategoryChanged() {
    if (!categoryCombo_) return;
    
    int index = categoryCombo_->currentIndex();
    if (index == 0) {
        currentCategory_ = ""; // All categories
    } else {
        currentCategory_ = categories_[index - 1]; // Adjust for "All Categories" option
    }
    
    std::cout << "[MenuDisplay] Category changed to: " << 
                 (currentCategory_.empty() ? "All" : currentCategory_) << std::endl;
    
    updateMenuItemsTable();
}

void MenuDisplay::populateCategoryCombo() {
    if (!categoryCombo_) return;
    
    categoryCombo_->clear();
    categoryCombo_->addItem(ALL_CATEGORIES);
    
    for (const auto& category : categories_) {
        categoryCombo_->addItem(category);
    }
    
    categoryCombo_->setCurrentIndex(0); // Select "All Categories"
}

std::vector<std::shared_ptr<MenuItem>> MenuDisplay::getFilteredItems() const {
    if (currentCategory_.empty() || currentCategory_ == ALL_CATEGORIES) {
        return menuItems_;
    }
    
    std::vector<std::shared_ptr<MenuItem>> filtered;
    for (const auto& item : menuItems_) {
        if (MenuItem::categoryToString(item->getCategory()) == currentCategory_) {
            filtered.push_back(item);
        }
    }
    
    return filtered;
}

void MenuDisplay::loadMenuItems() {
    if (!posService_) {
        std::cout << "[MenuDisplay] Warning: No POS service available" << std::endl;
        return;
    }
    
    menuItems_ = posService_->getMenuItems();
    organizeItemsByCategory();
    
    std::cout << "[MenuDisplay] Loaded " << menuItems_.size() << " menu items" << std::endl;
}

void MenuDisplay::organizeItemsByCategory() {
    categories_.clear();
    itemsByCategory_.clear();
    
    for (const auto& item : menuItems_) {
        std::string categoryName = MenuItem::categoryToString(item->getCategory());
        
        // Add category if not already present
        if (std::find(categories_.begin(), categories_.end(), categoryName) == categories_.end()) {
            categories_.push_back(categoryName);
        }
        
        itemsByCategory_[categoryName].push_back(item);
    }
    
    // Sort categories for consistent display
    std::sort(categories_.begin(), categories_.end());
}

// Utility methods
std::string MenuDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "$" << amount;
    return oss.str();
}

void MenuDisplay::updateItemCount() {
    if (!itemCountText_) return;
    
    auto filteredItems = getFilteredItems();
    std::string countText = std::to_string(filteredItems.size()) + " items";
    if (!currentCategory_.empty() && currentCategory_ != ALL_CATEGORIES) {
        countText += " in " + currentCategory_;
    }
    
    itemCountText_->setText(countText);
}

void MenuDisplay::updateRowStyling(int row, bool isEven) {
    if (!itemsTable_ || row >= itemsTable_->rowCount()) return;
    
    auto rowWidget = itemsTable_->rowAt(row);
    if (rowWidget) {
        if (isEven) {
            rowWidget->addStyleClass("table-row-even");
        } else {
            rowWidget->addStyleClass("table-row-odd");
        }
        rowWidget->addStyleClass("pos-table-row");
    }
}

// Event listeners
void MenuDisplay::setupEventListeners() {
    if (!eventManager_) return;
    
    // Listen for menu updates
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::MENU_UPDATED, [this](const std::any& data) {
            handleMenuUpdated(data);
        })
    );
    
    // Listen for current order changes
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED, [this](const std::any& data) {
            handleCurrentOrderChanged(data);
        })
    );
    
    // Listen for theme changes if theme service is available
    if (themeService_) {
        eventSubscriptions_.push_back(
            eventManager_->subscribe(POSEvents::THEME_CHANGED, [this](const std::any& data) {
                handleThemeChanged(data);
            })
        );
    }
}

void MenuDisplay::handleMenuUpdated(const std::any& eventData) {
    std::cout << "[MenuDisplay] Menu updated event received, refreshing..." << std::endl;
    refresh();
}

void MenuDisplay::handleCurrentOrderChanged(const std::any& eventData) {
    std::cout << "[MenuDisplay] Current order changed, updating button states..." << std::endl;
    updateMenuItemsTable(); // Refresh to update button states
}

void MenuDisplay::handleThemeChanged(const std::any& eventData) {
    std::cout << "[MenuDisplay] Theme changed, applying new styling..." << std::endl;
    applyTheme();
}

void MenuDisplay::applyTheme() {
    if (themeService_) {
        // Apply current theme styling
        // Implementation depends on your theme system
    }
}

// Public interface methods
void MenuDisplay::refresh() {
    loadMenuItems();
    populateCategoryCombo();
    updateMenuItemsTable();
}

void MenuDisplay::setCategory(const std::string& category) {
    currentCategory_ = category;
    
    if (categoryCombo_) {
        if (category.empty()) {
            categoryCombo_->setCurrentIndex(0); // All Categories
        } else {
            for (int i = 0; i < categoryCombo_->count(); ++i) {
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

// Validation methods
bool MenuDisplay::validateQuantity(int quantity) const {
    return quantity >= 1 && quantity <= MAX_QUANTITY;
}

bool MenuDisplay::validateSpecialInstructions(const std::string& instructions) const {
    return instructions.length() <= MAX_INSTRUCTIONS_LENGTH;
}

// Utility method implementation that was missing
std::string MenuDisplay::formatItemDescription(const std::shared_ptr<MenuItem>& item) const {
    return item->getName() + " - " + MenuItem::categoryToString(item->getCategory());
}
