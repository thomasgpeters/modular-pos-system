// ============================================================================
// FIXED POSModeContainer.cpp - Proper UI Mode Switching
// This fixes the issue where clicking "Start New Order" doesn't properly 
// switch from Active Orders to Menu Display and Current Order editing
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
    
    setStyleClass("pos-mode-container h-100");
    
    initializeUI();
    setupEventListeners();
    
    // CRITICAL FIX: Start in order entry mode
    showOrderEntryMode();
    
    std::cout << "[POSModeContainer] Initialized in ORDER_ENTRY mode" << std::endl;

    // At the very end of your constructor, replace the existing initialization with:

    setStyleClass("pos-mode-container h-100");
    
    // FORCE MAIN CONTAINER VISIBILITY
    show();
    setHidden(false);
    setAttributeValue("style", 
        "display: flex !important; height: calc(100vh - 120px) !important; "
        "width: 100% !important; background-color: #fafafa;");
    
    initializeUI();
    setupEventListeners();
    
    // FORCE PANEL VISIBILITY AFTER CREATION
    if (leftPanel_) {
        leftPanel_->show();
        leftPanel_->setHidden(false);
        std::cout << "[DEBUG] Constructor - leftPanel_ forced visible: " << leftPanel_->isVisible() << std::endl;
    }
    if (rightPanel_) {
        rightPanel_->show();
        rightPanel_->setHidden(false);
        std::cout << "[DEBUG] Constructor - rightPanel_ forced visible: " << rightPanel_->isVisible() << std::endl;
    }
    
    // Start in order entry mode
    showOrderEntryMode();
    
    std::cout << "[POSModeContainer] Constructor completed with FORCED visibility" << std::endl;
    
}

