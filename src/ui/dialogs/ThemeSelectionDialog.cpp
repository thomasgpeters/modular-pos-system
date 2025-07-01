//=============================================================================
// ThemeSelectionDialog.cpp
//=============================================================================

#include "../../../include/ui/dialogs/ThemeSelectionDialog.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGridLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WBreak.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>

#include <iostream>

ThemeSelectionDialog::ThemeSelectionDialog(std::shared_ptr<ThemeService> themeService,
                                           std::shared_ptr<EventManager> eventManager,
                                           ThemeSelectionCallback callback)
    : WDialog("Theme Selection"), themeService_(themeService), eventManager_(eventManager),
      selectionCallback_(callback), showPreviews_(true), themesContainer_(nullptr),
      themeButtonGroup_(nullptr), applyButton_(nullptr), cancelButton_(nullptr),
      previewButton_(nullptr) {
    
    // Set dialog properties
    setModal(true);
    setResizable(true);
    resize(700, 500);
    
    // Store original theme
    if (themeService_) {
        originalThemeId_ = themeService_->getCurrentThemeId();
        selectedThemeId_ = originalThemeId_;
    }
    
    // Load available themes
    refreshThemes();
    
    // Create dialog content
    createDialogContent();
    setupEventHandlers();
}

void ThemeSelectionDialog::createDialogContent() {
    auto content = contents()->addNew<Wt::WContainerWidget>();
    content->addStyleClass("preferences-dialog");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);
    
    // Title and description
    auto titleText = std::make_unique<Wt::WText>("Choose Your Theme");
    titleText->addStyleClass("h4 mb-3");
    layout->addWidget(std::move(titleText));
    
    auto descText = std::make_unique<Wt::WText>("Select a theme to customize the appearance of your POS system:");
    descText->addStyleClass("text-muted mb-4");
    layout->addWidget(std::move(descText));
    
    // Themes grid
    auto themesGrid = createThemesGrid();
    layout->addWidget(std::move(themesGrid));
    
    // Action buttons
    auto actionButtons = createActionButtons();
    layout->addWidget(std::move(actionButtons));
    
    content->setLayout(std::move(layout));
}

std::unique_ptr<Wt::WContainerWidget> ThemeSelectionDialog::createThemesGrid() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("themes-selection-container");
    
    themesContainer_ = container.get();
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    layout->setSpacing(10);
    
    themeButtonGroup_ = std::make_unique<Wt::WButtonGroup>();
    
    // Create theme cards
    auto gridLayout = std::make_unique<Wt::WGridLayout>();
    gridLayout->setHorizontalSpacing(15);
    gridLayout->setVerticalSpacing(15);
    
    int row = 0;
    int col = 0;
    const int maxCols = 2;
    
    for (const auto& theme : availableThemes_) {
        auto themeCard = createThemeCard(theme);
        gridLayout->addWidget(std::move(themeCard), row, col);
        
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
    
    auto gridContainer = std::make_unique<Wt::WContainerWidget>();
    gridContainer->setLayout(std::move(gridLayout));
    layout->addWidget(std::move(gridContainer));
    
    container->setLayout(std::move(layout));
    
    return container;
}

std::unique_ptr<Wt::WContainerWidget> ThemeSelectionDialog::createThemeCard(const ThemeService::ThemeInfo& theme) {
    auto card = std::make_unique<Wt::WContainerWidget>();
    card->addStyleClass("theme-card");
    
    auto layout = std::make_unique<Wt::WVBoxLayout>();
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);
    
    // Radio button for selection
    auto radioButton = std::make_unique<Wt::WRadioButton>(theme.name);
    radioButton->addStyleClass("theme-radio");
    
    auto* radioPtr = radioButton.get();
    themeButtonGroup_->addButton(radioButton.get());
    themeButtons_.push_back(radioPtr);
    
    // Set checked if current theme
    if (theme.id == selectedThemeId_) {
        radioButton->setChecked(true);
    }
    
    layout->addWidget(std::move(radioButton));
    
    // Theme preview (if enabled)
    if (showPreviews_) {
        auto preview = createColorPreview(theme);
        layout->addWidget(std::move(preview));
    }
    
    // Theme description
    auto description = std::make_unique<Wt::WText>(theme.description);
    description->addStyleClass("theme-description text-muted small");
    layout->addWidget(std::move(description));
    
    card->setLayout(std::move(layout));
    
    // Make card clickable to select theme
    card->clicked().connect([this, radioPtr, theme]() {
        radioPtr->setChecked(true);
        selectedThemeId_ = theme.id;
        updateApplyButton();
    });
    
    return card;
}

std::unique_ptr<Wt::WContainerWidget> ThemeSelectionDialog::createColorPreview(const ThemeService::ThemeInfo& theme) {
    auto preview = std::make_unique<Wt::WContainerWidget>();
    preview->addStyleClass("theme-preview " + theme.id);
    preview->setHeight(60);
    
    // Create color preview based on theme
    std::string style;
    if (theme.id == "bootstrap") {
        style = "background: linear-gradient(45deg, #007bff, #0056b3);";
    } else if (theme.id == "classic") {
        style = "background: linear-gradient(45deg, #f8f9fa, #e9ecef);";
    } else if (theme.id == "professional") {
        style = "background: linear-gradient(45deg, #2c3e50, #3498db);";
    } else if (theme.id == "colorful") {
        style = "background: linear-gradient(45deg, #667eea, #764ba2);";
    } else {
        style = "background: linear-gradient(45deg, #6c757d, #495057);";
    }
    
    preview->setAttributeValue("style", style + " border-radius: 8px; border: 2px solid #dee2e6;");
    
    return preview;
}

