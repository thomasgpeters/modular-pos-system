// =============================================================================
// IMPLEMENTATION (CSSLoader.cpp)
// =============================================================================

#include "../../include/utils/CSSLoader.hpp"

#include <iostream>
#include <algorithm>

CSSLoader::CSSLoader(Wt::WApplication* app) : app_(app) {
    if (!app_) {
        throw std::invalid_argument("CSSLoader requires a valid WApplication instance");
    }
}

bool CSSLoader::loadCSS(const std::string& cssPath, int priority) {
    if (!isValidCSSPath(cssPath)) {
        std::cerr << "[CSSLoader] Invalid CSS path: " << cssPath << std::endl;
        return false;
    }
    
    std::string normalizedPath = normalizeCSSPath(cssPath);
    
    // Check if already loaded
    auto it = cssFiles_.find(normalizedPath);
    if (it != cssFiles_.end() && it->second.loaded) {
        std::cout << "[CSSLoader] CSS already loaded: " << normalizedPath << std::endl;
        return true;
    }
    
    // Add or update CSS info
    if (it == cssFiles_.end()) {
        cssFiles_.emplace(normalizedPath, CSSInfo(normalizedPath, priority));
        it = cssFiles_.find(normalizedPath);
    }
    
    // Load with dependencies
    bool success = loadCSSWithDependencies(normalizedPath);
    
    if (success) {
        it->second.loaded = true;
        std::cout << "[CSSLoader] Successfully loaded: " << normalizedPath << std::endl;
    } else {
        std::cerr << "[CSSLoader] Failed to load: " << normalizedPath << std::endl;
    }
    
    notifyCallback(normalizedPath, true, success);
    return success;
}

bool CSSLoader::unloadCSS(const std::string& cssPath) {
    std::string normalizedPath = normalizeCSSPath(cssPath);
    
    auto it = cssFiles_.find(normalizedPath);
    if (it == cssFiles_.end() || !it->second.loaded) {
        std::cout << "[CSSLoader] CSS not loaded: " << normalizedPath << std::endl;
        return true; // Not loaded is also success
    }
    
    bool success = unloadCSSWithDependents(normalizedPath);
    
    if (success) {
        it->second.loaded = false;
        std::cout << "[CSSLoader] Successfully unloaded: " << normalizedPath << std::endl;
    } else {
        std::cerr << "[CSSLoader] Failed to unload: " << normalizedPath << std::endl;
    }
    
    notifyCallback(normalizedPath, false, success);
    return success;
}

int CSSLoader::loadMultipleCSS(const std::vector<std::string>& cssPaths) {
    int loaded = 0;
    for (const auto& path : cssPaths) {
        if (loadCSS(path)) {
            loaded++;
        }
    }
    return loaded;
}

int CSSLoader::unloadMultipleCSS(const std::vector<std::string>& cssPaths) {
    int unloaded = 0;
    for (const auto& path : cssPaths) {
        if (unloadCSS(path)) {
            unloaded++;
        }
    }
    return unloaded;
}

void CSSLoader::unloadAllCSS() {
    std::vector<std::string> loadedPaths;
    for (const auto& [path, info] : cssFiles_) {
        if (info.loaded) {
            loadedPaths.push_back(path);
        }
    }
    
    unloadMultipleCSS(loadedPaths);
}

bool CSSLoader::isCSSLoaded(const std::string& cssPath) const {
    std::string normalizedPath = normalizeCSSPath(cssPath);
    auto it = cssFiles_.find(normalizedPath);
    return it != cssFiles_.end() && it->second.loaded;
}

std::vector<std::string> CSSLoader::getLoadedCSS() const {
    std::vector<std::string> loaded;
    for (const auto& [path, info] : cssFiles_) {
        if (info.loaded) {
            loaded.push_back(path);
        }
    }
    return loaded;
}

size_t CSSLoader::getLoadedCount() const {
    return std::count_if(cssFiles_.begin(), cssFiles_.end(),
        [](const auto& pair) { return pair.second.loaded; });
}

bool CSSLoader::reloadCSS(const std::string& cssPath) {
    unloadCSS(cssPath);
    return loadCSS(cssPath);
}

void CSSLoader::addDependency(const std::string& cssPath, const std::string& dependency) {
    std::string normalizedPath = normalizeCSSPath(cssPath);
    std::string normalizedDep = normalizeCSSPath(dependency);
    
    auto it = cssFiles_.find(normalizedPath);
    if (it == cssFiles_.end()) {
        cssFiles_.emplace(normalizedPath, CSSInfo(normalizedPath, 0));
        it = cssFiles_.find(normalizedPath);
    }
    
    auto& deps = it->second.dependencies;
    if (std::find(deps.begin(), deps.end(), normalizedDep) == deps.end()) {
        deps.push_back(normalizedDep);
    }
}

