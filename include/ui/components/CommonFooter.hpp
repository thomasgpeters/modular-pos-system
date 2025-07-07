//============================================================================
// Fixed CommonFooter.hpp - Header File
//============================================================================

#ifndef COMMONFOOTER_H
#define COMMONFOOTER_H

#include "../../services/POSService.hpp"
#include "../../events/EventManager.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WHBoxLayout.h>

#include <memory>

/**
 * @class CommonFooter
 * @brief Common footer with status information and system info
 */
class CommonFooter : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructs the common footer
     * @param posService POS service for status information
     * @param eventManager Event manager for notifications
     */
    CommonFooter(std::shared_ptr<POSService> posService,
                std::shared_ptr<EventManager> eventManager);
    
    /**
     * @brief Updates the footer status information
     */
    void updateStatus();

protected:
    void initializeUI();
    void createStatusSection();
    void createSystemSection();
    void setupEventListeners();

private:
    std::shared_ptr<POSService> posService_;
    std::shared_ptr<EventManager> eventManager_;
    
    // UI components (raw pointers - Wt manages lifetime)
    Wt::WText* activeOrdersText_;
    Wt::WText* kitchenQueueText_;
    Wt::WText* systemStatusText_;
    Wt::WText* versionText_;
    
    // Event handlers
    void handleOrderStatusChanged(const std::any& eventData);
    void handleKitchenStatusChanged(const std::any& eventData);
    
    // Event subscriptions
    std::vector<EventManager::SubscriptionHandle> eventSubscriptions_;
};

#endif // COMMONFOOTER_H

