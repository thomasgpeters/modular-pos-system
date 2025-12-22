#include "../../include/services/POSService.hpp"
#include "../../include/utils/LoggingUtils.hpp"

#include <algorithm>
#include <iostream>
#include <regex>

POSService::POSService(std::shared_ptr<EventManager> eventManager)
    : logger_(Logger::getInstance())  // ADDED: Initialize logger reference
    , eventManager_(eventManager)
    , currentOrder_(nullptr)
    , orderCreatedCallback_(nullptr)
    , orderModifiedCallback_(nullptr) {
    
    logger_.info("[POSService] Initializing POS service...");
    
    initializeSubsystems();
    initializeMenuItems();
    
    LOG_OPERATION_STATUS(logger_, "POSService initialization", true);
}

void POSService::initializeSubsystems() {
    logger_.info("[POSService] Initializing subsystems...");
    
    try {
        // Initialize the major subsystems
        orderManager_ = std::make_unique<OrderManager>();
        kitchenInterface_ = std::make_unique<KitchenInterface>();
        paymentProcessor_ = std::make_unique<PaymentProcessor>();
        
        LOG_OPERATION_STATUS(logger_, "Subsystem initialization", true);
        logger_.info("POSService subsystems initialized: OrderManager, KitchenInterface, PaymentProcessor");
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "initializeSubsystems", e.what());
        throw;
    }
}

void POSService::initializeMenuItems() {
    logger_.info("[POSService] Initializing menu items...");
    
    menuItems_.clear();
    
    try {
        // Create some default menu items using the correct constructor
        // MenuItem(int id, const std::string& name, double price, Category category)
        menuItems_.push_back(std::make_shared<MenuItem>(1, "Caesar Salad", 8.99, MenuItem::APPETIZER));
        menuItems_.push_back(std::make_shared<MenuItem>(2, "Garlic Bread", 5.99, MenuItem::APPETIZER));
        menuItems_.push_back(std::make_shared<MenuItem>(3, "Grilled Chicken", 15.99, MenuItem::MAIN_COURSE));
        menuItems_.push_back(std::make_shared<MenuItem>(4, "Beef Steak", 22.99, MenuItem::MAIN_COURSE));
        menuItems_.push_back(std::make_shared<MenuItem>(5, "Chocolate Cake", 6.99, MenuItem::DESSERT));
        menuItems_.push_back(std::make_shared<MenuItem>(6, "Ice Cream", 4.99, MenuItem::DESSERT));
        menuItems_.push_back(std::make_shared<MenuItem>(7, "Coffee", 2.99, MenuItem::BEVERAGE));
        menuItems_.push_back(std::make_shared<MenuItem>(8, "Soft Drink", 2.49, MenuItem::BEVERAGE));
        menuItems_.push_back(std::make_shared<MenuItem>(9, "Today's Special", 18.99, MenuItem::SPECIAL));
        
        LOG_KEY_VALUE(logger_, info, "Menu items loaded", menuItems_.size());
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "initializeMenuItems", e.what());
        throw;
    }
}

// =====================================================================
// Order Management Methods (Delegate to OrderManager)
// =====================================================================

std::shared_ptr<Order> POSService::createOrder(const std::string& tableIdentifier) {
    logger_.info("[POSService] Creating order for table: " + tableIdentifier);
    
    if (!orderManager_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "createOrder", "OrderManager not initialized");
        return nullptr;
    }
    
    try {
        auto order = orderManager_->createOrder(tableIdentifier);
        
        if (order) {
            // Log order creation details
            POSEvents::EventLogger::logOrderEvent(
                POSEvents::ORDER_CREATED, 
                order, 
                "Order created for " + tableIdentifier
            );
            
            if (eventManager_) {
                // FIXED: Create JSON event manually
                Wt::Json::Object eventData;
                eventData["orderId"] = Wt::Json::Value(order->getOrderId());
                eventData["tableIdentifier"] = Wt::Json::Value(order->getTableIdentifier());
                eventData["status"] = Wt::Json::Value(static_cast<int>(order->getStatus()));
                eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                    std::chrono::system_clock::to_time_t(order->getTimestamp())));
                eventData["message"] = Wt::Json::Value("Order created for " + order->getTableIdentifier());
                
                eventManager_->publish(POSEvents::ORDER_CREATED, eventData, "POSService");
                
                // Also call the UI callback if registered
                if (orderCreatedCallback_) {
                    orderCreatedCallback_(order);
                }
            }
            
            LOG_OPERATION_STATUS(logger_, "Order creation", true);
        } else {
            LOG_COMPONENT_ERROR(logger_, "POSService", "createOrder", "Failed to create order");
        }
        
        return order;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "createOrder", e.what());
        return nullptr;
    }
}

