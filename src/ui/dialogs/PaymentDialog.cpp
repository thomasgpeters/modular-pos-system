//=============================================================================
// PaymentDialog.cpp
//=============================================================================

#include "../../../include/ui/dialogs/PaymentDialog.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WBreak.h>

#include <iostream>
#include <iomanip>
#include <sstream>

PaymentDialog::PaymentDialog(std::shared_ptr<Order> order,
                           std::shared_ptr<EventManager> eventManager,
                           PaymentCallback callback)
    : WDialog("Process Payment"), order_(order), eventManager_(eventManager),
      paymentCallback_(callback), splitPaymentEnabled_(false),
      selectedMethod_(PaymentProcessor::CASH), tipAmount_(0.0), paymentAmount_(0.0) {
    
    // Set dialog properties
    setModal(true);
    setResizable(false);
    resize(500, 600);
    
    // Set default payment methods
    availableMethods_ = {
        PaymentProcessor::CASH,
        PaymentProcessor::CREDIT_CARD,
        PaymentProcessor::DEBIT_CARD
    };
    
    // Set default tip suggestions
    tipSuggestions_ = {15.0, 18.0, 20.0, 25.0};
    
    createDialogContent();
    setupEventHandlers();
    updateTotals();
}

// Add destructor implementation:
PaymentDialog::~PaymentDialog() {
    delete paymentMethodGroup_;
    delete tipButtonGroup_;
}


void PaymentDialog::createDialogContent() {
    auto content = contents()->addNew<Wt::WContainerWidget>();
    content->addStyleClass("payment-dialog-content");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Order summary
    layout->addWidget(createOrderSummary());
    
    // Payment method selection
    layout->addWidget(createPaymentMethodSection());
    
    // Amount and tip section
    layout->addWidget(createAmountSection());
    
    // Action buttons
    layout->addWidget(createActionButtons());
    
    content->setLayout(std::move(layout));
}

// Fix for PaymentDialog.cpp - createOrderSummary method

std::unique_ptr<Wt::WContainerWidget> PaymentDialog::createOrderSummary() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("order-summary-section");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // FIXED: Create group box and add directly to layout
    auto groupBox = std::make_unique<Wt::WGroupBox>("Order Summary");
    auto groupBoxLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // Order details
    auto orderInfo = std::make_unique<Wt::WText>(
        "Order #" + std::to_string(order_->getOrderId()) + 
        " - Table " + std::to_string(order_->getTableNumber()));
    orderInfo->addStyleClass("order-info");
    groupBoxLayout->addWidget(std::move(orderInfo));
    
    // Items summary
    auto itemsText = std::make_unique<Wt::WText>(
        std::to_string(order_->getItems().size()) + " items");
    itemsText->addStyleClass("items-summary");
    groupBoxLayout->addWidget(std::move(itemsText));
    
    // Totals
    auto totalsContainer = std::make_unique<Wt::WContainerWidget>();
    totalsContainer->addStyleClass("totals-container");
    
    auto totalsLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto subtotalRow = std::make_unique<Wt::WContainerWidget>();
    subtotalRow->addStyleClass("total-row");
    auto subtotalLayout = std::make_unique<Wt::WHBoxLayout>();
    subtotalLayout->addWidget(std::make_unique<Wt::WText>("Subtotal:"), 1);
    orderTotalText_ = subtotalLayout->addWidget(std::make_unique<Wt::WText>("$0.00"));
    subtotalRow->setLayout(std::move(subtotalLayout));
    totalsLayout->addWidget(std::move(subtotalRow));
    
    auto taxRow = std::make_unique<Wt::WContainerWidget>();
    taxRow->addStyleClass("total-row");
    auto taxLayout = std::make_unique<Wt::WHBoxLayout>();
    taxLayout->addWidget(std::make_unique<Wt::WText>("Tax:"), 1);
    taxAmountText_ = taxLayout->addWidget(std::make_unique<Wt::WText>("$0.00"));
    taxRow->setLayout(std::move(taxLayout));
    totalsLayout->addWidget(std::move(taxRow));
    
    auto finalRow = std::make_unique<Wt::WContainerWidget>();
    finalRow->addStyleClass("total-row final-total");
    auto finalLayout = std::make_unique<Wt::WHBoxLayout>();
    finalLayout->addWidget(std::make_unique<Wt::WText>("Total:"), 1);
    finalTotalText_ = finalLayout->addWidget(std::make_unique<Wt::WText>("$0.00"));
    finalRow->setLayout(std::move(finalLayout));
    totalsLayout->addWidget(std::move(finalRow));
    
    totalsContainer->setLayout(std::move(totalsLayout));
    groupBoxLayout->addWidget(std::move(totalsContainer));
    
    groupBox->setLayout(std::move(groupBoxLayout));
    layout->addWidget(std::move(groupBox));
    
    container->setLayout(std::move(layout));
    return container;
}

