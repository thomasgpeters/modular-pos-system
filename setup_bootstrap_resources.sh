#!/bin/bash

#============================================================================
# setup_bootstrap_resources.sh - Download Bootstrap 5 Resources for Wt
#============================================================================

echo "🚀 Setting up Bootstrap 5 resources for Wt application..."

# Create the required directory structure
PROJECT_ROOT="$(pwd)"
RESOURCES_DIR="${PROJECT_ROOT}/resources"
BOOTSTRAP_DIR="${RESOURCES_DIR}/themes/bootstrap/5"

echo "📁 Creating directory structure..."
mkdir -p "${BOOTSTRAP_DIR}"
mkdir -p "${RESOURCES_DIR}/css"
mkdir -p "${RESOURCES_DIR}/js"

# Bootstrap 5.3.2 URLs (stable version)
BOOTSTRAP_VERSION="5.3.2"
BOOTSTRAP_CSS_URL="https://cdn.jsdelivr.net/npm/bootstrap@${BOOTSTRAP_VERSION}/dist/css/bootstrap.min.css"
BOOTSTRAP_JS_URL="https://cdn.jsdelivr.net/npm/bootstrap@${BOOTSTRAP_VERSION}/dist/js/bootstrap.bundle.min.js"

# Download Bootstrap CSS
echo "⬬ Downloading Bootstrap CSS..."
curl -L "${BOOTSTRAP_CSS_URL}" -o "${BOOTSTRAP_DIR}/bootstrap.min.css"
if [ $? -eq 0 ]; then
    echo "✅ Bootstrap CSS downloaded successfully"
else
    echo "❌ Failed to download Bootstrap CSS"
    exit 1
fi

# Download Bootstrap JavaScript
echo "⬬ Downloading Bootstrap JavaScript..."
curl -L "${BOOTSTRAP_JS_URL}" -o "${BOOTSTRAP_DIR}/bootstrap.bundle.min.js"
if [ $? -eq 0 ]; then
    echo "✅ Bootstrap JavaScript downloaded successfully"
else
    echo "❌ Failed to download Bootstrap JavaScript"
    exit 1
fi

# Create additional required files for Wt Bootstrap theme
echo "📝 Creating Wt-specific Bootstrap theme files..."

# Create bootstrap.css (main theme file)
cat > "${BOOTSTRAP_DIR}/bootstrap.css" << 'EOF'
/* Wt Bootstrap 5 Theme CSS */
@import url('bootstrap.min.css');

/* Additional Wt-specific styles */
.Wt-domRoot {
    height: 100vh;
}

/* POS System specific overrides */
.pos-mode {
    background-color: var(--bs-light);
}

.kitchen-mode {
    background-color: var(--bs-dark);
    color: var(--bs-light);
}

/* Table styling for POS displays */
.table-pos {
    font-size: 0.9rem;
}

.table-pos th {
    background-color: var(--bs-primary);
    color: white;
    border: none;
}

/* Button styling */
.btn-pos-primary {
    background-color: #0d6efd;
    border-color: #0d6efd;
}

.btn-pos-success {
    background-color: #198754;
    border-color: #198754;
}

/* Card styling for containers */
.card-pos {
    border-radius: 0.5rem;
    box-shadow: 0 0.125rem 0.25rem rgba(0, 0, 0, 0.075);
}

/* Responsive utilities */
@media (max-width: 768px) {
    .table-pos {
        font-size: 0.8rem;
    }
    
    .btn {
        padding: 0.25rem 0.5rem;
        font-size: 0.875rem;
    }
}
EOF

# Create theme configuration file
cat > "${BOOTSTRAP_DIR}/theme.xml" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<theme>
    <name>Bootstrap 5</name>
    <version>5.3.2</version>
    <css>
        <file>bootstrap.css</file>
    </css>
    <javascript>
        <file>bootstrap.bundle.min.js</file>
    </javascript>