std::shared_ptr<Order> POSService::createOrder(int tableNumber) {
    // Convert table number to string identifier for compatibility
    std::string tableIdentifier = "table " + std::to_string(tableNumber);
    logger_.debug("[POSService] Converting table number " + std::to_string(tableNumber) + " to identifier: " + tableIdentifier);
    return createOrder(tableIdentifier);
}

std::vector<std::shared_ptr<Order>> POSService::getActiveOrders() const {
    logger_.debug("[POSService] Retrieving active orders");
    
    if (!orderManager_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getActiveOrders", "OrderManager not initialized");
        return {};
    }
    
    try {
        auto orders = orderManager_->getActiveOrders();
        LOG_KEY_VALUE(logger_, debug, "Active orders found", orders.size());
        return orders;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getActiveOrders", e.what());
        return {};
    }
}

std::shared_ptr<Order> POSService::getOrderById(int orderId) const {
    logger_.debug("[POSService] Looking up order ID: " + std::to_string(orderId));
    
    if (!orderManager_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getOrderById", "OrderManager not initialized");
        return nullptr;
    }
    
    try {
        auto order = orderManager_->getOrder(orderId);
        if (order) {
            logger_.debug("[POSService] Found order " + std::to_string(orderId));
        } else {
            logger_.debug("[POSService] Order " + std::to_string(orderId) + " not found");
        }
        return order;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getOrderById", e.what());
        return nullptr;
    }
}

std::shared_ptr<Order> POSService::getCurrentOrder() const {
    if (currentOrder_) {
        logger_.debug("[POSService] Current order: " + std::to_string(currentOrder_->getOrderId()));
    } else {
        logger_.debug("[POSService] No current order set");
    }
    return currentOrder_;
}

void POSService::setCurrentOrder(std::shared_ptr<Order> order) {
    auto previousOrder = currentOrder_;
    currentOrder_ = order;
    
    if (order) {
        logger_.info("[POSService] Set current order to: " + std::to_string(order->getOrderId()));
    } else {
        logger_.info("[POSService] Cleared current order");
    }
    
    if (eventManager_) {
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
        
        eventData["reason"] = Wt::Json::Value(order ? "order_set" : "order_cleared");
        eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        eventData["message"] = Wt::Json::Value("Current order changed: " + std::string(order ? "order_set" : "order_cleared"));
        
        eventManager_->publish(POSEvents::CURRENT_ORDER_CHANGED, eventData, "POSService");
    }
}

bool POSService::cancelOrder(int orderId) {
    logger_.info("[POSService] Attempting to cancel order: " + std::to_string(orderId));
    
    if (!orderManager_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "cancelOrder", "OrderManager not initialized");
        return false;
    }
    
    try {
        bool success = orderManager_->cancelOrder(orderId);
        
        if (success) {
            logger_.info("[POSService] Successfully cancelled order: " + std::to_string(orderId));
            
            // Clear current order if it's the one being cancelled
            if (currentOrder_ && currentOrder_->getOrderId() == orderId) {
                setCurrentOrder(nullptr); // This will trigger CURRENT_ORDER_CHANGED event
            }
            
            if (eventManager_) {
                auto order = orderManager_->getOrder(orderId);
                if (order) {
                    // FIXED: Create JSON event manually
                    Wt::Json::Object eventData;
                    eventData["orderId"] = Wt::Json::Value(order->getOrderId());
                    eventData["tableIdentifier"] = Wt::Json::Value(order->getTableIdentifier());
                    eventData["status"] = Wt::Json::Value(static_cast<int>(order->getStatus()));
                    eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
                    eventData["message"] = Wt::Json::Value("Order " + std::to_string(order->getOrderId()) + " cancelled");
                    
                    eventManager_->publish(POSEvents::ORDER_CANCELLED, eventData, "POSService");
                }
            }
            
            LOG_OPERATION_STATUS(logger_, "Order cancellation", true);
        } else {
            LOG_COMPONENT_ERROR(logger_, "POSService", "cancelOrder", "Failed to cancel order " + std::to_string(orderId));
        }
        
        return success;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "cancelOrder", e.what());
        return false;
    }
}

