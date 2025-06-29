/**
 * @file test_pos.cpp
 * @brief Unit tests for Restaurant POS System
 * 
 * This file contains comprehensive unit tests for all major components
 * of the Restaurant POS system. Tests verify the three-legged foundation:
 * Order Management, Payment Processing, and Kitchen Interface.
 * 
 * To compile and run tests:
 *   g++ -std=c++17 -DUNIT_TESTING test_pos.cpp MenuItem.cpp Order.cpp \
 *       OrderManager.cpp PaymentProcessor.cpp KitchenInterface.cpp \
 *       $(pkg-config --cflags --libs wt) -o test_pos
 *   ./test_pos
 * 
 * @author Restaurant POS Team
 * @version 1.0.0
 */

#include "MenuItem.h"
#include "Order.h"
#include "OrderManager.h"
#include "PaymentProcessor.h"
#include "KitchenInterface.h"

#include <iostream>
#include <cassert>
#include <string>
#include <memory>
#include <vector>
#include <iomanip>

/**
 * @brief Simple test framework for unit testing
 */
class TestFramework {
public:
    static void runTest(const std::string& testName, std::function<void()> testFunction) {
        std::cout << "Running: " << testName << "... ";
        try {
            testFunction();
            std::cout << "✓ PASSED" << std::endl;
            passedTests_++;
        } catch (const std::exception& e) {
            std::cout << "✗ FAILED: " << e.what() << std::endl;
            failedTests_++;
        } catch (...) {
            std::cout << "✗ FAILED: Unknown exception" << std::endl;
            failedTests_++;
        }
        totalTests_++;
    }
    
    static void printSummary() {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "Total Tests:  " << totalTests_ << std::endl;
        std::cout << "Passed:       " << passedTests_ << " ✓" << std::endl;
        std::cout << "Failed:       " << failedTests_ << " ✗" << std::endl;
        std::cout << "Success Rate: " << std::fixed << std::setprecision(1) 
                  << (totalTests_ > 0 ? (100.0 * passedTests_ / totalTests_) : 0.0) << "%" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
    }
    
    static bool allTestsPassed() {
        return failedTests_ == 0 && totalTests_ > 0;
    }

private:
    static int totalTests_;
    static int passedTests_;
    static int failedTests_;
};

// Static member initialization
int TestFramework::totalTests_ = 0;
int TestFramework::passedTests_ = 0;
int TestFramework::failedTests_ = 0;

/**
 * @brief Custom assertion macro for clearer error messages
 */
#define ASSERT_TRUE(condition, message) \
    if (!(condition)) { \
        throw std::runtime_error(message); \
    }

#define ASSERT_FALSE(condition, message) \
    if (condition) { \
        throw std::runtime_error(message); \
    }

#define ASSERT_EQ(expected, actual, message) \
    if ((expected) != (actual)) { \
        throw std::runtime_error(message + " (expected: " + std::to_string(expected) + ", actual: " + std::to_string(actual) + ")"); \
    }

#define ASSERT_NEAR(expected, actual, tolerance, message) \
    if (std::abs((expected) - (actual)) > (tolerance)) { \
        throw std::runtime_error(message + " (expected: " + std::to_string(expected) + ", actual: " + std::to_string(actual) + ")"); \
    }

/**
 * @brief Test cases for MenuItem class
 */
class MenuItemTests {
public:
    static void testMenuItemCreation() {
        MenuItem item(1, "Test Item", 12.99, MenuItem::MAIN_COURSE);
        
        ASSERT_EQ(1, item.getId(), "MenuItem ID should match constructor value");
        ASSERT_TRUE(item.getName() == "Test Item", "MenuItem name should match constructor value");
        ASSERT_NEAR(12.99, item.getPrice(), 0.01, "MenuItem price should match constructor value");
        ASSERT_EQ(MenuItem::MAIN_COURSE, item.getCategory(), "MenuItem category should match constructor value");
        ASSERT_TRUE(item.isAvailable(), "MenuItem should be available by default");
    }
    
