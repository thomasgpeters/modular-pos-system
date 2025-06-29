#include "../include/RestaurantPOSApp.hpp"

#include <Wt/WBootstrapTheme.h>
#include <Wt/WCssTheme.h>
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
#include <Wt/WGroupBox.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WDialog.h>

#include <sstream>
#include <iomanip>
#include <map>
#include <fstream>
#include <iostream>

/**
 * @file RestaurantPOSApp.cpp (Complete Fixed Version)
 * @brief Implementation with working theme system and all methods
 */

RestaurantPOSApp::RestaurantPOSApp(const Wt::WEnvironment& env) : Wt::WApplication(env) {
    setTitle("Restaurant POS System - Three-Legged Foundation");
    
    // IMPORTANT: Set up Bootstrap theme FIRST before any CSS loading
    auto bootstrapTheme = std::make_shared<Wt::WBootstrapTheme>();
    bootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
    setTheme(bootstrapTheme);
    
    // Initialize theme system with hardcoded themes
    initializeHardcodedThemes();
    
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

void RestaurantPOSApp::initializeHardcodedThemes() {
    // Create hardcoded theme configuration (no JSON parsing)
    availableThemes_ = {
        {"bootstrap", "Bootstrap (Default)", "Modern responsive Bootstrap theme", 
         "themes/bootstrap.css", "", {"#007bff", "#0056b3"}, true},
        {"classic", "Classic", "Clean classic theme with traditional styling", 
         "themes/classic.css", "", {"#f8f9fa", "#e9ecef"}, false},
        {"professional", "Professional", "Professional business theme with dark headers", 
         "themes/professional.css", "", {"#2c3e50", "#3498db"}, false},
        {"colorful", "Colorful", "Vibrant theme with gradients and modern styling", 
         "themes/colorful.css", "", {"#667eea", "#764ba2"}, false}
    };
    
    currentTheme_ = "bootstrap";
    themeDirectory_ = "themes/";
    allowUserThemes_ = true;
    
    // Apply the default theme
    applyTheme(currentTheme_);
}

void RestaurantPOSApp::applyTheme(const std::string& themeName) {
    // Find the theme configuration
    auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
        [&themeName](const ThemeInfo& theme) { return theme.id == themeName; });
    
    if (it == availableThemes_.end()) {
        std::cerr << "Theme not found: " << themeName << std::endl;
        return; // Theme not found
    }
    
    const ThemeInfo& theme = *it;
    
    try {
        // Always use Bootstrap as the base theme for proper widget styling
        auto bootstrapTheme = std::make_shared<Wt::WBootstrapTheme>();
        bootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
        setTheme(bootstrapTheme);
        
        // Load base CSS first (contains common styles)
        std::cout << "Loading base CSS..." << std::endl;
        useStyleSheet(Wt::WLink("themes/base.css"));
        
        // Load theme-specific CSS
        if (!theme.cssFile.empty()) {
            std::cout << "Loading theme CSS: " << theme.cssFile << std::endl;
            useStyleSheet(Wt::WLink(theme.cssFile));
        }
        
        // Load external CSS if specified (for special themes)
        if (!theme.externalCss.empty()) {
            std::cout << "Loading external CSS: " << theme.externalCss << std::endl;
            useStyleSheet(Wt::WLink(theme.externalCss));
        }
        
        currentTheme_ = themeName;
        std::cout << "Theme applied successfully: " << themeName << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error applying theme " << themeName << ": " << e.what() << std::endl;
    }
    
    // Refresh the UI to apply new theme
    if (root()->children().size() > 0) {
        triggerUpdate();
    }
}

void RestaurantPOSApp::loadCSSFile(const std::string& filepath) {
    // DEPRECATED: This method is replaced by direct useStyleSheet calls in applyTheme
    // Keeping for backward compatibility but not used
    std::cerr << "Warning: loadCSSFile is deprecated, use applyTheme instead" << std::endl;
}

