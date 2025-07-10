// ============================================================================
// Theme Debug Helper - For Testing and Debugging Theme Changes
// File: include/utils/ThemeDebugHelper.hpp
// ============================================================================

#ifndef THEMEDEBUGHELPER_H
#define THEMEDEBUGHELPER_H

#include "../services/ThemeService.hpp"
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <memory>
#include <iostream>

/**
 * @class ThemeDebugHelper
 * @brief Utility class for testing and debugging theme changes
 */
class ThemeDebugHelper {
public:
    /**
     * @brief Creates a debug panel for testing themes
     * @param themeService Theme service to test
     * @param parent Parent container to add debug panel to
     * @return Pointer to the created debug panel
     */
    static Wt::WContainerWidget* createDebugPanel(
        std::shared_ptr<ThemeService> themeService,
        Wt::WContainerWidget* parent) {
        
        if (!themeService || !parent) {
            std::cerr << "[ThemeDebugHelper] Invalid parameters!" << std::endl;
            return nullptr;
        }
        
        std::cout << "[ThemeDebugHelper] Creating debug panel..." << std::endl;
        
        // Create debug container
        auto debugContainer = parent->addNew<Wt::WContainerWidget>();
        debugContainer->addStyleClass("theme-debug-panel p-3 m-3 border rounded");
        debugContainer->addStyleClass("bg-light");
        
        // Title
        auto title = debugContainer->addNew<Wt::WText>("<h5>🔧 Theme Debug Panel</h5>");
        title->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        // Current theme display
        auto currentThemeText = debugContainer->addNew<Wt::WText>();
        currentThemeText->addStyleClass("mb-3 d-block");
        
        auto updateCurrentTheme = [currentThemeText, themeService]() {
            auto current = themeService->getCurrentTheme();
            std::string text = "Current Theme: <strong>" + themeService->getThemeName(current) + 
                             "</strong> (Class: " + themeService->getThemeCSSClass(current) + ")";
            currentThemeText->setText(text);
            currentThemeText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        };
        
        updateCurrentTheme();
        
        // Theme test buttons
        auto buttonContainer = debugContainer->addNew<Wt::WContainerWidget>();
        buttonContainer->addStyleClass("d-flex flex-wrap gap-2 mb-3");
        
        auto themes = themeService->getAvailableThemes();
        for (const auto& theme : themes) {
            auto button = buttonContainer->addNew<Wt::WPushButton>(
                themeService->getThemeIcon(theme) + " " + themeService->getThemeName(theme)
            );
            button->addStyleClass("btn btn-sm btn-outline-primary");
            
            button->clicked().connect([themeService, theme, updateCurrentTheme]() {
                std::cout << "[ThemeDebugHelper] Testing theme: " << themeService->getThemeName(theme) << std::endl;
                
                try {
                    themeService->setTheme(theme, true);
                    updateCurrentTheme();
                    std::cout << "[ThemeDebugHelper] Theme applied successfully!" << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "[ThemeDebugHelper] Error applying theme: " << e.what() << std::endl;
                }
            });
        }
        
        // Manual CSS injection test
        auto cssTestContainer = debugContainer->addNew<Wt::WContainerWidget>();
        cssTestContainer->addStyleClass("mt-3 p-3 border rounded bg-white");
        
        auto cssTitle = cssTestContainer->addNew<Wt::WText>("<strong>CSS Injection Test:</strong>");
        cssTitle->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        cssTitle->addStyleClass("d-block mb-2");
        
        auto testRedButton = cssTestContainer->addNew<Wt::WPushButton>("Test Red Background");
        testRedButton->addStyleClass("btn btn-sm btn-warning me-2");
        
        auto testBlueButton = cssTestContainer->addNew<Wt::WPushButton>("Test Blue Background");
        testBlueButton->addStyleClass("btn btn-sm btn-info me-2");
        
        auto resetButton = cssTestContainer->addNew<Wt::WPushButton>("Reset");
        resetButton->addStyleClass("btn btn-sm btn-secondary");
        
        testRedButton->clicked().connect([debugContainer]() {
            std::cout << "[ThemeDebugHelper] Testing red background..." << std::endl;
            debugContainer->addStyleClass("bg-danger text-white");
            debugContainer->removeStyleClass("bg-light bg-primary");
        });
        
        testBlueButton->clicked().connect([debugContainer]() {
            std::cout << "[ThemeDebugHelper] Testing blue background..." << std::endl;
            debugContainer->addStyleClass("bg-primary text-white");
            debugContainer->removeStyleClass("bg-light bg-danger");
        });
        
        resetButton->clicked().connect([debugContainer]() {
            std::cout << "[ThemeDebugHelper] Resetting background..." << std::endl;
            debugContainer->removeStyleClass("bg-danger bg-primary text-white");
            debugContainer->addStyleClass("bg-light");
        });
        
        // Listen for theme changes
        themeService->onThemeChanged([updateCurrentTheme](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
            std::cout << "[ThemeDebugHelper] Theme changed detected!" << std::endl;
            updateCurrentTheme();
        });
        
        std::cout << "[ThemeDebugHelper] Debug panel created successfully" << std::endl;
        return debugContainer;
    }
    