POSModeContainer::~POSModeContainer() {
    std::cout << "[POSModeContainer] Destructor called - cleaning up" << std::endl;
    
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

// ============================================================================
// STARTUP LAYOUT FIX - Replace these methods in POSModeContainer.cpp
// ============================================================================

// 1. REPLACE only your setupLayout() method with this:
void POSModeContainer::setupLayout() {
    std::cout << "[POSModeContainer] Setting up clean layout..." << std::endl;
    
    // Apply CSS classes that match our fixed CSS
    setStyleClass("pos-mode-container");
    setId("pos-mode-container");
    
    // Create simple horizontal layout
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // Create panels
    leftPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    // Apply CSS classes for styling
    leftPanel_->setStyleClass("pos-left-panel");
    rightPanel_->setStyleClass("pos-right-panel");
    
    // Set layout proportions (35% / 65%)
    layout->setStretchFactor(leftPanel_, 35);
    layout->setStretchFactor(rightPanel_, 65);
    
    std::cout << "[POSModeContainer] ✓ Layout created" << std::endl;
}

// 2. REPLACE your createLeftPanel method to ensure proper layout:
void POSModeContainer::createLeftPanel() {
    if (!leftPanel_) {
        std::cerr << "[POSModeContainer] ERROR: leftPanel_ is null!" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Creating clean left panel layout..." << std::endl;
    
    // Create simple vertical layout for left panel
    auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    leftLayout->setContentsMargins(0, 0, 0, 0);  // Let CSS handle padding
    leftLayout->setSpacing(0);                    // Let CSS handle spacing
    
    // Ensure the panel maintains visibility
    leftPanel_->show();
    leftPanel_->setHidden(false);
    
    std::cout << "[POSModeContainer] ✓ Clean left panel layout created" << std::endl;
}

// 3. REPLACE your createRightPanel method for clean right panel:
void POSModeContainer::createRightPanel() {
    if (!rightPanel_) {
        std::cerr << "[POSModeContainer] ERROR: rightPanel_ is null!" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Creating clean right panel layout..." << std::endl;
    
    // Create simple vertical layout for right panel
    auto rightLayout = rightPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    rightLayout->setContentsMargins(15, 15, 15, 15);  // Some internal padding
    rightLayout->setSpacing(15);                       // Space between header and work area
    
    // Header container for title and controls
    auto headerContainer = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->setStyleClass("d-flex justify-content-between align-items-center");
    headerContainer->setHeight(Wt::WLength(50, Wt::LengthUnit::Pixel)); // Fixed header height
    
    // Work area title
    workAreaTitle_ = headerContainer->addNew<Wt::WText>("🍽️ Order Management");
    workAreaTitle_->setStyleClass("h4 text-primary mb-0");
    
    // Control buttons container
    auto controlsContainer = headerContainer->addNew<Wt::WContainerWidget>();
    controlsContainer->setStyleClass("d-flex gap-2");
    
    // Toggle button to go back to orders view
    toggleOrdersButton_ = controlsContainer->addNew<Wt::WPushButton>("📋 View Orders");
    toggleOrdersButton_->setStyleClass("btn btn-outline-info btn-sm");
    toggleOrdersButton_->clicked().connect([this]() {
        std::cout << "[POSModeContainer] Toggle Orders button clicked" << std::endl;
        showOrderEntryMode();
    });
    toggleOrdersButton_->hide(); // Hidden initially
    
    // Send to Kitchen button
    sendToKitchenButton_ = controlsContainer->addNew<Wt::WPushButton>("🚀 Send to Kitchen");
    sendToKitchenButton_->setStyleClass("btn btn-success btn-sm");
    sendToKitchenButton_->clicked().connect([this]() {
        std::cout << "[POSModeContainer] Send to Kitchen button clicked" << std::endl;
        sendCurrentOrderToKitchen();
    });
    sendToKitchenButton_->hide(); // Hidden initially
    
    // Work area - this will contain the dynamic content (Order Entry or Current Order)
    workArea_ = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 1);
    workArea_->setStyleClass("pos-dynamic-work-area");
    
    // CRITICAL: Ensure work area is visible and flexible
    workArea_->show();
    workArea_->setHidden(false);
    workArea_->setAttributeValue("style", 
        "display: block !important; visibility: visible !important; "
        "width: 100% !important; flex: 1 1 auto !important; "
        "overflow-y: auto !important;");
    
    // Ensure the right panel maintains visibility
    rightPanel_->show();
    rightPanel_->setHidden(false);
    
    std::cout << "[POSModeContainer] ✓ Clean right panel layout created with header and work area" << std::endl;
}

// ============================================================================
// FIXED UI MODE SWITCHING METHODS
// ============================================================================

// 5. ENHANCE your clearLeftPanel method to preserve visibility:
void POSModeContainer::clearLeftPanel() {
    if (!leftPanel_) {
        std::cout << "[POSModeContainer] leftPanel_ is null - cannot clear" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Clearing left panel (preserving layout and visibility)..." << std::endl;
    
    // Clear component pointers first
    activeOrdersDisplay_ = nullptr;
    menuDisplay_ = nullptr;
    
    // Clear children safely
    try {
        leftPanel_->clear();
        
        // Recreate the layout immediately
        auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
        leftLayout->setContentsMargins(0, 0, 0, 0);
        leftLayout->setSpacing(0);
        
        // CRITICAL: Restore visibility
        leftPanel_->show();
        leftPanel_->setHidden(false);
        leftPanel_->setAttributeValue("style", 
            "display: flex !important; flex-direction: column !important; "
            "visibility: visible !important; opacity: 1 !important; "
            "width: 100% !important; height: 100% !important;");
        
        std::cout << "[POSModeContainer] ✓ Left panel cleared and visibility restored" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error clearing left panel: " << e.what() << std::endl;
    }
}

void POSModeContainer::clearWorkArea() {
    if (workArea_ && workArea_->children().size() > 0) {
        std::cout << "[POSModeContainer] Clearing work area" << std::endl;
        
        // Clear component pointers
        orderEntryPanel_ = nullptr;
        currentOrderDisplay_ = nullptr;
        
        // Process events to ensure clean destruction
        Wt::WApplication::instance()->processEvents();
        workArea_->clear();
    }
}

// ============================================================================
// FIXED EVENT HANDLING
// ============================================================================

void POSModeContainer::setupEventListeners() {
    if (!eventManager_) return;
    
    // Listen for current order changes
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CURRENT_ORDER_CHANGED,
            [this](const std::any& data) { 
                if (isDestroying_) return;
                
                std::cout << "[POSModeContainer] CURRENT_ORDER_CHANGED event received" << std::endl;
                handleCurrentOrderChanged(data);
            })
    );
    
    // Listen for order creation
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED,
            [this](const std::any& data) { 
                if (isDestroying_) return;
                
                std::cout << "[POSModeContainer] ORDER_CREATED event received" << std::endl;
                handleOrderCreated(data);
            })
    );
    
    // Listen for order modifications to update Send to Kitchen button
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_MODIFIED,
            [this](const std::any& data) { 
                if (isDestroying_) return;
                updateSendToKitchenButton();
            })
    );
    
    std::cout << "[POSModeContainer] Event listeners setup complete" << std::endl;
}

