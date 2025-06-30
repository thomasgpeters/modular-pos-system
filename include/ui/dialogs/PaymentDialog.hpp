#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include "../../PaymentProcessor.hpp"
#include "../../Order.hpp"
#include "../../events/EventManager.hpp"

#include <Wt/WDialog.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WCheckBox.h>
#include <Wt/WDoubleSpinBox.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>

#include <memory>
#include <functional>
#include <vector>

/**
 * @file PaymentDialog.hpp
 * @brief Payment processing dialog for order payments
 * 
 * This dialog handles payment processing with support for multiple
 * payment methods, split payments, and tip calculation.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class PaymentDialog
 * @brief Dialog for processing order payments
 * 
 * The PaymentDialog provides a comprehensive interface for processing
 * payments including method selection, amount entry, tip calculation,
 * and split payment options.
 */
class PaymentDialog : public Wt::WDialog {
public:
    /**
     * @brief Payment completion callback type
     */
    using PaymentCallback = std::function<void(const PaymentProcessor::PaymentResult&)>;
    
    /**
     * @brief Constructs a payment dialog
     * @param order Order to process payment for
     * @param eventManager Event manager for notifications
     * @param callback Callback function for payment completion
     */
    PaymentDialog(std::shared_ptr<Order> order,
                 std::shared_ptr<EventManager> eventManager,
                 PaymentCallback callback = nullptr);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~PaymentDialog() = default;
    
    /**
     * @brief Shows the payment dialog
     */
    void showDialog();
    
    /**
     * @brief Sets available payment methods
     * @param methods Vector of available payment methods
     */
    void setAvailablePaymentMethods(const std::vector<PaymentProcessor::PaymentMethod>& methods);
    
    /**
     * @brief Sets suggested tip percentages
     * @param suggestions Vector of tip percentages
     */
    void setTipSuggestions(const std::vector<double>& suggestions);
    
    /**
     * @brief Enables or disables split payment mode
     * @param enabled True to enable split payments
     */
    void setSplitPaymentEnabled(bool enabled);

protected:
    /**
     * @brief Creates the dialog content
     */
    void createDialogContent();
    
    /**
     * @brief Creates the order summary section
     * @return Container widget with order summary
     */
    std::unique_ptr<Wt::WContainerWidget> createOrderSummary();
    
    /**
     * @brief Creates the payment method selection section
     * @return Container widget with payment methods
     */
    std::unique_ptr<Wt::WContainerWidget> createPaymentMethodSection();
    
    /**
     * @brief Creates the amount and tip section
     * @return Container widget with amount controls
     */
    std::unique_ptr<Wt::WContainerWidget> createAmountSection();
    
    /**
     * @brief Creates the action buttons section
     * @return Container widget with action buttons
     */
    std::unique_ptr<Wt::WContainerWidget> createActionButtons();
    
    /**
     * @brief Updates the total amounts display
     */
    void updateTotals();
    
    /**
     * @brief Handles payment method selection changes
     */
    void onPaymentMethodChanged();
    
    /**
     * @brief Handles tip amount changes
     */
    void onTipChanged();
    
    /**
     * @brief Handles payment processing
     */
    void processPayment();
    
    /**
     * @brief Validates payment input
     * @return True if input is valid
     */
    bool validatePaymentInput();
    
    /**
     * @brief Gets the payment method name for display
     * @param method Payment method enum
     * @return Human-readable method name
     */
    std::string getPaymentMethodName(PaymentProcessor::PaymentMethod method) const;

private:
    // Order and dependencies
    std::shared_ptr<Order> order_;
    std::shared_ptr<EventManager> eventManager_;
    PaymentCallback paymentCallback_;
    
    // Payment configuration
    std::vector<PaymentProcessor::PaymentMethod> availableMethods_;
    std::vector<double> tipSuggestions_;
    bool splitPaymentEnabled_;
    
    // UI components
    Wt::WText* orderTotalText_;
    Wt::WText* taxAmountText_;
    Wt::WText* finalTotalText_;
    
    Wt::WButtonGroup* paymentMethodGroup_;
    std::vector<Wt::WRadioButton*> methodButtons_;
    
    Wt::WDoubleSpinBox* customTipInput_;
    Wt::WButtonGroup* tipButtonGroup_;
    std::vector<Wt::WPushButton*> tipButtons_;
    
    Wt::WDoubleSpinBox* paymentAmountInput_;
    Wt::WText* changeAmountText_;
    
    Wt::WPushButton* processButton_;
    Wt::WPushButton* cancelButton_;
    Wt::WPushButton* splitPaymentButton_;
    
    // Payment state
    PaymentProcessor::PaymentMethod selectedMethod_;
    double tipAmount_;
    double paymentAmount_;
    
    // Helper methods
    void setupEventHandlers();
    void calculateChange();
    void resetDialog();
    PaymentProcessor::PaymentMethod getSelectedPaymentMethod() const;
};

#endif // PAYMENTDIALOG_H
