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
    , newOrderButton_(nullptr)
    , closeOrderButton_(nullptr)
    , toggleOrdersButton_(nullptr)
    , currentUIMode_(UI_MODE_NONE)  // Track current UI state
    , isDestroying_(false)  // Track destruction state
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
    createOrderEntryArea();
    createOrderEditArea();
}

void POSModeContainer::setupLayout() {
    // Create main horizontal layout
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    // FIXED: Create panels as children first, then set stretch factors
    leftPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    // FIXED: Set stretch factors after widgets are added
    layout->setStretchFactor(leftPanel_, 3);   // 30%
    layout->setStretchFactor(rightPanel_, 7);  // 70%
    
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
    
    // FIXED: Store reference to toggle button instead of using findWidget
    toggleOrdersButton_ = controlsContainer->addNew<Wt::WPushButton>("📋 Show Orders");
    toggleOrdersButton_->setStyleClass("btn btn-outline-info btn-sm");
    toggleOrdersButton_->clicked().connect([this]() {
        showActiveOrdersDisplay();
    });
    toggleOrdersButton_->setId("toggle-orders-button");
    toggleOrdersButton_->hide(); // Hidden initially when orders display is visible
    
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
    // Order entry area will be created in place when needed
    // This avoids the ownership transfer issues
    std::cout << "[POSModeContainer] Order entry area setup prepared" << std::endl;
}

void POSModeContainer::createOrderEditArea() {
    // Order edit area will be created in place when needed
    // This avoids the ownership transfer issues
    std::cout << "[POSModeContainer] Order edit area setup prepared" << std::endl;
}

void POSModeContainer::setupEventListeners() {
    if (!eventManager_) return;
    
    // Listen for current order changes to toggle display visibility
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { 
                // Extra safety check before handling events
                if (isDestroying_) {
                    std::cout << "[POSModeContainer] Ignoring CURRENT_ORDER_CHANGED - being destroyed" << std::endl;
                    return;
                }
                
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
                // Extra safety check before handling events
                if (isDestroying_) {
                    std::cout << "[POSModeContainer] Ignoring ORDER_CREATED - being destroyed" << std::endl;
                    return;
                }
                
                try {
                    handleOrderCreated(data); 
                } catch (const std::exception& e) {
                    std::cerr << "[POSModeContainer] Error handling order created: " << e.what() << std::endl;
                }
            })
    );
    
    std::cout << "[POSModeContainer] Event listeners set up with safety checks" << std::endl;
}

void POSModeContainer::updateWorkArea() {
    // Check if we're being destroyed
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
    
    // Update close button visibility
    if (closeOrderButton_) {
        try {
            if (hasCurrentOrder) {
                closeOrderButton_->show();
            } else {
                closeOrderButton_->hide();
            }
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error updating close button: " << e.what() << std::endl;
        }
    }
    
    // Only recreate UI if the mode has actually changed
    if (currentUIMode_ != targetMode) {
        std::cout << "[POSModeContainer] UI mode change detected - recreating interface" << std::endl;
        
        // CRITICAL: Clear component pointers BEFORE destroying widgets
        // This prevents the refresh methods from trying to access destroyed widgets
        orderEntryPanel_ = nullptr;
        menuDisplay_ = nullptr;
        currentOrderDisplay_ = nullptr;
        
        // Process any pending events and clear widgets safely
        if (workArea_ && workArea_->children().size() > 0) {
            std::cout << "[POSModeContainer] Processing pending events before widget destruction" << std::endl;
            
            try {
                Wt::WApplication::instance()->processEvents();
                
                // Give extra time for events to settle
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                
                workArea_->clear();
                std::cout << "[POSModeContainer] Work area cleared safely" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[POSModeContainer] Error clearing work area: " << e.what() << std::endl;
            }
        }
        
        // Check again if we're still valid before creating new widgets
        if (isDestroying_) {
            std::cout << "[POSModeContainer] Container destroyed during update - aborting" << std::endl;
            return;
        }
        
        if (hasCurrentOrder) {
            showOrderEdit();
            hideActiveOrdersDisplay(); // Hide orders list when editing
        } else {
            showOrderEntry();
            showActiveOrdersDisplay();  // Show orders list when not editing
        }
        
        currentUIMode_ = targetMode;
    } else {
        std::cout << "[POSModeContainer] UI mode unchanged - skipping recreation" << std::endl;
    }
}

