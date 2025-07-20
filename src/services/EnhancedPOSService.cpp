//============================================================================
// src/services/EnhancedPOSService.cpp - Implementation with Logging Integration - CORRECTED
//============================================================================

#include "../../include/services/EnhancedPOSService.hpp"

#include <iostream>
#include <algorithm>

EnhancedPOSService::EnhancedPOSService(std::shared_ptr<EventManager> eventManager,
                                       const ServiceConfig& config)
    : POSService(eventManager),  // Call base class constructor (initializes logger)
      config_(config), initialized_(false), menuCacheValid_(false) {
    
    getLogger().info("[EnhancedPOSService] Initializing with API integration...");
    LOG_CONFIG_STRING(getLogger(), info, "API Base URL", config_.apiBaseUrl);
    LOG_CONFIG_BOOL(getLogger(), info, "API Debug Mode", config_.debugMode);
    LOG_CONFIG_BOOL(getLogger(), info, "Caching Enabled", config_.enableCaching);
    LOG_CONFIG_STRING(getLogger(), info, "API Timeout", std::to_string(config_.apiTimeout) + "s");
}

bool EnhancedPOSService::initialize() {
    getLogger().info("[EnhancedPOSService] Starting service initialization...");
    
    try {
        initializeAPIComponents();
        initializeCaches();
        
        initialized_ = true;
        LOG_OPERATION_STATUS(getLogger(), "EnhancedPOSService initialization", true);
        
        return true;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "initialize", e.what());
        return false;
    }
}

void EnhancedPOSService::initializeAPIComponents() {
    getLogger().info("[EnhancedPOSService] Initializing API components...");
    
    try {
        // Create API client
        apiClient_ = std::make_shared<APIClient>(config_.apiBaseUrl);
        apiClient_->setTimeout(config_.apiTimeout);
        apiClient_->setDebugMode(config_.debugMode);
        
        if (!config_.authToken.empty()) {
            apiClient_->setAuthToken(config_.authToken);
            getLogger().info("[EnhancedPOSService] Auth token configured");
        }
        
        // Create repositories
        orderRepository_ = std::make_unique<OrderRepository>(apiClient_);
        menuItemRepository_ = std::make_unique<MenuItemRepository>(apiClient_);
        employeeRepository_ = std::make_unique<EmployeeRepository>(apiClient_);
        
        LOG_OPERATION_STATUS(getLogger(), "API components initialization", true);
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "initializeAPIComponents", e.what());
        throw;
    }
}

void EnhancedPOSService::initializeCaches() {
    getLogger().info("[EnhancedPOSService] Initializing caches...");
    
    menuItemsCache_.clear();
    menuItemByIdCache_.clear();
    menuCacheValid_ = false;
    
    LOG_OPERATION_STATUS(getLogger(), "Cache initialization", true);
}

// =================================================================
// Enhanced methods (do not override - enhance base class functionality)
// =================================================================

std::shared_ptr<Order> EnhancedPOSService::createOrder(const std::string& tableIdentifier) {
    getLogger().info("[EnhancedPOSService] Creating order with API persistence: " + tableIdentifier);
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "createOrder", 
                           "Service not initialized, falling back to base class");
        // Fall back to base class implementation
        return POSService::createOrder(tableIdentifier);
    }
    
    try {
        // Create order locally first
        static int nextOrderId = 1000; // In real app, get from API
        auto order = std::make_shared<Order>(nextOrderId++, tableIdentifier);
        
        // For synchronous version, we'll just return the local order
        // In async version, we save to API
        POSEvents::EventLogger::logOrderEvent(
            POSEvents::ORDER_CREATED,
            order,
            "Enhanced order created with API integration"
        );
        
        LOG_OPERATION_STATUS(getLogger(), "Enhanced order creation", true);
        return order;
        
    } catch (const std::exception& e) {
        handleAPIError("createOrder", e.what());
        // Fall back to base class on error
        return POSService::createOrder(tableIdentifier);
    }
}

