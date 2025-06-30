# Services Implementation Guide

## 🎯 **What I've Created**

I've implemented the complete service layer for your modular Restaurant POS system, including all four core services:

1. **NotificationService** - Centralized user notification system
2. **POSService** - Main business logic coordinator 
3. **ThemeService** - Dynamic theme management
4. **ConfigurationManager** - Centralized configuration handling

## 📁 **Files Created**

### **1. NotificationService**
- **Header**: `include/services/NotificationService.hpp`
- **Implementation**: `src/services/NotificationService.cpp`
- **Purpose**: Toast-style notifications, event-driven alerts, user feedback

### **2. POSService** 
- **Header**: `include/services/POSService.hpp` (was in attached docs)
- **Implementation**: `src/services/POSService.cpp`
- **Purpose**: Business logic coordinator between UI and core components

### **3. ThemeService**
- **Header**: `include/services/ThemeService.hpp` (was in attached docs)
- **Implementation**: `src/services/ThemeService.cpp`
- **Purpose**: Dynamic theme switching and management

### **4. ConfigurationManager**
- **Header**: `include/services/ConfigurationManager.hpp` (was in attached docs)
- **Implementation**: `src/services/ConfigurationManager.cpp`
- **Purpose**: Centralized configuration with file/environment support

## 🏗️ **Service Architecture Overview**

```
┌─────────────────────────────────────────┐
│              UI Layer                   │
│  (OrderEntryPanel, MenuDisplay, etc.)  │
└─────────────┬───────────────────────────┘
              │ Events & Service Calls
┌─────────────▼───────────────────────────┐
│           Service Layer                 │
│  ┌─────────────┐  ┌─────────────────┐   │
│  │ POSService  │  │ NotificationSvc │   │
│  └─────────────┘  └─────────────────┘   │
│  ┌─────────────┐  ┌─────────────────┐   │
│  │ ThemeService│  │ ConfigManager   │   │
│  └─────────────┘  └─────────────────┘   │
└─────────────┬───────────────────────────┘
              │ Coordinates & Orchestrates
┌─────────────▼───────────────────────────┐
│          Business Logic                 │
│  OrderManager • PaymentProcessor       │
│  KitchenInterface • MenuItem            │
└─────────────────────────────────────────┘
```

## 🔧 **Implementation Steps**

### **Step 1: Create Directory Structure**
```bash
mkdir -p include/services src/services
```

### **Step 2: Copy Header Files**
Copy all the headers from the artifacts to your include directory:
- `NotificationService.hpp` → `include/services/`
- Headers for other services are already provided in the docs

### **Step 3: Copy Implementation Files**
Copy all the implementation files from the artifacts to your src directory:
- `NotificationService.cpp` → `src/services/`
- `POSService.cpp` → `src/services/`
- `ThemeService.cpp` → `src/services/`
- `ConfigurationManager.cpp` → `src/services/`

### **Step 4: Update CMakeLists.txt**
Add the service library to your build system:

```cmake
# Add services library
add_library(pos_services
    src/services/NotificationService.cpp
    src/services/POSService.cpp
    src/services/ThemeService.cpp
    src/services/ConfigurationManager.cpp
)

target_link_libraries(pos_services
    pos_events
    pos_business_logic
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# Link services to main application
target_link_libraries(restaurant_pos
    pos_services
    # ... other libraries
)
```

## ⚡ **Key Features Implemented**

### **NotificationService Features**
- **Toast Notifications**: Info, Success, Warning, Error types
- **Auto-Dismissal**: Configurable timeout for each notification
- **Action Buttons**: Optional action buttons with callbacks
- **Event Integration**: Automatic notifications for business events
- **Queue Management**: Handles notification overflow gracefully
- **Position Control**: Configurable notification position (top-right, etc.)

### **POSService Features**
- **Business Coordination**: Bridges UI and business logic
- **Order Management**: Complete order lifecycle management
- **Menu Operations**: Add, update, categorize menu items
- **Kitchen Integration**: Send orders and track status
- **Payment Processing**: Handle payments and split bills
- **Event Publishing**: Publishes business events for UI updates
- **Statistics**: Business metrics and reporting data

### **ThemeService Features**
- **Dynamic Switching**: Change themes without restart
- **Theme Validation**: Ensures theme integrity
- **User Preferences**: Persistent theme selection
- **Multiple Themes**: Bootstrap, Dark, Light, Professional, Restaurant
- **Event Notifications**: Publishes theme change events
- **CSS Management**: Handles CSS file loading/unloading

### **ConfigurationManager Features**
- **Multiple Sources**: File, environment variables, defaults
- **Type Safety**: Generic getValue/setValue with type conversion
- **Sectioned Config**: Organized by restaurant, server, kitchen, etc.
- **Runtime Changes**: Modify configuration without restart
- **Persistence**: Save configuration back to files
- **Feature Flags**: Enable/disable features dynamically

