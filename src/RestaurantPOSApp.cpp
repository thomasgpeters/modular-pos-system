#include "../include/RestaurantPOSApp.hpp"

#include <Wt/WBootstrapTheme.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPanel.h>
#include <Wt/WTabWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WTextArea.h>
#include <Wt/WMessageBox.h>
#include <Wt/WDialog.h>
#include <Wt/WGridLayout.h>

#include <sstream>
#include <iomanip>

/**
 * @file RestaurantPOSApp.cpp
 * @brief Theme-enabled POS implementation with external CSS loading
 */

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env) : Wt::WApplication(env) {
    setTitle("Restaurant POS System");
    
    // Set up Bootstrap theme for modern UI
    auto bootstrapTheme = std::make_shared<Wt::WBootstrapTheme>();
    setTheme(bootstrapTheme);
    
    // Initialize theme system
    initializeThemes();
    
    // Load default theme
    loadTheme("default");
    
    // Initialize core components
    orderManager_ = std::make_unique<OrderManager>();
    paymentProcessor_ = std::make_unique<PaymentProcessor>();
    kitchenInterface_ = std::make_unique<KitchenInterface>();
    
    // Initialize sample menu items
    initializeSampleMenu();
    
    // Build user interface
    buildMainInterface();
    
    // Set up real-time updates
    setupRealTimeUpdates();
}

void RestaurantPOSApp::initializeThemes() {
    // Define available themes
    availableThemes_ = {
        {"default", "Professional Light"},
        {"dark", "Professional Dark"},
        {"blue", "Blue Professional"},
        {"green", "Green Nature"}
    };
    
    // Set default theme
    currentTheme_ = "default";
}

void RestaurantPOSApp::loadTheme(const std::string& themeName) {
    // Remove existing theme stylesheet if any
    if (currentThemeStyleSheet_) {
        removeStyleSheet(currentThemeStyleSheet_);
    }
    
    // Load new theme CSS file
    std::string themeFile = "themes/" + themeName + ".css";
    currentThemeStyleSheet_ = useStyleSheet(Wt::WLink(themeFile));
    
    // Update current theme
    currentTheme_ = themeName;
    
    // Store theme preference (in a real app, this would be saved to database/cookies)
    // For now, we'll just keep it in memory
}

void RestaurantPOSApp::switchTheme(const std::string& themeName) {
    if (availableThemes_.find(themeName) != availableThemes_.end()) {
        loadTheme(themeName);
        
        // Show notification
        showNotification("Theme changed to " + availableThemes_[themeName], "success");
    }
}