void POSModeContainer::showOrderEntry() {
    // Check if we're being destroyed
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping showOrderEntry - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Creating order entry area" << std::endl;
    
    // Clear work area safely - defer any pending events first
    if (workArea_->children().size() > 0) {
        std::cout << "[POSModeContainer] Processing events before clearing work area" << std::endl;
        
        // Process events multiple times to ensure everything settles
        for (int i = 0; i < 3; ++i) {
            Wt::WApplication::instance()->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
        std::cout << "[POSModeContainer] Clearing work area for order entry" << std::endl;
        workArea_->clear();
    }
    
    // Create order entry area directly in the work area
    auto orderEntryArea = workArea_->addNew<Wt::WContainerWidget>();
    orderEntryArea->setStyleClass("order-entry-area");
    
    auto layout = orderEntryArea->setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);
    
    // Welcome message
    auto welcomeText = layout->addWidget(std::make_unique<Wt::WText>(
        "Welcome to the Restaurant POS System"));
    welcomeText->setStyleClass("h5 text-center text-muted mb-4");
    
    // Order entry panel - store the pointer for refresh calls
    std::cout << "[POSModeContainer] Creating new OrderEntryPanel" << std::endl;
    orderEntryPanel_ = layout->addWidget(
        std::make_unique<OrderEntryPanel>(posService_, eventManager_));
    
    // Instructions
    auto instructionsText = layout->addWidget(std::make_unique<Wt::WText>(
        "💡 Select a table/location and click 'Start New Order' to begin"));
    instructionsText->setStyleClass("text-center text-muted small");
    
    std::cout << "[POSModeContainer] Order entry area created successfully" << std::endl;
}

void POSModeContainer::showOrderEdit() {
    // Check if we're being destroyed
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping showOrderEdit - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Creating order edit area" << std::endl;
    
    // Clear work area safely - defer any pending events first
    if (workArea_->children().size() > 0) {
        std::cout << "[POSModeContainer] Processing events before clearing work area" << std::endl;
        
        // Process events multiple times to ensure everything settles
        for (int i = 0; i < 3; ++i) {
            Wt::WApplication::instance()->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
        std::cout << "[POSModeContainer] Clearing work area for order edit" << std::endl;
        workArea_->clear();
    }
    
    // Create order edit area directly in the work area
    auto orderEditArea = workArea_->addNew<Wt::WContainerWidget>();
    orderEditArea->setStyleClass("order-edit-area");
    
    auto layout = orderEditArea->setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);
    
    // Left side: Menu Display (takes more space when active orders are hidden)
    auto menuContainer = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    menuContainer->setStyleClass("menu-section");
    
    auto menuLayout = menuContainer->setLayout(std::make_unique<Wt::WVBoxLayout>());
    menuLayout->setContentsMargins(0, 0, 0, 0);
    menuLayout->setSpacing(10);
    
    // Store the pointer for refresh calls
    std::cout << "[POSModeContainer] Creating new MenuDisplay" << std::endl;
    menuDisplay_ = menuLayout->addWidget(
        std::make_unique<MenuDisplay>(posService_, eventManager_));
    
    // Right side: Current Order Display
    auto orderContainer = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    orderContainer->setStyleClass("current-order-section");
    
    auto orderLayout = orderContainer->setLayout(std::make_unique<Wt::WVBoxLayout>());
    orderLayout->setContentsMargins(0, 0, 0, 0);
    orderLayout->setSpacing(10);
    
    // Store the pointer for refresh calls
    std::cout << "[POSModeContainer] Creating new CurrentOrderDisplay" << std::endl;
    currentOrderDisplay_ = orderLayout->addWidget(
        std::make_unique<CurrentOrderDisplay>(posService_, eventManager_));
    
    // Set layout proportions: Menu gets more space (65%), Current Order (35%)
    layout->setStretchFactor(menuContainer, 65);
    layout->setStretchFactor(orderContainer, 35);
    
    std::cout << "[POSModeContainer] Order edit area created successfully" << std::endl;
}

