#include "../../../include/ui/components/OrderEntryPanel.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WBorderLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WGroupBox.h>
#include <Wt/WMessageBox.h>
#include <Wt/WApplication.h>

#include <iostream>

OrderEntryPanel::OrderEntryPanel(std::shared_ptr<POSService> posService,
                                std::shared_ptr<EventManager> eventManager)
    : posService_(posService)
    , eventManager_(eventManager)
    , tableNumberEdit_(nullptr)
    , newOrderButton_(nullptr)
    , sendToKitchenButton_(nullptr)
    , processPaymentButton_(nullptr)
    , menuDisplay_(nullptr)
    , currentOrderDisplay_(nullptr) {
    
    initializeUI();
    setupEventListeners();
    updateOrderActionButtons();
}

void OrderEntryPanel::initializeUI() {
    addStyleClass("order-entry-panel");
    
    auto mainLayout = std::make_unique<Wt::WBorderLayout>();
    
    // Top section: Table selection and order actions
    auto topSection = std::make_unique<Wt::WContainerWidget>();
    topSection->addStyleClass("order-entry-top-section p-3 bg-light border-bottom");
    
    auto topLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Table selection
    auto tableSelection = createTableSelectionSection();
    topLayout->addWidget(std::move(tableSelection));
    
    // Order actions
    auto orderActions = createOrderActionsSection();
    topLayout->addWidget(std::move(orderActions));
    
    topSection->setLayout(std::move(topLayout));
    mainLayout->addWidget(std::move(topSection), Wt::LayoutPosition::North);
    
    // Center section: Split between menu and current order
    auto centerContainer = std::make_unique<Wt::WContainerWidget>();
    centerContainer->addStyleClass("order-entry-center-section");
    
    auto centerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // Left side: Menu display (60% width)
    auto menuContainer = std::make_unique<Wt::WContainerWidget>();
    menuContainer->addStyleClass("menu-section p-3");
    
    auto menuHeader = std::make_unique<Wt::WText>("Menu");
    menuHeader->addStyleClass("h4 mb-3");
    menuContainer->addWidget(std::move(menuHeader));
    
    menuDisplay_ = std::make_unique<MenuDisplay>(posService_, eventManager_);
    menuContainer->addWidget(std::move(menuDisplay_));
    
    centerLayout->addWidget(std::move(menuContainer), 3);
    
    // Right side: Current order display (40% width)
    auto orderContainer = std::make_unique<Wt::WContainerWidget>();
    orderContainer->addStyleClass("current-order-section p-3");
    
    currentOrderDisplay_ = std::make_unique<CurrentOrderDisplay>(posService_, eventManager_);
    orderContainer->addWidget(std::move(currentOrderDisplay_));
    
    centerLayout->addWidget(std::move(orderContainer), 2);
    
    centerContainer->setLayout(std::move(centerLayout));
    mainLayout->addWidget(std::move(centerContainer), Wt::LayoutPosition::Center);
    
    setLayout(std::move(mainLayout));
    
    std::cout << "✓ OrderEntryPanel UI initialized" << std::endl;
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

std::unique_ptr<Wt::WWidget> OrderEntryPanel::createTableSelectionSection() {
    auto section = std::make_unique<Wt::WGroupBox>("Table Selection");
    section->addStyleClass("table-selection-group");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Table number label and input
    auto tableLabel = std::make_unique<Wt::WLabel>("Table #:");
    tableLabel->addStyleClass("me-2");
    layout->addWidget(std::move(tableLabel));
    
    tableNumberEdit_ = section->addWidget(std::make_unique<Wt::WSpinBox>());
    tableNumberEdit_->setRange(1, 99);
    tableNumberEdit_->setValue(1);
    tableNumberEdit_->addStyleClass("form-control-sm me-3");
    tableNumberEdit_->setWidth(80);
    tableNumberEdit_->valueChanged().connect([this]() {
        onTableNumberChanged();
    });
    layout->addWidget(tableNumberEdit_);
    
    // New order button
    newOrderButton_ = section->addWidget(std::make_unique<Wt::WPushButton>("New Order"));
    newOrderButton_->addStyleClass("btn btn-success btn-sm");
    newOrderButton_->clicked().connect([this]() {
        onNewOrderClicked();
    });
    layout->addWidget(newOrderButton_);
    
    section->setLayout(std::move(layout));
    return std::move(section);
}

std::unique_ptr<Wt::WWidget> OrderEntryPanel::createOrderActionsSection() {
    auto section = std::make_unique<Wt::WGroupBox>("Order Actions");
    section->addStyleClass("order-actions-group");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Send to kitchen button
    sendToKitchenButton_ = section->addWidget(std::make_unique<Wt::WPushButton>("Send to Kitchen"));
    sendToKitchenButton_->addStyleClass("btn btn-primary btn-sm me-2");
    sendToKitchenButton_->clicked().connect([this]() {
        onSendToKitchenClicked();
    });
    layout->addWidget(sendToKitchenButton_);
    
    // Process payment button
    processPaymentButton_ = section->addWidget(std::make_unique<Wt::WPushButton>("Process Payment"));
    processPaymentButton_->addStyleClass("btn btn-warning btn-sm");
    processPaymentButton_->clicked().connect([this]() {
        onProcessPaymentClicked();
    });
    layout->addWidget(processPaymentButton_);
    
    section->setLayout(std::move(layout));
    return std::move(section);
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

void OrderEntryPanel::setTableNumber(int tableNumber) {
    if (tableNumberEdit_) {
        tableNumberEdit_->setValue(tableNumber);
        onTableNumberChanged();
    }
}

int OrderEntryPanel::getTableNumber() const {
    if (tableNumberEdit_) {
        return tableNumberEdit_->value();
    }
    return 1;
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

void OrderEntryPanel::onNewOrderClicked() {
    if (!posService_) {
        std::cerr << "Error: POSService not available for new order creation" << std::endl;
        return;
    }
    
    int tableNumber = getTableNumber();
    
    // Check if there's already a current order
    auto currentOrder = posService_->getCurrentOrder();
    if (currentOrder && !currentOrder->getItems().empty()) {
        auto messageBox = addChild(std::make_unique<Wt::WMessageBox>(
            "Current Order Exists",
            "There is already an order in progress for Table " + 
            std::to_string(currentOrder->getTableNumber()) + 
            ". Do you want to start a new order and lose the current one?",
            Wt::Icon::Warning,
            Wt::StandardButton::Yes | Wt::StandardButton::No));
        
        messageBox->buttonClicked().connect([this, messageBox, tableNumber](Wt::StandardButton button) {
            if (button == Wt::StandardButton::Yes) {
                createNewOrder(tableNumber);
            }
            removeChild(messageBox);
        });
        
        messageBox->show();
    } else {
        createNewOrder(tableNumber);
    }
}

void OrderEntryPanel::createNewOrder(int tableNumber) {
    if (!posService_) {
        return;
    }
    
    auto newOrder = posService_->createOrder(tableNumber);
    if (newOrder) {
        std::cout << "New order created for table " << tableNumber 
                  << " (Order #" << newOrder->getId() << ")" << std::endl;
        
        // Update UI
        refresh();
        
        // Show success message
        if (eventManager_) {
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED,
                POSEvents::createNotificationEvent(
                    "New order created for Table " + std::to_string(tableNumber),
                    "success",
                    3000));
        }
    } else {
        std::cerr << "Failed to create new order for table " << tableNumber << std::endl;
        
        // Show error message
        if (eventManager_) {
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED,
                POSEvents::createNotificationEvent(
                    "Failed to create new order",
                    "error",
                    5000));
        }
    }
}

void OrderEntryPanel::onSendToKitchenClicked() {
    if (!posService_) {
        std::cerr << "Error: POSService not available for kitchen order" << std::endl;
        return;
    }
    
    if (!validateCurrentOrder()) {
        return;
    }
    
    auto currentOrder = posService_->getCurrentOrder();
    
    // Confirm sending to kitchen
    auto messageBox = addChild(std::make_unique<Wt::WMessageBox>(
        "Send to Kitchen",
        "Send Order #" + std::to_string(currentOrder->getId()) + 
        " for Table " + std::to_string(currentOrder->getTableNumber()) + 
        " to the kitchen?",
        Wt::Icon::Question,
        Wt::StandardButton::Yes | Wt::StandardButton::No));
    
    messageBox->buttonClicked().connect([this, messageBox](Wt::StandardButton button) {
        if (button == Wt::StandardButton::Yes) {
            sendCurrentOrderToKitchen();
        }
        removeChild(messageBox);
    });
    
    messageBox->show();
}

void OrderEntryPanel::sendCurrentOrderToKitchen() {
    if (!posService_) {
        return;
    }
    
    bool success = posService_->sendCurrentOrderToKitchen();
    
    if (success) {
        std::cout << "Order sent to kitchen successfully" << std::endl;
        
        // Show success notification
        if (eventManager_) {
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED,
                POSEvents::createNotificationEvent(
                    "Order sent to kitchen successfully",
                    "success",
                    3000));
        }
        
        // Refresh UI
        refresh();
    } else {
        std::cerr << "Failed to send order to kitchen" << std::endl;
        
        // Show error notification
        if (eventManager_) {
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED,
                POSEvents::createNotificationEvent(
                    "Failed to send order to kitchen",
                    "error",
                    5000));
        }
    }
}

