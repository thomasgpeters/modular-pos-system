/* ============================================================================
   Dynamic CSS Loader Utility - Complete Header
   
   This utility provides dynamic loading and unloading of CSS files for the
   theme system. It manages CSS dependencies and ensures proper cleanup.
   
   File: include/utils/CSSLoader.hpp
   ============================================================================ */

#ifndef CSSLOADER_H
#define CSSLOADER_H

#include <Wt/WApplication.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <utility>

/**
 * @file CSSLoader.hpp
 * @brief Dynamic CSS loading and management utility
 * 
 * This utility provides functionality to dynamically load and unload CSS files
 * for the theme system, with proper dependency management and cleanup.
 * 
 * @author Restaurant POS Team
 * @version 2.2.0
 */

/**
 * @class CSSLoader
 * @brief Utility for dynamic CSS file management
 * 
 * The CSSLoader manages CSS file loading and unloading for the theme system,
 * tracking loaded files and handling dependencies properly.
 */
class CSSLoader {
public:
    /**
     * @brief CSS load callback type
     */
    using LoadCallback = std::function<void(const std::string& cssPath, bool success)>;
    
    /**
     * @brief Constructs the CSS loader
     * @param app Wt application instance
     */
    explicit CSSLoader(Wt::WApplication* app);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~CSSLoader() = default;
    
    // =================================================================
    // CSS Loading Methods
    // =================================================================
    
    /**
     * @brief Loads a CSS file
     * @param cssPath Path to CSS file
     * @param priority Loading priority (higher = loaded first)
     * @return True if loaded successfully
     */
    bool loadCSS(const std::string& cssPath, int priority = 0);
    
    /**
     * @brief Unloads a CSS file
     * @param cssPath Path to CSS file to unload
     * @return True if unloaded successfully
     */
    bool unloadCSS(const std::string& cssPath);
    
    /**
     * @brief Loads multiple CSS files
     * @param cssPaths Vector of CSS file paths
     * @return Number of files loaded successfully
     */
    int loadMultipleCSS(const std::vector<std::string>& cssPaths);
    
    /**
     * @brief Unloads multiple CSS files
     * @param cssPaths Vector of CSS file paths to unload
     * @return Number of files unloaded successfully
     */
    int unloadMultipleCSS(const std::vector<std::string>& cssPaths);
    
    /**
     * @brief Unloads all loaded CSS files
     */
    void unloadAllCSS();
    
    // =================================================================
    // CSS Management Methods
    // =================================================================
    
    /**
     * @brief Checks if a CSS file is loaded
     * @param cssPath Path to CSS file
     * @return True if file is loaded
     */
    bool isCSSLoaded(const std::string& cssPath) const;
    
    /**
     * @brief Gets list of loaded CSS files
     * @return Vector of loaded CSS file paths
     */
    std::vector<std::string> getLoadedCSS() const;
    
    /**
     * @brief Gets number of loaded CSS files
     * @return Number of loaded files
     */
    size_t getLoadedCount() const;
    
    /**
     * @brief Reloads a CSS file (unload then load)
     * @param cssPath Path to CSS file
     * @return True if reloaded successfully
     */
    bool reloadCSS(const std::string& cssPath);
    
    // =================================================================
    // Dependency Management
    // =================================================================
    
    /**
     * @brief Adds a CSS dependency
     * @param cssPath Main CSS file
     * @param dependency CSS file that must be loaded first
     */
    void addDependency(const std::string& cssPath, const std::string& dependency);
    
    /**
     * @brief Removes a CSS dependency
     * @param cssPath Main CSS file
     * @param dependency Dependency to remove
     */
    void removeDependency(const std::string& cssPath, const std::string& dependency);
    
    /**
     * @brief Gets dependencies for a CSS file
     * @param cssPath CSS file path
     * @return Vector of dependency paths
     */
    std::vector<std::string> getDependencies(const std::string& cssPath) const;
    
