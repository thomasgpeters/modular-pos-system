#include "../../../include/ui/components/ThemeSelector.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WDialog.h>
#include <Wt/WGroupBox.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WApplication.h>

#include <iostream>
#include <algorithm>

ThemeSelector::ThemeSelector(std::shared_ptr<ThemeService> themeService,
                           std::shared_ptr<EventManager> eventManager,
                           DisplayMode mode)
    : themeService_(themeService)
    , eventManager_(eventManager)
    , displayMode_(mode)
    , enabled_(true)
    , themeComboBox_(nullptr)
    , themeButton_(nullptr)
    , themeDescriptionText_(nullptr)
    , themePreviewContainer_(nullptr) {
    
    initializeUI();
    setupEventListeners();
    refresh();
}

void ThemeSelector::initializeUI() {
    addStyleClass("theme-selector");
    
    rebuildUI();
    
    std::cout << "✓ ThemeSelector UI initialized in " 
              << (displayMode_ == COMPACT ? "compact" : 
                  displayMode_ == DETAILED ? "detailed" : "button-only") 
              << " mode" << std::endl;
}

void ThemeSelector::setupEventListeners() {
    if (!eventManager_) {
        std::cerr << "Warning: EventManager not available for ThemeSelector" << std::endl;
        return;
    }
    
    // Subscribe to theme change events
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::THEME_CHANGED,
            [this](const std::any& data) { handleThemeChanged(data); }));
    
    // Subscribe to configuration changes that might affect themes
    eventSubscriptions_.push_back(
        eventManager_->subscribe(POSEvents::CONFIGURATION_CHANGED,
            [this](const std::any& data) { handleThemeListUpdated(data); }));
    
    std::cout << "✓ ThemeSelector event listeners setup complete" << std::endl;
}

void ThemeSelector::buildCompactMode() {
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Theme label
    auto label = std::make_unique<Wt::WLabel>("Theme:");
    label->addStyleClass("theme-selector-label me-2");
    layout->addWidget(std::move(label));
    
    // Theme dropdown
    themeComboBox_ = addWidget(std::make_unique<Wt::WComboBox>());
    themeComboBox_->addStyleClass("theme-selector-combo form-control-sm");
    themeComboBox_->changed().connect([this]() {
        onThemeSelectionChanged();
    });
    layout->addWidget(themeComboBox_);
    
    setLayout(std::move(layout));
}

void ThemeSelector::buildDetailedMode() {
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Header with theme selection
    auto headerContainer = std::make_unique<Wt::WContainerWidget>();
    auto headerLayout = std::make_unique<Wt::WHBoxLayout>();
    
    auto label = std::make_unique<Wt::WLabel>("Theme:");
    label->addStyleClass("theme-selector-label me-2");
    headerLayout->addWidget(std::move(label));
    
    themeComboBox_ = headerContainer->addWidget(std::make_unique<Wt::WComboBox>());
    themeComboBox_->addStyleClass("theme-selector-combo form-control-sm");
    themeComboBox_->changed().connect([this]() {
        onThemeSelectionChanged();
    });
    headerLayout->addWidget(themeComboBox_, 1);
    
    headerContainer->setLayout(std::move(headerLayout));
    layout->addWidget(std::move(headerContainer));
    
    // Theme description
    themeDescriptionText_ = addWidget(std::make_unique<Wt::WText>());
    themeDescriptionText_->addStyleClass("theme-description text-muted small mt-1");
    layout->addWidget(themeDescriptionText_);
    
    // Theme preview
    themePreviewContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    themePreviewContainer_->addStyleClass("theme-preview mt-2");
    layout->addWidget(themePreviewContainer_);
    
    setLayout(std::move(layout));
}

void ThemeSelector::buildButtonOnlyMode() {
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    themeButton_ = addWidget(std::make_unique<Wt::WPushButton>("⚙️ Themes"));
    themeButton_->addStyleClass("btn btn-outline-secondary btn-sm");
    themeButton_->setToolTip("Select application theme");
    themeButton_->clicked().connect([this]() {
        onThemeButtonClicked();
    });
    layout->addWidget(themeButton_);
    
    setLayout(std::move(layout));
}

void ThemeSelector::refresh() {
    if (!themeService_) {
        std::cerr << "Error: ThemeService not available for theme refresh" << std::endl;
        return;
    }
    
    updateThemeOptions();
    updateThemePreview();
    
    std::cout << "ThemeSelector refreshed" << std::endl;
}

void ThemeSelector::setDisplayMode(DisplayMode mode) {
    if (displayMode_ != mode) {
        displayMode_ = mode;
        rebuildUI();
        refresh();
        std::cout << "ThemeSelector display mode changed" << std::endl;
    }
}

ThemeSelector::DisplayMode ThemeSelector::getDisplayMode() const {
    return displayMode_;
}

void ThemeSelector::setEnabled(bool enabled) {
    enabled_ = enabled;
    
    if (themeComboBox_) {
        themeComboBox_->setEnabled(enabled);
    }
    
    if (themeButton_) {
        themeButton_->setEnabled(enabled);
    }
    
    std::cout << "ThemeSelector " << (enabled ? "enabled" : "disabled") << std::endl;
}

