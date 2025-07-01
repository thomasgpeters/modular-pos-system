//=============================================================================

#ifndef CATEGORYPOPOVER_H
#define CATEGORYPOPOVER_H

#include "../../MenuItem.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"

#include <Wt/WPopupWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>

#include <memory>
#include <vector>
#include <functional>

/**
 * @file CategoryPopover.hpp
 * @brief Category selection popup for menu browsing
 * 
 * This popover displays menu items within a specific category
 * and allows users to select items to add to their order.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @class CategoryPopover
 * @brief Popup widget for displaying menu items by category
 * 
 * The CategoryPopover provides an overlay interface for browsing
 * menu items within a specific category with item selection capabilities.
 */
class CategoryPopover : public Wt::WPopupWidget {
public:
    /**
     * @brief Item selection callback type
     */
    using ItemSelectionCallback = std::function<void(std::shared_ptr<MenuItem>)>;
    
    /**
     * @brief Constructs a category popover
     * @param category Menu category to display
     * @param items Menu items in the category
     * @param eventManager Event manager for notifications
     * @param callback Callback function for item selection
     */
    CategoryPopover(MenuItem::Category category,
                   const std::vector<std::shared_ptr<MenuItem>>& items,
                   std::shared_ptr<EventManager> eventManager,
                   ItemSelectionCallback callback = nullptr);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~CategoryPopover() = default;
    
    /**
     * @brief Shows the popover at a specific position
     * @param anchor Widget to anchor the popover to
     */
    void showPopover(Wt::WWidget* anchor);
    
    /**
     * @brief Updates the menu items displayed
     * @param items New menu items to display
     */
    void updateMenuItems(const std::vector<std::shared_ptr<MenuItem>>& items);
    
    /**
     * @brief Sets the maximum number of columns for item display
     * @param columns Maximum columns (default: 3)
     */
    void setMaxColumns(int columns);
    
    /**
     * @brief Enables or disables item descriptions
     * @param enabled True to show descriptions
     */
    void setShowDescriptions(bool enabled);

protected:
    /**
     * @brief Creates the popover content
     */
    void createPopoverContent();
    
    /**
     * @brief Creates the popover header
     * @return Container widget with header
     */
    std::unique_ptr<Wt::WContainerWidget> createHeader();
    
    /**
     * @brief Creates the menu items grid
     * @return Container widget with menu items
     */
    std::unique_ptr<Wt::WContainerWidget> createMenuItemsGrid();
    
    /**
     * @brief Creates a menu item card
     * @param item Menu item to create card for
     * @return Container widget representing the item
     */
    std::unique_ptr<Wt::WContainerWidget> createMenuItemCard(std::shared_ptr<MenuItem> item);
    
    /**
     * @brief Handles item selection
     * @param item Selected menu item
     */
    void onItemSelected(std::shared_ptr<MenuItem> item);
    
    /**
     * @brief Gets the category display name
     * @param category Menu category
     * @return Human-readable category name
     */
    std::string getCategoryDisplayName(MenuItem::Category category) const;
    
    /**
     * @brief Gets the category icon/emoji
     * @param category Menu category
     * @return Icon string for the category
     */
    std::string getCategoryIcon(MenuItem::Category category) const;

private:
    // Category and items
    MenuItem::Category category_;
    std::vector<std::shared_ptr<MenuItem>> menuItems_;
    std::shared_ptr<EventManager> eventManager_;
    ItemSelectionCallback selectionCallback_;
    
    // Display configuration
    int maxColumns_;
    bool showDescriptions_;
    
    // UI components
    Wt::WContainerWidget* contentContainer_;
    Wt::WContainerWidget* itemsContainer_;
    Wt::WText* headerText_;
    Wt::WPushButton* closeButton_;
    
    // Helper methods
    void setupStyling();
    void refreshItemsDisplay();
};

#endif // CATEGORYPOPOVER_H
