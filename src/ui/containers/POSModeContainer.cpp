//============================================================================
// POSModeContainer Implementation
//============================================================================

#include "../../../include/ui/containers/POSModeContainer.hpp"

#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>
#include <iostream>

POSModeContainer::POSModeContainer(std::shared_ptr<POSService> posService,
                                  std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , leftPanel_(nullptr)
    , rightPanel_(nullptr)
    , workArea_(nullptr)
    , activeOrdersDisplay_(nullptr)
    , orderEntryPanel_(nullptr)
    , menuDisplay_(nullptr)
    , currentOrderDisplay_(nullptr)
    , workAreaTitle_(nullptr)
    , orderEntryArea_(nullptr)
    , orderEditArea_(nullptr)
{
    initializeUI();
    setupEventListeners();
}

void POSModeContainer::initializeUI() {
    addStyleClass("pos-mode-container h-100");
    setupLayout();
    createLeftPanel();
    createRightPanel();
}

void POSModeContainer::setupLayout() {
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    // Left panel (40% width) - Active Orders
    leftPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    leftPanel_->addStyleClass("bg-white rounded shadow-sm p-3");
    leftPanel_->setWidth(Wt::WLength(40, Wt::WLength::Unit::Percentage));
    
    // Right panel (60% width) - Order Work Area
    rightPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightPanel_->addStyleClass("bg-white rounded shadow-sm p-3");
    rightPanel_->setWidth(Wt::WLength(60, Wt::WLength::Unit::Percentage));
}

void POSModeContainer::createLeftPanel() {
    auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    // Panel title
    auto title = leftLayout->addWidget(
        std::make_unique<Wt::WText>("📋 Active Orders")
    );
    title->addStyleClass("h5 mb-3 text-primary fw-bold");
    
    // Active orders display
    activeOrdersDisplay_ = leftLayout->addWidget(
        std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_)
    );
    
    std::cout << "✓ POS Mode: Left panel (Active Orders) created" << std::endl;
}

void POSModeContainer::createRightPanel() {
    auto rightLayout = rightPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    rightLayout->setContentsMargins(0, 0, 0, 0);
    
    // Work area title
    workAreaTitle_ = rightLayout->addWidget(
        std::make_unique<Wt::WText>("🍽️ Order Work Area")
    );
    workAreaTitle_->addStyleClass("h5 mb-3 text-primary fw-bold");
    
    // Work area container
    workArea_ = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    workArea_->addStyleClass("flex-grow-1");
    
    // Create order entry area (shown when no current order)
    createOrderEntryArea();
    
    // Create order edit area (shown when editing an order)
    createOrderEditArea();
    
    // Initially show order entry
    updateWorkArea();
    
    std::cout << "✓ POS Mode: Right panel (Work Area) created" << std::endl;
}

void POSModeContainer::createOrderEntryArea() {
    orderEntryArea_ = workArea_->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto entryLayout = orderEntryArea_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    
    // Welcome message
    auto welcomeText = entryLayout->addWidget(
        std::make_unique<Wt::WText>(
            "<h6>👋 Welcome to POS Mode</h6>"
            "<p class='text-muted'>Create a new order or select an existing order from the Active Orders list to begin.</p>"
        )
    );
    welcomeText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Order entry panel
    orderEntryPanel_ = entryLayout->addWidget(
        std::make_unique<OrderEntryPanel>(posService_, eventManager_)
    );
    
    // Action buttons
    auto buttonContainer = entryLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonContainer->addStyleClass("d-flex gap-2 mt-3");
    
    newOrderButton_ = buttonContainer->addWidget(
        std::make_unique<Wt::WPushButton>("🆕 Create New Order")
    );
    newOrderButton_->addStyleClass("btn btn-primary btn-lg");
    newOrderButton_->clicked().connect([this]() {
        createNewOrder("table 1"); // Default table for demo
    });
}

