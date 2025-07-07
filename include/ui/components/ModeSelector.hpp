//============================================================================
// MODE SELECTOR COMPONENT
//============================================================================

#ifndef MODESELECTOR_H
#define MODESELECTOR_H

#include "../../events/EventManager.hpp"
#include "../../events/POSEvents.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WHBoxLayout.h>

#include <memory>
#include <functional>

/**
 * @class ModeSelector
 * @brief Component for switching between POS and Kitchen modes
 */
class ModeSelector : public Wt::WContainerWidget {
public:
    enum Mode { POS_MODE, KITCHEN_MODE };
    
    using ModeChangeCallback = std::function<void(Mode)>;
    
    /**
     * @brief Constructs the mode selector
     * @param eventManager Event manager for notifications
     * @param callback Mode change callback
     */
    ModeSelector(std::shared_ptr<EventManager> eventManager,
                ModeChangeCallback callback = nullptr);
    
    /**
     * @brief Sets the current mode
     * @param mode Mode to set as current
     */
    void setCurrentMode(Mode mode);
    
    /**
     * @brief Gets the current mode
     * @return Current mode
     */
    Mode getCurrentMode() const { return currentMode_; }
    
    /**
     * @brief Sets the mode change callback
     * @param callback Function to call when mode changes
     */
    void setModeChangeCallback(ModeChangeCallback callback) { 
        modeChangeCallback_ = callback; 
    }

protected:
    void initializeUI();
    void updateButtonStates();
    void onPOSModeClicked();
    void onKitchenModeClicked();

private:
    std::shared_ptr<EventManager> eventManager_;
    ModeChangeCallback modeChangeCallback_;
    Mode currentMode_;
    
    // UI components
    Wt::WPushButton* posModeButton_;
    Wt::WPushButton* kitchenModeButton_;
    Wt::WText* currentModeText_;
};

#endif // MODESELECTOR_H
