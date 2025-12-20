#!/bin/bash

# Restaurant POS System - Build and Run Script
# This script builds and runs the modular POS application

set -e  # Exit on any error

echo "=============================================================="
echo "  Restaurant POS System - Modular Architecture v3.0.0-beta.1"
echo "=============================================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if Wt is installed
print_status "Checking for Wt library..."
if ! pkg-config --exists wt; then
    print_error "Wt library not found!"
    echo "Please install Wt library first:"
    echo "  Ubuntu/Debian: sudo apt-get install libwt-dev libwthttp-dev"
    echo "  Fedora/RHEL:   sudo dnf install wt-devel"
    echo "  macOS:         brew install wt"
    exit 1
fi

WT_VERSION=$(pkg-config --modversion wt)
print_success "Wt library found (version: $WT_VERSION)"

# Create directory structure
print_status "Creating directory structure..."
mkdir -p src/{core,services,events,utils}
mkdir -p include/{core,services,events,utils}
mkdir -p build
mkdir -p resources

# Check if source files exist
if [ ! -f "src/main.cpp" ]; then
    print_error "Source files not found! Please ensure all source files are in place."
    exit 1
fi

# Build the application
print_status "Building the application..."
cd build

# Run CMake
print_status "Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compile
print_status "Compiling..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

if [ $? -eq 0 ]; then
    print_success "Build completed successfully!"
else
    print_error "Build failed!"
    exit 1
fi

# Check if executable exists
if [ ! -f "bin/restaurant-pos" ]; then
    print_error "Executable not found!"
    exit 1
fi

print_success "Executable created: bin/restaurant-pos"

# Run the application
print_status "Starting the Restaurant POS System..."
echo ""
echo "=============================================================="
echo "  Server Information"
echo "=============================================================="
echo "  URL: http://localhost:8080/pos"
echo "  Config: wt_config.xml"
echo "  Log: access.log"
echo "=============================================================="
echo ""
print_status "Press Ctrl+C to stop the server"
echo ""

# Start the server
cd bin
./restaurant-pos --config=../wt_config.xml --docroot=../../resources --http-address=0.0.0.0 --http-port=8080

print_status "Server stopped."