void OrderEntryPanel::onProcessPaymentClicked() {
    if (!posService_) {
        std::cerr << "Error: POSService not available for payment processing" << std::endl;
        return;
    }
    
    if (!validateCurrentOrder()) {
        return;
    }
    
    auto currentOrder = posService_->getCurrentOrder();
    
    // Show payment dialog (this would be implemented in a PaymentDialog class)
    std::cout << "Processing payment for Order #" << currentOrder->getId() 
              << " - Total: $" << std::fixed << std::setprecision(2) 
              << currentOrder->getTotal() << std::endl;
    
    // For now, we'll simulate a cash payment
    auto result = posService_->processCurrentOrderPayment(
        PaymentProcessor::CASH,
        currentOrder->getTotal(),
        0.0 // No tip for now
    );
    
    if (result.success) {
        std::cout << "Payment processed successfully" << std::endl;
        
        // Show success notification
        if (eventManager_) {
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED,
                POSEvents::createNotificationEvent(
                    "Payment processed successfully",
                    "success",
                    3000));
        }
        
        // Refresh UI
        refresh();
    } else {
        std::cerr << "Payment processing failed: " << result.errorMessage << std::endl;
        
        // Show error notification
        if (eventManager_) {
            eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED,
                POSEvents::createNotificationEvent(
                    "Payment failed: " + result.errorMessage,
                    "error",
                    5000));
        }
    }
}

