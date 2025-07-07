#!/bin/bash

echo "🔧 IMMEDIATE BOOTSTRAP FIX"
echo "========================="

# Get current directory
CURRENT_DIR="$(pwd)"
echo "📁 Current directory: $CURRENT_DIR"

# Create resources directory structure
echo "📁 Creating directory structure..."
mkdir -p resources/themes/bootstrap/5
mkdir -p resources/css
mkdir -p resources/js

# Download Bootstrap files directly
echo "⬇️ Downloading Bootstrap 5.3.2 files..."

# Download CSS
echo "Downloading bootstrap.min.css..."
curl -L "https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" \
     -o "resources/themes/bootstrap/5/bootstrap.min.css"

if [ $? -eq 0 ]; then
    echo "✅ Bootstrap CSS downloaded"
else
    echo "❌ Failed to download Bootstrap CSS"
    exit 1
fi

# Download JavaScript
echo "Downloading bootstrap.bundle.min.js..."
curl -L "https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/js/bootstrap.bundle.min.js" \
     -o "resources/themes/bootstrap/5/bootstrap.bundle.min.js"

if [ $? -eq 0 ]; then
    echo "✅ Bootstrap JS downloaded"
else
    echo "❌ Failed to download Bootstrap JS"
    exit 1
fi

# Create bootstrap.css wrapper
echo "📝 Creating bootstrap.css wrapper..."
cat > "resources/themes/bootstrap/5/bootstrap.css" << 'EOF'
/* Wt Bootstrap 5 Theme CSS */
@import url('bootstrap.min.css');

/* POS System Overrides */
.Wt-domRoot {
    height: 100vh;
}

body {
    margin: 0;
    padding: 0;
}

.pos-header {
    background: #343a40;
    color: white;
    padding: 1rem;
}

.pos-footer {
    background: #6c757d;
    color: white;
    padding: 0.5rem 1rem;
    font-size: 0.875rem;
}

.h-100 {
    height: 100vh !important;
}

.flex-grow-1 {
    flex-grow: 1 !important;
}
EOF

# Verify downloads
echo ""
echo "🔍 Verifying downloads..."

if [ -f "resources/themes/bootstrap/5/bootstrap.min.css" ]; then
    CSS_SIZE=$(du -h "resources/themes/bootstrap/5/bootstrap.min.css" | cut -f1)
    echo "✅ bootstrap.min.css: $CSS_SIZE"
else
    echo "❌ bootstrap.min.css: MISSING"
fi

if [ -f "resources/themes/bootstrap/5/bootstrap.bundle.min.js" ]; then
    JS_SIZE=$(du -h "resources/themes/bootstrap/5/bootstrap.bundle.min.js" | cut -f1)
    echo "✅ bootstrap.bundle.min.js: $JS_SIZE"
else
    echo "❌ bootstrap.bundle.min.js: MISSING"
fi

echo ""
echo "📂 Directory structure:"
find resources -type f | sort

echo ""
echo "✅ Bootstrap files ready!"
echo ""
echo "🚀 Now run your application with:"
echo "   cd build"
echo "   ./bin/restaurant-pos-app --resources-dir=\"../resources\" --http-port=8080"