// ============================================================================
// MINIMAL FIX - Replace only these specific methods in your existing POSModeContainer.cpp
// Don't replace the whole file - just replace these individual methods
// ============================================================================

// 1. REPLACE the handleCurrentOrderChanged method:
void POSModeContainer::handleCurrentOrderChanged(const std::any& eventData) {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Ignoring current order changed event - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Current order changed event - checking mode switch" << std::endl;
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    UIMode targetMode = hasCurrentOrder ? UI_MODE_ORDER_EDIT : UI_MODE_ORDER_ENTRY;
    
    std::cout << "[POSModeContainer] hasCurrentOrder: " << hasCurrentOrder 
              << ", currentUIMode: " << currentUIMode_ << ", targetMode: " << targetMode << std::endl;
    
    // If mode needs to change, switch immediately
    if (currentUIMode_ != targetMode) {
        std::cout << "[POSModeContainer] Mode change required" << std::endl;
        
        if (hasCurrentOrder) {
            // Switch to order edit mode
            std::cout << "[POSModeContainer] Switching to ORDER_EDIT mode" << std::endl;
            showOrderEditMode();
        } else {
            // Switch to order entry mode  
            std::cout << "[POSModeContainer] Switching to ORDER_ENTRY mode" << std::endl;
            showOrderEntryMode();
        }
    } else {
        std::cout << "[POSModeContainer] Mode unchanged - just refreshing UI" << std::endl;
        updateSendToKitchenButton();
    }
}

// 2. ADD this new method (showOrderEditMode):
void POSModeContainer::showOrderEditMode() {
    if (isDestroying_) return;
    
    std::cout << "[POSModeContainer] ==> SWITCHING TO ORDER_EDIT MODE" << std::endl;
    
    // Clear component pointers to prevent refresh calls during destruction
    orderEntryPanel_ = nullptr;
    menuDisplay_ = nullptr;
    currentOrderDisplay_ = nullptr;
    
    try {
        // Clear left panel (remove Active Orders)
        if (leftPanel_ && leftPanel_->children().size() > 0) {
            std::cout << "[POSModeContainer] Clearing left panel for MenuDisplay" << std::endl;
            leftPanel_->clear();
            
            // Recreate layout
            auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
            leftLayout->setContentsMargins(0, 0, 0, 0);
            leftLayout->setSpacing(0);
        }
        
        // Clear work area (remove Order Entry Panel)
        if (workArea_ && workArea_->children().size() > 0) {
            std::cout << "[POSModeContainer] Clearing work area for CurrentOrderDisplay" << std::endl;
            workArea_->clear();
        }
        
        // LEFT PANEL: Create MenuDisplay
        std::cout << "[POSModeContainer] Creating MenuDisplay in left panel" << std::endl;
        menuDisplay_ = leftPanel_->addWidget(
            std::make_unique<MenuDisplay>(posService_, eventManager_));
        
        // RIGHT PANEL: Create CurrentOrderDisplay  
        std::cout << "[POSModeContainer] Creating CurrentOrderDisplay in work area" << std::endl;
        currentOrderDisplay_ = workArea_->addWidget(
            std::make_unique<CurrentOrderDisplay>(posService_, eventManager_));
        
        // Update UI state
        currentUIMode_ = UI_MODE_ORDER_EDIT;
        
        // Update title
        if (workAreaTitle_) {
            auto order = posService_->getCurrentOrder();
            if (order) {
                workAreaTitle_->setText("🍽️ Editing Order #" + std::to_string(order->getOrderId()) + 
                                       " - " + order->getTableIdentifier());
                workAreaTitle_->setStyleClass("h4 text-success mb-0");
            }
        }
        
        // Show toggle button to go back to orders view
        if (toggleOrdersButton_) {
            toggleOrdersButton_->show();
        }
        
        // Update send to kitchen button
        updateSendToKitchenButton();
        
        std::cout << "[POSModeContainer] ✅ ORDER_EDIT mode activated successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] ERROR in showOrderEditMode: " << e.what() << std::endl;
        
        // Fallback to order entry mode
        currentUIMode_ = UI_MODE_NONE;
        showOrderEntryMode();
    }
}

