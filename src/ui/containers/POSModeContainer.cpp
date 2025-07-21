#include "../../../include/ui/containers/POSModeContainer.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WApplication.h>
#include <Wt/WTimer.h>
#include <iostream>
#include <thread>
#include <chrono>

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
    , sendToKitchenButton_(nullptr)  
    , toggleOrdersButton_(nullptr)
    , currentUIMode_(UI_MODE_NONE)  
    , isDestroying_(false)  
{
    if (!posService_ || !eventManager_) {
        throw std::invalid_argument("POSModeContainer requires valid POSService and EventManager");
    }
    
    setStyleClass("pos-mode-container h-100");
    
    initializeUI();
    setupEventListeners();
    updateWorkArea();
    
    std::cout << "[POSModeContainer] Initialized with corrected layout management" << std::endl;
}

POSModeContainer::~POSModeContainer() {
    std::cout << "[POSModeContainer] Destructor called - cleaning up" << std::endl;
    
    // Set destruction flag to prevent further operations
    isDestroying_ = true;
    
    try {
        // Clear component pointers first
        orderEntryPanel_ = nullptr;
        menuDisplay_ = nullptr;
        currentOrderDisplay_ = nullptr;
        
        // Unsubscribe from all events
        if (eventManager_) {
            for (auto handle : eventSubscriptions_) {
                eventManager_->unsubscribe(handle);
            }
            eventSubscriptions_.clear();
        }
        
        std::cout << "[POSModeContainer] Cleanup completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error during destruction: " << e.what() << std::endl;
    }
}

void POSModeContainer::initializeUI() {
    setupLayout();
    createLeftPanel();
    createRightPanel();
}

void POSModeContainer::setupLayout() {
    // Create main horizontal layout
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    // Create panels as children first, then set stretch factors
    leftPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    // Set stretch factors: Left 60%, Right 40%
    layout->setStretchFactor(leftPanel_, 6);   // 60% - for Active Orders or Menu Display
    layout->setStretchFactor(rightPanel_, 4);  // 40% - for Order Entry or Current Order
    
    leftPanel_->setStyleClass("pos-left-panel");
    rightPanel_->setStyleClass("pos-right-panel");
    
    std::cout << "[POSModeContainer] Layout setup - Left: 60%, Right: 40%" << std::endl;
}

void POSModeContainer::createLeftPanel() {
    // Left panel will contain either Active Orders Display or Menu Display
    leftPanel_->setStyleClass("border-end bg-light");
    
    auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    
    std::cout << "[POSModeContainer] Left panel container created (will be populated dynamically)" << std::endl;
}

void POSModeContainer::createRightPanel() {
    // Right panel contains the work area and controls
    rightPanel_->setStyleClass("pos-work-panel bg-white");
    
    auto rightLayout = rightPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    rightLayout->setContentsMargins(15, 15, 15, 15);
    rightLayout->setSpacing(15);
    
    // Work area title and controls
    auto headerContainer = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->setStyleClass("d-flex justify-content-between align-items-center mb-3");
    
    workAreaTitle_ = headerContainer->addNew<Wt::WText>("🍽️ Order Management");
    workAreaTitle_->setStyleClass("h4 text-primary mb-0");
    
    // Control buttons container
    auto controlsContainer = headerContainer->addNew<Wt::WContainerWidget>();
    controlsContainer->setStyleClass("d-flex gap-2");
    
    // Toggle Active Orders button (shown when orders are hidden)
    toggleOrdersButton_ = controlsContainer->addNew<Wt::WPushButton>("📋 Show Orders");
    toggleOrdersButton_->setStyleClass("btn btn-outline-info btn-sm");
    toggleOrdersButton_->clicked().connect([this]() {
        showActiveOrdersDisplay();
    });
    toggleOrdersButton_->hide(); // Hidden initially when orders display is visible
    
    // Send to Kitchen button
    sendToKitchenButton_ = controlsContainer->addNew<Wt::WPushButton>("🚀 Send to Kitchen");
    sendToKitchenButton_->setStyleClass("btn btn-success btn-sm");
    sendToKitchenButton_->clicked().connect([this]() {
        sendCurrentOrderToKitchen();
    });
    sendToKitchenButton_->hide(); // Hidden initially
    
    // Work area (changes based on state)
    workArea_ = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 1);
    workArea_->setStyleClass("pos-dynamic-work-area");
    
    std::cout << "[POSModeContainer] Right panel (Work Area) created with controls" << std::endl;
}