void RestaurantPOSApp::initializeSampleMenu() {
    // Clean menu without excessive emojis
    menuItems_.push_back(std::make_shared<MenuItem>(1, "Caesar Salad", 12.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(2, "Buffalo Wings", 14.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(3, "Calamari Rings", 13.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(4, "Garlic Bread", 8.99, MenuItem::APPETIZER));
    
    // Main Courses
    menuItems_.push_back(std::make_shared<MenuItem>(5, "Grilled Salmon", 24.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(6, "Ribeye Steak", 32.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(7, "Chicken Parmesan", 19.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(8, "Pasta Primavera", 16.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(9, "Wood Fired Pizza", 18.99, MenuItem::MAIN_COURSE));
    
    // Desserts
    menuItems_.push_back(std::make_shared<MenuItem>(10, "Chocolate Cake", 8.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(11, "Tiramisu", 9.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(12, "Ice Cream Sundae", 6.99, MenuItem::DESSERT));
    
    // Beverages
    menuItems_.push_back(std::make_shared<MenuItem>(13, "House Wine", 7.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(14, "Craft Beer", 5.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(15, "Soft Drink", 2.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(16, "Fresh Juice", 4.99, MenuItem::BEVERAGE));
    
    // Daily Special
    menuItems_.push_back(std::make_shared<MenuItem>(17, "Chef's Special", 28.99, MenuItem::SPECIAL));
}

void RestaurantPOSApp::buildMainInterface() {
    root()->addStyleClass("pos-container");
    
    // Create clean header with theme selector
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("pos-header");
    header->setPositionScheme(Wt::PositionScheme::Relative);
    
    // Add theme selector
    createThemeSelector(header.get());
    
    auto headerTitle = std::make_unique<Wt::WText>("Restaurant POS System");
    headerTitle->setInline(false);
    header->addWidget(std::move(headerTitle));
    
    auto subtitle = std::make_unique<Wt::WText>("Order Management • Payment Processing • Kitchen Interface");
    subtitle->addStyleClass("subtitle");
    subtitle->setInline(false);
    header->addWidget(std::move(subtitle));
    
    root()->addWidget(std::move(header));
    
    // Create main content layout
    auto mainContainer = std::make_unique<Wt::WContainerWidget>();
    auto mainLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Left panel: Menu and order entry (60% width)
    auto leftPanel = createOrderEntryPanel();
    mainLayout->addWidget(std::move(leftPanel), 3);
    
    // Right panel: Active orders and kitchen status (40% width)
    auto rightPanel = createOrderStatusPanel();
    mainLayout->addWidget(std::move(rightPanel), 2);
    
    mainContainer->setLayout(std::move(mainLayout));
    root()->addWidget(std::move(mainContainer));
}

void RestaurantPOSApp::createThemeSelector(Wt::WContainerWidget* parent) {
    auto themeContainer = std::make_unique<Wt::WContainerWidget>();
    themeContainer->addStyleClass("theme-selector");
    
    themeSelector_ = themeContainer->addWidget(std::make_unique<Wt::WComboBox>());
    
    // Populate theme options
    for (const auto& theme : availableThemes_) {
        themeSelector_->addItem(theme.second); // Display name
        themeSelector_->setItemData(themeSelector_->count() - 1, theme.first); // Theme key
    }
    
    // Set current theme as selected
    for (int i = 0; i < themeSelector_->count(); ++i) {
        if (themeSelector_->itemData(i).toString() == currentTheme_) {
            themeSelector_->setCurrentIndex(i);
            break;
        }
    }
    
    // Handle theme changes
    themeSelector_->changed().connect([this] {
        int index = themeSelector_->currentIndex();
        if (index >= 0) {
            std::string selectedTheme = themeSelector_->itemData(index).toString();
            switchTheme(selectedTheme);
        }
    });
    
    parent->addWidget(std::move(themeContainer));
}

std::unique_ptr<Wt::WWidget> RestaurantPOSApp::createOrderEntryPanel() {
    auto panel = std::make_unique<Wt::WContainerWidget>();
    panel->addStyleClass("order-panel");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Table selection section
    auto tableContainer = std::make_unique<Wt::WContainerWidget>();
    tableContainer->addStyleClass("table-input-section");
    auto tableLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto tableLabel = std::make_unique<Wt::WText>("Table Number: ");
    tableLabel->addStyleClass("me-3");
    tableLayout->addWidget(std::move(tableLabel));
    
    tableNumberEdit_ = tableLayout->addWidget(std::make_unique<Wt::WSpinBox>());
    tableNumberEdit_->setMinimum(1);
    tableNumberEdit_->setMaximum(50);
    tableNumberEdit_->setValue(1);
    tableNumberEdit_->addStyleClass("form-control me-3");
    tableNumberEdit_->setWidth("100px");
    
    auto newOrderBtn = tableLayout->addWidget(std::make_unique<Wt::WPushButton>("Start New Order"));
    newOrderBtn->addStyleClass("btn btn-primary btn-subtle");
    newOrderBtn->clicked().connect([this] { startNewOrder(); });
    
    tableContainer->setLayout(std::move(tableLayout));
    layout->addWidget(std::move(tableContainer));
    
    // Menu categories section
    auto menuHeader = std::make_unique<Wt::WText>("Menu Categories");
    menuHeader->addStyleClass("section-header");
    layout->addWidget(std::move(menuHeader));
    
    // Create category tiles container
    categoryTilesContainer_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    categoryTilesContainer_->addStyleClass("category-tiles-container");
    buildCategoryTiles();
    
    // Current order section
    auto orderHeader = std::make_unique<Wt::WText>("Current Order");
    orderHeader->addStyleClass("section-header mt-custom");
    layout->addWidget(std::move(orderHeader));
    
    currentOrderTable_ = layout->addWidget(std::make_unique<Wt::WTable>());
    currentOrderTable_->addStyleClass("table table-sm order-summary");
    updateCurrentOrderTable();
    
    // Order actions section
    auto actionContainer = std::make_unique<Wt::WContainerWidget>();
    actionContainer->addStyleClass("mt-custom");
    auto actionLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto sendToKitchenBtn = actionLayout->addWidget(std::make_unique<Wt::WPushButton>("Send to Kitchen"));
    sendToKitchenBtn->addStyleClass("btn btn-subtle btn-kitchen me-3");
    sendToKitchenBtn->clicked().connect([this] { sendCurrentOrderToKitchen(); });
    
    auto processPaymentBtn = actionLayout->addWidget(std::make_unique<Wt::WPushButton>("Process Payment"));
    processPaymentBtn->addStyleClass("btn btn-subtle btn-payment");
    processPaymentBtn->clicked().connect([this] { showPaymentDialog(); });
    
    actionContainer->setLayout(std::move(actionLayout));
    layout->addWidget(std::move(actionContainer));
    
    panel->setLayout(std::move(layout));
    return std::move(panel);
}

void RestaurantPOSApp::buildCategoryTiles() {
    categoryTilesContainer_->clear();
    
    // Group items by category and count them
    std::map<MenuItem::Category, std::vector<std::shared_ptr<MenuItem>>> itemsByCategory;
    for (const auto& item : menuItems_) {
        itemsByCategory[item->getCategory()].push_back(item);
    }
    
    // Category icons
    std::map<MenuItem::Category, std::string> categoryIcons = {
        {MenuItem::APPETIZER, "🥗"},
        {MenuItem::MAIN_COURSE, "🍽️"},
        {MenuItem::DESSERT, "🍰"},
        {MenuItem::BEVERAGE, "🥤"},
        {MenuItem::SPECIAL, "⭐"}
    };
    
    // Create tiles for each category
    for (const auto& categoryPair : itemsByCategory) {
        auto tile = std::make_unique<Wt::WContainerWidget>();
        tile->addStyleClass("category-tile");
        
        // Icon
        auto icon = std::make_unique<Wt::WText>(categoryIcons[categoryPair.first]);
        icon->addStyleClass("category-tile-icon");
        tile->addWidget(std::move(icon));
        
        // Title
        auto title = std::make_unique<Wt::WText>(MenuItem::categoryToString(categoryPair.first));
        title->addStyleClass("category-tile-title");
        tile->addWidget(std::move(title));
        
        // Item count
        auto count = std::make_unique<Wt::WText>(std::to_string(categoryPair.second.size()) + " items");
        count->addStyleClass("category-tile-count");
        tile->addWidget(std::move(count));
        
        // Click handler
        tile->clicked().connect([this, categoryPair] {
            showCategoryPopover(categoryPair.first, categoryPair.second);
        });
        
        categoryTilesContainer_->addWidget(std::move(tile));
    }
}

void RestaurantPOSApp::showCategoryPopover(MenuItem::Category category, 
                                          const std::vector<std::shared_ptr<MenuItem>>& items) {
    // Create modal dialog
    auto dialog = addChild(std::make_unique<Wt::WDialog>(MenuItem::categoryToString(category)));
    dialog->addStyleClass("category-popover");
    dialog->setModal(true);
    dialog->setResizable(false);
    
    // Content container
    auto content = std::make_unique<Wt::WContainerWidget>();
    
    // Items list
    auto itemsContainer = std::make_unique<Wt::WContainerWidget>();
    itemsContainer->addStyleClass("popover-content");
    
    for (const auto& item : items) {
        auto itemCard = std::make_unique<Wt::WContainerWidget>();
        itemCard->addStyleClass("menu-item-card");
        
        // Item header with name and price
        auto headerContainer = std::make_unique<Wt::WContainerWidget>();
        headerContainer->addStyleClass("menu-item-header");
        auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
        
        auto itemName = std::make_unique<Wt::WText>(item->getName());
        itemName->addStyleClass("menu-item-name");
        headerLayout->addWidget(std::move(itemName));
        
        auto itemPrice = std::make_unique<Wt::WText>(formatCurrency(item->getPrice()));
        itemPrice->addStyleClass("menu-item-price");
        headerLayout->addWidget(std::move(itemPrice));
        
        headerContainer->setLayout(std::move(headerLayout));
        itemCard->addWidget(std::move(headerContainer));
        
        // Optional description (you could add this to MenuItem class later)
        auto description = std::make_unique<Wt::WText>("Delicious " + item->getName());
        description->addStyleClass("menu-item-description");
        itemCard->addWidget(std::move(description));
        
        // Click handler to add item to order
        itemCard->clicked().connect([this, item, dialog] {
            addItemToCurrentOrder(item);
            dialog->accept(); // Close the dialog
        });
        
        itemsContainer->addWidget(std::move(itemCard));
    }
    
    content->addWidget(std::move(itemsContainer));
    
    // Footer with cancel button
    auto footer = std::make_unique<Wt::WContainerWidget>();
    footer->addStyleClass("popover-footer");
    
    auto cancelBtn = std::make_unique<Wt::WPushButton>("Cancel");
    cancelBtn->addStyleClass("btn btn-subtle btn-cancel");
    cancelBtn->clicked().connect([dialog] {
        dialog->reject();
    });
    footer->addWidget(std::move(cancelBtn));
    
    content->addWidget(std::move(footer));
    
    dialog->contents()->addWidget(std::move(content));
    dialog->show();
}

std::unique_ptr<Wt::WWidget> RestaurantPOSApp::createOrderStatusPanel() {
    auto panel = std::make_unique<Wt::WContainerWidget>();
    panel->addStyleClass("status-panel");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Create tab widget for different views
    auto tabWidget = std::make_unique<Wt::WTabWidget>();
    tabWidget->addStyleClass("nav-pills status-tabs-container");
    
    // Active Orders tab
    auto activeOrdersContainer = std::make_unique<Wt::WContainerWidget>();
    activeOrdersContainer->addStyleClass("tab-content-container");
    auto activeOrdersLayout = std::make_unique<Wt::WVBoxLayout>();
    activeOrdersLayout->setContentsMargins(15, 15, 15, 15);
    
    auto activeHeader = std::make_unique<Wt::WText>("Active Orders");
    activeHeader->addStyleClass("section-header");
    activeOrdersLayout->addWidget(std::move(activeHeader));
    
    auto activeScrollContainer = std::make_unique<Wt::WContainerWidget>();
    activeScrollContainer->addStyleClass("active-orders-panel");
    
    activeOrdersTable_ = activeScrollContainer->addWidget(std::make_unique<Wt::WTable>());
    activeOrdersTable_->addStyleClass("table table-hover");
    activeOrdersTable_->setWidth("100%");
    updateActiveOrdersTable();
    
    activeOrdersLayout->addWidget(std::move(activeScrollContainer));
    activeOrdersContainer->setLayout(std::move(activeOrdersLayout));
    tabWidget->addTab(std::move(activeOrdersContainer), "Active Orders", Wt::ContentLoading::Lazy);
    
    // Kitchen Status tab
    auto kitchenContainer = std::make_unique<Wt::WContainerWidget>();
    kitchenContainer->addStyleClass("tab-content-container");
    auto kitchenLayout = std::make_unique<Wt::WVBoxLayout>();
    kitchenLayout->setContentsMargins(15, 15, 15, 15);
    
    auto kitchenHeader = std::make_unique<Wt::WText>("Kitchen Status");
    kitchenHeader->addStyleClass("section-header");
    kitchenLayout->addWidget(std::move(kitchenHeader));
    
    auto kitchenScrollContainer = std::make_unique<Wt::WContainerWidget>();
    kitchenScrollContainer->addStyleClass("kitchen-queue-panel");
    
    kitchenStatusTable_ = kitchenScrollContainer->addWidget(std::make_unique<Wt::WTable>());
    kitchenStatusTable_->addStyleClass("table table-hover");
    kitchenStatusTable_->setWidth("100%");
    updateKitchenStatusTable();
    
    kitchenLayout->addWidget(std::move(kitchenScrollContainer));
    kitchenContainer->setLayout(std::move(kitchenLayout));
    tabWidget->addTab(std::move(kitchenContainer), "Kitchen Status", Wt::ContentLoading::Lazy);
    
    layout->addWidget(std::move(tabWidget));
    panel->setLayout(std::move(layout));
    return std::move(panel);
}

// Rest of the methods remain the same as the category tiles implementation...
void RestaurantPOSApp::updateCurrentOrderTable() {
    currentOrderTable_->clear();
    
    if (!currentOrder_) {
        auto noOrderText = std::make_unique<Wt::WText>("No active order. Start a new order to begin.");
        noOrderText->addStyleClass("text-muted fst-italic text-center");
        currentOrderTable_->elementAt(0, 0)->addWidget(std::move(noOrderText));
        currentOrderTable_->elementAt(0, 0)->setColumnSpan(5);
        return;
    }
    
    // Headers
    currentOrderTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    currentOrderTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Qty"));
    currentOrderTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Unit Price"));
    currentOrderTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));
    currentOrderTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Action"));
    
    const auto& items = currentOrder_->getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        int row = i + 1;
        
        currentOrderTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(item.getMenuItem().getName()));
        currentOrderTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(std::to_string(item.getQuantity())));
        
        auto unitPrice = std::make_unique<Wt::WText>(formatCurrency(item.getMenuItem().getPrice()));
        unitPrice->addStyleClass("currency");
        currentOrderTable_->elementAt(row, 2)->addWidget(std::move(unitPrice));
        
        auto totalPrice = std::make_unique<Wt::WText>(formatCurrency(item.getTotalPrice()));
        totalPrice->addStyleClass("currency");
        currentOrderTable_->elementAt(row, 3)->addWidget(std::move(totalPrice));
        
        auto removeBtn = std::make_unique<Wt::WPushButton>("Remove");
        removeBtn->addStyleClass("btn btn-subtle btn-remove");
        removeBtn->clicked().connect([this, i] { removeItemFromCurrentOrder(i); });
        currentOrderTable_->elementAt(row, 4)->addWidget(std::move(removeBtn));
        
        // Center align quantity and price columns
        currentOrderTable_->elementAt(row, 1)->addStyleClass("text-center");
        currentOrderTable_->elementAt(row, 2)->addStyleClass("text-center");
        currentOrderTable_->elementAt(row, 3)->addStyleClass("text-center");
        currentOrderTable_->elementAt(row, 4)->addStyleClass("text-center");
    }
    
    // Totals section
    int totalRow = items.size() + 1;
    currentOrderTable_->elementAt(totalRow, 0)->addWidget(std::make_unique<Wt::WText>("Subtotal:"));
    currentOrderTable_->elementAt(totalRow, 0)->setColumnSpan(3);
    currentOrderTable_->elementAt(totalRow, 0)->addStyleClass("text-end fw-bold");
    
    auto subtotal = std::make_unique<Wt::WText>(formatCurrency(currentOrder_->getSubtotal()));
    subtotal->addStyleClass("currency");
    currentOrderTable_->elementAt(totalRow, 3)->addWidget(std::move(subtotal));
    currentOrderTable_->elementAt(totalRow, 3)->addStyleClass("text-center");
    
    totalRow++;
    currentOrderTable_->elementAt(totalRow, 0)->addWidget(std::make_unique<Wt::WText>("Tax:"));
    currentOrderTable_->elementAt(totalRow, 0)->setColumnSpan(3);
    currentOrderTable_->elementAt(totalRow, 0)->addStyleClass("text-end fw-bold");
    
    auto tax = std::make_unique<Wt::WText>(formatCurrency(currentOrder_->getTax()));
    tax->addStyleClass("currency");
    currentOrderTable_->elementAt(totalRow, 3)->addWidget(std::move(tax));
    currentOrderTable_->elementAt(totalRow, 3)->addStyleClass("text-center");
    
    totalRow++;
    auto totalLabel = std::make_unique<Wt::WText>("TOTAL:");
    totalLabel->addStyleClass("fw-bold text-white");
    currentOrderTable_->elementAt(totalRow, 0)->addWidget(std::move(totalLabel));
    currentOrderTable_->elementAt(totalRow, 0)->setColumnSpan(3);
    currentOrderTable_->elementAt(totalRow, 0)->addStyleClass("total-row text-end");
    
    auto totalAmount = std::make_unique<Wt::WText>(formatCurrency(currentOrder_->getTotal()));
    totalAmount->addStyleClass("fw-bold text-white");
    currentOrderTable_->elementAt(totalRow, 3)->addWidget(std::move(totalAmount));
    currentOrderTable_->elementAt(totalRow, 3)->addStyleClass("total-row text-center");
    currentOrderTable_->elementAt(totalRow, 4)->addStyleClass("total-row");
}

void RestaurantPOSApp::updateActiveOrdersTable() {
    activeOrdersTable_->clear();
    
    auto orders = orderManager_->getActiveOrders();
    
    if (orders.empty()) {
        auto emptyContainer = std::make_unique<Wt::WContainerWidget>();
        emptyContainer->addStyleClass("text-center p-4");
        auto noOrdersText = std::make_unique<Wt::WText>("No active orders.");
        noOrdersText->addStyleClass("text-muted fst-italic");
        emptyContainer->addWidget(std::move(noOrdersText));
        activeOrdersTable_->elementAt(0, 0)->addWidget(std::move(emptyContainer));
        activeOrdersTable_->elementAt(0, 0)->setColumnSpan(5);
        return;
    }
    
    // Headers
    auto orderHeader = std::make_unique<Wt::WText>("Order #");
    orderHeader->addStyleClass("fw-bold");
    activeOrdersTable_->elementAt(0, 0)->addWidget(std::move(orderHeader));
    
    auto tableHeader = std::make_unique<Wt::WText>("Table");
    tableHeader->addStyleClass("fw-bold");
    activeOrdersTable_->elementAt(0, 1)->addWidget(std::move(tableHeader));
    
    auto statusHeader = std::make_unique<Wt::WText>("Status");
    statusHeader->addStyleClass("fw-bold");
    activeOrdersTable_->elementAt(0, 2)->addWidget(std::move(statusHeader));
    
    auto totalHeader = std::make_unique<Wt::WText>("Total");
    totalHeader->addStyleClass("fw-bold");
    activeOrdersTable_->elementAt(0, 3)->addWidget(std::move(totalHeader));
    
    auto itemsHeader = std::make_unique<Wt::WText>("Items");
    itemsHeader->addStyleClass("fw-bold");
    activeOrdersTable_->elementAt(0, 4)->addWidget(std::move(itemsHeader));
    
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& order = orders[i];
        int row = i + 1;
        
        activeOrdersTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>("#" + std::to_string(order->getOrderId())));
        activeOrdersTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>("Table " + std::to_string(order->getTableNumber())));
        
        auto statusBadge = std::make_unique<Wt::WText>(getStatusString(order->getStatus()));
        statusBadge->addStyleClass("status-badge " + getStatusClass(order->getStatus()));
        activeOrdersTable_->elementAt(row, 2)->addWidget(std::move(statusBadge));
        
        auto total = std::make_unique<Wt::WText>(formatCurrency(order->getTotal()));
        total->addStyleClass("currency");
        activeOrdersTable_->elementAt(row, 3)->addWidget(std::move(total));
        
        activeOrdersTable_->elementAt(row, 4)->addWidget(std::make_unique<Wt::WText>(std::to_string(order->getItems().size()) + " items"));
        
        // Center align columns
        for (int col = 0; col < 5; ++col) {
            activeOrdersTable_->elementAt(row, col)->addStyleClass("text-center");
        }
    }
}

void RestaurantPOSApp::updateKitchenStatusTable() {
    kitchenStatusTable_->clear();
    
    auto tickets = kitchenInterface_->getActiveTickets();
    
    if (tickets.empty()) {
        auto emptyContainer = std::make_unique<Wt::WContainerWidget>();
        emptyContainer->addStyleClass("text-center p-4");
        auto noTicketsText = std::make_unique<Wt::WText>("No orders in kitchen queue.");
        noTicketsText->addStyleClass("text-muted fst-italic");
        emptyContainer->addWidget(std::move(noTicketsText));
        kitchenStatusTable_->elementAt(0, 0)->addWidget(std::move(emptyContainer));
        kitchenStatusTable_->elementAt(0, 0)->setColumnSpan(5);
        return;
    }
    
    // Headers
    auto orderHeader = std::make_unique<Wt::WText>("Order #");
    orderHeader->addStyleClass("fw-bold");
    kitchenStatusTable_->elementAt(0, 0)->addWidget(std::move(orderHeader));
    
    auto tableHeader = std::make_unique<Wt::WText>("Table");
    tableHeader->addStyleClass("fw-bold");
    kitchenStatusTable_->elementAt(0, 1)->addWidget(std::move(tableHeader));
    
    auto statusHeader = std::make_unique<Wt::WText>("Kitchen Status");
    statusHeader->addStyleClass("fw-bold");
    kitchenStatusTable_->elementAt(0, 2)->addWidget(std::move(statusHeader));
    
    auto timeHeader = std::make_unique<Wt::WText>("Est. Time");
    timeHeader->addStyleClass("fw-bold");
    kitchenStatusTable_->elementAt(0, 3)->addWidget(std::move(timeHeader));
    
    auto actionsHeader = std::make_unique<Wt::WText>("Actions");
    actionsHeader->addStyleClass("fw-bold");
    kitchenStatusTable_->elementAt(0, 4)->addWidget(std::move(actionsHeader));
    
    for (size_t i = 0; i < tickets.size(); ++i) {
        const auto& ticket = tickets[i];
        int row = i + 1;
        
        kitchenStatusTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>("#" + std::to_string(ticket.orderId)));
        kitchenStatusTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>("Table " + std::to_string(ticket.tableNumber)));
        
        auto statusBadge = std::make_unique<Wt::WText>(getKitchenStatusString(ticket.status));
        statusBadge->addStyleClass("status-badge " + getKitchenStatusClass(ticket.status));
        kitchenStatusTable_->elementAt(row, 2)->addWidget(std::move(statusBadge));
        
        kitchenStatusTable_->elementAt(row, 3)->addWidget(std::make_unique<Wt::WText>(std::to_string(ticket.estimatedPrepTime) + " min"));
        
        // Status update buttons
        auto buttonContainer = std::make_unique<Wt::WContainerWidget>();
        auto buttonLayout = std::make_unique<Wt::WHBoxLayout>();
        
        if (ticket.status == KitchenInterface::ORDER_RECEIVED) {
            auto startBtn = std::make_unique<Wt::WPushButton>("Start");
            startBtn->addStyleClass("btn btn-success btn-subtle kitchen-status-btn");
            startBtn->clicked().connect([this, ticket] { 
                updateOrderKitchenStatus(ticket.orderId, KitchenInterface::PREP_STARTED); 
            });
            buttonLayout->addWidget(std::move(startBtn));
        }
        
        if (ticket.status == KitchenInterface::PREP_STARTED) {
            auto readyBtn = std::make_unique<Wt::WPushButton>("Ready");
            readyBtn->addStyleClass("btn btn-warning btn-subtle kitchen-status-btn");
            readyBtn->clicked().connect([this, ticket] { 
                updateOrderKitchenStatus(ticket.orderId, KitchenInterface::READY_FOR_PICKUP); 
            });
            buttonLayout->addWidget(std::move(readyBtn));
        }
        
        if (ticket.status == KitchenInterface::READY_FOR_PICKUP) {
            auto servedBtn = std::make_unique<Wt::WPushButton>("Served");
            servedBtn->addStyleClass("btn btn-primary btn-subtle kitchen-status-btn");
            servedBtn->clicked().connect([this, ticket] { 
                updateOrderKitchenStatus(ticket.orderId, KitchenInterface::SERVED); 
            });
            buttonLayout->addWidget(std::move(servedBtn));
        }
        
        buttonContainer->setLayout(std::move(buttonLayout));
        kitchenStatusTable_->elementAt(row, 4)->addWidget(std::move(buttonContainer));
        
        // Center align columns
        for (int col = 0; col < 5; ++col) {
            kitchenStatusTable_->elementAt(row, col)->addStyleClass("text-center");
        }
    }
}

std::string RestaurantPOSApp::getStatusClass(Order::Status status) {
    switch (status) {
        case Order::PENDING: return "status-pending";
        case Order::SENT_TO_KITCHEN:
        case Order::PREPARING: return "status-kitchen";
        case Order::READY: return "status-ready";
        default: return "status-pending";
    }
}

std::string RestaurantPOSApp::getKitchenStatusClass(KitchenInterface::KitchenStatus status) {
    switch (status) {
        case KitchenInterface::ORDER_RECEIVED: return "status-pending";
        case KitchenInterface::PREP_STARTED: return "status-kitchen";
        case KitchenInterface::READY_FOR_PICKUP: return "status-ready";
        default: return "status-pending";
    }
}

// Existing implementation methods remain the same...
void RestaurantPOSApp::startNewOrder() {
    int tableNumber = tableNumberEdit_->value();
    currentOrder_ = orderManager_->createOrder(tableNumber);
    updateCurrentOrderTable();
    showNotification("New order started for table " + std::to_string(tableNumber), "success");
}

void RestaurantPOSApp::addItemToCurrentOrder(std::shared_ptr<MenuItem> menuItem) {
    if (!currentOrder_) {
        startNewOrder();
    }
    
    OrderItem item(*menuItem);
    currentOrder_->addItem(item);
    updateCurrentOrderTable();
    showNotification("Added " + menuItem->getName() + " to order", "success");
}

void RestaurantPOSApp::removeItemFromCurrentOrder(size_t index) {
    if (currentOrder_ && index < currentOrder_->getItems().size()) {
        std::string itemName = currentOrder_->getItems()[index].getMenuItem().getName();
        currentOrder_->removeItem(index);
        updateCurrentOrderTable();
        showNotification("Removed " + itemName + " from order", "info");
    }
}

void RestaurantPOSApp::sendCurrentOrderToKitchen() {
    if (currentOrder_ && !currentOrder_->getItems().empty()) {
        bool success = kitchenInterface_->sendOrderToKitchen(currentOrder_);
        if (success) {
            updateActiveOrdersTable();
            updateKitchenStatusTable();
            showNotification("Order #" + std::to_string(currentOrder_->getOrderId()) + " sent to kitchen", "success");
            
            // Clear current order
            currentOrder_.reset();
            updateCurrentOrderTable();
        } else {
            showNotification("Failed to send order to kitchen", "error");
        }
    } else {
        showNotification("No items in current order", "warning");
    }
}

void RestaurantPOSApp::showPaymentDialog() {
    if (!currentOrder_) {
        showNotification("No active order to process payment", "warning");
        return;
    }
    
    // Simple payment processing for demo - process as cash payment
    processPayment(PaymentProcessor::CASH, currentOrder_->getTotal());
}

void RestaurantPOSApp::processPayment(PaymentProcessor::PaymentMethod method, 
                                     double amount, double tipAmount) {
    if (!currentOrder_) return;
    
    auto result = paymentProcessor_->processPayment(currentOrder_, method, amount, tipAmount);
    
    if (result.success) {
        orderManager_->completeOrder(currentOrder_->getOrderId());
        updateActiveOrdersTable();
        updateKitchenStatusTable();
        showNotification("Payment processed successfully. Transaction ID: " + result.transactionId, "success");
        
        // Clear current order
        currentOrder_.reset();
        updateCurrentOrderTable();
    } else {
        showNotification("Payment failed: " + result.errorMessage, "error");
    }
}

void RestaurantPOSApp::updateOrderKitchenStatus(int orderId, KitchenInterface::KitchenStatus status) {
    bool success = kitchenInterface_->updateKitchenStatus(orderId, status);
    if (success) {
        // Update order status in order manager based on kitchen status
        Order::Status orderStatus = Order::PREPARING;
        switch (status) {
            case KitchenInterface::PREP_STARTED:
                orderStatus = Order::PREPARING;
                break;
            case KitchenInterface::READY_FOR_PICKUP:
                orderStatus = Order::READY;
                break;
            case KitchenInterface::SERVED:
                orderManager_->completeOrder(orderId);
                break;
            default:
                break;
        }
        
        if (status != KitchenInterface::SERVED) {
            orderManager_->updateOrderStatus(orderId, orderStatus);
        }
        
        updateActiveOrdersTable();
        updateKitchenStatusTable();
        showNotification("Kitchen status updated for order #" + std::to_string(orderId), "success");
    } else {
        showNotification("Failed to update kitchen status", "error");
    }
}

void RestaurantPOSApp::setupRealTimeUpdates() {
    updateTimer_ = root()->addChild(std::make_unique<Wt::WTimer>());
    updateTimer_->setInterval(std::chrono::seconds(UPDATE_INTERVAL_SECONDS));
    updateTimer_->timeout().connect([this] { performPeriodicUpdate(); });
    updateTimer_->start();
}

void RestaurantPOSApp::performPeriodicUpdate() {
    updateActiveOrdersTable();
    updateKitchenStatusTable();
}

std::string RestaurantPOSApp::getStatusString(Order::Status status) {
    return Order::statusToString(status);
}

std::string RestaurantPOSApp::getKitchenStatusString(KitchenInterface::KitchenStatus status) {
    return KitchenInterface::kitchenStatusToString(status);
}

std::string RestaurantPOSApp::formatCurrency(double amount) {
    std::stringstream ss;
    ss << "$" << std::fixed << std::setprecision(2) << amount;
    return ss.str();
}

void RestaurantPOSApp::showNotification(const std::string& message, const std::string& type) {
    // Simple notification using message box for demo
    // In a real implementation, this would use toast notifications or a notification system
    if (type == "error") {
        auto messageBox = addChild(std::make_unique<Wt::WMessageBox>("Error", message, Wt::Icon::Critical, Wt::StandardButton::Ok));
        messageBox->show();
    }
    // For other types, we could implement a toast notification system
}

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
