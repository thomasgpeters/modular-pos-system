//============================================================================
// src/services/EnhancedPOSService.cpp - Implementation
//============================================================================

#include "../../include/services/EnhancedPOSService.hpp"
#include <iostream>
#include <algorithm>

EnhancedPOSService::EnhancedPOSService(std::shared_ptr<EventManager> eventManager,
                                       const ServiceConfig& config)
    : POSService(eventManager),  // Call base class constructor
      config_(config), initialized_(false), menuCacheValid_(false) {
    
    std::cout << "[EnhancedPOSService] Initializing with API URL: " << config_.apiBaseUrl << std::endl;
}

bool EnhancedPOSService::initialize() {
    try {
        initializeAPIComponents();
        initializeCaches();
        
        initialized_ = true;
        std::cout << "[EnhancedPOSService] Initialization complete" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[EnhancedPOSService] Initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void EnhancedPOSService::initializeAPIComponents() {
    // Create API client
    apiClient_ = std::make_shared<APIClient>(config_.apiBaseUrl);
    apiClient_->setTimeout(config_.apiTimeout);
    apiClient_->setDebugMode(config_.debugMode);
    
    if (!config_.authToken.empty()) {
        apiClient_->setAuthToken(config_.authToken);
    }
    
    // Create repositories
    orderRepository_ = std::make_unique<OrderRepository>(apiClient_);
    menuItemRepository_ = std::make_unique<MenuItemRepository>(apiClient_);
    employeeRepository_ = std::make_unique<EmployeeRepository>(apiClient_);
    
    std::cout << "[EnhancedPOSService] API components initialized" << std::endl;
}

void EnhancedPOSService::initializeCaches() {
    menuItemsCache_.clear();
    menuItemByIdCache_.clear();
    menuCacheValid_ = false;
    
    std::cout << "[EnhancedPOSService] Caches initialized" << std::endl;
}

// =================================================================
// Enhanced methods (do not override - enhance base class functionality)
// =================================================================

std::shared_ptr<Order> EnhancedPOSService::createOrder(const std::string& tableIdentifier) {
    if (!initialized_) {
        std::cerr << "[EnhancedPOSService] Service not initialized, falling back to base class" << std::endl;
        // Fall back to base class implementation
        return POSService::createOrder(tableIdentifier);
    }
    
    try {
        // Create order locally first
        static int nextOrderId = 1000; // In real app, get from API
        auto order = std::make_shared<Order>(nextOrderId++, tableIdentifier);
        
        // For synchronous version, we'll just return the local order
        // In async version, we save to API
        std::cout << "[EnhancedPOSService] Order created: " << order->getOrderId() << std::endl;
        
        return order;
        
    } catch (const std::exception& e) {
        handleAPIError("createOrder", e.what());
        // Fall back to base class on error
        return POSService::createOrder(tableIdentifier);
    }
}

std::vector<std::shared_ptr<Order>> EnhancedPOSService::getActiveOrders() {
    if (!initialized_) {
        std::cerr << "[EnhancedPOSService] Service not initialized, falling back to base class" << std::endl;
        return POSService::getActiveOrders();
    }
    
    // For synchronous version, suggest using async and fall back to base class
    std::cout << "[EnhancedPOSService] Use getActiveOrdersAsync() for API data, falling back to base class" << std::endl;
    return POSService::getActiveOrders();
}

std::shared_ptr<Order> EnhancedPOSService::getCurrentOrder() const {
    // Use the enhanced local tracking first
    if (currentOrder_) {
        return currentOrder_;
    }
    // Fall back to base class if needed
    return POSService::getCurrentOrder();
}

void EnhancedPOSService::setCurrentOrder(std::shared_ptr<Order> order) {
    auto previousOrder = getCurrentOrder();
    
    // Update our local tracking
    currentOrder_ = order;
    
    // Also update the base class
    POSService::setCurrentOrder(order);
    
    // Publish enhanced event if event manager is available
    auto eventManager = getEventManager();
    if (eventManager) {
        auto eventData = POSEvents::createCurrentOrderChangedEvent(
            order, previousOrder, order ? "order_set" : "order_cleared"
        );
        publishEvent(POSEvents::CURRENT_ORDER_CHANGED, eventData);
    }
}

std::vector<std::shared_ptr<MenuItem>> EnhancedPOSService::getMenuItems() {
    // Return cached items if available
    if (config_.enableCaching && menuCacheValid_ && !isMenuCacheExpired()) {
        return menuItemsCache_;
    }
    
    if (!initialized_) {
        std::cerr << "[EnhancedPOSService] Service not initialized, falling back to base class" << std::endl;
        return POSService::getMenuItems();
    }
    
    // For sync version, fall back to base class if cache is invalid
    std::cout << "[EnhancedPOSService] Use getMenuItemsAsync() to refresh from API, falling back to base class" << std::endl;
    return POSService::getMenuItems();
}

std::shared_ptr<MenuItem> EnhancedPOSService::getMenuItemById(int itemId) {
    auto it = menuItemByIdCache_.find(itemId);
    if (it != menuItemByIdCache_.end()) {
        return it->second;
    }
    
    // Fall back to base class implementation
    return POSService::getMenuItemById(itemId);
}

// =================================================================
// Enhanced Async Methods (API-Backed)
// =================================================================

void EnhancedPOSService::createOrderAsync(const std::string& tableIdentifier,
                                         std::function<void(std::shared_ptr<Order>, bool)> callback) {
    
    if (!initialized_) {
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
                
                // Publish event
                auto eventManager = getEventManager();
                if (eventManager) {
                    auto eventData = POSEvents::createOrderCreatedEvent(order);
                    publishEvent(POSEvents::ORDER_CREATED, eventData);
                }
                
                std::cout << "[EnhancedPOSService] Order created: " << order->getOrderId() << std::endl;
                
                if (callback) callback(order, true);
            } else {
                handleAPIError("createOrder", "Failed to create order in API");
                if (callback) callback(nullptr, false);
            }
        });
        
    } catch (const std::exception& e) {
        handleAPIError("createOrder", e.what());
        if (callback) callback(nullptr, false);
    }
}

