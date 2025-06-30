#ifndef RESTAURANTPOSAPP_H
#define RESTAURANTPOSAPP_H

#include "../include/OrderManager.hpp"
#include "../include/PaymentProcessor.hpp"
#include "../include/KitchenInterface.hpp"
#include "../include/MenuItem.hpp"

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WSpinBox.h>
#include <Wt/WTimer.h>
#include <Wt/WEnvironment.h>
#include <Wt/WText.h>
#include <Wt/WComboBox.h>

#include <memory>
#include <vector>
#include <string>
#include <map>

/**
 * @file RestaurantPOSApp.h
 * @brief Main application class with user preferences system
 */

/**
 * @class RestaurantPOSApp
 * @brief Main application class with user preferences and theme support
 */
class RestaurantPOSApp : public Wt::WApplication {
public:
    /**
     * @enum MenuViewType
     * @brief Menu display options
     */
    enum MenuViewType {
        LIST_VIEW,      ///< Traditional table list view
        TILES_VIEW      ///< Category tiles with pop-overs
    };

    /**
     * @brief Simple theme information structure
     */
    struct ThemeInfo {
        std::string id;                      ///< Theme identifier
        std::string name;                    ///< Display name
        std::string description;             ///< Theme description
        std::string cssFile;                 ///< Path to CSS file
        std::string externalCss;             ///< External CSS URL (optional)
        std::vector<std::string> previewColors; ///< Colors for preview
        bool isDefault;                      ///< Whether this is the default theme
        
        // Constructor
        ThemeInfo() : isDefault(false) {}
        
        ThemeInfo(const std::string& id, const std::string& name, 
                 const std::string& description, const std::string& cssFile,
                 const std::string& externalCss = "",
                 const std::vector<std::string>& previewColors = {},
                 bool isDefault = false)
            : id(id), name(name), description(description), cssFile(cssFile),
              externalCss(externalCss), previewColors(previewColors), isDefault(isDefault) {}
    };

    /**
     * @brief User preferences structure
     */
    struct UserPreferences {
        std::string selectedTheme;           ///< Current theme ID
        MenuViewType menuView;               ///< Menu display type
        bool showMenuDescriptions;           ///< Show item descriptions
        bool showMenuCategories;             ///< Group by categories
        bool autoRefresh;                    ///< Auto-refresh active orders
        int refreshInterval;                 ///< Refresh interval in seconds
        
        // Constructor with defaults
        UserPreferences() 
            : selectedTheme("bootstrap")
            , menuView(LIST_VIEW)
            , showMenuDescriptions(false)
            , showMenuCategories(true)
            , autoRefresh(true)
            , refreshInterval(5) {}
    };

    /**
     * @brief Constructs the main POS application
     * @param env Wt web environment containing request information
     */
    RestaurantPOSApp(const Wt::WEnvironment& env);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~RestaurantPOSApp() = default;

private:
    // Preferences management
    /**
     * @brief Initializes hardcoded theme configuration
     */
    void initializeHardcodedThemes();
    
    /**
     * @brief Shows the user preferences dialog
     */
    void showPreferencesDialog();
    
    /**
     * @brief Applies user preferences to the interface
     */
    void applyUserPreferences();
    
    /**
     * @brief Applies the specified theme to the application
     * @param themeName Theme identifier to apply
     */
    void applyTheme(const std::string& themeName);
    
    /**
     * @brief Updates the menu display based on current view type
     */
    void updateMenuDisplay();
    
    /**
     * @brief Builds category tiles view
     */
    void buildCategoryTiles();
    
    /**
     * @brief Shows category pop-over with menu items
     * @param category Category to display
     * @param categoryName Display name for the category
     */
    void showCategoryPopover(MenuItem::Category category, const std::string& categoryName);
    
    /**
     * @brief Triggers a UI update to apply changes
     */
    void triggerUpdate();

    // Initialization methods
    /**
     * @brief Initializes sample menu items for demonstration
     */
    void initializeSampleMenu();
    
    /**
     * @brief Builds the main user interface
     */
    void buildMainInterface();
    
    /**
     * @brief Creates the order entry panel (left side)
     * @return Unique pointer to the order entry widget
     */
    std::unique_ptr<Wt::WWidget> createOrderEntryPanel();
    
    /**
     * @brief Creates the order status panel (right side)
     * @return Unique pointer to the order status widget
     */
    std::unique_ptr<Wt::WWidget> createOrderStatusPanel();
    
    // UI building methods
    /**
     * @brief Builds the menu items table (list view)
     */
    void buildMenuTable();
    