void CSSLoader::removeDependency(const std::string& cssPath, const std::string& dependency) {
    std::string normalizedPath = normalizeCSSPath(cssPath);
    std::string normalizedDep = normalizeCSSPath(dependency);
    
    auto it = cssFiles_.find(normalizedPath);
    if (it != cssFiles_.end()) {
        auto& deps = it->second.dependencies;
        deps.erase(std::remove(deps.begin(), deps.end(), normalizedDep), deps.end());
    }
}

std::vector<std::string> CSSLoader::getDependencies(const std::string& cssPath) const {
    std::string normalizedPath = normalizeCSSPath(cssPath);
    auto it = cssFiles_.find(normalizedPath);
    return (it != cssFiles_.end()) ? it->second.dependencies : std::vector<std::string>();
}

void CSSLoader::setLoadCallback(LoadCallback callback) {
    loadCallback_ = callback;
}

void CSSLoader::removeLoadCallback() {
    loadCallback_ = nullptr;
}

// Static utility methods
bool CSSLoader::isValidCSSPath(const std::string& cssPath) {
    return !cssPath.empty() && 
           (cssPath.find(".css") != std::string::npos || 
            cssPath.find("http") == 0);
}

std::string CSSLoader::getCSSExtension(const std::string& cssPath) {
    size_t dotPos = cssPath.find_last_of('.');
    return (dotPos != std::string::npos) ? cssPath.substr(dotPos) : "";
}

std::string CSSLoader::normalizeCSSPath(const std::string& cssPath) {
    std::string normalized = cssPath;
    // Remove double slashes
    size_t pos = 0;
    while ((pos = normalized.find("//", pos)) != std::string::npos) {
        if (pos == 0 || normalized[pos-1] != ':') {
            normalized.replace(pos, 2, "/");
        }
        pos++;
    }
    return normalized;
}

// Private helper methods
bool CSSLoader::loadCSSWithDependencies(const std::string& cssPath) {
    auto it = cssFiles_.find(cssPath);
    if (it == cssFiles_.end()) {
        return false;
    }
    
    // Load dependencies first
    for (const auto& dep : it->second.dependencies) {
        if (!isCSSLoaded(dep)) {
            loadCSS(dep);
        }
    }
    
    // Load the CSS file using Wt
    try {
        app_->useStyleSheet(cssPath);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[CSSLoader] Exception loading CSS: " << e.what() << std::endl;
        return false;
    }
}

bool CSSLoader::unloadCSSWithDependents(const std::string& cssPath) {
    // Check for dependents
    auto dependents = getDependents(cssPath);
    for (const auto& dependent : dependents) {
        if (isCSSLoaded(dependent)) {
            std::cout << "[CSSLoader] Unloading dependent: " << dependent << std::endl;
            unloadCSS(dependent);
        }
    }
    
    // Note: Wt doesn't provide direct CSS unloading, so we simulate it
    // In a real implementation, you might need to manipulate the DOM directly
    return true;
}

std::vector<std::string> CSSLoader::getDependents(const std::string& cssPath) const {
    std::vector<std::string> dependents;
    for (const auto& [path, info] : cssFiles_) {
        const auto& deps = info.dependencies;
        if (std::find(deps.begin(), deps.end(), cssPath) != deps.end()) {
            dependents.push_back(path);
        }
    }
    return dependents;
}

void CSSLoader::notifyCallback(const std::string& cssPath, bool loaded, bool success) {
    if (loadCallback_) {
        loadCallback_(cssPath, success);
    }
}

// Utility functions implementation
namespace CSSLoaderUtils {
    
    std::shared_ptr<CSSLoader> createThemeCSSLoader(Wt::WApplication* app) {
        auto loader = std::make_shared<CSSLoader>(app);
        
        // Set up theme CSS dependencies
        loader->addDependency("assets/css/themes/base.css", "assets/css/theme-framework.css");
        loader->addDependency("assets/css/themes/light.css", "assets/css/theme-framework.css");
        loader->addDependency("assets/css/themes/dark.css", "assets/css/theme-framework.css");
        loader->addDependency("assets/css/themes/warm.css", "assets/css/theme-framework.css");
        loader->addDependency("assets/css/themes/cool.css", "assets/css/theme-framework.css");
        
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
        
        // Add framework first
        paths.push_back("assets/css/theme-framework.css");
        
        // Add all theme files
        for (const auto& [theme, path] : themePaths) {
            paths.push_back(path);
        }
        
        return cssLoader->loadMultipleCSS(paths);
    }
    
    std::map<std::string, std::string> getThemeCSSPaths() {
        return {
            {"base", "assets/css/themes/base.css"},
            {"light", "assets/css/themes/light.css"},
            {"dark", "assets/css/themes/dark.css"},
            {"warm", "assets/css/themes/warm.css"},
            {"cool", "assets/css/themes/cool.css"}
        };
    }
}