std::vector<std::shared_ptr<Order>> EnhancedPOSService::getActiveOrders() {
    getLogger().debug("[EnhancedPOSService] Getting active orders (enhanced)");
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getActiveOrders", 
                           "Service not initialized, falling back to base class");
        return POSService::getActiveOrders();
    }
    
    // For synchronous version, suggest using async and fall back to base class
    getLogger().info("[EnhancedPOSService] Use getActiveOrdersAsync() for API data, falling back to base class");
    return POSService::getActiveOrders();
}

std::shared_ptr<Order> EnhancedPOSService::getCurrentOrder() const {
    // Use the enhanced local tracking first
    if (currentOrder_) {
        getLogger().debug("[EnhancedPOSService] Returning enhanced current order: " + 
                         std::to_string(currentOrder_->getOrderId()));
        return currentOrder_;
    }
    
    getLogger().debug("[EnhancedPOSService] No enhanced current order, checking base class");
    // Fall back to base class if needed
    return POSService::getCurrentOrder();
}

void EnhancedPOSService::setCurrentOrder(std::shared_ptr<Order> order) {
    auto previousOrder = getCurrentOrder();
    
    // Update our local tracking
    currentOrder_ = order;
    
    if (order) {
        getLogger().info("[EnhancedPOSService] Set enhanced current order: " + std::to_string(order->getOrderId()));
    } else {
        getLogger().info("[EnhancedPOSService] Cleared enhanced current order");
    }
    
    // Also update the base class
    POSService::setCurrentOrder(order);
    
    // Publish enhanced event if event manager is available
    auto eventManager = getEventManager();
    if (eventManager) {
        // FIXED: Create JSON event manually
        Wt::Json::Object eventData;
        
        if (order) {
            eventData["orderId"] = Wt::Json::Value(order->getOrderId());
            eventData["tableIdentifier"] = Wt::Json::Value(order->getTableIdentifier());
            eventData["hasCurrentOrder"] = Wt::Json::Value(true);
        } else {
            eventData["orderId"] = Wt::Json::Value(-1);
            eventData["tableIdentifier"] = Wt::Json::Value("");
            eventData["hasCurrentOrder"] = Wt::Json::Value(false);
        }
        
        if (previousOrder) {
            eventData["previousOrderId"] = Wt::Json::Value(previousOrder->getOrderId());
        }
        
        eventData["reason"] = Wt::Json::Value(order ? "enhanced_order_set" : "enhanced_order_cleared");
        eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        eventData["message"] = Wt::Json::Value("Current order changed: " + std::string(order ? "enhanced_order_set" : "enhanced_order_cleared"));
        
        publishEvent(POSEvents::CURRENT_ORDER_CHANGED, eventData);
    }
}

std::vector<std::shared_ptr<MenuItem>> EnhancedPOSService::getMenuItems() {
    getLogger().debug("[EnhancedPOSService] Getting menu items (enhanced with caching)");
    
    // Return cached items if available
    if (config_.enableCaching && menuCacheValid_ && !isMenuCacheExpired()) {
        LOG_KEY_VALUE(getLogger(), debug, "Menu items from cache", menuItemsCache_.size());
        return menuItemsCache_;
    }
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getMenuItems", 
                           "Service not initialized, falling back to base class");
        return POSService::getMenuItems();
    }
    
    // For sync version, fall back to base class if cache is invalid
    getLogger().info("[EnhancedPOSService] Use getMenuItemsAsync() to refresh from API, falling back to base class");
    return POSService::getMenuItems();
}

std::shared_ptr<MenuItem> EnhancedPOSService::getMenuItemById(int itemId) {
    getLogger().debug("[EnhancedPOSService] Looking up menu item with caching: " + std::to_string(itemId));
    
    auto it = menuItemByIdCache_.find(itemId);
    if (it != menuItemByIdCache_.end()) {
        getLogger().debug("[EnhancedPOSService] Found menu item in cache: " + std::to_string(itemId));
        return it->second;
    }
    
    getLogger().debug("[EnhancedPOSService] Menu item not in cache, checking base class: " + std::to_string(itemId));
    // Fall back to base class implementation
    return POSService::getMenuItemById(itemId);
}

// =================================================================
// Enhanced Async Methods (API-Backed)
// =================================================================