bool POSService::isTableIdentifierInUse(const std::string& tableIdentifier) const {
    logger_.debug("[POSService] Checking if table identifier in use: " + tableIdentifier);
    
    if (!orderManager_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "isTableIdentifierInUse", "OrderManager not initialized");
        return false;
    }
    
    try {
        bool inUse = orderManager_->isTableIdentifierInUse(tableIdentifier);
        LOG_KEY_VALUE(logger_, debug, "Table identifier " + tableIdentifier + " in use", LoggingUtils::boolToString(inUse));
        return inUse;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "isTableIdentifierInUse", e.what());
        return false;
    }
}

bool POSService::isValidTableIdentifier(const std::string& tableIdentifier) const {
    try {
        bool valid = Order::isValidTableIdentifier(tableIdentifier);
        LOG_KEY_VALUE(logger_, debug, "Table identifier " + tableIdentifier + " valid", LoggingUtils::boolToString(valid));
        return valid;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "isValidTableIdentifier", e.what());
        return false;
    }
}

// =====================================================================
// Current Order Management (ENHANCED to support quantity and instructions)
// =====================================================================

bool POSService::addItemToCurrentOrder(std::shared_ptr<MenuItem> item) {
    if (!item) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "addItemToCurrentOrder", "Null menu item provided");
        return false;
    }
    
    logger_.debug("[POSService] Adding item to current order (legacy): " + item->getName());
    // Legacy method - call enhanced version with default quantity and no instructions
    return addItemToCurrentOrder(*item, 1, "");
}

// ADDED: Enhanced method that supports quantity and special instructions
bool POSService::addItemToCurrentOrder(const MenuItem& item, int quantity, const std::string& instructions) {
    logger_.info("[POSService] Adding to current order: " + std::to_string(quantity) + "x " + item.getName());
    
    if (!item.isAvailable()) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "addItemToCurrentOrder", "Menu item not available: " + item.getName());
        return false;
    }
    
    if (!currentOrder_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "addItemToCurrentOrder", "No current order to add item to");
        return false;
    }
    
    if (quantity <= 0 || quantity > 99) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "addItemToCurrentOrder", "Invalid quantity: " + std::to_string(quantity));
        return false;
    }
    
    try {
        // Create OrderItem with the MenuItem, quantity, and special instructions
        OrderItem orderItem(item, quantity);
        if (!instructions.empty()) {
            orderItem.setSpecialInstructions(instructions);
            logger_.debug("[POSService] Special instructions added: " + instructions);
        }
        
        currentOrder_->addItem(orderItem);
        
        // Log the successful addition
        POSEvents::EventLogger::logOrderEvent(
            POSEvents::ORDER_ITEM_ADDED,
            currentOrder_,
            "Added " + std::to_string(quantity) + "x " + item.getName()
        );
        
        if (eventManager_) {
            // FIXED: Create JSON event manually
            Wt::Json::Object eventData;
            eventData["orderId"] = Wt::Json::Value(currentOrder_->getOrderId());
            eventData["tableIdentifier"] = Wt::Json::Value(currentOrder_->getTableIdentifier());
            eventData["status"] = Wt::Json::Value(static_cast<int>(currentOrder_->getStatus()));
            eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
            eventData["message"] = Wt::Json::Value("Order " + std::to_string(currentOrder_->getOrderId()) + " modified");
            
            eventManager_->publish(POSEvents::ORDER_ITEM_ADDED, eventData, "POSService");
            
            // Also call the UI callback if registered
            if (orderModifiedCallback_) {
                orderModifiedCallback_(currentOrder_);
            }
        }
        
        std::string logMsg = "Added " + std::to_string(quantity) + "x " + item.getName() + " to current order";
        if (!instructions.empty()) {
            logMsg += " (Note: " + instructions + ")";
        }
        logger_.info(logMsg);
        
        LOG_OPERATION_STATUS(logger_, "Add item to current order", true);
        return true;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "addItemToCurrentOrder", e.what());
        return false;
    }
}

