#include "../../../include/ui/dialogs/ThemeSelectionDialog.hpp"

// Default constructor
ThemeSelectionDialog::ThemeSelectionDialog() 
    : eventManager_(nullptr), previewMode_(false) {
    initializeDefaults();
}

// Constructor with event manager only
ThemeSelectionDialog::ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager), previewMode_(false) {
    initializeDefaults();
}

// Constructor with event manager and ThemeChangeCallback
ThemeSelectionDialog::ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                                         ThemeChangeCallback callback)
    : eventManager_(eventManager), themeChangeCallback_(callback), previewMode_(false) {
    initializeDefaults();
}

// Constructor with event manager and string callback
ThemeSelectionDialog::ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                                         StringCallback callback)
    : eventManager_(eventManager), previewMode_(false) {
    initializeDefaults();
    if (callback) {
        themeChangeCallback_ = convertStringCallback(callback);
    }
}

// Constructor with configuration options
ThemeSelectionDialog::ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                                         ThemeChangeCallback callback,
                                         bool showPreviews)
    : eventManager_(eventManager), themeChangeCallback_(callback), previewMode_(false) {
    initializeDefaults();
    showPreviews_ = showPreviews;
}

// Constructor with full configuration
ThemeSelectionDialog::ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                                         ThemeChangeCallback callback,
                                         bool showPreviews,
                                         bool showDescriptions)
    : eventManager_(eventManager), themeChangeCallback_(callback), previewMode_(false) {
    initializeDefaults();
    showPreviews_ = showPreviews;
    showDescriptions_ = showDescriptions;
}

// Constructor with extended configuration
ThemeSelectionDialog::ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                                         ThemeChangeCallback callback,
                                         bool showPreviews,
                                         bool showDescriptions,
                                         int maxThemes)
    : eventManager_(eventManager), themeChangeCallback_(callback), previewMode_(false) {
    initializeDefaults();
    showPreviews_ = showPreviews;
    showDescriptions_ = showDescriptions;
    maxThemes_ = maxThemes;
}

// Constructor with configuration map
ThemeSelectionDialog::ThemeSelectionDialog(std::shared_ptr<EventManager> eventManager,
                                         ThemeChangeCallback callback,
                                         const std::map<std::string, std::any>& config)
    : eventManager_(eventManager), themeChangeCallback_(callback), previewMode_(false) {
    initializeDefaults();
    setConfiguration(config);
}

void ThemeSelectionDialog::initializeDefaults() {
    showPreviews_ = true;
    livePreviewEnabled_ = true;
    showDescriptions_ = true;
    maxThemes_ = 10;
    previewWidth_ = 200;
    previewHeight_ = 150;
    
    currentThemeId_ = "default";
    selectedThemeId_ = "default";
    originalThemeId_ = "default";
    
    // Initialize UI pointers to nullptr
    tabContent_ = nullptr;
    themeTab_ = nullptr;
    generalTab_ = nullptr;
    displayTab_ = nullptr;
    currentThemeText_ = nullptr;
    autoSaveCheckbox_ = nullptr;
    notificationsCheckbox_ = nullptr;
    soundCheckbox_ = nullptr;
    languageCombo_ = nullptr;
    fontSizeSpinBox_ = nullptr;
    showDescriptionsCheckbox_ = nullptr;
    compactModeCheckbox_ = nullptr;
    animationsCheckbox_ = nullptr;
    rowsPerPageSpinBox_ = nullptr;
    stripedRowsCheckbox_ = nullptr;
    previewButton_ = nullptr;
    resetButton_ = nullptr;
    cancelButton_ = nullptr;
    applyButton_ = nullptr;
    
    // Load default themes
    loadDefaultThemes();
    
    setWindowTitle("Theme Selection");
    resize(Wt::WLength(600), Wt::WLength(500));
}

ThemeSelectionDialog::ThemeChangeCallback 
ThemeSelectionDialog::convertStringCallback(StringCallback stringCallback) {
    return [stringCallback](const ThemeInfo& theme) {
        if (stringCallback) {
            stringCallback(theme.id);
        }
    };
}

void ThemeSelectionDialog::setCurrentTheme(const std::string& themeId) {
    currentThemeId_ = themeId;
    selectedThemeId_ = themeId;
}

ThemeSelectionDialog::ThemeInfo 
ThemeSelectionDialog::getThemeInfo(const std::string& themeId) const {
    for (const auto& theme : availableThemes_) {
        if (theme.id == themeId) {
            return theme;
        }
    }
    // Return default theme info if not found
    return ThemeInfo("default", "Default Theme", "Basic default theme", "", true);
}