bool ThemeSelector::isEnabled() const {
    return enabled_;
}

std::string ThemeSelector::getCurrentThemeId() const {
    if (!themeService_) {
        return "";
    }
    
    return themeService_->getCurrentThemeId();
}

bool ThemeSelector::setCurrentTheme(const std::string& themeId) {
    if (!themeService_) {
        return false;
    }
    
    bool success = themeService_->applyTheme(themeId);
    if (success) {
        selectCurrentThemeInComboBox();
        updateThemePreview();
    }
    
    return success;
}

// =================================================================
// Event Handlers
// =================================================================

void ThemeSelector::handleThemeChanged(const std::any& eventData) {
    try {
        auto themeData = std::any_cast<POSEvents::ThemeEventData>(eventData);
        
        std::cout << "Theme changed event received: " << themeData.themeId << std::endl;
        
        selectCurrentThemeInComboBox();
        updateThemePreview();
        
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Error handling theme changed event: invalid data type" << std::endl;
    }
}

void ThemeSelector::handleThemeListUpdated(const std::any& eventData) {
    std::cout << "Theme list updated event received, refreshing selector" << std::endl;
    refresh();
}

// =================================================================
// UI Action Handlers
// =================================================================

void ThemeSelector::onThemeSelectionChanged() {
    if (!themeComboBox_ || !themeService_) {
        return;
    }
    
    int selectedIndex = themeComboBox_->currentIndex();
    if (selectedIndex < 0) {
        return;
    }
    
    auto enabledThemes = themeService_->getEnabledThemes();
    if (selectedIndex >= static_cast<int>(enabledThemes.size())) {
        return;
    }
    
    const auto& selectedTheme = enabledThemes[selectedIndex];
    
    std::cout << "Theme selection changed to: " << selectedTheme.name << std::endl;
    
    bool success = themeService_->applyTheme(selectedTheme.id);
    if (!success) {
        std::cerr << "Failed to apply selected theme: " << selectedTheme.id << std::endl;
        
        // Revert to current theme in UI
        selectCurrentThemeInComboBox();
    }
}

void ThemeSelector::onThemeButtonClicked() {
    showThemeSelectionDialog();
}

void ThemeSelector::onApplyThemeClicked(const std::string& themeId) {
    if (!themeService_) {
        return;
    }
    
    bool success = themeService_->applyTheme(themeId);
    if (success) {
        std::cout << "Theme applied successfully: " << themeId << std::endl;
    } else {
        std::cerr << "Failed to apply theme: " << themeId << std::endl;
    }
}

void ThemeSelector::showThemeSelectionDialog() {
    if (!themeService_) {
        return;
    }
    
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Select Theme"));
    dialog->setModal(true);
    dialog->setResizable(true);
    dialog->resize(500, 400);
    dialog->addStyleClass("theme-selection-dialog");
    
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("theme-selection-content p-3");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    
    // Instructions
    auto instructions = std::make_unique<Wt::WText>(
        "Select a theme to change the appearance of the application:");
    instructions->addStyleClass("mb-3");
    layout->addWidget(std::move(instructions));
    
    // Theme options
    auto buttonGroup = std::make_shared<Wt::WButtonGroup>();
    auto enabledThemes = themeService_->getEnabledThemes();
    std::string currentThemeId = themeService_->getCurrentThemeId();
    
    for (const auto& theme : enabledThemes) {
        auto themeOption = std::make_unique<Wt::WContainerWidget>();
        themeOption->addStyleClass("theme-option border rounded p-3 mb-2");
        
        auto optionLayout = std::make_unique<Wt::WHBoxLayout>();
        
        // Radio button
        auto radioButton = std::make_unique<Wt::WRadioButton>();
        radioButton->setChecked(theme.id == currentThemeId);
        buttonGroup->addButton(radioButton.get());
        optionLayout->addWidget(std::move(radioButton));
        
        // Theme info
        auto infoContainer = std::make_unique<Wt::WContainerWidget>();
        auto infoLayout = std::make_unique<Wt::WVBoxLayout>();
        
        auto nameText = std::make_unique<Wt::WText>(theme.name);
        nameText->addStyleClass("font-weight-bold");
        infoLayout->addWidget(std::move(nameText));
        
        auto descText = std::make_unique<Wt::WText>(theme.description);
        descText->addStyleClass("text-muted small");
        infoLayout->addWidget(std::move(descText));
        
        infoContainer->setLayout(std::move(infoLayout));
        optionLayout->addWidget(std::move(infoContainer), 1);
        
        // Theme preview
        auto preview = createThemePreview(theme);
        optionLayout->addWidget(std::move(preview));
        
        themeOption->setLayout(std::move(optionLayout));
        
        // Click handler for the entire option
        themeOption->clicked().connect([this, &theme, buttonGroup, dialog]() {
            // Find the radio button for this theme and select it
            for (auto button : buttonGroup->buttons()) {
                auto radioBtn = dynamic_cast<Wt::WRadioButton*>(button);
                if (radioBtn) {
                    radioBtn->setChecked(false);
                }
            }
            // This is simplified - in practice you'd need to track which button belongs to which theme
            onApplyThemeClicked(theme.id);
            dialog->accept();
        });
        
        layout->addWidget(std::move(themeOption));
    }
    
    container->setLayout(std::move(layout));
    dialog->contents()->addWidget(std::move(container));
    
    // Dialog buttons
    auto footer = std::make_unique<Wt::WContainerWidget>();
    footer->addStyleClass("dialog-footer");
    
    auto closeButton = std::make_unique<Wt::WPushButton>("Close");
    closeButton->addStyleClass("btn btn-secondary");
    closeButton->clicked().connect([dialog]() {
        dialog->reject();
    });
    footer->addWidget(std::move(closeButton));
    
    dialog->footer()->addWidget(std::move(footer));
    
    dialog->show();
    
    std::cout << "Theme selection dialog shown with " << enabledThemes.size() << " themes" << std::endl;
}