// Fix createPaymentMethodSection method:
std::unique_ptr<Wt::WContainerWidget> PaymentDialog::createPaymentMethodSection() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("payment-method-section");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    auto groupBox = std::make_unique<Wt::WGroupBox>("Payment Method");
    auto groupBoxLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // SIMPLE: Just create the button group with no parameters
    paymentMethodGroup_ = new Wt::WButtonGroup();
    
    for (auto method : availableMethods_) {
        auto methodContainer = std::make_unique<Wt::WContainerWidget>();
        methodContainer->addStyleClass("payment-method-option");
        
        auto radioButton = std::make_unique<Wt::WRadioButton>(getPaymentMethodName(method));
        radioButton->addStyleClass("payment-method-radio");
        
        auto* radioPtr = radioButton.get();
        paymentMethodGroup_->addButton(radioButton.get(), static_cast<int>(method));
        methodButtons_.push_back(radioPtr);
        
        methodContainer->addWidget(std::move(radioButton));
        groupBoxLayout->addWidget(std::move(methodContainer));
    }
    
    // Select first method by default
    if (!methodButtons_.empty()) {
        methodButtons_[0]->setChecked(true);
        selectedMethod_ = availableMethods_[0];
    }
    
    groupBox->setLayout(std::move(groupBoxLayout));
    layout->addWidget(std::move(groupBox));
    
    container->setLayout(std::move(layout));
    return container;
}

// Fix for PaymentDialog.cpp - createAmountSection method

// Fix createAmountSection method:
std::unique_ptr<Wt::WContainerWidget> PaymentDialog::createAmountSection() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("amount-section");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Tip section
    auto tipGroupBox = std::make_unique<Wt::WGroupBox>("Tip");
    auto tipLayout = std::make_unique<Wt::WVBoxLayout>();
    
    // Tip buttons
    auto tipButtonsContainer = std::make_unique<Wt::WContainerWidget>();
    tipButtonsContainer->addStyleClass("tip-buttons-container");
    auto tipButtonsLayout = std::make_unique<Wt::WHBoxLayout>();
    
    // SIMPLE: Just create the button group with no parameters
    tipButtonGroup_ = new Wt::WButtonGroup();
    
    for (double percentage : tipSuggestions_) {
        auto tipButton = std::make_unique<Wt::WPushButton>(std::to_string(static_cast<int>(percentage)) + "%");
        tipButton->addStyleClass("btn btn-outline-secondary tip-button");
        
        auto* buttonPtr = tipButton.get();
        tipButtons_.push_back(buttonPtr);
        
        tipButtonsLayout->addWidget(std::move(tipButton));
        
        buttonPtr->clicked().connect([this, percentage]() {
            tipAmount_ = order_->getSubtotal() * (percentage / 100.0);
            customTipInput_->setValue(tipAmount_);
            updateTotals();
        });
    }
    
    tipButtonsContainer->setLayout(std::move(tipButtonsLayout));
    tipLayout->addWidget(std::move(tipButtonsContainer));
    
    // Custom tip input
    auto customTipContainer = std::make_unique<Wt::WContainerWidget>();
    auto customTipLayout = std::make_unique<Wt::WHBoxLayout>();
    customTipLayout->addWidget(std::make_unique<Wt::WText>("Custom tip: $"));
    customTipInput_ = customTipLayout->addWidget(std::make_unique<Wt::WDoubleSpinBox>());
    customTipInput_->setMinimum(0.0);
    customTipInput_->setMaximum(999.99);
    customTipInput_->setDecimals(2);
    customTipInput_->setValue(0.0);
    customTipContainer->setLayout(std::move(customTipLayout));
    tipLayout->addWidget(std::move(customTipContainer));
    
    tipGroupBox->setLayout(std::move(tipLayout));
    layout->addWidget(std::move(tipGroupBox));
    
    // Payment amount section
    auto amountGroupBox = std::make_unique<Wt::WGroupBox>("Payment Amount");
    auto amountLayout = std::make_unique<Wt::WVBoxLayout>();
    
    auto amountContainer = std::make_unique<Wt::WContainerWidget>();
    auto amountRowLayout = std::make_unique<Wt::WHBoxLayout>();
    amountRowLayout->addWidget(std::make_unique<Wt::WText>("Amount: $"));
    paymentAmountInput_ = amountRowLayout->addWidget(std::make_unique<Wt::WDoubleSpinBox>());
    paymentAmountInput_->setMinimum(0.01);
    paymentAmountInput_->setMaximum(9999.99);
    paymentAmountInput_->setDecimals(2);
    amountContainer->setLayout(std::move(amountRowLayout));
    amountLayout->addWidget(std::move(amountContainer));
    
    // Change display
    auto changeContainer = std::make_unique<Wt::WContainerWidget>();
    auto changeLayout = std::make_unique<Wt::WHBoxLayout>();
    changeLayout->addWidget(std::make_unique<Wt::WText>("Change: "), 1);
    changeAmountText_ = changeLayout->addWidget(std::make_unique<Wt::WText>("$0.00"));
    changeAmountText_->addStyleClass("change-amount");
    changeContainer->setLayout(std::move(changeLayout));
    amountLayout->addWidget(std::move(changeContainer));
    
    amountGroupBox->setLayout(std::move(amountLayout));
    layout->addWidget(std::move(amountGroupBox));
    
    container->setLayout(std::move(layout));
    return container;
}