void EnhancedPOSService::getActiveOrdersAsync(std::function<void(std::vector<std::shared_ptr<Order>>, bool)> callback) {
    if (!initialized_) {
        if (callback) callback({}, false);
        return;
    }
    
    // Filter for active orders (not served or cancelled)
    std::map<std::string, std::string> params;
    params["filter[status]"] = "0,1,2,3"; // PENDING,SENT_TO_KITCHEN,PREPARING,READY
    
    orderRepository_->findAll(params, [callback](std::vector<Order> orders, bool success) {
        std::vector<std::shared_ptr<Order>> sharedOrders;
        
        if (success) {
            sharedOrders.reserve(orders.size());
            for (const auto& order : orders) {
                sharedOrders.push_back(std::make_shared<Order>(order));
            }
        }
        
        if (callback) callback(sharedOrders, success);
    });
}

// =================================================================
// Menu Management Implementation
// =================================================================

void EnhancedPOSService::getMenuItemsAsync(bool forceRefresh,
                                          std::function<void(std::vector<std::shared_ptr<MenuItem>>, bool)> callback) {
    
    if (!initialized_) {
        if (callback) callback({}, false);
        return;
    }
    
    // Check cache first (if enabled and not forced refresh)
    if (config_.enableCaching && !forceRefresh && menuCacheValid_ && !isMenuCacheExpired()) {
        std::cout << "[EnhancedPOSService] Returning cached menu items" << std::endl;
        if (callback) callback(menuItemsCache_, true);
        return;
    }
    
    // Fetch from API
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
            
            std::cout << "[EnhancedPOSService] Loaded " << sharedItems.size() << " menu items from API" << std::endl;
        } else {
            handleAPIError("getMenuItems", "Failed to fetch menu items");
        }
        
        if (callback) callback(sharedItems, success);
    });
}