void POSModeContainer::setupEventListeners() {
    if (!eventManager_) return;
    
    // Listen for current order changes to toggle display visibility
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { 
                if (isDestroying_) return;
                
                try {
                    handleCurrentOrderChanged(data); 
                } catch (const std::exception& e) {
                    std::cerr << "[POSModeContainer] Error handling current order changed: " << e.what() << std::endl;
                }
            })
    );
    
    // Listen for order creation
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED,
            [this](const std::any& data) { 
                if (isDestroying_) return;
                
                try {
                    handleOrderCreated(data); 
                } catch (const std::exception& e) {
                    std::cerr << "[POSModeContainer] Error handling order created: " << e.what() << std::endl;
                }
            })
    );
    
    // Listen for order modifications to update Send to Kitchen button
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_MODIFIED,
            [this](const std::any& data) { 
                if (isDestroying_) return;
                
                try {
                    updateSendToKitchenButton();
                } catch (const std::exception& e) {
                    std::cerr << "[POSModeContainer] Error updating Send to Kitchen button: " << e.what() << std::endl;
                }
            })
    );
    
    std::cout << "[POSModeContainer] Event listeners set up with safety checks" << std::endl;
}

void POSModeContainer::updateWorkArea() {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping updateWorkArea - container is being destroyed" << std::endl;
        return;
    }
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    UIMode targetMode = hasCurrentOrder ? UI_MODE_ORDER_EDIT : UI_MODE_ORDER_ENTRY;
    
    std::cout << "[POSModeContainer] Updating work area - hasCurrentOrder: " << hasCurrentOrder 
              << ", currentUIMode: " << currentUIMode_ << ", targetMode: " << targetMode << std::endl;
    
    // Update title
    if (workAreaTitle_) {
        try {
            if (hasCurrentOrder) {
                auto order = posService_->getCurrentOrder();
                if (order) {
                    workAreaTitle_->setText("🍽️ Editing Order #" + std::to_string(order->getOrderId()) + 
                                           " - " + order->getTableIdentifier());
                }
            } else {
                workAreaTitle_->setText("🍽️ Order Management");
            }
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error updating title: " << e.what() << std::endl;
        }
    }
    
    // Update Send to Kitchen button
    updateSendToKitchenButton();
    
    // Only recreate UI if the mode has actually changed
    if (currentUIMode_ != targetMode) {
        std::cout << "[POSModeContainer] UI mode change detected - recreating interface" << std::endl;
        
        // Clear component pointers BEFORE destroying widgets
        orderEntryPanel_ = nullptr;
        menuDisplay_ = nullptr;
        currentOrderDisplay_ = nullptr;
        activeOrdersDisplay_ = nullptr;
        
        // Process any pending events and clear widgets safely
        try {
            Wt::WApplication::instance()->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            // Clear both panels
            leftPanel_->clear();
            workArea_->clear();
            
            std::cout << "[POSModeContainer] Panels cleared safely" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error clearing panels: " << e.what() << std::endl;
        }
        
        // Check again if we're still valid before creating new widgets
        if (isDestroying_) {
            std::cout << "[POSModeContainer] Container destroyed during update - aborting" << std::endl;
            return;
        }
        
        if (hasCurrentOrder) {
            showOrderEdit();
        } else {
            showOrderEntry();
        }
        
        currentUIMode_ = targetMode;
    } else {
        std::cout << "[POSModeContainer] UI mode unchanged - skipping recreation" << std::endl;
    }
}

