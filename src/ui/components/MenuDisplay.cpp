#include "../../../include/ui/components/MenuDisplay.hpp"

#include <Wt/WTableCell.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGridLayout.h>
#include <Wt/WBorderLayout.h>
#include <Wt/WIcon.h>
#include <Wt/WDialog.h>
#include <Wt/WApplication.h>

#include <iostream>
#include <iomanip>
#include <sstream>

MenuDisplay::MenuDisplay(std::shared_ptr<POSService> posService,
                        std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , displayMode_(TABLE_VIEW)
    , categoryFilter_(MenuItem::SPECIAL)
    , hasFilter_(false)
    , menuContainer_(nullptr)
    , menuTable_(nullptr)
    , categoryTilesContainer_(nullptr) {
    
    initializeUI();
    setupEventListeners();
    refresh();
}

void MenuDisplay::initializeUI() {
    addStyleClass("menu-display");
    
    // Create main container for menu content
    menuContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    menuContainer_->addStyleClass("menu-container");
    
    std::cout << "✓ MenuDisplay UI initialized" << std::endl;
}

void MenuDisplay::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "Warning: EventManager not available for MenuDisplay" << std::endl;
        return;
    }
    
    // Subscribe to menu update events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CONFIGURATION_CHANGED,
            [this](const std::any& data) { handleMenuUpdated(data); }));
    
    // Subscribe to theme change events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::THEME_CHANGED,
            [this](const std::any& data) { handleThemeChanged(data); }));
    
    std::cout << "✓ MenuDisplay event listeners setup complete" << std::endl;
}

void MenuDisplay::refresh() {
    if (!posService_) {
        std::cerr << "Error: POSService not available for menu refresh" << std::endl;
        return;
    }
    
    clearMenuContainer();
    
    if (displayMode_ == TABLE_VIEW) {
        buildTableView();
    } else {
        buildCategoryTilesView();
    }
    
    std::cout << "Menu display refreshed in " 
              << (displayMode_ == TABLE_VIEW ? "table" : "tiles") 
              << " mode" << std::endl;
}

void MenuDisplay::setDisplayMode(DisplayMode mode) {
    if (displayMode_ != mode) {
        displayMode_ = mode;
        refresh();
        std::cout << "Menu display mode changed to " 
                  << (mode == TABLE_VIEW ? "table view" : "category tiles") << std::endl;
    }
}

MenuDisplay::DisplayMode MenuDisplay::getDisplayMode() const {
    return displayMode_;
}

void MenuDisplay::filterByCategory(MenuItem::Category category) {
    categoryFilter_ = category;
    hasFilter_ = true;
    
    if (displayMode_ == TABLE_VIEW) {
        updateMenuTable();
    }
    
    std::cout << "Menu filtered by category: " << getCategoryDisplayName(category) << std::endl;
}

void MenuDisplay::clearCategoryFilter() {
    hasFilter_ = false;
    
    if (displayMode_ == TABLE_VIEW) {
        updateMenuTable();
    }
    
    std::cout << "Menu category filter cleared" << std::endl;
}

void MenuDisplay::buildTableView() {
    auto tableContainer = std::make_unique<Wt::WContainerWidget>();
    tableContainer->addStyleClass("menu-table-container");
    
    // Create table
    menuTable_ = tableContainer->addWidget(std::make_unique<Wt::WTable>());
    menuTable_->addStyleClass("table table-striped table-hover menu-table");
    menuTable_->setHeaderCount(1);
    
    // Set up table headers
    menuTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    menuTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Description"));
    menuTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Price"));
    menuTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Category"));
    menuTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Action"));
    
    // Style headers
    for (int col = 0; col < 5; ++col) {
        menuTable_->elementAt(0, col)->addStyleClass("table-header");
    }
    
    updateMenuTable();
    
    menuContainer_->addWidget(std::move(tableContainer));
    std::cout << "✓ Table view built" << std::endl;
}