</theme>
EOF

# Create custom POS system CSS
echo "🎨 Creating custom POS system CSS..."
cat > "${RESOURCES_DIR}/css/pos-system.css" << 'EOF'
/* Restaurant POS System Custom Styles */

/* Layout */
.h-100 {
    height: 100vh !important;
}

.flex-grow-1 {
    flex-grow: 1 !important;
}

/* Header styling */
.pos-header {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    padding: 1rem;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.pos-header .navbar-brand {
    font-weight: bold;
    font-size: 1.5rem;
}

/* Mode selector styling */
.mode-selector {
    display: flex;
    gap: 0.5rem;
}

.mode-selector .btn {
    border-radius: 0.5rem;
    transition: all 0.2s ease;
}

.mode-selector .btn.active {
    transform: translateY(-1px);
    box-shadow: 0 4px 8px rgba(0,0,0,0.2);
}

/* Container styling */
.pos-container,
.kitchen-container {
    padding: 1rem;
    min-height: calc(100vh - 120px);
}

/* Panel styling */
.pos-panel,
.kitchen-panel {
    background: white;
    border-radius: 0.75rem;
    box-shadow: 0 0.5rem 1rem rgba(0, 0, 0, 0.15);
    padding: 1.5rem;
    margin-bottom: 1rem;
}

/* Table styling */
.table-pos {
    margin-bottom: 0;
}

.table-pos th {
    background-color: #495057;
    color: white;
    border: none;
    font-weight: 600;
    text-transform: uppercase;
    font-size: 0.875rem;
    letter-spacing: 0.05em;
}

.table-pos td {
    vertical-align: middle;
    border-bottom: 1px solid #dee2e6;
}

.table-pos tbody tr:hover {
    background-color: #f8f9fa;
}

/* Button styling */
.btn-pos {
    border-radius: 0.5rem;
    font-weight: 500;
    transition: all 0.2s ease;
    border: none;
}

.btn-pos:hover {
    transform: translateY(-1px);
    box-shadow: 0 4px 8px rgba(0,0,0,0.15);
}

.btn-pos-primary {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
}

.btn-pos-success {
    background: linear-gradient(135deg, #56ab2f 0%, #a8e6cf 100%);
    color: white;
}

.btn-pos-warning {
    background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
    color: white;
}

.btn-pos-danger {
    background: linear-gradient(135deg, #ff416c 0%, #ff4b2b 100%);
    color: white;
}

/* Badge styling */
.badge-status {
    font-size: 0.75rem;
    padding: 0.375rem 0.75rem;
    border-radius: 0.375rem;
}

.badge-pending {
    background-color: #6c757d;
}

.badge-preparing {
    background-color: #fd7e14;
}

.badge-ready {
    background-color: #198754;
}

.badge-served {
    background-color: #0d6efd;
}

/* Footer styling */
.pos-footer {
    background-color: #343a40;
    color: white;
    padding: 0.75rem 1rem;
    font-size: 0.875rem;
}

/* Kitchen mode specific styles */
.kitchen-mode {
    background-color: #212529;
    color: #f8f9fa;
}

.kitchen-mode .pos-panel {
    background-color: #343a40;
    color: #f8f9fa;
    border: 1px solid #495057;
}

.kitchen-mode .table-pos {
    color: #f8f9fa;
}

.kitchen-mode .table-pos td {
    border-color: #495057;
}

/* Responsive design */
@media (max-width: 768px) {
    .pos-container,
    .kitchen-container {
        padding: 0.5rem;
    }
    
    .pos-panel,
    .kitchen-panel {
        padding: 1rem;
        margin-bottom: 0.5rem;
    }
    
    .table-pos {
        font-size: 0.8rem;
    }
    
    .btn-pos {
        padding: 0.5rem 1rem;
        font-size: 0.875rem;
    }
}

/* Animation utilities */
@keyframes fadeIn {
    from { opacity: 0; transform: translateY(10px); }
    to { opacity: 1; transform: translateY(0); }
}

.fade-in {
    animation: fadeIn 0.3s ease-out;
}

/* Loading states */
.loading {
    position: relative;
    overflow: hidden;
}

.loading::after {
    content: '';
    position: absolute;
    top: 0;
    left: -100%;
    width: 100%;
    height: 100%;
    background: linear-gradient(90deg, transparent, rgba(255,255,255,0.4), transparent);
    animation: loading 1.5s infinite;
}

@keyframes loading {
    0% { left: -100%; }
    100% { left: 100%; }
}
EOF

# Create responsive CSS
cat > "${RESOURCES_DIR}/css/responsive.css" << 'EOF'
/* Responsive Utilities for POS System */

/* Mobile First Approach */
@media (min-width: 576px) {
    .container-pos {
        max-width: 540px;
    }
}

@media (min-width: 768px) {
    .container-pos {
        max-width: 720px;
    }
}

@media (min-width: 992px) {
    .container-pos {
        max-width: 960px;
    }
}

@media (min-width: 1200px) {
    .container-pos {
        max-width: 1140px;
    }
}

/* Hide/Show utilities */
.d-mobile-none {
    display: none !important;
}

.d-desktop-none {
    display: block !important;
}

@media (min-width: 768px) {
    .d-mobile-none {
        display: block !important;
    }
    
    .d-desktop-none {
        display: none !important;
    }
}

/* Mobile optimizations */
@media (max-width: 767px) {
    .table-responsive-mobile {
        font-size: 0.75rem;
    }
    
    .table-responsive-mobile th,
    .table-responsive-mobile td {
        padding: 0.5rem 0.25rem;
    }
    
    .btn-group-mobile .btn {
        padding: 0.25rem 0.5rem;
        font-size: 0.75rem;
    }
}
EOF

# Create directory structure summary
echo ""
echo "📋 Created directory structure:"
echo "📁 ${RESOURCES_DIR}/"
echo "├── 📁 themes/"
echo "│   └── 📁 bootstrap/"
echo "│       └── 📁 5/"
echo "│           ├── 📄 bootstrap.min.css"
echo "│           ├── 📄 bootstrap.bundle.min.js"
echo "│           ├── 📄 bootstrap.css"
echo "│           └── 📄 theme.xml"
echo "├── 📁 css/"
echo "│   ├── 📄 pos-system.css"
echo "│   └── 📄 responsive.css"
echo "└── 📁 js/"

# Verify files were created
echo ""
echo "🔍 Verifying downloaded files..."
if [ -f "${BOOTSTRAP_DIR}/bootstrap.min.css" ]; then
    CSS_SIZE=$(du -h "${BOOTSTRAP_DIR}/bootstrap.min.css" | cut -f1)
    echo "✅ Bootstrap CSS: ${CSS_SIZE}"
else
    echo "❌ Bootstrap CSS: Missing"
fi

if [ -f "${BOOTSTRAP_DIR}/bootstrap.bundle.min.js" ]; then
    JS_SIZE=$(du -h "${BOOTSTRAP_DIR}/bootstrap.bundle.min.js" | cut -f1)
    echo "✅ Bootstrap JS: ${JS_SIZE}"
else
    echo "❌ Bootstrap JS: Missing"
fi

echo ""
echo "🎉 Bootstrap 5 resources setup complete!"
echo ""
echo "💡 Next steps:"
echo "1. Make sure your CMakeLists.txt includes the resources directory"
echo "2. Update your Wt application to use the correct resource path"
echo "3. Test your application with: ./bin/restaurant-pos-app --resources-dir=./resources"
echo ""
echo "📖 To use in your Wt application:"
echo "   app.useStyleSheet(\"/resources/css/pos-system.css\");"
echo "   app.useStyleSheet(\"/resources/css/responsive.css\");"
