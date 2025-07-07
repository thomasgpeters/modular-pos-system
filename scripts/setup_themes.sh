#!/bin/bash
# ============================================================================
# Theme System Setup Script
# File: scripts/setup_themes.sh
# ============================================================================

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_ROOT=$(dirname "$(dirname "$(readlink -f "$0")")")
RESOURCES_DIR="$PROJECT_ROOT/resources/css"
BUILD_DIR="$PROJECT_ROOT/build"
DOCROOT_DIR="$BUILD_DIR/docroot/css"
EXTERNAL_RESOURCES_DIR="$BUILD_DIR/resources/css"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Restaurant POS Theme System Setup    ${NC}"
echo -e "${BLUE}========================================${NC}"
echo

# Function to print status messages
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to create directory if it doesn't exist
create_dir() {
    if [ ! -d "$1" ]; then
        mkdir -p "$1"
        print_status "Created directory: $1"
    else
        print_status "Directory already exists: $1"
    fi
}

# Function to copy file with status
copy_file() {
    local src="$1"
    local dst="$2"
    
    if [ -f "$src" ]; then
        cp "$src" "$dst"
        print_status "Copied: $(basename "$src") -> $dst"
    else
        print_error "Source file not found: $src"
        return 1
    fi
}

# Function to validate CSS file
validate_css() {
    local css_file="$1"
    
    if [ -f "$css_file" ]; then
        # Basic validation - check if file has CSS content
        if grep -q "theme-" "$css_file" && grep -q "{" "$css_file"; then
            print_status "CSS validation passed: $(basename "$css_file")"
            return 0
        else
            print_warning "CSS validation failed: $(basename "$css_file")"
            return 1
        fi
    else
        print_error "CSS file not found: $css_file"
        return 1
    fi
}