void OrderEntryPanel::onTableNumberChanged() {
    // Table number changed - no immediate action needed
    // Could potentially load existing orders for this table
    std::cout << "Table number changed to: " << getTableNumber() << std::endl;
}

// =================================================================
// Helper Methods
// =================================================================

void OrderEntryPanel::updateOrderActionButtons() {
    bool hasOrder = hasCurrentOrder();
    bool hasItems = false;
    
    if (hasOrder) {
        auto currentOrder = posService_->getCurrentOrder();
        hasItems = currentOrder && !currentOrder->getItems().empty();
    }
    
    // Send to Kitchen button: enabled if order has items
    if (sendToKitchenButton_) {
        sendToKitchenButton_->setEnabled(hasItems);
    }
    
    // Process Payment button: enabled if order has items
    if (processPaymentButton_) {
        processPaymentButton_->setEnabled(hasItems);
    }
    
    std::cout << "Order action buttons updated - hasOrder: " << hasOrder 
              << ", hasItems: " << hasItems << std::endl;
}

void OrderEntryPanel::validateCurrentOrder() {
    if (!hasCurrentOrder()) {
        showOrderValidationError("No current order to process");
        return;
    }
    
    auto currentOrder = posService_->getCurrentOrder();
    if (!currentOrder || currentOrder->getItems().empty()) {
        showOrderValidationError("Current order is empty");
        return;
    }
}

bool OrderEntryPanel::hasCurrentOrder() const {
    if (!posService_) {
        return false;
    }
    
    auto currentOrder = posService_->getCurrentOrder();
    return currentOrder != nullptr;
}

void OrderEntryPanel::showOrderValidationError(const std::string& message) {
    std::cerr << "Order validation error: " << message << std::endl;
    
    if (eventManager_) {
        eventManager_->publish(POSEvents::NOTIFICATION_REQUESTED,
            POSEvents::createNotificationEvent(message, "warning", 3000));
    }
}