std::unique_ptr<Wt::WContainerWidget> PaymentDialog::createActionButtons() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("action-buttons-section");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    if (splitPaymentEnabled_) {
        splitPaymentButton_ = layout->addWidget(std::make_unique<Wt::WPushButton>("Split Payment"));
        splitPaymentButton_->addStyleClass("btn btn-info");
    }
    
    layout->addStretch(1);
    
    cancelButton_ = layout->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancelButton_->addStyleClass("btn btn-secondary");
    
    processButton_ = layout->addWidget(std::make_unique<Wt::WPushButton>("Process Payment"));
    processButton_->addStyleClass("btn btn-success");
    
    container->setLayout(std::move(layout));
    
    return container;
}

void PaymentDialog::setupEventHandlers() {
    // Payment method change
    paymentMethodGroup_->checkedChanged().connect([this]() {
        onPaymentMethodChanged();
    });
    
    // Tip input change
    customTipInput_->valueChanged().connect([this]() {
        onTipChanged();
    });
    
    // Payment amount change
    paymentAmountInput_->valueChanged().connect([this]() {
        calculateChange();
    });
    
    // Button clicks
    processButton_->clicked().connect([this]() {
        processPayment();
    });
    
    cancelButton_->clicked().connect([this]() {
        reject();
    });
    
    if (splitPaymentButton_) {
        splitPaymentButton_->clicked().connect([]() {
            // TODO: Implement split payment dialog
            std::cout << "Split payment not yet implemented" << std::endl;
        });
    }
}

// Fix 1: updateTotals method - use getTax() instead of getTaxAmount()
void PaymentDialog::updateTotals() {
    if (!order_) return;
    
    double subtotal = order_->getSubtotal();
    double tax = order_->getTax();  // FIXED: use getTax() instead of getTaxAmount()
    double total = subtotal + tax + tipAmount_;
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss.str("");
    ss << "$" << subtotal;
    orderTotalText_->setText(ss.str());
    
    ss.str("");
    ss << "$" << tax;
    taxAmountText_->setText(ss.str());
    
    ss.str("");
    ss << "$" << total;
    finalTotalText_->setText(ss.str());
    
    // Set payment amount to total by default
    if (paymentAmountInput_->value() == 0.0) {
        paymentAmountInput_->setValue(total);
        paymentAmount_ = total;
    }
    
    calculateChange();
}

