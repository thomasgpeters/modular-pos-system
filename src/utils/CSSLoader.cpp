// ============================================================================
// Enhanced CSSLoader Implementation - src/utils/CSSLoader.cpp
// ============================================================================

#include "../../include/utils/CSSLoader.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <Wt/WApplication.h>

// Helper function to escape strings for use in JavaScript double-quoted strings
static std::string escapeForJSDoubleQuote(const std::string& input) {
    std::ostringstream escaped;
    for (char c : input) {
        switch (c) {
            case '\\': escaped << "\\\\"; break;
            case '"':  escaped << "\\\""; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default:   escaped << c; break;
        }
    }
    return escaped.str();
}

CSSLoader::CSSLoader(Wt::WApplication* app)
    : app_(app)
    , frameworkLoaded_(false)
{
    if (!app_) {
        throw std::invalid_argument("CSSLoader requires a valid WApplication instance");
    }
    
    std::cout << "[CSSLoader] Initialized" << std::endl;
}

bool CSSLoader::loadCSS(const std::string& cssPath, int priority) {
    if (!app_ || cssPath.empty()) {
        std::cerr << "[CSSLoader] Invalid parameters for loadCSS" << std::endl;
        return false;
    }
    
    // Normalize the path
    std::string normalizedPath = normalizeCSSPath(cssPath);
    
    // Check if already loaded
    if (isCSSLoaded(normalizedPath)) {
        std::cout << "[CSSLoader] CSS already loaded: " << normalizedPath << std::endl;
        return true;
    }
    
    // Validate the CSS path
    if (!isValidCSSPath(normalizedPath)) {
        std::cerr << "[CSSLoader] Invalid CSS path: " << normalizedPath << std::endl;
        notifyCallback(normalizedPath, true, false);
        return false;
    }
    
    try {
        // Load dependencies first
        auto it = cssFiles_.find(normalizedPath);
        if (it != cssFiles_.end()) {
            for (const auto& dependency : it->second.dependencies) {
                if (!loadCSS(dependency, priority + 1)) {
                    std::cerr << "[CSSLoader] Failed to load dependency: " << dependency << std::endl;
                    return false;
                }
            }
        }
        
        // Load the CSS file using Wt's built-in method
        app_->useStyleSheet(normalizedPath);
        
        // Track the loaded file
        loadedCSSFiles_.push_back(normalizedPath);
        
        // Update CSS info if it exists
        if (it != cssFiles_.end()) {
            it->second.loaded = true;
        } else {
            // Create new entry
            CSSInfo info(normalizedPath, priority);
            info.loaded = true;
            cssFiles_[normalizedPath] = std::move(info);
        }
        
        std::cout << "[CSSLoader] Successfully loaded CSS: " << normalizedPath << std::endl;
        notifyCallback(normalizedPath, true, true);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[CSSLoader] Exception loading CSS " << normalizedPath << ": " << e.what() << std::endl;
        notifyCallback(normalizedPath, true, false);
        return false;
    }
}

bool CSSLoader::unloadCSS(const std::string& cssPath) {
    if (!app_ || cssPath.empty()) {
        return false;
    }
    
    std::string normalizedPath = normalizeCSSPath(cssPath);
    
    // Check if the CSS is loaded
    if (!isCSSLoaded(normalizedPath)) {
        std::cout << "[CSSLoader] CSS not loaded: " << normalizedPath << std::endl;
        return true; // Not an error if it's not loaded
    }
    
    try {
        // Find and remove from loaded files
        auto it = std::find(loadedCSSFiles_.begin(), loadedCSSFiles_.end(), normalizedPath);
        if (it != loadedCSSFiles_.end()) {
            loadedCSSFiles_.erase(it);
        }
        
        // Update CSS info
        auto cssIt = cssFiles_.find(normalizedPath);
        if (cssIt != cssFiles_.end()) {
            cssIt->second.loaded = false;
        }
        
        // Note: Wt doesn't provide a direct way to unload stylesheets
        // We use JavaScript to remove the stylesheet link
        // FIXED: Properly escape the path for JavaScript
        std::string escapedPath = escapeForJSDoubleQuote(normalizedPath);
        std::string script =
            "var sheets = document.querySelectorAll('link[href*=\"" + escapedPath + "\"]');"
            "for (var i = 0; i < sheets.length; i++) {"
            "  if (sheets[i].parentNode) {"
            "    sheets[i].parentNode.removeChild(sheets[i]);"
            "  }"
            "}";
        
        app_->doJavaScript(script);
        
        std::cout << "[CSSLoader] Unloaded CSS: " << normalizedPath << std::endl;
        notifyCallback(normalizedPath, false, true);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[CSSLoader] Exception unloading CSS " << normalizedPath << ": " << e.what() << std::endl;
        notifyCallback(normalizedPath, false, false);
        return false;
    }
}

