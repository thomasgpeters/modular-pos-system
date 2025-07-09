# CSS Directory Structure for Restaurant POS

## 📁 Recommended Folder Layout

```
assets/
└── css/
    ├── theme-framework.css         # Core theme system
    ├── responsive.css              # Mobile/tablet support
    │
    ├── themes/                     # User-selectable themes
    │   ├── light-theme.css
    │   ├── dark-theme.css
    │   ├── warm-theme.css
    │   └── cool-theme.css
    │
    └── components/                 # Component-specific styles
        ├── common-components.css   # Shared components
        ├── pos-components.css      # POS mode specific
        └── kitchen-components.css  # Kitchen mode specific
```

## 🎯 How the CMakeLists.txt Works

### **Automatic CSS Deployment**
```bash
# During build, CMake will:
assets/css/themes/light-theme.css → build/assets/css/themes/light-theme.css
assets/css/components/pos-components.css → build/assets/css/components/pos-components.css
assets/css/theme-framework.css → build/assets/css/theme-framework.css
```

### **Development Commands**
```bash
# Full build (includes CSS copying)
make

# Update CSS files only (during development)
make update-css

# Clean CSS build files
make clean-css

# Copy all CSS assets
make copy_css_assets
```

## 🔧 Integration with Your Code

### **Using the Generated Config**
```cpp
#include "config/DocRootConfig.hpp"

// In your ThemeService or Application code:
app->useStyleSheet(Config::Themes::LIGHT_THEME);
app->useStyleSheet(Config::Components::POS_COMPONENTS);
```

### **Web Server Setup**
Your Wt application will serve files from:
- **Docroot**: `build/` directory  
- **CSS URL**: `http://localhost:8080/assets/css/themes/light-theme.css`
- **Local Path**: `build/assets/css/themes/light-theme.css`

## ✅ Setup Checklist

1. **Create the directories**:
   ```bash
   mkdir -p assets/css/themes
   mkdir -p assets/css/components
   ```

2. **Create the CMakeLists.txt** (from the artifact above)

3. **Create the DocRootConfig.hpp.in** (from the artifact above)

4. **Add your CSS files** to the appropriate directories

5. **Build and test**:
   ```bash
   cmake .
   make
   ./restaurant_pos --docroot=build --http-port=8080
   ```

## 🎨 Theme Development Workflow

1. **Edit source CSS files** in `assets/css/`
2. **Run `make update-css`** to copy changes to build directory  
3. **Refresh browser** to see changes
4. **No need to rebuild C++ code** for CSS-only changes

This setup gives you **hot CSS reloading** during development while ensuring proper deployment structure!