void PaymentDialog::onPaymentMethodChanged() {
    selectedMethod_ = getSelectedPaymentMethod();
    std::cout << "Payment method changed to: " << getPaymentMethodName(selectedMethod_) << std::endl;
}

void PaymentDialog::onTipChanged() {
    tipAmount_ = customTipInput_->value();
    updateTotals();
}

// Fix 2: calculateChange method - use getTax() instead of getTaxAmount()
void PaymentDialog::calculateChange() {
    paymentAmount_ = paymentAmountInput_->value();
    double total = order_->getSubtotal() + order_->getTax() + tipAmount_;  // FIXED: getTax()
    double change = paymentAmount_ - total;
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << "$" << std::max(0.0, change);
    changeAmountText_->setText(ss.str());
    
    // Update change text color
    if (change < 0) {
        changeAmountText_->addStyleClass("text-danger");
        changeAmountText_->removeStyleClass("text-success");
    } else {
        changeAmountText_->addStyleClass("text-success");
        changeAmountText_->removeStyleClass("text-danger");
    }
}

// Fix 4: processPayment method - fix PaymentResult structure
void PaymentDialog::processPayment() {
    if (!validatePaymentInput()) {
        return;
    }
    
    // Create payment result (simplified - in real implementation this would call PaymentProcessor)
    PaymentProcessor::PaymentResult result;
    result.success = true;
    // REMOVED: result.amount = paymentAmount_;     // This field doesn't exist
    result.method = selectedMethod_;
    // REMOVED: result.tipAmount = tipAmount_;     // This field doesn't exist
    result.transactionId = "TXN-" + std::to_string(std::time(nullptr));
    result.timestamp = std::chrono::system_clock::now();
    
    // If you need to store payment amount and tip, check what fields actually exist
    // in PaymentProcessor::PaymentResult or create a custom structure
    
    std::cout << "Processing payment: " << getPaymentMethodName(selectedMethod_) 
              << " - $" << paymentAmount_ << " (tip: $" << tipAmount_ << ")" << std::endl;
    
    // Publish payment event
    if (eventManager_) {
        eventManager_->publish(POSEvents::PAYMENT_COMPLETED,
            POSEvents::createPaymentCompletedEvent(result, order_));
    }
    
    // Call callback
    if (paymentCallback_) {
        paymentCallback_(result);
    }
    
    accept();
}


// Fix 3: validatePaymentInput method - use getTax() instead of getTaxAmount()
bool PaymentDialog::validatePaymentInput() {
    double total = order_->getSubtotal() + order_->getTax() + tipAmount_;  // FIXED: getTax()
    
    if (paymentAmount_ < total) {
        // Show error message
        std::cerr << "Payment amount insufficient" << std::endl;
        return false;
    }
    
    return true;
}

PaymentProcessor::PaymentMethod PaymentDialog::getSelectedPaymentMethod() const {
    int selectedIndex = paymentMethodGroup_->checkedId();
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(availableMethods_.size())) {
        return availableMethods_[selectedIndex];
    }
    return PaymentProcessor::CASH;
}

std::string PaymentDialog::getPaymentMethodName(PaymentProcessor::PaymentMethod method) const {
    switch (method) {
        case PaymentProcessor::CASH: return "Cash";
        case PaymentProcessor::CREDIT_CARD: return "Credit Card";
        case PaymentProcessor::DEBIT_CARD: return "Debit Card";
        case PaymentProcessor::MOBILE_PAY: return "Mobile Pay";
        case PaymentProcessor::GIFT_CARD: return "Gift Card";
        default: return "Unknown";
    }
}

void PaymentDialog::showDialog() {
    updateTotals();
    show();
}

void PaymentDialog::setAvailablePaymentMethods(const std::vector<PaymentProcessor::PaymentMethod>& methods) {
    availableMethods_ = methods;
    // Would need to recreate payment method section in a full implementation
}

void PaymentDialog::setTipSuggestions(const std::vector<double>& suggestions) {
    tipSuggestions_ = suggestions;
    // Would need to recreate tip section in a full implementation
}

void PaymentDialog::setSplitPaymentEnabled(bool enabled) {
    splitPaymentEnabled_ = enabled;
    // Would need to show/hide split payment button in a full implementation
}