// 3. REPLACE your showOrderEntryMode method with this definitive fix:
// ============================================================================
// SIMPLE TEST VERSION - Replace showOrderEntryMode with this basic version
// This will help us confirm the layout works before dealing with ActiveOrdersDisplay
// ============================================================================

// 4. ENHANCE your showOrderEntryMode method with better component creation:
/* void POSModeContainer::showOrderEntryMode() {
    if (isDestroying_) return;
    
    std::cout << "[POSModeContainer] ==> SWITCHING TO ORDER_ENTRY MODE (Clean Version)" << std::endl;
    
    // Clear existing components
    clearLeftPanel();
    clearWorkArea();
    
    try {
        // LEFT PANEL: Create ActiveOrdersDisplay
        std::cout << "[POSModeContainer] Creating ActiveOrdersDisplay in left panel..." << std::endl;
        
        if (leftPanel_) {
            // Ensure left panel has proper layout
            if (!leftPanel_->layout()) {
                auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
                leftLayout->setContentsMargins(0, 0, 0, 0);
                leftLayout->setSpacing(0);
            }
            
            // Create ActiveOrdersDisplay
            activeOrdersDisplay_ = leftPanel_->addWidget(
                std::make_unique<ActiveOrdersDisplay>(posService_, eventManager_, true));
            
            activeOrdersDisplay_->setStyleClass("active-orders-display");
            activeOrdersDisplay_->show();
            activeOrdersDisplay_->setHidden(false);
            
            std::cout << "[POSModeContainer] ✓ ActiveOrdersDisplay created and visible" << std::endl;
        }
        
        // RIGHT PANEL: Create OrderEntryPanel in work area
        std::cout << "[POSModeContainer] Creating OrderEntryPanel in work area..." << std::endl;
        
        if (workArea_) {
            orderEntryPanel_ = workArea_->addWidget(
                std::make_unique<OrderEntryPanel>(posService_, eventManager_));
            
            orderEntryPanel_->setStyleClass("order-entry-panel");
            orderEntryPanel_->show();
            orderEntryPanel_->setHidden(false);
            
            std::cout << "[POSModeContainer] ✓ OrderEntryPanel created and visible" << std::endl;
        }
        
        // Update UI state
        currentUIMode_ = UI_MODE_ORDER_ENTRY;
        
        // Update title and buttons
        if (workAreaTitle_) {
            workAreaTitle_->setText("🍽️ Order Management");
            workAreaTitle_->setStyleClass("h4 text-primary mb-0");
        }
        
        if (toggleOrdersButton_) {
            toggleOrdersButton_->hide();
        }
        
        if (sendToKitchenButton_) {
            sendToKitchenButton_->hide();
        }
        
        // Force a refresh of both components
        if (activeOrdersDisplay_) {
            activeOrdersDisplay_->refresh();
        }
        if (orderEntryPanel_) {
            orderEntryPanel_->refresh();
        }
        
        // Force panel visibility one more time
        if (leftPanel_) {
            leftPanel_->show();
            leftPanel_->setHidden(false);
        }
        if (rightPanel_) {
            rightPanel_->show();
            rightPanel_->setHidden(false);
        }
        
        std::cout << "[POSModeContainer] ✅ ORDER_ENTRY mode activated successfully" << std::endl;
        std::cout << "[POSModeContainer]   - ActiveOrdersDisplay in left panel: " 
                  << (activeOrdersDisplay_ ? "CREATED" : "FAILED") << std::endl;
        std::cout << "[POSModeContainer]   - OrderEntryPanel in work area: " 
                  << (orderEntryPanel_ ? "CREATED" : "FAILED") << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] ERROR in showOrderEntryMode: " << e.what() << std::endl;
    }
}
 */

 void POSModeContainer::showOrderEntryMode() {
    if (isDestroying_) return;
    
    std::cout << "[POSModeContainer] ==> SIMPLE TEST" << std::endl;
    
    // Clear left panel
    if (leftPanel_) {
        leftPanel_->clear();
        auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
        leftLayout->setContentsMargins(10, 10, 10, 10);
        leftLayout->setSpacing(10);
        
        // Add simple test content
        auto testText = leftPanel_->addNew<Wt::WText>("TEST: Active Orders Should Appear Here");
        testText->setStyleClass("h4 text-danger");
        testText->setAttributeValue("style", "background: yellow; padding: 10px; border: 2px solid black;");
        
        auto instructionText = leftPanel_->addNew<Wt::WText>(
            "If you can see this yellow text, the left panel is working. "
            "The ActiveOrdersDisplay component creation might be failing.");
        instructionText->setAttributeValue("style", "background: lightblue; padding: 10px; margin-top: 10px;");
    }
    
    currentUIMode_ = UI_MODE_ORDER_ENTRY;
    std::cout << "[TEST] Simple test content added" << std::endl;
}