bool POSService::removeItemFromCurrentOrder(size_t itemIndex) {
    logger_.info("[POSService] Removing item at index " + std::to_string(itemIndex) + " from current order");
    
    if (!currentOrder_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "removeItemFromCurrentOrder", "No current order");
        return false;
    }
    
    const auto& items = currentOrder_->getItems();
    if (itemIndex >= items.size()) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "removeItemFromCurrentOrder", 
                           "Invalid item index: " + std::to_string(itemIndex) + " (max: " + std::to_string(items.size()) + ")");
        return false;
    }
    
    try {
        // Log the item being removed
        const auto& itemToRemove = items[itemIndex];
        std::string itemName = itemToRemove.getMenuItem().getName();
        
        currentOrder_->removeItem(itemIndex);
        
        logger_.info("[POSService] Removed item: " + itemName);
        
        if (eventManager_) {
            // FIXED: Create JSON event manually
            Wt::Json::Object eventData;
            eventData["orderId"] = Wt::Json::Value(currentOrder_->getOrderId());
            eventData["tableIdentifier"] = Wt::Json::Value(currentOrder_->getTableIdentifier());
            eventData["status"] = Wt::Json::Value(static_cast<int>(currentOrder_->getStatus()));
            eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
            eventData["message"] = Wt::Json::Value("Order " + std::to_string(currentOrder_->getOrderId()) + " modified");
            
            eventManager_->publish(POSEvents::ORDER_ITEM_REMOVED, eventData, "POSService");
            
            // Also call the UI callback if registered
            if (orderModifiedCallback_) {
                orderModifiedCallback_(currentOrder_);
            }
        }
        
        LOG_OPERATION_STATUS(logger_, "Remove item from current order", true);
        return true;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "removeItemFromCurrentOrder", e.what());
        return false;
    }
}

bool POSService::updateCurrentOrderItemQuantity(size_t itemIndex, int newQuantity) {
    logger_.info("[POSService] Updating item quantity at index " + std::to_string(itemIndex) + 
                " to " + std::to_string(newQuantity));
    
    if (!currentOrder_ || newQuantity <= 0) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "updateCurrentOrderItemQuantity", 
                           "No current order or invalid quantity: " + std::to_string(newQuantity));
        return false;
    }
    
    const auto& items = currentOrder_->getItems();
    if (itemIndex >= items.size()) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "updateCurrentOrderItemQuantity", 
                           "Invalid item index: " + std::to_string(itemIndex));
        return false;
    }
    
    try {
        const auto& itemToUpdate = items[itemIndex];
        std::string itemName = itemToUpdate.getMenuItem().getName();
        int oldQuantity = itemToUpdate.getQuantity();
        
        currentOrder_->updateItemQuantity(itemIndex, newQuantity);
        
        logger_.info("[POSService] Updated " + itemName + " quantity from " + 
                    std::to_string(oldQuantity) + " to " + std::to_string(newQuantity));
        
        if (eventManager_) {
            // FIXED: Create JSON event manually
            Wt::Json::Object eventData;
            eventData["orderId"] = Wt::Json::Value(currentOrder_->getOrderId());
            eventData["tableIdentifier"] = Wt::Json::Value(currentOrder_->getTableIdentifier());
            eventData["status"] = Wt::Json::Value(static_cast<int>(currentOrder_->getStatus()));
            eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
            eventData["message"] = Wt::Json::Value("Order " + std::to_string(currentOrder_->getOrderId()) + " modified");
            
            eventManager_->publish(POSEvents::ORDER_MODIFIED, eventData, "POSService");
            
            // Also call the UI callback if registered
            if (orderModifiedCallback_) {
                orderModifiedCallback_(currentOrder_);
            }
        }
        
        LOG_OPERATION_STATUS(logger_, "Update item quantity", true);
        return true;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "updateCurrentOrderItemQuantity", e.what());
        return false;
    }
}

