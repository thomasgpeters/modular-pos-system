#ifndef ORDERENTRYPANEL_H
#define ORDERENTRYPANEL_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"
#include "MenuDisplay.hpp"
#include "CurrentOrderDisplay.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WBorderLayout.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>

#include <memory>

/**
 * @file OrderEntryPanel.hpp
 * @brief Enhanced order entry UI component for the Restaurant POS System
 * 
 * This component handles the complete order workflow including table selection,
 * menu display, order building, and kitchen/payment processing.
 * Updated to support string-based table identifiers and complete order workflow.
 * 
 * @author Restaurant POS Team
 * @version 2.2.0 - Enhanced with complete order workflow
 */

/**
 * @class OrderEntryPanel
 * @brief UI component for complete order entry workflow
 * 
 * The OrderEntryPanel manages the complete order creation workflow:
 * 1. Table identifier selection
 * 2. Order creation
 * 3. Menu browsing and item selection
 * 4. Current order management
 * 5. Kitchen submission and payment processing
 * 
 * It coordinates with the POSService for business operations and uses
 * the event system for communication with other components.
 */
class OrderEntryPanel : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the order entry panel
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     */
    OrderEntryPanel(std::shared_ptr<POSService> posService,
                   std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~OrderEntryPanel() = default;
    
    /**
     * @brief Refreshes the panel data from the service
     */
    void refresh();
    
    /**
     * @brief Sets the current table identifier
     * @param tableIdentifier Table identifier to set (e.g., "table 5", "walk-in")
     */
    void setTableIdentifier(const std::string& tableIdentifier);
    
    /**
     * @brief Gets the current table identifier
     * @return Current table identifier
     */
    std::string getTableIdentifier() const;
    
    /**
     * @brief Sets the current table number (legacy compatibility)
     * @deprecated Use setTableIdentifier() instead
     * @param tableNumber Table number to set
     */
    void setTableNumber(int tableNumber);
    
    /**
     * @brief Gets the current table number (legacy compatibility)
     * @deprecated Use getTableIdentifier() instead
     * @return Current table number
     */
    int getTableNumber() const;
    
    /**
     * @brief Enables or disables the order entry controls
     * @param enabled True to enable, false to disable
     */
    void setOrderEntryEnabled(bool enabled);
    
    /**
     * @brief Sets the available table identifier options
     * @param identifiers Vector of available table identifiers
     */
    void setAvailableTableIdentifiers(const std::vector<std::string>& identifiers);
    
    /**
     * @brief Gets the selected table identifier from the combo box
     * @return Selected table identifier, empty string if none selected
     */
    std::string getSelectedTableIdentifier() const;
    
    /**
     * @brief Gets the menu display component
     * @return Pointer to menu display component
     */
    MenuDisplay* getMenuDisplay() const { return menuDisplay_; }
    
    /**
     * @brief Gets the current order display component
     * @return Pointer to current order display component
     */
    CurrentOrderDisplay* getCurrentOrderDisplay() const { return currentOrderDisplay_; }

protected:
    /**
     * @brief Initializes the UI components
     */
    void initializeUI();
    
    /**
     * @brief Sets up the table selection section
     * @param parent Parent container for the table selection UI
     */
    void setupTableSelectionSection(Wt::WContainerWidget* parent);
    
    /**
     * @brief Sets up the order action buttons section
     * @param parent Parent container for the action buttons
     */
    void setupOrderActionsSection(Wt::WContainerWidget* parent);
    
    /**
     * @brief Sets up the menu and order display components
     * @param parent Parent container for the display components
     */
    void setupMenuAndOrderDisplays(Wt::WContainerWidget* parent);
    
    /**
     * @brief Sets up event listeners
     */
    void setupEventListeners();
    
    /**
     * @brief Sets up event handlers
     */
    void setupEventHandlers();
    
    /**
     * @brief Creates the table identifier combo box
     * @param parent Parent container for the combo box
     */
    void createTableIdentifierCombo(Wt::WContainerWidget* parent);
    
    /**
     * @brief Populates the table identifier combo box
     */
    void populateTableIdentifierCombo();

private:
    // Services and dependencies
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // UI components
    Wt::WGroupBox* tableSelectionGroup_;
    Wt::WLabel* tableIdentifierLabel_;
    Wt::WComboBox* tableIdentifierCombo_;
    Wt::WText* tableStatusText_;
    Wt::WPushButton* newOrderButton_;
    Wt::WPushButton* sendToKitchenButton_;
    Wt::WPushButton* processPaymentButton_;
    
    // Child components (raw pointers - Wt manages lifetime)
    MenuDisplay* menuDisplay_;
    CurrentOrderDisplay* currentOrderDisplay_;
    
    // Available table identifiers
    std::vector<std::string> availableTableIdentifiers_;
    
    // Event subscription handles
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
    
    // Event handlers
    void handleOrderCreated(const std::any& eventData);
    void handleOrderModified(const std::any& eventData);
    void handleCurrentOrderChanged(const std::any& eventData);
    
    // UI action handlers
    void onNewOrderClicked();
    void onSendToKitchenClicked();
    void onProcessPaymentClicked();
    void onTableIdentifierChanged();
    
    // Business logic methods
    /**
     * @brief Creates a new order for the specified table identifier
     * @param tableIdentifier Table identifier for the new order
     */
    void createNewOrder(const std::string& tableIdentifier);
    
    /**
     * @brief Creates a new order for the specified table number (legacy)
     * @deprecated Use createNewOrder(const std::string&) instead
     * @param tableNumber Table number for the new order
     */
    void createNewOrder(int tableNumber);
    
    /**
     * @brief Sends the current order to the kitchen
     */
    void sendCurrentOrderToKitchen();
    
    // Helper methods
    void updateOrderActionButtons();
    
    /**
     * @brief Validates the current order state
     * @return True if current order is valid, false otherwise
     */
    bool validateCurrentOrder();
    
    /**
     * @brief Validates the table identifier selection
     * @return True if a valid table identifier is selected
     */
    bool validateTableIdentifierSelection();
    
    /**
     * @brief Checks if there is a current order
     * @return True if there is a current order
     */
    bool hasCurrentOrder() const;
    
    /**
     * @brief Checks if current order has items
     * @return True if current order exists and has items
     */
    bool hasOrderWithItems() const;
    
    /**
     * @brief Shows validation error message
     * @param message Error message to display
     */
    void showOrderValidationError(const std::string& message);
    
    /**
     * @brief Shows validation message with specified type
     * @param message Message to display
     * @param type Message type ("success", "warning", "error", "info")
     */
    void showOrderValidationMessage(const std::string& message, const std::string& type);
    
    void updateTableStatus();
    std::string formatTableIdentifier(const std::string& identifier) const;
    std::string getTableIdentifierIcon(const std::string& identifier) const;
    bool isTableIdentifierAvailable(const std::string& identifier) const;
    void refreshAvailableIdentifiers();
    
    // Table identifier management
    std::string extractTableIdentifierFromDisplayText(const std::string& displayText) const;
    std::string formatTableIdentifierForDisplay(const std::string& identifier) const;
    
    // Default table identifiers
    std::vector<std::string> getDefaultTableIdentifiers() const;
    
    // Styling methods
    void applyTableSelectionStyling();
    void updateTableIdentifierStyling();
};

#endif // ORDERENTRYPANEL_H
