//============================================================================
// include/services/LLMQueryService.hpp - LLM Query Service for Business Demographics
//============================================================================

#ifndef LLMQUERYSERVICE_H
#define LLMQUERYSERVICE_H

#include "../events/EventManager.hpp"
#include "../utils/Logging.hpp"
#include "../api/APIClient.hpp"

#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Value.h>

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <future>

/**
 * @file LLMQueryService.hpp
 * @brief Service for querying LLM APIs for local business demographic information
 *
 * This service provides methods to query AI language models (like Claude, GPT, etc.)
 * to retrieve demographic information about local businesses based on geolocation.
 *
 * @author Restaurant POS Team
 * @version 1.0.0
 */

/**
 * @struct GeoLocation
 * @brief Represents a geographic location with latitude and longitude
 */
struct GeoLocation {
    double latitude;
    double longitude;
    std::string address;      // Optional: human-readable address
    std::string city;         // Optional: city name
    std::string state;        // Optional: state/province
    std::string country;      // Optional: country code (e.g., "US")
    double radiusKm;          // Search radius in kilometers

    GeoLocation() : latitude(0.0), longitude(0.0), radiusKm(5.0) {}

    GeoLocation(double lat, double lon, double radius = 5.0)
        : latitude(lat), longitude(lon), radiusKm(radius) {}

    /**
     * @brief Converts to JSON object for API requests
     */
    Wt::Json::Object toJson() const;

    /**
     * @brief Creates from JSON object
     */
    static GeoLocation fromJson(const Wt::Json::Object& json);

    /**
     * @brief Returns a formatted string representation
     */
    std::string toString() const;

    /**
     * @brief Validates the geolocation coordinates
     */
    bool isValid() const;
};

/**
 * @struct BusinessInfo
 * @brief Information about a local business
 */
struct BusinessInfo {
    std::string name;
    std::string type;            // e.g., "restaurant", "retail", "service"
    std::string category;        // e.g., "Italian Restaurant", "Coffee Shop"
    std::string address;
    GeoLocation location;
    std::string phone;
    std::string website;
    double rating;               // Average rating (0-5)
    int reviewCount;
    std::string priceRange;      // e.g., "$", "$$", "$$$"
    std::vector<std::string> cuisineTypes;  // For restaurants
    bool isOpen;
    std::string operatingHours;

    BusinessInfo() : rating(0.0), reviewCount(0), isOpen(false) {}

    Wt::Json::Object toJson() const;
    static BusinessInfo fromJson(const Wt::Json::Object& json);
};

/**
 * @struct BusinessDemographics
 * @brief Demographic analysis of businesses in an area
 */
struct BusinessDemographics {
    GeoLocation searchLocation;
    int totalBusinesses;
    int totalRestaurants;
    int totalRetail;
    int totalServices;

    // Market analysis
    std::string marketSaturation;     // "low", "medium", "high"
    std::string dominantCuisine;      // Most common cuisine type
    std::string averagePriceRange;    // Average price range in area
    double averageRating;             // Average business rating

    // Competition analysis
    std::vector<BusinessInfo> competitors;
    std::vector<std::string> underservedCategories;
    std::vector<std::string> oversaturatedCategories;

    // Foot traffic estimates
    std::string peakHours;
    std::string peakDays;
    std::string trafficLevel;         // "low", "medium", "high"

    // Demographics
    std::string primaryDemographic;   // e.g., "young professionals", "families"
    std::string incomeLevel;          // "low", "middle", "upper-middle", "high"
    int estimatedPopulation;

    // AI-generated insights
    std::string summary;
    std::vector<std::string> opportunities;
    std::vector<std::string> challenges;
    std::vector<std::string> recommendations;

    BusinessDemographics()
        : totalBusinesses(0), totalRestaurants(0), totalRetail(0),
          totalServices(0), averageRating(0.0), estimatedPopulation(0) {}

    Wt::Json::Object toJson() const;
    static BusinessDemographics fromJson(const Wt::Json::Object& json);
};

