#!/bin/bash

# QUICK CSS FIX - Get your POS system looking professional immediately!

echo "🎨 Fixing CSS Loading Issue..."

cd /Users/thomaspeters/Developer/C-Projects/Restuarant-Management/modular-pos-system

# 1. Replace the application files with CSS-enabled versions
echo "📝 Updating application files with CSS support..."

# You need to replace these files with the artifacts I provided:
# - src/core/RestaurantPOSApp.cpp (Enhanced with CSS loading)
# - include/core/RestaurantPOSApp.hpp (Updated header with CSS methods)

# 2. Rebuild with CSS support
echo "🔨 Rebuilding with CSS support..."
cd build
make -j$(nproc)

# 3. Start server with proper static content serving
echo "🚀 Starting server with enhanced CSS..."
cd bin

# Create a resources directory if it doesn't exist
mkdir -p ../../resources

# Start server with static file serving enabled
echo "=============================================================="
echo "  🎨 Restaurant POS System - CSS Enhanced"
echo "=============================================================="
echo "  URL: http://localhost:8080/pos"
echo "  Features: Bootstrap 3 + Custom CSS + Responsive Design"
echo "  Press Ctrl+C to stop"
echo "=============================================================="

# Start with proper configuration for CSS serving
./restaurant-pos \
  --config=../wt_config.xml \
  --docroot=../../resources \
  --http-address=0.0.0.0 \
  --http-port=8081 \
  --accesslog=access.log

echo "Server stopped."