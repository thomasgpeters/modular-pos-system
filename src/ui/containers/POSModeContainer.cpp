#include "../../../include/ui/containers/POSModeContainer.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGroupBox.h>
#include <iostream>

POSModeContainer::POSModeContainer(std::shared_ptr<POSService> posService,
                                  std::shared_ptr<EventManager> eventManager)
    : Wt::WContainerWidget()
    , posService_(posService)
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
    , newOrderButton_(nullptr)
    , closeOrderButton_(nullptr)
{
    if (!posService_ || !eventManager_) {
        throw std::invalid_argument("POSModeContainer requires valid POSService and EventManager");
    }
    
    setStyleClass("pos-mode-container h-100");
    
    initializeUI();
    setupEventListeners();
    updateWorkArea();
    
    std::cout << "[POSModeContainer] Initialized with smart Active Orders toggle" << std::endl;
}

void POSModeContainer::initializeUI() {
    setupLayout();
    createLeftPanel();
    createRightPanel();
    createOrderEntryArea();
    createOrderEditArea();
}

void POSModeContainer::setupLayout() {
    // Create main horizontal layout
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    // Create panels
    leftPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    // Initial layout proportions
    // When both panels visible: 30% left (orders), 70% right (work area)
    // When left panel hidden: 0% left, 100% right (full screen for work)
    layout->addWidget(leftPanel_, 3);   // 30%
    layout->addWidget(rightPanel_, 7);  // 70%
    
    leftPanel_->setStyleClass("pos-left-panel");
    rightPanel_->setStyleClass("pos-right-panel");
}

void POSModeContainer::createLeftPanel() {
    // Left panel contains Active Orders Display
    leftPanel_->setStyleClass("border-end bg-light p-3");
    
    auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    
    // Create Active Orders Display
    activeOrdersDisplay_ = leftLayout->addWidget(
        std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_));
    
    std::cout << "[POSModeContainer] Left panel (Active Orders) created" << std::endl;
}

void POSModeContainer::createRightPanel() {
    // Right panel contains the work area
    rightPanel_->setStyleClass("pos-work-panel p-3");
    
    auto rightLayout = rightPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);
    
    // Work area title and controls
    auto headerContainer = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->setStyleClass("d-flex justify-content-between align-items-center mb-3");
    
    workAreaTitle_ = headerContainer->addNew<Wt::WText>("🍽️ Order Management");
    workAreaTitle_->setStyleClass("h4 text-primary mb-0");
    
    // Control buttons container
    auto controlsContainer = headerContainer->addNew<Wt::WContainerWidget>();
    controlsContainer->setStyleClass("d-flex gap-2");
    
    // Toggle Active Orders button (shows when orders display is hidden)
    auto toggleOrdersButton = controlsContainer->addNew<Wt::WPushButton>("📋 Show Orders");
    toggleOrdersButton->setStyleClass("btn btn-outline-info btn-sm");
    toggleOrdersButton->clicked().connect([this]() {
        showActiveOrdersDisplay();
    });
    toggleOrdersButton->setId("toggle-orders-button");
    toggleOrdersButton->hide(); // Hidden initially when orders display is visible
    
    // Close current order button (shows when editing an order)
    closeOrderButton_ = controlsContainer->addNew<Wt::WPushButton>("❌ Close Order");
    closeOrderButton_->setStyleClass("btn btn-outline-secondary btn-sm");
    closeOrderButton_->clicked().connect([this]() {
        closeCurrentOrder();
    });
    closeOrderButton_->hide(); // Hidden initially
    
    // Work area (changes based on state)
    workArea_ = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 1);
    workArea_->setStyleClass("pos-dynamic-work-area");
    
    std::cout << "[POSModeContainer] Right panel (Work Area) created" << std::endl;
}

void POSModeContainer::createOrderEntryArea() {
    // Order entry area - shown when no current order
    orderEntryArea_ = std::make_unique<Wt::WContainerWidget>();
    orderEntryArea_->setStyleClass("order-entry-area");
    
    auto layout = orderEntryArea_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);
    
    // Welcome message
    auto welcomeText = layout->addWidget(std::make_unique<Wt::WText>(
        "Welcome to the Restaurant POS System"));
    welcomeText->setStyleClass("h5 text-center text-muted mb-4");
    
    // Order entry panel
    orderEntryPanel_ = layout->addWidget(
        std::make_unique<OrderEntryPanel>(posService_, eventManager_));
    
    // Instructions
    auto instructionsText = layout->addWidget(std::make_unique<Wt::WText>(
        "💡 Select a table/location and click 'Start New Order' to begin"));
    instructionsText->setStyleClass("text-center text-muted small");
    
    std::cout << "[POSModeContainer] Order entry area created" << std::endl;
}

