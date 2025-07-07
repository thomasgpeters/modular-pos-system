//============================================================================
// Updated CommonHeader.hpp - Added Missing Method Declarations
//============================================================================

#ifndef COMMONHEADER_H
#define COMMONHEADER_H

#include "../../services/ThemeService.hpp"
#include "../../events/EventManager.hpp"
#include "ModeSelector.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WString.h>

#include <memory>
#include <functional>

/**
 * @class CommonHeader
 * @brief Common header with branding, mode selector, and theme controls
 */
class CommonHeader : public Wt::WContainerWidget {
public:
    using ModeChangeCallback = std::function<void(ModeSelector::Mode)>;
    
    /**
     * @brief Constructs the common header
     * @param themeService Theme service for theme controls
     * @param eventManager Event manager for notifications
     * @param modeChangeCallback Callback for mode changes
     */
    CommonHeader(std::shared_ptr<ThemeService> themeService,
                std::shared_ptr<EventManager> eventManager,
                ModeChangeCallback modeChangeCallback = nullptr);
    
    /**
     * @brief Sets the current mode in the selector
     * @param mode Mode to set
     */
    void setCurrentMode(ModeSelector::Mode mode);
    
    /**
     * @brief Gets the current mode from the selector
     * @return Current mode
     */
    ModeSelector::Mode getCurrentMode() const;

protected:
    void initializeUI();
    void createBrandingSection();
    void createModeSection();
    void createThemeSection();
    void createUserSection();
    void setupEventHandlers();

private:
    std::shared_ptr<ThemeService> themeService_;
    std::shared_ptr<EventManager> eventManager_;
    ModeChangeCallback modeChangeCallback_;
    
    // UI components (raw pointers - Wt manages lifetime)
    Wt::WContainerWidget* brandingContainer_;
    Wt::WText* brandingText_;
    
    ModeSelector* modeSelector_;
    
    Wt::WContainerWidget* themeContainer_;
    Wt::WComboBox* themeComboBox_;
    Wt::WPushButton* themeToggleButton_;
    
    Wt::WContainerWidget* userContainer_;
    Wt::WText* userInfo_;
    Wt::WText* timeDisplay_;
    
    // Helper methods
    void updateTimeDisplay();
    void onThemeSelectionChanged(int index);                    // NEW: For activated() signal
    void onThemeSelectionChangedByText(const Wt::WString& text); // NEW: For sactivated() signal
    void onThemeToggleClicked();
    void updateThemeControls();
};

#endif // COMMONHEADER_H