void EnhancedPOSService::createOrderAsync(const std::string& tableIdentifier,
                                         std::function<void(std::shared_ptr<Order>, bool)> callback) {
    
    getLogger().info("[EnhancedPOSService] Creating order asynchronously: " + tableIdentifier);
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "createOrderAsync", "Service not initialized");
        if (callback) callback(nullptr, false);
        return;
    }
    
    try {
        // Create order locally first
        static int nextOrderId = 1000; // In real app, get from API
        auto order = std::make_shared<Order>(nextOrderId++, tableIdentifier);
        
        // Save to API
        orderRepository_->create(*order, [this, order, callback](std::unique_ptr<Order> createdOrder, bool success) {
            if (success && createdOrder) {
                // Update local order with API response data
                // For now, we'll use the local order
                
                // Log the successful creation
                POSEvents::EventLogger::logOrderEvent(
                    POSEvents::ORDER_CREATED,
                    order,
                    "Order created via API"
                );
                
                // Publish event
                auto eventManager = getEventManager();
                if (eventManager) {
                    // FIXED: Create JSON event manually
                    Wt::Json::Object eventData;
                    eventData["orderId"] = Wt::Json::Value(order->getOrderId());
                    eventData["tableIdentifier"] = Wt::Json::Value(order->getTableIdentifier());
                    eventData["status"] = Wt::Json::Value(static_cast<int>(order->getStatus()));
                    eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                        std::chrono::system_clock::to_time_t(order->getTimestamp())));
                    eventData["message"] = Wt::Json::Value("Order created for " + order->getTableIdentifier());
                    
                    publishEvent(POSEvents::ORDER_CREATED, eventData);
                }
                
                LOG_OPERATION_STATUS(getLogger(), "Async order creation", true);
                if (callback) callback(order, true);
                
            } else {
                handleAPIError("createOrderAsync", "Failed to create order in API");
                if (callback) callback(nullptr, false);
            }
        });
        
    } catch (const std::exception& e) {
        handleAPIError("createOrderAsync", e.what());
        if (callback) callback(nullptr, false);
    }
}

void EnhancedPOSService::getActiveOrdersAsync(std::function<void(std::vector<std::shared_ptr<Order>>, bool)> callback) {
    getLogger().info("[EnhancedPOSService] Getting active orders asynchronously from API");
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getActiveOrdersAsync", "Service not initialized");
        if (callback) callback({}, false);
        return;
    }
    
    // Filter for active orders (not served or cancelled)
    std::map<std::string, std::string> params;
    params["filter[status]"] = "0,1,2,3"; // PENDING,SENT_TO_KITCHEN,PREPARING,READY
    
    orderRepository_->findAll(params, [this, callback](std::vector<Order> orders, bool success) {
        std::vector<std::shared_ptr<Order>> sharedOrders;
        
        if (success) {
            sharedOrders.reserve(orders.size());
            for (const auto& order : orders) {
                sharedOrders.push_back(std::make_shared<Order>(order));
            }
            
            LOG_KEY_VALUE(getLogger(), info, "Active orders retrieved from API", sharedOrders.size());
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getActiveOrdersAsync", "Failed to retrieve from API");
        }
        
        if (callback) callback(sharedOrders, success);
    });
}

// =================================================================
// Menu Management Implementation
// =================================================================

void EnhancedPOSService::getMenuItemsAsync(bool forceRefresh,
                                          std::function<void(std::vector<std::shared_ptr<MenuItem>>, bool)> callback) {
    
    getLogger().info("[EnhancedPOSService] Getting menu items asynchronously, force refresh: " + 
                    LoggingUtils::boolToString(forceRefresh));
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getMenuItemsAsync", "Service not initialized");
        if (callback) callback({}, false);
        return;
    }
    
    // Check cache first (if enabled and not forced refresh)
    if (config_.enableCaching && !forceRefresh && menuCacheValid_ && !isMenuCacheExpired()) {
        getLogger().info("[EnhancedPOSService] Returning cached menu items");
        LOG_KEY_VALUE(getLogger(), debug, "Cached items returned", menuItemsCache_.size());
        if (callback) callback(menuItemsCache_, true);
        return;
    }
    
    // Fetch from API
    getLogger().info("[EnhancedPOSService] Fetching menu items from API...");
    menuItemRepository_->findAll({}, [this, callback](std::vector<MenuItem> items, bool success) {
        std::vector<std::shared_ptr<MenuItem>> sharedItems;
        
        if (success) {
            sharedItems.reserve(items.size());
            for (const auto& item : items) {
                sharedItems.push_back(std::make_shared<MenuItem>(item));
            }
            
            // Update cache
            if (config_.enableCaching) {
                updateMenuCache(sharedItems);
            }
            
            LOG_KEY_VALUE(getLogger(), info, "Menu items loaded from API", sharedItems.size());
        } else {
            handleAPIError("getMenuItemsAsync", "Failed to fetch menu items");
        }
        
        if (callback) callback(sharedItems, success);
    });
}

