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
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>

#include <memory>

/**
 * @file OrderEntryPanel.hpp
 * @brief Order entry UI component for the Restaurant POS System
 * 
 * This component handles the left panel of the POS interface, including
 * table selection, menu display, and current order management.
 * Updated to support string-based table identifiers.
 * 
 * @author Restaurant POS Team
 * @version 2.1.0 - Updated for string-based table identifiers
 */

/**
 * @class OrderEntryPanel
 * @brief UI component for order entry and menu selection
 * 
 * The OrderEntryPanel manages the order creation workflow, including
 * table identifier selection, menu browsing, and current order building.
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
     * @brief Sets up event handlers
     */
    void setupEventHandlers();
    
    /**
     * @brief Creates the table selection section
     * @return Container widget with table selection controls
     */
    std::unique_ptr<Wt::WWidget> createTableSelectionSection();
    
    /**
     * @brief Creates the order actions section
     * @return Container widget with order action buttons
     */
    std::unique_ptr<Wt::WWidget> createOrderActionsSection();
    
    /**
     * @brief Creates the table identifier combo box
     */
    void createTableIdentifierCombo();
    
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
    
    bool hasCurrentOrder() const;
    void showOrderValidationError(const std::string& message);
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