// Method 1: updateMenuTable() - Fix removeRow call
void MenuDisplay::updateMenuTable() {
    if (!menuTable_) return;
    
    // Clear existing rows (keep header)
    while (menuTable_->rowCount() > 1) {
        menuTable_->removeRow(1); // FIXED: was deleteRow(1)
    }
    
    // Get filtered menu items
    auto items = getFilteredMenuItems();
    
    if (items.empty()) {
        // Show "no items" message
        auto noItemsText = std::make_unique<Wt::WText>("No menu items found");
        noItemsText->addStyleClass("text-muted text-center");
        menuTable_->elementAt(1, 0)->addWidget(std::move(noItemsText));
        menuTable_->elementAt(1, 0)->setColumnSpan(6);
        return;
    }
    
    // Add menu items to table
    int row = 1;
    for (const auto& item : items) {
        if (hasFilter_ && item->getCategory() != categoryFilter_) {
            continue;
        }
        
        addMenuItemToTable(item, row); // This method is now declared in header
        row++;
    }
}

// Method 2: addMenuItemToTable() - Complete corrected implementation
void MenuDisplay::addMenuItemToTable(std::shared_ptr<MenuItem> item, int row) {
    if (!menuTable_ || !item) return;
    
    // Item name
    auto nameText = std::make_unique<Wt::WText>(item->getName());
    nameText->addStyleClass("fw-bold");
    menuTable_->elementAt(row, 0)->addWidget(std::move(nameText));
    
    // Category (instead of description since MenuItem has no description)
    auto categoryText = std::make_unique<Wt::WText>(getCategoryDisplayName(item->getCategory()));
    categoryText->addStyleClass("text-muted small");
    menuTable_->elementAt(row, 1)->addWidget(std::move(categoryText));
    
    // Price
    auto priceText = std::make_unique<Wt::WText>(formatCurrency(item->getPrice()));
    priceText->addStyleClass("fw-bold text-success");
    menuTable_->elementAt(row, 2)->addWidget(std::move(priceText));
    
    // Availability
    auto availText = std::make_unique<Wt::WText>(item->isAvailable() ? "Available" : "Unavailable");
    availText->addStyleClass(item->isAvailable() ? "badge bg-success" : "badge bg-danger");
    menuTable_->elementAt(row, 3)->addWidget(std::move(availText));
    
    // Add button
    auto addBtn = std::make_unique<Wt::WPushButton>("Add to Order");
    addBtn->addStyleClass("btn btn-primary btn-sm");
    addBtn->setEnabled(item->isAvailable());
    
    if (!item->isAvailable()) {
        addBtn->addStyleClass("disabled");
    }
    
    addBtn->clicked().connect([this, item]() {
        onMenuItemSelected(item);
    });
    
    menuTable_->elementAt(row, 4)->addWidget(std::move(addBtn));
}

void MenuDisplay::buildCategoryTilesView() {
    auto tilesContainer = std::make_unique<Wt::WContainerWidget>();
    tilesContainer->addStyleClass("category-tiles-container");
    
    // Create grid layout for tiles
    auto gridLayout = std::make_unique<Wt::WGridLayout>();
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);
    
    auto categorizedItems = groupMenuItemsByCategory();
    
    int row = 0, col = 0;
    for (const auto& categoryPair : categorizedItems) {
        auto category = categoryPair.first;
        const auto& items = categoryPair.second;
        
        if (items.empty()) continue;
        
        auto tile = createCategoryTile(category, items.size());
        gridLayout->addWidget(std::move(tile), row, col);
        
        col++;
        if (col >= 3) {
            col = 0;
            row++;
        }
    }
    
    tilesContainer->setLayout(std::move(gridLayout));
    categoryTilesContainer_ = tilesContainer.get();
    menuContainer_->addWidget(std::move(tilesContainer));
    
    std::cout << "✓ Category tiles view built with " << categorizedItems.size() << " categories" << std::endl;
}