void EnhancedPOSService::getMenuItemsByCategoryAsync(MenuItem::Category category,
                                                    std::function<void(std::vector<std::shared_ptr<MenuItem>>, bool)> callback) {
    
    // If cache is valid, filter locally
    if (config_.enableCaching && menuCacheValid_ && !isMenuCacheExpired()) {
        std::vector<std::shared_ptr<MenuItem>> categoryItems;
        
        for (const auto& item : menuItemsCache_) {
            if (item && item->getCategory() == category) {
                categoryItems.push_back(item);
            }
        }
        
        if (callback) callback(categoryItems, true);
        return;
    }
    
    // Otherwise fetch from API
    menuItemRepository_->findByCategory(category, [callback](std::vector<MenuItem> items, bool success) {
        std::vector<std::shared_ptr<MenuItem>> sharedItems;
        
        if (success) {
            sharedItems.reserve(items.size());
            for (const auto& item : items) {
                sharedItems.push_back(std::make_shared<MenuItem>(item));
            }
        }
        
        if (callback) callback(sharedItems, success);
    });
}

// =================================================================
// Current Order Management Implementation
// =================================================================

void EnhancedPOSService::addItemToCurrentOrderAsync(const MenuItem& item, int quantity,
                                                   const std::string& instructions,
                                                   std::function<void(bool)> callback) {
    
    if (!currentOrder_) {
        std::cerr << "[EnhancedPOSService] No current order to add item to" << std::endl;
        if (callback) callback(false);
        return;
    }
    
    if (!item.isAvailable()) {
        std::cerr << "[EnhancedPOSService] Menu item not available: " << item.getName() << std::endl;
        if (callback) callback(false);
        return;
    }
    
    try {
        // Add item locally
        OrderItem orderItem(item, quantity);
        if (!instructions.empty()) {
            orderItem.setSpecialInstructions(instructions);
        }
        
        currentOrder_->addItem(orderItem);
        
        // Save current order to API (async)
        saveCurrentOrderAsync([this, callback](bool success) {
            if (success) {
                // Publish event
                auto eventManager = getEventManager();
                if (eventManager) {
                    auto eventData = POSEvents::createOrderModifiedEvent(currentOrder_);
                    publishEvent(POSEvents::ORDER_ITEM_ADDED, eventData);
                }
                
                std::cout << "[EnhancedPOSService] Added item to current order and saved to API" << std::endl;
            } else {
                std::cerr << "[EnhancedPOSService] Failed to save order to API after adding item" << std::endl;
            }
            
            if (callback) callback(success);
        });
        
    } catch (const std::exception& e) {
        handleAPIError("addItemToCurrentOrder", e.what());
        if (callback) callback(false);
    }
}

void EnhancedPOSService::saveCurrentOrderAsync(std::function<void(bool)> callback) {
    if (!currentOrder_) {
        if (callback) callback(false);
        return;
    }
    
    // Save to API
    orderRepository_->update(currentOrder_->getOrderId(), *currentOrder_, 
                           [callback](std::unique_ptr<Order> updatedOrder, bool success) {
        if (callback) callback(success);
    });
}

// =================================================================
// Employee Management
// =================================================================

void EnhancedPOSService::getEmployeesAsync(std::function<void(std::vector<Employee>, bool)> callback) {
    if (!initialized_) {
        if (callback) callback({}, false);
        return;
    }
    
    employeeRepository_->findAll({}, callback);
}

void EnhancedPOSService::getEmployeesByRoleAsync(const std::string& role,
                                                 std::function<void(std::vector<Employee>, bool)> callback) {
    if (!initialized_) {
        if (callback) callback({}, false);
        return;
    }
    
    employeeRepository_->findByRole(role, callback);
}

void EnhancedPOSService::getActiveEmployeesAsync(std::function<void(std::vector<Employee>, bool)> callback) {
    if (!initialized_) {
        if (callback) callback({}, false);
        return;
    }
    
    employeeRepository_->findActive(callback);
}

// =================================================================
// Configuration Methods
// =================================================================

void EnhancedPOSService::setAuthToken(const std::string& token) {
    config_.authToken = token;
    if (apiClient_) {
        apiClient_->setAuthToken(token);
    }
}

void EnhancedPOSService::clearCaches() {
    menuItemsCache_.clear();
    menuItemByIdCache_.clear();
    menuCacheValid_ = false;
    
    std::cout << "[EnhancedPOSService] All caches cleared" << std::endl;
}