bool POSService::sendCurrentOrderToKitchen() {
    logger_.info("[POSService] Sending current order to kitchen");
    
    if (!currentOrder_ || !kitchenInterface_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "sendCurrentOrderToKitchen", 
                           "No current order or kitchen interface not available");
        return false;
    }
    
    if (currentOrder_->getItems().empty()) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "sendCurrentOrderToKitchen", "Cannot send empty order to kitchen");
        return false;
    }
    
    try {
        int orderId = currentOrder_->getOrderId();
        
        // Send order to kitchen using the KitchenInterface
        bool success = kitchenInterface_->sendOrderToKitchen(currentOrder_);
        
        if (success) {
            // Update order status
            currentOrder_->setStatus(Order::SENT_TO_KITCHEN);
            
            logger_.info("[POSService] Order #" + std::to_string(orderId) + " sent to kitchen successfully");
            
            if (eventManager_) {
                // FIXED: Create JSON event manually
                Wt::Json::Object eventData;
                eventData["orderId"] = Wt::Json::Value(currentOrder_->getOrderId());
                eventData["tableIdentifier"] = Wt::Json::Value(currentOrder_->getTableIdentifier());
                eventData["status"] = Wt::Json::Value(static_cast<int>(currentOrder_->getStatus()));
                eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                    std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
                eventData["message"] = Wt::Json::Value("Order " + std::to_string(currentOrder_->getOrderId()) + " sent to kitchen");
                
                eventManager_->publish(POSEvents::ORDER_SENT_TO_KITCHEN, eventData, "POSService");
            }
            
            // Clear current order after sending to kitchen
            setCurrentOrder(nullptr); // This will trigger CURRENT_ORDER_CHANGED event
            
            LOG_OPERATION_STATUS(logger_, "Send order to kitchen", true);
        } else {
            LOG_COMPONENT_ERROR(logger_, "POSService", "sendCurrentOrderToKitchen", 
                               "Kitchen interface failed to accept order");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "sendCurrentOrderToKitchen", e.what());
        return false;
    }
}

void POSService::clearCurrentOrder() {
    logger_.info("[POSService] Clearing current order");

    if (currentOrder_) {
        int orderId = currentOrder_->getOrderId();
        currentOrder_ = nullptr;

        // Notify listeners
        if (eventManager_) {
            eventManager_->publish(POSEvents::CURRENT_ORDER_CHANGED,
                std::any(), "POSService");
        }

        logger_.info("[POSService] Current order cleared (was Order #" + std::to_string(orderId) + ")");
    }
}

bool POSService::cancelCurrentOrder() {
    logger_.info("[POSService] Cancelling current order");

    if (!currentOrder_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "cancelCurrentOrder", "No current order to cancel");
        return false;
    }

    try {
        int orderId = currentOrder_->getOrderId();

        // Update order status to cancelled
        currentOrder_->setStatus(Order::CANCELLED);

        // Notify listeners
        if (eventManager_) {
            eventManager_->publish(POSEvents::ORDER_CANCELLED,
                std::any(orderId), "POSService");
        }

        // Clear the current order
        currentOrder_ = nullptr;

        // Publish current order changed event
        if (eventManager_) {
            eventManager_->publish(POSEvents::CURRENT_ORDER_CHANGED,
                std::any(), "POSService");
        }

        logger_.info("[POSService] Order #" + std::to_string(orderId) + " cancelled successfully");
        return true;

    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "cancelCurrentOrder", e.what());
        return false;
    }
}

// =====================================================================
// Kitchen Interface Methods
// =====================================================================

std::vector<KitchenInterface::KitchenTicket> POSService::getKitchenTickets() const {
    logger_.debug("[POSService] Retrieving kitchen tickets");
    
    if (!kitchenInterface_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getKitchenTickets", "Kitchen interface not available");
        return {};
    }
    
    try {
        auto tickets = kitchenInterface_->getActiveTickets();
        LOG_KEY_VALUE(logger_, debug, "Kitchen tickets found", tickets.size());
        return tickets;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getKitchenTickets", e.what());
        return {};
    }
}

int POSService::getEstimatedWaitTime() const {
    logger_.debug("[POSService] Getting estimated wait time");
    
    if (!kitchenInterface_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getEstimatedWaitTime", "Kitchen interface not available");
        return 0;
    }
    
    try {
        int waitTime = kitchenInterface_->getEstimatedWaitTime();
        LOG_KEY_VALUE(logger_, debug, "Estimated wait time (minutes)", waitTime);
        return waitTime;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getEstimatedWaitTime", e.what());
        return 0;
    }
}