    static void testMenuItemPriceUpdate() {
        MenuItem item(1, "Test Item", 12.99, MenuItem::MAIN_COURSE);
        
        item.setPrice(15.99);
        ASSERT_NEAR(15.99, item.getPrice(), 0.01, "MenuItem price should update correctly");
    }
    
    static void testMenuItemAvailability() {
        MenuItem item(1, "Test Item", 12.99, MenuItem::MAIN_COURSE);
        
        item.setAvailable(false);
        ASSERT_FALSE(item.isAvailable(), "MenuItem should be unavailable after setting to false");
        
        item.setAvailable(true);
        ASSERT_TRUE(item.isAvailable(), "MenuItem should be available after setting to true");
    }
    
    static void testMenuItemJSON() {
        MenuItem item(1, "Test Item", 12.99, MenuItem::MAIN_COURSE);
        auto json = item.toJson();
        
        ASSERT_TRUE(json.contains("id"), "JSON should contain id field");
        ASSERT_TRUE(json.contains("name"), "JSON should contain name field");
        ASSERT_TRUE(json.contains("price"), "JSON should contain price field");
        ASSERT_TRUE(json.contains("category"), "JSON should contain category field");
        ASSERT_TRUE(json.contains("available"), "JSON should contain available field");
    }
    
    static void runAllTests() {
        TestFramework::runTest("MenuItem Creation", testMenuItemCreation);
        TestFramework::runTest("MenuItem Price Update", testMenuItemPriceUpdate);
        TestFramework::runTest("MenuItem Availability", testMenuItemAvailability);
        TestFramework::runTest("MenuItem JSON Serialization", testMenuItemJSON);
    }
};

/**
 * @brief Test cases for Order and OrderItem classes
 */
class OrderTests {
public:
    static void testOrderCreation() {
        Order order(1001, 5);
        
        ASSERT_EQ(1001, order.getOrderId(), "Order ID should match constructor value");
        ASSERT_EQ(5, order.getTableNumber(), "Table number should match constructor value");
        ASSERT_EQ(Order::PENDING, order.getStatus(), "Order should be PENDING by default");
        ASSERT_TRUE(order.getItems().empty(), "New order should have no items");
        ASSERT_NEAR(0.0, order.getTotal(), 0.01, "New order total should be 0");
    }
    
    static void testOrderItemAddition() {
        Order order(1001, 5);
        MenuItem item(1, "Test Item", 10.00, MenuItem::MAIN_COURSE);
        OrderItem orderItem(item, 2);
        
        order.addItem(orderItem);
        
        ASSERT_EQ(1, order.getItems().size(), "Order should have 1 item after addition");
        ASSERT_NEAR(20.00, order.getSubtotal(), 0.01, "Subtotal should be 2 * $10.00");
        ASSERT_NEAR(1.60, order.getTax(), 0.01, "Tax should be 8% of subtotal");
        ASSERT_NEAR(21.60, order.getTotal(), 0.01, "Total should be subtotal + tax");
    }
    
    static void testOrderItemRemoval() {
        Order order(1001, 5);
        MenuItem item1(1, "Item 1", 10.00, MenuItem::MAIN_COURSE);
        MenuItem item2(2, "Item 2", 5.00, MenuItem::APPETIZER);
        
        order.addItem(OrderItem(item1, 1));
        order.addItem(OrderItem(item2, 1));
        
        ASSERT_EQ(2, order.getItems().size(), "Order should have 2 items");
        
        order.removeItem(0);
        
        ASSERT_EQ(1, order.getItems().size(), "Order should have 1 item after removal");
        ASSERT_NEAR(5.00, order.getSubtotal(), 0.01, "Subtotal should be $5.00 after removal");
    }
    
    static void testOrderItemQuantityUpdate() {
        Order order(1001, 5);
        MenuItem item(1, "Test Item", 10.00, MenuItem::MAIN_COURSE);
        
        order.addItem(OrderItem(item, 1));
        order.updateItemQuantity(0, 3);
        
        ASSERT_EQ(3, order.getItems()[0].getQuantity(), "Item quantity should be updated to 3");
        ASSERT_NEAR(30.00, order.getSubtotal(), 0.01, "Subtotal should reflect quantity change");
    }
    
