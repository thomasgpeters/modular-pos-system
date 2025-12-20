// ============================================================================
// Fixed CommonHeader Implementation - Enhanced Theme Integration
// File: src/ui/components/CommonHeader.cpp
// ============================================================================

#include "../../../include/ui/components/CommonHeader.hpp"
#include "../../../include/utils/UIStyleHelper.hpp"

#include <Wt/WApplication.h>
#include <Wt/WTimer.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

CommonHeader::CommonHeader(std::shared_ptr<ThemeService> themeService,
                          std::shared_ptr<EventManager> eventManager,
                          ModeChangeCallback modeChangeCallback)
    : Wt::WContainerWidget()
    , themeService_(themeService)
    , eventManager_(eventManager)
    , modeChangeCallback_(modeChangeCallback)
    , brandingContainer_(nullptr)
    , brandingText_(nullptr)
    , modeSelector_(nullptr)
    , themeContainer_(nullptr)
    , themeComboBox_(nullptr)
    , themeToggleButton_(nullptr)
    , userContainer_(nullptr)
    , userInfo_(nullptr)
    , timeDisplay_(nullptr)
{
    std::cout << "[CommonHeader] Initializing..." << std::endl;
    
    // Apply header styling
    UIStyleHelper::styleHeaderComponent(this);
    
    // Initialize the UI
    initializeUI();
    
    // Set up event handlers
    setupEventHandlers();
    
    // Start time updates
    updateTimeDisplay();
    
    std::cout << "[CommonHeader] Initialization complete" << std::endl;
}

void CommonHeader::initializeUI() {
    std::cout << "[CommonHeader] Setting up UI layout..." << std::endl;

    // Single row header - horizontal flex layout
    setAttributeValue("style",
        "display: flex; justify-content: space-between; align-items: center; "
        "width: 100%; padding: 10px 15px; margin: 0; background: #2c3e50;");
    addStyleClass("pos-header");

    // Create sections directly in this container
    createBrandingSection(this);
    createModeSection(this);
    createThemeSection(this);
    createUserSection(this);

    std::cout << "[CommonHeader] UI layout complete" << std::endl;
}

void CommonHeader::createBrandingSection(Wt::WContainerWidget* parent) {
    brandingContainer_ = parent->addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(brandingContainer_, "start", "center");
    brandingContainer_->addStyleClass("branding-section");

    brandingText_ = brandingContainer_->addNew<Wt::WText>("🍽️ Restaurant POS");
    brandingText_->setAttributeValue("style", "color: white; font-size: 1.1rem; font-weight: bold;");

    std::cout << "[CommonHeader] Branding section created" << std::endl;
}

void CommonHeader::createModeSection(Wt::WContainerWidget* parent) {
    auto modeContainer = parent->addNew<Wt::WContainerWidget>();
    modeContainer->addStyleClass("mode-section mx-3");

    // Create mode selector with callback
    modeSelector_ = modeContainer->addNew<ModeSelector>(
        eventManager_,
        [this](ModeSelector::Mode mode) {
            std::cout << "[CommonHeader] Mode changed via selector: "
                      << (mode == ModeSelector::POS_MODE ? "POS" : "Kitchen") << std::endl;

            if (modeChangeCallback_) {
                modeChangeCallback_(mode);
            }
        }
    );

    std::cout << "[CommonHeader] Mode section created" << std::endl;
}