/**
 * @struct LLMQueryResult
 * @brief Result from an LLM query
 */
struct LLMQueryResult {
    bool success;
    std::string errorMessage;
    BusinessDemographics demographics;
    std::string rawResponse;          // Raw LLM response for debugging
    int tokensUsed;
    double queryTimeMs;
    std::string model;                // Model used for the query

    LLMQueryResult() : success(false), tokensUsed(0), queryTimeMs(0.0) {}
};

/**
 * @class LLMQueryService
 * @brief Service for querying LLMs about local business demographics
 *
 * Provides async and sync methods to query various LLM providers for
 * demographic information about businesses near a given geolocation.
 */
class LLMQueryService {
public:
    /**
     * @brief LLM provider types
     */
    enum class LLMProvider {
        ANTHROPIC,    // Claude
        OPENAI,       // GPT-4, etc.
        GOOGLE,       // Gemini
        LOCAL         // Local/self-hosted models
    };

    /**
     * @brief Callback type for async queries
     */
    using QueryCallback = std::function<void(const LLMQueryResult&)>;

    /**
     * @brief Constructs the LLM query service
     * @param eventManager Event manager for notifications
     */
    explicit LLMQueryService(std::shared_ptr<EventManager> eventManager);

    /**
     * @brief Destructor
     */
    virtual ~LLMQueryService();

    // =================================================================
    // Configuration
    // =================================================================

    /**
     * @brief Initializes the service with configuration
     * @param apiKey API key for the LLM provider
     * @param provider LLM provider to use
     * @param baseUrl Optional custom base URL for API
     * @return True if initialization successful
     */
    bool initialize(const std::string& apiKey,
                   LLMProvider provider = LLMProvider::ANTHROPIC,
                   const std::string& baseUrl = "");

    /**
     * @brief Sets the LLM provider
     * @param provider Provider to use
     */
    void setProvider(LLMProvider provider);

    /**
     * @brief Gets the current provider
     * @return Current LLM provider
     */
    LLMProvider getProvider() const;

    /**
     * @brief Sets the model to use
     * @param model Model identifier (e.g., "claude-3-sonnet-20240229")
     */
    void setModel(const std::string& model);

    /**
     * @brief Gets the current model
     * @return Current model identifier
     */
    std::string getModel() const;

    /**
     * @brief Sets request timeout
     * @param timeoutSeconds Timeout in seconds
     */
    void setTimeout(int timeoutSeconds);

    /**
     * @brief Enables/disables debug mode
     * @param enabled True to enable debug logging
     */
    void setDebugMode(bool enabled);

    /**
     * @brief Checks if the service is initialized and ready
     * @return True if ready to make queries
     */
    bool isReady() const;

    // =================================================================
    // Query Methods - Async
    // =================================================================

    /**
     * @brief Queries for business demographics at a location (async)
     * @param location Geographic location to query
     * @param callback Callback function for results
     */
    void queryBusinessDemographics(const GeoLocation& location,
                                   QueryCallback callback);

    /**
     * @brief Queries for local competitors (async)
     * @param location Geographic location
     * @param businessType Type of business to find competitors for
     * @param callback Callback function for results
     */
    void queryLocalCompetitors(const GeoLocation& location,
                               const std::string& businessType,
                               QueryCallback callback);

    /**
     * @brief Queries for market opportunities (async)
     * @param location Geographic location
     * @param callback Callback function for results
     */
    void queryMarketOpportunities(const GeoLocation& location,
                                  QueryCallback callback);

    /**
     * @brief Custom query with specific prompt (async)
     * @param location Geographic location for context
     * @param customPrompt Custom prompt to send to LLM
     * @param callback Callback function for results
     */
    void queryCustom(const GeoLocation& location,
                     const std::string& customPrompt,
                     QueryCallback callback);

    // =================================================================
    // Query Methods - Sync
    // =================================================================

    /**
     * @brief Queries for business demographics (sync)
     * @param location Geographic location to query
     * @return Query result with demographics
     */
    LLMQueryResult queryBusinessDemographicsSync(const GeoLocation& location);