    static void testOrderStatusChange() {
        Order order(1001, 5);
        
        order.setStatus(Order::SENT_TO_KITCHEN);
        ASSERT_EQ(Order::SENT_TO_KITCHEN, order.getStatus(), "Order status should update to SENT_TO_KITCHEN");
        
        order.setStatus(Order::READY);
        ASSERT_EQ(Order::READY, order.getStatus(), "Order status should update to READY");
    }
    
    static void runAllTests() {
        TestFramework::runTest("Order Creation", testOrderCreation);
        TestFramework::runTest("Order Item Addition", testOrderItemAddition);
        TestFramework::runTest("Order Item Removal", testOrderItemRemoval);
        TestFramework::runTest("Order Item Quantity Update", testOrderItemQuantityUpdate);
        TestFramework::runTest("Order Status Change", testOrderStatusChange);
    }
};

/**
 * @brief Test cases for OrderManager class
 */
class OrderManagerTests {
public:
    static void testOrderManagerCreation() {
        OrderManager manager;
        
        ASSERT_EQ(0, manager.getActiveOrderCount(), "New manager should have no active orders");
        ASSERT_EQ(0, manager.getCompletedOrderCount(), "New manager should have no completed orders");
        ASSERT_EQ(1000, manager.getNextOrderId(), "Starting order ID should be 1000");
    }
    
    static void testOrderCreation() {
        OrderManager manager;
        
        auto order = manager.createOrder(5);
        
        ASSERT_TRUE(order != nullptr, "Created order should not be null");
        ASSERT_EQ(1000, order->getOrderId(), "First order should have ID 1000");
        ASSERT_EQ(5, order->getTableNumber(), "Order should have correct table number");
        ASSERT_EQ(1, manager.getActiveOrderCount(), "Manager should have 1 active order");
    }
    
    static void testOrderRetrieval() {
        OrderManager manager;
        
        auto order1 = manager.createOrder(5);
        auto order2 = manager.createOrder(10);
        
        auto retrieved = manager.getOrder(1000);
        ASSERT_TRUE(retrieved != nullptr, "Should retrieve order by ID");
        ASSERT_EQ(1000, retrieved->getOrderId(), "Retrieved order should have correct ID");
        
        auto notFound = manager.getOrder(9999);
        ASSERT_TRUE(notFound == nullptr, "Should return null for non-existent order");
    }
    
    static void testOrderCompletion() {
        OrderManager manager;
        
        auto order = manager.createOrder(5);
        int orderId = order->getOrderId();
        
        bool completed = manager.completeOrder(orderId);
        
        ASSERT_TRUE(completed, "Order completion should succeed");
        ASSERT_EQ(0, manager.getActiveOrderCount(), "Should have no active orders after completion");
        ASSERT_EQ(1, manager.getCompletedOrderCount(), "Should have 1 completed order");
        
        auto retrieved = manager.getOrder(orderId);
        ASSERT_TRUE(retrieved == nullptr, "Completed order should not be in active orders");
    }
    
    static void testOrdersByTable() {
        OrderManager manager;
        
        manager.createOrder(5);
        manager.createOrder(5);
        manager.createOrder(10);
        
        auto table5Orders = manager.getOrdersByTable(5);
        auto table10Orders = manager.getOrdersByTable(10);
        
        ASSERT_EQ(2, table5Orders.size(), "Should have 2 orders for table 5");
        ASSERT_EQ(1, table10Orders.size(), "Should have 1 order for table 10");
    }
    
    static void runAllTests() {
        TestFramework::runTest("OrderManager Creation", testOrderManagerCreation);
        TestFramework::runTest("Order Creation", testOrderCreation);
        TestFramework::runTest("Order Retrieval", testOrderRetrieval);
        TestFramework::runTest("Order Completion", testOrderCompletion);
        TestFramework::runTest("Orders By Table", testOrdersByTable);
    }
};

/**
 * @brief Test cases for PaymentProcessor class
 */