Wt::Json::Object POSService::getKitchenQueueStatus() const {
    logger_.debug("[POSService] Getting kitchen queue status");
    
    if (!kitchenInterface_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getKitchenQueueStatus", "Kitchen interface not available");
        Wt::Json::Object emptyStatus;
        emptyStatus["queueSize"] = 0;
        emptyStatus["estimatedWaitTime"] = 0;
        return emptyStatus;
    }
    
    try {
        auto status = kitchenInterface_->getKitchenQueueStatus();
        logger_.debug("[POSService] Kitchen queue status retrieved");
        return status;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getKitchenQueueStatus", e.what());
        Wt::Json::Object emptyStatus;
        emptyStatus["queueSize"] = 0;
        emptyStatus["estimatedWaitTime"] = 0;
        return emptyStatus;
    }
}

// =====================================================================
// Menu Management Methods (ENHANCED with event publishing)
// =====================================================================

std::vector<std::shared_ptr<MenuItem>> POSService::getMenuItems() const {
    logger_.debug("[POSService] Retrieving all menu items");
    LOG_KEY_VALUE(logger_, debug, "Total menu items available", menuItems_.size());
    return menuItems_;
}

std::vector<std::shared_ptr<MenuItem>> POSService::getMenuItemsByCategory(MenuItem::Category category) const {
    logger_.debug("[POSService] Retrieving menu items by category");
    
    std::vector<std::shared_ptr<MenuItem>> categoryItems;
    
    for (const auto& item : menuItems_) {
        if (item && item->getCategory() == category) {
            categoryItems.push_back(item);
        }
    }
    
    LOG_KEY_VALUE(logger_, debug, "Items found in category", categoryItems.size());
    return categoryItems;
}

// ADDED: Method to find menu item by ID (useful for MenuDisplay)
std::shared_ptr<MenuItem> POSService::getMenuItemById(int itemId) const {
    logger_.debug("[POSService] Looking up menu item ID: " + std::to_string(itemId));
    
    auto it = std::find_if(menuItems_.begin(), menuItems_.end(),
        [itemId](const std::shared_ptr<MenuItem>& item) {
            return item && item->getId() == itemId;
        });
    
    bool found = (it != menuItems_.end());
    LOG_KEY_VALUE(logger_, debug, "Menu item " + std::to_string(itemId) + " found", LoggingUtils::boolToString(found));
    
    return found ? *it : nullptr;
}

// ADDED: Method to refresh menu and publish event
void POSService::refreshMenu() {
    logger_.info("[POSService] Refreshing menu");
    
    // In a real implementation, this would reload from database
    // For now, just trigger the event
    if (eventManager_) {
        // FIXED: Create JSON event manually instead of calling non-existent function
        Wt::Json::Object eventData;
        eventData["itemCount"] = Wt::Json::Value(static_cast<int>(menuItems_.size()));
        eventData["reason"] = Wt::Json::Value("refresh");
        eventData["menuVersion"] = Wt::Json::Value("1.0");
        eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        eventData["message"] = Wt::Json::Value("Menu updated: refresh (" + std::to_string(menuItems_.size()) + " items)");
        
        eventManager_->publish(POSEvents::MENU_UPDATED, eventData, "POSService");
    }
    
    LOG_KEY_VALUE(logger_, info, "Menu refreshed with items", menuItems_.size());
}

// =====================================================================
// Payment Processing Methods
// =====================================================================

