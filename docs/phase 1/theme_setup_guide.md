# Restaurant POS Theme System Setup Guide

## Overview
The Restaurant POS system now uses an external theme system that allows you to modify styles without recompiling the application. This guide shows you how to set up and customize themes.

## Directory Structure

Create the following directory structure in your application root:

```
restaurant-pos/
├── src/
│   ├── main.cpp
│   ├── RestaurantPOSApp.cpp          # Updated implementation
│   └── ... (other source files)
├── include/
│   ├── RestaurantPOSApp.hpp          # Updated header
│   └── ... (other header files)
├── themes/                           # Theme directory (NEW)
│   ├── theme-config.json            # Theme configuration
│   ├── base.css                     # Base styles (required)
│   ├── bootstrap.css                # Bootstrap theme
│   ├── classic.css                  # Classic theme
│   ├── professional.css             # Professional theme
│   ├── colorful.css                 # Colorful theme
│   └── custom/                      # User-defined themes
│       ├── my-theme.css
│       └── another-theme.css
├── build/
└── ... (other project files)
```

## Installation Steps

### 1. Create Theme Directory
```bash
mkdir -p themes/custom
```

### 2. Copy Theme Files
Copy all the provided CSS files to your `themes/` directory:
- `theme-config.json`
- `base.css`
- `bootstrap.css`
- `classic.css`
- `professional.css`
- `colorful.css`

### 3. Update Your Code
Replace your existing `RestaurantPOSApp.cpp` and `RestaurantPOSApp.hpp` files with the updated versions provided.

### 4. Update CMakeLists.txt (Optional)
Add theme files to your build system:

```cmake
# Copy theme files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/themes DESTINATION ${CMAKE_BINARY_DIR})

# Install theme files
install(DIRECTORY themes/ DESTINATION themes)
```

### 5. Compile and Run
```bash
mkdir build && cd build
cmake ..
make
./restaurant_pos --http-port 8081 --docroot ../
```

## Theme Configuration

### JSON Configuration File
The `themes/theme-config.json` file controls all theme settings:

```json
{
  "themes": [
    {
      "id": "my-custom-theme",
      "name": "My Custom Theme",
      "description": "A personalized theme for our restaurant",
      "cssFile": "themes/custom/my-theme.css",
      "previewColors": ["#ff6b6b", "#4ecdc4"],
      "isDefault": false
    }
  ],
  "config": {
    "defaultTheme": "bootstrap",
    "allowUserThemes": true,
    "themeDirectory": "themes/",
    "customThemeDirectory": "themes/custom/"
  }
}
```

## Creating Custom Themes

### Method 1: Modify Existing Themes
1. Copy an existing theme file (e.g., `bootstrap.css`)
2. Rename it (e.g., `my-restaurant.css`)
3. Modify the CSS variables and styles
4. Update `theme-config.json` to include your theme

### Method 2: Create From Scratch
1. Create a new CSS file in `themes/custom/`
2. Import the base styles: `@import url('../base.css');`
3. Override CSS variables and add custom styles
4. Add theme configuration to JSON file

### Example Custom Theme CSS:
```css
/* themes/custom/my-restaurant.css */
@import url('../base.css');

:root {
    --primary-color: #8B4513;      /* Brown */
    --secondary-color: #D2691E;     /* Chocolate */
    --success-color: #228B22;       /* Forest Green */
    --danger-color: #DC143C;        /* Crimson */
    --body-bg: #FFF8DC;            /* Cornsilk */
}

body {
    font-family: 'Georgia', serif;
    background-image: url('wood-texture.jpg');
}

.btn-primary {
    background: linear-gradient(45deg, var(--primary-color), var(--secondary-color));
    border-radius: 10px;
}
```

## CSS Variables Reference

The theme system uses CSS custom properties (variables) that you can override:

