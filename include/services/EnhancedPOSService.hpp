//============================================================================
// include/services/EnhancedPOSService.hpp - API-Integrated POS Service
//============================================================================

#ifndef ENHANCEDPOSSERVICE_H
#define ENHANCEDPOSSERVICE_H

#include "../Order.hpp"
#include "../Employee.hpp"
#include "../MenuItem.hpp"
#include "../OrderManager.hpp"
#include "../KitchenInterface.hpp"
#include "../PaymentProcessor.hpp"
#include "../events/EventManager.hpp"
#include "../events/POSEvents.hpp"

// IMPORTANT: Include base class BEFORE declaring derived class
#include "POSService.hpp"

// API Components
#include "../api/APIClient.hpp"
#include "../api/repositories/OrderRepository.hpp"
#include "../api/repositories/MenuItemRepository.hpp"
#include "../api/repositories/EmployeeRepository.hpp"

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <future>
#include <chrono>

/**
 * @class EnhancedPOSService
 * @brief API-integrated POS service with middleware persistence
 * 
 * This enhanced version extends POSService to replace local data storage 
 * with API calls to the middleware system, providing real persistence 
 * and multi-client synchronization.
 */
class EnhancedPOSService : public POSService {
public:
    /**
     * @brief Service configuration
     */
    struct ServiceConfig {
        std::string apiBaseUrl;
        std::string authToken;
        int apiTimeout;
        bool enableCaching;
        bool debugMode;
        
        // Default constructor with default values
        ServiceConfig() 
            : apiBaseUrl("http://localhost:5656/api")
            , authToken("")
            , apiTimeout(30)
            , enableCaching(true)
            , debugMode(false) {}
    };
    
    /**
     * @brief Constructs the enhanced POS service
     * @param eventManager Event manager for notifications
     * @param config Service configuration
     */
    explicit EnhancedPOSService(std::shared_ptr<EventManager> eventManager,
                               const ServiceConfig& config = ServiceConfig());
    
    /**
     * @brief Virtual destructor
     */
    virtual ~EnhancedPOSService() = default;
    
    /**
     * @brief Initializes the service and API connections
     * @return True if initialization successful
     */
    bool initialize();
    
    // =================================================================
    // Enhanced Methods (do not override - base class methods are not virtual)
    // These methods enhance the base class functionality with API integration
    // =================================================================
    
    /**
     * @brief Enhanced: Creates a new order (persisted to API)
     * Note: This enhances the base class method with API persistence
     */
    std::shared_ptr<Order> createOrder(const std::string& tableIdentifier);
    
    /**
     * @brief Enhanced: Gets all active orders from API
     * Note: This enhances the base class method with API data
     */
    std::vector<std::shared_ptr<Order>> getActiveOrders();
    
    /**
     * @brief Enhanced: Gets current order
     * Note: This enhances the base class method with API sync
     */
    std::shared_ptr<Order> getCurrentOrder() const;
    
    /**
     * @brief Enhanced: Sets current order
     * Note: This enhances the base class method with API sync
     */
    void setCurrentOrder(std::shared_ptr<Order> order);
    
    /**
     * @brief Enhanced: Gets menu items from API (with caching)
     * Note: This enhances the base class method with API data
     */
    std::vector<std::shared_ptr<MenuItem>> getMenuItems();
    
    /**
     * @brief Enhanced: Gets menu item by ID (cached)
     * Note: This enhances the base class method with API data
     */
    std::shared_ptr<MenuItem> getMenuItemById(int itemId);
    
    // =================================================================
    // Enhanced Async Methods (API-Backed)
    // =================================================================
    
    /**
     * @brief Creates a new order (persisted to API) - Async version
     * @param tableIdentifier Table identifier
     * @param callback Callback with created order
     */
    void createOrderAsync(const std::string& tableIdentifier,
                         std::function<void(std::shared_ptr<Order>, bool)> callback = nullptr);
    
    /**
     * @brief Gets all active orders from API - Async version
     * @param callback Callback with order list
     */
    void getActiveOrdersAsync(std::function<void(std::vector<std::shared_ptr<Order>>, bool)> callback = nullptr);
    
