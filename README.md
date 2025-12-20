# Restaurant POS System - Modular Architecture v3.0.0

A modern, extensible Point of Sale system built with C++ and the Wt web framework. This system demonstrates enterprise-level software architecture with clean separation of concerns, event-driven communication, and a responsive web UI.

## Recent Updates (v3.0.0)

### UI Layout Improvements
- **Restructured Header/Footer**: Full-width CommonHeader and CommonFooter spanning the entire page
- **Improved Panel Layout**: Clean 30/70 split between Active Orders panel and Order Entry panel
- **Active Orders Widget**: Blue sub-header with refresh button integrated into the left panel
- **Dark Footer**: Professional dark theme footer (#1a1a2e) with status indicators
- **Removed Duplicate Elements**: Eliminated redundant header/footer structures for cleaner DOM

### Layout Structure
```
┌─────────────────────────────────────────────────────────────┐
│  CommonHeader (dark blue #2c3e50)                           │
│  [Restaurant POS] [POS|Kitchen] [Theme] [Operator] [Time]   │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐  ┌──────────────────────────────────────┐ │
│  │ Active Orders│  │                                      │ │
│  │ (blue header)│  │         Order Entry Panel            │ │
│  │ ─────────────│  │                                      │ │
│  │              │  │   - Select Table/Location            │ │
│  │ Orders List  │  │   - Start New Order                  │ │
│  │ (scrollable) │  │   - Menu Categories                  │ │
│  │              │  │   - Current Order Items              │ │
│  │   30%        │  │              70%                     │ │
│  └──────────────┘  └──────────────────────────────────────┘ │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  CommonFooter (dark #1a1a2e)                                │
│  [Active Orders: 0] [Kitchen Queue: 0] [System: Online] [v3]│
└─────────────────────────────────────────────────────────────┘
```

## Project Structure

```
modular-pos-system/
├── CMakeLists.txt                    # CMake build configuration
├── MakeFile                          # Alternative build system
├── README.md                         # This documentation file
├── wt_config.xml                     # Wt server configuration
├── pos_config.xml                    # POS system configuration
│
├── src/                              # Source implementation files
│   ├── main.cpp                      # Application entry point
│   ├── MenuItem.cpp                  # Menu item management
│   ├── Order.cpp                     # Order and OrderItem classes
│   ├── OrderManager.cpp              # Order lifecycle management (LEG 1)
│   ├── PaymentProcessor.cpp          # Payment processing (LEG 2)
│   ├── KitchenInterface.cpp          # Kitchen communication (LEG 3)
│   ├── Employee.cpp                  # Employee management
│   │
│   ├── core/                         # Core application
│   │   ├── RestaurantPOSApp.cpp      # Main Wt application
│   │   └── ConfigurationManager.cpp  # Configuration management
│   │
│   ├── services/                     # Business services
│   │   ├── POSService.cpp            # Central POS coordinator
│   │   ├── EnhancedPOSService.cpp    # Extended POS with API
│   │   ├── ThemeService.cpp          # Theme management
│   │   └── NotificationService.cpp   # User notifications
│   │
│   ├── events/                       # Event system
│   │   ├── EventManager.cpp          # Pub/sub event manager
│   │   └── POSEvents.cpp             # Event definitions (40+ types)
│   │
│   ├── ui/                           # User interface
│   │   ├── components/               # UI widgets
│   │   │   ├── MenuDisplay.cpp       # Menu item display
│   │   │   ├── OrderEntryPanel.cpp   # Order creation
│   │   │   ├── CurrentOrderDisplay.cpp
│   │   │   ├── ActiveOrdersDisplay.cpp
│   │   │   ├── KitchenStatusDisplay.cpp
│   │   │   ├── CommonHeader.cpp      # App header (full-width)
│   │   │   ├── CommonFooter.cpp      # App footer (full-width, dark)
│   │   │   ├── ModeSelector.cpp      # POS/Kitchen toggle
│   │   │   └── ThemeSelector.cpp     # Theme picker
│   │   │
│   │   ├── containers/               # Mode containers
│   │   │   ├── POSModeContainer.cpp  # POS interface (left/right panels)
│   │   │   └── KitchenModeContainer.cpp
│   │   │
│   │   ├── dialogs/                  # Modal dialogs
│   │   │   ├── PaymentDialog.cpp
│   │   │   ├── ThemeSelectionDialog.cpp
│   │   │   └── CategoryPopover.cpp
│   │   │
│   │   └── factories/
│   │       └── UIComponentFactory.cpp
│   │
│   ├── api/                          # API integration
│   │   ├── APIClient.cpp
│   │   └── APIConfiguration.cpp
│   │
│   └── utils/                        # Utilities
│       ├── Logging.cpp               # Logging framework
│       ├── CSSLoader.cpp
│       ├── FormatUtils.cpp
│       └── UIHelpers.cpp
│
├── include/                          # Header files (mirrors src/)
│
├── assets/                           # Static resources
│   └── css/                          # Stylesheets (39 files)
│       ├── base.css                  # Base styles
│       ├── main.css                  # Main layout
│       ├── components/               # Component styles
│       └── themes/                   # Theme variations
│
├── config/                           # Configuration templates
│   └── wt_config.xml.in
│
├── docs/                             # Documentation (51 files)
│
└── test/                             # Unit tests
    └── test_pos.cpp
```

## Architecture: Three-Legged Foundation

### 1. Order Management (`OrderManager`)
- **Purpose**: Handles order creation, modification, and lifecycle tracking
- **Features**:
  - Multi-table order management with string identifiers
  - Order status tracking (PENDING → SENT_TO_KITCHEN → PREPARING → READY → SERVED)
  - Item customizations and special instructions
  - Order history and JSON serialization
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
  - Kitchen ticket management
  - Status tracking (ORDER_RECEIVED → PREP_STARTED → READY_FOR_PICKUP → SERVED)
  - Queue management and wait time estimation
  - Kitchen busy state detection
- **Extension Points**: WebSocket communication, kitchen equipment integration, inventory tracking

## Key Features

- **Event-Driven Architecture**: Loose coupling via publish-subscribe pattern (40+ event types)
- **Dual Operating Modes**: POS Mode and Kitchen Display Mode
- **Theme System**: Multiple themes (Light, Dark, Warm, Cool, Restaurant)
- **Responsive Web UI**: Bootstrap 5 based design with Wt framework
- **Real-time Updates**: Automatic refresh of order status
- **Comprehensive Logging**: File rotation, multiple log levels
- **API Ready**: RESTful API integration layer
- **Clean Layout**: Full-width header/footer with organized panel structure

## UI Components

### CommonHeader
- Single-row dark header spanning full page width
- Restaurant POS branding
- POS/Kitchen mode selector
- Theme dropdown with toggle button
- Operator info and real-time clock

### POSModeContainer
- **Left Panel (30%)**: Active Orders widget with blue sub-header
- **Right Panel (70%)**: Order Entry panel or Current Order display
- Automatic mode switching between Order Entry and Order Edit

### CommonFooter
- Dark footer (#1a1a2e) spanning full page width
- Active orders count
- Kitchen queue count
- System status indicator (Online/Busy/Moderate/Error)
- Version information

## Prerequisites

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libwt-dev libwthttp-dev
```

### Fedora/RHEL/CentOS
```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake pkg-config wt-devel
```

### macOS (with Homebrew)
```bash
brew install cmake pkg-config wt
```

## Building

### Using CMake (Recommended)
```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./RestaurantPOSSystem --http-listen 0.0.0.0:8080 --docroot ../assets
```

### Using the Build Script
```bash
./build_and_run.sh
```

### Using Makefile
```bash
# Build release version
make

# Build debug version
make debug

# Run
make run
```

## Running the Application

1. **Start the server:**
   ```bash
   ./RestaurantPOSSystem --http-listen 0.0.0.0:8080 --docroot ../assets
   ```

2. **Open your browser:**
   ```
   http://localhost:8080
   ```

3. **Interface Overview:**
   - **Header**: Mode toggle (POS/Kitchen), theme selector, operator info
   - **Left Panel**: Active orders list with blue sub-header
   - **Right Panel**: Order entry or current order editing
   - **Footer**: System status and order counts

## Usage

### Creating an Order
1. Enter table identifier (e.g., "Table 5", "Delivery-123")
2. Click "Start New Order"
3. Browse menu by category
4. Click "+ Add" to add items to order
5. Adjust quantities as needed

### Sending to Kitchen
1. Review order items and totals
2. Click "Send to Kitchen"
3. Order appears in Active Orders list
4. Track status as kitchen processes order

### Processing Payment
1. Select order from Active Orders
2. Click "Process Payment"
3. Choose payment method
4. Complete transaction

### Kitchen Mode
1. Click "Kitchen" button in header
2. View incoming orders
3. Update order status as items are prepared
4. Mark orders ready for pickup

## Configuration

### Command Line Options
```bash
./RestaurantPOSSystem --help

Common options:
  --http-listen 0.0.0.0:8080  # Address and port
  --docroot ../assets         # Document root for assets
  --config wt_config.xml      # Configuration file
```

### wt_config.xml
```xml
<server>
    <application-settings location="*">
        <progressive-bootstrap>false</progressive-bootstrap>
        <theme>bootstrap5</theme>
    </application-settings>
</server>
```

## Design Patterns

| Pattern | Implementation | Purpose |
|---------|---------------|---------|
| **Publish-Subscribe** | EventManager | Loose coupling between components |
| **Factory** | UIComponentFactory | Consistent widget creation |
| **Facade** | POSService | Central business logic coordinator |
| **Repository** | API Repositories | Data access abstraction |
| **Singleton** | Logger | Centralized logging |

## Event System

The system uses 40+ event types for component communication:

- **Order Events**: ORDER_CREATED, ORDER_MODIFIED, ORDER_COMPLETED, CURRENT_ORDER_CHANGED
- **Kitchen Events**: ORDER_SENT_TO_KITCHEN, KITCHEN_STATUS_CHANGED
- **Payment Events**: PAYMENT_INITIATED, PAYMENT_COMPLETED
- **UI Events**: THEME_CHANGED, MODE_CHANGED, NOTIFICATION_REQUESTED, REFRESH_ACTIVE_ORDERS

## Extending the System

### Custom Order Processing
```cpp
class CustomOrderManager : public OrderManager {
    void onOrderCreated(std::shared_ptr<Order> order) override {
        // Database persistence, analytics, notifications
    }
};
```

### Payment Gateway Integration
```cpp
class CustomPaymentProcessor : public PaymentProcessor {
    PaymentResult processCardPayment(...) override {
        // Real payment gateway API calls
    }
};
```

### Kitchen Equipment Integration
```cpp
class CustomKitchenInterface : public KitchenInterface {
    void onOrderSentToKitchen(...) override {
        // WebSocket to kitchen displays
        // Printer integration
    }
};
```

## Troubleshooting

### Build Issues
- Ensure Wt development packages are installed (libwt-dev, libwthttp-dev)
- Verify C++17 compiler support
- Check pkg-config can find Wt libraries: `pkg-config --exists wt && echo "Found"`

### Runtime Issues
- Check port availability: `lsof -i :8080`
- Ensure docroot directory contains assets/css/
- Verify wt_config.xml is accessible

### Common Errors
```bash
# Config file not found
./RestaurantPOSSystem --config ./wt_config.xml

# CSS not loading - check docroot
./RestaurantPOSSystem --docroot /path/to/assets

# Port in use
./RestaurantPOSSystem --http-listen 0.0.0.0:8081

# Address/port format
# Use --http-listen instead of separate --address and --port
./RestaurantPOSSystem --http-listen 0.0.0.0:8080
```

## Project Statistics

| Metric | Count |
|--------|-------|
| C++ Source Files | 82+ |
| Lines of Code | ~10,000+ |
| CSS Stylesheets | 39 |
| Documentation Files | 51 |
| Event Types | 40+ |

## Version History

### v3.0.0 (Current)
- Restructured UI layout with proper header/footer hierarchy
- Fixed CSS selector issues (ID vs class)
- Removed duplicate header/footer elements
- Added Active Orders sub-header to left panel widget
- Improved CommonFooter with dark theme
- Fixed EventManager method calls (publish vs emit)

### v2.0.0
- Modular architecture implementation
- Event-driven communication system
- Theme service integration
- Kitchen mode support

### v1.0.0
- Initial POS functionality
- Basic order management
- Menu display

## Future Enhancements

- Database persistence (PostgreSQL, MySQL)
- Real payment gateway integration
- WebSocket for real-time updates
- Advanced reporting dashboard
- Mobile app integration
- Multi-location support
- Inventory management
- Staff authentication
- Customer loyalty programs

## License

This project is provided as a foundation for restaurant POS development. Modify and extend as needed for your specific requirements.

## Documentation

See the `docs/` directory for detailed documentation:
- `comprehensive_operating_manual.md` - Full operations guide
- `architecture_principals.md` - Design principles
- `logging_documentation.md` - Logging system guide
- `styling_integration_guide.md` - CSS and theming
- `api_integration_guide.md` - API usage

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Support

For issues and feature requests, please use the project's issue tracker.
