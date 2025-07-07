// ============================================================================
// CSSLoader Implementation - Dynamic CSS Management
// File: src/utils/CSSLoader.cpp
// ============================================================================

#include "../../include/utils/CSSLoader.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

CSSLoader::CSSLoader(Wt::WApplication* app)
    : app_(app)
    , frameworkLoaded_(false)
{
    if (!app_) {
        throw std::invalid_argument("CSSLoader: Application instance cannot be null");
    }
}

bool CSSLoader::loadCSS(const std::string& cssPath, int priority) {
    if (!isValidCSSPath(cssPath)) {
        std::cerr << "CSSLoader: Invalid CSS path: " << cssPath << std::endl;
        return false;
    }
    
    // Check if already loaded
    if (isCSSLoaded(cssPath)) {
        std::cout << "CSSLoader: CSS already loaded: " << cssPath << std::endl;
        return true;
    }
    
    try {
        // Create CSS info
        CSSInfo cssInfo(cssPath, priority);
        
        // Load dependencies first
        if (loadCSSWithDependencies(cssPath)) {
            // Use Wt's built-in CSS loading
            app_->useStyleSheet(cssPath);
            
            // Mark as loaded
            cssInfo.loaded = true;
            cssFiles_[cssPath] = cssInfo;
            
            std::cout << "CSSLoader: Successfully loaded CSS: " << cssPath << std::endl;
            
            // Notify callback
            notifyCallback(cssPath, true, true);
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "CSSLoader: Failed to load CSS " << cssPath << ": " << e.what() << std::endl;
        notifyCallback(cssPath, true, false);
    }
    
    return false;
}

bool CSSLoader::unloadCSS(const std::string& cssPath) {
    auto it = cssFiles_.find(cssPath);
    if (it == cssFiles_.end() || !it->second.loaded) {
        std::cout << "CSSLoader: CSS not loaded or already unloaded: " << cssPath << std::endl;
        return false;
    }
    
    try {
        // Check for dependents
        auto dependents = getDependents(cssPath);
        if (!dependents.empty()) {
            std::cout << "CSSLoader: Cannot unload CSS with dependents: " << cssPath << std::endl;
            for (const auto& dependent : dependents) {
                std::cout << "  Dependent: " << dependent << std::endl;
            }
            return false;
        }
        
        // Mark as unloaded
        it->second.loaded = false;
        
        // Note: Wt doesn't provide a direct way to unload stylesheets
        // In a production implementation, you might:
        // 1. Keep track of loaded stylesheets and recreate the page
        // 2. Use JavaScript to manipulate the DOM
        // 3. Use a custom CSS loading mechanism
        
        std::cout << "CSSLoader: Marked CSS as unloaded: " << cssPath << std::endl;
        
        // Notify callback
        notifyCallback(cssPath, false, true);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "CSSLoader: Failed to unload CSS " << cssPath << ": " << e.what() << std::endl;
        notifyCallback(cssPath, false, false);
    }
    
    return false;
}

int CSSLoader::loadMultipleCSS(const std::vector<std::string>& cssPaths) {
    int loadedCount = 0;
    
    // Sort by priority (higher priority first)
    std::vector<std::pair<std::string, int>> pathsWithPriority;
    for (const auto& path : cssPaths) {
        pathsWithPriority.emplace_back(path, 0); // Default priority
    }
    
    std::sort(pathsWithPriority.begin(), pathsWithPriority.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second; // Higher priority first
        });
    
    for (const auto& pair : pathsWithPriority) {
        if (loadCSS(pair.first, pair.second)) {
            loadedCount++;
        }
    }
    
    std::cout << "CSSLoader: Loaded " << loadedCount << " out of " << cssPaths.size() << " CSS files" << std::endl;
    return loadedCount;
}

int CSSLoader::unloadMultipleCSS(const std::vector<std::string>& cssPaths) {
    int unloadedCount = 0;
    
    for (const auto& path : cssPaths) {
        if (unloadCSS(path)) {
            unloadedCount++;
        }
    }
    
    std::cout << "CSSLoader: Unloaded " << unloadedCount << " out of " << cssPaths.size() << " CSS files" << std::endl;
    return unloadedCount;
}

void CSSLoader::unloadAllCSS() {
    std::vector<std::string> loadedPaths;
    
    for (const auto& pair : cssFiles_) {
        if (pair.second.loaded) {
            loadedPaths.push_back(pair.first);
        }
    }
    
    // Unload in reverse dependency order
    std::reverse(loadedPaths.begin(), loadedPaths.end());
    
    for (const auto& path : loadedPaths) {
        unloadCSS(path);
    }
    
    cssFiles_.clear();
    std::cout << "CSSLoader: Unloaded all CSS files" << std::endl;
}

bool CSSLoader::isCSSLoaded(const std::string& cssPath) const {
    auto it = cssFiles_.find(cssPath);
    return it != cssFiles_.end() && it->second.loaded;
}

std::vector<std::string> CSSLoader::getLoadedCSS() const {
    std::vector<std::string> loadedPaths;
    
    for (const auto& pair : cssFiles_) {
        if (pair.second.loaded) {
            loadedPaths.push_back(pair.first);
        }
    }
    
    return loadedPaths;
}

