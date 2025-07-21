// ============================================================================
// Clean POSModeContainer.cpp - Minimal Borders, Streamlined Layout
// Replace your existing POSModeContainer.cpp with this cleaner version
// ============================================================================

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
    
    // CLEAN: Simple container class - no excessive styling
    setStyleClass("pos-mode-container h-100");
    
    initializeUI();
    setupEventListeners();
    updateWorkArea();
    
    std::cout << "[POSModeContainer] Initialized with clean design" << std::endl;
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
    // createOrderEntryArea();
    // createOrderEditArea();
}

// MINIMAL FIX: Just fix the panel proportions in POSModeContainer.cpp
// Replace ONLY the setupLayout() method - don't change anything else

void POSModeContainer::setupLayout() {
    // Create main horizontal layout
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(10, 10, 10, 10); // Restore original margins
    layout->setSpacing(15); // Restore original spacing
    
    // Create panels as children first, then set stretch factors
    leftPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    // FIX: Correct proportions - Left smaller, Right larger
    layout->setStretchFactor(leftPanel_, 3);   // 30% - Active Orders (left)
    layout->setStretchFactor(rightPanel_, 7);  // 70% - Work Area (right)
    
    // Keep original styling
    leftPanel_->setStyleClass("pos-left-panel");
    rightPanel_->setStyleClass("pos-right-panel");
}

void POSModeContainer::createLeftPanel() {
    // CLEAN: Left panel with minimal styling - just background and subtle right border
    auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    leftLayout->setContentsMargins(0, 0, 0, 0); // CLEAN: No margins
    leftLayout->setSpacing(0); // CLEAN: No spacing
    
    // Create Active Orders Display directly
    activeOrdersDisplay_ = leftLayout->addWidget(
        std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_));
    
    std::cout << "[POSModeContainer] Left panel (Active Orders) created with clean layout" << std::endl;
}

void POSModeContainer::createRightPanel() {
    // CLEAN: Right panel with minimal styling
    auto rightLayout = rightPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    rightLayout->setContentsMargins(0, 0, 0, 0); // CLEAN: No margins
    rightLayout->setSpacing(15); // Clean spacing between elements
    
    // CLEAN: Simple header container without excessive borders
    auto headerContainer = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->setStyleClass("d-flex justify-content-between align-items-center p-3");
    
    workAreaTitle_ = headerContainer->addNew<Wt::WText>("🍽️ Order Management");
    workAreaTitle_->setStyleClass("h4 text-primary mb-0");
    
    // Control buttons container
    auto controlsContainer = headerContainer->addNew<Wt::WContainerWidget>();
    controlsContainer->setStyleClass("d-flex gap-2");
    
    // Toggle button (hidden initially when orders display is visible)
    toggleOrdersButton_ = controlsContainer->addNew<Wt::WPushButton>("📋 Show Orders");
    toggleOrdersButton_->setStyleClass("btn btn-outline-info btn-sm");
    toggleOrdersButton_->clicked().connect([this]() {
        showActiveOrdersDisplay();
    });
    toggleOrdersButton_->setId("toggle-orders-button");
    toggleOrdersButton_->hide();
    
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
    
    std::cout << "[POSModeContainer] Right panel (Work Area) created with clean layout" << std::endl;
}

// void POSModeContainer::createOrderEntryArea() {
//     // Order entry area will be created in place when needed
//     std::cout << "[POSModeContainer] Order entry area setup prepared" << std::endl;
// }

// void POSModeContainer::createOrderEditArea() {
//     // Order edit area will be created in place when needed
//     std::cout << "[POSModeContainer] Order edit area setup prepared" << std::endl;
// }

