# Restaurant POS - Consistent Styling Integration Guide

## Overview

This guide shows how to integrate the consistent styling framework across all POS components. The system uses `UIStyleHelper.hpp` combined with `pos-consistent-styles.css` to ensure uniform appearance and behavior.

## Quick Start

### 1. Include Required Files

```cpp
// In your component .cpp files
#include "UIStyleHelper.hpp" // Always include this for styling

// In your main application
#include "../../include/services/ThemeService.hpp"
```

### 2. Add CSS to Your Application

```cpp
// In RestaurantPOSApp constructor or main setup
app->useStyleSheet("css/pos-consistent-styles.css");
app->useStyleSheet("css/bootstrap.min.css"); // Ensure Bootstrap is loaded first
```

### 3. Apply Base Container Styling

```cpp
// For any main component container
UIStyleHelper::styleContainer(this, "panel"); // or "header", "content", "sidebar"

// For group boxes
UIStyleHelper::styleGroupBox(groupBox, "primary"); // or "success", "warning"

// For tables  
UIStyleHelper::styleTable(table, "orders"); // or "menu", "compact"
```

## Component-Specific Integration Examples

### ActiveOrdersDisplay - Fixed Implementation

```cpp
void ActiveOrdersDisplay::initializeUI() {
    // Apply consistent container styling
    UIStyleHelper::styleOrderDisplay(this);
    
    // Create main group with consistent styling
    auto ordersGroup = addNew<Wt::WGroupBox>("📋 Active Orders");
    UIStyleHelper::styleGroupBox(ordersGroup, "primary");
    
    // Create table with consistent styling
    ordersTable_ = container->addNew<Wt::WTable>();
    UIStyleHelper::styleTable(ordersTable_, "orders");
    
    // Add animations
    UIStyleHelper::addFadeIn(this);
}

void ActiveOrdersDisplay::addOrderRow(std::shared_ptr<Order> order, int row) {
    // Status badge with consistent styling
    auto statusText = std::make_unique<Wt::WText>(formatOrderStatus(order->getStatus()));
    UIStyleHelper::styleBadge(statusText.get(), getStatusBadgeVariant(order->getStatus()));
    
    // Action buttons with consistent styling
    auto viewButton = container->addNew<Wt::WPushButton>("👁️ View");
    UIStyleHelper::styleButton(viewButton, "outline-primary", "sm");
    UIStyleHelper::addHoverEffect(viewButton, "lift");
}
```

### OrderEntryPanel - Fixed Implementation

```cpp
void OrderEntryPanel::setupOrderActionsSection() {
    // Create container with consistent layout
    auto buttonContainer = actionsContainer_->addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(buttonContainer, "start", "center");
    
    // Style buttons consistently
    newOrderButton_ = buttonContainer->addNew<Wt::WPushButton>("🆕 Start New Order");
    UIStyleHelper::styleButton(newOrderButton_, "success", "lg");
    UIStyleHelper::addHoverEffect(newOrderButton_, "lift");
    
    sendToKitchenButton_ = buttonContainer->addNew<Wt::WPushButton>("🍳 Send to Kitchen");
    UIStyleHelper::styleButton(sendToKitchenButton_, "primary", "lg");
    UIStyleHelper::addHoverEffect(sendToKitchenButton_, "lift");
}

void OrderEntryPanel::showOrderValidationMessage(const std::string& message, const std::string& type) {
    if (tableStatusText_) {
        UIStyleHelper::styleStatusText(tableStatusText_, type);
        
        // Add contextual background styling
        if (type == "success") {
            tableStatusText_->addStyleClass("bg-success-subtle border-success");
        } else if (type == "warning") {
            tableStatusText_->addStyleClass("bg-warning-subtle border-warning");
        }
        
        tableStatusText_->setText("✅ " + message);
        UIStyleHelper::addFadeIn(tableStatusText_);
    }
}
```

### MenuDisplay - Fixed Implementation