void RestaurantPOSApp::showThemeSelector() {
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Choose Theme"));
    dialog->addStyleClass("theme-dialog");
    dialog->setWidth("600px");
    dialog->setResizable(false);
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Header
    auto header = std::make_unique<Wt::WText>("Select Your Preferred Theme");
    header->addStyleClass("h4 mb-4 text-center");
    layout->addWidget(std::move(header));
    
    // Theme options
    auto buttonGroup = std::make_shared<Wt::WButtonGroup>();
    
    for (size_t i = 0; i < availableThemes_.size(); ++i) {
        const auto& theme = availableThemes_[i];
        
        auto themeContainer = std::make_unique<Wt::WContainerWidget>();
        themeContainer->addStyleClass("mb-3 p-3 border rounded");
        
        auto themeLayout = std::make_unique<Wt::WHBoxLayout>();
        
        // Theme preview with dynamic colors
        auto preview = std::make_unique<Wt::WContainerWidget>();
        preview->addStyleClass("theme-preview");
        preview->setWidth("80px");
        preview->setHeight("60px");
        
        // Create gradient background from preview colors
        if (theme.previewColors.size() >= 2) {
            std::string gradient = "background: linear-gradient(45deg, " + 
                                 theme.previewColors[0] + ", " + 
                                 theme.previewColors[1] + ");";
            preview->setAttributeValue("style", gradient);
        } else if (theme.previewColors.size() == 1) {
            preview->setAttributeValue("style", "background-color: " + theme.previewColors[0] + ";");
        }
        
        themeLayout->addWidget(std::move(preview));
        
        // Theme info
        auto infoContainer = std::make_unique<Wt::WContainerWidget>();
        auto infoLayout = std::make_unique<Wt::WVBoxLayout>();
        
        auto radio = std::make_unique<Wt::WRadioButton>(theme.name);
        radio->addStyleClass("h6 mb-1");
        if (theme.id == currentTheme_) {
            radio->setChecked(true);
        }
        
        // Store theme index in radio button - use setId instead of data attribute
        radio->setId("theme-radio-" + std::to_string(i));
        buttonGroup->addButton(radio.get());
        
        auto description = std::make_unique<Wt::WText>(theme.description);
        description->addStyleClass("text-muted small");
        
        // Show CSS file path for debugging
        auto cssPath = std::make_unique<Wt::WText>("CSS: " + theme.cssFile);
        cssPath->addStyleClass("text-muted small font-monospace");
        
        infoLayout->addWidget(std::move(radio));
        infoLayout->addWidget(std::move(description));
        infoLayout->addWidget(std::move(cssPath));
        infoContainer->setLayout(std::move(infoLayout));
        
        themeLayout->addWidget(std::move(infoContainer), 1);
        themeContainer->setLayout(std::move(themeLayout));
        
        layout->addWidget(std::move(themeContainer));
    }
    
    // Buttons
    auto buttonContainer = std::make_unique<Wt::WContainerWidget>();
    buttonContainer->addStyleClass("mt-4 text-center");
    auto buttonLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto applyBtn = std::make_unique<Wt::WPushButton>("Apply Theme");
    applyBtn->addStyleClass("btn btn-primary me-2");
    
    auto reloadBtn = std::make_unique<Wt::WPushButton>("Reload Themes");
    reloadBtn->addStyleClass("btn btn-secondary me-2");
    
    auto cancelBtn = std::make_unique<Wt::WPushButton>("Cancel");
    cancelBtn->addStyleClass("btn btn-outline-secondary");
    
    // Event handlers - simplified approach using IDs
    applyBtn->clicked().connect([this, buttonGroup, dialog]() {
        auto checkedButton = buttonGroup->checkedButton();
        if (checkedButton) {
            std::string buttonId = checkedButton->id();
            if (buttonId.substr(0, 12) == "theme-radio-") {
                try {
                    std::string indexStr = buttonId.substr(12);
                    size_t themeIndex = std::stoul(indexStr);
                    if (themeIndex < availableThemes_.size()) {
                        const auto& selectedTheme = availableThemes_[themeIndex];
                        std::cout << "Applying theme: " << selectedTheme.id << std::endl;
                        applyTheme(selectedTheme.id);
                        showNotification("Theme changed to " + selectedTheme.name, "success");
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error applying theme: " << e.what() << std::endl;
                    showNotification("Error applying theme", "error");
                }
            }
        }
        dialog->accept();
    });
    
    reloadBtn->clicked().connect([this, dialog]() {
        initializeHardcodedThemes();
        showNotification("Themes reloaded", "info");
        dialog->reject();
        // Reopen dialog with updated themes
        showThemeSelector();
    });
    
    cancelBtn->clicked().connect([dialog]() {
        dialog->reject();
    });
    
    buttonLayout->addWidget(std::move(applyBtn));
    buttonLayout->addWidget(std::move(reloadBtn));
    buttonLayout->addWidget(std::move(cancelBtn));
    buttonContainer->setLayout(std::move(buttonLayout));
    
    layout->addWidget(std::move(buttonContainer));
    
    dialog->contents()->setLayout(std::move(layout));
    dialog->show();
}

void RestaurantPOSApp::buildMainInterface() {
    root()->addStyleClass("container-fluid");
    
    // Add theme selector button
    auto themeSelector = std::make_unique<Wt::WContainerWidget>();
    themeSelector->addStyleClass("pos-theme-selector");
    
    auto themeBtn = std::make_unique<Wt::WPushButton>("🎨 Themes");
    themeBtn->addStyleClass("btn btn-outline-secondary btn-sm theme-button");
    themeBtn->clicked().connect([this]() { showThemeSelector(); });
    themeSelector->addWidget(std::move(themeBtn));
    root()->addWidget(std::move(themeSelector));
    
    // Add header with current theme indicator
    auto header = std::make_unique<Wt::WContainerWidget>();
    header->addStyleClass("pos-header");
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto title = std::make_unique<Wt::WText>("🍽️ Restaurant POS System");
    title->addStyleClass("h1 text-primary");
    headerLayout->addWidget(std::move(title));
    
    auto subtitle = std::make_unique<Wt::WText>("Three-Legged Foundation: Order Management • Payment Processing • Kitchen Interface");
    subtitle->addStyleClass("h6 text-muted");
    headerLayout->addWidget(std::move(subtitle));
    
    auto themeIndicator = std::make_unique<Wt::WText>();
    themeIndicator->addStyleClass("h6 text-muted");
    updateThemeIndicator(themeIndicator.get());
    themeIndicator_ = themeIndicator.get();
    headerLayout->addWidget(std::move(themeIndicator));
    
    header->setLayout(std::move(headerLayout));
    root()->addWidget(std::move(header));
    
    // Create main layout
    auto mainLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Left panel: Menu and order entry (60% width)
    auto leftPanel = createOrderEntryPanel();
    mainLayout->addWidget(std::move(leftPanel), 3);
    
    // Right panel: Active orders and kitchen status (40% width)
    auto rightPanel = createOrderStatusPanel();
    mainLayout->addWidget(std::move(rightPanel), 2);
    
    auto mainContainer = std::make_unique<Wt::WContainerWidget>();
    mainContainer->setLayout(std::move(mainLayout));
    root()->addWidget(std::move(mainContainer));
}

void RestaurantPOSApp::updateThemeIndicator(Wt::WText* indicator) {
    if (indicator) {
        auto it = std::find_if(availableThemes_.begin(), availableThemes_.end(),
            [this](const ThemeInfo& theme) { return theme.id == currentTheme_; });
        
        if (it != availableThemes_.end()) {
            indicator->setText("Current Theme: " + it->name);
        }
    }
}

void RestaurantPOSApp::triggerUpdate() {
    // Update theme indicator
    updateThemeIndicator(themeIndicator_);
    
    // Refresh all tables to apply new theme
    if (menuTable_) buildMenuTable();
    if (currentOrderTable_) updateCurrentOrderTable();
    if (activeOrdersTable_) updateActiveOrdersTable();
    if (kitchenStatusTable_) updateKitchenStatusTable();
}

void RestaurantPOSApp::initializeSampleMenu() {
    // Appetizers with emojis for visual appeal
    menuItems_.push_back(std::make_shared<MenuItem>(1, "🥗 Caesar Salad", 12.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(2, "🍗 Buffalo Wings", 14.99, MenuItem::APPETIZER));
    menuItems_.push_back(std::make_shared<MenuItem>(3, "🦑 Calamari Rings", 13.99, MenuItem::APPETIZER));
    
    // Main Courses
    menuItems_.push_back(std::make_shared<MenuItem>(4, "🐟 Grilled Salmon", 24.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(5, "🥩 Ribeye Steak", 32.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(6, "🍗 Chicken Parmesan", 19.99, MenuItem::MAIN_COURSE));
    menuItems_.push_back(std::make_shared<MenuItem>(7, "🍝 Pasta Primavera", 16.99, MenuItem::MAIN_COURSE));
    
    // Desserts
    menuItems_.push_back(std::make_shared<MenuItem>(8, "🍰 Chocolate Cake", 8.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(9, "🧁 Tiramisu", 9.99, MenuItem::DESSERT));
    menuItems_.push_back(std::make_shared<MenuItem>(10, "🍨 Ice Cream Sundae", 6.99, MenuItem::DESSERT));
    
    // Beverages
    menuItems_.push_back(std::make_shared<MenuItem>(11, "🍷 House Wine", 7.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(12, "🍺 Craft Beer", 5.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(13, "🥤 Soft Drink", 2.99, MenuItem::BEVERAGE));
    menuItems_.push_back(std::make_shared<MenuItem>(14, "🧃 Fresh Juice", 4.99, MenuItem::BEVERAGE));
    
    // Daily Special
    menuItems_.push_back(std::make_shared<MenuItem>(15, "👨‍🍳 Chef's Special", 28.99, MenuItem::SPECIAL));
}

std::unique_ptr<Wt::WWidget> RestaurantPOSApp::createOrderEntryPanel() {
    auto panel = std::make_unique<Wt::WContainerWidget>();
    panel->addStyleClass("p-3 border-end bg-light");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Header
    auto header = std::make_unique<Wt::WText>("Order Entry System");
    header->addStyleClass("h2 mb-4 text-primary");
    layout->addWidget(std::move(header));
    
    // Table selection section
    auto tableContainer = std::make_unique<Wt::WContainerWidget>();
    tableContainer->addStyleClass("mb-4 p-3 border rounded bg-white");
    auto tableLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto tableLabel = std::make_unique<Wt::WText>("Table Number: ");
    tableLabel->addStyleClass("h6 me-3");
    tableLayout->addWidget(std::move(tableLabel));
    
    tableNumberEdit_ = tableLayout->addWidget(std::make_unique<Wt::WSpinBox>());
    tableNumberEdit_->setMinimum(1);
    tableNumberEdit_->setMaximum(50);
    tableNumberEdit_->setValue(1);
    tableNumberEdit_->addStyleClass("form-control me-3");
    tableNumberEdit_->setWidth("100px");
    
    auto newOrderBtn = tableLayout->addWidget(std::make_unique<Wt::WPushButton>("Start New Order"));
    newOrderBtn->addStyleClass("btn btn-primary");
    newOrderBtn->clicked().connect([this] { startNewOrder(); });
    
    tableContainer->setLayout(std::move(tableLayout));
    layout->addWidget(std::move(tableContainer));
    
    // Menu items section
    auto menuHeader = std::make_unique<Wt::WText>("Menu Items");
    menuHeader->addStyleClass("h4 mb-3 text-secondary");
    layout->addWidget(std::move(menuHeader));
    
    menuTable_ = layout->addWidget(std::make_unique<Wt::WTable>());
    menuTable_->addStyleClass("table table-striped table-hover");
    buildMenuTable();
    
    // Current order section
    auto orderHeader = std::make_unique<Wt::WText>("Current Order");
    orderHeader->addStyleClass("h4 mt-4 mb-3 text-secondary");
    layout->addWidget(std::move(orderHeader));
    
    currentOrderTable_ = layout->addWidget(std::make_unique<Wt::WTable>());
    currentOrderTable_->addStyleClass("table table-sm table-bordered");
    updateCurrentOrderTable();
    
    // Order actions section
    auto actionContainer = std::make_unique<Wt::WContainerWidget>();
    actionContainer->addStyleClass("mt-3");
    auto actionLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto sendToKitchenBtn = actionLayout->addWidget(std::make_unique<Wt::WPushButton>("Send to Kitchen"));
    sendToKitchenBtn->addStyleClass("btn btn-success me-2");
    sendToKitchenBtn->clicked().connect([this] { sendCurrentOrderToKitchen(); });
    
    auto processPaymentBtn = actionLayout->addWidget(std::make_unique<Wt::WPushButton>("Process Payment"));
    processPaymentBtn->addStyleClass("btn btn-warning");
    processPaymentBtn->clicked().connect([this] { showPaymentDialog(); });
    
    actionContainer->setLayout(std::move(actionLayout));
    layout->addWidget(std::move(actionContainer));
    
    panel->setLayout(std::move(layout));
    return std::move(panel);
}

std::unique_ptr<Wt::WWidget> RestaurantPOSApp::createOrderStatusPanel() {
    auto panel = std::make_unique<Wt::WContainerWidget>();
    panel->addStyleClass("p-3 bg-light");
    
    // Create tab widget for different views
    auto tabWidget = std::make_unique<Wt::WTabWidget>();
    
    // Active Orders tab
    auto activeOrdersContainer = std::make_unique<Wt::WContainerWidget>();
    auto activeOrdersLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto activeHeader = std::make_unique<Wt::WText>("Active Orders");
    activeHeader->addStyleClass("h4 mb-3 text-primary");
    activeOrdersLayout->addWidget(std::move(activeHeader));
    
    activeOrdersTable_ = activeOrdersLayout->addWidget(std::make_unique<Wt::WTable>());
    activeOrdersTable_->addStyleClass("table table-hover table-sm");
    updateActiveOrdersTable();
    
    activeOrdersContainer->setLayout(std::move(activeOrdersLayout));
    tabWidget->addTab(std::move(activeOrdersContainer), "Active Orders", Wt::ContentLoading::Lazy);
    
    // Kitchen Status tab
    auto kitchenContainer = std::make_unique<Wt::WContainerWidget>();
    auto kitchenLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto kitchenHeader = std::make_unique<Wt::WText>("Kitchen Status");
    kitchenHeader->addStyleClass("h4 mb-3 text-primary");
    kitchenLayout->addWidget(std::move(kitchenHeader));
    
    kitchenStatusTable_ = kitchenLayout->addWidget(std::make_unique<Wt::WTable>());
    kitchenStatusTable_->addStyleClass("table table-hover table-sm");
    updateKitchenStatusTable();
    
    kitchenContainer->setLayout(std::move(kitchenLayout));
    tabWidget->addTab(std::move(kitchenContainer), "Kitchen Status", Wt::ContentLoading::Lazy);
    
    panel->addWidget(std::move(tabWidget));
    return std::move(panel);
}

void RestaurantPOSApp::buildMenuTable() {
    menuTable_->clear();
    
    // Headers
    menuTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    menuTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Category"));
    menuTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Price"));
    menuTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Action"));
    
    // Apply header styling
    for (int col = 0; col < 4; ++col) {
        menuTable_->elementAt(0, col)->addStyleClass("table-dark");
    }
    
    // Menu items grouped by category
    std::map<MenuItem::Category, std::vector<std::shared_ptr<MenuItem>>> itemsByCategory;
    for (const auto& item : menuItems_) {
        itemsByCategory[item->getCategory()].push_back(item);
    }
    
    int row = 1;
    for (const auto& categoryPair : itemsByCategory) {
        // Category header
        auto categoryText = std::make_unique<Wt::WText>(MenuItem::categoryToString(categoryPair.first));
        categoryText->addStyleClass("fw-bold text-primary");
        menuTable_->elementAt(row, 0)->addWidget(std::move(categoryText));
        menuTable_->elementAt(row, 0)->setColumnSpan(4);
        menuTable_->elementAt(row, 0)->addStyleClass("table-secondary");
        row++;
        
        // Items in category
        for (const auto& item : categoryPair.second) {
            menuTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(item->getName()));
            menuTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(MenuItem::categoryToString(item->getCategory())));
            menuTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(formatCurrency(item->getPrice())));
            
            auto addBtn = std::make_unique<Wt::WPushButton>("Add");
            addBtn->addStyleClass("btn btn-sm btn-outline-primary");
            addBtn->clicked().connect([this, item] { addItemToCurrentOrder(item); });
            menuTable_->elementAt(row, 3)->addWidget(std::move(addBtn));
            
            row++;
        }
    }
}

void RestaurantPOSApp::updateCurrentOrderTable() {
    currentOrderTable_->clear();
    
    if (!currentOrder_) {
        auto noOrderText = std::make_unique<Wt::WText>("No active order. Start a new order to begin.");
        noOrderText->addStyleClass("text-muted fst-italic");
        currentOrderTable_->elementAt(0, 0)->addWidget(std::move(noOrderText));
        return;
    }
    
    // Headers
    currentOrderTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Item"));
    currentOrderTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Qty"));
    currentOrderTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Price"));
    currentOrderTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));
    currentOrderTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Action"));
    
    // Apply header styling
    for (int col = 0; col < 5; ++col) {
        currentOrderTable_->elementAt(0, col)->addStyleClass("table-dark");
    }
    
    const auto& items = currentOrder_->getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        int row = i + 1;
        
        currentOrderTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(item.getMenuItem().getName()));
        currentOrderTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(std::to_string(item.getQuantity())));
        currentOrderTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(formatCurrency(item.getMenuItem().getPrice())));
        currentOrderTable_->elementAt(row, 3)->addWidget(std::make_unique<Wt::WText>(formatCurrency(item.getTotalPrice())));
        
        auto removeBtn = std::make_unique<Wt::WPushButton>("Remove");
        removeBtn->addStyleClass("btn btn-sm btn-outline-danger");
        removeBtn->clicked().connect([this, i] { removeItemFromCurrentOrder(i); });
        currentOrderTable_->elementAt(row, 4)->addWidget(std::move(removeBtn));
    }
    
    // Totals section
    int totalRow = items.size() + 1;
    currentOrderTable_->elementAt(totalRow, 0)->addWidget(std::make_unique<Wt::WText>("Subtotal:"));
    currentOrderTable_->elementAt(totalRow, 3)->addWidget(std::make_unique<Wt::WText>(formatCurrency(currentOrder_->getSubtotal())));
    
    totalRow++;
    currentOrderTable_->elementAt(totalRow, 0)->addWidget(std::make_unique<Wt::WText>("Tax:"));
    currentOrderTable_->elementAt(totalRow, 3)->addWidget(std::make_unique<Wt::WText>(formatCurrency(currentOrder_->getTax())));
    
    totalRow++;
    auto totalLabel = std::make_unique<Wt::WText>("Total:");
    totalLabel->addStyleClass("fw-bold");
    currentOrderTable_->elementAt(totalRow, 0)->addWidget(std::move(totalLabel));
    auto totalAmount = std::make_unique<Wt::WText>(formatCurrency(currentOrder_->getTotal()));
    totalAmount->addStyleClass("fw-bold currency");
    currentOrderTable_->elementAt(totalRow, 3)->addWidget(std::move(totalAmount));
}

