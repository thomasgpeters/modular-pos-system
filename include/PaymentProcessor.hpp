#ifndef PAYMENTPROCESSOR_H
#define PAYMENTPROCESSOR_H

#include "../include/Order.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>

/**
 * @file PaymentProcessor.h
 * @brief Payment processing system for the Restaurant POS
 * 
 * This file contains the PaymentProcessor class which handles various payment
 * methods and transaction processing. Part of the three-legged foundation
 * of the POS system (Order Management, Payment Processing, Kitchen Interface).
 * 
 * @author Restaurant POS Team
 * @version 1.0.0
 */

/**
 * @class PaymentProcessor
 * @brief Handles payment processing and transaction management (LEG 2 of POS Foundation)
 * 
 * The PaymentProcessor class manages various payment methods including cash, credit cards,
 * mobile payments, and gift cards. It provides transaction processing, split payments,
 * and integration points for external payment gateways.
 */
class PaymentProcessor {
public:
    /**
     * @enum PaymentMethod
     * @brief Supported payment methods
     */
    enum PaymentMethod { 
        CASH,           ///< Cash payment
        CREDIT_CARD,    ///< Credit card payment
        DEBIT_CARD,     ///< Debit card payment
        MOBILE_PAY,     ///< Mobile payment (Apple Pay, Google Pay, etc.)
        GIFT_CARD       ///< Gift card payment
    };
    
    /**
     * @struct PaymentResult
     * @brief Result of a payment processing attempt
     */
    struct PaymentResult {
        bool success;                   ///< Whether the payment was successful
        std::string transactionId;      ///< Unique transaction identifier
        std::string errorMessage;       ///< Error message if payment failed
        double amountProcessed;         ///< Amount successfully processed
        PaymentMethod method;           ///< Payment method used
        std::chrono::system_clock::time_point timestamp; ///< Transaction timestamp
        
        PaymentResult() : success(false), amountProcessed(0.0), 
                         method(CASH), timestamp(std::chrono::system_clock::now()) {}
    };
    
    /**
     * @brief Constructs a new PaymentProcessor
     */
    PaymentProcessor();
    
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~PaymentProcessor() = default;
    
    /**
     * @brief Processes a payment for an order
     * @param order The order to process payment for
     * @param method Payment method to use
     * @param amount Amount to charge
     * @param tipAmount Tip amount (optional, default: 0.0)
     * @return Payment result with transaction details
     */
    PaymentResult processPayment(std::shared_ptr<Order> order, 
                                PaymentMethod method, 
                                double amount,
                                double tipAmount = 0.0);
    
    /**
     * @brief Handles split payments across multiple payment methods
     * @param order Order to process payment for
     * @param payments Vector of payment method and amount pairs
     * @return Vector of payment results for each payment attempt
     */
    std::vector<PaymentResult> processSplitPayment(
        std::shared_ptr<Order> order,
        const std::vector<std::pair<PaymentMethod, double>>& payments);
    
    /**
     * @brief Processes a refund for a previous transaction
     * @param transactionId Original transaction ID to refund
     * @param amount Amount to refund (must not exceed original amount)
     * @return Payment result for the refund
     */
    PaymentResult processRefund(const std::string& transactionId, double amount);
    
    /**
     * @brief Validates if a payment amount is valid for an order
     * @param order Order to validate against
     * @param amount Payment amount
     * @return True if amount is valid, false otherwise
     */
    bool validatePaymentAmount(std::shared_ptr<Order> order, double amount);
    
    /**
     * @brief Gets the string representation of a payment method
     * @param method Payment method to convert
     * @return String representation of the payment method
     */
    static std::string paymentMethodToString(PaymentMethod method);
    
    /**
     * @brief Gets all transaction history
     * @return Vector of all payment results
     */
    const std::vector<PaymentResult>& getTransactionHistory() const { 
        return transactionHistory_; 
    }
    
    /**
     * @brief Gets transaction history for a specific order
     * @param orderId Order ID to filter by
     * @return Vector of payment results for the specified order
     */
    std::vector<PaymentResult> getTransactionsByOrder(int orderId);
    
    // Extension points for payment gateway integration
    /**
     * @brief Called before processing any payment
     * Override in derived classes for custom validation
     * @param order Order being paid for
     * @param method Payment method
     * @param amount Payment amount
     * @param tipAmount Tip amount
     * @return True to continue processing, false to abort
     */
    virtual bool onPrePayment(std::shared_ptr<Order> order, 
                             PaymentMethod method, 
                             double amount, 
                             double tipAmount) { return true; }
    
    /**
     * @brief Called after successful payment processing
     * Override in derived classes for custom behavior
     * @param result Payment result
     */
    virtual void onPaymentSuccess(const PaymentResult& result) {}
    
    /**
     * @brief Called after failed payment processing
     * Override in derived classes for custom behavior
     * @param result Payment result with error information
     */
    virtual void onPaymentFailure(const PaymentResult& result) {}

protected:
    /**
     * @brief Processes cash payment
     * @param order Order being paid for
     * @param amount Payment amount
     * @param tip Tip amount
     * @return Payment result
     */
    virtual PaymentResult processCashPayment(std::shared_ptr<Order> order, 
                                           double amount, double tip);
    
    /**
     * @brief Processes card payment (credit or debit)
     * @param order Order being paid for
     * @param method Card type (CREDIT_CARD or DEBIT_CARD)
     * @param amount Payment amount
     * @param tip Tip amount
     * @return Payment result
     */
    virtual PaymentResult processCardPayment(std::shared_ptr<Order> order, 
                                           PaymentMethod method, 
                                           double amount, double tip);
    
    /**
     * @brief Processes mobile payment
     * @param order Order being paid for
     * @param amount Payment amount
     * @param tip Tip amount
     * @return Payment result
     */
    virtual PaymentResult processMobilePayment(std::shared_ptr<Order> order, 
                                             double amount, double tip);
    
    /**
     * @brief Processes gift card payment
     * @param order Order being paid for
     * @param amount Payment amount
     * @param tip Tip amount
     * @return Payment result
     */
    virtual PaymentResult processGiftCardPayment(std::shared_ptr<Order> order, 
                                               double amount, double tip);

private:
    /**
     * @brief Generates a unique transaction ID
     * @param prefix Prefix for the transaction ID
     * @return Unique transaction ID string
     */
    std::string generateTransactionId(const std::string& prefix);
    
    /**
     * @brief Records a transaction for audit and reporting
     * @param result Payment result to record
     */
    void recordTransaction(const PaymentResult& result);
    
    std::vector<PaymentResult> transactionHistory_;  ///< All transaction history
    int nextTransactionNumber_;                      ///< Next transaction sequence number
};

#endif // PAYMENTPROCESSOR_H