    /**
     * @brief Gets order by ID from API
     * @param orderId Order ID to retrieve
     * @param callback Callback with order
     */
    void getOrderByIdAsync(int orderId,
                          std::function<void(std::shared_ptr<Order>, bool)> callback = nullptr);
    
    /**
     * @brief Gets orders by table identifier
     * @param tableIdentifier Table identifier
     * @param callback Callback with order list
     */
    void getOrdersByTableIdentifierAsync(const std::string& tableIdentifier,
                                        std::function<void(std::vector<std::shared_ptr<Order>>, bool)> callback = nullptr);
    
    /**
     * @brief Updates order status in API
     * @param orderId Order ID
     * @param status New status
     * @param callback Callback with success status
     */
    void updateOrderStatusAsync(int orderId, Order::Status status,
                               std::function<void(bool)> callback = nullptr);
    
    /**
     * @brief Cancels order in API
     * @param orderId Order ID to cancel
     * @param callback Callback with success status
     */
    void cancelOrderAsync(int orderId,
                         std::function<void(bool)> callback = nullptr);
    
    /**
     * @brief Sends order to kitchen via API
     * @param orderId Order ID
     * @param callback Callback with success status
     */
    void sendOrderToKitchenAsync(int orderId,
                                std::function<void(bool)> callback = nullptr);
    
    // =================================================================
    // Current Order Management (Hybrid Local/API)
    // =================================================================
    
    /**
     * @brief Adds item to current order (local + API save)
     * @param item Menu item to add
     * @param quantity Quantity to add
     * @param instructions Special instructions
     * @param callback Callback with success status
     */
    void addItemToCurrentOrderAsync(const MenuItem& item, int quantity = 1, 
                                   const std::string& instructions = "",
                                   std::function<void(bool)> callback = nullptr);
    
    /**
     * @brief Removes item from current order
     * @param itemIndex Index of item to remove
     * @param callback Callback with success status
     */
    void removeItemFromCurrentOrderAsync(size_t itemIndex,
                                        std::function<void(bool)> callback = nullptr);
    
    /**
     * @brief Updates item quantity in current order
     * @param itemIndex Item index
     * @param newQuantity New quantity
     * @param callback Callback with success status
     */
    void updateCurrentOrderItemQuantityAsync(size_t itemIndex, int newQuantity,
                                            std::function<void(bool)> callback = nullptr);
    
    /**
     * @brief Saves current order to API
     * @param callback Callback with success status
     */
    void saveCurrentOrderAsync(std::function<void(bool)> callback = nullptr);
    
    /**
     * @brief Sends current order to kitchen
     * @param callback Callback with success status
     */
    void sendCurrentOrderToKitchenAsync(std::function<void(bool)> callback = nullptr);
    
    // =================================================================
    // Menu Management (API-Backed with Caching)
    // =================================================================
    
    /**
     * @brief Gets all menu items from API (with caching)
     * @param forceRefresh Force API refresh, ignore cache
     * @param callback Callback with menu items
     */
    void getMenuItemsAsync(bool forceRefresh = false,
                          std::function<void(std::vector<std::shared_ptr<MenuItem>>, bool)> callback = nullptr);
    
    /**
     * @brief Gets menu items by category (cached)
     * @param category Menu category
     * @param callback Callback with filtered items
     */
    void getMenuItemsByCategoryAsync(MenuItem::Category category,
                                    std::function<void(std::vector<std::shared_ptr<MenuItem>>, bool)> callback = nullptr);
    
    /**
     * @brief Refreshes menu cache from API
     * @param callback Callback with success status
     */
    void refreshMenuCacheAsync(std::function<void(bool)> callback = nullptr);
    
    /**
     * @brief Gets available menu items only
     * @param callback Callback with available items
     */
    void getAvailableMenuItemsAsync(std::function<void(std::vector<std::shared_ptr<MenuItem>>, bool)> callback = nullptr);
    
    // =================================================================
    // Employee Management (API-Backed)
    // =================================================================
    
    /**
     * @brief Gets all employees from API
     * @param callback Callback with employee list
     */
    void getEmployeesAsync(std::function<void(std::vector<Employee>, bool)> callback = nullptr);
    
    /**
     * @brief Gets employees by role
     * @param role Employee role
     * @param callback Callback with filtered employees
     */
    void getEmployeesByRoleAsync(const std::string& role,
                                std::function<void(std::vector<Employee>, bool)> callback = nullptr);
    