// =================================================================
// Helper Methods Implementation
// =================================================================

void EnhancedPOSService::handleAPIError(const std::string& operation, const std::string& error) {
    std::cerr << "[EnhancedPOSService] API Error in " << operation << ": " << error << std::endl;
    
    auto eventManager = getEventManager();
    if (eventManager) {
        auto errorData = POSEvents::createErrorEvent(
            "API Error in " + operation + ": " + error,
            "API_ERROR",
            "EnhancedPOSService",
            true
        );
        publishEvent(POSEvents::SYSTEM_ERROR, errorData);
    }
}

void EnhancedPOSService::publishEvent(const std::string& eventType, const Wt::Json::Object& eventData) {
    auto eventManager = getEventManager();
    if (eventManager) {
        eventManager->publish(eventType, eventData);
    }
}

bool EnhancedPOSService::isMenuCacheExpired() const {
    auto now = std::chrono::system_clock::now();
    auto cacheAge = std::chrono::duration_cast<std::chrono::minutes>(now - menuCacheTime_);
    return cacheAge.count() >= CACHE_TIMEOUT_MINUTES;
}

void EnhancedPOSService::updateMenuCache(const std::vector<std::shared_ptr<MenuItem>>& items) {
    menuItemsCache_ = items;
    menuItemByIdCache_.clear();
    
    for (const auto& item : items) {
        if (item) {
            menuItemByIdCache_[item->getId()] = item;
        }
    }
    
    menuCacheTime_ = std::chrono::system_clock::now();
    menuCacheValid_ = true;
    
    std::cout << "[EnhancedPOSService] Menu cache updated with " << items.size() << " items" << std::endl;
}

//============================================================================
// Additional methods for EnhancedPOSService.cpp - Missing API methods
//============================================================================

// Add these methods to the existing EnhancedPOSService.cpp file

void EnhancedPOSService::getOrderByIdAsync(int orderId,
                                          std::function<void(std::shared_ptr<Order>, bool)> callback) {
    if (!initialized_) {
        if (callback) callback(nullptr, false);
        return;
    }
    
    orderRepository_->findById(orderId, [callback](std::unique_ptr<Order> order, bool success) {
        std::shared_ptr<Order> sharedOrder = nullptr;
        
        if (success && order) {
            sharedOrder = std::make_shared<Order>(*order);
        }
        
        if (callback) callback(sharedOrder, success);
    });
}

void EnhancedPOSService::getOrdersByTableIdentifierAsync(const std::string& tableIdentifier,
                                                        std::function<void(std::vector<std::shared_ptr<Order>>, bool)> callback) {
    if (!initialized_) {
        if (callback) callback({}, false);
        return;
    }
    
    // Use repository to find orders by table identifier
    std::map<std::string, std::string> params;
    params["filter[table_identifier]"] = tableIdentifier;
    
    orderRepository_->findAll(params, [callback](std::vector<Order> orders, bool success) {
        std::vector<std::shared_ptr<Order>> sharedOrders;
        
        if (success) {
            sharedOrders.reserve(orders.size());
            for (const auto& order : orders) {
                sharedOrders.push_back(std::make_shared<Order>(order));
            }
        }
        
        if (callback) callback(sharedOrders, success);
    });
}

void EnhancedPOSService::updateOrderStatusAsync(int orderId, Order::Status status,
                                               std::function<void(bool)> callback) {
    if (!initialized_) {
        if (callback) callback(false);
        return;
    }
    
    // First, get the current order
    getOrderByIdAsync(orderId, [this, orderId, status, callback](std::shared_ptr<Order> order, bool success) {
        if (!success || !order) {
            std::cerr << "[EnhancedPOSService] Could not retrieve order " << orderId << " for status update" << std::endl;
            if (callback) callback(false);
            return;
        }
        
        // Update the status locally
        order->setStatus(status);
        
        // Save the updated order to API
        orderRepository_->update(orderId, *order, [this, orderId, status, callback](std::unique_ptr<Order> updatedOrder, bool updateSuccess) {
            if (updateSuccess) {
                std::cout << "[EnhancedPOSService] Order " << orderId << " status updated to " << Order::statusToString(status) << std::endl;
                
                // Publish event
                auto eventManager = getEventManager();
                if (eventManager) {
                    auto eventData = POSEvents::createOrderModifiedEvent(std::make_shared<Order>(*updatedOrder));
                    publishEvent(POSEvents::ORDER_STATUS_CHANGED, eventData);
                }
            } else {
                std::cerr << "[EnhancedPOSService] Failed to update order " << orderId << " status" << std::endl;
            }
            
            if (callback) callback(updateSuccess);
        });
    });
}

