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
    , middleContainer_(nullptr)
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
    setId("pos-mode-container");  // CRITICAL: Set ID so CSS #pos-mode-container selectors work

    // Force container visibility with column layout
    show();
    setHidden(false);
    setAttributeValue("style",
        "display: flex !important; flex-direction: column !important; "
        "height: calc(100vh - 60px) !important; width: 100% !important; "
        "padding: 0 !important; margin: 0 !important; background-color: #fafafa;");

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
    // Note: createLeftPanel() and createRightPanel() are no longer called here
    // Panel content is now managed directly in showOrderEntryMode() and showOrderEditMode()
}

// ============================================================================
// STARTUP LAYOUT FIX - Replace these methods in POSModeContainer.cpp
// ============================================================================

// POSModeContainer: horizontal left/right panel split
// Left panel has its own "Active Orders" sub-header
void POSModeContainer::setupLayout() {
    std::cout << "[POSModeContainer] Setting up left/right panel layout..." << std::endl;

    // Horizontal flex layout for left/right panels
    setAttributeValue("style",
        "display: flex !important; flex-direction: row !important; "
        "width: 100% !important; height: 100% !important; "
        "padding: 10px !important; gap: 10px !important; box-sizing: border-box !important; "
        "background: #f5f5f5;");

    // LEFT PANEL - 30% width with sub-header
    leftPanel_ = addNew<Wt::WContainerWidget>();
    leftPanel_->setStyleClass("pos-left-panel");
    leftPanel_->setAttributeValue("style",
        "background: #ffffff; padding: 0; margin: 0; "
        "width: 30% !important; min-width: 280px; max-width: 400px; "
        "display: flex; flex-direction: column; "
        "border: 1px solid #e0e0e0; border-radius: 6px; overflow: hidden;");

    // LEFT PANEL SUB-HEADER (blue "Active Orders" bar)
    auto leftHeader = leftPanel_->addNew<Wt::WContainerWidget>();
    leftHeader->setAttributeValue("style",
        "background: #0d6efd; padding: 10px 12px; "
        "display: flex; justify-content: space-between; align-items: center; "
        "flex-shrink: 0;");

    auto titleContainer = leftHeader->addNew<Wt::WContainerWidget>();
    titleContainer->setAttributeValue("style", "display: flex; align-items: center; gap: 8px;");

    auto headerIcon = titleContainer->addNew<Wt::WText>("📋");
    headerIcon->setAttributeValue("style", "font-size: 1.1rem;");

    auto headerText = titleContainer->addNew<Wt::WText>("Active Orders");
    headerText->setAttributeValue("style", "color: white; font-size: 1rem; font-weight: 600;");

    auto refreshBtn = leftHeader->addNew<Wt::WPushButton>("↻");
    refreshBtn->setAttributeValue("style",
        "background: rgba(255,255,255,0.2); border: none; color: white; "
        "padding: 4px 10px; border-radius: 4px; cursor: pointer; font-size: 1rem;");
    refreshBtn->clicked().connect([this]() {
        showOrderEntryMode();
    });

    // LEFT PANEL CONTENT AREA (where orders list goes)
    auto leftContent = leftPanel_->addNew<Wt::WContainerWidget>();
    leftContent->setAttributeValue("style",
        "flex: 1; padding: 10px; overflow-y: auto; background: #fafafa;");
    middleContainer_ = leftContent;

    // RIGHT PANEL - fills remaining space (70%)
    rightPanel_ = addNew<Wt::WContainerWidget>();
    rightPanel_->setStyleClass("pos-right-panel");
    rightPanel_->setAttributeValue("style",
        "background: #ffffff; margin: 0; padding: 12px; "
        "flex: 1; min-width: 0; "
        "border: 1px solid #e0e0e0; border-radius: 6px; "
        "display: flex; flex-direction: column; overflow: hidden;");

    std::cout << "[POSModeContainer] Layout setup complete - left/right panels ready" << std::endl;
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

// 4. FIXED clearLeftPanel - only clears content area, preserves header
void POSModeContainer::clearLeftPanel() {
    std::cout << "[POSModeContainer] Clearing left panel content area..." << std::endl;

    // Clear component pointers
    activeOrdersDisplay_ = nullptr;
    menuDisplay_ = nullptr;

    // Only clear the content area (middleContainer_), not the whole leftPanel_
    // leftPanel_ contains the blue header which should be preserved
    if (middleContainer_) {
        try {
            middleContainer_->clear();
            std::cout << "[POSModeContainer] ✓ Left panel content cleared" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[POSModeContainer] Error clearing left panel content: " << e.what() << std::endl;
        }
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

// 2. FIXED showOrderEditMode - uses middleContainer_ for left content, rightPanel_ for right content
void POSModeContainer::showOrderEditMode() {
    if (isDestroying_) return;

    std::cout << "[POSModeContainer] ==> SWITCHING TO ORDER_EDIT MODE" << std::endl;

    // Clear component pointers to prevent refresh calls during destruction
    orderEntryPanel_ = nullptr;
    menuDisplay_ = nullptr;
    currentOrderDisplay_ = nullptr;
    activeOrdersDisplay_ = nullptr;

    try {
        // Clear left panel content area (middleContainer_), NOT leftPanel_ which has the header
        if (middleContainer_) {
            std::cout << "[POSModeContainer] Clearing left panel content for MenuDisplay" << std::endl;
            middleContainer_->clear();

            // LEFT PANEL: Create MenuDisplay
            std::cout << "[POSModeContainer] Creating MenuDisplay in left panel content" << std::endl;
            menuDisplay_ = middleContainer_->addWidget(
                std::make_unique<MenuDisplay>(posService_, eventManager_));
        }

        // Clear right panel and add CurrentOrderDisplay
        if (rightPanel_) {
            std::cout << "[POSModeContainer] Clearing right panel for CurrentOrderDisplay" << std::endl;
            rightPanel_->clear();

            // RIGHT PANEL: Create CurrentOrderDisplay
            std::cout << "[POSModeContainer] Creating CurrentOrderDisplay in right panel" << std::endl;
            currentOrderDisplay_ = rightPanel_->addWidget(
                std::make_unique<CurrentOrderDisplay>(posService_, eventManager_));
        }

        // Update UI state
        currentUIMode_ = UI_MODE_ORDER_EDIT;

        std::cout << "[POSModeContainer] ✅ ORDER_EDIT mode activated successfully" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] ERROR in showOrderEditMode: " << e.what() << std::endl;

        // Fallback to order entry mode
        currentUIMode_ = UI_MODE_NONE;
        showOrderEntryMode();
    }
}

// 3. FIXED showOrderEntryMode - uses middleContainer_ for left panel content
void POSModeContainer::showOrderEntryMode() {
    if (isDestroying_) return;

    std::cout << "[POSModeContainer] ==> Showing ORDER_ENTRY MODE" << std::endl;

    // Clear component pointers
    orderEntryPanel_ = nullptr;
    menuDisplay_ = nullptr;
    currentOrderDisplay_ = nullptr;
    activeOrdersDisplay_ = nullptr;

    try {
        // Get orders from service
        auto orders = posService_->getActiveOrders();

        // Clear only the content area (middleContainer_), NOT leftPanel_ which has the header
        if (middleContainer_) {
            middleContainer_->clear();

            if (orders.empty()) {
                // Simple concise empty message
                auto emptyContainer = middleContainer_->addNew<Wt::WContainerWidget>();
                emptyContainer->setAttributeValue("style",
                    "display: flex; align-items: center; justify-content: center; "
                    "background: #f8f9fa; height: 100%; border-radius: 4px;");

                auto emptyText = emptyContainer->addNew<Wt::WText>("No active orders");
                emptyText->setAttributeValue("style", "color: #6c757d; font-style: italic;");
            } else {
                // Orders table
                auto tableContainer = middleContainer_->addNew<Wt::WContainerWidget>();
                tableContainer->setAttributeValue("style",
                    "background: white; padding: 8px; border-radius: 4px; overflow-y: auto;");

                auto ordersTable = tableContainer->addNew<Wt::WTable>();
                ordersTable->setStyleClass("table table-hover table-sm mb-0");
                ordersTable->setHeaderCount(1);

                ordersTable->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("#"));
                ordersTable->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Table"));
                ordersTable->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Status"));
                ordersTable->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Total"));

                ordersTable->rowAt(0)->setStyleClass("table-primary");

                int row = 1;
                for (const auto& order : orders) {
                    // Order number - clickable
                    auto orderNumBtn = ordersTable->elementAt(row, 0)->addWidget(
                        std::make_unique<Wt::WPushButton>("#" + std::to_string(order->getOrderId())));
                    orderNumBtn->setStyleClass("btn btn-link btn-sm p-0 text-decoration-none");

                    // Capture order ID for click handler
                    int orderId = order->getOrderId();
                    orderNumBtn->clicked().connect([this, orderId]() {
                        std::cout << "[POSModeContainer] Opening order #" << orderId << std::endl;
                        openOrderForEditing(orderId);
                    });

                    // Table/Location
                    std::string tableInfo = order->getTableIdentifier();
                    if (tableInfo.empty()) tableInfo = "Takeout";
                    ordersTable->elementAt(row, 1)->addWidget(
                        std::make_unique<Wt::WText>(tableInfo));

                    // Status with badge
                    std::string statusClass = "badge ";
                    std::string statusText = Order::statusToString(order->getStatus());
                    if (statusText == "Pending") statusClass += "bg-warning text-dark";
                    else if (statusText == "In Progress") statusClass += "bg-info";
                    else if (statusText == "Ready") statusClass += "bg-success";
                    else if (statusText == "Completed") statusClass += "bg-secondary";
                    else statusClass += "bg-primary";

                    auto statusBadge = ordersTable->elementAt(row, 2)->addWidget(
                        std::make_unique<Wt::WText>(statusText));
                    statusBadge->setStyleClass(statusClass);

                    // Total
                    std::ostringstream totalStr;
                    totalStr << "$" << std::fixed << std::setprecision(2) << order->getTotal();
                    ordersTable->elementAt(row, 3)->addWidget(
                        std::make_unique<Wt::WText>(totalStr.str()));
                    ordersTable->elementAt(row, 3)->setStyleClass("fw-bold");

                    row++;
                }
            }

            std::cout << "[POSModeContainer] Active Orders list created with "
                      << orders.size() << " orders" << std::endl;
        }

        // RIGHT PANEL: Order Entry Panel
        std::cout << "[POSModeContainer] Creating OrderEntryPanel..." << std::endl;
        if (rightPanel_) {
            rightPanel_->clear();
            orderEntryPanel_ = rightPanel_->addNew<OrderEntryPanel>(posService_, eventManager_);

            if (orderEntryPanel_) {
                std::cout << "[POSModeContainer] OrderEntryPanel created" << std::endl;
            }
        }

        // Update UI state
        currentUIMode_ = UI_MODE_ORDER_ENTRY;

        std::cout << "[POSModeContainer] ORDER_ENTRY MODE - Both panels ready" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[POSModeContainer] ERROR in showOrderEntryMode: " << e.what() << std::endl;
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
        showOrderEditMode();
        std::cout << "[POSModeContainer] Order #" << order->getOrderId() << " opened for editing" << std::endl;
    }
}

void POSModeContainer::openOrderForEditing(int orderId) {
    if (!posService_) return;

    // Find the order by ID
    auto orders = posService_->getActiveOrders();
    for (const auto& order : orders) {
        if (order->getOrderId() == orderId) {
            openOrderForEditing(order);
            return;
        }
    }

    std::cerr << "[POSModeContainer] Order #" << orderId << " not found" << std::endl;
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