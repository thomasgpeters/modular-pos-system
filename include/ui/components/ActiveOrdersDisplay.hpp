// =============================================================================
// COMPLETE FIXED ActiveOrdersDisplay EXAMPLE
// =============================================================================

#ifndef ACTIVEORDERSDISPLAY_H
#define ACTIVEORDERSDISPLAY_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"
#include "../../utils/UIHelpers.hpp"
#include "../../utils/FormatUtils.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>

#include <memory>
#include <vector>

class ActiveOrdersDisplay : public Wt::WContainerWidget {
public:
    ActiveOrdersDisplay(std::shared_ptr<POSService> posService,
                       std::shared_ptr<EventManager> eventManager)
        : posService_(posService), eventManager_(eventManager) {
        
        setupUI();
        setupEventHandlers();
        refreshDisplay();
    }
    
    void refresh() {
        refreshDisplay();
    }

private:
    void setupUI() {
        addStyleClass("active-orders-display");
        
        // Create main layout - CORRECT WAY
        auto layout = std::make_unique<Wt::WVBoxLayout>();
        
        // Header section - CORRECT PATTERN
        auto headerContainer = std::make_unique<Wt::WContainerWidget>();
        headerContainer->addStyleClass("orders-header");
        
        auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
        
        // Title - Method 1: Direct creation
        headerLayout->addWidget(std::make_unique<Wt::WText>("Active Orders"));
        
        // Refresh button - Method 2: Create, configure, add
        auto refreshBtn = std::make_unique<Wt::WPushButton>("Refresh");
        refreshBtn->addStyleClass("btn btn-secondary btn-sm");
        refreshButton_ = headerLayout->addWidget(std::move(refreshBtn));
        
        headerContainer->setLayout(std::move(headerLayout));
        layout->addWidget(std::move(headerContainer));
        
        // Table section - CORRECT PATTERN
        auto tableContainer = std::make_unique<Wt::WContainerWidget>();
        tableContainer->addStyleClass("table-container");
        
        // Create table using UIHelpers
        auto table = UIHelpers::createTable("table table-striped table-hover");
        ordersTable_ = tableContainer->addWidget(std::move(table));
        
        layout->addWidget(std::move(tableContainer));
        
        // Status section - CORRECT PATTERN  
        auto statusContainer = std::make_unique<Wt::WContainerWidget>();
        statusContainer->addStyleClass("status-container");
        
        // Status text - Method 2: Store pointer for updates
        auto statusText = std::make_unique<Wt::WText>("Ready");
        statusText->addStyleClass("status-text");
        statusLabel_ = statusContainer->addWidget(std::move(statusText));
        
        layout->addWidget(std::move(statusContainer));
        
        // Set the layout
        setLayout(std::move(layout));
    }
    
    void setupEventHandlers() {
        // Event handlers - use stored pointers
        if (refreshButton_) {
            refreshButton_->clicked().connect([this]() {
                refreshDisplay();
            });
        }
        
        // Subscribe to events
        if (eventManager_) {
            eventManager_->subscribe("ORDER_CREATED", [this](const std::any&) {
                refreshDisplay();
            });
        }
    }
    
    void refreshDisplay() {
        if (!ordersTable_ || !posService_) return;
        
        // Clear existing data
        UIHelpers::clearTableData(ordersTable_);
        
        // Add headers
        std::vector<std::string> headers = {"Order #", "Table", "Items", "Total", "Status", "Actions"};
        UIHelpers::addTableHeader(ordersTable_, headers);
        
        // Get active orders
        auto orders = posService_->getActiveOrders();
        
        // Add order rows
        int row = 1;
        for (const auto& order : orders) {
            // Order ID
            UIHelpers::setTableCell(ordersTable_, row, 0, 
                FormatUtils::formatOrderId(order->getId()));
            
            // Table number
            UIHelpers::setTableCell(ordersTable_, row, 1,
                FormatUtils::formatTableNumber(order->getTableNumber()));
            
            // Item count
            UIHelpers::setTableCell(ordersTable_, row, 2,
                std::to_string(order->getItems().size()));
            
            // Total
            UIHelpers::setTableCell(ordersTable_, row, 3,
                FormatUtils::formatCurrency(order->getTotalWithTax()));
            
            // Status
            UIHelpers::setTableCell(ordersTable_, row, 4,
                FormatUtils::formatOrderStatus(order->getStatus()));
            
            // Actions - CORRECT WAY to add button to table
            auto actionsContainer = std::make_unique<Wt::WContainerWidget>();
            auto actionLayout = std::make_unique<Wt::WHBoxLayout>();
            
            auto viewBtn = std::make_unique<Wt::WPushButton>("View");
            viewBtn->addStyleClass("btn btn-sm btn-info");
            auto* viewBtnPtr = actionLayout->addWidget(std::move(viewBtn));
            
            auto completeBtn = std::make_unique<Wt::WPushButton>("Complete");
            completeBtn->addStyleClass("btn btn-sm btn-success");
            auto* completeBtnPtr = actionLayout->addWidget(std::move(completeBtn));
            
            // Set up event handlers
            viewBtnPtr->clicked().connect([this, order]() {
                viewOrder(order);
            });
            
            completeBtnPtr->clicked().connect([this, order]() {
                completeOrder(order->getId());
            });
            
            actionsContainer->setLayout(std::move(actionLayout));
            UIHelpers::setTableCellWidget(ordersTable_, row, 5, std::move(actionsContainer));
            
            row++;
        }
        
        // Update status
        if (statusLabel_) {
            statusLabel_->setText(std::to_string(orders.size()) + " active orders");
        }
    }
    
    void viewOrder(std::shared_ptr<Order> order) {
        // Implementation for viewing order details
        if (eventManager_) {
            // Could publish event to show order details dialog
        }
    }
    
    void completeOrder(int orderId) {
        if (posService_) {
            posService_->completeOrder(orderId);
            refreshDisplay();
        }
    }
    
    // Member variables - store raw pointers for widgets we need to access
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    Wt::WTable* ordersTable_ = nullptr;
    Wt::WPushButton* refreshButton_ = nullptr;
    Wt::WText* statusLabel_ = nullptr;
};

#endif // ACTIVEORDERSDISPLAY_H