void EnhancedPOSService::cancelOrderAsync(int orderId,
                                         std::function<void(bool)> callback) {
    std::cout << "[EnhancedPOSService] Cancelling order " << orderId << " via API..." << std::endl;
    
    updateOrderStatusAsync(orderId, Order::CANCELLED, [this, orderId, callback](bool success) {
        if (success) {
            std::cout << "[EnhancedPOSService] Order " << orderId << " cancelled successfully" << std::endl;
            
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
        }
        
        if (callback) callback(success);
    });
}

void EnhancedPOSService::sendOrderToKitchenAsync(int orderId,
                                                std::function<void(bool)> callback) {
    std::cout << "[EnhancedPOSService] Sending order " << orderId << " to kitchen via API..." << std::endl;
    
    updateOrderStatusAsync(orderId, Order::SENT_TO_KITCHEN, [this, orderId, callback](bool success) {
        if (success) {
            std::cout << "[EnhancedPOSService] Order " << orderId << " sent to kitchen successfully" << std::endl;
            
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
        }
        
        if (callback) callback(success);
    });
}

void EnhancedPOSService::removeItemFromCurrentOrderAsync(size_t itemIndex,
                                                        std::function<void(bool)> callback) {
    if (!currentOrder_) {
        std::cerr << "[EnhancedPOSService] No current order to remove item from" << std::endl;
        if (callback) callback(false);
        return;
    }
    
    if (itemIndex >= currentOrder_->getItems().size()) {
        std::cerr << "[EnhancedPOSService] Invalid item index: " << itemIndex << std::endl;
        if (callback) callback(false);
        return;
    }
    
    try {
        // Remove item locally
        currentOrder_->removeItem(itemIndex);
        
        // Save current order to API
        saveCurrentOrderAsync([this, callback](bool success) {
            if (success) {
                // Publish event
                auto eventManager = getEventManager();
                if (eventManager) {
                    auto eventData = POSEvents::createOrderModifiedEvent(currentOrder_);
                    publishEvent(POSEvents::ORDER_ITEM_REMOVED, eventData);
                }
                
                std::cout << "[EnhancedPOSService] Removed item from current order and saved to API" << std::endl;
            } else {
                std::cerr << "[EnhancedPOSService] Failed to save order to API after removing item" << std::endl;
            }
            
            if (callback) callback(success);
        });
        
    } catch (const std::exception& e) {
        handleAPIError("removeItemFromCurrentOrder", e.what());
        if (callback) callback(false);
    }
}

void EnhancedPOSService::updateCurrentOrderItemQuantityAsync(size_t itemIndex, int newQuantity,
                                                            std::function<void(bool)> callback) {
    if (!currentOrder_) {
        std::cerr << "[EnhancedPOSService] No current order to update item in" << std::endl;
        if (callback) callback(false);
        return;
    }
    
    if (itemIndex >= currentOrder_->getItems().size()) {
        std::cerr << "[EnhancedPOSService] Invalid item index: " << itemIndex << std::endl;
        if (callback) callback(false);
        return;
    }
    
    if (newQuantity <= 0) {
        std::cerr << "[EnhancedPOSService] Invalid quantity: " << newQuantity << std::endl;
        if (callback) callback(false);
        return;
    }
    
    try {
        // Update quantity locally
        currentOrder_->updateItemQuantity(itemIndex, newQuantity);
        
        // Save current order to API
        saveCurrentOrderAsync([this, callback](bool success) {
            if (success) {
                // Publish event
                auto eventManager = getEventManager();
                if (eventManager) {
                    auto eventData = POSEvents::createOrderModifiedEvent(currentOrder_);
                    publishEvent(POSEvents::ORDER_MODIFIED, eventData);
                }
                
                std::cout << "[EnhancedPOSService] Updated item quantity in current order and saved to API" << std::endl;
            } else {
                std::cerr << "[EnhancedPOSService] Failed to save order to API after updating quantity" << std::endl;
            }
            
            if (callback) callback(success);
        });
        
    } catch (const std::exception& e) {
        handleAPIError("updateCurrentOrderItemQuantity", e.what());
        if (callback) callback(false);
    }
}