# Main setup function
main() {
    print_status "Starting theme system setup..."
    print_status "Project root: $PROJECT_ROOT"
    
    # Check if we're in the right directory
    if [ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]; then
        print_error "CMakeLists.txt not found. Please run this script from the project root or its scripts directory."
        exit 1
    fi
    
    # Create necessary directories
    print_status "Creating directory structure..."
    create_dir "$RESOURCES_DIR"
    create_dir "$BUILD_DIR"
    create_dir "$DOCROOT_DIR"
    create_dir "$EXTERNAL_RESOURCES_DIR"
    create_dir "$PROJECT_ROOT/config"
    
    # Check if CSS files exist in resources
    CSS_FILES=(
        "theme-framework.css"
        "theme-light.css"
        "theme-dark.css"
        "theme-warm.css"
        "theme-cool.css"
    )
    
    print_status "Checking CSS files..."
    missing_files=0
    
    for css_file in "${CSS_FILES[@]}"; do
        if [ ! -f "$RESOURCES_DIR/$css_file" ]; then
            print_warning "CSS file missing: $css_file"
            missing_files=$((missing_files + 1))
        else
            validate_css "$RESOURCES_DIR/$css_file"
        fi
    done
    
    if [ $missing_files -gt 0 ]; then
        print_error "Missing $missing_files CSS files. Please ensure all theme CSS files are in $RESOURCES_DIR"
        print_status "Required files: ${CSS_FILES[*]}"
        exit 1
    fi
    
    # Copy CSS files to build locations
    print_status "Copying CSS files..."
    
    for css_file in "${CSS_FILES[@]}"; do
        # Copy to docroot for web access
        copy_file "$RESOURCES_DIR/$css_file" "$DOCROOT_DIR/$css_file"
        
        # Copy to external resources
        copy_file "$RESOURCES_DIR/$css_file" "$EXTERNAL_RESOURCES_DIR/$css_file"
    done
    
    # Create index.html for testing (optional)
    print_status "Creating test HTML file..."
    cat > "$DOCROOT_DIR/../index.html" << 'EOF'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Restaurant POS - Theme Test</title>
    <link href="css/theme-framework.css" rel="stylesheet">
</head>
<body class="theme-base">
    <div class="container mt-4">
        <h1>Restaurant POS Theme System Test</h1>
        <p>This page can be used to test the theme system.</p>
        
        <div class="pos-card">
            <h3>Theme Selector Test</h3>
            <div class="pos-mode-selector">
                <button class="pos-btn pos-btn-primary" onclick="switchTheme('base')">Base</button>
                <button class="pos-btn pos-btn-secondary" onclick="switchTheme('light')">Light</button>
                <button class="pos-btn pos-btn-secondary" onclick="switchTheme('dark')">Dark</button>
                <button class="pos-btn pos-btn-secondary" onclick="switchTheme('warm')">Warm</button>
                <button class="pos-btn pos-btn-secondary" onclick="switchTheme('cool')">Cool</button>
            </div>
        </div>
    </div>
    
    <script>
    function switchTheme(theme) {
        // Remove all theme classes
        document.body.className = document.body.className.replace(/theme-\w+/g, '');
        
        // Add new theme class
        document.body.classList.add('theme-' + theme);
        
        // Load theme CSS if not base
        if (theme !== 'base') {
            var link = document.getElementById('theme-css');
            if (!link) {
                link = document.createElement('link');
                link.id = 'theme-css';
                link.rel = 'stylesheet';
                document.head.appendChild(link);
            }
            link.href = 'css/theme-' + theme + '.css';
        } else {
            var link = document.getElementById('theme-css');
            if (link) {
                link.remove();
            }
        }
        
        // Update button states
        var buttons = document.querySelectorAll('.pos-mode-selector .pos-btn');
        buttons.forEach(btn => btn.className = 'pos-btn pos-btn-secondary');
        event.target.className = 'pos-btn pos-btn-primary';
    }
    </script>
</body>
</html>
EOF
    
    # Set up build configuration
    print_status "Setting up build configuration..."
    
    # Check if CMake is available
    if command_exists cmake; then
        print_status "CMake found, configuring build..."
        cd "$BUILD_DIR"
        cmake .. -DCMAKE_BUILD_TYPE=Debug
        print_status "Build configured. Run 'make' in the build directory to build the project."
    else
        print_warning "CMake not found. You'll need to configure the build manually."
    fi
    
    # Create a quick verification script
    print_status "Creating verification script..."
    cat > "$PROJECT_ROOT/verify_themes.sh" << 'EOF'
#!/bin/bash
echo "Theme System Verification"
echo "========================="

# Check CSS files
echo "Checking CSS files..."
for css in build/docroot/css/theme-*.css; do
    if [ -f "$css" ]; then
        echo "✓ $(basename "$css")"
    else
        echo "✗ $(basename "$css") - MISSING"
    fi
done

# Check if test page exists
if [ -f "build/docroot/index.html" ]; then
    echo "✓ Test page available at http://localhost:8080/"
else
    echo "✗ Test page not found"
fi

echo
echo "To test themes:"
echo "1. Build and run your POS application"
echo "2. Open http://localhost:8080/ in your browser"
echo "3. Click the theme buttons to test switching"
EOF
    
    chmod +x "$PROJECT_ROOT/verify_themes.sh"
    
    # Summary
    echo
    print_status "Theme system setup completed successfully!"
    echo
    echo -e "${BLUE}Summary:${NC}"
    echo "  • CSS files copied to: $DOCROOT_DIR"
    echo "  • External resources: $EXTERNAL_RESOURCES_DIR"
    echo "  • Test page created: $BUILD_DIR/docroot/index.html"
    echo "  • Verification script: $PROJECT_ROOT/verify_themes.sh"
    echo
    echo -e "${BLUE}Next steps:${NC}"
    echo "  1. Add the CMakeLists.txt theme configuration to your project"
    echo "  2. Include the ThemeService implementation in your build"
    echo "  3. Build and run your application"
    echo "  4. Run ./verify_themes.sh to test the setup"
    echo
    echo -e "${GREEN}Setup completed successfully!${NC}"
}

# Handle command line arguments
case "${1:-}" in
    "--help"|"-h")
        echo "Usage: $0 [--help|--verify]"
        echo
        echo "Options:"
        echo "  --help     Show this help message"
        echo "  --verify   Verify existing theme setup"
        echo
        echo "This script sets up the Restaurant POS theme system by:"
        echo "  • Creating necessary directory structure"
        echo "  • Copying CSS files to appropriate locations"
        echo "  • Creating test and verification files"
        echo "  • Configuring the build system"
        exit 0
        ;;
    "--verify")
        print_status "Verifying theme system setup..."
        if [ -f "$PROJECT_ROOT/verify_themes.sh" ]; then
            "$PROJECT_ROOT/verify_themes.sh"
        else
            print_error "Verification script not found. Please run setup first."
            exit 1
        fi
        exit 0
        ;;
    "")
        main
        ;;
    *)
        print_error "Unknown option: $1"
        print_status "Use --help for usage information"
        exit 1
        ;;
esac
