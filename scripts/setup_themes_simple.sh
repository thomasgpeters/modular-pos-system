#!/bin/bash
# ============================================================================
# Simple Theme Setup Script (No external dependencies)
# File: setup_themes_simple.sh
# ============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}Restaurant POS Theme Setup${NC}"
echo "=========================="

# Get project root (assuming script is run from project root)
PROJECT_ROOT="$(pwd)"
RESOURCES_DIR="$PROJECT_ROOT/resources/css"
BUILD_DIR="$PROJECT_ROOT/build"

# Function to print status
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the right place
if [ ! -f "CMakeLists.txt" ]; then
    print_error "CMakeLists.txt not found. Please run this script from your project root directory."
    exit 1
fi

print_status "Project root: $PROJECT_ROOT"

# Create resources directory if it doesn't exist
if [ ! -d "$RESOURCES_DIR" ]; then
    mkdir -p "$RESOURCES_DIR"
    print_status "Created: $RESOURCES_DIR"
fi

# Check for CSS files
CSS_FILES=(
    "theme-framework.css"
    "theme-light.css" 
    "theme-dark.css"
    "theme-warm.css"
    "theme-cool.css"
)

print_status "Checking for CSS files in $RESOURCES_DIR..."

missing_count=0
for css_file in "${CSS_FILES[@]}"; do
    if [ -f "$RESOURCES_DIR/$css_file" ]; then
        print_status "✓ Found: $css_file"
    else
        print_warning "✗ Missing: $css_file"
        missing_count=$((missing_count + 1))
    fi
done

if [ $missing_count -gt 0 ]; then
    print_error "Missing $missing_count CSS files!"
    echo
    echo "Please copy the following files to $RESOURCES_DIR:"
    for css_file in "${CSS_FILES[@]}"; do
        if [ ! -f "$RESOURCES_DIR/$css_file" ]; then
            echo "  - $css_file"
        fi
    done
    echo
    echo "You can find these files in the artifacts I provided."
    exit 1
fi

# Create build directories
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
    print_status "Created build directory"
fi

# Create docroot directory structure
mkdir -p "$BUILD_DIR/docroot/css"
mkdir -p "$BUILD_DIR/build/resources/css"
print_status "Created build directory structure"

# Copy CSS files
print_status "Copying CSS files to build locations..."
for css_file in "${CSS_FILES[@]}"; do
    if [ -f "$RESOURCES_DIR/$css_file" ]; then
        cp "$RESOURCES_DIR/$css_file" "$BUILD_DIR/docroot/css/"
        cp "$RESOURCES_DIR/$css_file" "$BUILD_DIR/build/resources/css/"
        print_status "Copied: $css_file"
    fi
done