```cpp
void MenuDisplay::initializeUI() {
    // Apply menu display styling
    UIStyleHelper::styleMenuDisplay(this);
    
    // Create menu group with consistent styling
    menuGroup_ = addNew<Wt::WGroupBox>("🍽️ Restaurant Menu");
    UIStyleHelper::styleGroupBox(menuGroup_, "primary");
    UIStyleHelper::addHoverEffect(menuGroup_, "shadow");
    
    // Create table with menu-specific styling
    itemsTable_ = container->addNew<Wt::WTable>();
    UIStyleHelper::styleTable(itemsTable_, "menu");
}

void MenuDisplay::addMenuItemRow(const std::shared_ptr<MenuItem>& item, size_t index) {
    // Category badge with consistent styling
    auto categoryBadge = container->addNew<Wt::WText>(categoryName);
    UIStyleHelper::styleBadge(categoryBadge, getCategoryBadgeVariant(item->getCategory()));
    
    // Add to order button with consistent styling
    auto addButton = container->addNew<Wt::WPushButton>("➕ Add");
    if (item->isAvailable() && canAddToOrder()) {
        UIStyleHelper::styleButton(addButton, "primary", "sm");
        UIStyleHelper::addHoverEffect(addButton, "lift");
    } else {
        UIStyleHelper::styleButton(addButton, "outline-secondary", "sm");
    }
}
```

## Updated CommonHeader with Consistent Styling

```cpp
void CommonHeader::initializeUI() {
    // Apply header styling
    UIStyleHelper::styleHeaderComponent(this);
    
    // Create layout container
    auto headerLayout = addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(headerLayout, "between", "center");
    
    // Branding section
    brandingText_ = headerLayout->addNew<Wt::WText>("🍴 Restaurant POS");
    UIStyleHelper::styleHeading(brandingText_, 3, "white");
    brandingText_->addStyleClass("pos-text-shadow");
    
    // Theme controls
    themeComboBox_ = headerLayout->addNew<Wt::WComboBox>();
    UIStyleHelper::styleComboBox(themeComboBox_, "sm");
    UIStyleHelper::addHoverEffect(themeComboBox_, "glow");
    
    themeToggleButton_ = headerLayout->addNew<Wt::WPushButton>("🌙");
    UIStyleHelper::styleButton(themeToggleButton_, "outline-light", "sm");
}
```

## Updated CommonFooter with Consistent Styling

```cpp
void CommonFooter::initializeUI() {
    // Apply footer styling
    UIStyleHelper::styleFooterComponent(this);
    
    // Status section with consistent layout
    auto statusContainer = addNew<Wt::WContainerWidget>();
    UIStyleHelper::styleFlexRow(statusContainer, "start", "center");
    
    // Status indicators with consistent styling
    activeOrdersText_ = statusContainer->addNew<Wt::WText>("📋 Active Orders: 0");
    UIStyleHelper::styleStatusText(activeOrdersText_, "info");
    
    kitchenQueueText_ = statusContainer->addNew<Wt::WText>("👨‍🍳 Kitchen Queue: 0");
    UIStyleHelper::styleStatusText(kitchenQueueText_, "info");
    
    systemStatusText_ = statusContainer->addNew<Wt::WText>("✅ System: Online");
    UIStyleHelper::styleStatusText(systemStatusText_, "success");
}

void CommonFooter::updateStatus() {
    // Update with consistent badge styling
    if (activeOrders.size() > 10) {
        UIStyleHelper::styleStatusText(systemStatusText_, "danger");
        systemStatusText_->setText("⚠️ System: Busy");
    } else if (activeOrders.size() > 5) {
        UIStyleHelper::styleStatusText(systemStatusText_, "warning");
        systemStatusText_->setText("🟡 System: Moderate");
    } else {
        UIStyleHelper::styleStatusText(systemStatusText_, "success");
        systemStatusText_->setText("✅ System: Online");
    }
}
```

## Component Style Mapping

| Component | Container Style | Primary Elements | Special Features |
|-----------|----------------|------------------|------------------|
| ActiveOrdersDisplay | `styleOrderDisplay()` | Table: `"orders"`, Buttons: `"outline-primary"` | Status badges, hover effects |
| OrderEntryPanel | `styleContainer("panel")` | Buttons: `"success"/"primary"/"warning"` | Form validation styling |
| MenuDisplay | `styleMenuDisplay()` | Table: `"menu"`, Buttons: `"primary"` | Category badges, item availability |
| CurrentOrderDisplay | `styleOrderDisplay()` | Table: `"compact"`, Buttons: `"outline-danger"` | Price highlighting |
| KitchenStatusDisplay | `styleKitchenDisplay()` | Progress bars, status indicators | Real-time updates |
| CommonHeader | `styleHeaderComponent()` | Headings: `"white"`, Buttons: `"outline-light"` | Brand styling |
| CommonFooter | `styleFooterComponent()` | Status text, info badges | System status colors |

## Theme Integration