void POSModeContainer::handleOrderCreated(const std::any& eventData) {
    if (isDestroying_) return;
    
    std::cout << "[POSModeContainer] Order created - checking mode switch" << std::endl;
    
    // Refresh active orders display if it exists
    if (activeOrdersDisplay_ && currentUIMode_ == UI_MODE_ORDER_ENTRY) {
        activeOrdersDisplay_->refresh();
    }
    
    // The current order change event should handle the mode switch
    // but we can trigger it manually if needed
    bool hasCurrentOrder = this->hasCurrentOrder();
    if (hasCurrentOrder && currentUIMode_ != UI_MODE_ORDER_EDIT) {
        std::cout << "[POSModeContainer] New order created - switching to edit mode" << std::endl;
        showOrderEditMode();
    }
}

// ============================================================================
// SEND TO KITCHEN FUNCTIONALITY
// ============================================================================

// 6. REPLACE the sendCurrentOrderToKitchen method:
void POSModeContainer::sendCurrentOrderToKitchen() {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping sendCurrentOrderToKitchen - container is being destroyed" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Sending current order to kitchen" << std::endl;
    
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
        // Send order to kitchen
        bool success = posService_->sendCurrentOrderToKitchen();
        
        if (success) {
            std::cout << "[POSModeContainer] ✅ Order #" << orderId << " sent to kitchen successfully" << std::endl;
            
            // Show success feedback
            showOrderSentFeedback(orderId);
            
            // Clear the current order - this will trigger the mode switch back to order entry
            posService_->setCurrentOrder(nullptr);
            
        } else {
            std::cout << "[POSModeContainer] ❌ Failed to send order #" << orderId << " to kitchen" << std::endl;
            
            // Still clear the current order
            posService_->setCurrentOrder(nullptr);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error during send to kitchen: " << e.what() << std::endl;
        
        // Emergency cleanup
        if (posService_) {
            posService_->setCurrentOrder(nullptr);
        }
    }
}

void POSModeContainer::showOrderSentFeedback(int orderId) {
    if (workAreaTitle_) {
        try {
            workAreaTitle_->setText("✅ Order #" + std::to_string(orderId) + " sent to kitchen!");
            workAreaTitle_->setStyleClass("h4 text-success mb-0");
            
            // Reset title after 3 seconds
            Wt::WTimer::singleShot(std::chrono::milliseconds(3000), [this]() {
                if (!isDestroying_ && workAreaTitle_ && currentUIMode_ == UI_MODE_ORDER_ENTRY) {
                    workAreaTitle_->setText("🍽️ Order Management");
                    workAreaTitle_->setStyleClass("h4 text-primary mb-0");
                }
            });
            
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error showing feedback: " << e.what() << std::endl;
        }
    }
}

// ============================================================================
// HELPER METHODS
// ============================================================================

bool POSModeContainer::hasCurrentOrder() const {
    return posService_ && posService_->getCurrentOrder() != nullptr;
}

bool POSModeContainer::hasOrderWithItems() const {
    auto order = posService_ ? posService_->getCurrentOrder() : nullptr;
    return order && !order->getItems().empty();
}

void POSModeContainer::updateSendToKitchenButton() {
    if (isDestroying_ || !sendToKitchenButton_) return;
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    bool hasItems = hasOrderWithItems();
    
    std::cout << "[POSModeContainer] Updating Send to Kitchen button - hasOrder: " 
              << hasCurrentOrder << ", hasItems: " << hasItems << std::endl;
    
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
        std::cerr << "[POSModeContainer] Error updating button: " << e.what() << std::endl;
    }
}

// ============================================================================
// PUBLIC INTERFACE METHODS
// ============================================================================

