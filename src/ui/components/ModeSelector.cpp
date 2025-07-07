//============================================================================
// ModeSelector Implementation
//============================================================================

#include "../../../include/ui/components/ModeSelector.hpp"

ModeSelector::ModeSelector(std::shared_ptr<EventManager> eventManager,
                          ModeChangeCallback callback)
    : eventManager_(eventManager)
    , modeChangeCallback_(callback)
    , currentMode_(POS_MODE)
{
    initializeUI();
}

void ModeSelector::initializeUI() {
    addStyleClass("mode-selector d-flex align-items-center gap-2");
    
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    
    // POS Mode Button
    posModeButton_ = layout->addWidget(std::make_unique<Wt::WPushButton>("📋 POS"));
    posModeButton_->addStyleClass("btn btn-outline-light");
    posModeButton_->clicked().connect([this]() { onPOSModeClicked(); });
    
    // Kitchen Mode Button
    kitchenModeButton_ = layout->addWidget(std::make_unique<Wt::WPushButton>("👨‍🍳 Kitchen"));
    kitchenModeButton_->addStyleClass("btn btn-outline-light");
    kitchenModeButton_->clicked().connect([this]() { onKitchenModeClicked(); });
    
    // Current mode text
    currentModeText_ = layout->addWidget(std::make_unique<Wt::WText>("Current: POS Mode"));
    currentModeText_->addStyleClass("text-light ms-3 fw-bold");
    
    updateButtonStates();
}

void ModeSelector::setCurrentMode(Mode mode) {
    if (currentMode_ != mode) {
        currentMode_ = mode;
        updateButtonStates();
    }
}

void ModeSelector::updateButtonStates() {
    // Reset button styles
    posModeButton_->removeStyleClass("active");
    kitchenModeButton_->removeStyleClass("active");
    
    // Set active button
    switch (currentMode_) {
        case POS_MODE:
            posModeButton_->addStyleClass("active");
            currentModeText_->setText("Current: POS Mode");
            break;
        case KITCHEN_MODE:
            kitchenModeButton_->addStyleClass("active");
            currentModeText_->setText("Current: Kitchen Mode");
            break;
    }
}

void ModeSelector::onPOSModeClicked() {
    setCurrentMode(POS_MODE);
    if (modeChangeCallback_) {
        modeChangeCallback_(POS_MODE);
    }
}

void ModeSelector::onKitchenModeClicked() {
    setCurrentMode(KITCHEN_MODE);
    if (modeChangeCallback_) {
        modeChangeCallback_(KITCHEN_MODE);
    }
}