void EnhancedPOSService::getMenuItemsByCategoryAsync(MenuItem::Category category,
                                                    std::function<void(std::vector<std::shared_ptr<MenuItem>>, bool)> callback) {
    
    getLogger().info("[EnhancedPOSService] Getting menu items by category asynchronously");
    
    // If cache is valid, filter locally
    if (config_.enableCaching && menuCacheValid_ && !isMenuCacheExpired()) {
        getLogger().debug("[EnhancedPOSService] Filtering menu items from cache by category");
        
        std::vector<std::shared_ptr<MenuItem>> categoryItems;
        
        for (const auto& item : menuItemsCache_) {
            if (item && item->getCategory() == category) {
                categoryItems.push_back(item);
            }
        }
        
        LOG_KEY_VALUE(getLogger(), debug, "Category items from cache", categoryItems.size());
        if (callback) callback(categoryItems, true);
        return;
    }
    
    // Otherwise fetch from API
    getLogger().info("[EnhancedPOSService] Fetching category items from API...");
    menuItemRepository_->findByCategory(category, [this, callback](std::vector<MenuItem> items, bool success) {
        std::vector<std::shared_ptr<MenuItem>> sharedItems;
        
        if (success) {
            sharedItems.reserve(items.size());
            for (const auto& item : items) {
                sharedItems.push_back(std::make_shared<MenuItem>(item));
            }
            
            LOG_KEY_VALUE(getLogger(), info, "Category items loaded from API", sharedItems.size());
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getMenuItemsByCategoryAsync", 
                               "Failed to fetch category items");
        }
        
        if (callback) callback(sharedItems, success);
    });
}

// =================================================================
// Current Order Management Implementation - FIXED LAMBDA CAPTURES
// =================================================================

void EnhancedPOSService::addItemToCurrentOrderAsync(const MenuItem& item, int quantity,
                                                   const std::string& instructions,
                                                   std::function<void(bool)> callback) {
    
    getLogger().info("[EnhancedPOSService] Adding item to current order asynchronously: " + 
                    std::to_string(quantity) + "x " + item.getName());
    
    if (!currentOrder_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "addItemToCurrentOrderAsync", 
                           "No current order to add item to");
        if (callback) callback(false);
        return;
    }
    
    if (!item.isAvailable()) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "addItemToCurrentOrderAsync", 
                           "Menu item not available: " + item.getName());
        if (callback) callback(false);
        return;
    }
    
    try {
        // Add item locally
        OrderItem orderItem(item, quantity);
        if (!instructions.empty()) {
            orderItem.setSpecialInstructions(instructions);
            getLogger().debug("[EnhancedPOSService] Special instructions: " + instructions);
        }
        
        currentOrder_->addItem(orderItem);
        
        // FIXED: Removed unused lambda captures
        saveCurrentOrderAsync([this, callback](bool success) {
            if (success) {
                // Publish event
                auto eventManager = getEventManager();
                if (eventManager) {
                    // FIXED: Create JSON event manually
                    Wt::Json::Object eventData;
                    eventData["orderId"] = Wt::Json::Value(currentOrder_->getOrderId());
                    eventData["tableIdentifier"] = Wt::Json::Value(currentOrder_->getTableIdentifier());
                    eventData["status"] = Wt::Json::Value(static_cast<int>(currentOrder_->getStatus()));
                    eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
                    eventData["message"] = Wt::Json::Value("Order " + std::to_string(currentOrder_->getOrderId()) + " modified");
                    
                    publishEvent(POSEvents::ORDER_ITEM_ADDED, eventData);
                }
                
                LOG_OPERATION_STATUS(getLogger(), "Add item to current order (async)", true);
            } else {
                LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "addItemToCurrentOrderAsync", 
                                   "Failed to save order to API after adding item");
            }
            
            if (callback) callback(success);
        });
        
    } catch (const std::exception& e) {
        handleAPIError("addItemToCurrentOrderAsync", e.what());
        if (callback) callback(false);
    }
}

