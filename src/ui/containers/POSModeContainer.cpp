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

    // Force container visibility
    show();
    setHidden(false);
    setAttributeValue("style",
        "display: flex !important; height: calc(100vh - 120px) !important; "
        "width: 100% !important; background-color: #fafafa;");

    // Initialize UI components
    initializeUI();
    setupEventListeners();

    // Force panel visibility after creation
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

// 1. REPLACE your setupLayout method with this fixed version:
void POSModeContainer::setupLayout() {
    std::cout << "[POSModeContainer] Setting up layout with forced visibility..." << std::endl;
    
    // Create main horizontal layout
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    // Create left panel first
    leftPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    // CRITICAL: Set explicit size for left panel
    leftPanel_->setWidth(Wt::WLength(30, Wt::LengthUnit::Percentage));
    leftPanel_->setMinimumSize(Wt::WLength(300, Wt::LengthUnit::Pixel), Wt::WLength::Auto);
    
    // Create right panel
    rightPanel_ = layout->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    // CRITICAL: Set explicit size for right panel  
    rightPanel_->setWidth(Wt::WLength(70, Wt::LengthUnit::Percentage));
    
    // Set stretch factors (this should work now with explicit widths)
    layout->setStretchFactor(leftPanel_, 3);   // 30%
    layout->setStretchFactor(rightPanel_, 7);  // 70%
    
    // Apply styling WITHOUT debug borders
    leftPanel_->setStyleClass("pos-left-panel");
    rightPanel_->setStyleClass("pos-right-panel");
    
    // FORCE VISIBILITY AND LAYOUT PROPERTIES
    leftPanel_->show();
    leftPanel_->setHidden(false);
    leftPanel_->setInline(false);
    
    rightPanel_->show();
    rightPanel_->setHidden(false);
    rightPanel_->setInline(false);
    
    // Debug output
    std::cout << "[DEBUG] Layout created with explicit sizing:" << std::endl;
    std::cout << "[DEBUG]   leftPanel_ width: 30%" << std::endl;
    std::cout << "[DEBUG]   rightPanel_ width: 70%" << std::endl;
    std::cout << "[DEBUG]   leftPanel_ visible: " << leftPanel_->isVisible() << std::endl;
    std::cout << "[DEBUG]   rightPanel_ visible: " << rightPanel_->isVisible() << std::endl;
    
    std::cout << "[POSModeContainer] ✓ Layout setup complete with forced panel visibility" << std::endl;
}

// 2. REPLACE your createLeftPanel method:
void POSModeContainer::createLeftPanel() {
    if (!leftPanel_) {
        std::cerr << "[POSModeContainer] ERROR: leftPanel_ is null in createLeftPanel!" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Creating left panel layout..." << std::endl;
    
    // Create layout for left panel
    auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    
    // Force panel properties
    leftPanel_->show();
    leftPanel_->setHidden(false);
    
    std::cout << "[POSModeContainer] ✓ Left panel layout created" << std::endl;
}

void POSModeContainer::createRightPanel() {
    auto rightLayout = rightPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);
    
    // Header container
    auto headerContainer = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->setStyleClass("d-flex justify-content-between align-items-center p-3");
    
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
    
    // Work area (changes based on state)
    workArea_ = rightLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 1);
    workArea_->setStyleClass("pos-dynamic-work-area");
}

// ============================================================================
// FIXED UI MODE SWITCHING METHODS
// ============================================================================

