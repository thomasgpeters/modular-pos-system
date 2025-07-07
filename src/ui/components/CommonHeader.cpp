//============================================================================
// Fixed CommonHeader.cpp - Corrected Signal Connections
//============================================================================

#include "../../../include/ui/components/CommonHeader.hpp"

#include <Wt/WTimer.h>
#include <iomanip>
#include <sstream>
#include <ctime>

CommonHeader::CommonHeader(std::shared_ptr<ThemeService> themeService,
                          std::shared_ptr<EventManager> eventManager,
                          ModeChangeCallback modeChangeCallback)
    : themeService_(themeService)
    , eventManager_(eventManager)
    , modeChangeCallback_(modeChangeCallback)
    , brandingContainer_(nullptr)
    , modeSelector_(nullptr)
    , themeContainer_(nullptr)
    , themeComboBox_(nullptr)
    , themeToggleButton_(nullptr)
    , userContainer_(nullptr)
    , userInfo_(nullptr)
    , timeDisplay_(nullptr)
{
    initializeUI();
    setupEventHandlers();
}

void CommonHeader::initializeUI() {
    addStyleClass("common-header p-3");
    
    auto layout = setLayout(std::make_unique<Wt::WHBoxLayout>());
    layout->setContentsMargins(0, 0, 0, 0);
    
    createBrandingSection();
    createModeSection();
    createThemeSection();
    createUserSection();
}

void CommonHeader::createBrandingSection() {
    // Create branding container and add to layout
    auto brandingWidget = std::make_unique<Wt::WContainerWidget>();
    brandingContainer_ = brandingWidget.get();
    layout()->addWidget(std::move(brandingWidget));
    
    brandingContainer_->addStyleClass("d-flex align-items-center");
    
    brandingText_ = brandingContainer_->addWidget(
        std::make_unique<Wt::WText>("🍴 Restaurant POS")
    );
    brandingText_->addStyleClass("h4 mb-0 text-white fw-bold");
}

void CommonHeader::createModeSection() {
    auto modeWidget = std::make_unique<Wt::WContainerWidget>();
    auto modeContainer = modeWidget.get();
    layout()->addWidget(std::move(modeWidget));
    
    modeContainer->addStyleClass("mx-4");
    
    modeSelector_ = modeContainer->addWidget(
        std::make_unique<ModeSelector>(eventManager_, modeChangeCallback_)
    );
}

void CommonHeader::createThemeSection() {
    // Create theme container and add to layout
    auto themeWidget = std::make_unique<Wt::WContainerWidget>();
    themeContainer_ = themeWidget.get();
    layout()->addWidget(std::move(themeWidget));
    
    themeContainer_->addStyleClass("d-flex align-items-center gap-2");
    
    // Theme selector dropdown
    themeComboBox_ = themeContainer_->addWidget(std::make_unique<Wt::WComboBox>());
    themeComboBox_->addStyleClass("form-select form-select-sm");
    themeComboBox_->addItem("Base Theme");
    themeComboBox_->addItem("Light Theme");
    themeComboBox_->addItem("Dark Theme");
    themeComboBox_->addItem("Warm Theme");
    themeComboBox_->addItem("Cool Theme");
    
    // Theme toggle button
    themeToggleButton_ = themeContainer_->addWidget(
        std::make_unique<Wt::WPushButton>("🌙")
    );
    themeToggleButton_->addStyleClass("btn btn-outline-light btn-sm");
    themeToggleButton_->setToolTip("Toggle Dark/Light Theme");
}

void CommonHeader::createUserSection() {
    // Create user container and add to layout
    auto userWidget = std::make_unique<Wt::WContainerWidget>();
    userContainer_ = userWidget.get();
    layout()->addWidget(std::move(userWidget));
    
    userContainer_->addStyleClass("d-flex align-items-center gap-3 ms-auto");
    
    timeDisplay_ = userContainer_->addWidget(std::make_unique<Wt::WText>());
    timeDisplay_->addStyleClass("text-light");
    updateTimeDisplay();
    
    userInfo_ = userContainer_->addWidget(
        std::make_unique<Wt::WText>("👤 POS User")
    );
    userInfo_->addStyleClass("text-light");
}

