//============================================================================
// Fixed OrderEntryPanel.hpp - Added Destruction Safety
//============================================================================

#ifndef ORDERENTRYPANEL_H
#define ORDERENTRYPANEL_H

#include "../../utils/UIStyleHelper.hpp" // Include our styling helper

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"

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
 * @file OrderEntryPanel.hpp - Simplified Version with Destruction Safety
 * @brief Simple order entry controls without embedded components
 * 
 * This simplified version focuses only on table selection and order actions.
 * MenuDisplay and CurrentOrderDisplay are handled separately in the main app.
 * 
 * @author Restaurant POS Team
 * @version 2.4.0 - Added destruction safety
 */

/**
 * @class OrderEntryPanel
 * @brief Simplified UI component for order entry controls only
 * 
 * The OrderEntryPanel now handles only:
 * 1. Table identifier selection
 * 2. New order creation
 * 3. Send to kitchen action
 * 4. Process payment action
 * 
 * MenuDisplay and CurrentOrderDisplay are separate components.
 */
class OrderEntryPanel : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the simplified order entry panel
     * @param posService Shared POS service for business operations
     * @param eventManager Shared event manager for component communication
     */
    OrderEntryPanel(std::shared_ptr<POSService> posService,
                   std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Virtual destructor with cleanup
     */
    virtual ~OrderEntryPanel();
    
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
     * @brief Sets up the table selection section
     */
    void setupTableSelectionSection();
    
    /**
     * @brief Sets up the order action buttons section
     */
    void setupOrderActionsSection();
    
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
     * @param parent Parent container to add the combo box to
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
    
    // ADDED: Destruction safety flag
    bool isDestroying_;
    
    // UI components (raw pointers - Wt manages lifetime)
    Wt::WGroupBox* tableSelectionGroup_;
    Wt::WLabel* tableIdentifierLabel_;
    Wt::WComboBox* tableIdentifierCombo_;
    Wt::WText* tableStatusText_;
    
    Wt::WContainerWidget* actionsContainer_;
    Wt::WPushButton* newOrderButton_;
    Wt::WPushButton* sendToKitchenButton_;
    Wt::WPushButton* processPaymentButton_;
    
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
    void createNewOrder(const std::string& tableIdentifier);
    void createNewOrder(int tableNumber); // Legacy compatibility
    void sendCurrentOrderToKitchen();
    
    // Helper methods
    void updateOrderActionButtons();
    bool validateCurrentOrder();
    bool validateTableIdentifierSelection() const;
    bool hasCurrentOrder() const;
    bool hasOrderWithItems() const;
    void showOrderValidationError(const std::string& message);
    void showOrderValidationMessage(const std::string& message, const std::string& type);
    void updateTableStatus();
    std::string formatTableIdentifier(const std::string& identifier) const;
    std::string getTableIdentifierIcon(const std::string& identifier) const;
    bool isTableIdentifierAvailable(const std::string& identifier) const;
    void refreshAvailableIdentifiers();
    
    // Table identifier management
    std::string extractTableIdentifierFromDisplayText(const std::string& displayText) const;
    std::string formatTableIdentifierForDisplay(const std::string& identifier) const;
    std::vector<std::string> getDefaultTableIdentifiers() const;
    
    // Styling methods
    void applyTableSelectionStyling();
    void updateTableIdentifierStyling();
};

#endif // ORDERENTRYPANEL_H