    // =================================================================
    // Event Handling
    // =================================================================
    
    /**
     * @brief Sets load callback for notifications
     * @param callback Function to call when CSS is loaded/unloaded
     */
    void setLoadCallback(LoadCallback callback);
    
    /**
     * @brief Removes load callback
     */
    void removeLoadCallback();
    
    // =================================================================
    // Utility Methods
    // =================================================================
    
    /**
     * @brief Validates CSS file path
     * @param cssPath Path to validate
     * @return True if path is valid
     */
    static bool isValidCSSPath(const std::string& cssPath);
    
    /**
     * @brief Gets CSS file extension
     * @param cssPath Path to CSS file
     * @return File extension
     */
    static std::string getCSSExtension(const std::string& cssPath);
    
    /**
     * @brief Normalizes CSS file path
     * @param cssPath Path to normalize
     * @return Normalized path
     */
    static std::string normalizeCSSPath(const std::string& cssPath);

private:
    struct CSSInfo {
        std::string path;
        int priority;
        bool loaded;
        std::vector<std::string> dependencies;
        
        // Default constructor
        CSSInfo() : path(""), priority(0), loaded(false) {}
        
        // Parameterized constructor
        CSSInfo(const std::string& p, int pr) 
            : path(p), priority(pr), loaded(false) {}
        
        // Copy constructor
        CSSInfo(const CSSInfo& other) 
            : path(other.path), priority(other.priority), loaded(other.loaded), dependencies(other.dependencies) {}
        
        // Move constructor
        CSSInfo(CSSInfo&& other) noexcept
            : path(std::move(other.path)), priority(other.priority), loaded(other.loaded), dependencies(std::move(other.dependencies)) {}
        
        // Copy assignment operator
        CSSInfo& operator=(const CSSInfo& other) {
            if (this != &other) {
                path = other.path;
                priority = other.priority;
                loaded = other.loaded;
                dependencies = other.dependencies;
            }
            return *this;
        }
        
        // Move assignment operator
        CSSInfo& operator=(CSSInfo&& other) noexcept {
            if (this != &other) {
                path = std::move(other.path);
                priority = other.priority;
                loaded = other.loaded;
                dependencies = std::move(other.dependencies);
            }
            return *this;
        }
    };
    
    Wt::WApplication* app_;
    std::map<std::string, CSSInfo> cssFiles_;
    LoadCallback loadCallback_;
    
    // Loaded CSS tracking
    std::vector<std::string> loadedCSSFiles_;
    bool frameworkLoaded_;
    
    // Helper methods
    bool loadCSSWithDependencies(const std::string& cssPath);
    bool unloadCSSWithDependents(const std::string& cssPath);
    std::vector<std::string> getDependents(const std::string& cssPath) const;
    void notifyCallback(const std::string& cssPath, bool loaded, bool success);
};

/**
 * @brief CSS Loader utility functions
 */
namespace CSSLoaderUtils {
    
    /**
     * @brief Creates a CSS loader for theme system
     * @param app Wt application instance
     * @return Shared pointer to CSS loader
     */
    std::shared_ptr<CSSLoader> createThemeCSSLoader(Wt::WApplication* app);
    
    /**
     * @brief Creates CSS loader callback for ThemeService
     * @param cssLoader CSS loader instance
     * @return ThemeService CSS loader callback
     */
    std::function<void(const std::string&, bool)> createThemeCallback(
        std::shared_ptr<CSSLoader> cssLoader);
    
    /**
     * @brief Preloads all theme CSS files
     * @param cssLoader CSS loader instance
     * @return Number of files preloaded
     */
    int preloadThemeCSS(std::shared_ptr<CSSLoader> cssLoader);
    
    /**
     * @brief Gets theme CSS file paths
     * @return Map of theme names to CSS file paths
     */
    std::map<std::string, std::string> getThemeCSSPaths();
}

#endif // CSSLOADER_H