    /**
     * @brief Logs current theme information
     * @param themeService Theme service to inspect
     */
    static void logThemeInfo(std::shared_ptr<ThemeService> themeService) {
        if (!themeService) {
            std::cerr << "[ThemeDebugHelper] ThemeService is null!" << std::endl;
            return;
        }
        
        auto current = themeService->getCurrentTheme();
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "THEME DEBUG INFO" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "Current Theme: " << themeService->getThemeName(current) << std::endl;
        std::cout << "CSS Class: " << themeService->getThemeCSSClass(current) << std::endl;
        std::cout << "Primary Color: " << themeService->getThemePrimaryColor(current) << std::endl;
        std::cout << "Description: " << themeService->getThemeDescription(current) << std::endl;
        std::cout << "CSS Path: " << themeService->getThemeCSSPath(current) << std::endl;
        std::cout << "Category: " << themeService->getThemeCategory(current) << std::endl;
        std::cout << "Is Dark: " << (themeService->isThemeDark(current) ? "Yes" : "No") << std::endl;
        std::cout << "Contrast Ratio: " << themeService->getThemeContrastRatio(current) << std::endl;
        std::cout << "Accessible: " << (themeService->isThemeAccessible(current) ? "Yes" : "No") << std::endl;
        
        std::cout << "\nAvailable Themes:" << std::endl;
        auto themes = themeService->getAvailableThemes();
        for (const auto& theme : themes) {
            std::cout << "  - " << themeService->getThemeIcon(theme) << " " 
                      << themeService->getThemeName(theme) << std::endl;
        }
        
        std::cout << std::string(50, '=') << std::endl << std::endl;
    }
    
    /**
     * @brief Tests theme cycling
     * @param themeService Theme service to test
     */
    static void testThemeCycling(std::shared_ptr<ThemeService> themeService) {
        if (!themeService) {
            std::cerr << "[ThemeDebugHelper] ThemeService is null!" << std::endl;
            return;
        }
        
        std::cout << "[ThemeDebugHelper] Testing theme cycling..." << std::endl;
        
        auto themes = themeService->getAvailableThemes();
        
        for (const auto& theme : themes) {
            std::cout << "[ThemeDebugHelper] Applying theme: " << themeService->getThemeName(theme) << std::endl;
            
            try {
                themeService->setTheme(theme);
                
                // Small delay for visual effect (in real app, this would be handled by UI)
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                
                std::cout << "[ThemeDebugHelper] ✓ Theme applied successfully" << std::endl;
                
            } catch (const std::exception& e) {
                std::cerr << "[ThemeDebugHelper] ✗ Error applying theme: " << e.what() << std::endl;
            }
        }
        
        std::cout << "[ThemeDebugHelper] Theme cycling test complete" << std::endl;
    }
    
    /**
     * @brief Creates a simple theme test widget
     * @param themeService Theme service to use
     * @param parent Parent container
     * @return Test widget container
     */
    static Wt::WContainerWidget* createThemeTestWidget(
        std::shared_ptr<ThemeService> themeService,
        Wt::WContainerWidget* parent) {
        
        if (!parent) return nullptr;
        
        auto testWidget = parent->addNew<Wt::WContainerWidget>();
        testWidget->addStyleClass("theme-test-widget p-4 m-3 border rounded");
        
        // Sample content that should be affected by themes
        auto title = testWidget->addNew<Wt::WText>("<h4>Theme Test Widget</h4>");
        title->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        auto description = testWidget->addNew<Wt::WText>(
            "This widget should change appearance when themes are applied. "
            "Watch for background color, text color, and border changes."
        );
        description->addStyleClass("mb-3");
        
        auto primaryButton = testWidget->addNew<Wt::WPushButton>("Primary Button");
        primaryButton->addStyleClass("btn btn-primary me-2");
        
        auto secondaryButton = testWidget->addNew<Wt::WPushButton>("Secondary Button");
        secondaryButton->addStyleClass("btn btn-secondary me-2");
        
        auto successButton = testWidget->addNew<Wt::WPushButton>("Success Button");
        successButton->addStyleClass("btn btn-success");
        
        // Apply theme to this widget whenever theme changes
        if (themeService) {
            themeService->onThemeChanged([testWidget, themeService](ThemeService::Theme oldTheme, ThemeService::Theme newTheme) {
                std::cout << "[ThemeDebugHelper] Applying theme to test widget: " << themeService->getThemeName(newTheme) << std::endl;
                themeService->applyThemeToContainer(testWidget, newTheme);
            });
            
            // Apply current theme
            themeService->applyThemeToContainer(testWidget, themeService->getCurrentTheme());
        }
        
        return testWidget;
    }
};

// Macro for easy debugging
#define THEME_DEBUG_LOG(themeService) \
    ThemeDebugHelper::logThemeInfo(themeService)

#define THEME_DEBUG_PANEL(themeService, parent) \
    ThemeDebugHelper::createDebugPanel(themeService, parent)

#define THEME_TEST_WIDGET(themeService, parent) \
    ThemeDebugHelper::createThemeTestWidget(themeService, parent)

#endif // THEMEDEBUGHELPER_H