void EnhancedPOSService::saveCurrentOrderAsync(std::function<void(bool)> callback) {
    getLogger().info("[EnhancedPOSService] Saving current order to API asynchronously");
    
    if (!currentOrder_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "saveCurrentOrderAsync", "No current order to save");
        if (callback) callback(false);
        return;
    }
    
    int orderId = currentOrder_->getOrderId();
    
    // Save to API
    orderRepository_->update(orderId, *currentOrder_, 
                           [this, orderId, callback](std::unique_ptr<Order> updatedOrder, bool success) {
        if (success) {
            LOG_OPERATION_STATUS(getLogger(), "Save current order to API", true);
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "saveCurrentOrderAsync", 
                               "Failed to save order " + std::to_string(orderId) + " to API");
        }
        
        if (callback) callback(success);
    });
}

// =================================================================
// Employee Management
// =================================================================

void EnhancedPOSService::getEmployeesAsync(std::function<void(std::vector<Employee>, bool)> callback) {
    getLogger().info("[EnhancedPOSService] Getting employees from API asynchronously");
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getEmployeesAsync", "Service not initialized");
        if (callback) callback({}, false);
        return;
    }
    
    employeeRepository_->findAll({}, [this, callback](std::vector<Employee> employees, bool success) {
        if (success) {
            LOG_KEY_VALUE(getLogger(), info, "Employees retrieved from API", employees.size());
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getEmployeesAsync", "Failed to retrieve employees");
        }
        
        if (callback) callback(employees, success);
    });
}

void EnhancedPOSService::getEmployeesByRoleAsync(const std::string& role,
                                                 std::function<void(std::vector<Employee>, bool)> callback) {
    getLogger().info("[EnhancedPOSService] Getting employees by role from API: " + role);
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getEmployeesByRoleAsync", "Service not initialized");
        if (callback) callback({}, false);
        return;
    }
    
    employeeRepository_->findByRole(role, [this, role, callback](std::vector<Employee> employees, bool success) {
        if (success) {
            LOG_KEY_VALUE(getLogger(), info, "Employees with role '" + role + "' found", employees.size());
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getEmployeesByRoleAsync", 
                               "Failed to retrieve employees with role: " + role);
        }
        
        if (callback) callback(employees, success);
    });
}

void EnhancedPOSService::getActiveEmployeesAsync(std::function<void(std::vector<Employee>, bool)> callback) {
    getLogger().info("[EnhancedPOSService] Getting active employees from API");
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getActiveEmployeesAsync", "Service not initialized");
        if (callback) callback({}, false);
        return;
    }
    
    employeeRepository_->findActive([this, callback](std::vector<Employee> employees, bool success) {
        if (success) {
            LOG_KEY_VALUE(getLogger(), info, "Active employees retrieved from API", employees.size());
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getActiveEmployeesAsync", 
                               "Failed to retrieve active employees");
        }
        
        if (callback) callback(employees, success);
    });
}

// =================================================================
// Configuration Methods
// =================================================================

void EnhancedPOSService::setAuthToken(const std::string& token) {
    getLogger().info("[EnhancedPOSService] Updating API authentication token");
    
    config_.authToken = token;
    if (apiClient_) {
        apiClient_->setAuthToken(token);
        LOG_OPERATION_STATUS(getLogger(), "Auth token update", true);
    } else {
        getLogger().warn("[EnhancedPOSService] API client not initialized, token stored for later use");
    }
}