// =================================================================
// Helper Methods
// =================================================================

void ThemeSelector::updateThemeOptions() {
    if (themeComboBox_) {
        populateThemeComboBox();
        selectCurrentThemeInComboBox();
    }
    
    // Update button text if in button mode
    if (themeButton_ && themeService_) {
        auto currentTheme = themeService_->getCurrentTheme();
        if (currentTheme) {
            themeButton_->setText("⚙️ " + currentTheme->name);
        }
    }
}

void ThemeSelector::updateThemePreview() {
    if (!themePreviewContainer_ || !themeService_) {
        return;
    }
    
    themePreviewContainer_->clear();
    
    auto currentTheme = themeService_->getCurrentTheme();
    if (!currentTheme) {
        return;
    }
    
    // Update description
    if (themeDescriptionText_) {
        themeDescriptionText_->setText(currentTheme->description);
    }
    
    // Create preview
    auto preview = createThemePreview(*currentTheme);
    themePreviewContainer_->addWidget(std::move(preview));
}

void ThemeSelector::populateThemeComboBox() {
    if (!themeComboBox_ || !themeService_) {
        return;
    }
    
    themeComboBox_->clear();
    
    auto enabledThemes = themeService_->getEnabledThemes();
    
    for (const auto& theme : enabledThemes) {
        themeComboBox_->addItem(getThemeDisplayText(theme));
    }
    
    std::cout << "Populated theme combo box with " << enabledThemes.size() << " themes" << std::endl;
}

void ThemeSelector::selectCurrentThemeInComboBox() {
    if (!themeComboBox_ || !themeService_) {
        return;
    }
    
    std::string currentThemeId = themeService_->getCurrentThemeId();
    auto enabledThemes = themeService_->getEnabledThemes();
    
    for (size_t i = 0; i < enabledThemes.size(); ++i) {
        if (enabledThemes[i].id == currentThemeId) {
            themeComboBox_->setCurrentIndex(static_cast<int>(i));
            break;
        }
    }
}

std::string ThemeSelector::getThemeDisplayText(const ThemeService::ThemeInfo& theme) const {
    return theme.name;
}

std::unique_ptr<Wt::WWidget> ThemeSelector::createThemePreview(const ThemeService::ThemeInfo& theme) {
    auto preview = std::make_unique<Wt::WContainerWidget>();
    preview->addStyleClass("theme-preview-colors d-flex");
    
    if (theme.previewColors.empty()) {
        // Default preview if no colors specified
        auto defaultText = std::make_unique<Wt::WText>("No preview");
        defaultText->addStyleClass("text-muted small");
        preview->addWidget(std::move(defaultText));
        return std::move(preview);
    }
    
    // Create color swatches
    for (const auto& color : theme.previewColors) {
        if (color.empty()) continue;
        
        auto colorSwatch = std::make_unique<Wt::WContainerWidget>();
        colorSwatch->addStyleClass("theme-preview-color me-1");
        colorSwatch->setWidth(PREVIEW_COLOR_SIZE);
        colorSwatch->setHeight(PREVIEW_COLOR_SIZE);
        
        // Set background color using inline style
        std::string style = "background-color: " + color + "; border: 1px solid #ccc; border-radius: 2px;";
        colorSwatch->decorationStyle().font().setSize(Wt::FontSize::XXSmall);
        
        // Create a styled div with the color
        auto colorDiv = std::make_unique<Wt::WText>();
        colorDiv->setText("&nbsp;");
        colorDiv->setInline(false);
        colorDiv->setAttributeValue("style", style);
        colorSwatch->addWidget(std::move(colorDiv));
        
        preview->addWidget(std::move(colorSwatch));
    }
    
    return std::move(preview);
}

void ThemeSelector::clearUI() {
    clear();
    themeComboBox_ = nullptr;
    themeButton_ = nullptr;
    themeDescriptionText_ = nullptr;
    themePreviewContainer_ = nullptr;
}

void ThemeSelector::rebuildUI() {
    clearUI();
    
    switch (displayMode_) {
        case COMPACT:
            buildCompactMode();
            break;
        case DETAILED:
            buildDetailedMode();
            break;
        case BUTTON_ONLY:
            buildButtonOnlyMode();
            break;
    }
}