std::unique_ptr<Wt::WWidget> MenuDisplay::createCategoryTile(MenuItem::Category category, int itemCount) {
    auto tile = std::make_unique<Wt::WContainerWidget>();
    tile->addStyleClass("category-tile card h-100");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Category icon and header
    auto headerContainer = std::make_unique<Wt::WContainerWidget>();
    headerContainer->addStyleClass("category-tile-header text-center p-3");
    
    auto icon = std::make_unique<Wt::WText>(getCategoryIcon(category));
    icon->addStyleClass("category-icon h1");
    headerContainer->addWidget(std::move(icon));
    
    auto title = std::make_unique<Wt::WText>(getCategoryDisplayName(category));
    title->addStyleClass("category-title h4 mt-2");
    headerContainer->addWidget(std::move(title));
    
    layout->addWidget(std::move(headerContainer));
    
    // Item count
    auto countContainer = std::make_unique<Wt::WContainerWidget>();
    countContainer->addStyleClass("category-tile-body text-center p-2");
    
    auto countText = std::make_unique<Wt::WText>(std::to_string(itemCount) + " items");
    countText->addStyleClass("category-count text-muted");
    countContainer->addWidget(std::move(countText));
    
    layout->addWidget(std::move(countContainer));
    
    // Footer with action
    auto footerContainer = std::make_unique<Wt::WContainerWidget>();
    footerContainer->addStyleClass("category-tile-footer p-2");
    
    auto viewButton = std::make_unique<Wt::WPushButton>("View Items");
    viewButton->addStyleClass("btn btn-outline-primary btn-block");
    viewButton->clicked().connect([this, category]() {
        onCategoryTileClicked(category);
    });
    footerContainer->addWidget(std::move(viewButton));
    
    layout->addWidget(std::move(footerContainer));
    
    tile->setLayout(std::move(layout));
    
    // Add hover effects
    tile->mouseWentOver().connect([tile = tile.get()]() {
        tile->addStyleClass("category-tile-hover");
    });
    
    tile->mouseWentOut().connect([tile = tile.get()]() {
        tile->removeStyleClass("category-tile-hover");
    });
    
    return std::move(tile);
}

// Method 3: showCategoryPopover() - Fix getDescription() call
void MenuDisplay::showCategoryPopover(MenuItem::Category category, 
                                     const std::vector<std::shared_ptr<MenuItem>>& items) {
    if (items.empty()) return;
    
    // Create popover container
    auto popoverContainer = std::make_unique<Wt::WContainerWidget>();
    popoverContainer->addStyleClass("menu-category-popover");
    
    // Add title
    auto titleText = std::make_unique<Wt::WText>(getCategoryDisplayName(category));
    titleText->addStyleClass("h5 mb-3");
    popoverContainer->addWidget(std::move(titleText));
    
    // Add items
    for (const auto& item : items) {
        auto itemContainer = std::make_unique<Wt::WContainerWidget>();
        itemContainer->addStyleClass("menu-item-card mb-2 p-2 border rounded");
        
        // Item name
        auto nameText = std::make_unique<Wt::WText>(item->getName());
        nameText->addStyleClass("fw-bold");
        itemContainer->addWidget(std::move(nameText));
        
        // FIXED: Instead of getDescription(), show price and availability
        auto detailsText = std::make_unique<Wt::WText>(
            formatCurrency(item->getPrice()) + 
            (item->isAvailable() ? " - Available" : " - Unavailable")
        );
        detailsText->addStyleClass("text-muted small");
        itemContainer->addWidget(std::move(detailsText));
        
        // Add button
        auto addBtn = std::make_unique<Wt::WPushButton>("Add");
        addBtn->addStyleClass("btn btn-primary btn-sm mt-1");
        addBtn->setEnabled(item->isAvailable());
        
        addBtn->clicked().connect([this, item]() {
            onMenuItemSelected(item);
        });
        
        itemContainer->addWidget(std::move(addBtn));
        popoverContainer->addWidget(std::move(itemContainer));
    }
    
    // Show the popover (implementation depends on your popover system)
    menuContainer_->addWidget(std::move(popoverContainer));
}