void POSModeContainer::createOrderEditArea() {
    orderEditArea_ = workArea_->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto editLayout = orderEditArea_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    
    // Order edit header
    auto headerContainer = editLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->addStyleClass("d-flex justify-content-between align-items-center mb-3");
    
    auto editTitle = headerContainer->addWidget(
        std::make_unique<Wt::WText>("✏️ Editing Order")
    );
    editTitle->addStyleClass("h6 mb-0 text-success");
    
    closeOrderButton_ = headerContainer->addWidget(
        std::make_unique<Wt::WPushButton>("❌ Close Order")
    );
    closeOrderButton_->addStyleClass("btn btn-outline-secondary btn-sm");
    closeOrderButton_->clicked().connect([this]() {
        closeCurrentOrder();
    });
    
    // Create split layout for menu and current order
    auto contentContainer = editLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentContainer->addStyleClass("flex-grow-1");
    auto contentLayout = contentContainer->setLayout(std::make_unique<Wt::WHBoxLayout>());
    contentLayout->setSpacing(15);
    
    // Menu display (left side)
    auto menuContainer = contentLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    menuContainer->addStyleClass("border rounded p-3");
    menuContainer->setWidth(Wt::WLength(60, Wt::WLength::Unit::Percentage));
    
    auto menuTitle = menuContainer->addWidget(
        std::make_unique<Wt::WText>("📖 Menu")
    );
    menuTitle->addStyleClass("h6 mb-3 text-primary");
    
    menuDisplay_ = menuContainer->addWidget(
        std::make_unique<MenuDisplay>(posService_, eventManager_)
    );
    
    // Current order display (right side)
    auto orderContainer = contentLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    orderContainer->addStyleClass("border rounded p-3");
    orderContainer->setWidth(Wt::WLength(40, Wt::WLength::Unit::Percentage));
    
    auto orderTitle = orderContainer->addWidget(
        std::make_unique<Wt::WText>("🧾 Current Order")
    );
    orderTitle->addStyleClass("h6 mb-3 text-primary");
    
    currentOrderDisplay_ = orderContainer->addWidget(
        std::make_unique<CurrentOrderDisplay>(posService_, eventManager_)
    );
    
    // Initially hide order edit area
    orderEditArea_->hide();
}

void POSModeContainer::updateWorkArea() {
    bool hasOrder = hasCurrentOrder();
    
    if (hasOrder) {
        showOrderEdit();
    } else {
        showOrderEntry();
    }
}

void POSModeContainer::showOrderEntry() {
    orderEditArea_->hide();
    orderEntryArea_->show();
    workAreaTitle_->setText("🍽️ Order Work Area - Ready for New Order");
}

void POSModeContainer::showOrderEdit() {
    orderEntryArea_->hide();
    orderEditArea_->show();
    
    auto currentOrder = posService_->getCurrentOrder();
    if (currentOrder) {
        workAreaTitle_->setText("✏️ Editing Order #" + std::to_string(currentOrder->getOrderId()) + 
                               " - " + currentOrder->getTableIdentifier());
    }
}

void POSModeContainer::setupEventListeners() {
    // Listen for order creation
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED, [this](const std::any& data) {
            handleOrderCreated(data);
        })
    );
    
    // Listen for current order changes
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED, [this](const std::any& data) {
            handleCurrentOrderChanged(data);
        })
    );
    
    std::cout << "✓ POS Mode: Event listeners configured" << std::endl;
}

void POSModeContainer::createNewOrder(const std::string& tableIdentifier) {
    auto order = posService_->createOrder(tableIdentifier);
    if (order) {
        posService_->setCurrentOrder(order);
        std::cout << "📝 Created new order #" << order->getOrderId() 
                  << " for " << tableIdentifier << std::endl;
    }
}

void POSModeContainer::openOrderForEditing(std::shared_ptr<Order> order) {
    if (order) {
        posService_->setCurrentOrder(order);
        std::cout << "✏️ Opened order #" << order->getOrderId() << " for editing" << std::endl;
    }
}

void POSModeContainer::closeCurrentOrder() {
    posService_->setCurrentOrder(nullptr);
    std::cout << "❌ Closed current order" << std::endl;
}

bool POSModeContainer::hasCurrentOrder() const {
    return posService_->getCurrentOrder() != nullptr;
}

void POSModeContainer::refresh() {
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
    
    if (hasCurrentOrder()) {
        if (menuDisplay_) menuDisplay_->refresh();
        if (currentOrderDisplay_) currentOrderDisplay_->refresh();
    }
}

void POSModeContainer::handleOrderCreated(const std::any& eventData) {
    updateWorkArea();
    refresh();
}

void POSModeContainer::handleCurrentOrderChanged(const std::any& eventData) {
    updateWorkArea();
}