int CSSLoader::loadMultipleCSS(const std::vector<std::string>& cssPaths) {
    int successCount = 0;
    
    // Sort by priority if CSS info exists
    std::vector<std::pair<std::string, int>> pathsWithPriority;
    for (const auto& path : cssPaths) {
        int priority = 0;
        auto it = cssFiles_.find(path);
        if (it != cssFiles_.end()) {
            priority = it->second.priority;
        }
        pathsWithPriority.emplace_back(path, priority);
    }
    
    // Sort by priority (higher priority loads first)
    std::sort(pathsWithPriority.begin(), pathsWithPriority.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Load in priority order
    for (const auto& pathPair : pathsWithPriority) {
        if (loadCSS(pathPair.first, pathPair.second)) {
            successCount++;
        }
    }
    
    std::cout << "[CSSLoader] Loaded " << successCount << "/" << cssPaths.size() << " CSS files" << std::endl;
    return successCount;
}

int CSSLoader::unloadMultipleCSS(const std::vector<std::string>& cssPaths) {
    int successCount = 0;
    
    // Unload in reverse order (last loaded, first unloaded)
    for (auto it = cssPaths.rbegin(); it != cssPaths.rend(); ++it) {
        if (unloadCSS(*it)) {
            successCount++;
        }
    }
    
    std::cout << "[CSSLoader] Unloaded " << successCount << "/" << cssPaths.size() << " CSS files" << std::endl;
    return successCount;
}

void CSSLoader::unloadAllCSS() {
    std::cout << "[CSSLoader] Unloading all CSS files..." << std::endl;
    
    // Create a copy of loaded files to iterate over
    auto filesToUnload = loadedCSSFiles_;
    
    // Unload in reverse order
    for (auto it = filesToUnload.rbegin(); it != filesToUnload.rend(); ++it) {
        unloadCSS(*it);
    }
    
    // Clear tracking
    loadedCSSFiles_.clear();
    frameworkLoaded_ = false;
    
    // Reset all CSS info loaded status
    for (auto& pair : cssFiles_) {
        pair.second.loaded = false;
    }
    
    std::cout << "[CSSLoader] All CSS files unloaded" << std::endl;
}

bool CSSLoader::isCSSLoaded(const std::string& cssPath) const {
    std::string normalizedPath = normalizeCSSPath(cssPath);
    return std::find(loadedCSSFiles_.begin(), loadedCSSFiles_.end(), normalizedPath) != loadedCSSFiles_.end();
}

std::vector<std::string> CSSLoader::getLoadedCSS() const {
    return loadedCSSFiles_;
}

size_t CSSLoader::getLoadedCount() const {
    return loadedCSSFiles_.size();
}

bool CSSLoader::reloadCSS(const std::string& cssPath) {
    std::cout << "[CSSLoader] Reloading CSS: " << cssPath << std::endl;
    
    // Get priority before unloading
    int priority = 0;
    auto it = cssFiles_.find(cssPath);
    if (it != cssFiles_.end()) {
        priority = it->second.priority;
    }
    
    // Unload then reload
    unloadCSS(cssPath);
    return loadCSS(cssPath, priority);
}

void CSSLoader::addDependency(const std::string& cssPath, const std::string& dependency) {
    auto& cssInfo = cssFiles_[cssPath];
    cssInfo.path = cssPath;
    
    // Add dependency if not already present
    if (std::find(cssInfo.dependencies.begin(), cssInfo.dependencies.end(), dependency) == cssInfo.dependencies.end()) {
        cssInfo.dependencies.push_back(dependency);
        std::cout << "[CSSLoader] Added dependency " << dependency << " to " << cssPath << std::endl;
    }
}

void CSSLoader::removeDependency(const std::string& cssPath, const std::string& dependency) {
    auto it = cssFiles_.find(cssPath);
    if (it != cssFiles_.end()) {
        auto& deps = it->second.dependencies;
        deps.erase(std::remove(deps.begin(), deps.end(), dependency), deps.end());
        std::cout << "[CSSLoader] Removed dependency " << dependency << " from " << cssPath << std::endl;
    }
}

std::vector<std::string> CSSLoader::getDependencies(const std::string& cssPath) const {
    auto it = cssFiles_.find(cssPath);
    if (it != cssFiles_.end()) {
        return it->second.dependencies;
    }
    return {};
}

void CSSLoader::setLoadCallback(LoadCallback callback) {
    loadCallback_ = callback;
}

void CSSLoader::removeLoadCallback() {
    loadCallback_ = nullptr;
}

bool CSSLoader::isValidCSSPath(const std::string& cssPath) {
    if (cssPath.empty()) {
        return false;
    }
    
    // Check for valid CSS extension
    std::string ext = getCSSExtension(cssPath);
    if (ext != ".css") {
        return false;
    }
    
    // Basic path validation - no dangerous characters
    if (cssPath.find("..") != std::string::npos ||
        cssPath.find("<") != std::string::npos ||
        cssPath.find(">") != std::string::npos) {
        return false;
    }
    
    return true;
}

std::string CSSLoader::getCSSExtension(const std::string& cssPath) {
    size_t lastDot = cssPath.find_last_of('.');
    if (lastDot != std::string::npos) {
        return cssPath.substr(lastDot);
    }
    return "";
}

std::string CSSLoader::normalizeCSSPath(const std::string& cssPath) {
    std::string normalized = cssPath;
    
    // Replace backslashes with forward slashes
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // Remove duplicate slashes
    size_t pos = 0;
    while ((pos = normalized.find("//", pos)) != std::string::npos) {
        normalized.replace(pos, 2, "/");
        pos += 1;
    }
    
    // Remove leading "./"
    if (normalized.substr(0, 2) == "./") {
        normalized = normalized.substr(2);
    }
    
    return normalized;
}

bool CSSLoader::loadCSSWithDependencies(const std::string& cssPath) {
    // Get dependencies
    auto dependencies = getDependencies(cssPath);
    
    // Load dependencies first
    for (const auto& dep : dependencies) {
        if (!loadCSS(dep)) {
            std::cerr << "[CSSLoader] Failed to load dependency: " << dep << std::endl;
            return false;
        }
    }
    
    // Load the main CSS file
    return loadCSS(cssPath);
}

bool CSSLoader::unloadCSSWithDependents(const std::string& cssPath) {
    // Find all files that depend on this CSS
    auto dependents = getDependents(cssPath);
    
    // Unload dependents first
    for (const auto& dependent : dependents) {
        unloadCSS(dependent);
    }
    
    // Unload the main CSS file
    return unloadCSS(cssPath);
}

std::vector<std::string> CSSLoader::getDependents(const std::string& cssPath) const {
    std::vector<std::string> dependents;
    
    for (const auto& pair : cssFiles_) {
        const auto& deps = pair.second.dependencies;
        if (std::find(deps.begin(), deps.end(), cssPath) != deps.end()) {
            dependents.push_back(pair.first);
        }
    }
    
    return dependents;
}

void CSSLoader::notifyCallback(const std::string& cssPath, bool loaded, bool success) {
    if (loadCallback_) {
        try {
            loadCallback_(cssPath, success);
        } catch (const std::exception& e) {
            std::cerr << "[CSSLoader] Exception in load callback: " << e.what() << std::endl;
        }
    }
}

// ============================================================================
// CSSLoaderUtils Implementation
// ============================================================================

namespace CSSLoaderUtils {

std::shared_ptr<CSSLoader> createThemeCSSLoader(Wt::WApplication* app) {
    auto loader = std::make_shared<CSSLoader>(app);
    
    // Set up theme CSS dependencies
    loader->addDependency("assets/css/themes/light-theme.css", "assets/css/theme-framework.css");
    loader->addDependency("assets/css/themes/dark-theme.css", "assets/css/theme-framework.css");
    loader->addDependency("assets/css/themes/warm-theme.css", "assets/css/theme-framework.css");
    loader->addDependency("assets/css/themes/cool-theme.css", "assets/css/theme-framework.css");
    
    std::cout << "[CSSLoaderUtils] Theme CSS loader created with dependencies" << std::endl;
    return loader;
}

std::function<void(const std::string&, bool)> createThemeCallback(std::shared_ptr<CSSLoader> cssLoader) {
    return [cssLoader](const std::string& cssPath, bool load) {
        if (load) {
            cssLoader->loadCSS(cssPath);
        } else {
            cssLoader->unloadCSS(cssPath);
        }
    };
}

int preloadThemeCSS(std::shared_ptr<CSSLoader> cssLoader) {
    std::vector<std::string> themeFiles = {
        "assets/css/theme-framework.css",
        "assets/css/themes/light-theme.css",
        "assets/css/themes/dark-theme.css",
        "assets/css/themes/warm-theme.css", 
        "assets/css/themes/cool-theme.css"
    };
    
    return cssLoader->loadMultipleCSS(themeFiles);
}

std::map<std::string, std::string> getThemeCSSPaths() {
    return {
        {"base", "assets/css/theme-framework.css"},
        {"light", "assets/css/themes/light-theme.css"},
        {"dark", "assets/css/themes/dark-theme.css"},
        {"warm", "assets/css/themes/warm-theme.css"},
        {"cool", "assets/css/themes/cool-theme.css"}
    };
}

} // namespace CSSLoaderUtils