void EnhancedPOSService::clearCaches() {
    getLogger().info("[EnhancedPOSService] Clearing all caches");
    
    size_t menuItemsCleared = menuItemsCache_.size();
    size_t menuByIdCleared = menuItemByIdCache_.size();
    
    menuItemsCache_.clear();
    menuItemByIdCache_.clear();
    menuCacheValid_ = false;
    
    LOG_KEY_VALUE(getLogger(), info, "Menu items cache cleared", menuItemsCleared);
    LOG_KEY_VALUE(getLogger(), info, "Menu by ID cache cleared", menuByIdCleared);
    LOG_OPERATION_STATUS(getLogger(), "Cache clearing", true);
}

// =================================================================
// Helper Methods Implementation - FIXED ERROR HANDLING
// =================================================================

void EnhancedPOSService::handleAPIError(const std::string& operation, const std::string& error) {
    LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", operation, "API Error: " + error);
    
    auto eventManager = getEventManager();
    if (eventManager) {
        // FIXED: Create JSON error event manually since createErrorEvent doesn't exist
        Wt::Json::Object errorData;
        errorData["errorMessage"] = Wt::Json::Value("API Error in " + operation + ": " + error);
        errorData["errorCode"] = Wt::Json::Value("API_ERROR");
        errorData["component"] = Wt::Json::Value("EnhancedPOSService");
        errorData["isCritical"] = Wt::Json::Value(true);
        errorData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        
        publishEvent(POSEvents::SYSTEM_ERROR, errorData);
    }
}

void EnhancedPOSService::publishEvent(const std::string& eventType, const Wt::Json::Object& eventData) {
    auto eventManager = getEventManager();
    if (eventManager) {
        getLogger().debug("[EnhancedPOSService] Publishing event: " + eventType);
        eventManager->publish(eventType, eventData, "EnhancedPOSService");
    }
}

bool EnhancedPOSService::isMenuCacheExpired() const {
    auto now = std::chrono::system_clock::now();
    auto cacheAge = std::chrono::duration_cast<std::chrono::minutes>(now - menuCacheTime_);
    bool expired = cacheAge.count() >= CACHE_TIMEOUT_MINUTES;
    
    if (expired) {
        getLogger().debug("[EnhancedPOSService] Menu cache expired (age: " + std::to_string(cacheAge.count()) + " minutes)");
    }
    
    return expired;
}

void EnhancedPOSService::updateMenuCache(const std::vector<std::shared_ptr<MenuItem>>& items) {
    getLogger().info("[EnhancedPOSService] Updating menu cache...");
    
    menuItemsCache_ = items;
    menuItemByIdCache_.clear();
    
    for (const auto& item : items) {
        if (item) {
            menuItemByIdCache_[item->getId()] = item;
        }
    }
    
    menuCacheTime_ = std::chrono::system_clock::now();
    menuCacheValid_ = true;
    
    LOG_KEY_VALUE(getLogger(), info, "Menu cache updated with items", items.size());
    LOG_KEY_VALUE(getLogger(), debug, "Menu by ID cache entries", menuItemByIdCache_.size());
}

//============================================================================
// Additional methods for EnhancedPOSService.cpp - FIXED LAMBDA CAPTURES
//============================================================================

void EnhancedPOSService::getOrderByIdAsync(int orderId,
                                          std::function<void(std::shared_ptr<Order>, bool)> callback) {
    getLogger().info("[EnhancedPOSService] Getting order by ID asynchronously: " + std::to_string(orderId));
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getOrderByIdAsync", "Service not initialized");
        if (callback) callback(nullptr, false);
        return;
    }
    
    orderRepository_->findById(orderId, [this, orderId, callback](std::unique_ptr<Order> order, bool success) {
        std::shared_ptr<Order> sharedOrder = nullptr;
        
        if (success && order) {
            sharedOrder = std::make_shared<Order>(*order);
            getLogger().info("[EnhancedPOSService] Order " + std::to_string(orderId) + " retrieved from API");
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getOrderByIdAsync", 
                               "Failed to retrieve order " + std::to_string(orderId));
        }
        
        if (callback) callback(sharedOrder, success);
    });
}