### Apply Theme-Aware Styling

```cpp
// Make any component theme-aware
UIStyleHelper::applyThemeAwareStyle(widget);

// Apply theme to containers (if ThemeService available)
if (themeService_) {
    themeService_->applyThemeToContainer(container);
}
```

### Theme-Specific Overrides

The CSS automatically handles theme overrides:

- **Light Theme**: High contrast, bright colors
- **Dark Theme**: Dark backgrounds, light text
- **Warm Theme**: Earth tones, restaurant-friendly
- **Cool Theme**: Blue tones, tech-focused

## Animation and Effects

### Add Animations Consistently

```cpp
// Fade-in animation for new components
UIStyleHelper::addFadeIn(widget);

// Hover effects for interactive elements
UIStyleHelper::addHoverEffect(button, "lift");   // Lift on hover
UIStyleHelper::addHoverEffect(card, "shadow");   // Shadow on hover
UIStyleHelper::addHoverEffect(input, "glow");    // Glow on hover
```

### Responsive Design

```cpp
// Add responsive spacing
UIStyleHelper::addResponsiveSpacing(container);

// Hide on mobile/tablet
UIStyleHelper::addResponsiveVisibility(widget, true, false); // Hide on mobile only
UIStyleHelper::addResponsiveVisibility(widget, true, true);  // Hide on mobile and tablet
```

## Best Practices

### 1. Always Use UIStyleHelper

```cpp
// ✅ Good - Consistent styling
UIStyleHelper::styleButton(button, "primary", "lg");

// ❌ Bad - Direct CSS classes
button->addStyleClass("btn btn-primary btn-lg");
```

### 2. Apply Container Styles First

```cpp
// ✅ Good - Apply container style first
UIStyleHelper::styleContainer(this, "panel");
addStyleClass("my-specific-class");

// ❌ Bad - No base container styling
addStyleClass("my-specific-class");
```

### 3. Use Semantic Variants

```cpp
// ✅ Good - Semantic meaning
UIStyleHelper::styleButton(saveButton, "success", "lg");
UIStyleHelper::styleButton(cancelButton, "secondary", "md");
UIStyleHelper::styleButton(deleteButton, "danger", "sm");

// ❌ Bad - No semantic meaning
UIStyleHelper::styleButton(saveButton, "primary", "lg");
UIStyleHelper::styleButton(cancelButton, "primary", "md");
UIStyleHelper::styleButton(deleteButton, "primary", "sm");
```

### 4. Consistent Badge Usage

```cpp
// Status badges
UIStyleHelper::styleBadge(statusText, "success");  // For completed/ready
UIStyleHelper::styleBadge(statusText, "warning");  // For in-progress
UIStyleHelper::styleBadge(statusText, "danger");   // For cancelled/error
UIStyleHelper::styleBadge(statusText, "info");     // For information
UIStyleHelper::styleBadge(statusText, "secondary"); // For neutral states
```

## Troubleshooting

### CSS Not Loading
1. Ensure `pos-consistent-styles.css` is in the correct path
2. Verify Bootstrap is loaded before custom CSS
3. Check browser console for CSS loading errors

### Styles Not Applying
1. Ensure `UIStyleHelper.hpp` is included
2. Check that containers are styled before child elements
3. Verify theme service is properly initialized

### Inconsistent Appearance
1. Use UIStyleHelper methods instead of direct CSS classes
2. Apply consistent container styles across all components
3. Use semantic color variants (success, warning, danger, etc.)

### Performance Issues
1. Apply animations only to interactive elements
2. Use `prefers-reduced-motion` media query for accessibility
3. Limit hover effects on mobile devices

## Migration from Old Styling

### Step 1: Update Includes
```cpp
// Add to all component files
#include "UIStyleHelper.hpp"
```

### Step 2: Replace Direct CSS Classes
```cpp
// Old approach
button->addStyleClass("btn btn-primary btn-lg me-2 mb-2");

// New approach
UIStyleHelper::styleButton(button, "primary", "lg");
```

### Step 3: Apply Container Styling
```cpp
// Add to component constructors
UIStyleHelper::styleContainer(this, "panel"); // or appropriate variant
```

### Step 4: Update Theme Handling
```cpp
// Old approach
if (isDarkMode) {
    container->addStyleClass("dark-theme");
}

// New approach
UIStyleHelper::applyThemeAwareStyle(container);
```

This integration ensures all components have consistent, professional styling that works across all themes and devices.