void POSModeContainer::showOrderEntry() {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping showOrderEntry - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Creating order entry area" << std::endl;
    
    // Clear work area safely
    if (workArea_->children().size() > 0) {
        std::cout << "[POSModeContainer] Processing events before clearing work area" << std::endl;
        
        for (int i = 0; i < 3; ++i) {
            Wt::WApplication::instance()->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
        std::cout << "[POSModeContainer] Clearing work area for order entry" << std::endl;
        workArea_->clear();
    }
    
    // CLEAN: Create order entry area directly without excessive containers
    auto orderEntryArea = workArea_->addNew<Wt::WContainerWidget>();
    orderEntryArea->setStyleClass("order-entry-area");
    
    auto layout = orderEntryArea->setLayout(std::make_unique<Wt::WVBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20); // Clean spacing between elements
    
    // Order entry panel - store the pointer for refresh calls
    std::cout << "[POSModeContainer] Creating new OrderEntryPanel" << std::endl;
    orderEntryPanel_ = layout->addWidget(
        std::make_unique<OrderEntryPanel>(posService_, eventManager_));
    
    // CLEAN: Simple instructions without excessive styling
    auto instructionsText = layout->addWidget(std::make_unique<Wt::WText>(
        "💡 Select a table/location and click 'Start New Order' to begin"));
    instructionsText->setStyleClass("text-center text-muted small");
    
    std::cout << "[POSModeContainer] Order entry area created successfully" << std::endl;
}

void POSModeContainer::showOrderEdit() {
    if (isDestroying_) {
        return;
    }
    
    std::cout << "[DEBUG] showOrderEdit() called" << std::endl;
    
    // Clear component pointers first
    menuDisplay_ = nullptr;
    currentOrderDisplay_ = nullptr;
    
    // Clear the right panel work area
    if (workArea_ && workArea_->children().size() > 0) {
        std::cout << "[DEBUG] Clearing workArea_ - had " << workArea_->children().size() << " children" << std::endl;
        workArea_->clear();
    }
    
    // Clear the left panel 
    if (leftPanel_ && leftPanel_->children().size() > 0) {
        std::cout << "[DEBUG] Clearing leftPanel_ - had " << leftPanel_->children().size() << " children" << std::endl;
        leftPanel_->clear();
    }
    
    // Check if leftPanel_ is visible
    std::cout << "[DEBUG] leftPanel_ visible: " << (leftPanel_ && leftPanel_->isVisible() ? "YES" : "NO") << std::endl;
    std::cout << "[DEBUG] leftPanel_ hidden: " << (leftPanel_ && leftPanel_->isHidden() ? "YES" : "NO") << std::endl;
    
    try {
        // LEFT PANEL: Menu Display 
        auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
        leftLayout->setContentsMargins(0, 0, 0, 0);
        leftLayout->setSpacing(0);
        
        std::cout << "[DEBUG] Creating MenuDisplay in left panel..." << std::endl;
        menuDisplay_ = leftPanel_->addWidget(
            std::make_unique<MenuDisplay>(posService_, eventManager_));
        
        if (menuDisplay_) {
            std::cout << "[DEBUG] ✓ MenuDisplay created successfully" << std::endl;
            std::cout << "[DEBUG] MenuDisplay visible: " << menuDisplay_->isVisible() << std::endl;
            std::cout << "[DEBUG] MenuDisplay hidden: " << menuDisplay_->isHidden() << std::endl;
            std::cout << "[DEBUG] MenuDisplay parent: " << (menuDisplay_->parent() ? "EXISTS" : "NULL") << std::endl;
        }
        
        // RIGHT PANEL: Current Order Display 
        std::cout << "[DEBUG] Creating CurrentOrderDisplay in right panel..." << std::endl;
        currentOrderDisplay_ = workArea_->addWidget(
            std::make_unique<CurrentOrderDisplay>(posService_, eventManager_));
        
        if (currentOrderDisplay_) {
            std::cout << "[DEBUG] ✓ CurrentOrderDisplay created successfully" << std::endl;
        }
        
        std::cout << "[DEBUG] Final panel children count:" << std::endl;
        std::cout << "[DEBUG]   leftPanel_: " << leftPanel_->children().size() << " children" << std::endl;
        std::cout << "[DEBUG]   workArea_: " << workArea_->children().size() << " children" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[DEBUG] Error in showOrderEdit: " << e.what() << std::endl;
    }
    
    std::cout << "[DEBUG] showOrderEdit() completed" << std::endl;
}

void POSModeContainer::hideActiveOrdersDisplay() {
    if (!leftPanel_) return;
    
    std::cout << "[POSModeContainer] Hiding Active Orders Display - giving more space to work area" << std::endl;
    
    // Hide the left panel
    leftPanel_->hide();
    
    // Update layout to give right panel full width
    auto layout = dynamic_cast<Wt::WHBoxLayout*>(this->layout());
    if (layout) {
        layout->setStretchFactor(leftPanel_, 0);   // 0% - hidden
        layout->setStretchFactor(rightPanel_, 1);  // 100% - full width
    }
    
    // Show toggle button
    if (toggleOrdersButton_) {
        toggleOrdersButton_->show();
    }
    
    // CLEAN: Add full-width class instead of excessive styling
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
    
    // Hide toggle button
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

void POSModeContainer::sendCurrentOrderToKitchen() {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping sendCurrentOrderToKitchen - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Sending current order to kitchen - safe process" << std::endl;
    
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
        // Process any pending events to clear the queue
        std::cout << "[POSModeContainer] Processing pending events before sending to kitchen" << std::endl;
        for (int i = 0; i < 3; ++i) {
            Wt::WApplication::instance()->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
        // Clear component pointers immediately to prevent any refresh calls
        std::cout << "[POSModeContainer] Clearing component pointers" << std::endl;
        orderEntryPanel_ = nullptr;
        menuDisplay_ = nullptr;
        currentOrderDisplay_ = nullptr;
        
        // Clear the widgets first to destroy them and unsubscribe from events
        std::cout << "[POSModeContainer] Clearing work area widgets to stop event subscriptions" << std::endl;
        if (workArea_ && workArea_->children().size() > 0) {
            Wt::WApplication::instance()->processEvents();
            workArea_->clear();
        }
        
        // Process events again to ensure widget destruction is complete
        std::cout << "[POSModeContainer] Processing events after widget destruction" << std::endl;
        for (int i = 0; i < 3; ++i) {
            Wt::WApplication::instance()->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
        // NOW safely send the order to kitchen
        std::cout << "[POSModeContainer] Sending order #" << orderId << " to kitchen (widgets destroyed)" << std::endl;
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
                
                // Still clear the current order
                posService_->setCurrentOrder(nullptr);
            }
        }
        
        // Force UI mode change
        std::cout << "[POSModeContainer] Forcing UI mode change after kitchen submission" << std::endl;
        currentUIMode_ = UI_MODE_NONE;
        
        // Show order entry immediately (synchronous)
        showOrderEntry();
        
        // Show active orders display
        showActiveOrdersDisplay();
        
        // Update UI mode
        currentUIMode_ = UI_MODE_ORDER_ENTRY;
        
        std::cout << "[POSModeContainer] Order sent to kitchen and UI updated successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error during send to kitchen: " << e.what() << std::endl;
        
        // Emergency fallback - just clear everything
        if (workArea_) {
            workArea_->clear();
        }
        currentUIMode_ = UI_MODE_NONE;
        
        // Still try to clear the current order
        if (posService_) {
            posService_->setCurrentOrder(nullptr);
        }
    }
}

// Keep all other existing methods unchanged but with clean logging...

void POSModeContainer::updateWorkArea() {
    // Check if we're being destroyed
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping updateWorkArea - container is being destroyed" << std::endl;
        return;
    }
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    UIMode targetMode = hasCurrentOrder ? UI_MODE_ORDER_EDIT : UI_MODE_ORDER_ENTRY;
    
    // DEBUG OUTPUT
    std::cout << "[POSModeContainer] updateWorkArea() called" << std::endl;
    std::cout << "[POSModeContainer]   hasCurrentOrder: " << hasCurrentOrder << std::endl;
    std::cout << "[POSModeContainer]   targetMode: " << (targetMode == UI_MODE_ORDER_EDIT ? "EDIT" : "ENTRY") << std::endl;
    std::cout << "[POSModeContainer]   currentUIMode: " << currentUIMode_ << std::endl;
    
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
                workAreaTitle_->addStyleClass("h4 text-primary mb-4");
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
            // EDIT MODE: Show MenuDisplay in left panel, CurrentOrderDisplay in right panel
            showOrderEdit();
            
            // CRITICAL FIX: DON'T hide the left panel when MenuDisplay is showing
            // The MenuDisplay needs the left panel to be visible
            std::cout << "[POSModeContainer] Order edit mode - keeping left panel visible for MenuDisplay" << std::endl;
            
        } else {
            // ENTRY MODE: Show ActiveOrdersDisplay in left panel, OrderEntryPanel in right panel
            showOrderEntry();
            showActiveOrdersDisplay();  // Show orders list when not editing
        }
        
        currentUIMode_ = targetMode;
    } else {
        std::cout << "[POSModeContainer] UI mode unchanged - skipping recreation" << std::endl;
    }
}

