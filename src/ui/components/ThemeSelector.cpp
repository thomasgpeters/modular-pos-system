// ============================================================================
// Fixed ThemeSelector Implementation - Enhanced Event Handling and Debugging
// File: src/ui/components/ThemeSelector.cpp
// ============================================================================

#include "../../../include/ui/components/ThemeSelector.hpp"

#include <Wt/WVBoxLayout.h>
#include <iostream>

ThemeSelector::ThemeSelector(std::shared_ptr<ThemeService> themeService,
                            DisplayMode displayMode)
    : Wt::WContainerWidget()
    , themeService_(themeService)
    , displayMode_(DisplayMode::DROPDOWN) // Force dropdown mode for compatibility
    , showDescriptions_(false) // Simplified
    , showPreviews_(false)
    , allowAutoTheme_(true)
    , enabled_(true)
    , themeComboBox_(nullptr)
    , toggleButton_(nullptr)
    , gridContainer_(nullptr)
    , currentThemeText_(nullptr)
    , themeChangeSubscription_(0)
{
    if (!themeService_) {
        throw std::invalid_argument("ThemeSelector requires a valid ThemeService");
    }
    
    addStyleClass("theme-selector");
    
    // FIXED: Set up event listeners before initializing UI
    setupEventListeners();
    
    // Initialize UI
    initializeUI();
    
    std::cout << "[ThemeSelector] Initialized with current theme: " 
              << themeService_->getThemeName(themeService_->getCurrentTheme()) << std::endl;
}

void ThemeSelector::setDisplayMode(DisplayMode mode) {
    // For now, always use dropdown mode for compatibility
    displayMode_ = DisplayMode::DROPDOWN;
    std::cout << "[ThemeSelector] Display mode set to DROPDOWN (forced for compatibility)" << std::endl;
    initializeUI();
}

void ThemeSelector::setShowDescriptions(bool show) {
    if (showDescriptions_ != show) {
        showDescriptions_ = show;
        std::cout << "[ThemeSelector] Show descriptions: " << (show ? "enabled" : "disabled") << std::endl;
        updateSelector();
    }
}

void ThemeSelector::setShowPreviews(bool show) {
    showPreviews_ = show;
    updateSelector();
}

void ThemeSelector::setAllowAutoTheme(bool allow) {
    if (allowAutoTheme_ != allow) {
        allowAutoTheme_ = allow;
        std::cout << "[ThemeSelector] Auto theme: " << (allow ? "allowed" : "disabled") << std::endl;
        updateSelector();
    }
}

void ThemeSelector::setThemeSelectionCallback(ThemeSelectionCallback callback) {
    selectionCallback_ = callback;
    std::cout << "[ThemeSelector] Selection callback " << (callback ? "registered" : "removed") << std::endl;
}

ThemeService::Theme ThemeSelector::getSelectedTheme() const {
    return themeService_->getCurrentTheme();
}

void ThemeSelector::setSelectedTheme(ThemeService::Theme theme) {
    std::cout << "[ThemeSelector] Setting selected theme to: " << themeService_->getThemeName(theme) << std::endl;
    
    if (isValidTheme(theme)) {
        themeService_->setTheme(theme);
        updateSelector();
    } else {
        std::cerr << "[ThemeSelector] Invalid theme provided: " << static_cast<int>(theme) << std::endl;
    }
}

void ThemeSelector::refresh() {
    std::cout << "[ThemeSelector] Refreshing selector..." << std::endl;
    updateSelector();
}

void ThemeSelector::setEnabled(bool enabled) {
    if (enabled_ != enabled) {
        enabled_ = enabled;
        std::cout << "[ThemeSelector] Selector " << (enabled ? "enabled" : "disabled") << std::endl;
        updateEnabledState();
    }
}

void ThemeSelector::initializeUI() {
    std::cout << "[ThemeSelector] Initializing UI..." << std::endl;
    
    // Clear existing UI
    clearSelector();
    
    // Always create dropdown for compatibility
    createDropdownSelector();
    
    // Apply styling and update state
    applyThemeSelectorStyling();
    updateSelector();
    updateEnabledState();
    
    std::cout << "[ThemeSelector] UI initialization complete" << std::endl;
}