### Color Variables
```css
:root {
    --primary-color: #007bff;       /* Main brand color */
    --primary-dark: #0056b3;        /* Darker shade */
    --secondary-color: #6c757d;     /* Secondary color */
    --success-color: #28a745;       /* Success indicators */
    --danger-color: #dc3545;        /* Error/danger */
    --warning-color: #ffc107;       /* Warnings */
    --info-color: #17a2b8;          /* Information */
    --light-color: #f8f9fa;         /* Light backgrounds */
    --dark-color: #343a40;          /* Dark elements */
    
    --text-color: #333;             /* Main text */
    --muted-color: #6c757d;         /* Muted text */
    --border-color: #dee2e6;        /* Borders */
    --input-bg: #fff;               /* Form inputs */
    --table-bg: #fff;               /* Table background */
    --body-bg: #f8f9fa;            /* Page background */
}
```

### Component Classes
Target specific POS components:
- `.pos-header` - Main header
- `.theme-button` - Theme selector button
- `.order-summary` - Order summary panels
- `.kitchen-status` - Kitchen status indicators
- `.notification` - Toast notifications

## Runtime Theme Management

### Switching Themes
- Users can click the "🎨 Themes" button
- Select from available themes
- Changes apply immediately without restart

### Reloading Themes
- Click "Reload Themes" in the theme selector
- Loads any new themes added to the configuration
- Useful during development

### Adding Themes at Runtime
1. Add new CSS file to `themes/` directory
2. Update `theme-config.json`
3. Click "Reload Themes" in application
4. New theme becomes available immediately

## Deployment Considerations

### Web Server Configuration
Ensure your web server can serve CSS files:

**Apache (.htaccess):**
```apache
<FilesMatch "\.(css)$">
    Header set Cache-Control "max-age=86400"
</FilesMatch>
```

**Nginx:**
```nginx
location ~* \.(css)$ {
    expires 1d;
    add_header Cache-Control "public, immutable";
}
```

### File Permissions
```bash
chmod 644 themes/*.css
chmod 644 themes/theme-config.json
chmod 755 themes/
```

## Troubleshooting

### Theme Not Loading
1. Check file permissions
2. Verify CSS file path in configuration
3. Check browser console for 404 errors
4. Ensure CSS syntax is valid

### Styles Not Applying
1. Clear browser cache
2. Check CSS specificity
3. Verify CSS variables are properly defined
4. Use browser developer tools to inspect styles

### Configuration Errors
1. Validate JSON syntax in `theme-config.json`
2. Check file paths are correct
3. Ensure theme IDs are unique
4. Verify all required fields are present

## Best Practices

### Theme Development
1. **Start with base.css** - Always import base styles
2. **Use CSS variables** - Override variables instead of hardcoding values
3. **Test responsiveness** - Ensure themes work on mobile devices
4. **Maintain contrast** - Ensure adequate color contrast for accessibility
5. **Document changes** - Add comments explaining custom modifications

### Performance
1. **Minimize CSS size** - Remove unused styles
2. **Optimize images** - Compress background images
3. **Use CDN** - Consider CDN for external resources
4. **Cache strategies** - Implement proper caching headers

### Maintenance
1. **Version control** - Track theme changes in git
2. **Backup themes** - Keep backups of custom themes
3. **Test updates** - Test themes after application updates
4. **User feedback** - Collect feedback on theme usability

## Advanced Customization

### Dynamic Theme Variables
Create themes that adapt to user preferences:

```css
/* themes/adaptive.css */
@import url('base.css');

:root {
    --primary-hue: 210;  /* Blue by default */
    --primary-color: hsl(var(--primary-hue), 80%, 50%);
    --primary-dark: hsl(var(--primary-hue), 80%, 40%);
}

/* Can be modified via JavaScript */
[data-user-preference="warm"] {
    --primary-hue: 30;   /* Orange tones */
}

[data-user-preference="cool"] {
    --primary-hue: 200;  /* Blue tones */
}
```

### Theme-Specific Components
```css
/* Add theme-specific enhancements */
.colorful-theme .btn {
    position: relative;
    overflow: hidden;
}

.colorful-theme .btn::before {
    content: '';
    position: absolute;
    top: 0;
    left: -100%;
    width: 100%;
    height: 100%;
    background: linear-gradient(90deg, transparent, rgba(255,255,255,0.2), transparent);
    transition: left 0.5s;
}

.colorful-theme .btn:hover::before {
    left: 100%;
}
```

This decoupled theme system provides maximum flexibility for customizing your Restaurant POS appearance without requiring code changes or recompilation!