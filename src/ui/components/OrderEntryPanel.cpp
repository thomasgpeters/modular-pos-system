#include "../../../include/ui/components/OrderEntryPanel.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WBorderLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WGroupBox.h>
#include <Wt/WMessageBox.h>
#include <Wt/WApplication.h>

#include <iostream>
#include <iomanip>  // For std::setprecision

// Constructor
OrderEntryPanel::OrderEntryPanel(std::shared_ptr<POSService> posService,
                                std::shared_ptr<EventManager> eventManager)
    : posService_(posService), eventManager_(eventManager),
      tableNumberEdit_(nullptr), newOrderButton_(nullptr),
      sendToKitchenButton_(nullptr), processPaymentButton_(nullptr),
      menuDisplay_(nullptr), currentOrderDisplay_(nullptr) {
    
    initializeUI();
}

// Complete initializeUI implementation
void OrderEntryPanel::initializeUI() {
    setStyleClass("order-entry-panel");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Table selection section
    layout->addWidget(createTableSelectionSection());
    
    // Create and add menu display
    auto menuDisplayWidget = std::make_unique<MenuDisplay>(posService_, eventManager_);
    menuDisplay_ = menuDisplayWidget.get();  // Store raw pointer for access
    layout->addWidget(std::move(menuDisplayWidget));
    
    // Create and add current order display
    auto currentOrderDisplayWidget = std::make_unique<CurrentOrderDisplay>(posService_, eventManager_);
    currentOrderDisplay_ = currentOrderDisplayWidget.get();  // Store raw pointer for access
    layout->addWidget(std::move(currentOrderDisplayWidget));
    
    // Order actions section
    layout->addWidget(createOrderActionsSection());
    
    setLayout(std::move(layout));
}

// Event handlers setup
void OrderEntryPanel::setupEventHandlers() {
    // Table number change handler
    if (tableNumberEdit_) {
        tableNumberEdit_->changed().connect([this]() {
            onTableNumberChanged();
        });
    }
    
    // Button click handlers
    if (newOrderButton_) {
        newOrderButton_->clicked().connect([this]() {
            onNewOrderClicked();
        });
    }
    
    if (sendToKitchenButton_) {
        sendToKitchenButton_->clicked().connect([this]() {
            onSendToKitchenClicked();
        });
    }
    
    if (processPaymentButton_) {
        processPaymentButton_->clicked().connect([this]() {
            onProcessPaymentClicked();
        });
    }
    
    // Subscribe to business events
    if (eventManager_) {
        auto orderCreatedHandle = eventManager_->subscribe(POSEvents::ORDER_CREATED,
            [this](const std::any& data) { handleOrderCreated(data); });
        eventSubscriptions_.push_back(orderCreatedHandle);
        
        auto orderModifiedHandle = eventManager_->subscribe(POSEvents::ORDER_MODIFIED,
            [this](const std::any& data) { handleOrderModified(data); });
        eventSubscriptions_.push_back(orderModifiedHandle);
    }
}

void OrderEntryPanel::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "Warning: EventManager not available for OrderEntryPanel" << std::endl;
        return;
    }
    
    // Subscribe to order events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_CREATED,
            [this](const std::any& data) { handleOrderCreated(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_MODIFIED,
            [this](const std::any& data) { handleOrderModified(data); }));
    
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::ORDER_SENT_TO_KITCHEN,
            [this](const std::any& data) { handleCurrentOrderChanged(data); }));
    
    std::cout << "✓ OrderEntryPanel event listeners setup complete" << std::endl;
}

// Fix 1: Correct table selection section creation
std::unique_ptr<Wt::WWidget> OrderEntryPanel::createTableSelectionSection() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("table-selection-section mb-3 p-3 border rounded");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Table number label
    auto label = std::make_unique<Wt::WText>("Table Number:");
    label->addStyleClass("form-label");
    layout->addWidget(std::move(label));
    
    // Table number input - CORRECT WAY
    auto tableEdit = std::make_unique<Wt::WSpinBox>();
    tableEdit->setRange(1, 99);
    tableEdit->setValue(1);
    tableEdit->addStyleClass("form-control");
    
    // Store pointer for later access
    tableNumberEdit_ = tableEdit.get();
    
    // Add to layout using move
    layout->addWidget(std::move(tableEdit));
    
    container->setLayout(std::move(layout));
    return container;
}