// =================================================================
// Event Handlers
// =================================================================

void MenuDisplay::handleMenuUpdated(const std::any& eventData) {
    std::cout << "Menu update event received, refreshing display" << std::endl;
    refresh();
}

void MenuDisplay::handleThemeChanged(const std::any& eventData) {
    std::cout << "Theme change event received, updating menu display styling" << std::endl;
    // Theme changes are handled automatically by CSS, but we could add
    // specific styling updates here if needed
}

void MenuDisplay::onMenuItemSelected(std::shared_ptr<MenuItem> item) {
    if (!posService_) {
        std::cerr << "Error: POSService not available for item selection" << std::endl;
        return;
    }
    
    bool success = posService_->addItemToCurrentOrder(item);
    
    if (success) {
        std::cout << "Added " << item->getName() << " to current order" << std::endl;
    } else {
        std::cerr << "Failed to add " << item->getName() << " to current order" << std::endl;
    }
}

void MenuDisplay::onCategoryTileClicked(MenuItem::Category category) {
    auto items = posService_->getMenuItemsByCategory(category);
    showCategoryPopover(category, items);
}

// =================================================================
// Helper Methods
// =================================================================

std::vector<std::shared_ptr<MenuItem>> MenuDisplay::getFilteredMenuItems() const {
    if (!posService_) {
        return {};
    }
    
    if (hasFilter_) {
        return posService_->getMenuItemsByCategory(categoryFilter_);
    } else {
        return posService_->getMenuItems();
    }
}

std::map<MenuItem::Category, std::vector<std::shared_ptr<MenuItem>>> 
MenuDisplay::groupMenuItemsByCategory() const {
    std::map<MenuItem::Category, std::vector<std::shared_ptr<MenuItem>>> categorizedItems;
    
    auto allItems = posService_->getMenuItems();
    
    for (const auto& item : allItems) {
        categorizedItems[item->getCategory()].push_back(item);
    }
    
    return categorizedItems;
}

std::string MenuDisplay::getCategoryIcon(MenuItem::Category category) const {
    switch (category) {
        case MenuItem::APPETIZER:  return "🥗";
        case MenuItem::MAIN_COURSE: return "🍽️";
        case MenuItem::DESSERT:    return "🍰";
        case MenuItem::BEVERAGE:   return "🥤";
        case MenuItem::SPECIAL:    return "⭐";
        default:                   return "🍴";
    }
}

// Helper method: getCategoryDisplayName() - if not already implemented
std::string MenuDisplay::getCategoryDisplayName(MenuItem::Category category) const {
    switch (category) {
        case MenuItem::APPETIZER:   return "Appetizer";
        case MenuItem::MAIN_COURSE: return "Main Course";
        case MenuItem::DESSERT:     return "Dessert";
        case MenuItem::BEVERAGE:    return "Beverage";
        case MenuItem::SPECIAL:     return "Special";
        default:                    return "Unknown";
    }
}

// Helper method: formatCurrency() - if not already implemented
std::string MenuDisplay::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "$" << amount;
    return oss.str();
}

// Method 4: formatItemDescription() - Complete corrected implementation
std::string MenuDisplay::formatItemDescription(const std::shared_ptr<MenuItem>& item) const {
    if (!item) return "";
    
    // FIXED: Create description from available MenuItem fields
    std::string description = item->getName();
    description += " - " + getCategoryDisplayName(item->getCategory());
    description += " - " + formatCurrency(item->getPrice());
    
    if (!item->isAvailable()) {
        description += " (Currently Unavailable)";
    }
    
    return description;
}

void MenuDisplay::clearMenuContainer() {
    if (menuContainer_) {
        menuContainer_->clear();
        menuTable_ = nullptr;
        categoryTilesContainer_ = nullptr;
    }
}