class PaymentProcessorTests {
public:
    static void testPaymentProcessorCreation() {
        PaymentProcessor processor;
        
        ASSERT_EQ(0, processor.getTransactionHistory().size(), "New processor should have no transactions");
    }
    
    static void testCashPayment() {
        PaymentProcessor processor;
        OrderManager manager;
        MenuItem item(1, "Test Item", 10.00, MenuItem::MAIN_COURSE);
        
        auto order = manager.createOrder(5);
        order->addItem(OrderItem(item, 1));
        
        auto result = processor.processPayment(order, PaymentProcessor::CASH, order->getTotal());
        
        ASSERT_TRUE(result.success, "Cash payment should succeed");
        ASSERT_FALSE(result.transactionId.empty(), "Should generate transaction ID");
        ASSERT_EQ(PaymentProcessor::CASH, result.method, "Should record correct payment method");
        ASSERT_NEAR(order->getTotal(), result.amountProcessed, 0.01, "Should process correct amount");
    }
    
    static void testInvalidPaymentAmount() {
        PaymentProcessor processor;
        OrderManager manager;
        MenuItem item(1, "Test Item", 10.00, MenuItem::MAIN_COURSE);
        
        auto order = manager.createOrder(5);
        order->addItem(OrderItem(item, 1));
        
        // Test overpayment
        auto result = processor.processPayment(order, PaymentProcessor::CASH, order->getTotal() + 100);
        ASSERT_FALSE(result.success, "Should reject overpayment");
        
        // Test negative payment
        result = processor.processPayment(order, PaymentProcessor::CASH, -5.00);
        ASSERT_FALSE(result.success, "Should reject negative payment");
        
        // Test zero payment
        result = processor.processPayment(order, PaymentProcessor::CASH, 0.00);
        ASSERT_FALSE(result.success, "Should reject zero payment");
    }
    
    static void testSplitPayment() {
        PaymentProcessor processor;
        OrderManager manager;
        MenuItem item(1, "Test Item", 20.00, MenuItem::MAIN_COURSE);
        
        auto order = manager.createOrder(5);
        order->addItem(OrderItem(item, 1));
        
        std::vector<std::pair<PaymentProcessor::PaymentMethod, double>> payments = {
            {PaymentProcessor::CASH, 10.00},
            {PaymentProcessor::CREDIT_CARD, order->getTotal() - 10.00}
        };
        
        auto results = processor.processSplitPayment(order, payments);
        
        ASSERT_EQ(2, results.size(), "Should have 2 payment results");
        ASSERT_TRUE(results[0].success, "First payment should succeed");
        ASSERT_TRUE(results[1].success, "Second payment should succeed");
    }
    
    static void runAllTests() {
        TestFramework::runTest("PaymentProcessor Creation", testPaymentProcessorCreation);
        TestFramework::runTest("Cash Payment", testCashPayment);
        TestFramework::runTest("Invalid Payment Amount", testInvalidPaymentAmount);
        TestFramework::runTest("Split Payment", testSplitPayment);
    }
};

/**
 * @brief Test cases for KitchenInterface class
 */
class KitchenInterfaceTests {
public:
    static void testKitchenInterfaceCreation() {
        KitchenInterface kitchen;
        
        ASSERT_EQ(0, kitchen.getQueueLength(), "New kitchen interface should have empty queue");
        ASSERT_FALSE(kitchen.isKitchenBusy(), "Kitchen should not be busy with empty queue");
    }
    
    static void testSendOrderToKitchen() {
        KitchenInterface kitchen;
        OrderManager manager;
        MenuItem item(1, "Test Item", 10.00, MenuItem::MAIN_COURSE);
        
        auto order = manager.createOrder(5);
        order->addItem(OrderItem(item, 2));
        
        bool sent = kitchen.sendOrderToKitchen(order);
        
        ASSERT_TRUE(sent, "Should successfully send order to kitchen");
        ASSERT_EQ(1, kitchen.getQueueLength(), "Kitchen queue should have 1 order");
        ASSERT_EQ(Order::SENT_TO_KITCHEN, order->getStatus(), "Order status should be SENT_TO_KITCHEN");
    }
    
