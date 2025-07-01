# Restaurant POS System - Modular Architecture v2.0.0

A modern, modular Restaurant Point-of-Sale system built with C++ and the Wt web framework. This system demonstrates enterprise-level software architecture with clean separation of concerns, event-driven communication, and responsive web UI.

## 🏗️ Architecture Overview

### Modular Design
- **Core Services**: Business logic coordination and configuration management
- **Event System**: Loose coupling between components via publish-subscribe pattern
- **Business Logic**: Order management, payment processing, and kitchen interface
- **UI Components**: Responsive web interface built with Wt framework

### Key Features
- ✅ **Order Management**: Create, modify, and track customer orders
- ✅ **Payment Processing**: Multiple payment methods with transaction tracking
- ✅ **Kitchen Interface**: Real-time order communication with kitchen systems
- ✅ **Event-Driven Architecture**: Loose coupling for maintainability and testing
- ✅ **Responsive Design**: Modern web UI that works on all devices
- ✅ **Real-Time Updates**: Live status updates and notifications

## 🚀 Quick Start

### Prerequisites

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install cmake build-essential libwt-dev libwthttp-dev
```

#### Fedora/RHEL/CentOS
```bash
sudo dnf install cmake gcc-c++ wt-devel
```

#### macOS
```bash
brew install cmake wt
```

### Build and Run

1. **Clone and prepare the project:**
```bash
# Create project directory
mkdir restaurant-pos && cd restaurant-pos

# Copy all the source files to their respective directories
# (Use the artifacts provided to create the file structure)
```

2. **Set up the directory structure:**
```
restaurant-pos/
├── CMakeLists.txt
├── wt_config.xml.in
├── build_and_run.sh
├── src/
│   ├── main.cpp
│   ├── core/
│   │   └── RestaurantPOSApp.cpp
│   ├── services/
│   │   └── POSService.cpp
│   ├── events/
│   │   ├── EventManager.cpp
│   │   └── POSEvents.cpp
│   ├── MenuItem.cpp
│   ├── Order.cpp
│   ├── OrderManager.cpp
│   ├── PaymentProcessor.cpp
│   └── KitchenInterface.cpp
├── include/
│   └── (all header files)
└── resources/ (optional)
```

3. **Build and run:**
```bash
chmod +x build_and_run.sh
./build_and_run.sh
```

4. **Access the application:**
   - Open your browser to: http://localhost:8080/pos
   - The application will display the POS interface

## 🔧 Manual Build (Alternative)

If you prefer to build manually:

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Run
cd bin
./restaurant-pos --config=../wt_config.xml --http-address=0.0.0.0 --http-port=8080
```

## 🏛️ System Architecture

### Service Layer
- **POSService**: Central business logic coordinator
- **EventManager**: Publish-subscribe event system
- **ConfigurationManager**: Centralized configuration management

### Business Logic Layer
- **OrderManager**: Order lifecycle management
- **PaymentProcessor**: Multi-method payment handling
- **KitchenInterface**: Kitchen communication and workflow

### Event System
```cpp
// Event-driven communication example
eventManager->subscribe(POSEvents::ORDER_CREATED, 
    [](const std::any& data) {
        // Handle order creation
    });

eventManager->publish(POSEvents::ORDER_CREATED, orderData);
```

## 🧪 Testing the System

### Basic Functionality Test
1. **Open the application** in your browser
2. **Click "Test Order Creation"** to create a sample order
3. **Observe the status updates** showing order creation and item additions
4. **Check the console output** for detailed logging

### Order Management Test
```cpp
// Create order
auto order = posService->createOrder(5); // Table 5

// Add items
posService->setCurrentOrder(order);
posService->addItemToCurrentOrder(menuItem, 2, "Extra sauce");

// Send to kitchen
posService->sendCurrentOrderToKitchen();

// Process payment
auto result = posService->processCurrentOrderPayment(
    PaymentProcessor::CREDIT_CARD, order->getTotal());
```

