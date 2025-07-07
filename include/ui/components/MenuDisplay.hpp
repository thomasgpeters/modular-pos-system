#ifndef MENUDISPLAY_H
#define MENUDISPLAY_H

#include "../../utils/UIStyleHelper.hpp" // Include our styling helper

#include "../../services/POSService.hpp"
#include "../../services/ThemeService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"
#include "../../MenuItem.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WSpinBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WGroupBox.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>

#include <memory>
#include <vector>
#include <map>

/**
 * @file MenuDisplay.hpp
 * @brief Menu display component for the Restaurant POS System
 * 
 * This component displays the restaurant menu with categories and allows
 * users to add items to the current order with quantity and special instructions.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0 - Enhanced with theme support
 */

/**
 * @class MenuDisplay
 * @brief UI component for displaying menu items and adding them to orders
 * 
 * The MenuDisplay shows menu items organized by categories, with functionality
 * to add items to the current order with specified quantities and special instructions.
 * It integrates with the theme system for consistent styling.
 */
class MenuDisplay : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the menu display
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     * @param themeService Optional theme service for styling support
     */
    MenuDisplay(std::shared_ptr<POSService> posService,
                std::shared_ptr<EventManager> eventManager,
                std::shared_ptr<ThemeService> themeService = nullptr);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~MenuDisplay() = default;
    
    /**
     * @brief Refreshes the menu display from the service
     */
    void refresh();
    
    /**
     * @brief Sets the current menu category filter
     * @param category Category to display, empty string shows all
     */
    void setCategory(const std::string& category);
    
    /**
     * @brief Gets the current category filter
     * @return Current category filter
     */
    std::string getCurrentCategory() const;
    
    /**
     * @brief Sets whether the menu is in selection mode
     * @param enabled True to enable item selection, false for display only
     */
    void setSelectionEnabled(bool enabled);
    
    /**
     * @brief Checks if selection mode is enabled
     * @return True if selection is enabled
     */
    bool isSelectionEnabled() const;
    
    /**
     * @brief Gets all available menu categories
     * @return Vector of category names
     */
    std::vector<std::string> getAvailableCategories() const;

protected:
    /**
     * @brief Initializes the UI components
     */
    void initializeUI();
    
    /**
     * @brief Initializes table headers
     */
    void initializeTableHeaders();
    
    /**
     * @brief Sets up event listeners
     */
    void setupEventListeners();
    
    /**
     * @brief Creates the menu header with category filter
     * @return Container widget with header controls
     */
    std::unique_ptr<Wt::WWidget> createMenuHeader();
    
    /**
     * @brief Updates the menu items table with current category filter
     */
    void updateMenuItemsTable();
    
    /**
     * @brief Adds a menu item row to the table
     * @param item Menu item to add
     * @param index Index for event handling
     */
    void addMenuItemRow(const std::shared_ptr<MenuItem>& item, size_t index);

private:
    // Services and dependencies
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    std::shared_ptr<ThemeService> themeService_;
    
    // Configuration
    std::string currentCategory_;
    bool selectionEnabled_;
    
    // UI components
    Wt::WGroupBox* menuGroup_;
    Wt::WComboBox* categoryCombo_;
    Wt::WText* itemCountText_;
    Wt::WTable* itemsTable_;
    Wt::WContainerWidget* headerContainer_;
    
    // Menu data cache
    std::vector<std::shared_ptr<MenuItem>> menuItems_;
    std::vector<std::string> categories_;
    std::map<std::string, std::vector<std::shared_ptr<MenuItem>>> itemsByCategory_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers
    void handleMenuUpdated(const std::any& eventData);
    void handleCurrentOrderChanged(const std::any& eventData);
    void handleThemeChanged(const std::any& eventData);
    
    // UI action handlers
    void onCategoryChanged();
    void onAddToOrderClicked(const std::shared_ptr<MenuItem>& item, int quantity, const std::string& instructions);
    void onItemRowClicked(const std::shared_ptr<MenuItem>& item);
    
    // Business logic methods
    void addItemToCurrentOrder(const MenuItem& item, int quantity, const std::string& instructions);
    bool canAddToOrder() const;
    void showAddToOrderDialog(const std::shared_ptr<MenuItem>& item);
    
    // Data management methods
    void loadMenuItems();
    void organizeItemsByCategory();
    void populateCategoryCombo();
    std::vector<std::shared_ptr<MenuItem>> getFilteredItems() const;
    
    // Helper methods
    std::string formatCurrency(double amount) const;
    std::string formatItemDescription(const std::shared_ptr<MenuItem>& item) const;
    void updateItemCount();
    void showMessage(const std::string& message, const std::string& type = "info");
    
    // Validation methods
    bool validateQuantity(int quantity) const;
    bool validateSpecialInstructions(const std::string& instructions) const;
    
    // Styling methods
    void applyTheme();
    void applyTableStyling();
    void applyHeaderStyling();
    void updateRowStyling(int row, bool isEven);
    void applyItemRowStyling(int row, const std::shared_ptr<MenuItem>& item);
    
    // Constants
    static constexpr int MAX_QUANTITY = 99;
    static constexpr int MAX_INSTRUCTIONS_LENGTH = 200;
    static constexpr const char* ALL_CATEGORIES = "All Categories";
};

#endif // MENUDISPLAY_H