## 🎯 **Usage Examples**

### **Using NotificationService**
```cpp
// In your component constructor
notificationService_->showSuccess("Order created successfully!");
notificationService_->showError("Payment failed", 0); // Permanent error
notificationService_->showNotificationWithAction(
    "Kitchen is busy", 
    NotificationService::WARNING,
    "View Queue",
    [this]() { showKitchenQueue(); }
);
```

### **Using POSService**
```cpp
// Create and manage orders
auto order = posService_->createOrder(tableNumber);
posService_->addItemToCurrentOrder(menuItem, quantity);
posService_->sendCurrentOrderToKitchen();

// Process payments
auto result = posService_->processCurrentOrderPayment(
    PaymentProcessor::CREDIT_CARD, total, tip);
```

### **Using ThemeService**
```cpp
// Switch themes
themeService_->applyTheme("dark");

// Get available themes for UI
auto themes = themeService_->getEnabledThemes();
for (const auto& theme : themes) {
    // Create theme selection UI
}
```

### **Using ConfigurationManager**
```cpp
// Access configuration
std::string restaurantName = configManager_->getRestaurantName();
int refreshRate = configManager_->getKitchenRefreshRate();
bool featureEnabled = configManager_->isFeatureEnabled("inventory");

// Modify configuration
configManager_->setUIUpdateInterval(3);
configManager_->setFeatureEnabled("loyalty.program", true);
```

## 🔗 **Event System Integration**

All services are fully integrated with the event system:

### **Events Published**
- **NotificationService**: Handles `NOTIFICATION_REQUESTED` events
- **POSService**: Publishes `ORDER_CREATED`, `ORDER_MODIFIED`, `PAYMENT_COMPLETED`
- **ThemeService**: Publishes `THEME_CHANGED` events
- **ConfigurationManager**: Publishes `CONFIGURATION_CHANGED` events

### **Event Subscriptions**
Services automatically subscribe to relevant events:
- Business events trigger notifications
- Configuration changes reload services
- Theme changes update UI components

## 🚀 **Next Steps**

1. **Copy Files**: Copy all header and implementation files to your project
2. **Update Build**: Modify CMakeLists.txt to include services
3. **Test Compilation**: Build and resolve any dependency issues
4. **Integration**: Update your main application to use services
5. **UI Components**: Create the UI components that use these services

## 📋 **Dependencies Required**

### **External Libraries**
- **Wt (Web Toolkit)**: For web application framework
- **Standard C++**: C++17 or later for std::any support

### **Internal Dependencies**
- **EventManager**: Event system (provided in previous docs)
- **POSEvents**: Event type definitions (provided in previous docs)
- **Business Logic**: OrderManager, PaymentProcessor, KitchenInterface
- **Data Models**: MenuItem, Order classes

## 🔧 **Configuration Files**

The ConfigurationManager supports configuration files in this format:

```ini
# Restaurant POS Configuration
restaurant.name=My Restaurant
restaurant.address=123 Main St
restaurant.tax.rate=0.0825

server.port=8081
server.address=0.0.0.0

ui.default.theme=bootstrap
ui.update.interval=5

features.inventory=false
features.reporting=true

payment.enabled.methods=cash,credit_card,debit_card
payment.tip.suggestions=15,18,20,25
```

## 🎨 **Theme Files**

Create theme CSS files in the `themes/` directory:
- `themes/bootstrap.css` - Default Bootstrap theme
- `themes/dark.css` - Dark mode theme
- `themes/light.css` - Light mode theme
- `themes/professional.css` - Professional corporate theme
- `themes/restaurant.css` - Restaurant-specific warm theme

## 🚦 **Error Handling**

All services include comprehensive error handling:
- **Graceful Degradation**: Services continue operating with reduced functionality
- **Error Logging**: All errors are logged to console
- **Event Publishing**: Critical errors publish system error events
- **Validation**: Input validation prevents invalid operations

## 📊 **Performance Considerations**

- **Lazy Loading**: Themes and configurations loaded on demand
- **Memory Management**: Smart pointers prevent memory leaks
- **Event Efficiency**: Event system uses efficient callback mechanisms
- **Resource Cleanup**: Proper cleanup in destructors

## 🔍 **Debugging Tips**

1. **Enable Logging**: All services log their operations to console
2. **Check Events**: Use event system debugging to trace communication
3. **Validate Config**: Ensure configuration files are properly formatted
4. **Theme Paths**: Verify theme CSS files exist and are accessible
5. **Dependencies**: Ensure all required libraries are linked

This service layer provides a solid foundation for your modular Restaurant POS system, enabling clean separation of concerns and enterprise-level scalability! 🎉