    /**
     * @brief Adds a menu item to the table at the specified row
     * @param item Menu item to add
     * @param row Row number in the table
     */
    void addMenuItemToTable(std::shared_ptr<MenuItem> item, int row);
    
    /**
     * @brief Updates the current order display table
     */
    void updateCurrentOrderTable();
    
    /**
     * @brief Updates the active orders display table
     */
    void updateActiveOrdersTable();
    
    /**
     * @brief Updates the kitchen status display
     */
    void updateKitchenStatusTable();
    
    // User interaction handlers
    /**
     * @brief Starts a new order for the selected table
     */
    void startNewOrder();
    
    /**
     * @brief Adds a menu item to the current order
     * @param menuItem Menu item to add
     */
    void addItemToCurrentOrder(std::shared_ptr<MenuItem> menuItem);
    
    /**
     * @brief Removes an item from the current order
     * @param index Index of the item to remove
     */
    void removeItemFromCurrentOrder(size_t index);
    
    /**
     * @brief Sends the current order to the kitchen
     */
    void sendCurrentOrderToKitchen();
    
    /**
     * @brief Shows the payment processing dialog
     */
    void showPaymentDialog();
    
    /**
     * @brief Processes payment for the current order
     * @param method Payment method to use
     * @param amount Payment amount
     * @param tipAmount Tip amount
     */
    void processPayment(PaymentProcessor::PaymentMethod method, 
                       double amount, double tipAmount = 0.0);
    
    /**
     * @brief Updates an order's kitchen status
     * @param orderId Order ID to update
     * @param status New kitchen status
     */
    void updateOrderKitchenStatus(int orderId, KitchenInterface::KitchenStatus status);
    
    // Real-time updates
    /**
     * @brief Sets up automatic real-time updates
     */
    void setupRealTimeUpdates();
    
    /**
     * @brief Performs periodic UI updates
     */
    void performPeriodicUpdate();
    
    // Utility methods
    /**
     * @brief Converts order status to display string
     * @param status Order status to convert
     * @return Human-readable status string
     */
    std::string getStatusString(Order::Status status);
    
    /**
     * @brief Converts kitchen status to display string
     * @param status Kitchen status to convert
     * @return Human-readable status string
     */
    std::string getKitchenStatusString(KitchenInterface::KitchenStatus status);
    
    /**
     * @brief Formats currency amount for display
     * @param amount Amount to format
     * @return Formatted currency string
     */
    std::string formatCurrency(double amount);
    
    /**
     * @brief Shows a notification to the user
     * @param message Notification message
     * @param type Notification type (success, warning, error)
     */
    void showNotification(const std::string& message, const std::string& type = "info");
    
    // Core components (the three legs)
    std::unique_ptr<OrderManager> orderManager_;        ///< Order management system
    std::unique_ptr<PaymentProcessor> paymentProcessor_; ///< Payment processing system
    std::unique_ptr<KitchenInterface> kitchenInterface_; ///< Kitchen communication system
    
    // User preferences and theme system
    UserPreferences userPreferences_;           ///< Current user preferences
    std::vector<ThemeInfo> availableThemes_;    ///< Available theme options
    std::string themeDirectory_;                ///< Directory containing theme files
    bool allowUserThemes_;                      ///< Whether to allow user-defined themes
    
    // UI components
    Wt::WSpinBox* tableNumberEdit_;         ///< Table number input
    Wt::WContainerWidget* menuContainer_;   ///< Container for menu display
    Wt::WTable* menuTable_;                 ///< Menu items display (list view)
    Wt::WContainerWidget* categoryTilesContainer_; ///< Category tiles container
    Wt::WTable* currentOrderTable_;         ///< Current order display
    Wt::WTable* activeOrdersTable_;         ///< Active orders display
    Wt::WTable* kitchenStatusTable_;        ///< Kitchen status display
    Wt::WTimer* updateTimer_;               ///< Real-time update timer
    
    // Application data
    std::vector<std::shared_ptr<MenuItem>> menuItems_;  ///< Available menu items
    std::shared_ptr<Order> currentOrder_;               ///< Currently active order
    
    // Configuration
    static constexpr int UPDATE_INTERVAL_SECONDS = 5;   ///< UI update interval
    static constexpr double DEFAULT_TIP_PERCENTAGE = 0.18; ///< Default tip percentage
};

/**
 * @brief Application factory function
 * @param env Wt web environment
 * @return Unique pointer to the created application
 */
std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env);

#endif // RESTAURANTPOSAPP_H