void POSModeContainer::showOrderEntry() {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping showOrderEntry - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Creating order entry mode layout" << std::endl;
    
    // LEFT PANEL: Active Orders Display
    try {
        activeOrdersDisplay_ = leftPanel_->addNew<ActiveOrdersDisplay>(posService_, eventManager_);
        activeOrdersDisplay_->addStyleClass("h-100");
        std::cout << "[POSModeContainer] ✓ Active Orders Display created in left panel" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error creating Active Orders Display: " << e.what() << std::endl;
        auto fallbackText = leftPanel_->addNew<Wt::WText>("❌ Unable to load active orders");
        fallbackText->addStyleClass("text-center text-danger p-4");
    }
    
    // RIGHT PANEL: Order Entry Panel
    try {
        orderEntryPanel_ = workArea_->addNew<OrderEntryPanel>(posService_, eventManager_);
        orderEntryPanel_->addStyleClass("h-100");
        
        // Add instructions
        auto instructionsText = workArea_->addNew<Wt::WText>(
            "💡 Select a table/location and click 'Start New Order' to begin");
        instructionsText->setStyleClass("text-center text-muted small mt-3");
        
        std::cout << "[POSModeContainer] ✓ Order Entry Panel created in right panel" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error creating Order Entry Panel: " << e.what() << std::endl;
        auto fallbackText = workArea_->addNew<Wt::WText>("❌ Unable to load order entry");
        fallbackText->addStyleClass("text-center text-danger p-4");
    }
    
    // Show toggle button (hidden initially since orders are visible)
    if (toggleOrdersButton_) {
        toggleOrdersButton_->hide();
    }
    
    std::cout << "[POSModeContainer] Order entry mode layout created successfully" << std::endl;
}

void POSModeContainer::showOrderEdit() {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping showOrderEdit - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Creating order edit mode layout" << std::endl;
    
    // LEFT PANEL: Menu Display (replaces Active Orders Display)
    try {
        menuDisplay_ = leftPanel_->addNew<MenuDisplay>(posService_, eventManager_);
        menuDisplay_->addStyleClass("h-100");
        std::cout << "[POSModeContainer] ✓ Menu Display created in left panel" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error creating Menu Display: " << e.what() << std::endl;
        auto fallbackText = leftPanel_->addNew<Wt::WText>("❌ Unable to load menu");
        fallbackText->addStyleClass("text-center text-danger p-4");
    }
    
    // RIGHT PANEL: Current Order Display
    try {
        currentOrderDisplay_ = workArea_->addNew<CurrentOrderDisplay>(posService_, eventManager_);
        currentOrderDisplay_->addStyleClass("h-100");
        std::cout << "[POSModeContainer] ✓ Current Order Display created in right panel" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error creating Current Order Display: " << e.what() << std::endl;
        auto fallbackText = workArea_->addNew<Wt::WText>("❌ Unable to load current order");
        fallbackText->addStyleClass("text-center text-danger p-4");
    }
    
    // Show toggle button so user can switch back to orders view
    if (toggleOrdersButton_) {
        toggleOrdersButton_->setText("📋 Show Orders");
        toggleOrdersButton_->show();
    }
    
    std::cout << "[POSModeContainer] Order edit mode layout created successfully" << std::endl;
}

void POSModeContainer::showActiveOrdersDisplay() {
    if (isDestroying_) return;
    
    std::cout << "[POSModeContainer] Switching to Active Orders Display" << std::endl;
    
    // Force switch back to order entry mode to show active orders
    if (posService_) {
        posService_->setCurrentOrder(nullptr); // Clear current order
    }
    
    // This will trigger the layout change through the event system
    currentUIMode_ = UI_MODE_NONE; // Force mode change
    updateWorkArea();
}

void POSModeContainer::hideActiveOrdersDisplay() {
    // This method is no longer needed since we're using full panel replacement
    // but kept for compatibility
    std::cout << "[POSModeContainer] hideActiveOrdersDisplay() called - using full panel replacement instead" << std::endl;
}