## 🔍 Key Components

### POSService (Central Coordinator)
```cpp
class POSService {
public:
    // Order management
    std::shared_ptr<Order> createOrder(int tableNumber);
    bool addItemToCurrentOrder(std::shared_ptr<MenuItem> item, int qty);
    
    // Kitchen operations
    bool sendOrderToKitchen(std::shared_ptr<Order> order);
    
    // Payment processing
    PaymentResult processOrderPayment(PaymentMethod method, double amount);
};
```

### Event System
```cpp
// Subscribe to events
eventManager->subscribe(POSEvents::ORDER_CREATED, callback);

// Publish events
eventManager->publish(POSEvents::PAYMENT_COMPLETED, paymentData);
```

### Order Management
```cpp
// Order workflow
Order order(orderId, tableNumber);
order.addItem(OrderItem(menuItem, quantity));
order.setStatus(Order::SENT_TO_KITCHEN);
```

## 🛠️ Development

### Adding New Features

1. **Create business logic** in appropriate service classes
2. **Define events** in POSEvents.hpp for component communication
3. **Implement UI components** that subscribe to relevant events
4. **Register components** with the UIComponentFactory

### Event-Driven Development
```cpp
// 1. Define event constants
const std::string FEATURE_EVENT = "feature.event";

// 2. Create event data structures
struct FeatureEventData {
    std::string message;
    int value;
};

// 3. Publish events
eventManager->publish(FEATURE_EVENT, eventData);

// 4. Subscribe to events
eventManager->subscribe(FEATURE_EVENT, [](const std::any& data) {
    auto featureData = std::any_cast<FeatureEventData>(data);
    // Handle the event
});
```

## 📁 Project Structure

```
restaurant-pos/
├── src/                      # Source files
│   ├── core/                # Application core
│   ├── services/            # Business services
│   ├── events/              # Event system
│   └── *.cpp               # Business logic implementations
├── include/                  # Header files
│   ├── core/               # Core headers
│   ├── services/           # Service headers
│   ├── events/             # Event headers
│   ├── ui/                 # UI component headers
│   └── utils/              # Utility headers
├── build/                   # Build artifacts
├── resources/               # Static resources
└── docs/                   # Documentation
```

## 🚦 Common Issues and Solutions

### Issue: Wt Library Not Found
```bash
# Ubuntu/Debian
sudo apt-get install libwt-dev libwthttp-dev

# Fedora/RHEL
sudo dnf install wt-devel

# macOS
brew install wt
```

### Issue: Port Already in Use
```bash
# Find process using port 8080
sudo lsof -i :8080

# Kill the process
sudo kill -9 <PID>

# Or use a different port
./restaurant-pos --http-port=8081
```

### Issue: CMake Version Too Old
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# Or install from source
wget https://cmake.org/files/v3.21/cmake-3.21.0.tar.gz
```

## 🎯 Next Steps

1. **Extend UI Components**: Add more sophisticated UI panels
2. **Database Integration**: Add persistent storage
3. **Authentication**: Implement user management
4. **Reporting**: Add sales and analytics reports
5. **API Integration**: Connect to external payment gateways

## 📈 Performance Notes

- **Memory Management**: Uses smart pointers for automatic memory management
- **Event System**: Efficient publish-subscribe with minimal overhead
- **Web Framework**: Wt provides excellent performance for web applications
- **Modular Design**: Components can be optimized independently

## 🤝 Contributing

This is a demonstration of modular architecture principles. To extend:

1. Follow the existing patterns for new components
2. Use the event system for component communication
3. Maintain separation of concerns between layers
4. Add comprehensive error handling
5. Include unit tests for new functionality

## 📄 License

This is a demonstration project showcasing modular architecture patterns in C++.

---

**Happy coding! 🚀**

For questions about the architecture or implementation details, refer to the comprehensive comments in the source code.