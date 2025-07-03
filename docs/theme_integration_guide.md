# Restaurant POS Theme System Integration Guide

## Overview

This guide explains how to integrate the modular theme system into your Restaurant POS application. The theme system provides four distinct themes (Light, Dark, Colorful, Base) with smooth transitions and Bootstrap 4 integration.

## 🚀 Integration Steps

### 1. File Structure

Ensure you have these files in your project:

```
include/
├── core/
│   └── RestaurantPOSApp.hpp          # Updated with theme support
├── services/
│   └── ThemeService.hpp              # Theme management service
src/
├── core/
│   └── RestaurantPOSApp.cpp          # Updated implementation
├── services/
│   └── ThemeService.cpp              # Theme service implementation
static/
└── css/
    └── themes.css                    # Theme CSS framework
```

### 2. CSS Integration

Add the theme CSS file to your static resources. In your CMakeLists.txt or build system:

```cmake
# Copy CSS files to build directory
configure_file(${CMAKE_SOURCE_DIR}/static/css/themes.css 
               ${CMAKE_BINARY_DIR}/static/css/themes.css COPYONLY)
```

In your main application, load the CSS:

```cpp
// In RestaurantPOSApp constructor, after setupBootstrapTheme()
useStyleSheet("static/css/themes.css");
```

### 3. Compilation Requirements

Ensure your CMakeLists.txt includes the ThemeService:

```cmake
# Add ThemeService to your sources
set(SOURCES
    src/core/RestaurantPOSApp.cpp
    src/services/ThemeService.cpp
    src/services/POSService.cpp
    src/events/EventManager.cpp
    # ... other sources
)
```

### 4. Header Includes

Make sure your includes are correct in RestaurantPOSApp.cpp:

```cpp
#include "../../include/core/RestaurantPOSApp.hpp"
#include "../../include/services/ThemeService.hpp"
```

## 🎨 Theme Features

### Available Themes

1. **Light Theme** - Clean, professional appearance
2. **Dark Theme** - Easy on the eyes, perfect for low-light environments
3. **Colorful Theme** - Vibrant gradients and modern aesthetics
4. **Base Theme** - Minimal, monochromatic design

### Theme Controls

The integration adds these UI components:

- **Theme Selector Dropdown**: Choose from all available themes
- **Theme Toggle Button**: Quick switch between light/dark modes
- **Automatic Theme Persistence**: User preferences are saved

### CSS Variables System

The theme system uses CSS variables for maximum flexibility:

```css
/* Example usage in your custom CSS */
.my-custom-component {
    background-color: var(--pos-card-bg);
    color: var(--pos-card-text);
    border: 1px solid var(--pos-border-color);
    transition: all var(--pos-transition-normal);
}
```

## 🔧 Customization

### Adding Custom Themes

1. **Define CSS Variables** in `themes.css`:

```css
.theme-custom {
    --pos-primary: #your-color;
    --pos-card-bg: #your-bg;
    /* ... other variables */
}
```

2. **Update ThemeService** to include your theme:

```cpp
// In ThemeService.hpp
enum class Theme {
    LIGHT, DARK, COLORFUL, BASE, CUSTOM, AUTO
};

// In ThemeService.cpp initializeThemeMetadata()
themeNames_[Theme::CUSTOM] = "Custom";
themeCSSClasses_[Theme::CUSTOM] = "theme-custom";
```

### Extending Components

To make your custom components theme-aware:

```cpp
// Apply theme to any container
themeService_->applyThemeToContainer(myContainer);

// Listen for theme changes
themeService_->onThemeChanged([this](Theme oldTheme, Theme newTheme) {
    // Update your component's appearance
    updateMyComponentTheme();
});
```

## 📱 Features

### Responsive Design
- Mobile-optimized theme controls
- Tablet-friendly layouts
- Desktop-enhanced experience

### Accessibility
- High contrast mode support
- Reduced motion respect
- Keyboard navigation
- Screen reader compatibility

### Performance
- CSS variable-based theming (no class swapping)
- Smooth transitions with `will-change` optimizations
- Minimal JavaScript overhead

## 🐛 Troubleshooting

### Common Issues

1. **CSS Not Loading**
   ```cpp
   // Make sure you call this after setupBootstrapTheme()
   useStyleSheet("static/css/themes.css");
   ```

2. **Theme Not Persisting**
   ```cpp
   // Ensure ThemeService is initialized before UI setup
   initializeThemeService();
   ```

3. **Compilation Errors**
   ```bash
   # Make sure all includes are correct
   find . -name "*.hpp" | xargs grep -l "ThemeService"
   ```

### Debug Output

Enable theme debugging:

```cpp
// In your application constructor
std::cout << "[THEME DEBUG] Current theme: " 
          << themeService_->getThemeName(themeService_->getCurrentTheme()) 
          << std::endl;
```

## 🎯 Best Practices

### Code Organization
- Keep theme logic in ThemeService
- Use CSS variables for all themeable properties
- Apply themes at container level, not individual elements

### Performance
- Avoid inline styles that override theme variables
- Use transitions sparingly on frequently updated elements
- Batch theme changes when possible

### Maintenance
- Test all themes in different lighting conditions
- Verify accessibility standards (WCAG AA)
- Keep theme metadata synchronized

## 🚦 Testing

### Manual Testing Checklist

- [ ] All four themes load correctly
- [ ] Theme selector updates properly
- [ ] Toggle button works
- [ ] Theme persistence across sessions
- [ ] Responsive behavior on mobile
- [ ] Accessibility with screen readers
- [ ] Print styles work correctly

### Automated Testing

```cpp
// Example unit test
TEST(ThemeServiceTest, ThemeChanging) {
    auto app = std::make_unique<Wt::WApplication>(env);
    auto themeService = std::make_shared<ThemeService>(app.get());
    
    themeService->setTheme(ThemeService::Theme::DARK);
    EXPECT_EQ(themeService->getCurrentTheme(), ThemeService::Theme::DARK);
}
```

## 📚 Advanced Usage

### Custom Theme Animations

```cpp
// Add custom transition effects
void RestaurantPOSApp::applyCustomThemeTransition() {
    if (themeService_->getCurrentTheme() == ThemeService::Theme::COLORFUL) {
        doJavaScript("document.body.classList.add('colorful-transition');");
    }
}
```

### Theme-Based Logic

```cpp
// Conditional behavior based on theme
if (themeService_->isThemeDark(themeService_->getCurrentTheme())) {
    // Adjust chart colors for dark theme
    updateChartForDarkMode();
}
```

### Integration with Other Services

```cpp
// Coordinate with other services
themeService_->onThemeChanged([this](Theme oldTheme, Theme newTheme) {
    // Update chart service colors
    chartService_->updateTheme(newTheme);
    
    // Notify dashboard service
    dashboardService_->themeChanged(newTheme);
});
```

## 📞 Support

If you encounter issues:

1. Check the console output for theme-related messages
2. Verify CSS file is loaded correctly
3. Ensure all includes are present
4. Test with a minimal theme implementation first

The theme system is designed to be modular and extensible, allowing you to easily add new themes or modify existing ones without affecting the core application logic.