void CommonHeader::createThemeSection(Wt::WContainerWidget* parent) {
    std::cout << "[CommonHeader] Creating theme section..." << std::endl;

    themeContainer_ = parent->addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(themeContainer_, "center", "center");
    themeContainer_->addStyleClass("theme-section mx-3");
    
    if (themeService_) {
        // FIXED: Create theme combo box with proper event handling
        themeComboBox_ = themeContainer_->addNew<Wt::WComboBox>();
        themeComboBox_->addStyleClass("form-select form-select-sm theme-selector");
        themeComboBox_->setWidth(Wt::WLength(150));
        
        // Populate theme options
        auto themes = themeService_->getAvailableThemes();
        for (const auto& theme : themes) {
            std::string displayText = themeService_->getThemeIcon(theme) + " " + themeService_->getThemeName(theme);
            themeComboBox_->addItem(displayText);
        }
        
        // Set current selection
        auto currentTheme = themeService_->getCurrentTheme();
        for (size_t i = 0; i < themes.size(); ++i) {
            if (themes[i] == currentTheme) {
                themeComboBox_->setCurrentIndex(static_cast<int>(i));
                break;
            }
        }
        
        // FIXED: Enhanced event connections for reliable theme changing
        themeComboBox_->changed().connect([this, themes]() {
            std::cout << "[CommonHeader] Theme combo box changed!" << std::endl;
            onThemeSelectionChanged(themeComboBox_->currentIndex());
        });
        
        themeComboBox_->sactivated().connect([this](const Wt::WString& text) {
            std::cout << "[CommonHeader] Theme combo box activated: " << text.toUTF8() << std::endl;
            onThemeSelectionChangedByText(text);
        });
        
        // FIXED: Add toggle button as backup
        themeToggleButton_ = themeContainer_->addNew<Wt::WPushButton>("🔄");
        UIStyleHelper::styleButton(themeToggleButton_, "outline-light", "sm");
        themeToggleButton_->setToolTip("Toggle Theme");
        
        themeToggleButton_->clicked().connect([this]() {
            std::cout << "[CommonHeader] Theme toggle button clicked!" << std::endl;
            onThemeToggleClicked();
        });
        
        // Listen for theme changes from the service
        themeService_->onThemeChanged([this](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
            std::cout << "[CommonHeader] Theme service changed, updating controls..." << std::endl;
            updateThemeControls();
        });
        
        std::cout << "[CommonHeader] Theme controls created with " << themes.size() << " options" << std::endl;
        
    } else {
        std::cerr << "[CommonHeader] Warning: ThemeService not available" << std::endl;
        
        // Create placeholder
        auto placeholderText = themeContainer_->addNew<Wt::WText>("Theme: Default");
        placeholderText->addStyleClass("text-light small");
    }
    
    std::cout << "[CommonHeader] Theme section complete" << std::endl;
}

void CommonHeader::createUserSection(Wt::WContainerWidget* parent) {
    userContainer_ = parent->addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(userContainer_, "end", "center");
    userContainer_->addStyleClass("user-section");

    // User info
    userInfo_ = userContainer_->addNew<Wt::WText>("👤 Operator");
    userInfo_->setAttributeValue("style", "color: rgba(255,255,255,0.8); margin-right: 12px; font-size: 0.85rem;");

    // Time display
    timeDisplay_ = userContainer_->addNew<Wt::WText>();
    timeDisplay_->setAttributeValue("style", "color: rgba(255,255,255,0.8); font-size: 0.85rem; font-family: monospace;");

    // Set up timer for time updates
    auto timer = addChild(std::make_unique<Wt::WTimer>());
    timer->setInterval(std::chrono::seconds(1));
    timer->timeout().connect(this, &CommonHeader::updateTimeDisplay);
    timer->start();

    std::cout << "[CommonHeader] User section created" << std::endl;
}

void CommonHeader::setupEventHandlers() {
    std::cout << "[CommonHeader] Setting up event handlers..." << std::endl;
    
    // Any additional event setup can go here
    
    std::cout << "[CommonHeader] Event handlers setup complete" << std::endl;
}

void CommonHeader::setCurrentMode(ModeSelector::Mode mode) {
    if (modeSelector_) {
        modeSelector_->setCurrentMode(mode);
        std::cout << "[CommonHeader] Mode set to: " << (mode == ModeSelector::POS_MODE ? "POS" : "Kitchen") << std::endl;
    }
}

ModeSelector::Mode CommonHeader::getCurrentMode() const {
    if (modeSelector_) {
        return modeSelector_->getCurrentMode();
    }
    return ModeSelector::POS_MODE; // Default
}