// Fix 2: Correct order actions section creation
std::unique_ptr<Wt::WWidget> OrderEntryPanel::createOrderActionsSection() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("order-actions-section mb-3");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // New order button
    auto newOrderBtn = std::make_unique<Wt::WPushButton>("Start New Order");
    newOrderBtn->addStyleClass("btn btn-primary mb-2");
    newOrderButton_ = newOrderBtn.get();
    layout->addWidget(std::move(newOrderBtn));
    
    // Send to kitchen button
    auto sendBtn = std::make_unique<Wt::WPushButton>("Send to Kitchen");
    sendBtn->addStyleClass("btn btn-warning mb-2");
    sendBtn->setEnabled(false); // Initially disabled
    sendToKitchenButton_ = sendBtn.get();
    layout->addWidget(std::move(sendBtn));
    
    // Process payment button
    auto paymentBtn = std::make_unique<Wt::WPushButton>("Process Payment");
    paymentBtn->addStyleClass("btn btn-success");
    paymentBtn->setEnabled(false); // Initially disabled
    processPaymentButton_ = paymentBtn.get();
    layout->addWidget(std::move(paymentBtn));
    
    container->setLayout(std::move(layout));
    return container;
}

void OrderEntryPanel::refresh() {
    if (menuDisplay_) {
        menuDisplay_->refresh();
    }
    
    if (currentOrderDisplay_) {
        currentOrderDisplay_->refresh();
    }
    
    updateOrderActionButtons();
    
    std::cout << "OrderEntryPanel refreshed" << std::endl;
}

int OrderEntryPanel::getTableNumber() const {
    return tableNumberEdit_ ? tableNumberEdit_->value() : 1;
}

void OrderEntryPanel::setTableNumber(int tableNumber) {
    if (tableNumberEdit_) {
        tableNumberEdit_->setValue(tableNumber);
    }
}

void OrderEntryPanel::setOrderEntryEnabled(bool enabled) {
    if (tableNumberEdit_) {
        tableNumberEdit_->setEnabled(enabled);
    }
    
    if (newOrderButton_) {
        newOrderButton_->setEnabled(enabled);
    }
    
    if (menuDisplay_) {
        // MenuDisplay doesn't have a setEnabled method, but we could add one
        // For now, we'll just update the visual state
    }
    
    if (currentOrderDisplay_) {
        currentOrderDisplay_->setEditable(enabled);
    }
    
    std::cout << "Order entry " << (enabled ? "enabled" : "disabled") << std::endl;
}

// =================================================================
// Event Handlers
// =================================================================

void OrderEntryPanel::handleOrderCreated(const std::any& eventData) {
    std::cout << "Order created event received in OrderEntryPanel" << std::endl;
    updateOrderActionButtons();
}

void OrderEntryPanel::handleOrderModified(const std::any& eventData) {
    std::cout << "Order modified event received in OrderEntryPanel" << std::endl;
    updateOrderActionButtons();
}

void OrderEntryPanel::handleCurrentOrderChanged(const std::any& eventData) {
    std::cout << "Current order changed event received in OrderEntryPanel" << std::endl;
    updateOrderActionButtons();
}

// =================================================================
// UI Action Handlers
// =================================================================

// Action handlers
void OrderEntryPanel::onNewOrderClicked() {
    if (!posService_) return;
    
    try {
        int tableNumber = getTableNumber();
        auto order = posService_->createOrder(tableNumber);
        
        if (order) {
            posService_->setCurrentOrder(order);
            updateOrderActionButtons();
            
            // Refresh displays
            refresh();
        }
    } catch (const std::exception& e) {
        showOrderValidationError("Failed to create new order: " + std::string(e.what()));
    }
}

