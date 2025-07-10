#ifndef CURRENTORDERDISPLAY_H
#define CURRENTORDERDISPLAY_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"
#include "../../Order.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WSpinBox.h>

#include <memory>
#include <vector>

/**
 * @file CurrentOrderDisplay.hpp - Enhanced with Destruction Safety
 * @brief Current order display component for the Restaurant POS System
 * 
 * This component displays the items in the current order being built,
 * allowing for quantity modifications and item removal.
 * 
 * @author Restaurant POS Team
 * @version 2.1.0 - Enhanced with destruction safety
 */

/**
 * @class CurrentOrderDisplay
 * @brief UI component for displaying and managing the current order
 * 
 * The CurrentOrderDisplay shows all items in the current order with
 * functionality to modify quantities, remove items, and display totals.
 * It automatically updates when the current order changes.
 */
class CurrentOrderDisplay : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the current order display
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     */
    CurrentOrderDisplay(std::shared_ptr<POSService> posService,
                       std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor with cleanup
     */
    virtual ~CurrentOrderDisplay();
    
    /**
     * @brief Refreshes the order display from the service
     */
    void refresh();
    
    /**
     * @brief Gets the current order being displayed
     * @return Pointer to current order, or nullptr if none
     */
    std::shared_ptr<Order> getCurrentOrder() const;
    
    /**
     * @brief Sets whether the display is in edit mode
     * @param editable True to allow editing, false for read-only
     */
    void setEditable(bool editable);
    
    /**
     * @brief Checks if the display is in edit mode
     * @return True if editable, false if read-only
     */
    bool isEditable() const;
    
    /**
     * @brief Clears the current order display
     */
    void clearOrder();

protected:
    /**
     * @brief Initializes the UI components
     */
    void initializeUI();

    /**
     * @brief Initializes the UI simple components
     */
    void initializeUI_Simple();

    
    /**
     * @brief Sets up event listeners
     */
    void setupEventListeners();
    
    /**
     * @brief Creates the order header section
     * @return Container widget with order header
     */
    std::unique_ptr<Wt::WWidget> createOrderHeader();
    
    /**
     * @brief Creates the order items table
     */
    void createOrderItemsTable();
    
    /**
     * @brief Creates the order summary section
     * @return Container widget with order totals
     */
    std::unique_ptr<Wt::WWidget> createOrderSummary();
    
    /**
     * @brief Updates the order items table
     */
    void updateOrderItemsTable();
    
    /**
     * @brief Updates the order summary totals
     */
    void updateOrderSummary();
    
    /**
     * @brief Adds an order item row to the table
     * @param item Order item to add
     * @param index Index of the item in the order
     */
    void addOrderItemRow(const OrderItem& item, size_t index);

    /**
     * @brief Initializes table headers
     */
    void initializeTableHeaders();
    
    /**
     * @brief Applies consistent styling to a table row
     * @param row Row number to style
     * @param isEven True if this is an even-numbered row
     */
    void applyRowStyling(int row, bool isEven);

    /**
     * @brief Creates the order summary content in the summary container
     */
    void createOrderSummaryContent();

private:
    // Services and dependencies
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // ADDED: Destruction safety flag
    bool isDestroying_;
    
    // Configuration
    bool editable_;
    
    // UI components
    Wt::WContainerWidget* headerContainer_;
    Wt::WText* tableNumberText_;
    Wt::WText* orderIdText_;
    Wt::WTable* itemsTable_;
    Wt::WContainerWidget* summaryContainer_;
    Wt::WText* subtotalText_;
    Wt::WText* taxText_;
    Wt::WText* totalText_;
    Wt::WText* itemCountText_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers
    void handleOrderCreated(const std::any& eventData);
    void handleOrderModified(const std::any& eventData);
    void handleCurrentOrderChanged(const std::any& eventData);
    
    // UI action handlers
    void onQuantityChanged(size_t itemIndex, int newQuantity);
    void onRemoveItemClicked(size_t itemIndex);
    void onSpecialInstructionsChanged(size_t itemIndex, const std::string& instructions);
    
    // Helper methods
    std::string formatCurrency(double amount) const;
    std::string formatItemName(const OrderItem& item) const;
    std::string formatItemPrice(const OrderItem& item) const;
    void showEmptyOrderMessage();
    void hideEmptyOrderMessage();
    bool hasCurrentOrder() const;
    void validateOrderDisplay();
    
    // Styling methods
    void applyTableStyling();
    void applySummaryStyling();
    void updateRowStyling(int row, bool isEven);
};

#endif // CURRENTORDERDISPLAY_H
