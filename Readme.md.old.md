# Restaurant POS System

A modern, extensible Point of Sale system built with C++ and Wt framework, focusing on the three core functions: Order Management, Payment Processing, and Kitchen Communications.

## Project Structure

```
restaurant-pos/
├── CMakeLists.txt              # Modern CMake build configuration
├── Makefile                    # Alternative build system
├── README.md                   # This documentation file
├── PROJECT_OVERVIEW.md         # Detailed architecture guide
├── pos_config.xml              # Configuration template
├── main.cpp                    # Application entry point
├── MenuItem.h/.cpp             # Menu item management
├── Order.h/.cpp                # Order and OrderItem classes
├── OrderManager.h/.cpp         # Order lifecycle management (LEG 1)
├── PaymentProcessor.h/.cpp     # Payment processing system (LEG 2)
├── KitchenInterface.h/.cpp     # Kitchen communication (LEG 3)
├── RestaurantPOSApp.h/.cpp     # Main application and UI
└── test_pos.cpp                # Comprehensive unit test suite
```

## Architecture: Three-Legged Foundation

### 1. Order Management (`OrderManager`)
- **Purpose**: Handles order creation, modification, and lifecycle tracking
- **Features**: 
  - Multi-table order management
  - Order status tracking
  - Item customizations and special instructions
  - Order history and analytics hooks
- **Extension Points**: Database persistence, order analytics, workflow automation

### 2. Payment Processing (`PaymentProcessor`)
- **Purpose**: Manages multiple payment methods and transaction processing
- **Features**:
  - Cash, credit/debit cards, mobile payments, gift cards
  - Split payment support
  - Transaction recording and audit trails
  - Refund processing
- **Extension Points**: Real payment gateway integration, fraud detection, loyalty programs

### 3. Kitchen Interface (`KitchenInterface`)
- **Purpose**: Real-time communication with kitchen display systems
- **Features**:
  - Order transmission to kitchen
  - Status tracking (received, preparing, ready, served)
  - Queue management and timing estimates
  - Kitchen workflow optimization
- **Extension Points**: WebSocket communication, kitchen equipment integration, inventory tracking

## Features

- **Modern Web Interface**: Bootstrap-themed responsive design
- **Real-time Updates**: Automatic refresh of order status and kitchen queue
- **Multi-table Support**: Handle multiple concurrent orders
- **Extensible Architecture**: Clean separation of concerns with extension points
- **JSON API Ready**: All data models support JSON serialization
- **Comprehensive Documentation**: Detailed inline documentation and examples

## Prerequisites

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libwt-dev libwthttp-dev
```

### CentOS/RHEL/Fedora
```bash
sudo yum groupinstall "Development Tools"
sudo yum install cmake pkg-config
sudo yum install wt-devel  # or compile from source
```

### macOS (with Homebrew)
```bash
brew install cmake pkg-config
brew install wt
```

## Building

### Option 1: Using CMake (Recommended)
```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make

# Run
./bin/restaurant_pos --http-port 8081 --docroot ../
```

### Option 2: Manual Compilation
```bash
# Compile all source files
g++ -std=c++17 -o restaurant_pos \
    main.cpp MenuItem.cpp Order.cpp OrderManager.cpp \
    PaymentProcessor.cpp KitchenInterface.cpp RestaurantPOSApp.cpp \
    $(pkg-config --cflags --libs wt wthttp)

