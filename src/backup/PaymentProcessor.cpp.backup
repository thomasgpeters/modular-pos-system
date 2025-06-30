#include "../include/PaymentProcessor.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

/**
 * @file PaymentProcessor.cpp
 * @brief Implementation of the PaymentProcessor class
 */

PaymentProcessor::PaymentProcessor() : nextTransactionNumber_(1) {
}

PaymentProcessor::PaymentResult PaymentProcessor::processPayment(
    std::shared_ptr<Order> order, 
    PaymentMethod method, 
    double amount,
    double tipAmount) {
    
    PaymentResult result;
    result.method = method;
    result.timestamp = std::chrono::system_clock::now();
    
    // Validate inputs
    if (!order) {
        result.errorMessage = "Invalid order";
        return result;
    }
    
    if (!validatePaymentAmount(order, amount)) {
        result.errorMessage = "Invalid payment amount";
        return result;
    }
    
    if (tipAmount < 0) {
        result.errorMessage = "Tip amount cannot be negative";
        return result;
    }
    
    // Call pre-payment hook
    if (!onPrePayment(order, method, amount, tipAmount)) {
        result.errorMessage = "Payment rejected by pre-payment validation";
        return result;
    }
    
    // Process based on payment method
    switch (method) {
        case CASH:
            result = processCashPayment(order, amount, tipAmount);
            break;
        case CREDIT_CARD:
        case DEBIT_CARD:
            result = processCardPayment(order, method, amount, tipAmount);
            break;
        case MOBILE_PAY:
            result = processMobilePayment(order, amount, tipAmount);
            break;
        case GIFT_CARD:
            result = processGiftCardPayment(order, amount, tipAmount);
            break;
        default:
            result.errorMessage = "Unsupported payment method";
            return result;
    }
    
    // Record transaction and notify observers
    if (result.success) {
        recordTransaction(result);
        onPaymentSuccess(result);
    } else {
        onPaymentFailure(result);
    }
    
    return result;
}

std::vector<PaymentProcessor::PaymentResult> PaymentProcessor::processSplitPayment(
    std::shared_ptr<Order> order,
    const std::vector<std::pair<PaymentMethod, double>>& payments) {
    
    std::vector<PaymentResult> results;
    double totalProcessed = 0.0;
    
    for (const auto& payment : payments) {
        auto result = processPayment(order, payment.first, payment.second);
        results.push_back(result);
        if (result.success) {
            totalProcessed += result.amountProcessed;
        }
    }
    
    return results;
}

PaymentProcessor::PaymentResult PaymentProcessor::processRefund(
    const std::string& transactionId, double amount) {
    
    PaymentResult result;
    result.timestamp = std::chrono::system_clock::now();
    
    // Find original transaction
    auto it = std::find_if(transactionHistory_.begin(), transactionHistory_.end(),
        [&transactionId](const PaymentResult& transaction) {
            return transaction.transactionId == transactionId;
        });
    
    if (it == transactionHistory_.end()) {
        result.errorMessage = "Original transaction not found";
        return result;
    }
    
    if (amount > it->amountProcessed) {
        result.errorMessage = "Refund amount exceeds original transaction amount";
        return result;
    }
    
    // Process refund (simplified for demo)
    result.success = true;
    result.transactionId = generateTransactionId("REFUND");
    result.amountProcessed = -amount; // Negative for refund
    result.method = it->method;
    
    recordTransaction(result);
    
    return result;
}

bool PaymentProcessor::validatePaymentAmount(std::shared_ptr<Order> order, double amount) {
    return order && amount > 0 && amount <= order->getTotal();
}

std::string PaymentProcessor::paymentMethodToString(PaymentMethod method) {
    switch (method) {
        case CASH:          return "Cash";
        case CREDIT_CARD:   return "Credit Card";
        case DEBIT_CARD:    return "Debit Card";
        case MOBILE_PAY:    return "Mobile Pay";
        case GIFT_CARD:     return "Gift Card";
        default:            return "Unknown";
    }
}

std::vector<PaymentProcessor::PaymentResult> PaymentProcessor::getTransactionsByOrder(int orderId) {
    std::vector<PaymentResult> orderTransactions;
    
    std::copy_if(transactionHistory_.begin(), transactionHistory_.end(),
                 std::back_inserter(orderTransactions),
                 [orderId](const PaymentResult& transaction) {
                     // This would require storing orderId in PaymentResult
                     // For now, return empty vector as placeholder
                     return false;
                 });
    
    return orderTransactions;
}

// Protected payment processing methods

PaymentProcessor::PaymentResult PaymentProcessor::processCashPayment(
    std::shared_ptr<Order> order, double amount, double tip) {
    
    PaymentResult result;
    result.method = CASH;
    result.timestamp = std::chrono::system_clock::now();
    
    // Cash payment always succeeds (no external validation needed)
    result.success = true;
    result.transactionId = generateTransactionId("CASH");
    result.amountProcessed = amount + tip;
    
    return result;
}

PaymentProcessor::PaymentResult PaymentProcessor::processCardPayment(
    std::shared_ptr<Order> order, PaymentMethod method, double amount, double tip) {
    
    PaymentResult result;
    result.method = method;
    result.timestamp = std::chrono::system_clock::now();
    
    // Placeholder for actual card processing integration
    // In real implementation, this would:
    // 1. Connect to payment gateway
    // 2. Send card details securely
    // 3. Process authorization
    // 4. Handle response
    
    // Simulate successful payment for demo
    result.success = true;
    result.transactionId = generateTransactionId(method == CREDIT_CARD ? "CC" : "DC");
    result.amountProcessed = amount + tip;
    
    return result;
}

PaymentProcessor::PaymentResult PaymentProcessor::processMobilePayment(
    std::shared_ptr<Order> order, double amount, double tip) {
    
    PaymentResult result;
    result.method = MOBILE_PAY;
    result.timestamp = std::chrono::system_clock::now();
    
    // Placeholder for mobile payment integration
    // In real implementation, this would integrate with:
    // - Apple Pay
    // - Google Pay
    // - Samsung Pay
    // - Other mobile payment providers
    
    // Simulate successful payment for demo
    result.success = true;
    result.transactionId = generateTransactionId("MOBILE");
    result.amountProcessed = amount + tip;
    
    return result;
}

PaymentProcessor::PaymentResult PaymentProcessor::processGiftCardPayment(
    std::shared_ptr<Order> order, double amount, double tip) {
    
    PaymentResult result;
    result.method = GIFT_CARD;
    result.timestamp = std::chrono::system_clock::now();
    
    // Placeholder for gift card processing
    // In real implementation, this would:
    // 1. Validate gift card number
    // 2. Check balance
    // 3. Deduct amount
    // 4. Update gift card balance
    
    // Simulate successful payment for demo
    result.success = true;
    result.transactionId = generateTransactionId("GIFT");
    result.amountProcessed = amount + tip;
    
    return result;
}

// Private helper methods

std::string PaymentProcessor::generateTransactionId(const std::string& prefix) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::stringstream ss;
    ss << prefix << "_" << std::setfill('0') << std::setw(6) 
       << nextTransactionNumber_++ << "_" << timestamp;
    
    return ss.str();
}

void PaymentProcessor::recordTransaction(const PaymentResult& result) {
    transactionHistory_.push_back(result);
    
    // In a real implementation, this would also:
    // 1. Store transaction in database
    // 2. Generate receipt
    // 3. Update accounting records
    // 4. Send to reporting system
}