void POSModeContainer::hideActiveOrdersDisplay() {
    if (!leftPanel_) return;
    
    std::cout << "[POSModeContainer] Hiding Active Orders Display - giving more space to work area" << std::endl;
    
    // Hide the left panel
    leftPanel_->hide();
    
    // Update layout to give right panel full width
    auto layout = dynamic_cast<Wt::WHBoxLayout*>(this->layout());
    if (layout) {
        // Update stretch factors
        layout->setStretchFactor(leftPanel_, 0);   // 0% - hidden
        layout->setStretchFactor(rightPanel_, 1);  // 100% - full width
    }
    
    // FIXED: Use stored reference instead of findWidget
    if (toggleOrdersButton_) {
        toggleOrdersButton_->show();
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
    
    // FIXED: Use stored reference instead of findWidget
    if (toggleOrdersButton_) {
        toggleOrdersButton_->hide();
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
    // Check if we're being destroyed
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Ignoring current order changed event - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Current order changed - scheduling UI mode update" << std::endl;
    
    // Use a longer timer to ensure all events settle completely
    // Capture 'this' by weak reference to avoid use-after-free
    Wt::WTimer::singleShot(std::chrono::milliseconds(500), [this]() {
        // Double-check we're not being destroyed
        if (isDestroying_) {
            std::cout << "[POSModeContainer] Skipping deferred update - container destroyed" << std::endl;
            return;
        }
        
        std::cout << "[POSModeContainer] Executing deferred current order change update" << std::endl;
        
        // Force mode change by resetting currentUIMode_
        currentUIMode_ = UI_MODE_NONE;
        updateWorkArea();
    });
}

void POSModeContainer::handleOrderCreated(const std::any& eventData) {
    // Check if we're being destroyed
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Ignoring order created event - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Order created - refreshing displays" << std::endl;
    
    // Don't immediately force UI recreation - just refresh existing components
    // The CURRENT_ORDER_CHANGED event will handle the UI mode switch if needed
    if (activeOrdersDisplay_) {
        try {
            activeOrdersDisplay_->refresh();
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error refreshing active orders after order creation: " << e.what() << std::endl;
        }
    }
    
    // Use a longer timer to check if we need to update the work area after events settle
    Wt::WTimer::singleShot(std::chrono::milliseconds(500), [this]() {
        // Double-check we're not being destroyed
        if (isDestroying_) {
            std::cout << "[POSModeContainer] Skipping deferred order creation check - container destroyed" << std::endl;
            return;
        }
        
        std::cout << "[POSModeContainer] Executing deferred order creation check" << std::endl;
        
        bool hasCurrentOrder = this->hasCurrentOrder();
        UIMode targetMode = hasCurrentOrder ? UI_MODE_ORDER_EDIT : UI_MODE_ORDER_ENTRY;
        
        std::cout << "[POSModeContainer] Deferred check - hasCurrentOrder: " << hasCurrentOrder 
                  << ", currentUIMode: " << currentUIMode_ << ", targetMode: " << targetMode << std::endl;
        
        if (currentUIMode_ != targetMode) {
            std::cout << "[POSModeContainer] Mode change needed after order creation" << std::endl;
            currentUIMode_ = UI_MODE_NONE;
            updateWorkArea();
        }
    });
}

// Public interface methods
void POSModeContainer::refresh() {
    // Smart refresh - only update work area if needed, otherwise just refresh existing components
    bool hasCurrentOrder = this->hasCurrentOrder();
    UIMode targetMode = hasCurrentOrder ? UI_MODE_ORDER_EDIT : UI_MODE_ORDER_ENTRY;
    
    // Always refresh the active orders display
    if (activeOrdersDisplay_) {
        activeOrdersDisplay_->refresh();
    }
    
    // Only call updateWorkArea if the UI mode needs to change
    if (currentUIMode_ != targetMode) {
        std::cout << "[POSModeContainer] Mode change required - calling updateWorkArea" << std::endl;
        updateWorkArea();
    } else {
        // Just refresh the existing components without recreating them
        std::cout << "[POSModeContainer] Refreshing existing components in place" << std::endl;
        refreshDataOnly();
    }
    
    std::cout << "[POSModeContainer] Refresh completed (hasCurrentOrder: " << hasCurrentOrder 
              << ", currentUIMode: " << currentUIMode_ << ")" << std::endl;
}

void POSModeContainer::refreshDataOnly() {
    // Data-only refresh - never recreates UI components, safe for periodic updates
    bool hasCurrentOrder = this->hasCurrentOrder();
    
    std::cout << "[POSModeContainer] Data-only refresh (preserving UI state)" << std::endl;
    
    // Always refresh the active orders display (this is safe)
    if (activeOrdersDisplay_) {
        try {
            activeOrdersDisplay_->refresh();
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error refreshing active orders: " << e.what() << std::endl;
        }
    }
    
    // Update title and buttons (these don't recreate components, just update text/visibility)
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
    
    if (closeOrderButton_) {
        try {
            if (hasCurrentOrder) {
                closeOrderButton_->show();
            } else {
                closeOrderButton_->hide();
            }
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error updating close button: " << e.what() << std::endl;
        }
    }
    
    // CRITICAL: Only refresh components if they exist AND are valid
    // Check both pointer existence and that the widget hasn't been destroyed
    try {
        if (!hasCurrentOrder && orderEntryPanel_ && orderEntryPanel_->parent()) {
            // In order entry mode - refresh order entry panel (but preserve selections)
            std::cout << "[POSModeContainer] Refreshing OrderEntryPanel" << std::endl;
            orderEntryPanel_->refresh();
        } else if (hasCurrentOrder) {
            // In order edit mode - refresh menu and current order displays for live updates
            if (menuDisplay_ && menuDisplay_->parent()) {
                std::cout << "[POSModeContainer] Refreshing MenuDisplay" << std::endl;
                menuDisplay_->refresh();
            }
            if (currentOrderDisplay_ && currentOrderDisplay_->parent()) {
                std::cout << "[POSModeContainer] Refreshing CurrentOrderDisplay" << std::endl;
                currentOrderDisplay_->refresh(); // This gives immediate feedback for cart updates
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error refreshing components: " << e.what() << std::endl;
        
        // Clear the component pointers since they're apparently invalid
        if (!hasCurrentOrder) {
            orderEntryPanel_ = nullptr;
        } else {
            menuDisplay_ = nullptr;
            currentOrderDisplay_ = nullptr;
        }
    }
    
    std::cout << "[POSModeContainer] Data-only refresh completed" << std::endl;
}

void POSModeContainer::createNewOrder(const std::string& tableIdentifier) {
    if (posService_) {
        auto order = posService_->createOrder(tableIdentifier);
        if (order) {
            posService_->setCurrentOrder(order);
            // Force UI mode change
            currentUIMode_ = UI_MODE_NONE;
            std::cout << "[POSModeContainer] New order created: #" << order->getOrderId() << std::endl;
        }
    }
}

void POSModeContainer::openOrderForEditing(std::shared_ptr<Order> order) {
    if (order && posService_) {
        posService_->setCurrentOrder(order);
        // Force UI mode change
        currentUIMode_ = UI_MODE_NONE;
        std::cout << "[POSModeContainer] Order #" << order->getOrderId() << " opened for editing" << std::endl;
    }
}

void POSModeContainer::closeCurrentOrder() {
    // Check if we're being destroyed
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping closeCurrentOrder - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Closing current order - safe UI cleanup" << std::endl;
    
    try {
        // Step 1: Process any pending events to clear the queue
        std::cout << "[POSModeContainer] Processing pending events before close" << std::endl;
        for (int i = 0; i < 3; ++i) {
            Wt::WApplication::instance()->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
        // Step 2: Clear component pointers immediately to prevent any refresh calls
        std::cout << "[POSModeContainer] Clearing component pointers" << std::endl;
        orderEntryPanel_ = nullptr;
        menuDisplay_ = nullptr;
        currentOrderDisplay_ = nullptr;
        
        // Step 3: CLEAR THE WIDGETS FIRST to destroy them and unsubscribe from events
        std::cout << "[POSModeContainer] Clearing work area widgets to stop event subscriptions" << std::endl;
        if (workArea_ && workArea_->children().size() > 0) {
            // Process events one more time before clearing
            Wt::WApplication::instance()->processEvents();
            workArea_->clear();  // This destroys widgets and their event subscriptions
        }
        
        // Step 4: Process events again to ensure widget destruction is complete
        std::cout << "[POSModeContainer] Processing events after widget destruction" << std::endl;
        for (int i = 0; i < 3; ++i) {
            Wt::WApplication::instance()->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
        // Step 5: NOW safely clear the current order from the service
        std::cout << "[POSModeContainer] Clearing current order from service (widgets destroyed)" << std::endl;
        if (posService_) {
            posService_->setCurrentOrder(nullptr);
        }
        
        // Step 6: Force UI mode change
        std::cout << "[POSModeContainer] Forcing UI mode change" << std::endl;
        currentUIMode_ = UI_MODE_NONE;
        
        // Step 7: Show order entry immediately (synchronous)
        showOrderEntry();
        
        // Step 8: Show active orders display
        showActiveOrdersDisplay();
        
        // Step 9: Update UI mode
        currentUIMode_ = UI_MODE_ORDER_ENTRY;
        
        std::cout << "[POSModeContainer] Order closed and UI updated successfully (safe sequence)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error during safe close order: " << e.what() << std::endl;
        
        // Emergency fallback - just clear everything
        if (workArea_) {
            workArea_->clear();
        }
        currentUIMode_ = UI_MODE_NONE;
    }
}

bool POSModeContainer::hasCurrentOrder() const {
    return posService_ && posService_->getCurrentOrder() != nullptr;
}