std::unique_ptr<Wt::WContainerWidget> ThemeSelectionDialog::createActionButtons() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("modal-footer");
    
    auto layout = std::make_unique<Wt::WHBoxLayout>();
    
    // Preview button
    previewButton_ = layout->addWidget(std::make_unique<Wt::WPushButton>("Preview"));
    previewButton_->addStyleClass("btn btn-info");
    
    layout->addStretch(1);
    
    // Cancel button
    cancelButton_ = layout->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancelButton_->addStyleClass("btn btn-secondary");
    
    // Apply button
    applyButton_ = layout->addWidget(std::make_unique<Wt::WPushButton>("Apply"));
    applyButton_->addStyleClass("btn btn-success");
    
    container->setLayout(std::move(layout));
    
    // Initial state
    updateApplyButton();
    
    return container;
}

void ThemeSelectionDialog::setupEventHandlers() {
    // Theme selection change
    themeButtonGroup_->checkedChanged().connect([this]() {
        onThemeSelectionChanged();
    });
    
    // Preview button
    previewButton_->clicked().connect([this]() {
        previewTheme();
    });
    
    // Cancel button
    cancelButton_->clicked().connect([this]() {
        restoreOriginalTheme();
        reject();
    });
    
    // Apply button
    applyButton_->clicked().connect([this]() {
        applySelectedTheme();
    });
}

void ThemeSelectionDialog::onThemeSelectionChanged() {
    selectedThemeId_ = getSelectedThemeId();
    updateApplyButton();
}

void ThemeSelectionDialog::applySelectedTheme() {
    if (selectedThemeId_.empty() || !themeService_) {
        return;
    }
    
    // Apply the theme through the service
    if (themeService_->setCurrentTheme(selectedThemeId_)) {
        std::cout << "Applied theme: " << selectedThemeId_ << std::endl;
        
        // Publish theme change event
        if (eventManager_) {
            eventManager_->publish(POSEvents::THEME_CHANGED, selectedThemeId_);
        }
        
        // Call selection callback
        if (selectionCallback_) {
            selectionCallback_(selectedThemeId_);
        }
        
        accept();
    } else {
        std::cerr << "Failed to apply theme: " << selectedThemeId_ << std::endl;
    }
}

std::string ThemeSelectionDialog::getSelectedThemeId() const {
    if (!themeButtonGroup_) {
        return "";
    }
    
    auto* checkedButton = themeButtonGroup_->checkedButton();
    if (!checkedButton) {
        return "";
    }
    
    // Find the theme ID for the checked button
    for (size_t i = 0; i < themeButtons_.size() && i < availableThemes_.size(); ++i) {
        if (themeButtons_[i] == checkedButton) {
            return availableThemes_[i].id;
        }
    }
    
    return "";
}

void ThemeSelectionDialog::updateApplyButton() {
    if (!applyButton_) {
        return;
    }
    
    bool hasSelection = !selectedThemeId_.empty();
    bool isChanged = selectedThemeId_ != originalThemeId_;
    
    applyButton_->setEnabled(hasSelection && isChanged);
    
    if (isChanged) {
        applyButton_->setText("Apply Theme");
    } else {
        applyButton_->setText("No Changes");
    }
}

void ThemeSelectionDialog::previewTheme() {
    if (selectedThemeId_.empty() || !themeService_) {
        return;
    }
    
    // Temporarily apply the theme for preview
    themeService_->setCurrentTheme(selectedThemeId_);
    
    // Update button text
    previewButton_->setText("Previewing...");
    previewButton_->setEnabled(false);
    
    // Re-enable after a short delay (simulated preview)
    // In a real implementation, you might want to add a "Stop Preview" functionality
}

void ThemeSelectionDialog::restoreOriginalTheme() {
    if (!originalThemeId_.empty() && themeService_) {
        themeService_->setCurrentTheme(originalThemeId_);
    }
}

void ThemeSelectionDialog::showDialog() {
    refreshThemes();
    show();
}

void ThemeSelectionDialog::refreshThemes() {
    if (!themeService_) {
        return;
    }
    
    availableThemes_ = themeService_->getAvailableThemes();
    selectedThemeId_ = themeService_->getCurrentThemeId();
    
    // If dialog is already created, rebuild the themes grid
    if (themesContainer_) {
        // Clear existing content and rebuild
        themesContainer_->clear();
        themeButtons_.clear();
        themeButtonGroup_ = std::make_unique<Wt::WButtonGroup>();
        
        auto newGrid = createThemesGrid();
        // Note: In a real implementation, you'd need to properly replace the content
        // This is a simplified approach
    }
}

void ThemeSelectionDialog::setShowPreviews(bool enabled) {
    showPreviews_ = enabled;
    
    // If dialog is already created, refresh to show/hide previews
    if (themesContainer_) {
        refreshThemes();
    }
}