void CommonHeader::updateTimeDisplay() {
    if (timeDisplay_) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto localtime = *std::localtime(&time_t);
        
        std::stringstream ss;
        ss << std::put_time(&localtime, "%H:%M:%S");
        
        timeDisplay_->setText(ss.str());
    }
}

// FIXED: Enhanced theme selection handling
void CommonHeader::onThemeSelectionChanged(int index) {
    std::cout << "[CommonHeader] Theme selection changed to index: " << index << std::endl;
    
    if (!themeService_) {
        std::cerr << "[CommonHeader] ThemeService not available!" << std::endl;
        return;
    }
    
    try {
        auto themes = themeService_->getAvailableThemes();
        
        if (index >= 0 && index < static_cast<int>(themes.size())) {
            ThemeService::Theme selectedTheme = themes[index];
            std::cout << "[CommonHeader] Applying theme: " << themeService_->getThemeName(selectedTheme) << std::endl;
            
            // FIXED: Apply theme with immediate effect
            themeService_->setTheme(selectedTheme, true);
            
            std::cout << "[CommonHeader] Theme applied successfully" << std::endl;
        } else {
            std::cerr << "[CommonHeader] Invalid theme index: " << index << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CommonHeader] Error changing theme: " << e.what() << std::endl;
    }
}

void CommonHeader::onThemeSelectionChangedByText(const Wt::WString& text) {
    std::cout << "[CommonHeader] Theme selected by text: " << text.toUTF8() << std::endl;
    
    // This will trigger the index-based handler, so we just need to ensure 
    // the index is properly set
    if (themeComboBox_) {
        int currentIndex = themeComboBox_->currentIndex();
        onThemeSelectionChanged(currentIndex);
    }
}

void CommonHeader::onThemeToggleClicked() {
    std::cout << "[CommonHeader] Theme toggle clicked!" << std::endl;
    
    if (!themeService_) {
        std::cerr << "[CommonHeader] ThemeService not available for toggle!" << std::endl;
        return;
    }
    
    try {
        // FIXED: Simple toggle between light and dark themes
        auto currentTheme = themeService_->getCurrentTheme();
        ThemeService::Theme newTheme;
        
        switch (currentTheme) {
            case ThemeService::Theme::LIGHT:
                newTheme = ThemeService::Theme::DARK;
                break;
            case ThemeService::Theme::DARK:
                newTheme = ThemeService::Theme::WARM;
                break;
            case ThemeService::Theme::WARM:
                newTheme = ThemeService::Theme::COOL;
                break;
            case ThemeService::Theme::COOL:
            case ThemeService::Theme::BASE:
            case ThemeService::Theme::AUTO:
            default:
                newTheme = ThemeService::Theme::LIGHT;
                break;
        }
        
        std::cout << "[CommonHeader] Toggling from " << themeService_->getThemeName(currentTheme) 
                  << " to " << themeService_->getThemeName(newTheme) << std::endl;
        
        themeService_->setTheme(newTheme, true);
        
        std::cout << "[CommonHeader] Theme toggle completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[CommonHeader] Error toggling theme: " << e.what() << std::endl;
    }
}

void CommonHeader::updateThemeControls() {
    std::cout << "[CommonHeader] Updating theme controls..." << std::endl;
    
    if (!themeService_ || !themeComboBox_) {
        return;
    }
    
    try {
        // Update combo box selection to match current theme
        auto currentTheme = themeService_->getCurrentTheme();
        auto themes = themeService_->getAvailableThemes();
        
        for (size_t i = 0; i < themes.size(); ++i) {
            if (themes[i] == currentTheme) {
                // Temporarily disconnect to avoid triggering events
                themeComboBox_->setCurrentIndex(static_cast<int>(i));
                std::cout << "[CommonHeader] Updated combo box to index " << i 
                          << " (" << themeService_->getThemeName(currentTheme) << ")" << std::endl;
                break;
            }
        }
        
        std::cout << "[CommonHeader] Theme controls updated" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[CommonHeader] Error updating theme controls: " << e.what() << std::endl;
    }
}
