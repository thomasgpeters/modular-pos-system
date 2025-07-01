//=============================================================================
// CategoryPopover.cpp
//=============================================================================

#include "../../../include/ui/dialogs/CategoryPopover.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WBreak.h>
#include <Wt/WImage.h>
#include <iomanip>
#include <sstream>

CategoryPopover::CategoryPopover(MenuItem::Category category,
                               const std::vector<std::shared_ptr<MenuItem>>& items,
                               std::shared_ptr<EventManager> eventManager,
                               ItemSelectionCallback callback)
    : WPopupWidget(std::make_unique<Wt::WContainerWidget>()),
      category_(category), menuItems_(items), eventManager_(eventManager),
      selectionCallback_(callback), maxColumns_(3), showDescriptions_(true) {
    
    // Get the implementation container
    contentContainer_ = static_cast<Wt::WContainerWidget*>(implementation());
    
    createPopoverContent();
    setupStyling();
}

void CategoryPopover::createPopoverContent() {
    contentContainer_->clear();
    contentContainer_->addStyleClass("category-popover-content");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    
    // Add header
    auto header = createHeader();
    layout->addWidget(std::move(header));
    
    // Add menu items grid
    auto itemsGrid = createMenuItemsGrid();
    layout->addWidget(std::move(itemsGrid));
    
    contentContainer_->setLayout(std::move(layout));
}

std::unique_ptr<Wt::WContainerWidget> CategoryPopover::createHeader() {
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("popover-header");
    
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Category icon and title
    auto titleContainer = std::make_unique<Wt::WContainerWidget>();
    auto titleLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto iconText = std::make_unique<Wt::WText>(getCategoryIcon(category_));
    iconText->addStyleClass("category-tile-icon");
    titleLayout->addWidget(std::move(iconText));
    
    headerText_ = titleLayout->addWidget(std::make_unique<Wt::WText>(getCategoryDisplayName(category_)));
    headerText_->addStyleClass("popover-title");
    
    titleContainer->setLayout(std::move(titleLayout));
    headerLayout->addWidget(std::move(titleContainer), 1);
    
    // Close button
    closeButton_ = headerLayout->addWidget(std::make_unique<Wt::WPushButton>("×"));
    closeButton_->addStyleClass("btn btn-close");
    closeButton_->clicked().connect([this]() {
        hide();
    });
    
    header->setLayout(std::move(headerLayout));
    
    return header;
}

std::unique_ptr<Wt::WContainerWidget> CategoryPopover::createMenuItemsGrid() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("popover-content");
    
    itemsContainer_ = container.get();
    
    refreshItemsDisplay();
    
    return container;
}

void CategoryPopover::refreshItemsDisplay() {
    if (!itemsContainer_) return;
    
    itemsContainer_->clear();
    
    // Create a vertical layout for menu items (no grid, just stacked items as per CSS)
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    for (const auto& item : menuItems_) {
        if (!item || !item->isAvailable()) continue;
        
        auto itemCard = createMenuItemCard(item);
        layout->addWidget(std::move(itemCard));
    }
    
    itemsContainer_->setLayout(std::move(layout));
}

std::unique_ptr<Wt::WContainerWidget> CategoryPopover::createMenuItemCard(std::shared_ptr<MenuItem> item) {
    auto card = std::make_unique<Wt::WContainerWidget>();
    card->addStyleClass("menu-item-card");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // Create header with name and price
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("menu-item-header");
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Item name
    auto nameText = std::make_unique<Wt::WText>(item->getName());
    nameText->addStyleClass("menu-item-name");
    headerLayout->addWidget(std::move(nameText), 1); // flex: 1
    
    // Item price
    std::stringstream priceStream;
    priceStream << std::fixed << std::setprecision(2) << "$" << item->getPrice();
    auto priceText = std::make_unique<Wt::WText>(priceStream.str());
    priceText->addStyleClass("menu-item-price");
    headerLayout->addWidget(std::move(priceText));
    
    header->setLayout(std::move(headerLayout));
    layout->addWidget(std::move(header));
    
    // Add description if available and enabled
    // Note: MenuItem doesn't have getDescription() method yet
    // If you add it later, uncomment the following:
    /*
    if (showDescriptions_ && !item->getDescription().empty()) {
        auto descText = std::make_unique<Wt::WText>(item->getDescription());
        descText->addStyleClass("menu-item-description");
        layout->addWidget(std::move(descText));
    }
    */
    
    card->setLayout(std::move(layout));
    
    // Make card clickable
    card->clicked().connect([this, item]() {
        onItemSelected(item);
    });
    
    return card;
}

void CategoryPopover::onItemSelected(std::shared_ptr<MenuItem> item) {
    if (selectionCallback_) {
        selectionCallback_(item);
    }
    
    // Publish event
    if (eventManager_) {
        eventManager_->publish("MENU_ITEM_SELECTED", item);
    }
    
    // Hide popover after selection
    hide();
}

void CategoryPopover::showPopover(Wt::WWidget* anchor) {
    if (!anchor) return;
    
    // Show popover - positioning will be handled by Wt's popup system
    // Position can be set using setPositionScheme if needed
    show();
}

void CategoryPopover::updateMenuItems(const std::vector<std::shared_ptr<MenuItem>>& items) {
    menuItems_ = items;
    refreshItemsDisplay();
}

void CategoryPopover::setMaxColumns(int columns) {
    // Note: This method is kept for API compatibility but doesn't affect layout
    // since the popover now uses a vertical list layout as per CSS design
    maxColumns_ = std::max(1, columns);
    // No need to refresh display as layout is now vertical
}

void CategoryPopover::setShowDescriptions(bool enabled) {
    showDescriptions_ = enabled;
    refreshItemsDisplay();
}

std::string CategoryPopover::getCategoryDisplayName(MenuItem::Category category) const {
    switch (category) {
        case MenuItem::APPETIZER: return "Appetizers";
        case MenuItem::MAIN_COURSE: return "Main Courses";
        case MenuItem::DESSERT: return "Desserts";
        case MenuItem::BEVERAGE: return "Beverages";
        case MenuItem::SPECIAL: return "Daily Specials";
        default: return "Menu Items";
    }
}

std::string CategoryPopover::getCategoryIcon(MenuItem::Category category) const {
    switch (category) {
        case MenuItem::APPETIZER: return "🥗";
        case MenuItem::MAIN_COURSE: return "🍽️";
        case MenuItem::DESSERT: return "🍰";
        case MenuItem::BEVERAGE: return "🥤";
        case MenuItem::SPECIAL: return "⭐";
        default: return "🍴";
    }
}

void CategoryPopover::setupStyling() {
    // Apply CSS classes that are defined in the theme system
    contentContainer_->addStyleClass("category-popover");
    
    // Dimensions are controlled by CSS - no need to set programmatically
}
