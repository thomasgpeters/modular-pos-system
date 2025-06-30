#ifndef MENUDISPLAY_H
#define MENUDISPLAY_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"
#include "../../MenuItem.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

#include <memory>
#include <vector>
#include <map>

/**
 * @file MenuDisplay.hpp
 * @brief Menu display component for the Restaurant POS System
 * 
 * This component manages the display of menu items, including category
 * organization and item selection functionality.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class MenuDisplay
 * @brief UI component for displaying and selecting menu items
 * 
 * The MenuDisplay component shows menu items organized by category and
 * provides functionality for adding items to the current order. It supports
 * both table view and category tiles view.
 */
class MenuDisplay : public Wt::WContainerWidget {
public:
    /**
     * @enum DisplayMode
     * @brief Display modes for the menu
     */
    enum DisplayMode {
        TABLE_VIEW,     ///< Traditional table view with all items
        CATEGORY_TILES  ///< Category tiles with popover selection
    };
    
    /**
     * @brief Constructs the menu display
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     */
    MenuDisplay(std::shared_ptr<POSService> posService,
               std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~MenuDisplay() = default;
    
    /**
     * @brief Refreshes the menu display from the service
     */
    void refresh();
    
    /**
     * @brief Sets the display mode
     * @param mode Display mode to use
     */
    void setDisplayMode(DisplayMode mode);
    
    /**
     * @brief Gets the current display mode
     * @return Current display mode
     */
    DisplayMode getDisplayMode() const;
    
    /**
     * @brief Filters menu items by category
     * @param category Category to show (use SPECIAL to show all)
     */
    void filterByCategory(MenuItem::Category category);
    
    /**
     * @brief Clears any category filter
     */
    void clearCategoryFilter();

protected:
    /**
     * @brief Initializes the UI components
     */
    void initializeUI();
    
    /**
     * @brief Sets up event listeners
     */
    void setupEventListeners();
    
    /**
     * @brief Builds the table view display
     */
    void buildTableView();
    
    /**
     * @brief Builds the category tiles display
     */
    void buildCategoryTilesView();
    
    /**
     * @brief Creates a category tile
     * @param category Category for the tile
     * @param itemCount Number of items in the category
     * @return Container widget for the tile
     */
    std::unique_ptr<Wt::WWidget> createCategoryTile(MenuItem::Category category, int itemCount);
    
    /**
     * @brief Shows category popover with items
     * @param category Category to show
     * @param items Items in the category
     */
    void showCategoryPopover(MenuItem::Category category, 
                           const std::vector<std::shared_ptr<MenuItem>>& items);

private:
    // Services and dependencies
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // Configuration
    DisplayMode displayMode_;
    MenuItem::Category categoryFilter_;
    bool hasFilter_;
    
    // UI components
    Wt::WContainerWidget* menuContainer_;
    Wt::WTable* menuTable_;
    Wt::WContainerWidget* categoryTilesContainer_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers
    void handleMenuUpdated(const std::any& eventData);
    void handleThemeChanged(const std::any& eventData);
    
    // UI action handlers
    void onMenuItemSelected(std::shared_ptr<MenuItem> item);
    void onCategoryTileClicked(MenuItem::Category category);
    
    // Helper methods
    std::vector<std::shared_ptr<MenuItem>> getFilteredMenuItems() const;
    std::map<MenuItem::Category, std::vector<std::shared_ptr<MenuItem>>> groupMenuItemsByCategory() const;
    std::string getCategoryIcon(MenuItem::Category category) const;
    std::string getCategoryDisplayName(MenuItem::Category category) const;
    void updateMenuTable();
    void updateCategoryTiles();
    void clearMenuContainer();
    
    // Formatting helpers
    std::string formatCurrency(double amount) const;
    std::string formatItemDescription(const std::shared_ptr<MenuItem>& item) const;
};

#endif // MENUDISPLAY_H