void POSModeContainer::sendCurrentOrderToKitchen() {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping sendCurrentOrderToKitchen - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Sending current order to kitchen" << std::endl;
    
    // Validate that we have an order with items
    if (!hasCurrentOrder()) {
        std::cout << "[POSModeContainer] No current order to send to kitchen" << std::endl;
        return;
    }
    
    if (!hasOrderWithItems()) {
        std::cout << "[POSModeContainer] Current order has no items - cannot send to kitchen" << std::endl;
        return;
    }
    
    auto currentOrder = posService_->getCurrentOrder();
    int orderId = currentOrder ? currentOrder->getOrderId() : -1;
    
    try {
        // Step 1: Clear component pointers to prevent refresh calls
        std::cout << "[POSModeContainer] Clearing component pointers" << std::endl;
        orderEntryPanel_ = nullptr;
        menuDisplay_ = nullptr;
        currentOrderDisplay_ = nullptr;
        activeOrdersDisplay_ = nullptr;
        
        // Step 2: Clear widgets to stop event subscriptions
        std::cout << "[POSModeContainer] Clearing widgets" << std::endl;
        if (leftPanel_) leftPanel_->clear();
        if (workArea_) workArea_->clear();
        
        // Step 3: Send the order to kitchen
        std::cout << "[POSModeContainer] Sending order #" << orderId << " to kitchen" << std::endl;
        if (posService_) {
            bool success = posService_->sendCurrentOrderToKitchen();
            
            if (success) {
                std::cout << "[POSModeContainer] ✅ Order #" << orderId << " sent to kitchen successfully" << std::endl;
                
                // Clear the current order after successful kitchen submission
                posService_->setCurrentOrder(nullptr);
                
                // Show success feedback
                showOrderSentFeedback(orderId);
                
            } else {
                std::cout << "[POSModeContainer] ❌ Failed to send order #" << orderId << " to kitchen" << std::endl;
                posService_->setCurrentOrder(nullptr);
            }
        }
        
        // Step 4: Force UI mode change and return to order entry
        std::cout << "[POSModeContainer] Returning to order entry mode" << std::endl;
        currentUIMode_ = UI_MODE_NONE;
        showOrderEntry(); // This will show Active Orders on left, Order Entry on right
        currentUIMode_ = UI_MODE_ORDER_ENTRY;
        
        std::cout << "[POSModeContainer] Order sent to kitchen and returned to order entry mode" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error during send to kitchen: " << e.what() << std::endl;
        
        // Emergency fallback
        if (leftPanel_) leftPanel_->clear();
        if (workArea_) workArea_->clear();
        currentUIMode_ = UI_MODE_NONE;
        
        if (posService_) {
            posService_->setCurrentOrder(nullptr);
        }
    }
}

// Event handlers
void POSModeContainer::handleCurrentOrderChanged(const std::any& eventData) {
    if (isDestroying_) return;
    
    std::cout << "[POSModeContainer] Current order changed - scheduling UI update" << std::endl;
    
    // Use timer with safer capture using raw pointer
    POSModeContainer* self = this;
    
    Wt::WTimer::singleShot(std::chrono::milliseconds(300), [self]() {
        // Double-check we're still valid
        if (self->isDestroying_ || !self->parent()) {
            std::cout << "[POSModeContainer] Container destroyed during deferred update - aborting" << std::endl;
            return;
        }
        
        try {
            std::cout << "[POSModeContainer] Executing deferred current order change update" << std::endl;
            self->currentUIMode_ = UI_MODE_NONE;
            self->updateWorkArea();
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Exception in deferred update: " << e.what() << std::endl;
        }
    });
}

void POSModeContainer::handleOrderCreated(const std::any& eventData) {
    if (isDestroying_) return;
    
    std::cout << "[POSModeContainer] Order created - will be handled by current order changed event" << std::endl;
}

// Public interface methods
void POSModeContainer::refresh() {
    bool hasCurrentOrder = this->hasCurrentOrder();
    UIMode targetMode = hasCurrentOrder ? UI_MODE_ORDER_EDIT : UI_MODE_ORDER_ENTRY;
    
    // Only call updateWorkArea if the UI mode needs to change
    if (currentUIMode_ != targetMode) {
        std::cout << "[POSModeContainer] Mode change required - calling updateWorkArea" << std::endl;
        updateWorkArea();
    } else {
        // Just refresh the existing components without recreating them
        std::cout << "[POSModeContainer] Refreshing existing components in place" << std::endl;
        refreshDataOnly();
    }
    
    std::cout << "[POSModeContainer] Refresh completed" << std::endl;
}