void EnhancedPOSService::sendCurrentOrderToKitchenAsync(std::function<void(bool)> callback) {
    if (!currentOrder_) {
        std::cerr << "[EnhancedPOSService] No current order to send to kitchen" << std::endl;
        if (callback) callback(false);
        return;
    }
    
    if (currentOrder_->getItems().empty()) {
        std::cerr << "[EnhancedPOSService] Cannot send empty order to kitchen" << std::endl;
        if (callback) callback(false);
        return;
    }
    
    int orderId = currentOrder_->getOrderId();
    
    sendOrderToKitchenAsync(orderId, [this, callback](bool success) {
        if (success) {
            // Clear current order after sending to kitchen
            setCurrentOrder(nullptr);
        }
        
        if (callback) callback(success);
    });
}

void EnhancedPOSService::refreshMenuCacheAsync(std::function<void(bool)> callback) {
    getMenuItemsAsync(true, [callback](std::vector<std::shared_ptr<MenuItem>> items, bool success) {
        if (success) {
            std::cout << "[EnhancedPOSService] Menu cache refreshed with " << items.size() << " items" << std::endl;
        } else {
            std::cout << "[EnhancedPOSService] Failed to refresh menu cache" << std::endl;
        }
        
        if (callback) callback(success);
    });
}

void EnhancedPOSService::getAvailableMenuItemsAsync(std::function<void(std::vector<std::shared_ptr<MenuItem>>, bool)> callback) {
    if (!initialized_) {
        if (callback) callback({}, false);
        return;
    }
    
    // Fetch available items from API
    menuItemRepository_->findAvailable([callback](std::vector<MenuItem> items, bool success) {
        std::vector<std::shared_ptr<MenuItem>> sharedItems;
        
        if (success) {
            sharedItems.reserve(items.size());
            for (const auto& item : items) {
                if (item.isAvailable()) {
                    sharedItems.push_back(std::make_shared<MenuItem>(item));
                }
            }
        }
        
        if (callback) callback(sharedItems, success);
    });
}

void EnhancedPOSService::getKitchenTicketsAsync(std::function<void(std::vector<KitchenInterface::KitchenTicket>, bool)> callback) {
    if (!initialized_) {
        if (callback) callback({}, false);
        return;
    }
    
    // For now, we'll simulate kitchen tickets from orders
    // In a real implementation, you'd have a separate KitchenTicket endpoint
    getActiveOrdersAsync([callback](std::vector<std::shared_ptr<Order>> orders, bool success) {
        std::vector<KitchenInterface::KitchenTicket> tickets;
        
        if (success) {
            for (const auto& order : orders) {
                if (order->getStatus() == Order::SENT_TO_KITCHEN || 
                    order->getStatus() == Order::PREPARING) {
                    
                    KitchenInterface::KitchenTicket ticket;
                    ticket.orderId = order->getOrderId();
                    ticket.tableNumber = order->getTableNumber();
                    ticket.timestamp = order->getTimestamp();
                    ticket.status = (order->getStatus() == Order::SENT_TO_KITCHEN) ? 
                                   KitchenInterface::ORDER_RECEIVED : 
                                   KitchenInterface::PREP_STARTED;
                    
                    // Convert order items to string list
                    for (const auto& item : order->getItems()) {
                        std::string itemStr = std::to_string(item.getQuantity()) + "x " + 
                                            item.getMenuItem().getName();
                        if (!item.getSpecialInstructions().empty()) {
                            itemStr += " (" + item.getSpecialInstructions() + ")";
                        }
                        ticket.items.push_back(itemStr);
                    }
                    
                    tickets.push_back(ticket);
                }
            }
        }
        
        if (callback) callback(tickets, success);
    });
}