size_t CSSLoader::getLoadedCount() const {
    size_t count = 0;
    for (const auto& pair : cssFiles_) {
        if (pair.second.loaded) {
            count++;
        }
    }
    return count;
}

bool CSSLoader::reloadCSS(const std::string& cssPath) {
    std::cout << "CSSLoader: Reloading CSS: " << cssPath << std::endl;
    
    // Get current priority
    int priority = 0;
    auto it = cssFiles_.find(cssPath);
    if (it != cssFiles_.end()) {
        priority = it->second.priority;
    }
    
    // Unload and reload
    unloadCSS(cssPath);
    return loadCSS(cssPath, priority);
}

void CSSLoader::addDependency(const std::string& cssPath, const std::string& dependency) {
    auto it = cssFiles_.find(cssPath);
    if (it != cssFiles_.end()) {
        auto& deps = it->second.dependencies;
        if (std::find(deps.begin(), deps.end(), dependency) == deps.end()) {
            deps.push_back(dependency);
            std::cout << "CSSLoader: Added dependency " << dependency << " to " << cssPath << std::endl;
        }
    } else {
        // Create entry for future use
        CSSInfo cssInfo(cssPath, 0);
        cssInfo.dependencies.push_back(dependency);
        cssFiles_[cssPath] = cssInfo;
        std::cout << "CSSLoader: Created CSS entry with dependency " << dependency << " for " << cssPath << std::endl;
    }
}

void CSSLoader::removeDependency(const std::string& cssPath, const std::string& dependency) {
    auto it = cssFiles_.find(cssPath);
    if (it != cssFiles_.end()) {
        auto& deps = it->second.dependencies;
        auto depIt = std::find(deps.begin(), deps.end(), dependency);
        if (depIt != deps.end()) {
            deps.erase(depIt);
            std::cout << "CSSLoader: Removed dependency " << dependency << " from " << cssPath << std::endl;
        }
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
    
    // Check file extension
    std::string ext = getCSSExtension(cssPath);
    if (ext != ".css") {
        return false;
    }
    
    // Check for invalid characters (basic validation)
    if (cssPath.find("..") != std::string::npos) {
        return false; // Prevent directory traversal
    }
    
    return true;
}

std::string CSSLoader::getCSSExtension(const std::string& cssPath) {
    size_t dotPos = cssPath.find_last_of('.');
    if (dotPos != std::string::npos) {
        return cssPath.substr(dotPos);
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
    
    return normalized;
}

bool CSSLoader::loadCSSWithDependencies(const std::string& cssPath) {
    auto deps = getDependencies(cssPath);
    
    for (const auto& dep : deps) {
        if (!isCSSLoaded(dep)) {
            if (!loadCSS(dep)) {
                std::cerr << "CSSLoader: Failed to load dependency " << dep << " for " << cssPath << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

bool CSSLoader::unloadCSSWithDependents(const std::string& cssPath) {
    auto dependents = getDependents(cssPath);
    
    if (!dependents.empty()) {
        std::cout << "CSSLoader: Unloading dependents of " << cssPath << std::endl;
        for (const auto& dependent : dependents) {
            unloadCSS(dependent);
        }
    }
    
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
            std::cerr << "CSSLoader: Error in load callback: " << e.what() << std::endl;
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
    loader->addDependency("theme-light.css", "theme-framework.css");
    loader->addDependency("theme-dark.css", "theme-framework.css");
    loader->addDependency("theme-warm.css", "theme-framework.css");
    loader->addDependency("theme-cool.css", "theme-framework.css");
    
    std::cout << "CSSLoaderUtils: Created theme CSS loader with dependencies" << std::endl;
    return loader;
}

std::function<void(const std::string&, bool)> createThemeCallback(
    std::shared_ptr<CSSLoader> cssLoader) {
    
    return [cssLoader](const std::string& cssPath, bool load) {
        if (load) {
            cssLoader->loadCSS(cssPath);
        } else {
            cssLoader->unloadCSS(cssPath);
        }
    };
}

int preloadThemeCSS(std::shared_ptr<CSSLoader> cssLoader) {
    auto themePaths = getThemeCSSPaths();
    std::vector<std::string> paths;
    
    for (const auto& pair : themePaths) {
        paths.push_back(pair.second);
    }
    
    int loaded = cssLoader->loadMultipleCSS(paths);
    std::cout << "CSSLoaderUtils: Preloaded " << loaded << " theme CSS files" << std::endl;
    return loaded;
}

std::map<std::string, std::string> getThemeCSSPaths() {
    std::map<std::string, std::string> paths;
    
    // Determine base path
    std::string basePath = "css/"; // Default web-accessible path
    
    // Check for external resources
    std::ifstream externalCheck("build/resources/css/theme-framework.css");
    if (externalCheck.good()) {
        basePath = "build/resources/css/";
    } else {
        // Check for internal resources
        std::ifstream internalCheck("resources/css/theme-framework.css");
        if (internalCheck.good()) {
            basePath = "resources/css/";
        }
    }
    
    // Build theme paths
    paths["framework"] = basePath + "theme-framework.css";
    paths["light"] = basePath + "theme-light.css";
    paths["dark"] = basePath + "theme-dark.css";
    paths["warm"] = basePath + "theme-warm.css";
    paths["cool"] = basePath + "theme-cool.css";
    
    return paths;
}

} // namespace CSSLoaderUtils
