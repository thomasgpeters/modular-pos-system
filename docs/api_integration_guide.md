# RestaurantPOS API Integration Setup Guide

## Overview

Your ActiveOrdersDisplay has been enhanced to fetch orders from the API endpoint at `http://localhost:5656/api/Order/` with full JSON:API compliance. The system now supports both API-backed data (when available) and local data (as fallback).

## Key Changes Made

### 1. Enhanced ActiveOrdersDisplay

- **API Integration**: Now fetches orders from `/Order` endpoint with proper includes and field selection
- **Async Loading**: Shows loading state while fetching from API
- **Fallback Support**: Automatically falls back to local data if API is unavailable
- **Real-time Updates**: Refreshes from API when order events occur

### 2. Enhanced API Methods

Added missing async methods to `EnhancedPOSService`:
- `getActiveOrdersAsync()` - Fetches active orders from API
- `updateOrderStatusAsync()` - Updates order status via API
- `cancelOrderAsync()` - Cancels orders via API
- `getOrderByIdAsync()` - Retrieves specific orders
- And more...

## Setup Instructions

### 1. Update Your Configuration

Ensure your `ConfigurationManager` has API settings enabled:

```cpp
// In your main application or configuration file
configManager->setValue("api.enabled", true);
configManager->setValue("api.base_url", "http://localhost:5656/api");
configManager->setValue("api.timeout", 30);
configManager->setValue("api.debug_mode", true); // For testing
```

### 2. Use Enhanced Service

Make sure your `RestaurantPOSApp` uses `EnhancedPOSService`:

```cpp
// In RestaurantPOSApp initialization
auto enhancedService = APIServiceFactory::createEnhancedPOSService(
    eventManager_, configManager_
);

if (enhancedService && enhancedService->initialize()) {
    posService_ = std::static_pointer_cast<POSService>(enhancedService);
    std::cout << "Using API-backed POS service" << std::endl;
} else {
    posService_ = APIServiceFactory::createLocalPOSService(eventManager_);
    std::cout << "Falling back to local POS service" << std::endl;
}
```

### 3. Update OrderRepository (if needed)

Ensure your `OrderRepository::fromJson()` method correctly parses the API response:

```cpp
std::unique_ptr<Order> OrderRepository::fromJson(const Wt::Json::Value& json) {
    if (json.type() != Wt::Json::Type::Object) {
        return nullptr;
    }
    
    auto jsonObj = static_cast<const Wt::Json::Object&>(json);
    
    // Parse JSON:API format
    std::string orderId = safeGetString(jsonObj, "id");
    auto attributesValue = safeGetValue(jsonObj, "attributes");
    
    if (attributesValue.type() != Wt::Json::Type::Object) {
        return nullptr;
    }
    
    auto attrs = static_cast<const Wt::Json::Object&>(attributesValue);
    
    // Extract order data
    int orderIdInt = std::stoi(orderId);
    std::string tableIdentifier = safeGetString(attrs, "table_identifier");
    
    auto order = std::make_unique<Order>(orderIdInt, tableIdentifier);
    
    // Set other properties
    std::string statusStr = safeGetString(attrs, "status");
    // Convert status string to enum...
    
    return order;
}
```

## Expected API Response Format

The system expects JSON:API format from your ALS endpoint:

```json
{
  "data": [
    {
      "type": "Order",
      "id": "1000",
      "attributes": {
        "order_id": 1000,
        "table_identifier": "table 1", 
        "status": "PENDING",
        "subtotal": 20.0,
        "tax_amount": 1.2,
        "total_amount": 24.2,
        "created_at": "2025-07-17T10:30:00Z",
        "updated_at": "2025-07-17T10:30:00Z"
      },
      "relationships": {
        "OrderItemList": {
          "data": [{"id": "item1", "type": "OrderItem"}]
        }
      }
    }
  ],
  "included": [...],
  "meta": {
    "count": 1,
    "total": 1
  }
}
```

## Testing the Integration

### 1. Start Your ALS Server
```bash
# Make sure your ALS is running on port 5656
curl http://localhost:5656/api/Order/
```

### 2. Enable Debug Mode
Set `api.debug_mode = true` to see API calls in console.

### 3. Monitor Console Output
Look for these messages:
- `"Loading orders from API..."`
- `"Loaded X orders from API"`
- `"Enhanced service not available, using local data"` (if API fails)

### 4. Test Order Operations
- Create orders and verify they appear in ActiveOrdersDisplay
- Cancel orders and verify real-time updates
- Check that the display shows loading states

## Troubleshooting

### API Not Available
If you see "Enhanced service not available, using local data":
1. Check ALS server is running on localhost:5656
2. Verify `api.enabled = true` in configuration
3. Check network connectivity

### JSON Parsing Errors
If orders don't display correctly:
1. Verify API response format matches JSON:API spec
2. Check `OrderRepository::fromJson()` implementation
3. Enable debug mode to see raw API responses

### Performance Issues
If loading is slow:
1. Increase `api.timeout` setting
2. Optimize API query with field selection
3. Consider pagination for large datasets

## Next Steps

1. **Test with Real Data**: Connect to your running ALS instance
2. **Implement Order Creation**: Enhance order creation to use API
3. **Add Menu Integration**: Connect MenuDisplay to API
4. **Real-time Updates**: Consider WebSocket integration for live updates

The system is now ready to work with your ALS API endpoint and will provide a seamless experience with automatic fallback to local data when needed.