# Run
./restaurant_pos --http-port 8081 --docroot .
```

## Running the Application

1. After building, start the server:
   ```bash
   ./restaurant_pos --http-port 8081--docroot .
   ```

2. Open your web browser and navigate to:
   ```
   http://localhost:8081
   ```

3. The POS interface will load with:
   - Left panel: Order entry and menu
   - Right panel: Active orders and status

## Usage

### Creating an Order
1. Set table number in the "Table" field
2. Click "New Order" to start
3. Click "Add" next to menu items to add them to the order
4. Review the current order in the bottom section

### Sending to Kitchen
1. After adding items, click "Send to Kitchen"
2. Order will appear in the Active Orders panel
3. Order status will update as it progresses

### Processing Payment
1. Click "Process Payment" for the current order
2. Currently configured for cash payments (demo)
3. Order will be completed and removed from active list

## Configuration Options

The server accepts various command-line options:

```bash
./restaurant_pos --help
```

Common options:
- `--http-port 8081` - Set HTTP port (default: 8081)
- `--docroot .` - Set document root directory
- `--http-address 0.0.0.0` - Bind to all interfaces
- `--config wt_config.xml` - Use configuration file

## Architecture

### Core Components (Three-Legged Foundation)

1. **OrderManager**: Handles order lifecycle
   - Order creation and modification
   - Status tracking
   - Order completion

2. **PaymentProcessor**: Manages transactions
   - Multiple payment methods
   - Split payment support
   - Transaction recording

3. **KitchenInterface**: Kitchen communication
   - Real-time order transmission
   - Status updates
   - Queue management

### Extension Points

The system is designed for easy extension:

- **Database Integration**: Replace in-memory storage
- **Payment Gateways**: Add real payment processor APIs
- **WebSocket Communication**: Enable real-time updates
- **User Authentication**: Add staff login system
- **Inventory Management**: Track ingredient usage
- **Reporting System**: Add sales analytics

## Development

### File Organization

The project follows C++ best practices with separate header and implementation files:

- **Header Files (.h)**: Class declarations, public interfaces, documentation
- **Implementation Files (.cpp)**: Method implementations, private logic
- **Modular Design**: Each major component is in its own file pair
- **Clean Dependencies**: Minimal coupling between components

### Adding New Features

1. **Menu Items**: Extend `MenuItem` class or modify `initializeSampleMenu()`
2. **Payment Methods**: Add new methods to `PaymentProcessor::processPayment()`
3. **Kitchen Features**: Override virtual methods in `KitchenInterface`
4. **UI Components**: Extend `RestaurantPOSApp` with new panels or dialogs

### Extending the Three-Legged Foundation

Each leg provides extension points for additional functionality:

**Order Management Extensions:**
```cpp
class CustomOrderManager : public OrderManager {
    void onOrderCreated(std::shared_ptr<Order> order) override {
        // Custom order creation logic
        // Database persistence, analytics, notifications
    }
};
```

**Payment Processing Extensions:**
```cpp
class CustomPaymentProcessor : public PaymentProcessor {
    PaymentResult processCardPayment(std::shared_ptr<Order> order, 
                                   PaymentMethod method, 
                                   double amount, double tip) override {
        // Real payment gateway integration
        // Fraud detection, loyalty programs
    }
};
```

**Kitchen Interface Extensions:**
```cpp
class CustomKitchenInterface : public KitchenInterface {
    void onOrderSentToKitchen(std::shared_ptr<Order> order, 
                             const KitchenTicket& ticket) override {
        // WebSocket broadcasting, equipment integration
        // Inventory updates, timing optimization
    }
};
```

## Troubleshooting

### Build Issues
- Ensure Wt development packages are installed
- Check C++20 compiler support
- Verify pkg-config can find Wt libraries

### Runtime Issues
- Check port availability (default 8081)
- Ensure sufficient permissions for HTTP binding
- Verify docroot directory exists and is readable

### Common Errors
```bash
# Port already in use
./restaurant_pos --http-port 8081

# Permission denied
sudo ./restaurant_pos --http-port 80

# Missing docroot
mkdir www && ./restaurant_pos --docroot www
```

## License

This project is provided as a foundation for restaurant POS development. Modify and extend as needed for your specific requirements.

## Future Enhancements

- Database persistence (PostgreSQL, MySQL)
- Real payment gateway integration
- Advanced reporting dashboard
- Mobile app integration
- Multi-location support
- Inventory management
- Staff scheduling
- Customer loyalty programs