void POSModeContainer::createOrderEditArea() {
    // Order edit area - shown when editing an order
    orderEditArea_ = std::make_unique<Wt::WContainerWidget>();
    orderEditArea_->setStyleClass("order-edit-area");
    
    auto layout = orderEditArea_->setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);
    
    // Left side: Menu Display (takes more space when active orders are hidden)
    auto menuContainer = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    menuContainer->setStyleClass("menu-section");
    
    auto menuLayout = menuContainer->setLayout(std::make_unique<Wt::WVBoxLayout>());
    menuLayout->setContentsMargins(0, 0, 0, 0);
    menuLayout->setSpacing(10);
    
    menuDisplay_ = menuLayout->addWidget(
        std::make_unique<MenuDisplay>(posService_, eventManager_));
    
    // Right side: Current Order Display
    auto orderContainer = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    orderContainer->setStyleClass("current-order-section");
    
    auto orderLayout = orderContainer->setLayout(std::make_unique<Wt::WVBoxLayout>());
    orderLayout->setContentsMargins(0, 0, 0, 0);
    orderLayout->setSpacing(10);
    
    currentOrderDisplay_ = orderLayout->addWidget(
        std::make_unique<CurrentOrderDisplay>(posService_, eventManager_));
    
    // Set layout proportions: Menu gets more space (65%), Current Order (35%)
    layout->addWidget(menuContainer, 65);
    layout->addWidget(orderContainer, 35);
    
    std::cout << "[POSModeContainer] Order edit area created" << std::endl;
}

void POSModeContainer::setupEventListeners() {
    if (!eventManager_) return;
    
    // Listen for current order changes to toggle display visibility
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { handleCurrentOrderChanged(data); })
    );
    
    // Listen for order creation
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED,
            [this](const std::any& data) { handleOrderCreated(data); })
    );
    
    std::cout << "[POSModeContainer] Event listeners set up" << std::endl;
}

void POSModeContainer::updateWorkArea() {
    bool hasCurrentOrder = this->hasCurrentOrder();
    
    std::cout << "[POSModeContainer] Updating work area - hasCurrentOrder: " << hasCurrentOrder << std::endl;
    
    // Update title
    if (workAreaTitle_) {
        if (hasCurrentOrder) {
            auto order = posService_->getCurrentOrder();
            workAreaTitle_->setText("🍽️ Editing Order #" + std::to_string(order->getOrderId()) + 
                                   " - " + order->getTableIdentifier());
        } else {
            workAreaTitle_->setText("🍽️ Order Management");
        }
    }
    
    // Update close button visibility
    if (closeOrderButton_) {
        if (hasCurrentOrder) {
            closeOrderButton_->show();
        } else {
            closeOrderButton_->hide();
        }
    }
    
    // Clear work area and add appropriate content
    workArea_->clear();
    
    if (hasCurrentOrder) {
        showOrderEdit();
        hideActiveOrdersDisplay(); // Hide orders list when editing
    } else {
        showOrderEntry();
        showActiveOrdersDisplay();  // Show orders list when not editing
    }
}

void POSModeContainer::showOrderEntry() {
    if (orderEntryArea_) {
        workArea_->addWidget(std::unique_ptr<Wt::WWidget>(orderEntryArea_.release()));
        orderEntryArea_ = nullptr; // Transfer ownership
    }
    std::cout << "[POSModeContainer] Showing order entry area" << std::endl;
}

void POSModeContainer::showOrderEdit() {
    if (orderEditArea_) {
        workArea_->addWidget(std::unique_ptr<Wt::WWidget>(orderEditArea_.release()));
        orderEditArea_ = nullptr; // Transfer ownership
    }
    std::cout << "[POSModeContainer] Showing order edit area" << std::endl;
}

void POSModeContainer::hideActiveOrdersDisplay() {
    if (!leftPanel_) return;
    
    std::cout << "[POSModeContainer] Hiding Active Orders Display - giving more space to work area" << std::endl;
    
    // Hide the left panel
    leftPanel_->hide();
    
    // Update layout to give right panel full width
    auto layout = dynamic_cast<Wt::WHBoxLayout*>(this->layout());
    if (layout) {
        // Temporarily store the current stretch factors
        layout->setStretchFactor(leftPanel_, 0);   // 0% - hidden
        layout->setStretchFactor(rightPanel_, 1);  // 100% - full width
    }
    
    // Show the toggle button to bring orders back
    auto toggleButton = findWidget("toggle-orders-button");
    if (toggleButton) {
        toggleButton->show();
    }
    
    // Update styling for full-width mode
    rightPanel_->addStyleClass("pos-full-width-mode");
}