void EnhancedPOSService::getOrdersByTableIdentifierAsync(const std::string& tableIdentifier,
                                                        std::function<void(std::vector<std::shared_ptr<Order>>, bool)> callback) {
    getLogger().info("[EnhancedPOSService] Getting orders by table identifier asynchronously: " + tableIdentifier);
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getOrdersByTableIdentifierAsync", "Service not initialized");
        if (callback) callback({}, false);
        return;
    }
    
    // Use repository to find orders by table identifier
    std::map<std::string, std::string> params;
    params["filter[table_identifier]"] = tableIdentifier;
    
    orderRepository_->findAll(params, [this, tableIdentifier, callback](std::vector<Order> orders, bool success) {
        std::vector<std::shared_ptr<Order>> sharedOrders;
        
        if (success) {
            sharedOrders.reserve(orders.size());
            for (const auto& order : orders) {
                sharedOrders.push_back(std::make_shared<Order>(order));
            }
            
            LOG_KEY_VALUE(getLogger(), info, "Orders for table '" + tableIdentifier + "'", sharedOrders.size());
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "getOrdersByTableIdentifierAsync", 
                               "Failed to retrieve orders for table: " + tableIdentifier);
        }
        
        if (callback) callback(sharedOrders, success);
    });
}

void EnhancedPOSService::updateOrderStatusAsync(int orderId, Order::Status status,
                                               std::function<void(bool)> callback) {
    getLogger().info("[EnhancedPOSService] Updating order status asynchronously: Order " + 
                    std::to_string(orderId) + " to status " + std::to_string(static_cast<int>(status)));
    
    if (!initialized_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "updateOrderStatusAsync", "Service not initialized");
        if (callback) callback(false);
        return;
    }
    
    // First, get the current order
    getOrderByIdAsync(orderId, [this, orderId, status, callback](std::shared_ptr<Order> order, bool success) {
        if (!success || !order) {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "updateOrderStatusAsync", 
                               "Could not retrieve order " + std::to_string(orderId) + " for status update");
            if (callback) callback(false);
            return;
        }
        
        // Update the status locally
        order->setStatus(status);
        
        // FIXED: Removed unused lambda capture 'status'
        orderRepository_->update(orderId, *order, [this, orderId, callback](std::unique_ptr<Order> updatedOrder, bool updateSuccess) {
            if (updateSuccess) {
                getLogger().info("[EnhancedPOSService] Order " + std::to_string(orderId) + " status updated successfully");
                
                // Publish event
                auto eventManager = getEventManager();
                if (eventManager) {
                    // FIXED: Create JSON event manually
                    Wt::Json::Object eventData;
                    eventData["orderId"] = Wt::Json::Value(updatedOrder->getOrderId());
                    eventData["tableIdentifier"] = Wt::Json::Value(updatedOrder->getTableIdentifier());
                    eventData["status"] = Wt::Json::Value(static_cast<int>(updatedOrder->getStatus()));
                    eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
                    eventData["message"] = Wt::Json::Value("Order " + std::to_string(updatedOrder->getOrderId()) + " modified");
                    
                    publishEvent(POSEvents::ORDER_STATUS_CHANGED, eventData);
                }
                
                LOG_OPERATION_STATUS(getLogger(), "Order status update", true);
            } else {
                LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "updateOrderStatusAsync", 
                                   "Failed to update order " + std::to_string(orderId) + " status");
            }
            
            if (callback) callback(updateSuccess);
        });
    });
}

void EnhancedPOSService::cancelOrderAsync(int orderId,
                                         std::function<void(bool)> callback) {
    getLogger().info("[EnhancedPOSService] Cancelling order asynchronously: " + std::to_string(orderId));
    
    updateOrderStatusAsync(orderId, Order::CANCELLED, [this, orderId, callback](bool success) {
        if (success) {
            getLogger().info("[EnhancedPOSService] Order " + std::to_string(orderId) + " cancelled successfully");
            
            // Clear current order if it's the one being cancelled
            if (currentOrder_ && currentOrder_->getOrderId() == orderId) {
                setCurrentOrder(nullptr);
            }
            
            // Publish cancellation event
            auto eventManager = getEventManager();
            if (eventManager) {
                Wt::Json::Object eventData;
                eventData["orderId"] = Wt::Json::Value(orderId);
                eventData["status"] = Wt::Json::Value("cancelled");
                eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                    std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
                
                publishEvent(POSEvents::ORDER_CANCELLED, eventData);
            }
            
            LOG_OPERATION_STATUS(getLogger(), "Order cancellation", true);
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "cancelOrderAsync", 
                               "Failed to cancel order " + std::to_string(orderId));
        }
        
        if (callback) callback(success);
    });
}