    /**
     * @brief Queries for local competitors (sync)
     * @param location Geographic location
     * @param businessType Type of business
     * @return Query result with competitor info
     */
    LLMQueryResult queryLocalCompetitorsSync(const GeoLocation& location,
                                             const std::string& businessType);

    /**
     * @brief Queries for market opportunities (sync)
     * @param location Geographic location
     * @return Query result with opportunities
     */
    LLMQueryResult queryMarketOpportunitiesSync(const GeoLocation& location);

    // =================================================================
    // Utility Methods
    // =================================================================

    /**
     * @brief Validates API credentials
     * @return True if credentials are valid
     */
    bool validateCredentials();

    /**
     * @brief Gets usage statistics
     * @return JSON object with usage stats
     */
    Wt::Json::Object getUsageStats() const;

    /**
     * @brief Clears cached results
     */
    void clearCache();

    /**
     * @brief Gets the provider name as string
     * @param provider Provider enum value
     * @return Provider name string
     */
    static std::string providerToString(LLMProvider provider);

    /**
     * @brief Converts string to provider enum
     * @param providerStr Provider name string
     * @return Provider enum value
     */
    static LLMProvider stringToProvider(const std::string& providerStr);

protected:
    /**
     * @brief Builds the prompt for demographics query
     * @param location Location to query about
     * @return Formatted prompt string
     */
    std::string buildDemographicsPrompt(const GeoLocation& location) const;

    /**
     * @brief Builds the prompt for competitor query
     * @param location Location to query
     * @param businessType Business type to find competitors for
     * @return Formatted prompt string
     */
    std::string buildCompetitorPrompt(const GeoLocation& location,
                                      const std::string& businessType) const;

    /**
     * @brief Builds the prompt for opportunities query
     * @param location Location to query
     * @return Formatted prompt string
     */
    std::string buildOpportunitiesPrompt(const GeoLocation& location) const;

    /**
     * @brief Sends a query to the LLM API
     * @param prompt The prompt to send
     * @param callback Callback for response
     */
    void sendQuery(const std::string& prompt, QueryCallback callback);

    /**
     * @brief Sends a query synchronously
     * @param prompt The prompt to send
     * @return Query result
     */
    LLMQueryResult sendQuerySync(const std::string& prompt);

    /**
     * @brief Parses the LLM response into structured data
     * @param response Raw response from LLM
     * @return Parsed business demographics
     */
    BusinessDemographics parseResponse(const std::string& response) const;

    /**
     * @brief Gets the API endpoint for the current provider
     * @return API endpoint URL
     */
    std::string getApiEndpoint() const;

    /**
     * @brief Builds request headers for the current provider
     * @return Map of header name to value
     */
    std::map<std::string, std::string> buildHeaders() const;

    /**
     * @brief Builds the request body for the current provider
     * @param prompt The prompt to send
     * @return JSON request body
     */
    Wt::Json::Object buildRequestBody(const std::string& prompt) const;

private:
    // Logger reference
    Logger& logger_;

    // Event manager for notifications
    std::shared_ptr<EventManager> eventManager_;

    // HTTP client for API requests
    std::unique_ptr<APIClient> apiClient_;

    // Configuration
    std::string apiKey_;
    std::string baseUrl_;
    std::string model_;
    LLMProvider provider_;
    int timeoutSeconds_;
    bool debugMode_;
    bool initialized_;

    // Usage tracking
    int totalQueries_;
    int totalTokensUsed_;
    double totalCost_;

    // Cache (simple in-memory cache)
    std::map<std::string, LLMQueryResult> queryCache_;

    // Helper methods
    void logDebug(const std::string& message);
    void logError(const std::string& message);
    void publishEvent(const std::string& eventType, const Wt::Json::Object& data);
    std::string generateCacheKey(const GeoLocation& location, const std::string& queryType) const;

    // Provider-specific methods
    std::string getDefaultModel() const;
    std::string getDefaultBaseUrl() const;
};

#endif // LLMQUERYSERVICE_H