void ThemeSelectionDialog::setConfiguration(const std::map<std::string, std::any>& config) {
    try {
        if (config.find("showPreviews") != config.end()) {
            showPreviews_ = std::any_cast<bool>(config.at("showPreviews"));
        }
        if (config.find("showDescriptions") != config.end()) {
            showDescriptions_ = std::any_cast<bool>(config.at("showDescriptions"));
        }
        if (config.find("maxThemes") != config.end()) {
            maxThemes_ = std::any_cast<int>(config.at("maxThemes"));
        }
        if (config.find("previewWidth") != config.end()) {
            previewWidth_ = std::any_cast<int>(config.at("previewWidth"));
        }
        if (config.find("previewHeight") != config.end()) {
            previewHeight_ = std::any_cast<int>(config.at("previewHeight"));
        }
        if (config.find("livePreviewEnabled") != config.end()) {
            livePreviewEnabled_ = std::any_cast<bool>(config.at("livePreviewEnabled"));
        }
    } catch (const std::bad_any_cast& e) {
        // Ignore configuration errors for now
    }
}

// Basic stub implementations for required methods
void ThemeSelectionDialog::createDialogContent() {
    // Basic implementation - can be enhanced later
}

void ThemeSelectionDialog::createThemePanel() {
    // Basic implementation - can be enhanced later
}

void ThemeSelectionDialog::createGeneralPanel() {
    // Basic implementation - can be enhanced later
}

void ThemeSelectionDialog::createDisplayPanel() {
    // Basic implementation - can be enhanced later
}

std::unique_ptr<Wt::WContainerWidget> 
ThemeSelectionDialog::createThemeCard(const ThemeInfo& theme) {
    auto card = std::make_unique<Wt::WContainerWidget>();
    card->addNew<Wt::WText>(theme.name);
    return card;
}

std::unique_ptr<Wt::WContainerWidget> 
ThemeSelectionDialog::createActionButtons() {
    auto container = std::make_unique<Wt::WContainerWidget>();
    // Add basic buttons
    return container;
}

void ThemeSelectionDialog::setupEventHandlers() {
    // Basic implementation - can be enhanced later
}

void ThemeSelectionDialog::showThemePanel() {
    // Basic implementation - can be enhanced later
}

void ThemeSelectionDialog::showGeneralPanel() {
    // Basic implementation - can be enhanced later
}

void ThemeSelectionDialog::showDisplayPanel() {
    // Basic implementation - can be enhanced later
}

void ThemeSelectionDialog::onThemeSelectionChanged() {
    // Basic implementation - can be enhanced later
}

void ThemeSelectionDialog::onThemeSelected(const std::string& themeId) {
    selectedThemeId_ = themeId;
    if (themeChangeCallback_) {
        ThemeInfo theme = getThemeInfo(themeId);
        themeChangeCallback_(theme);
    }
}

void ThemeSelectionDialog::togglePreviewMode() {
    previewMode_ = !previewMode_;
}

void ThemeSelectionDialog::resetToDefaults() {
    initializeDefaults();
}

void ThemeSelectionDialog::applyChanges() {
    if (selectedThemeId_ != currentThemeId_) {
        onThemeSelected(selectedThemeId_);
    }
    accept();
}

void ThemeSelectionDialog::applyTheme(const std::string& themeId) {
    onThemeSelected(themeId);
}

void ThemeSelectionDialog::loadThemeConfiguration() {
    // Load from configuration file - basic implementation
    loadDefaultThemes();
}

void ThemeSelectionDialog::loadDefaultThemes() {
    availableThemes_.clear();
    
    // Add some default themes
    availableThemes_.emplace_back("default", "Default Theme", 
                                 "Clean and simple default theme", 
                                 "themes/default.css", true,
                                 std::vector<std::string>{"#ffffff", "#000000", "#0066cc"});
    
    availableThemes_.emplace_back("dark", "Dark Theme", 
                                 "Dark theme for low-light environments", 
                                 "themes/dark.css", false,
                                 std::vector<std::string>{"#2b2b2b", "#ffffff", "#ff6b35"});
    
    availableThemes_.emplace_back("modern", "Modern Theme", 
                                 "Clean modern interface theme", 
                                 "themes/modern.css", false,
                                 std::vector<std::string>{"#f8f9fa", "#212529", "#007bff"});
    
    availableThemes_.emplace_back("restaurant", "Restaurant Theme", 
                                 "Warm colors perfect for restaurant atmosphere", 
                                 "themes/restaurant.css", false,
                                 std::vector<std::string>{"#fdf6e3", "#8b4513", "#d2691e"});
}

void ThemeSelectionDialog::loadCurrentTheme() {
    // Load current theme from preferences - basic implementation
    currentThemeId_ = "default";
    selectedThemeId_ = "default";
}

void ThemeSelectionDialog::savePreferences() {
    // Save preferences to storage - basic implementation
}