void RestaurantPOSApp::updateActiveOrdersTable() {
    activeOrdersTable_->clear();
    
    auto orders = orderManager_->getActiveOrders();
    
    if (orders.empty()) {
        auto noOrdersText = std::make_unique<Wt::WText>("No active orders.");
        noOrdersText->addStyleClass("text-muted fst-italic");
        activeOrdersTable_->elementAt(0, 0)->addWidget(std::move(noOrdersText));
        return;
    }
    
    // Headers
    activeOrdersTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Order #"));
    activeOrdersTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Table"));
    activeOrdersTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Status"));
    activeOrdersTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));
    activeOrdersTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Items"));
    
    // Apply header styling
    for (int col = 0; col < 5; ++col) {
        activeOrdersTable_->elementAt(0, col)->addStyleClass("table-dark");
    }
    
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& order = orders[i];
        int row = i + 1;
        
        activeOrdersTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(std::to_string(order->getOrderId())));
        activeOrdersTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(std::to_string(order->getTableNumber())));
        activeOrdersTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(getStatusString(order->getStatus())));
        activeOrdersTable_->elementAt(row, 3)->addWidget(std::make_unique<Wt::WText>(formatCurrency(order->getTotal())));
        activeOrdersTable_->elementAt(row, 4)->addWidget(std::make_unique<Wt::WText>(std::to_string(order->getItems().size()) + " items"));
    }
}