PaymentProcessor::PaymentResult POSService::processPayment(
    std::shared_ptr<Order> order,
    PaymentProcessor::PaymentMethod method,
    double amount,
    double tipAmount) {
    
    if (!order) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "processPayment", "Null order provided");
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "Invalid order";
        return result;
    }
    
    logger_.info("[POSService] Processing payment for order " + std::to_string(order->getOrderId()) + 
                ", amount: $" + LoggingUtils::toString(amount) + 
                ", method: " + POSEvents::EventLogger::paymentMethodToString(method));
    
    if (!paymentProcessor_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "processPayment", "Payment processor not available");
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "Payment processor not available";
        return result;
    }
    
    try {
        auto result = paymentProcessor_->processPayment(order, method, amount, tipAmount);
        
        // Log payment result
        POSEvents::EventLogger::logPaymentEvent(
            result.success ? POSEvents::PAYMENT_COMPLETED : POSEvents::PAYMENT_FAILED,
            result,
            order
        );
        
        if (result.success && eventManager_) {
            // FIXED: Create JSON event manually instead of calling non-existent function
            Wt::Json::Object eventData;
            eventData["orderId"] = Wt::Json::Value(order->getOrderId());
            eventData["paymentSuccess"] = Wt::Json::Value(result.success);
            eventData["paymentAmount"] = Wt::Json::Value(result.amountProcessed);
            eventData["paymentMethod"] = Wt::Json::Value(static_cast<int>(result.method));
            eventData["transactionId"] = Wt::Json::Value(result.transactionId);
            eventData["timestamp"] = Wt::Json::Value(static_cast<int64_t>(
                std::chrono::system_clock::to_time_t(result.timestamp)));
            eventData["message"] = Wt::Json::Value("Payment completed for order " + std::to_string(order->getOrderId()));
            
            eventManager_->publish(POSEvents::PAYMENT_COMPLETED, eventData, "POSService");
        }
        
        LOG_OPERATION_STATUS(logger_, "Payment processing", result.success);
        return result;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "processPayment", e.what());
        
        PaymentProcessor::PaymentResult result;
        result.success = false;
        result.errorMessage = "Payment processing error: " + std::string(e.what());
        return result;
    }
}

std::vector<PaymentProcessor::PaymentResult> POSService::getTransactionHistory() const {
    logger_.debug("[POSService] Retrieving transaction history");
    
    if (!paymentProcessor_) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getTransactionHistory", "Payment processor not available");
        return {};
    }
    
    try {
        auto transactions = paymentProcessor_->getTransactionHistory();
        LOG_KEY_VALUE(logger_, debug, "Transactions found", transactions.size());
        return transactions;
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "getTransactionHistory", e.what());
        return {};
    }
}

// =====================================================================
// Configuration Methods
// =====================================================================

std::vector<std::string> POSService::getEnabledPaymentMethods() const {
    logger_.debug("[POSService] Getting enabled payment methods");
    
    // Return default enabled payment methods
    std::vector<std::string> methods = {"cash", "credit_card", "debit_card", "mobile_pay"};
    LOG_KEY_VALUE(logger_, debug, "Payment methods available", methods.size());
    return methods;
}

std::vector<double> POSService::getTipSuggestions() const {
    logger_.debug("[POSService] Getting tip suggestions");
    
    // Return default tip suggestions (as decimal percentages)
    std::vector<double> suggestions = {0.15, 0.18, 0.20, 0.25};
    LOG_KEY_VALUE(logger_, debug, "Tip suggestions available", suggestions.size());
    return suggestions;
}

// =====================================================================
// Initialization and Callback Registration Methods
// =====================================================================

void POSService::initializeMenu() {
    logger_.info("[POSService] Initializing menu via external call");
    
    // This method is called by RestaurantPOSApp to initialize the menu
    initializeMenuItems();
    refreshMenu(); // Trigger menu updated event
    
    LOG_OPERATION_STATUS(logger_, "Menu initialization", true);
}

void POSService::onOrderCreated(std::function<void(std::shared_ptr<Order>)> callback) {
    orderCreatedCallback_ = callback;
    logger_.info("[POSService] Order created callback registered");
}

void POSService::onOrderModified(std::function<void(std::shared_ptr<Order>)> callback) {
    orderModifiedCallback_ = callback;
    logger_.info("[POSService] Order modified callback registered");
}

// =====================================================================
// Helper Methods
// =====================================================================

std::vector<std::string> POSService::convertOrderItemsToStringList(const std::vector<OrderItem>& items) const {
    logger_.debug("[POSService] Converting " + std::to_string(items.size()) + " order items to string list");
    
    std::vector<std::string> itemList;
    
    try {
        for (const auto& item : items) {
            std::string itemString = std::to_string(item.getQuantity()) + "x " + 
                                    item.getMenuItem().getName();
            
            if (!item.getSpecialInstructions().empty()) {
                itemString += " (" + item.getSpecialInstructions() + ")";
            }
            
            itemList.push_back(itemString);
        }
        
        LOG_KEY_VALUE(logger_, debug, "Order items converted", itemList.size());
        
    } catch (const std::exception& e) {
        LOG_COMPONENT_ERROR(logger_, "POSService", "convertOrderItemsToStringList", e.what());
    }
    
    return itemList;
}