void EnhancedPOSService::getEstimatedWaitTimeAsync(std::function<void(int, bool)> callback) {
    if (!initialized_) {
        if (callback) callback(0, false);
        return;
    }
    
    // Calculate estimated wait time based on active orders
    getKitchenTicketsAsync([callback](std::vector<KitchenInterface::KitchenTicket> tickets, bool success) {
        int estimatedTime = 0;
        
        if (success) {
            // Simple calculation: 15 minutes base + 5 minutes per order in queue
            estimatedTime = 15 + (static_cast<int>(tickets.size()) * 5);
            
            // Cap at reasonable maximum
            if (estimatedTime > 60) {
                estimatedTime = 60;
            }
        }
        
        if (callback) callback(estimatedTime, success);
    });
}

void EnhancedPOSService::getKitchenQueueStatusAsync(std::function<void(Wt::Json::Object, bool)> callback) {
    if (!initialized_) {
        Wt::Json::Object emptyStatus;
        emptyStatus["queueSize"] = Wt::Json::Value(0);
        emptyStatus["estimatedWaitTime"] = Wt::Json::Value(0);
        if (callback) callback(emptyStatus, false);
        return;
    }
    
    getKitchenTicketsAsync([callback](std::vector<KitchenInterface::KitchenTicket> tickets, bool success) {
        Wt::Json::Object status;
        
        if (success) {
            status["queueSize"] = Wt::Json::Value(static_cast<int>(tickets.size()));
            
            // Calculate estimated wait time
            int estimatedTime = 15 + (static_cast<int>(tickets.size()) * 5);
            if (estimatedTime > 60) estimatedTime = 60;
            
            status["estimatedWaitTime"] = Wt::Json::Value(estimatedTime);
            status["isBusy"] = Wt::Json::Value(tickets.size() > 5);
            status["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        } else {
            status["queueSize"] = Wt::Json::Value(0);
            status["estimatedWaitTime"] = Wt::Json::Value(0);
            status["isBusy"] = Wt::Json::Value(false);
        }
        
        if (callback) callback(status, success);
    });
}

void EnhancedPOSService::processPaymentAsync(std::shared_ptr<Order> order,
                                            PaymentProcessor::PaymentMethod method,
                                            double amount, double tipAmount,
                                            std::function<void(PaymentProcessor::PaymentResult, bool)> callback) {
    if (!order) {
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "Invalid order";
        if (callback) callback(result, false);
        return;
    }
    
    // For now, process payment locally and simulate API call
    // In a real implementation, you'd call a Payment API endpoint
    
    try {
        // Use the local payment processor
        auto result = processPayment(order, method, amount, tipAmount);
        
        if (result.success) {
            // Update order status to indicate payment received
            updateOrderStatusAsync(order->getOrderId(), Order::SERVED, [result, order, callback](bool statusUpdateSuccess) {
                if (statusUpdateSuccess) {
                    std::cout << "[EnhancedPOSService] Payment processed and order status updated" << std::endl;
                } else {
                    std::cout << "[EnhancedPOSService] Payment processed but failed to update order status" << std::endl;
                }
                
                if (callback) callback(result, result.success);
            });
        } else {
            if (callback) callback(result, false);
        }
        
    } catch (const std::exception& e) {
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "Payment processing error: " + std::string(e.what());
        
        handleAPIError("processPayment", e.what());
        if (callback) callback(result, false);
    }
}

void EnhancedPOSService::getTransactionHistoryAsync(std::function<void(std::vector<PaymentProcessor::PaymentResult>, bool)> callback) {
    if (!initialized_) {
        if (callback) callback({}, false);
        return;
    }
    
    // For now, return local transaction history
    // In a real implementation, you'd fetch from a Transactions API endpoint
    
    try {
        auto transactions = getTransactionHistory();
        if (callback) callback(transactions, true);
        
    } catch (const std::exception& e) {
        handleAPIError("getTransactionHistory", e.what());
        if (callback) callback({}, false);
    }
}