void CommonHeader::setupEventHandlers() {
    // Fixed signal connections for different Wt versions
    if (themeComboBox_) {
        // Try multiple signal connection approaches
        try {
            // Method 1: Use activated() signal (takes int index)
            themeComboBox_->activated().connect([this](int index) {
                onThemeSelectionChanged(index);
            });
        } catch (...) {
            try {
                // Method 2: Use sactivated() signal (takes WString)
                themeComboBox_->sactivated().connect([this](const Wt::WString& text) {
                    onThemeSelectionChangedByText(text);
                });
            } catch (...) {
                // Method 3: Use changed() signal as fallback
                themeComboBox_->changed().connect([this]() {
                    onThemeSelectionChanged(themeComboBox_->currentIndex());
                });
            }
        }
    }
    
    if (themeToggleButton_) {
        themeToggleButton_->clicked().connect([this]() {
            onThemeToggleClicked();
        });
    }
    
    // Create and configure timer for time updates
    auto timerWidget = std::make_unique<Wt::WTimer>();
    auto timer = timerWidget.get();
    addChild(std::move(timerWidget));
    
    timer->timeout().connect([this]() { updateTimeDisplay(); });
    timer->setInterval(std::chrono::minutes(1));
    timer->start();
}

void CommonHeader::updateTimeDisplay() {
    if (!timeDisplay_) return;
    
    auto now = std::time(nullptr);
    auto localTime = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%H:%M");
    timeDisplay_->setText(oss.str());
}

void CommonHeader::setCurrentMode(ModeSelector::Mode mode) {
    if (modeSelector_) {
        modeSelector_->setCurrentMode(mode);
    }
}

ModeSelector::Mode CommonHeader::getCurrentMode() const {
    if (modeSelector_) {
        return modeSelector_->getCurrentMode();
    }
    return ModeSelector::POS_MODE;
}

// Handle theme selection by index (for activated() signal)
void CommonHeader::onThemeSelectionChanged(int index) {
    if (!themeService_) return;
    
    ThemeService::Theme theme;
    
    switch (index) {
        case 0: theme = ThemeService::Theme::BASE; break;
        case 1: theme = ThemeService::Theme::LIGHT; break;
        case 2: theme = ThemeService::Theme::DARK; break;
        case 3: theme = ThemeService::Theme::WARM; break;
        case 4: theme = ThemeService::Theme::COOL; break;
        default: theme = ThemeService::Theme::BASE; break;
    }
    
    themeService_->setTheme(theme);
}

// Handle theme selection by text (for sactivated() signal)
void CommonHeader::onThemeSelectionChangedByText(const Wt::WString& text) {
    if (!themeService_) return;
    
    std::string themeText = text.toUTF8();
    ThemeService::Theme theme = ThemeService::Theme::BASE;
    
    if (themeText == "Base Theme") theme = ThemeService::Theme::BASE;
    else if (themeText == "Light Theme") theme = ThemeService::Theme::LIGHT;
    else if (themeText == "Dark Theme") theme = ThemeService::Theme::DARK;
    else if (themeText == "Warm Theme") theme = ThemeService::Theme::WARM;
    else if (themeText == "Cool Theme") theme = ThemeService::Theme::COOL;
    
    themeService_->setTheme(theme);
}

void CommonHeader::onThemeToggleClicked() {
    if (!themeService_) return;
    
    themeService_->toggleTheme();
}

void CommonHeader::updateThemeControls() {
    // Update theme controls based on current theme
    // This would be called when theme changes externally
}

//============================================================================
// Alternative Simplified Version without ThemeService
//============================================================================

/*
// Use this if you don't have ThemeService yet:

void CommonHeader::setupEventHandlers() {
    if (themeComboBox_) {
        // Simple version without actual theme switching
        themeComboBox_->activated().connect([this](int index) {
            std::cout << "Theme selected: " << index << std::endl;
        });
    }
    
    if (themeToggleButton_) {
        themeToggleButton_->clicked().connect([this]() {
            std::cout << "Theme toggle clicked" << std::endl;
        });
    }
    
    // Timer setup (same as above)
    auto timerWidget = std::make_unique<Wt::WTimer>();
    auto timer = timerWidget.get();
    addChild(std::move(timerWidget));
    
    timer->timeout().connect([this]() { updateTimeDisplay(); });
    timer->setInterval(std::chrono::minutes(1));
    timer->start();
}
*/