void RestaurantPOSApp::updateKitchenStatusTable() {
    kitchenStatusTable_->clear();
    
    auto tickets = kitchenInterface_->getActiveTickets();
    
    if (tickets.empty()) {
        auto noTicketsText = std::make_unique<Wt::WText>("No orders in kitchen queue.");
        noTicketsText->addStyleClass("text-muted fst-italic");
        kitchenStatusTable_->elementAt(0, 0)->addWidget(std::move(noTicketsText));
        return;
    }
    
    // Headers
    kitchenStatusTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Order #"));
    kitchenStatusTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Table"));
    kitchenStatusTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Kitchen Status"));
    kitchenStatusTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Est. Time"));
    kitchenStatusTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));
    
    // Apply header styling
    for (int col = 0; col < 5; ++col) {
        kitchenStatusTable_->elementAt(0, col)->addStyleClass("table-dark");
    }
    
    for (size_t i = 0; i < tickets.size(); ++i) {
        const auto& ticket = tickets[i];
        int row = i + 1;
        
        kitchenStatusTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(std::to_string(ticket.orderId)));
        kitchenStatusTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(std::to_string(ticket.tableNumber)));
        kitchenStatusTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(getKitchenStatusString(ticket.status)));
        kitchenStatusTable_->elementAt(row, 3)->addWidget(std::make_unique<Wt::WText>(std::to_string(ticket.estimatedPrepTime) + " min"));
        
        // Status update buttons
        auto buttonContainer = std::make_unique<Wt::WContainerWidget>();
        auto buttonLayout = std::make_unique<Wt::WHBoxLayout>();
        
        if (ticket.status == KitchenInterface::ORDER_RECEIVED) {
            auto startBtn = std::make_unique<Wt::WPushButton>("Start");
            startBtn->addStyleClass("btn btn-sm btn-success me-1");
            startBtn->clicked().connect([this, ticket] { 
                updateOrderKitchenStatus(ticket.orderId, KitchenInterface::PREP_STARTED); 
            });
            buttonLayout->addWidget(std::move(startBtn));
        }
        
        if (ticket.status == KitchenInterface::PREP_STARTED) {
            auto readyBtn = std::make_unique<Wt::WPushButton>("Ready");
            readyBtn->addStyleClass("btn btn-sm btn-warning me-1");
            readyBtn->clicked().connect([this, ticket] { 
                updateOrderKitchenStatus(ticket.orderId, KitchenInterface::READY_FOR_PICKUP); 
            });
            buttonLayout->addWidget(std::move(readyBtn));
        }
        
        if (ticket.status == KitchenInterface::READY_FOR_PICKUP) {
            auto servedBtn = std::make_unique<Wt::WPushButton>("Served");
            servedBtn->addStyleClass("btn btn-sm btn-primary me-1");
            servedBtn->clicked().connect([this, ticket] { 
                updateOrderKitchenStatus(ticket.orderId, KitchenInterface::SERVED); 
            });
            buttonLayout->addWidget(std::move(servedBtn));
        }
        
        buttonContainer->setLayout(std::move(buttonLayout));
        kitchenStatusTable_->elementAt(row, 4)->addWidget(std::move(buttonContainer));
    }
}

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
    if (type == "error") {
        auto messageBox = addChild(std::make_unique<Wt::WMessageBox>("Error", message, Wt::Icon::Critical, Wt::StandardButton::Ok));
        messageBox->show();
    } else if (type == "warning") {
        auto messageBox = addChild(std::make_unique<Wt::WMessageBox>("Warning", message, Wt::Icon::Warning, Wt::StandardButton::Ok));
        messageBox->show();
    } else {
        // For info and success, show in console for now
        std::cout << "Notification (" << type << "): " << message << std::endl;
    }
}

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}