    /**
     * @brief Gets active employees only
     * @param callback Callback with active employees
     */
    void getActiveEmployeesAsync(std::function<void(std::vector<Employee>, bool)> callback = nullptr);
    
    // =================================================================
    // Kitchen Interface (Enhanced with API Integration)
    // =================================================================
    
    /**
     * @brief Gets kitchen tickets from API
     * @param callback Callback with kitchen tickets
     */
    void getKitchenTicketsAsync(std::function<void(std::vector<KitchenInterface::KitchenTicket>, bool)> callback = nullptr);
    
    /**
     * @brief Gets estimated wait time from API
     * @param callback Callback with wait time
     */
    void getEstimatedWaitTimeAsync(std::function<void(int, bool)> callback = nullptr);
    
    /**
     * @brief Gets kitchen queue status from API
     * @param callback Callback with queue status
     */
    void getKitchenQueueStatusAsync(std::function<void(Wt::Json::Object, bool)> callback = nullptr);
    
    // =================================================================
    // Payment Processing (API Integration)
    // =================================================================
    
    /**
     * @brief Processes payment via API
     * @param order Order to process payment for
     * @param method Payment method
     * @param amount Payment amount
     * @param tipAmount Tip amount
     * @param callback Callback with payment result
     */
    void processPaymentAsync(std::shared_ptr<Order> order,
                            PaymentProcessor::PaymentMethod method,
                            double amount, double tipAmount = 0.0,
                            std::function<void(PaymentProcessor::PaymentResult, bool)> callback = nullptr);
    
    /**
     * @brief Gets transaction history from API
     * @param callback Callback with transaction list
     */
    void getTransactionHistoryAsync(std::function<void(std::vector<PaymentProcessor::PaymentResult>, bool)> callback = nullptr);
    
    // =================================================================
    // Configuration and Utilities
    // =================================================================
    
    /**
     * @brief Gets service configuration
     */
    const ServiceConfig& getConfig() const { return config_; }
    
    /**
     * @brief Updates API authentication token
     * @param token New auth token
     */
    void setAuthToken(const std::string& token);
    
    /**
     * @brief Checks if service is connected to API
     * @return True if connected
     */
    bool isConnected() const { return initialized_; }
    
    /**
     * @brief Gets API client for advanced operations
     * @return Shared pointer to API client
     */
    std::shared_ptr<APIClient> getAPIClient() const { return apiClient_; }
    
    /**
     * @brief Clears all local caches
     */
    void clearCaches();

protected:
    /**
     * @brief Initializes API client and repositories
     */
    void initializeAPIComponents();
    
    /**
     * @brief Initializes local caches
     */
    void initializeCaches();
    
    /**
     * @brief Handles API connection errors
     * @param operation Operation that failed
     * @param error Error message
     */
    void handleAPIError(const std::string& operation, const std::string& error);
    
    /**
     * @brief Publishes event to event manager
     * @param eventType Event type
     * @param eventData Event data
     */
    void publishEvent(const std::string& eventType, const Wt::Json::Object& eventData);

private:
    // Enhanced service configuration
    ServiceConfig config_;
    bool initialized_;
    
    // API components
    std::shared_ptr<APIClient> apiClient_;
    std::unique_ptr<OrderRepository> orderRepository_;
    std::unique_ptr<MenuItemRepository> menuItemRepository_;
    std::unique_ptr<EmployeeRepository> employeeRepository_;
    
    // Local current order tracking (enhances base class)
    std::shared_ptr<Order> currentOrder_;
    
    // Caches
    std::vector<std::shared_ptr<MenuItem>> menuItemsCache_;
    std::map<int, std::shared_ptr<MenuItem>> menuItemByIdCache_;
    std::chrono::system_clock::time_point menuCacheTime_;
    bool menuCacheValid_;
    
    // Cache management
    static constexpr int CACHE_TIMEOUT_MINUTES = 5;
    
    // Helper methods
    bool isMenuCacheExpired() const;
    void updateMenuCache(const std::vector<std::shared_ptr<MenuItem>>& items);
    std::shared_ptr<MenuItem> convertFromAPIMenuItem(const MenuItem& apiItem);
};

#endif // ENHANCEDPOSSERVICE_H
