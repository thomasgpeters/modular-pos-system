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
    
    /**
     * @brief Gets the maximum number of columns
     * @return Maximum columns
     */
    int getMaxColumns() const { return maxColumns_; }
    
    /**
     * @brief Gets whether descriptions are shown
     * @return True if descriptions are shown
     */
    bool getShowDescriptions() const { return showDescriptions_; }
    
    // =================================================================
    // MISSING METHODS - Adding these to fix compilation errors
    // =================================================================
    
    /**
     * @brief Sets the popover size
     * @param width Popover width
     * @param height Popover height
     */
    void setPopoverSize(int width, int height) {
        popoverWidth_ = width;
        popoverHeight_ = height;
        if (contentContainer_) {
            contentContainer_->resize(Wt::WLength(width), Wt::WLength(height));
        }
    }
    
    /**
     * @brief Gets the popover width
     * @return Popover width
     */
    int getPopoverWidth() const { return popoverWidth_; }
    
    /**
     * @brief Gets the popover height
     * @return Popover height
     */
    int getPopoverHeight() const { return popoverHeight_; }
    
    /**
     * @brief Sets whether to show item prices
     * @param showPrices True to show prices
     */
    void setShowPrices(bool showPrices) { showPrices_ = showPrices; }
    
    /**
     * @brief Gets whether prices are shown
     * @return True if prices are shown
     */
    bool getShowPrices() const { return showPrices_; }
    
    /**
     * @brief Sets whether to show item images
     * @param showImages True to show images
     */
    void setShowImages(bool showImages) { showImages_ = showImages; }
    
    /**
     * @brief Gets whether images are shown
     * @return True if images are shown
     */
    bool getShowImages() const { return showImages_; }
    
    /**
     * @brief Sets the item card size
     * @param width Card width
     * @param height Card height
     */
    void setItemCardSize(int width, int height) {
        itemCardWidth_ = width;
        itemCardHeight_ = height;
    }
    
    /**
     * @brief Gets the item card width
     * @return Card width
     */
    int getItemCardWidth() const { return itemCardWidth_; }
    
    /**
     * @brief Gets the item card height
     * @return Card height
     */
    int getItemCardHeight() const { return itemCardHeight_; }
    
    /**
     * @brief Sets whether to enable item hover effects
     * @param enabled True to enable hover effects
     */
    void setHoverEffectsEnabled(bool enabled) { hoverEffectsEnabled_ = enabled; }
    
    /**
     * @brief Gets whether hover effects are enabled
     * @return True if hover effects are enabled
     */
    bool isHoverEffectsEnabled() const { return hoverEffectsEnabled_; }
    
    /**
     * @brief Sets the category for this popover
     * @param category New category
     */
    void setCategory(MenuItem::Category category) { 
        category_ = category; 
        refreshDisplay();
    }
    
    /**
     * @brief Gets the current category
     * @return Current category
     */
    MenuItem::Category getCategory() const { return category_; }
    
    /**
     * @brief Sets whether to auto-close on item selection
     * @param autoClose True to auto-close
     */
    void setAutoCloseOnSelection(bool autoClose) { autoCloseOnSelection_ = autoClose; }
    
    /**
     * @brief Gets whether auto-close is enabled
     * @return True if auto-close is enabled
     */
    bool getAutoCloseOnSelection() const { return autoCloseOnSelection_; }
    
    /**
     * @brief Sets the popover animation type
     * @param animationType Animation type ("fade", "slide", "none")
     */
    void setAnimationType(const std::string& animationType) { animationType_ = animationType; }
    
    /**
     * @brief Gets the animation type
     * @return Animation type
     */
    const std::string& getAnimationType() const { return animationType_; }

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
    
    /**
     * @brief Refreshes the display after configuration changes
     */
    void refreshDisplay();

private:
    // Category and items
    MenuItem::Category category_;
    std::vector<std::shared_ptr<MenuItem>> menuItems_;
    std::shared_ptr<EventManager> eventManager_;
    ItemSelectionCallback selectionCallback_;
    
    // Display configuration
    int maxColumns_;
    bool showDescriptions_;
    
    // Additional configuration options - Added these to support missing methods
    int popoverWidth_;
    int popoverHeight_;
    bool showPrices_;
    bool showImages_;
    int itemCardWidth_;
    int itemCardHeight_;
    bool hoverEffectsEnabled_;
    bool autoCloseOnSelection_;
    std::string animationType_;
    
    // UI components
    Wt::WContainerWidget* contentContainer_;
    Wt::WContainerWidget* itemsContainer_;
    Wt::WText* headerText_;
    Wt::WPushButton* closeButton_;
    
    // Helper methods
    void setupStyling();
    void refreshItemsDisplay();
    void applyItemCardStyling(Wt::WContainerWidget* card);
    void setupItemCardEvents(Wt::WContainerWidget* card, std::shared_ptr<MenuItem> item);
};

#endif // CATEGORYPOPOVER_H