    static void testKitchenStatusUpdate() {
        KitchenInterface kitchen;
        OrderManager manager;
        MenuItem item(1, "Test Item", 10.00, MenuItem::MAIN_COURSE);
        
        auto order = manager.createOrder(5);
        order->addItem(OrderItem(item, 1));
        kitchen.sendOrderToKitchen(order);
        
        bool updated = kitchen.updateKitchenStatus(order->getOrderId(), KitchenInterface::PREP_STARTED);
        
        ASSERT_TRUE(updated, "Should successfully update kitchen status");
        
        auto ticket = kitchen.getTicketByOrderId(order->getOrderId());
        ASSERT_TRUE(ticket != nullptr, "Should find ticket by order ID");
        ASSERT_EQ(KitchenInterface::PREP_STARTED, ticket->status, "Ticket status should be updated");
    }
    
    static void testOrderServed() {
        KitchenInterface kitchen;
        OrderManager manager;
        MenuItem item(1, "Test Item", 10.00, MenuItem::MAIN_COURSE);
        
        auto order = manager.createOrder(5);
        order->addItem(OrderItem(item, 1));
        kitchen.sendOrderToKitchen(order);
        
        kitchen.updateKitchenStatus(order->getOrderId(), KitchenInterface::SERVED);
        
        ASSERT_EQ(0, kitchen.getQueueLength(), "Kitchen queue should be empty after serving");
        
        auto ticket = kitchen.getTicketByOrderId(order->getOrderId());
        ASSERT_TRUE(ticket == nullptr, "Served ticket should be removed from queue");
    }
    
    static void testKitchenBusyStatus() {
        KitchenInterface kitchen;
        OrderManager manager;
        MenuItem item(1, "Test Item", 10.00, MenuItem::MAIN_COURSE);
        
        // Add orders to make kitchen busy (default threshold is 5)
        for (int i = 0; i < 6; ++i) {
            auto order = manager.createOrder(i + 1);
            order->addItem(OrderItem(item, 1));
            kitchen.sendOrderToKitchen(order);
        }
        
        ASSERT_TRUE(kitchen.isKitchenBusy(), "Kitchen should be busy with 6 orders");
        ASSERT_TRUE(kitchen.getEstimatedWaitTime() > 0, "Should have positive estimated wait time");
    }
    
    static void runAllTests() {
        TestFramework::runTest("KitchenInterface Creation", testKitchenInterfaceCreation);
        TestFramework::runTest("Send Order to Kitchen", testSendOrderToKitchen);
        TestFramework::runTest("Kitchen Status Update", testKitchenStatusUpdate);
        TestFramework::runTest("Order Served", testOrderServed);
        TestFramework::runTest("Kitchen Busy Status", testKitchenBusyStatus);
    }
};

/**
 * @brief Integration tests that verify components work together
 */
class IntegrationTests {
public:
    static void testCompleteOrderFlow() {
        // Initialize all components
        OrderManager orderManager;
        PaymentProcessor paymentProcessor;
        KitchenInterface kitchenInterface;
        
        // Create menu items
        MenuItem appetizer(1, "Caesar Salad", 8.99, MenuItem::APPETIZER);
        MenuItem mainCourse(2, "Grilled Salmon", 24.99, MenuItem::MAIN_COURSE);
        
        // Create order
        auto order = orderManager.createOrder(5);
        order->addItem(OrderItem(appetizer, 1));
        order->addItem(OrderItem(mainCourse, 1));
        
        ASSERT_EQ(2, order->getItems().size(), "Order should have 2 items");
        ASSERT_NEAR(33.98, order->getSubtotal(), 0.01, "Subtotal should be correct");
        
        // Send to kitchen
        bool sentToKitchen = kitchenInterface.sendOrderToKitchen(order);
        ASSERT_TRUE(sentToKitchen, "Order should be sent to kitchen successfully");
        ASSERT_EQ(Order::SENT_TO_KITCHEN, order->getStatus(), "Order status should be updated");
        
        // Process kitchen workflow
        kitchenInterface.updateKitchenStatus(order->getOrderId(), KitchenInterface::PREP_STARTED);
        kitchenInterface.updateKitchenStatus(order->getOrderId(), KitchenInterface::READY_FOR_PICKUP);
        
        // Process payment
        auto paymentResult = paymentProcessor.processPayment(
            order, PaymentProcessor::CASH, order->getTotal()
        );
        ASSERT_TRUE(paymentResult.success, "Payment should be processed successfully");
        
        // Complete order
        bool completed = orderManager.completeOrder(order->getOrderId());
        ASSERT_TRUE(completed, "Order should be completed successfully");
        
        // Serve order (removes from kitchen queue)
        kitchenInterface.updateKitchenStatus(order->getOrderId(), KitchenInterface::SERVED);
        
        ASSERT_EQ(0, orderManager.getActiveOrderCount(), "Should have no active orders");
        ASSERT_EQ(1, orderManager.getCompletedOrderCount(), "Should have 1 completed order");
        ASSERT_EQ(0, kitchenInterface.getQueueLength(), "Kitchen queue should be empty");
    }
    