void POSModeContainer::refreshDataOnly() {
    bool hasCurrentOrder = this->hasCurrentOrder();
    
    std::cout << "[POSModeContainer] Data-only refresh (preserving UI state)" << std::endl;
    
    // Update title and buttons
    if (workAreaTitle_) {
        try {
            if (hasCurrentOrder) {
                auto order = posService_->getCurrentOrder();
                if (order) {
                    workAreaTitle_->setText("🍽️ Editing Order #" + std::to_string(order->getOrderId()) + 
                                           " - " + order->getTableIdentifier());
                }
            } else {
                workAreaTitle_->setText("🍽️ Order Management");
            }
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error updating title: " << e.what() << std::endl;
        }
    }
    
    updateSendToKitchenButton();
    
    // Refresh components if they exist and are valid
    try {
        if (activeOrdersDisplay_ && activeOrdersDisplay_->parent()) {
            activeOrdersDisplay_->refresh();
        }
        if (orderEntryPanel_ && orderEntryPanel_->parent()) {
            orderEntryPanel_->refresh();
        }
        if (menuDisplay_ && menuDisplay_->parent()) {
            menuDisplay_->refresh();
        }
        if (currentOrderDisplay_ && currentOrderDisplay_->parent()) {
            currentOrderDisplay_->refresh();
        }
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error refreshing components: " << e.what() << std::endl;
    }
    
    std::cout << "[POSModeContainer] Data-only refresh completed" << std::endl;
}

void POSModeContainer::createNewOrder(const std::string& tableIdentifier) {
    if (posService_) {
        auto order = posService_->createOrder(tableIdentifier);
        if (order) {
            posService_->setCurrentOrder(order);
            currentUIMode_ = UI_MODE_NONE; // Force UI mode change
            std::cout << "[POSModeContainer] New order created: #" << order->getOrderId() << std::endl;
        }
    }
}

void POSModeContainer::openOrderForEditing(std::shared_ptr<Order> order) {
    if (order && posService_) {
        posService_->setCurrentOrder(order);
        currentUIMode_ = UI_MODE_NONE; // Force UI mode change
        std::cout << "[POSModeContainer] Order #" << order->getOrderId() << " opened for editing" << std::endl;
    }
}

// Helper methods
bool POSModeContainer::hasOrderWithItems() const {
    auto order = posService_ ? posService_->getCurrentOrder() : nullptr;
    return order && !order->getItems().empty();
}

bool POSModeContainer::hasCurrentOrder() const {
    return posService_ && posService_->getCurrentOrder() != nullptr;
}

void POSModeContainer::updateSendToKitchenButton() {
    if (isDestroying_ || !sendToKitchenButton_) return;
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    bool hasItems = hasOrderWithItems();
    
    try {
        if (hasCurrentOrder && hasItems) {
            sendToKitchenButton_->show();
            sendToKitchenButton_->setEnabled(true);
            sendToKitchenButton_->setText("🚀 Send to Kitchen");
            sendToKitchenButton_->setStyleClass("btn btn-success btn-sm");
        } else if (hasCurrentOrder && !hasItems) {
            sendToKitchenButton_->show();
            sendToKitchenButton_->setEnabled(false);
            sendToKitchenButton_->setText("🚀 Add Items First");
            sendToKitchenButton_->setStyleClass("btn btn-outline-secondary btn-sm");
        } else {
            sendToKitchenButton_->hide();
        }
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error updating Send to Kitchen button: " << e.what() << std::endl;
    }
}

void POSModeContainer::showOrderSentFeedback(int orderId) {
    if (workAreaTitle_) {
        try {
            workAreaTitle_->setText("✅ Order #" + std::to_string(orderId) + " sent to kitchen!");
            workAreaTitle_->setStyleClass("h4 text-success mb-4");
            
            // Reset the title after 3 seconds using safer approach
            POSModeContainer* self = this;
            
            Wt::WTimer::singleShot(std::chrono::milliseconds(3000), [self]() {
                if (!self->isDestroying_ && self->workAreaTitle_ && self->parent()) {
                    self->workAreaTitle_->setText("🍽️ Order Management");
                    self->workAreaTitle_->setStyleClass("h4 text-primary mb-4");
                }
            });
            
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error showing order sent feedback: " << e.what() << std::endl;
        }
    }
    
    std::cout << "[POSModeContainer] Success feedback displayed for order #" << orderId << std::endl;
}