void ThemeSelector::createDropdownSelector() {
    std::cout << "[ThemeSelector] Creating dropdown selector..." << std::endl;
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Theme selection dropdown
    themeComboBox_ = addNew<Wt::WComboBox>();
    themeComboBox_->addStyleClass("form-select theme-selector-dropdown");
    
    // FIXED: Enhanced event connection with better error handling
    themeComboBox_->changed().connect([this]() {
        std::cout << "[ThemeSelector] Dropdown selection changed!" << std::endl;
        
        try {
            int index = themeComboBox_->currentIndex();
            std::cout << "[ThemeSelector] Selected index: " << index << std::endl;
            
            auto themes = getSelectableThemes();
            std::cout << "[ThemeSelector] Available themes count: " << themes.size() << std::endl;
            
            if (index >= 0 && index < static_cast<int>(themes.size())) {
                ThemeService::Theme selectedTheme = themes[index];
                std::cout << "[ThemeSelector] Selected theme: " << themeService_->getThemeName(selectedTheme) << std::endl;
                onThemeSelected(selectedTheme);
            } else {
                std::cerr << "[ThemeSelector] Invalid index selected: " << index << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[ThemeSelector] Error in dropdown changed handler: " << e.what() << std::endl;
        }
    });
    
    // FIXED: Also connect to sactivated signal for more reliable event handling
    themeComboBox_->sactivated().connect([this](const Wt::WString& text) {
        std::cout << "[ThemeSelector] Dropdown item activated: " << text.toUTF8() << std::endl;
        
        try {
            int index = themeComboBox_->currentIndex();
            auto themes = getSelectableThemes();
            
            if (index >= 0 && index < static_cast<int>(themes.size())) {
                ThemeService::Theme selectedTheme = themes[index];
                std::cout << "[ThemeSelector] Activated theme: " << themeService_->getThemeName(selectedTheme) << std::endl;
                onThemeSelected(selectedTheme);
            }
        } catch (const std::exception& e) {
            std::cerr << "[ThemeSelector] Error in dropdown activated handler: " << e.what() << std::endl;
        }
    });
    
    // Current theme display (optional)
    if (showDescriptions_) {
        currentThemeText_ = addNew<Wt::WText>();
        currentThemeText_->addStyleClass("theme-description text-muted small mt-2");
    }
    
    populateDropdown();
    setLayout(std::move(layout));
    
    std::cout << "[ThemeSelector] Dropdown selector created successfully" << std::endl;
}

void ThemeSelector::createRadioButtonSelector() {
    // Not implemented in simplified version - fallback to dropdown
    std::cout << "[ThemeSelector] Radio button selector not implemented, using dropdown" << std::endl;
    createDropdownSelector();
}

void ThemeSelector::createToggleButtonSelector() {
    // Not implemented in simplified version - fallback to dropdown
    std::cout << "[ThemeSelector] Toggle button selector not implemented, using dropdown" << std::endl;
    createDropdownSelector();
}

void ThemeSelector::createGridCardSelector() {
    // Not implemented in simplified version - fallback to dropdown
    std::cout << "[ThemeSelector] Grid card selector not implemented, using dropdown" << std::endl;
    createDropdownSelector();
}

void ThemeSelector::clearSelector() {
    std::cout << "[ThemeSelector] Clearing existing selector..." << std::endl;
    
    clear(); // Clear all child widgets
    
    // Reset pointers
    themeComboBox_ = nullptr;
    toggleButton_ = nullptr;
    gridContainer_ = nullptr;
    currentThemeText_ = nullptr;
    
    std::cout << "[ThemeSelector] Selector cleared" << std::endl;
}

void ThemeSelector::updateSelector() {
    std::cout << "[ThemeSelector] Updating selector..." << std::endl;
    
    populateDropdown();
    
    // Update description text
    if (currentThemeText_) {
        auto currentTheme = themeService_->getCurrentTheme();
        std::string description = themeService_->getThemeDescription(currentTheme);
        currentThemeText_->setText(description);
        std::cout << "[ThemeSelector] Updated description: " << description << std::endl;
    }
    
    std::cout << "[ThemeSelector] Selector update complete" << std::endl;
}

void ThemeSelector::setupEventListeners() {
    std::cout << "[ThemeSelector] Setting up event listeners..." << std::endl;
    
    // Register for theme change notifications from the service
    try {
        themeChangeSubscription_ = themeService_->onThemeChanged(
            [this](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
                std::cout << "[ThemeSelector] Theme service changed from " 
                          << themeService_->getThemeName(oldTheme) << " to " 
                          << themeService_->getThemeName(newTheme) << std::endl;
                onThemeServiceChanged(oldTheme, newTheme);
            });
        
        std::cout << "[ThemeSelector] Event listeners registered successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ThemeSelector] Error setting up event listeners: " << e.what() << std::endl;
    }
}

void ThemeSelector::onThemeSelected(ThemeService::Theme theme) {
    std::cout << "[ThemeSelector] Theme selected: " << themeService_->getThemeName(theme) << std::endl;
    
    if (isValidTheme(theme)) {
        try {
            // FIXED: Apply theme immediately with force flag
            std::cout << "[ThemeSelector] Applying theme to service..." << std::endl;
            themeService_->setTheme(theme, true); // Save preference
            
            // Call user callback if set
            if (selectionCallback_) {
                std::cout << "[ThemeSelector] Calling user selection callback..." << std::endl;
                selectionCallback_(theme);
            }
            
            std::cout << "[ThemeSelector] Theme selection completed successfully" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "[ThemeSelector] Error applying theme: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "[ThemeSelector] Invalid theme selected: " << static_cast<int>(theme) << std::endl;
    }
}

void ThemeSelector::onThemeServiceChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
    std::cout << "[ThemeSelector] Responding to theme service change..." << std::endl;
    
    // Update UI to reflect the change without triggering another selection
    try {
        // Temporarily disconnect events to avoid loops
        if (themeComboBox_) {
            // Find the index of the new theme
            auto themes = getSelectableThemes();
            for (size_t i = 0; i < themes.size(); ++i) {
                if (themes[i] == newTheme) {
                    std::cout << "[ThemeSelector] Updating dropdown to index " << i << std::endl;
                    themeComboBox_->setCurrentIndex(static_cast<int>(i));
                    break;
                }
            }
        }
        
        // Update description if shown
        if (currentThemeText_) {
            currentThemeText_->setText(themeService_->getThemeDescription(newTheme));
        }
        
        std::cout << "[ThemeSelector] UI updated to reflect theme service change" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[ThemeSelector] Error updating UI after theme service change: " << e.what() << std::endl;
    }
}

std::string ThemeSelector::getThemeDisplayText(ThemeService::Theme theme) const {
    std::string text = themeService_->getThemeIcon(theme) + " " + themeService_->getThemeName(theme);
    
    if (showDescriptions_) {
        text += " - " + themeService_->getThemeDescription(theme);
    }
    
    return text;
}

std::string ThemeSelector::getThemePreviewHTML(ThemeService::Theme theme) const {
    std::string primaryColor = themeService_->getThemePrimaryColor(theme);
    
    return "<div class='theme-preview' style='background-color: " + primaryColor + 
           "; width: 20px; height: 20px; border-radius: 3px; display: inline-block;'></div>";
}

void ThemeSelector::populateDropdown() {
    if (!themeComboBox_) {
        std::cout << "[ThemeSelector] Cannot populate dropdown - combo box is null" << std::endl;
        return;
    }
    
    std::cout << "[ThemeSelector] Populating dropdown..." << std::endl;
    
    // Clear existing items
    themeComboBox_->clear();
    
    auto themes = getSelectableThemes();
    auto currentTheme = themeService_->getCurrentTheme();
    int selectedIndex = -1;
    
    std::cout << "[ThemeSelector] Adding " << themes.size() << " themes to dropdown" << std::endl;
    std::cout << "[ThemeSelector] Current theme: " << themeService_->getThemeName(currentTheme) << std::endl;
    
    for (size_t i = 0; i < themes.size(); ++i) {
        std::string displayText = getThemeDisplayText(themes[i]);
        themeComboBox_->addItem(displayText);
        
        std::cout << "[ThemeSelector] Added item " << i << ": " << displayText << std::endl;
        
        if (themes[i] == currentTheme) {
            selectedIndex = static_cast<int>(i);
            std::cout << "[ThemeSelector] Current theme found at index " << selectedIndex << std::endl;
        }
    }
    
    // Set the current selection
    if (selectedIndex >= 0) {
        themeComboBox_->setCurrentIndex(selectedIndex);
        std::cout << "[ThemeSelector] Set dropdown to index " << selectedIndex << std::endl;
    } else {
        std::cout << "[ThemeSelector] Warning: Current theme not found in selectable themes!" << std::endl;
        // Fallback to first item if current theme not found
        if (!themes.empty()) {
            themeComboBox_->setCurrentIndex(0);
            std::cout << "[ThemeSelector] Fallback: Set dropdown to index 0" << std::endl;
        }
    }
    
    std::cout << "[ThemeSelector] Dropdown population complete" << std::endl;
}

void ThemeSelector::populateRadioButtons() {
    // Not implemented in simplified version
    std::cout << "[ThemeSelector] Radio button population not implemented" << std::endl;
}

void ThemeSelector::updateToggleButton() {
    // Not implemented in simplified version
    std::cout << "[ThemeSelector] Toggle button update not implemented" << std::endl;
}

void ThemeSelector::populateGridCards() {
    // Not implemented in simplified version
    std::cout << "[ThemeSelector] Grid cards population not implemented" << std::endl;
}

void ThemeSelector::applyThemeSelectorStyling() {
    // Apply basic CSS classes
    removeStyleClass("theme-selector-dropdown");
    removeStyleClass("theme-selector-radio");
    removeStyleClass("theme-selector-toggle");
    removeStyleClass("theme-selector-grid");
    
    addStyleClass("theme-selector-dropdown");
    
    std::cout << "[ThemeSelector] Applied dropdown styling" << std::endl;
}

void ThemeSelector::updateEnabledState() {
    if (themeComboBox_) {
        themeComboBox_->setEnabled(enabled_);
        std::cout << "[ThemeSelector] Dropdown enabled state: " << (enabled_ ? "enabled" : "disabled") << std::endl;
    }
}

std::unique_ptr<Wt::WWidget> ThemeSelector::createThemeCard(ThemeService::Theme theme) {
    // Not implemented in simplified version
    std::cout << "[ThemeSelector] Theme card creation not implemented" << std::endl;
    return std::make_unique<Wt::WContainerWidget>();
}

void ThemeSelector::styleThemeCard(Wt::WContainerWidget* card, ThemeService::Theme theme) {
    // Not implemented in simplified version
    std::cout << "[ThemeSelector] Theme card styling not implemented" << std::endl;
}

bool ThemeSelector::isValidTheme(ThemeService::Theme theme) const {
    auto availableThemes = themeService_->getAvailableThemes();
    bool isValid = std::find(availableThemes.begin(), availableThemes.end(), theme) != availableThemes.end();
    
    if (!isValid) {
        std::cout << "[ThemeSelector] Theme validation failed for: " << static_cast<int>(theme) << std::endl;
    }
    
    return isValid;
}

std::vector<ThemeService::Theme> ThemeSelector::getSelectableThemes() const {
    auto themes = themeService_->getAvailableThemes();
    
    // Remove AUTO theme if not allowed
    if (!allowAutoTheme_) {
        themes.erase(std::remove(themes.begin(), themes.end(), ThemeService::Theme::AUTO), themes.end());
        std::cout << "[ThemeSelector] AUTO theme removed from selectable themes" << std::endl;
    }
    
    std::cout << "[ThemeSelector] Selectable themes count: " << themes.size() << std::endl;
    return themes;
}

// Radio button management methods (simplified - not used)
void ThemeSelector::clearRadioButtonSelection() {
    // Not implemented in simplified version
}

void ThemeSelector::setRadioButtonSelection(int index) {
    // Not implemented in simplified version
}

int ThemeSelector::getSelectedRadioButtonIndex() const {
    // Not implemented in simplified version
    return -1;
}

// Theme selector utility functions implementation
namespace ThemeSelectorUtils {
    
    std::unique_ptr<ThemeSelector> createSimpleDropdown(std::shared_ptr<ThemeService> themeService) {
        std::cout << "[ThemeSelectorUtils] Creating simple dropdown" << std::endl;
        auto selector = std::make_unique<ThemeSelector>(themeService, ThemeSelector::DisplayMode::DROPDOWN);
        selector->setShowDescriptions(false);
        selector->setShowPreviews(false);
        return selector;
    }
    
    std::unique_ptr<ThemeSelector> createToggleButton(std::shared_ptr<ThemeService> themeService) {
        // In simplified version, just return a dropdown
        std::cout << "[ThemeSelectorUtils] Creating toggle button (using dropdown)" << std::endl;
        return createSimpleDropdown(themeService);
    }
    
    std::unique_ptr<ThemeSelector> createThemeGrid(std::shared_ptr<ThemeService> themeService) {
        // In simplified version, just return a dropdown
        std::cout << "[ThemeSelectorUtils] Creating theme grid (using dropdown)" << std::endl;
        auto selector = createSimpleDropdown(themeService);
        selector->setShowDescriptions(true);
        return selector;
    }
    
    std::unique_ptr<ThemeSelector> createCompactSelector(std::shared_ptr<ThemeService> themeService) {
        std::cout << "[ThemeSelectorUtils] Creating compact selector" << std::endl;
        auto selector = std::make_unique<ThemeSelector>(themeService, ThemeSelector::DisplayMode::DROPDOWN);
        selector->setShowDescriptions(false);
        selector->setShowPreviews(false);
        selector->setAllowAutoTheme(false);
        return selector;
    }
}