    static void testMultipleTableOrders() {
        OrderManager orderManager;
        KitchenInterface kitchenInterface;
        MenuItem item(1, "Test Item", 10.00, MenuItem::MAIN_COURSE);
        
        // Create orders for different tables
        auto order1 = orderManager.createOrder(1);
        auto order2 = orderManager.createOrder(2);
        auto order3 = orderManager.createOrder(1); // Same table as order1
        
        order1->addItem(OrderItem(item, 1));
        order2->addItem(OrderItem(item, 2));
        order3->addItem(OrderItem(item, 1));
        
        // Send all to kitchen
        kitchenInterface.sendOrderToKitchen(order1);
        kitchenInterface.sendOrderToKitchen(order2);
        kitchenInterface.sendOrderToKitchen(order3);
        
        ASSERT_EQ(3, orderManager.getActiveOrderCount(), "Should have 3 active orders");
        ASSERT_EQ(3, kitchenInterface.getQueueLength(), "Kitchen should have 3 orders");
        
        // Check orders by table
        auto table1Orders = orderManager.getOrdersByTable(1);
        auto table2Orders = orderManager.getOrdersByTable(2);
        
        ASSERT_EQ(2, table1Orders.size(), "Table 1 should have 2 orders");
        ASSERT_EQ(1, table2Orders.size(), "Table 2 should have 1 order");
    }
    
    static void runAllTests() {
        TestFramework::runTest("Complete Order Flow", testCompleteOrderFlow);
        TestFramework::runTest("Multiple Table Orders", testMultipleTableOrders);
    }
};

/**
 * @brief Main test runner
 */
int main() {
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "RESTAURANT POS SYSTEM - UNIT TESTS" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Testing three-legged foundation:" << std::endl;
    std::cout << "1. Order Management" << std::endl;
    std::cout << "2. Payment Processing" << std::endl;
    std::cout << "3. Kitchen Interface" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << std::endl;
    
    // Run all test suites
    std::cout << "📋 MenuItem Tests:" << std::endl;
    MenuItemTests::runAllTests();
    std::cout << std::endl;
    
    std::cout << "📋 Order Tests:" << std::endl;
    OrderTests::runAllTests();
    std::cout << std::endl;
    
    std::cout << "📋 OrderManager Tests:" << std::endl;
    OrderManagerTests::runAllTests();
    std::cout << std::endl;
    
    std::cout << "📋 PaymentProcessor Tests:" << std::endl;
    PaymentProcessorTests::runAllTests();
    std::cout << std::endl;
    
    std::cout << "📋 KitchenInterface Tests:" << std::endl;
    KitchenInterfaceTests::runAllTests();
    std::cout << std::endl;
    
    std::cout << "📋 Integration Tests:" << std::endl;
    IntegrationTests::runAllTests();
    std::cout << std::endl;
    
    // Print test summary
    TestFramework::printSummary();
    
    // Return appropriate exit code
    return TestFramework::allTestsPassed() ? 0 : 1;
}