# Create a simple test HTML file
print_status "Creating test page..."
cat > "$BUILD_DIR/docroot/test_themes.html" << 'EOF'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Restaurant POS - Theme Test</title>
    <link href="https://cdnjs.cloudflare.com/ajax/libs/bootstrap/5.3.0/css/bootstrap.min.css" rel="stylesheet">
    <link href="css/theme-framework.css" rel="stylesheet">
    <style>
        .theme-demo { margin: 20px 0; padding: 20px; }
        .color-box { width: 50px; height: 50px; display: inline-block; margin: 5px; border: 1px solid #ccc; }
    </style>
</head>
<body class="theme-base">
    <div class="container mt-4">
        <h1>🍽️ Restaurant POS Theme Test</h1>
        <p>Use this page to test your theme system before integrating with your main application.</p>
        
        <div class="pos-card">
            <h3>Theme Selector</h3>
            <div class="pos-mode-selector mb-3">
                <button class="pos-btn pos-btn-primary active" onclick="switchTheme('base', this)">Base</button>
                <button class="pos-btn pos-btn-secondary" onclick="switchTheme('light', this)">Light</button>
                <button class="pos-btn pos-btn-secondary" onclick="switchTheme('dark', this)">Dark</button>
                <button class="pos-btn pos-btn-secondary" onclick="switchTheme('warm', this)">Warm</button>
                <button class="pos-btn pos-btn-secondary" onclick="switchTheme('cool', this)">Cool</button>
            </div>
            <p id="current-theme">Current theme: <strong>Base</strong></p>
        </div>
        
        <div class="row">
            <div class="col-md-6">
                <div class="pos-card">
                    <h4>Menu Items Demo</h4>
                    <table class="pos-table table">
                        <thead>
                            <tr>
                                <th>Item</th>
                                <th>Price</th>
                                <th>Status</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>Burger</td>
                                <td>$12.99</td>
                                <td><span class="pos-badge pos-badge-success">Available</span></td>
                            </tr>
                            <tr>
                                <td>Pizza</td>
                                <td>$15.99</td>
                                <td><span class="pos-badge pos-badge-warning">Limited</span></td>
                            </tr>
                            <tr>
                                <td>Salad</td>
                                <td>$8.99</td>
                                <td><span class="pos-badge pos-badge-danger">Out of Stock</span></td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>
            
            <div class="col-md-6">
                <div class="pos-card">
                    <h4>Order Status Demo</h4>
                    <div class="order-pending p-2 mb-2 rounded">Order #1001 - Pending</div>
                    <div class="order-preparing p-2 mb-2 rounded">Order #1002 - Preparing</div>
                    <div class="order-ready p-2 mb-2 rounded">Order #1003 - Ready</div>
                    <div class="order-served p-2 mb-2 rounded">Order #1004 - Served</div>
                </div>
            </div>
        </div>
        
        <div class="pos-card">
            <h4>Form Controls Demo</h4>
            <div class="row">
                <div class="col-md-4">
                    <input type="text" class="pos-form-control mb-2" placeholder="Customer name">
                </div>
                <div class="col-md-4">
                    <select class="pos-form-control mb-2">
                        <option>Table 1</option>
                        <option>Table 2</option>
                        <option>Takeout</option>
                    </select>
                </div>
                <div class="col-md-4">
                    <button class="pos-btn pos-btn-success">Add Order</button>
                </div>
            </div>
        </div>
    </div>
    
    <script>
    let currentThemeLink = null;
    
    function switchTheme(theme, button) {
        // Remove all theme classes
        document.body.className = document.body.className.replace(/theme-\w+/g, '');
        
        // Add new theme class
        document.body.classList.add('theme-' + theme);
        
        // Load theme CSS
        if (currentThemeLink) {
            currentThemeLink.remove();
            currentThemeLink = null;
        }
        
        if (theme !== 'base') {
            currentThemeLink = document.createElement('link');
            currentThemeLink.rel = 'stylesheet';
            currentThemeLink.href = 'css/theme-' + theme + '.css';
            document.head.appendChild(currentThemeLink);
        }
        
        // Update button states
        document.querySelectorAll('.pos-mode-selector .pos-btn').forEach(btn => {
            btn.className = 'pos-btn pos-btn-secondary';
        });
        button.className = 'pos-btn pos-btn-primary active';
        
        // Update current theme display
        document.getElementById('current-theme').innerHTML = 'Current theme: <strong>' + 
            theme.charAt(0).toUpperCase() + theme.slice(1) + '</strong>';
        
        console.log('Switched to theme:', theme);
    }
    </script>
</body>
</html>
EOF

# Create verification script
print_status "Creating verification script..."
cat > "$PROJECT_ROOT/verify_themes.sh" << 'EOF'
#!/bin/bash

echo "Theme System Verification"
echo "========================="

# Check CSS files in docroot
echo "Checking CSS files in build/docroot/css/:"
for css in build/docroot/css/theme-*.css; do
    if [ -f "$css" ]; then
        echo "✓ $(basename "$css")"
    else
        echo "✗ $(basename "$css") - MISSING"
    fi
done

echo
echo "Checking CSS files in build/build/resources/css/:"
for css in build/build/resources/css/theme-*.css; do
    if [ -f "$css" ]; then
        echo "✓ $(basename "$css")"
    else
        echo "✗ $(basename "$css") - MISSING"
    fi
done

echo
echo "Theme test page: build/docroot/test_themes.html"
if [ -f "build/docroot/test_themes.html" ]; then
    echo "✓ Test page created"
    echo
    echo "To test themes:"
    echo "  1. Start a local web server in the build/docroot directory:"
    echo "     cd build/docroot && python3 -m http.server 8000"
    echo "  2. Open http://localhost:8000/test_themes.html"
    echo "  3. Click the theme buttons to test switching"
else
    echo "✗ Test page not found"
fi

echo
echo "Build configuration:"
echo "  Run: cd build && cmake .. && make"
EOF

chmod +x "$PROJECT_ROOT/verify_themes.sh"

print_status "Setup completed successfully!"
echo
echo -e "${BLUE}Next steps:${NC}"
echo "1. Update your CMakeLists.txt with the corrected theme configuration"
echo "2. Update the target name in CMakeLists.txt (look for POS_TARGET_NAME)"
echo "3. Build your project: cd build && cmake .. && make"
echo "4. Run verification: ./verify_themes.sh"
echo "5. Test themes using the test page"
echo
echo -e "${GREEN}Files created:${NC}"
echo "  • build/docroot/css/ (CSS files for web access)"
echo "  • build/build/resources/css/ (CSS files for external access)"
echo "  • build/docroot/test_themes.html (theme test page)"
echo "  • verify_themes.sh (verification script)"
echo
echo -e "${YELLOW}Important:${NC} Make sure to update the POS_TARGET_NAME in your CMakeLists.txt!"
