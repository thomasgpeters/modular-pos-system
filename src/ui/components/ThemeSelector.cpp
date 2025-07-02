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
    
    // Initialize UI and event handling
    setupEventListeners();
    initializeUI();
    
    std::cout << "[ThemeSelector] Initialized in simplified mode" << std::endl;
}

void ThemeSelector::setDisplayMode(DisplayMode mode) {
    // For now, always use dropdown mode for compatibility
    displayMode_ = DisplayMode::DROPDOWN;
    initializeUI();
}

void ThemeSelector::setShowDescriptions(bool show) {
    showDescriptions_ = show;
    updateSelector();
}

void ThemeSelector::setShowPreviews(bool show) {
    showPreviews_ = show;
    updateSelector();
}

void ThemeSelector::setAllowAutoTheme(bool allow) {
    allowAutoTheme_ = allow;
    updateSelector();
}

void ThemeSelector::setThemeSelectionCallback(ThemeSelectionCallback callback) {
    selectionCallback_ = callback;
}

ThemeService::Theme ThemeSelector::getSelectedTheme() const {
    return themeService_->getCurrentTheme();
}

void ThemeSelector::setSelectedTheme(ThemeService::Theme theme) {
    if (isValidTheme(theme)) {
        themeService_->setTheme(theme);
        updateSelector();
    }
}

void ThemeSelector::refresh() {
    updateSelector();
}

void ThemeSelector::setEnabled(bool enabled) {
    enabled_ = enabled;
    updateEnabledState();
}

void ThemeSelector::initializeUI() {
    // Clear existing UI
    clearSelector();
    
    // Always create dropdown for compatibility
    createDropdownSelector();
    
    // Apply styling and update state
    applyThemeSelectorStyling();
    updateSelector();
    updateEnabledState();
}

void ThemeSelector::createDropdownSelector() {
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Theme selection dropdown
    themeComboBox_ = addNew<Wt::WComboBox>();
    themeComboBox_->addStyleClass("form-select theme-selector-dropdown");
    
    // Connect change event
    themeComboBox_->changed().connect([this]() {
        int index = themeComboBox_->currentIndex();
        auto themes = getSelectableThemes();
        if (index >= 0 && index < static_cast<int>(themes.size())) {
            onThemeSelected(themes[index]);
        }
    });
    
    // Current theme display (optional)
    if (showDescriptions_) {
        currentThemeText_ = addNew<Wt::WText>();
        currentThemeText_->addStyleClass("theme-description text-muted small mt-2");
    }
    
    populateDropdown();
    setLayout(std::move(layout));
}

void ThemeSelector::createRadioButtonSelector() {
    // Not implemented in simplified version - fallback to dropdown
    createDropdownSelector();
}

void ThemeSelector::createToggleButtonSelector() {
    // Not implemented in simplified version - fallback to dropdown
    createDropdownSelector();
}

void ThemeSelector::createGridCardSelector() {
    // Not implemented in simplified version - fallback to dropdown
    createDropdownSelector();
}

void ThemeSelector::clearSelector() {
    clear(); // Clear all child widgets
    
    // Reset pointers
    themeComboBox_ = nullptr;
    toggleButton_ = nullptr;
    gridContainer_ = nullptr;
    currentThemeText_ = nullptr;
}

void ThemeSelector::updateSelector() {
    populateDropdown();
    
    // Update description text
    if (currentThemeText_) {
        auto currentTheme = themeService_->getCurrentTheme();
        currentThemeText_->setText(themeService_->getThemeDescription(currentTheme));
    }
}

void ThemeSelector::setupEventListeners() {
    // Register for theme change notifications from the service
    themeChangeSubscription_ = themeService_->onThemeChanged(
        [this](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
            onThemeServiceChanged(oldTheme, newTheme);
        });
}