void EnhancedPOSService::sendOrderToKitchenAsync(int orderId,
                                                std::function<void(bool)> callback) {
    getLogger().info("[EnhancedPOSService] Sending order to kitchen asynchronously: " + std::to_string(orderId));
    
    updateOrderStatusAsync(orderId, Order::SENT_TO_KITCHEN, [this, orderId, callback](bool success) {
        if (success) {
            getLogger().info("[EnhancedPOSService] Order " + std::to_string(orderId) + " sent to kitchen successfully");
            
            // Publish kitchen event
            auto eventManager = getEventManager();
            if (eventManager) {
                Wt::Json::Object eventData;
                eventData["orderId"] = Wt::Json::Value(orderId);
                eventData["status"] = Wt::Json::Value("sent_to_kitchen");
                eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                    std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
                
                publishEvent(POSEvents::ORDER_SENT_TO_KITCHEN, eventData);
            }
            
            LOG_OPERATION_STATUS(getLogger(), "Send order to kitchen", true);
        } else {
            LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "sendOrderToKitchenAsync", 
                               "Failed to send order " + std::to_string(orderId) + " to kitchen");
        }
        
        if (callback) callback(success);
    });
}

void EnhancedPOSService::removeItemFromCurrentOrderAsync(size_t itemIndex,
                                                        std::function<void(bool)> callback) {
    getLogger().info("[EnhancedPOSService] Removing item from current order asynchronously: index " + std::to_string(itemIndex));
    
    if (!currentOrder_) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "removeItemFromCurrentOrderAsync", 
                           "No current order to remove item from");
        if (callback) callback(false);
        return;
    }
    
    if (itemIndex >= currentOrder_->getItems().size()) {
        LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "removeItemFromCurrentOrderAsync", 
                           "Invalid item index: " + std::to_string(itemIndex));
        if (callback) callback(false);
        return;
    }
    
    try {
        // Log the item being removed
        const auto& items = currentOrder_->getItems();
        const auto& itemToRemove = items[itemIndex];
        std::string itemName = itemToRemove.getMenuItem().getName();
        
        // Remove item locally
        currentOrder_->removeItem(itemIndex);
        
        getLogger().info("[EnhancedPOSService] Removed item: " + itemName);
        
        // FIXED: Removed unused lambda capture 'itemName'
        saveCurrentOrderAsync([this, callback](bool success) {
            if (success) {
                // Publish event
                auto eventManager = getEventManager();
                if (eventManager) {
                    // FIXED: Create JSON event manually
                    Wt::Json::Object eventData;
                    eventData["orderId"] = Wt::Json::Value(currentOrder_->getOrderId());
                    eventData["tableIdentifier"] = Wt::Json::Value(currentOrder_->getTableIdentifier());
                    eventData["status"] = Wt::Json::Value(static_cast<int>(currentOrder_->getStatus()));
                    eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
                    eventData["message"] = Wt::Json::Value("Order " + std::to_string(currentOrder_->getOrderId()) + " modified");
                    
                    publishEvent(POSEvents::ORDER_ITEM_REMOVED, eventData);
                }
                
                LOG_OPERATION_STATUS(getLogger(), "Remove item from current order (async)", true);
            } else {
                LOG_COMPONENT_ERROR(getLogger(), "EnhancedPOSService", "removeItemFromCurrentOrderAsync", 
                                   "Failed to save order to API after removing item");
            }
            
            if (callback) callback(success);
        });
        
    } catch (const std::exception& e) {
        handleAPIError("removeItemFromCurrentOrderAsync", e.what());
        if (callback) callback(false);
    }
}

// Additional methods would follow the same pattern of fixed lambda captures...
