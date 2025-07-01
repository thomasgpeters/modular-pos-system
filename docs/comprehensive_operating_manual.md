# 🍽️ Restaurant POS System - Comprehensive Operating Manual

**Version 2.0.0 - Modular Architecture**  
**Framework:** C++ with Wt Web Toolkit  
**Architecture:** Event-Driven Modular Design

---

## 📋 Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Server Operations](#server-operations)
3. [POS System Features](#pos-system-features)
4. [Business Workflows](#business-workflows)
5. [User Interface Guide](#user-interface-guide)
6. [Configuration & Settings](#configuration--settings)
7. [API & Integration](#api--integration)
8. [Troubleshooting](#troubleshooting)
9. [System Architecture](#system-architecture)
10. [Developer Guide](#developer-guide)

---

## 🚀 Quick Start Guide

### Starting the Server

```bash
# Navigate to your project directory
cd /Users/thomaspeters/Developer/C-Projects/Restuarant-Management/modular-pos-system

# Start the server
cd build/bin
./restaurant-pos --config=../wt_config.xml --http-address=0.0.0.0 --http-port=8080

# Alternative: Use with custom settings
./restaurant-pos --docroot=../../resources --http-port=8081
```

### Accessing the System

- **Main URL:** http://localhost:8080/pos
- **Server Status:** Watch console output for real-time logs
- **Stop Server:** Press `Ctrl+C`

### First Steps

1. **Open Browser** → Navigate to http://localhost:8080/pos
2. **Test Basic Function** → Click "Test Order Creation"
3. **Verify Status** → Check real-time updates every 5 seconds
4. **Explore Menu** → Review the 10 sample menu items
5. **Check Console** → Monitor event system logging

---

## 🖥️ Server Operations

### Server Startup

```bash
# Standard startup
./restaurant-pos --config=../wt_config.xml --http-port=8080

# Development mode with verbose logging
./restaurant-pos --config=../wt_config.xml --http-port=8080 --log-level=debug

# Custom document root for static files
./restaurant-pos --docroot=/path/to/resources --http-port=8080
```

### Server Configuration Options

| Parameter | Description | Default |
|-----------|-------------|---------|
| `--http-port` | Server port | 8080 |
| `--http-address` | Bind address | 0.0.0.0 |
| `--config` | Configuration file | wt_config.xml |
| `--docroot` | Static files directory | resources |
| `--log-level` | Logging level | info |

### Monitoring Server Health

**Console Output Indicators:**
```
✅ Restaurant POS Application initialized successfully
✅ All services initialized
✅ Main layout setup complete
🌐 Server starting on http://localhost:8080/pos
```

**Performance Metrics:**
- **Memory Usage:** Monitor via system tools
- **Active Connections:** Check server logs
- **Event Processing:** Watch console event flow
- **Response Times:** Browser developer tools

### Server Management

```bash
# Check if server is running
lsof -i :8080

# Kill running server
sudo kill -9 $(lsof -t -i:8080)

# Restart with different port
./restaurant-pos --http-port=8081
```

---

## 🍽️ POS System Features

### Core Business Functions

#### Order Management
- **Create Orders** for specific tables
- **Add Menu Items** with quantities and special instructions
- **Modify Orders** before sending to kitchen
- **Track Order Status** through complete lifecycle
- **Calculate Totals** with automatic tax computation

#### Payment Processing
- **Multiple Payment Methods:**
  - Cash payments with change calculation
  - Credit/Debit card processing
  - Mobile payments (Apple Pay, Google Pay)
  - Gift card redemption
- **Split Payments** across multiple methods
- **Tip Management** with suggested percentages
- **Transaction History** and audit trail

#### Kitchen Operations
- **Send Orders to Kitchen** with preparation instructions
- **Track Kitchen Status** (Received → Preparing → Ready → Served)
- **Kitchen Queue Management** with estimated wait times
- **Special Instructions** and dietary requirements
- **Kitchen Load Monitoring** and capacity alerts

### Advanced Features

#### Event-Driven Architecture
- **Real-Time Updates** across all system components
- **Component Communication** via publish-subscribe events
- **Workflow Automation** based on business rules
- **Extensible Design** for easy feature additions

#### Business Intelligence
- **Order Statistics** and performance metrics
- **Revenue Tracking** with payment method breakdown
- **Kitchen Efficiency** monitoring and optimization
- **Customer Service** metrics and analysis

---

## 💼 Business Workflows

### 🔄 Standard Order Flow

#### 1. Order Creation
```
Staff Action → Create New Order
System Action → Assign Order ID, Set Table Number
Result → Order #1001 created for Table 5
```

#### 2. Menu Selection
```
Staff Action → Browse Menu, Select Items
System Action → Add Items to Order, Calculate Subtotal
Result → 2x Caesar Salad ($25.98) + 1x Ribeye Steak ($32.99)
```

#### 3. Order Customization
```
Staff Action → Add Special Instructions
System Action → Attach Notes to Order Items
Result → "Caesar Salad - Extra croutons, dressing on side"
```

#### 4. Kitchen Transmission
```
Staff Action → Send Order to Kitchen
System Action → Create Kitchen Ticket, Update Order Status
Result → Kitchen displays order, prep timer starts
```

#### 5. Kitchen Processing
```
Kitchen Action → Update Preparation Status
System Action → Broadcast Status Changes
Result → Order Status: Preparing → Ready → Served
```

#### 6. Payment Processing
```
Staff Action → Process Payment
System Action → Calculate Total, Process Transaction
Result → Payment complete, order closed
```

### 🍳 Kitchen Workflow

#### Kitchen Display Operations
- **Incoming Orders** appear automatically
- **Preparation Status** updated by kitchen staff
- **Special Instructions** prominently displayed
- **Estimated Times** calculated and shown

#### Status Management
| Status | Description | Action Required |
|--------|-------------|-----------------|
| **Order Received** | New order in queue | Begin preparation |
| **Prep Started** | Kitchen working on order | Continue cooking |
| **Ready for Pickup** | Order complete | Server pickup |
| **Served** | Delivered to customer | Close ticket |

### 💳 Payment Workflows

#### Cash Payment
1. Enter payment amount
2. System calculates change
3. Process transaction
4. Print receipt

#### Card Payment
1. Select card type (Credit/Debit)
2. Process through payment gateway
3. Handle approval/decline
4. Store transaction record

#### Split Payment
1. Enter first payment method and amount
2. Add additional payment methods
3. Verify total coverage
4. Process all transactions

---

## 🎨 User Interface Guide

### Main Application Layout

```
┌─────────────────────────────────────────────────────────┐
│ 🍽️ Restaurant POS System                    [Theme ▼] │
├─────────────────────────────────────────────────────────┤
│ Order Entry Panel (60%)    │ Order Status Panel (40%) │
│                            │                          │
│ ┌─ Table Selection ─────┐  │ ┌─ Active Orders ──────┐ │
│ │ Table: [5] [New Order]│  │ │ Order #1001  Table 5 │ │
│ └───────────────────────┘  │ │ Order #1002  Table 3 │ │
│                            │ │ Order #1003  Table 1 │ │
│ ┌─ Menu Display ────────┐  │ └──────────────────────┘ │
│ │ 🥗 Caesar Salad  $12.99│  │                          │
│ │ 🍗 Buffalo Wings $14.99│  │ ┌─ Kitchen Status ─────┐ │
│ │ 🐟 Grilled Salmon $24.99│ │ │ Queue: 3 orders      │ │
│ │ 🥩 Ribeye Steak $32.99│  │ │ Wait Time: 15 min    │ │
│ └───────────────────────┘  │ │ Status: Busy         │ │
│                            │ └──────────────────────┘ │
│ ┌─ Current Order ───────┐  │                          │
│ │ Order #1001  Table 5  │  │ ┌─ System Status ──────┐ │
│ │ 2x Caesar Salad $25.98│  │ │ Active Orders: 3     │ │
│ │ 1x Ribeye Steak $32.99│  │ │ Total Revenue: $245  │ │
│ │ Subtotal:      $58.97 │  │ │ Transactions: 12     │ │
│ │ Tax (8%):      $4.72  │  │ │ Last Update: 14:32   │ │
│ │ Total:         $63.69 │  │ └──────────────────────┘ │
│ └───────────────────────┘  │                          │
│                            │                          │
│ [Send to Kitchen] [Payment]│                          │
└─────────────────────────────────────────────────────────┘
```

### Navigation Elements

#### Header Bar
- **Application Title** with restaurant branding
- **Theme Selector** for visual customization
- **System Status** indicator

#### Order Entry Panel (Left Side)
- **Table Selection** with number input
- **Menu Browser** with categorized items
- **Current Order Display** with running totals
- **Action Buttons** for kitchen/payment

#### Order Status Panel (Right Side)
- **Active Orders List** with real-time updates
- **Kitchen Status Display** with queue information
- **System Metrics** and performance data

### Interactive Elements

#### Buttons and Controls
- **Primary Actions:** Blue buttons for main operations
- **Secondary Actions:** Gray buttons for supporting functions
- **Dangerous Actions:** Red buttons for delete/cancel
- **Input Fields:** Clean, responsive form controls

#### Real-Time Updates
- **Status Changes** automatically refresh
- **New Orders** appear without page reload
- **Kitchen Updates** broadcast to all screens
- **Payment Confirmations** show immediately

### Responsive Design
- **Desktop:** Full layout with side-by-side panels
- **Tablet:** Stacked layout with tab navigation
- **Mobile:** Compact view with essential functions

---

## ⚙️ Configuration & Settings

### Server Configuration (wt_config.xml)

```xml
<?xml version="1.0" encoding="utf-8"?>
<server>
    <application-settings location="*">
        <session-timeout>600</session-timeout>
        <max-request-size>128</max-request-size>
        
        <properties>
            <property name="restaurantName">Your Restaurant Name</property>
            <property name="taxRate">0.08</property>
            <property name="maxActiveOrders">50</property>
        </properties>
    </application-settings>
    
    <http-server>
        <bind>0.0.0.0</bind>
        <port>8080</port>
        <threads>10</threads>
    </http-server>
</server>
```

### Business Settings

#### Restaurant Information
```cpp
// Configurable in POSService
restaurantName = "Your Restaurant Name";
restaurantAddress = "123 Main St, City, State";
restaurantPhone = "(555) 123-4567";
taxRate = 0.08; // 8% sales tax
```

#### Order Management
```cpp
// Order settings
startingOrderId = 1000;
orderTimeout = 30; // minutes
maxItemsPerOrder = 50;
```

#### Kitchen Configuration
```cpp
// Kitchen settings
kitchenRefreshRate = 5; // seconds
kitchenBusyThreshold = 5; // orders
estimatedPrepTimes = {
    {"APPETIZER", 10},    // minutes
    {"MAIN_COURSE", 20},
    {"DESSERT", 8},
    {"BEVERAGE", 2}
};
```

#### Payment Settings
```cpp
// Payment configuration
enabledPaymentMethods = {"CASH", "CREDIT_CARD", "MOBILE_PAY"};
tipSuggestions = {0.15, 0.18, 0.20, 0.25}; // percentages
allowSplitPayments = true;
```

### Menu Configuration

#### Sample Menu Items
```cpp
// Built-in menu items
menuItems = {
    {1, "Caesar Salad", 12.99, APPETIZER},
    {2, "Buffalo Wings", 14.99, APPETIZER},
    {3, "Grilled Salmon", 24.99, MAIN_COURSE},
    {4, "Ribeye Steak", 32.99, MAIN_COURSE},
    {5, "Margherita Pizza", 18.99, MAIN_COURSE},
    {6, "Chocolate Cake", 8.99, DESSERT},
    {7, "Tiramisu", 9.99, DESSERT},
    {8, "Coca Cola", 3.99, BEVERAGE},
    {9, "Coffee", 4.99, BEVERAGE},
    {10, "Chef's Special", 28.99, SPECIAL}
};
```

---

## 🔌 API & Integration

### Event System API

#### Subscribing to Events
```cpp
// Order events
eventManager->subscribe(POSEvents::ORDER_CREATED, 
    [](const std::any& data) {
        auto orderData = std::any_cast<POSEvents::OrderEventData>(data);
        // Handle order creation
    });

// Kitchen events
eventManager->subscribe(POSEvents::KITCHEN_STATUS_CHANGED,
    [](const std::any& data) {
        auto kitchenData = std::any_cast<POSEvents::KitchenEventData>(data);
        // Handle kitchen status updates
    });

// Payment events
eventManager->subscribe(POSEvents::PAYMENT_COMPLETED,
    [](const std::any& data) {
        auto paymentResult = std::any_cast<PaymentProcessor::PaymentResult>(data);
        // Handle payment completion
    });
```

#### Publishing Events
```cpp
// Publish order creation
eventManager->publish(POSEvents::ORDER_CREATED, 
    POSEvents::createOrderCreatedEvent(order));

// Publish kitchen status change
eventManager->publish(POSEvents::KITCHEN_STATUS_CHANGED,
    POSEvents::createKitchenStatusChangedEvent(orderId, newStatus, oldStatus));
```

### Business Logic API

#### POSService Interface
```cpp
// Order management
auto order = posService->createOrder(tableNumber);
posService->addItemToCurrentOrder(menuItem, quantity, instructions);
posService->sendCurrentOrderToKitchen();

// Payment processing
auto result = posService->processCurrentOrderPayment(method, amount, tip);

// Kitchen operations
posService->updateKitchenStatus(orderId, status);
auto tickets = posService->getKitchenTickets();

// Statistics
auto stats = posService->getBusinessStatistics();
```

### External Integration Points

#### Payment Gateway Integration
```cpp
class CustomPaymentProcessor : public PaymentProcessor {
protected:
    PaymentResult processCardPayment(std::shared_ptr<Order> order,
                                   PaymentMethod method,
                                   double amount, double tip) override {
        // Integrate with external payment gateway
        // Return PaymentResult with transaction details
    }
};
```

#### Kitchen Display Integration
```cpp
class CustomKitchenInterface : public KitchenInterface {
protected:
    bool broadcastToKitchen(const Wt::Json::Object& message) override {
        // Send to external kitchen display system
        // Return success status
    }
};
```

---

## 🛠️ Troubleshooting

### Common Issues and Solutions

#### Server Won't Start

**Problem:** Port already in use
```bash
# Check what's using the port
lsof -i :8080

# Kill the process
sudo kill -9 $(lsof -t -i:8080)

# Or use different port
./restaurant-pos --http-port=8081
```

**Problem:** Wt library not found
```bash
# macOS
brew install wt

# Ubuntu/Debian
sudo apt-get install libwt-dev libwthttp-dev

# Verify installation
pkg-config --modversion wt
```

#### Application Errors

**Problem:** Orders not saving
- **Check:** Event system initialization
- **Solution:** Verify EventManager is properly constructed
- **Debug:** Monitor console for event publishing errors

**Problem:** Kitchen status not updating
- **Check:** KitchenInterface connection
- **Solution:** Verify kitchen event subscriptions
- **Debug:** Check network connectivity to kitchen displays

**Problem:** Payment processing fails
- **Check:** PaymentProcessor configuration
- **Solution:** Verify payment method settings
- **Debug:** Review transaction logs and error messages

#### Performance Issues

**Problem:** Slow response times
- **Solutions:**
  - Increase server thread count in wt_config.xml
  - Optimize database queries (when added)
  - Monitor memory usage
  - Check network latency

**Problem:** High memory usage
- **Solutions:**
  - Monitor event subscription cleanup
  - Check for memory leaks in custom components
  - Optimize order history storage
  - Review smart pointer usage

### Debug Mode Operations

#### Enable Verbose Logging
```bash
# Start with debug logging
./restaurant-pos --config=../wt_config.xml --log-level=debug

# Monitor specific components
export WT_DEBUG_LOGGING=1
./restaurant-pos
```

#### System Diagnostics
```cpp
// Check system status
auto stats = posService->getBusinessStatistics();
std::cout << "Active Orders: " << stats["activeOrderCount"] << std::endl;
std::cout << "Kitchen Queue: " << stats["kitchenQueueSize"] << std::endl;
std::cout << "Total Revenue: $" << stats["totalRevenue"] << std::endl;

// Verify event system
std::cout << "Event subscribers: " 
          << eventManager->getSubscriberCount(POSEvents::ORDER_CREATED) 
          << std::endl;
```

### Log Analysis

#### Important Log Messages
```
✅ Restaurant POS Application initialized successfully
✅ All services initialized
🔄 Order created successfully! Order #1001
🍳 Order sent to kitchen: #1001
💳 Payment processed: $63.69 via CREDIT_CARD
❌ ERROR: Payment failed - card declined
⚠️  WARNING: Kitchen queue is busy (7 orders)
```

#### Error Patterns to Watch
- **Memory allocation failures**
- **Network connection timeouts**
- **Database connection errors** (when implemented)
- **Event system communication failures**

---

## 🏗️ System Architecture

### Modular Design Overview

```
┌─────────────────────────────────────────────────────────┐
│                    Presentation Layer                   │
├─────────────────────────────────────────────────────────┤
│ RestaurantPOSApp │ UI Components │ Dialogs & Factories │
├─────────────────────────────────────────────────────────┤
│                     Service Layer                       │
├─────────────────────────────────────────────────────────┤
│   POSService   │ ThemeService │ NotificationService    │
├─────────────────────────────────────────────────────────┤
│                   Business Logic                        │
├─────────────────────────────────────────────────────────┤
│ OrderManager │ PaymentProcessor │ KitchenInterface     │
├─────────────────────────────────────────────────────────┤
│                    Event System                         │
├─────────────────────────────────────────────────────────┤
│         EventManager │ POSEvents │ Event Bus           │
├─────────────────────────────────────────────────────────┤
│                   Data Models                           │
├─────────────────────────────────────────────────────────┤
│    Order │ OrderItem │ MenuItem │ Payment Results      │
└─────────────────────────────────────────────────────────┘
```

### Component Relationships

#### Core Business Flow
```
User Input → UI Components → POSService → Business Logic → Event System
    ↓             ↓              ↓             ↓             ↓
Real-time Updates ← Event Handlers ← Status Changes ← Data Updates
```

#### Event-Driven Communication
```
OrderManager ─┐
             ├─→ EventManager ─→ All Subscribers
PaymentProcessor ─┤               ↓
             ├─→ POSEvents ─→ Type-Safe Events
KitchenInterface ─┘               ↓
                             Real-time UI Updates
```

### Extension Points

#### Adding New Components
1. **Create Component Class** inheriting from appropriate base
2. **Register with EventManager** for relevant events
3. **Publish Events** for status changes
4. **Integrate with POSService** for business operations

#### Custom Business Rules
```cpp
class CustomOrderManager : public OrderManager {
public:
    void onOrderCreated(std::shared_ptr<Order> order) override {
        // Custom business logic
        if (order->getTotal() > 100.0) {
            // Apply VIP customer handling
        }
    }
};
```

---

## 👨‍💻 Developer Guide

### Building and Development

#### Development Setup
```bash
# Clone/setup project
git clone <repository>
cd modular-pos-system

# Install dependencies
brew install wt cmake  # macOS
# or
sudo apt-get install libwt-dev cmake  # Ubuntu

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Development Workflow
1. **Make Code Changes** in src/ or include/
2. **Rebuild** with `make` in build directory
3. **Test** changes with development server
4. **Monitor** console output for errors
5. **Debug** with IDE or command-line tools

### Adding New Features

#### 1. Create New Business Component
```cpp
// include/MyNewComponent.hpp
class MyNewComponent {
public:
    MyNewComponent(std::shared_ptr<EventManager> eventManager);
    void performAction();
    
private:
    std::shared_ptr<EventManager> eventManager_;
};

// src/MyNewComponent.cpp
void MyNewComponent::performAction() {
    // Business logic
    eventManager_->publish("my.custom.event", eventData);
}
```

#### 2. Integrate with POSService
```cpp
// In POSService.hpp
std::unique_ptr<MyNewComponent> myComponent_;

// In POSService.cpp constructor
myComponent_ = std::make_unique<MyNewComponent>(eventManager_);
```

#### 3. Add UI Component
```cpp
// include/ui/components/MyUIComponent.hpp
class MyUIComponent : public Wt::WContainerWidget {
public:
    MyUIComponent(std::shared_ptr<POSService> posService);
    
private:
    std::shared_ptr<POSService> posService_;
    void onButtonClicked();
};
```

#### 4. Register Event Handlers
```cpp
// Subscribe to events
eventManager_->subscribe("my.custom.event", 
    [this](const std::any& data) {
        handleCustomEvent(data);
    });
```

### Testing Strategy

#### Unit Testing Components
```cpp
// Test individual components
TEST(OrderManagerTest, CreateOrder) {
    auto eventManager = std::make_shared<EventManager>();
    OrderManager manager;
    
    auto order = manager.createOrder(5);
    EXPECT_EQ(order->getTableNumber(), 5);
    EXPECT_EQ(order->getStatus(), Order::PENDING);
}
```

#### Integration Testing
```cpp
// Test component interaction
TEST(POSServiceTest, OrderWorkflow) {
    auto eventManager = std::make_shared<EventManager>();
    POSService service(eventManager);
    
    // Test complete workflow
    auto order = service.createOrder(5);
    service.addItemToCurrentOrder(menuItem, 2);
    bool success = service.sendCurrentOrderToKitchen();
    EXPECT_TRUE(success);
}
```

#### End-to-End Testing
- **Automated Browser Testing** with Selenium
- **API Testing** for event system
- **Performance Testing** under load
- **User Acceptance Testing** with real workflows

### Code Style Guidelines

#### Naming Conventions
- **Classes:** PascalCase (`OrderManager`)
- **Methods:** camelCase (`createOrder`)
- **Variables:** camelCase (`orderTotal`)
- **Constants:** UPPER_SNAKE_CASE (`ORDER_CREATED`)
- **Files:** PascalCase (`OrderManager.hpp`)

#### Architecture Principles
- **Single Responsibility:** Each class has one clear purpose
- **Event-Driven:** Components communicate via events
- **Dependency Injection:** Services passed as constructor parameters
- **RAII:** Resource management with smart pointers
- **Const Correctness:** Use const wherever possible

### Performance Optimization

#### Event System Performance
- **Minimize Event Payload** - only send necessary data
- **Batch Events** when processing multiple items
- **Unsubscribe** from events when components are destroyed
- **Avoid Circular Dependencies** in event chains

#### Memory Management
- **Smart Pointers** for automatic memory management
- **RAII** for resource cleanup
- **Event Subscription Cleanup** in destructors
- **Minimize Object Copying** with move semantics

#### UI Performance
- **Lazy Loading** for large menu displays
- **Virtual Scrolling** for long order lists
- **Debounced Updates** for real-time status
- **Efficient Re-rendering** strategies

---

## 📞 Support & Resources

### Getting Help

#### Documentation
- **This Manual** - Comprehensive operating guide
- **Code Comments** - Inline documentation in source files
- **Header Files** - API documentation with method descriptions
- **Architecture Documentation** - System design and patterns

#### Community Resources
- **Wt Framework Documentation** - https://www.webtoolkit.eu/wt/doc/
- **C++ Reference** - https://cppreference.com/
- **Modern C++ Patterns** - Best practices and idioms

#### Professional Support
- **System Architecture** - Modular design consultation
- **Custom Development** - Feature additions and modifications
- **Performance Optimization** - Scalability and efficiency improvements
- **Integration Services** - External system connections

### Maintenance and Updates

#### Regular Maintenance
- **Monitor Server Logs** for errors and performance issues
- **Update Dependencies** when security patches available
- **Backup Configuration** files and custom modifications
- **Performance Monitoring** and optimization

#### Version Updates
- **Backup Current System** before major updates
- **Test New Features** in development environment
- **Update Documentation** for any configuration changes
- **Train Staff** on new functionality

#### Security Considerations
- **Network Security** - Secure server communications
- **Payment Security** - PCI compliance for payment processing
- **Data Privacy** - Customer information protection
- **Access Control** - User authentication and authorization

---

## 🎉 Conclusion

The Restaurant POS System provides a robust, modular foundation for restaurant operations with:

- ✅ **Complete Order Management** from creation to completion
- ✅ **Multi-Method Payment Processing** with transaction tracking
- ✅ **Real-Time Kitchen Communication** and status monitoring
- ✅ **Event-Driven Architecture** for scalability and maintenance
- ✅ **Modern Web Interface** with responsive design
- ✅ **Extensible Design** for future enhancements

The system is designed for **reliability**, **performance**, and **ease of use**, providing restaurant staff with the tools they need for efficient operations while maintaining the flexibility for future growth and customization.

**Ready to serve your restaurant's needs! 🚀**

---

*This manual covers version 2.0.0 of the Restaurant POS System. For the latest updates and additional resources, refer to the project documentation and development team.*