void ThemeSelector::onThemeSelected(ThemeService::Theme theme) {
    if (isValidTheme(theme)) {
        themeService_->setTheme(theme);
        
        // Call user callback if set
        if (selectionCallback_) {
            selectionCallback_(theme);
        }
        
        std::cout << "[ThemeSelector] Theme selected: " 
                  << themeService_->getThemeName(theme) << std::endl;
    }
}

void ThemeSelector::onThemeServiceChanged(ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
    // Update UI to reflect the change
    updateSelector();
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
    if (!themeComboBox_) return;
    
    themeComboBox_->clear();
    
    auto themes = getSelectableThemes();
    auto currentTheme = themeService_->getCurrentTheme();
    int selectedIndex = -1;
    
    for (size_t i = 0; i < themes.size(); ++i) {
        std::string displayText = getThemeDisplayText(themes[i]);
        themeComboBox_->addItem(displayText);
        
        if (themes[i] == currentTheme) {
            selectedIndex = static_cast<int>(i);
        }
    }
    
    if (selectedIndex >= 0) {
        themeComboBox_->setCurrentIndex(selectedIndex);
    }
}

void ThemeSelector::populateRadioButtons() {
    // Not implemented in simplified version
}

void ThemeSelector::updateToggleButton() {
    // Not implemented in simplified version
}

void ThemeSelector::populateGridCards() {
    // Not implemented in simplified version
}

void ThemeSelector::applyThemeSelectorStyling() {
    // Apply basic CSS classes
    removeStyleClass("theme-selector-dropdown");
    removeStyleClass("theme-selector-radio");
    removeStyleClass("theme-selector-toggle");
    removeStyleClass("theme-selector-grid");
    
    addStyleClass("theme-selector-dropdown");
}

void ThemeSelector::updateEnabledState() {
    if (themeComboBox_) themeComboBox_->setEnabled(enabled_);
}

std::unique_ptr<Wt::WWidget> ThemeSelector::createThemeCard(ThemeService::Theme theme) {
    // Not implemented in simplified version
    return std::make_unique<Wt::WContainerWidget>();
}

void ThemeSelector::styleThemeCard(Wt::WContainerWidget* card, ThemeService::Theme theme) {
    // Not implemented in simplified version
}

bool ThemeSelector::isValidTheme(ThemeService::Theme theme) const {
    auto availableThemes = themeService_->getAvailableThemes();
    return std::find(availableThemes.begin(), availableThemes.end(), theme) != availableThemes.end();
}

std::vector<ThemeService::Theme> ThemeSelector::getSelectableThemes() const {
    auto themes = themeService_->getAvailableThemes();
    
    // Remove AUTO theme if not allowed
    if (!allowAutoTheme_) {
        themes.erase(std::remove(themes.begin(), themes.end(), ThemeService::Theme::AUTO), themes.end());
    }
    
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
        auto selector = std::make_unique<ThemeSelector>(themeService, ThemeSelector::DisplayMode::DROPDOWN);
        selector->setShowDescriptions(false);
        selector->setShowPreviews(false);
        return selector;
    }
    
    std::unique_ptr<ThemeSelector> createToggleButton(std::shared_ptr<ThemeService> themeService) {
        // In simplified version, just return a dropdown
        return createSimpleDropdown(themeService);
    }
    
    std::unique_ptr<ThemeSelector> createThemeGrid(std::shared_ptr<ThemeService> themeService) {
        // In simplified version, just return a dropdown
        auto selector = createSimpleDropdown(themeService);
        selector->setShowDescriptions(true);
        return selector;
    }
    
    std::unique_ptr<ThemeSelector> createCompactSelector(std::shared_ptr<ThemeService> themeService) {
        auto selector = std::make_unique<ThemeSelector>(themeService, ThemeSelector::DisplayMode::DROPDOWN);
        selector->setShowDescriptions(false);
        selector->setShowPreviews(false);
        selector->setAllowAutoTheme(false);
        return selector;
    }
}
