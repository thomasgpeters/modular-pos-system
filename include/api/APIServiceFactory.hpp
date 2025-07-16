//============================================================================
// include/api/APIServiceFactory.hpp - Factory for Creating API Services
//============================================================================

#ifndef APISERVICEFACTORY_H
#define APISERVICEFACTORY_H

#include "../services/POSService.hpp"
#include "../services/EnhancedPOSService.hpp"
#include "../core/ConfigurationManager.hpp"
#include "../events/EventManager.hpp"
#include <memory>
#include <iostream>

/**
 * @class APIServiceFactory
 * @brief Factory for creating appropriate POS service based on configuration
 * 
 * This factory determines whether to use the original POSService (local data)
 * or the EnhancedPOSService (API-backed) based on configuration settings.
 */
class APIServiceFactory {
public:
    /**
     * @brief Service type enumeration
     */
    enum ServiceType {
        LOCAL_SERVICE,      ///< Use original POSService with local data
        API_SERVICE         ///< Use EnhancedPOSService with API backend
    };
    
    /**
     * @brief Creates appropriate POS service based on configuration
     * @param eventManager Event manager instance
     * @param configManager Configuration manager instance
     * @return Shared pointer to POS service (either type)
     */
    static std::shared_ptr<POSService> createPOSService(
        std::shared_ptr<EventManager> eventManager,
        std::shared_ptr<ConfigurationManager> configManager) {
        
        // Check configuration to determine service type
        bool useAPI = configManager->getValue<bool>("api.enabled", false);
        
        if (useAPI) {
            std::cout << "[APIServiceFactory] Creating API-backed POS service" << std::endl;
            
            // Create enhanced service and cast to base type
            auto enhancedService = createEnhancedPOSService(eventManager, configManager);
            if (enhancedService) {
                // Cast to base class pointer
                return std::static_pointer_cast<POSService>(enhancedService);
            } else {
                // Fallback to local service if API service creation fails
                std::cout << "[APIServiceFactory] API service creation failed, falling back to local service" << std::endl;
                return createLocalPOSService(eventManager);
            }
        } else {
            std::cout << "[APIServiceFactory] Creating local POS service" << std::endl;
            return createLocalPOSService(eventManager);
        }
    }
    
    /**
     * @brief Creates enhanced (API-backed) POS service
     * @param eventManager Event manager instance
     * @param configManager Configuration manager instance
     * @return Shared pointer to enhanced POS service
     */
    static std::shared_ptr<EnhancedPOSService> createEnhancedPOSService(
        std::shared_ptr<EventManager> eventManager,
        std::shared_ptr<ConfigurationManager> configManager) {
        
        // Validate API configuration first
        if (!validateAPIConfiguration(configManager)) {
            std::cerr << "[APIServiceFactory] Invalid API configuration" << std::endl;
            return nullptr;
        }
        
        // Build configuration from ConfigurationManager
        EnhancedPOSService::ServiceConfig config;
        config.apiBaseUrl = configManager->getValue<std::string>("api.base_url", "http://localhost:5656/api");
        config.authToken = configManager->getValue<std::string>("api.auth_token", "");
        config.apiTimeout = configManager->getValue<int>("api.timeout", 30);
        config.enableCaching = configManager->getValue<bool>("api.enable_caching", true);
        config.debugMode = configManager->getValue<bool>("api.debug_mode", false);
        
        try {
            auto service = std::make_shared<EnhancedPOSService>(eventManager, config);
            
            if (!service->initialize()) {
                std::cerr << "[APIServiceFactory] Failed to initialize enhanced POS service" << std::endl;
                return nullptr;
            }
            
            std::cout << "[APIServiceFactory] Enhanced POS service created successfully" << std::endl;
            return service;
            
        } catch (const std::exception& e) {
            std::cerr << "[APIServiceFactory] Exception creating enhanced POS service: " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    /**
     * @brief Creates local (original) POS service
     * @param eventManager Event manager instance
     * @return Shared pointer to local POS service
     */
    static std::shared_ptr<POSService> createLocalPOSService(
        std::shared_ptr<EventManager> eventManager) {
        
        try {
            auto service = std::make_shared<POSService>(eventManager);
            std::cout << "[APIServiceFactory] Local POS service created successfully" << std::endl;
            return service;
            
        } catch (const std::exception& e) {
            std::cerr << "[APIServiceFactory] Exception creating local POS service: " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    /**
     * @brief Validates API configuration
     * @param configManager Configuration manager instance
     * @return True if API configuration is valid
     */
    static bool validateAPIConfiguration(std::shared_ptr<ConfigurationManager> configManager) {
        if (!configManager) {
            std::cerr << "[APIServiceFactory] Configuration manager is null" << std::endl;
            return false;
        }
        
        std::string baseUrl = configManager->getValue<std::string>("api.base_url", "");
        
        if (baseUrl.empty()) {
            std::cerr << "[APIServiceFactory] API base URL not configured" << std::endl;
            return false;
        }
        
        // Validate URL format (basic check)
        if (baseUrl.find("http://") != 0 && baseUrl.find("https://") != 0) {
            std::cerr << "[APIServiceFactory] Invalid API base URL format: " << baseUrl << std::endl;
            return false;
        }
        
        int timeout = configManager->getValue<int>("api.timeout", 30);
        if (timeout <= 0 || timeout > 300) { // 5 minute max timeout
            std::cerr << "[APIServiceFactory] Invalid API timeout value: " << timeout << std::endl;
            return false;
        }
        
        std::cout << "[APIServiceFactory] API configuration validation passed" << std::endl;
        return true;
    }
    
    /**
     * @brief Gets the service type that would be created with current configuration
     * @param configManager Configuration manager instance
     * @return Service type that would be created
     */
    static ServiceType getServiceType(std::shared_ptr<ConfigurationManager> configManager) {
        bool useAPI = configManager->getValue<bool>("api.enabled", false);
        return useAPI ? API_SERVICE : LOCAL_SERVICE;
    }
    
    /**
     * @brief Creates service with explicit type override
     * @param serviceType Explicit service type to create
     * @param eventManager Event manager instance
     * @param configManager Configuration manager instance
     * @return Shared pointer to POS service
     */
    static std::shared_ptr<POSService> createPOSService(
        ServiceType serviceType,
        std::shared_ptr<EventManager> eventManager,
        std::shared_ptr<ConfigurationManager> configManager) {
        
        switch (serviceType) {
            case API_SERVICE: {
                auto enhancedService = createEnhancedPOSService(eventManager, configManager);
                if (enhancedService) {
                    return std::static_pointer_cast<POSService>(enhancedService);
                }
                // Fallback to local service
                return createLocalPOSService(eventManager);
            }
            case LOCAL_SERVICE:
            default:
                return createLocalPOSService(eventManager);
        }
    }
};

#endif // APISERVICEFACTORY_H