void POSModeContainer::refresh() {
    if (isDestroying_) return;
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    UIMode targetMode = hasCurrentOrder ? UI_MODE_ORDER_EDIT : UI_MODE_ORDER_ENTRY;
    
    std::cout << "[POSModeContainer] Refresh called - hasOrder: " << hasCurrentOrder 
              << ", currentMode: " << currentUIMode_ << ", targetMode: " << targetMode << std::endl;
    
    if (currentUIMode_ != targetMode) {
        std::cout << "[POSModeContainer] Mode change required during refresh" << std::endl;
        if (hasCurrentOrder) {
            showOrderEditMode();
        } else {
            showOrderEntryMode();
        }
    } else {
        std::cout << "[POSModeContainer] Refreshing current mode components" << std::endl;
        refreshDataOnly();
    }
}

void POSModeContainer::refreshDataOnly() {
    if (isDestroying_) return;
    
    std::cout << "[POSModeContainer] Data-only refresh" << std::endl;
    
    // Refresh active components
    if (activeOrdersDisplay_ && activeOrdersDisplay_->parent()) {
        try {
            activeOrdersDisplay_->refresh();
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] ActiveOrders refresh failed: " << e.what() << std::endl;
        }
    }
    
    if (orderEntryPanel_ && orderEntryPanel_->parent()) {
        try {
            orderEntryPanel_->refresh();
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] OrderEntry refresh failed: " << e.what() << std::endl;
        }
    }
    
    if (menuDisplay_ && menuDisplay_->parent()) {
        try {
            menuDisplay_->refresh();
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] MenuDisplay refresh failed: " << e.what() << std::endl;
        }
    }
    
    if (currentOrderDisplay_ && currentOrderDisplay_->parent()) {
        try {
            currentOrderDisplay_->refresh();
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] CurrentOrder refresh failed: " << e.what() << std::endl;
        }
    }
    
    updateSendToKitchenButton();
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

// ============================================================================
// DEPRECATED/COMPATIBILITY METHODS
// ============================================================================

void POSModeContainer::showOrderEntry() {
    showOrderEntryMode();
}

void POSModeContainer::showOrderEdit() {
    showOrderEditMode();
}

void POSModeContainer::hideActiveOrdersDisplay() {
    // This is now handled by the mode switching
    if (currentUIMode_ != UI_MODE_ORDER_EDIT) {
        showOrderEditMode();
    }
}

void POSModeContainer::showActiveOrdersDisplay() {
    // This is now handled by the mode switching
    if (currentUIMode_ != UI_MODE_ORDER_ENTRY) {
        showOrderEntryMode();
    }
}

// 4. REPLACE the updateWorkArea method:
void POSModeContainer::updateWorkArea() {
    if (isDestroying_) {
        std::cout << "[POSModeContainer] Skipping updateWorkArea - container is being destroyed" << std::endl;
        return;
    }
    
    bool hasCurrentOrder = this->hasCurrentOrder();
    UIMode targetMode = hasCurrentOrder ? UI_MODE_ORDER_EDIT : UI_MODE_ORDER_ENTRY;
    
    std::cout << "[POSModeContainer] updateWorkArea() called" << std::endl;
    std::cout << "[POSModeContainer]   hasCurrentOrder: " << hasCurrentOrder << std::endl;
    std::cout << "[POSModeContainer]   currentUIMode: " << currentUIMode_ << std::endl;
    std::cout << "[POSModeContainer]   targetMode: " << (targetMode == UI_MODE_ORDER_EDIT ? "EDIT" : "ENTRY") << std::endl;
    
    // Only recreate UI if the mode has actually changed
    if (currentUIMode_ != targetMode) {
        std::cout << "[POSModeContainer] Mode change detected - switching modes" << std::endl;
        
        if (targetMode == UI_MODE_ORDER_EDIT) {
            showOrderEditMode();
        } else {
            showOrderEntryMode();
        }
    } else {
        std::cout << "[POSModeContainer] Mode unchanged - updating buttons only" << std::endl;
        updateSendToKitchenButton();
        
        // Update title if in edit mode
        if (currentUIMode_ == UI_MODE_ORDER_EDIT && hasCurrentOrder) {
            if (workAreaTitle_) {
                auto order = posService_->getCurrentOrder();
                if (order) {
                    workAreaTitle_->setText("🍽️ Editing Order #" + std::to_string(order->getOrderId()) + 
                                           " - " + order->getTableIdentifier());
                }
            }
        }
    }
}