void POSModeContainer::showActiveOrdersDisplay() {
    if (!leftPanel_) return;
    
    std::cout << "[POSModeContainer] Showing Active Orders Display - restoring split layout" << std::endl;
    
    // Show the left panel
    leftPanel_->show();
    
    // Restore layout proportions
    auto layout = dynamic_cast<Wt::WHBoxLayout*>(this->layout());
    if (layout) {
        layout->setStretchFactor(leftPanel_, 3);   // 30%
        layout->setStretchFactor(rightPanel_, 7);  // 70%
    }
    
    // Hide the toggle button
    auto toggleButton = findWidget("toggle-orders-button");
    if (toggleButton) {
        toggleButton->hide();
    }
    
    // Remove full-width styling
    rightPanel_->removeStyleClass("pos-full-width-mode");
    
    // Refresh the active orders to ensure current data
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
}

// Event handlers
void POSModeContainer::handleCurrentOrderChanged(const std::any& eventData) {
    std::cout << "[POSModeContainer] Current order changed - updating work area" << std::endl;
    updateWorkArea();
}

void POSModeContainer::handleOrderCreated(const std::any& eventData) {
    std::cout << "[POSModeContainer] Order created - updating work area" << std::endl;
    updateWorkArea();
}

// Public interface methods
void POSModeContainer::refresh() {
    updateWorkArea();
    
    // Refresh all components
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
    if (orderEntryPanel_) {
        orderEntryPanel_->refresh();
    }
    if (menuDisplay_) {
        menuDisplay_->refresh();
    }
    if (currentOrderDisplay_) {
        currentOrderDisplay_->refresh();
    }
    
    std::cout << "[POSModeContainer] All components refreshed" << std::endl;
}

void POSModeContainer::createNewOrder(const std::string& tableIdentifier) {
    if (posService_) {
        auto order = posService_->createOrder(tableIdentifier);
        if (order) {
            posService_->setCurrentOrder(order);
            std::cout << "[POSModeContainer] New order created: #" << order->getOrderId() << std::endl;
        }
    }
}

void POSModeContainer::openOrderForEditing(std::shared_ptr<Order> order) {
    if (order && posService_) {
        posService_->setCurrentOrder(order);
        std::cout << "[POSModeContainer] Order #" << order->getOrderId() << " opened for editing" << std::endl;
    }
}

void POSModeContainer::closeCurrentOrder() {
    std::cout << "[POSModeContainer] Closing current order - returning to order selection" << std::endl;
    
    if (posService_) {
        // Clear the current order (but don't delete it - it stays in active orders)
        posService_->setCurrentOrder(nullptr);
    }
    
    // This will trigger the CURRENT_ORDER_CHANGED event, which will:
    // 1. Show the Active Orders Display again
    // 2. Show the Order Entry area
    // 3. Hide the Close Order button
    
    // Publish the event manually if needed
    if (eventManager_) {
        auto orderChangedEvent = POSEvents::createCurrentOrderChangedData(
            nullptr, nullptr, "closed");
        eventManager_->publish(POSEvents::CURRENT_ORDER_CHANGED, orderChangedEvent);
    }
}

bool POSModeContainer::hasCurrentOrder() const {
    return posService_ && posService_->getCurrentOrder() != nullptr;
}

// Utility method to safely transfer widget ownership back to unique_ptr
template<typename T>
std::unique_ptr<T> POSModeContainer::extractWidget(T* widget) {
    if (!widget) return nullptr;
    
    // Remove from parent if it has one
    if (widget->parent()) {
        widget->parent()->removeWidget(widget);
    }
    
    // Return as unique_ptr (assuming we have ownership)
    return std::unique_ptr<T>(widget);
}

// Store widgets when switching between areas
void POSModeContainer::storeCurrentWorkArea() {
    if (!workArea_ || workArea_->children().empty()) return;
    
    // Store the current work area content
    auto currentWidget = workArea_->children()[0];
    
    if (dynamic_cast<Wt::WContainerWidget*>(currentWidget)) {
        // Figure out which area this is and store it appropriately
        bool isOrderEntry = (orderEntryPanel_ && 
                           currentWidget->findWidget(orderEntryPanel_));
        
        if (isOrderEntry && !orderEntryArea_) {
            orderEntryArea_ = extractWidget(
                dynamic_cast<Wt::WContainerWidget*>(currentWidget));
        } else if (!isOrderEntry && !orderEditArea_) {
            orderEditArea_ = extractWidget(
                dynamic_cast<Wt::WContainerWidget*>(currentWidget));
        }
    }
}
