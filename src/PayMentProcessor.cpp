#include "../include/PaymentProcessor.hpp"
#include <sstream>
#include <iomanip>
#include <random>

PaymentProcessor::PaymentProcessor() : nextTransactionNumber_(1000) {}

PaymentProcessor::PaymentResult PaymentProcessor::processPayment(
    std::shared_ptr<Order> order, 
    PaymentMethod method, 
    double amount,
    double tipAmount) {
    
    PaymentResult result;
    result.method = method;
    result.timestamp = std::chrono::system_clock::now();
    
    // Call pre-payment hook
    if (!onPrePayment(order, method, amount, tipAmount)) {
        result.success = false;
        result.errorMessage = "Payment validation failed";
        onPaymentFailure(result);
        return result;
    }
    
    // Validate payment amount
    if (!validatePaymentAmount(order, amount)) {
        result.success = false;
        result.errorMessage = "Invalid payment amount";
        onPaymentFailure(result);
        return result;
    }
    
    // Process payment based on method
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
            result.success = false;
            result.errorMessage = "Unsupported payment method";
    }
    
    // Record transaction
    recordTransaction(result);
    
    // Call post-payment hooks
    if (result.success) {
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
    results.reserve(payments.size());
    
    [[maybe_unused]] double totalProcessed = 0.0;
    double orderTotal = order->getTotal();
    
    for (const auto& [method, amount] : payments) {
        auto result = processPayment(order, method, amount, 0.0);
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
    result.success = false;
    result.errorMessage = "Refund processing not implemented in demo";
    result.amountProcessed = 0.0;
    result.transactionId = generateTransactionId("REF");
    result.timestamp = std::chrono::system_clock::now();
    
    return result;
}

bool PaymentProcessor::validatePaymentAmount(std::shared_ptr<Order> order, double amount) {
    return amount > 0.0 && amount <= (order->getTotal() * 1.5); // Allow 50% overpayment for tips
}

std::string PaymentProcessor::paymentMethodToString(PaymentMethod method) {
    switch (method) {
        case CASH:        return "Cash";
        case CREDIT_CARD: return "Credit Card";
        case DEBIT_CARD:  return "Debit Card";
        case MOBILE_PAY:  return "Mobile Pay";
        case GIFT_CARD:   return "Gift Card";
        default:          return "Unknown";
    }
}

std::vector<PaymentProcessor::PaymentResult> PaymentProcessor::getTransactionsByOrder(int orderId) {
    std::vector<PaymentResult> orderTransactions;
    
    for (const auto& transaction : transactionHistory_) {
        // Note: In a real implementation, we'd store orderId with each transaction
        // For this demo, we'll return all transactions
        orderTransactions.push_back(transaction);
    }
    
    return orderTransactions;
}

PaymentProcessor::PaymentResult PaymentProcessor::processCashPayment(
    std::shared_ptr<Order> order, double amount, double tip) {
    
    PaymentResult result;
    result.success = true;
    result.amountProcessed = amount + tip;
    result.transactionId = generateTransactionId("CASH");
    result.method = CASH;
    result.timestamp = std::chrono::system_clock::now();
    
    return result;
}

PaymentProcessor::PaymentResult PaymentProcessor::processCardPayment(
    std::shared_ptr<Order> order, PaymentMethod method, double amount, double tip) {
    
    PaymentResult result;
    
    // Simulate card processing (90% success rate)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    if (dis(gen) < 0.9) {
        result.success = true;
        result.amountProcessed = amount + tip;
        result.transactionId = generateTransactionId(method == CREDIT_CARD ? "CC" : "DC");
    } else {
        result.success = false;
        result.errorMessage = "Card declined";
        result.amountProcessed = 0.0;
        result.transactionId = "";
    }
    
    result.method = method;
    result.timestamp = std::chrono::system_clock::now();
    
    return result;
}

PaymentProcessor::PaymentResult PaymentProcessor::processMobilePayment(
    std::shared_ptr<Order> order, double amount, double tip) {
    
    PaymentResult result;
    result.success = true;
    result.amountProcessed = amount + tip;
    result.transactionId = generateTransactionId("MP");
    result.method = MOBILE_PAY;
    result.timestamp = std::chrono::system_clock::now();
    
    return result;
}

PaymentProcessor::PaymentResult PaymentProcessor::processGiftCardPayment(
    std::shared_ptr<Order> order, double amount, double tip) {
    
    PaymentResult result;
    result.success = true;
    result.amountProcessed = amount; // Gift cards typically don't include tips
    result.transactionId = generateTransactionId("GC");
    result.method = GIFT_CARD;
    result.timestamp = std::chrono::system_clock::now();
    
    return result;
}

std::string PaymentProcessor::generateTransactionId(const std::string& prefix) {
    std::stringstream ss;
    ss << prefix << "-" << std::setfill('0') << std::setw(6) << nextTransactionNumber_++;
    return ss.str();
}

void PaymentProcessor::recordTransaction(const PaymentResult& result) {
    transactionHistory_.push_back(result);
}