// 4. ENHANCE your clearLeftPanel method to preserve layout:
void POSModeContainer::clearLeftPanel() {
    if (!leftPanel_) {
        std::cout << "[POSModeContainer] leftPanel_ is null - cannot clear" << std::endl;
        return;
    }
    
    std::cout << "[POSModeContainer] Clearing left panel (preserving visibility)..." << std::endl;
    
    // Clear component pointers
    activeOrdersDisplay_ = nullptr;
    menuDisplay_ = nullptr;
    
    // Clear children if any exist
    if (leftPanel_->children().size() > 0) {
        try {
            leftPanel_->clear();
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error clearing left panel: " << e.what() << std::endl;
        }
    }
    
    // ALWAYS recreate layout and preserve visibility
    try {
        auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
        leftLayout->setContentsMargins(0, 0, 0, 0);
        leftLayout->setSpacing(0);
        
        // CRITICAL: Restore visibility and size
        leftPanel_->show();
        leftPanel_->setHidden(false);
        leftPanel_->setWidth(Wt::WLength(30, Wt::LengthUnit::Percentage));
        leftPanel_->setMinimumSize(Wt::WLength(280, Wt::LengthUnit::Pixel), Wt::WLength::Auto);
        
        std::cout << "[POSModeContainer] ✓ Left panel cleared and visibility restored" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] Error recreating left panel layout: " << e.what() << std::endl;
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

void POSModeContainer::showOrderEntryMode() {
    if (isDestroying_) return;
    
    std::cout << "[POSModeContainer] ==> SIMPLE TEST VERSION - ORDER_ENTRY MODE" << std::endl;
    
    // Clear component pointers
    orderEntryPanel_ = nullptr;
    menuDisplay_ = nullptr;
    currentOrderDisplay_ = nullptr;
    activeOrdersDisplay_ = nullptr;
    
    try {
        // Clear panels
        if (leftPanel_ && leftPanel_->children().size() > 0) {
            leftPanel_->clear();
            auto leftLayout = leftPanel_->setLayout(std::make_unique<Wt::WVBoxLayout>());
            leftLayout->setContentsMargins(10, 10, 10, 10);
            leftLayout->setSpacing(10);
        }
        
        if (workArea_ && workArea_->children().size() > 0) {
            workArea_->clear();
        }
        
        // LEFT PANEL: Simple test content first
        std::cout << "[POSModeContainer] Creating simple test content for left panel..." << std::endl;
        
        if (leftPanel_) {
            // Header
            auto headerText = leftPanel_->addNew<Wt::WText>("📋 Active Orders");
            headerText->setStyleClass("h4 text-primary mb-3");
            
            // Status text
            auto statusText = leftPanel_->addNew<Wt::WText>("Status: No active orders");
            statusText->setStyleClass("text-muted mb-3");
            
            // Sample table
            auto testTable = leftPanel_->addNew<Wt::WTable>();
            testTable->setStyleClass("table table-striped");
            
            // Table headers
            testTable->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Order #"));
            testTable->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Table"));
            testTable->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Status"));
            
            // Sample row
            testTable->elementAt(1, 0)->addWidget(std::make_unique<Wt::WText>("No orders"));
            testTable->elementAt(1, 1)->addWidget(std::make_unique<Wt::WText>("--"));
            testTable->elementAt(1, 2)->addWidget(std::make_unique<Wt::WText>("Ready"));
            
            // Instructions
            auto instructionText = leftPanel_->addNew<Wt::WText>("💡 Orders will appear here when created");
            instructionText->setStyleClass("text-info small mt-3");
            
            std::cout << "[POSModeContainer] ✓ Simple test content created for left panel" << std::endl;
        }
        
        // RIGHT PANEL: Order Entry Panel (unchanged)
        std::cout << "[POSModeContainer] Creating OrderEntryPanel..." << std::endl;
        if (workArea_) {
            orderEntryPanel_ = workArea_->addWidget(
                std::make_unique<OrderEntryPanel>(posService_, eventManager_));
            
            if (orderEntryPanel_) {
                std::cout << "[POSModeContainer] ✓ OrderEntryPanel created" << std::endl;
            }
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
        
        std::cout << "[POSModeContainer] ✅ SIMPLE TEST VERSION - Both panels should now be visible" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] ERROR in simple test version: " << e.what() << std::endl;
    }
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