// Fix the createNewOrder method implementation
void OrderEntryPanel::createNewOrder(int tableNumber) {
    if (tableNumber <= 0) {
        showOrderValidationError("Please select a valid table number");
        return;
    }
    
    auto newOrder = posService_->createOrder(tableNumber);
    if (newOrder) {
        posService_->setCurrentOrder(newOrder);
        
        // Assuming Order class has a method to get ID - check your Order.hpp
        // If Order doesn't have getId(), you might need to add it or use a different method
        // For now, commenting out the problematic line:
        std::cout << "Created new order for table " << tableNumber 
                  /* << " (Order #" << newOrder->getId() << ")" */ << std::endl;
        
        updateOrderActionButtons();
        
        // Publish event
        eventManager_->publish(POSEvents::ORDER_CREATED, 
                              POSEvents::createOrderCreatedEvent(newOrder));
    } else {
        showOrderValidationError("Failed to create new order");
    }
}

void OrderEntryPanel::onSendToKitchenClicked() {
    if (!posService_ || !hasCurrentOrder()) {
        showOrderValidationError("No current order to send to kitchen");
        return;
    }
    
    try {
        bool success = posService_->sendCurrentOrderToKitchen();
        
        if (success) {
            updateOrderActionButtons();
            refresh();
        } else {
            showOrderValidationError("Failed to send order to kitchen");
        }
    } catch (const std::exception& e) {
        showOrderValidationError("Error sending to kitchen: " + std::string(e.what()));
    }
}

// Fix the sendCurrentOrderToKitchen method implementation
void OrderEntryPanel::sendCurrentOrderToKitchen() {
    if (!validateCurrentOrder()) {
        return;
    }
    
    auto currentOrder = posService_->getCurrentOrder();
    if (!currentOrder) {
        showOrderValidationError("No current order to send to kitchen");
        return;
    }
    
    if (currentOrder->getItems().empty()) {
        showOrderValidationError("Cannot send empty order to kitchen");
        return;
    }
    
    if (posService_->sendCurrentOrderToKitchen()) {
        std::cout << "Order sent to kitchen successfully" << std::endl;
        
        // Clear current order after sending to kitchen
        posService_->setCurrentOrder(nullptr);
        updateOrderActionButtons();
        
        // Publish event
        eventManager_->publish(POSEvents::ORDER_SENT_TO_KITCHEN, 
                              POSEvents::createOrderCreatedEvent(currentOrder));
    } else {
        showOrderValidationError("Failed to send order to kitchen");
    }
}


// Fix the processPayment method (in onProcessPaymentClicked)
void OrderEntryPanel::onProcessPaymentClicked() {
    if (!validateCurrentOrder()) {  // Now returns bool, so this works
        return;
    }
    
    auto currentOrder = posService_->getCurrentOrder();
    if (!currentOrder) {
        showOrderValidationError("No current order to process payment for");
        return;
    }
    
    // Fixed: Added std::fixed and proper include for setprecision
    std::cout << "Processing payment for Order " 
              << " - Total: $" << std::fixed << std::setprecision(2) 
              << currentOrder->getTotal() << std::endl;
    
    // Here you would typically open a payment dialog
    // For now, we'll simulate a successful payment
    
    // This would typically be handled by a payment dialog
    // posService_->processCurrentOrderPayment(PaymentProcessor::CASH, currentOrder->getTotal());
}

void OrderEntryPanel::onTableNumberChanged() {
    // Update current order table number if needed
    updateOrderActionButtons();
}

// =================================================================
// Helper Methods
// =================================================================

void OrderEntryPanel::updateOrderActionButtons() {
    bool hasOrder = hasCurrentOrder();
    
    if (sendToKitchenButton_) {
        sendToKitchenButton_->setEnabled(hasOrder);
    }
    
    if (processPaymentButton_) {
        processPaymentButton_->setEnabled(hasOrder);
    }
}

// Fix the validateCurrentOrder method to return bool
bool OrderEntryPanel::validateCurrentOrder() {
    auto currentOrder = posService_->getCurrentOrder();
    if (!currentOrder) {
        showOrderValidationError("No current order");
        return false;
    }
    
    if (currentOrder->getItems().empty()) {
        showOrderValidationError("Current order is empty");
        return false;
    }
    
    return true;
}

bool OrderEntryPanel::hasCurrentOrder() const {
    return posService_ && posService_->getCurrentOrder() != nullptr;
}


void OrderEntryPanel::showOrderValidationError(const std::string& message) {
    std::cerr << "Order validation error: " << message << std::endl;
    
    if (eventManager_) {
        eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED,
            POSEvents::createNotificationEvent(message, "warning", 3000));
    }
}
