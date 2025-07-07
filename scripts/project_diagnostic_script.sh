#!/bin/bash
# ============================================================================
# Project Diagnostic Script
# This will help you understand your current project structure
# ============================================================================

echo "🔍 Restaurant POS Project Diagnostic"
echo "===================================="
echo

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_status() { echo -e "${GREEN}[INFO]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARN]${NC} $1"; }
print_error() { echo -e "${RED}[ERROR]${NC} $1"; }
print_section() { echo -e "${BLUE}=== $1 ===${NC}"; }

# Check current directory
print_section "Project Location"
echo "Current directory: $(pwd)"
echo "Contents:"
ls -la
echo

# Check CMakeLists.txt
print_section "CMakeLists.txt Analysis"
if [ -f "CMakeLists.txt" ]; then
    print_status "Found CMakeLists.txt"
    
    # Check for project() command
    if grep -q "project(" CMakeLists.txt; then
        PROJECT_NAME=$(grep "project(" CMakeLists.txt | head -1)
        print_status "Project definition: $PROJECT_NAME"
    else
        print_warning "No project() command found"
    fi
    
    # Check for cmake_minimum_required
    if grep -q "cmake_minimum_required" CMakeLists.txt; then
        CMAKE_MIN=$(grep "cmake_minimum_required" CMakeLists.txt | head -1)
        print_status "CMake version: $CMAKE_MIN"
    else
        print_warning "No cmake_minimum_required() found"
    fi
    
    # Check for executable targets
    echo
    print_status "Looking for executable targets:"
    if grep -q "add_executable" CMakeLists.txt; then
        grep "add_executable" CMakeLists.txt | while read line; do
            echo "  Found: $line"
        done
    else
        print_warning "No add_executable() commands found"
    fi
    
    # Check for Wt
    echo
    if grep -q -i "wt\|witty" CMakeLists.txt; then
        print_status "Wt framework references found"
        grep -i "wt\|witty" CMakeLists.txt | head -3
    else
        print_warning "No Wt framework references found"
    fi
    
else
    print_error "CMakeLists.txt not found!"
    echo "This script should be run from your project root directory."
fi

echo

# Check directory structure
print_section "Directory Structure"
for dir in src include resources build; do
    if [ -d "$dir" ]; then
        print_status "Found: $dir/"
        if [ "$dir" = "src" ]; then
            echo "  Source files:"
            find src -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" | head -5 | sed 's/^/    /'
            total_src=$(find src -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" | wc -l)
            echo "    Total: $total_src source files"
        elif [ "$dir" = "include" ]; then
            echo "  Header files:"
            find include -name "*.hpp" -o -name "*.h" | head -5 | sed 's/^/    /'
            total_headers=$(find include -name "*.hpp" -o -name "*.h" | wc -l)
            echo "    Total: $total_headers header files"
        elif [ "$dir" = "resources" ]; then
            if [ -d "resources/css" ]; then
                echo "  CSS files:"
                find resources/css -name "*.css" | sed 's/^/    /'
            else
                echo "    No css subdirectory"
            fi
        fi
    else
        print_warning "Missing: $dir/"
    fi
done

echo

# Check for main function
print_section "Main Function Search"
if [ -d "src" ]; then
    main_files=$(find src -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" -exec grep -l "int main" {} \;)
    if [ -n "$main_files" ]; then
        print_status "Found main() function in:"
        echo "$main_files" | sed 's/^/  /'
    else
        print_warning "No main() function found in src/"
    fi
else
    print_warning "No src/ directory to search"
fi

echo

# Check for theme CSS files
print_section "Theme System Check"
theme_files=("theme-framework.css" "theme-light.css" "theme-dark.css" "theme-warm.css" "theme-cool.css")
css_found=0

if [ -d "resources/css" ]; then
    for css_file in "${theme_files[@]}"; do
        if [ -f "resources/css/$css_file" ]; then
            print_status "✓ $css_file"
            css_found=$((css_found + 1))
        else
            print_warning "✗ $css_file"
        fi
    done
    echo "Theme files found: $css_found/${#theme_files[@]}"
else
    print_warning "resources/css/ directory not found"
    echo "Theme files found: 0/${#theme_files[@]}"
fi

echo

# Check build directory
print_section "Build Status"
if [ -d "build" ]; then
    print_status "Build directory exists"
    if [ -f "build/CMakeCache.txt" ]; then
        print_status "CMake has been configured"
        if [ -f "build/Makefile" ]; then
            print_status "Makefiles generated"
        else
            print_warning "No Makefiles found"
        fi
    else
        print_warning "CMake not configured yet"
    fi
    
    # Check for built executables
    executables=$(find build -name "RestaurantPOS*" -o -name "restaurant*" -o -name "pos*" -type f -executable 2>/dev/null)
    if [ -n "$executables" ]; then
        print_status "Found built executables:"
        echo "$executables" | sed 's/^/  /'
    else
        print_warning "No built executables found"
    fi
else
    print_warning "No build directory"
fi

echo

# Recommendations
print_section "Recommendations"

if [ ! -f "CMakeLists.txt" ]; then
    print_error "1. You need a CMakeLists.txt file"
    echo "   Use the complete template I provided"
elif ! grep -q "project(" CMakeLists.txt; then
    print_error "1. Your CMakeLists.txt is missing project() command"
    echo "   Replace it with the complete template I provided"
else
    print_status "1. CMakeLists.txt structure looks OK"
fi

if [ ! -d "src" ] || [ $(find src -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" | wc -l) -eq 0 ]; then
    print_error "2. You need source files in src/ directory"
    echo "   Make sure your .cpp files are in src/"
else
    print_status "2. Source files found"
fi

if [ $css_found -eq 0 ]; then
    print_error "3. You need theme CSS files"
    echo "   Copy the 5 CSS files I provided to resources/css/"
elif [ $css_found -lt 5 ]; then
    print_warning "3. Some theme CSS files missing ($css_found/5)"
    echo "   Copy missing files to resources/css/"
else
    print_status "3. All theme CSS files present"
fi

if [ ! -d "build" ]; then
    print_warning "4. Create build directory and configure:"
    echo "   mkdir build && cd build && cmake .."
else
    print_status "4. Build directory exists"
fi

echo

# Quick fix suggestions
print_section "Quick Fix Commands"
echo "If you need to start fresh:"
echo
echo "1. Replace CMakeLists.txt:"
echo "   # Use the complete template I provided"
echo
echo "2. Create directory structure:"
echo "   mkdir -p src include resources/css build"
echo
echo "3. Copy CSS files:"
echo "   # Copy the 5 theme CSS files to resources/css/"
echo
echo "4. Configure and build:"
echo "   cd build && cmake .. && make"
echo
echo "5. Run diagnostic again:"
echo "   ./diagnostic.sh"
echo

print_section "Next Steps"
if [ -f "CMakeLists.txt" ] && grep -q "project(" CMakeLists.txt && [ -d "src" ] && [ $css_found -gt 0 ]; then
    print_status "Your project looks mostly ready!"
    echo "Try: cd build && cmake .. && make"
else
    print_warning "Please address the issues above first"
    echo "1. Fix CMakeLists.txt"
    echo "2. Ensure source files are in src/"
    echo "3. Add theme CSS files to resources/css/"
    echo "4. Then run: cd build && cmake .. && make"
fi