// Keep all existing event handlers and other methods unchanged...

void POSModeContainer::setupEventListeners() {
    if (!eventManager_) return;
    
    // Listen for current order changes to toggle display visibility
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { 
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

// Event handlers with proper destruction checks
void POSModeContainer::handleCurrentOrderChanged(const std::any& eventData) {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Ignoring current order changed event - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Current order changed - scheduling UI mode update" << std::endl;
    
    Wt::WTimer::singleShot(std::chrono::milliseconds(500), [this]() {
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
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Ignoring order created event - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Order created - refreshing displays" << std::endl;
    
    if (activeOrdersDisplay_) {
        try {
            activeOrdersDisplay_->refresh();
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error refreshing active orders after order creation: " << e.what() << std::endl;
        }
    }
    
    Wt::WTimer::singleShot(std::chrono::milliseconds(500), [this]() {
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
        std::cout << "[POSModeContainer] Refreshing existing components in place" << std::endl;
        refreshDataOnly();
    }
    
    std::cout << "[POSModeContainer] Refresh completed (hasCurrentOrder: " << hasCurrentOrder 
              << ", currentUIMode: " << currentUIMode_ << ")" << std::endl;
}

void POSModeContainer::refreshDataOnly() {
    // SAFETY: Don't do anything if being destroyed
    if (isDestroying_) {
        return;
    }

    bool hasCurrentOrder = this->hasCurrentOrder();
    std::cout << "[POSModeContainer] Data-only refresh (preserving UI state)" << std::endl;
    
    // SAFE: Always refresh active orders display
    if (activeOrdersDisplay_) {
        try {
            // Check if widget is still valid before calling methods
            if (activeOrdersDisplay_->parent()) {
                activeOrdersDisplay_->refresh();
            } else {
                activeOrdersDisplay_ = nullptr; // Clear invalid pointer
            }
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Active orders refresh failed: " << e.what() << std::endl;
            activeOrdersDisplay_ = nullptr;
        }
    }
    
    // SAFE: Update title if it exists
    if (workAreaTitle_ && workAreaTitle_->parent()) {
        try {
            if (hasCurrentOrder) {
                auto order = posService_->getCurrentOrder();
                if (order) {
                    workAreaTitle_->setText("🍽️ Editing Order #" + std::to_string(order->getOrderId()));
                }
            } else {
                workAreaTitle_->setText("🍽️ Order Management");
            }
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Title update failed: " << e.what() << std::endl;
        }
    }
    
    // SAFE: Update button
    updateSendToKitchenButton();
    
    // CRITICAL: Only refresh if pointers are valid AND widgets have parents
    if (!hasCurrentOrder) {
        // In order entry mode
        if (orderEntryPanel_ && orderEntryPanel_->parent()) {
            try {
                orderEntryPanel_->refresh();
            } catch (const std::exception& e) {
                std::cerr << "[POSModeContainer] OrderEntry refresh failed" << std::endl;
                orderEntryPanel_ = nullptr;
            }
        }
    } else {
        // In order edit mode - be VERY careful with these pointers
        if (menuDisplay_) {
            try {
                if (menuDisplay_->parent()) {
                    menuDisplay_->refresh();
                } else {
                    std::cout << "[POSModeContainer] MenuDisplay has no parent - clearing pointer" << std::endl;
                    menuDisplay_ = nullptr;
                }
            } catch (const std::exception& e) {
                std::cerr << "[POSModeContainer] MenuDisplay refresh failed - clearing pointer" << std::endl;
                menuDisplay_ = nullptr;
            }
        }
        
        if (currentOrderDisplay_) {
            try {
                if (currentOrderDisplay_->parent()) {
                    currentOrderDisplay_->refresh();
                } else {
                    std::cout << "[POSModeContainer] CurrentOrderDisplay has no parent - clearing pointer" << std::endl;
                    currentOrderDisplay_ = nullptr;
                }
            } catch (const std::exception& e) {
                std::cerr << "[POSModeContainer] CurrentOrderDisplay refresh failed - clearing pointer" << std::endl;
                currentOrderDisplay_ = nullptr;
            }
        }
    }
    
    std::cout << "[POSModeContainer] Data-only refresh completed" << std::endl;
}

// Helper methods
bool POSModeContainer::hasOrderWithItems() const {
    auto order = posService_ ? posService_->getCurrentOrder() : nullptr;
    return order && !order->getItems().empty();
}

bool POSModeContainer::hasCurrentOrder() const {
    bool result = posService_ && posService_->getCurrentOrder() != nullptr;
    
    // DEBUG: Let's see what we get
    if (posService_) {
        auto order = posService_->getCurrentOrder();
        std::cout << "[POSModeContainer] hasCurrentOrder() check:" << std::endl;
        std::cout << "   posService_ exists: YES" << std::endl;
        std::cout << "   getCurrentOrder(): " << (order ? "EXISTS" : "NULL") << std::endl;
        if (order) {
            std::cout << "   Order ID: " << order->getOrderId() << std::endl;
            std::cout << "   Table: " << order->getTableIdentifier() << std::endl;
        }
    } else {
        std::cout << "[POSModeContainer] hasCurrentOrder() check: posService_ is NULL" << std::endl;
    }
    
    std::cout << "[POSModeContainer] hasCurrentOrder() returning: " << result << std::endl;
    return result;
}

void POSModeContainer::updateSendToKitchenButton() {
    if (isDestroying_ || !sendToKitchenButton_) return;
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    bool hasItems = hasOrderWithItems();
    
    std::cout << "[POSModeContainer] Updating Send to Kitchen button - hasOrder: " << hasCurrentOrder 
              << ", hasItems: " << hasItems << std::endl;
    
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
    // Update the work area title temporarily to show success
    if (workAreaTitle_) {
        try {
            workAreaTitle_->setText("✅ Order #" + std::to_string(orderId) + " sent to kitchen!");
            workAreaTitle_->setStyleClass("h4 text-success mb-4");
            
            // Reset the title after 3 seconds
            Wt::WTimer::singleShot(std::chrono::milliseconds(3000), [this]() {
                if (!isDestroying_ && workAreaTitle_) {
                    workAreaTitle_->setText("🍽️ Order Management");
                    workAreaTitle_->setStyleClass("h4 text-primary mb-4");
                }
            });
            
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error showing order sent feedback: " << e.what() << std::endl;
        }
    }
    
    std::cout << "[POSModeContainer] Showing success feedback for order #" << orderId << std::endl;
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
