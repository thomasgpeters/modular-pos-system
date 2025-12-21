# LLM Query Service for Local Business Demographics - Implementation Guide

## Overview

This guide describes how to add an LLM Query Service to a C++ Wt web application that queries AI language models (Claude, GPT, Gemini, etc.) for local business demographic information based on geolocation.

## Architecture Pattern

The implementation follows the existing service-oriented architecture:
- **Service class** with async/sync methods and callback patterns
- **Configuration via XML** with environment variable support
- **EventManager integration** for notifications
- **APIClient** for HTTP communication

## Files to Create

### 1. Header File: `include/services/LLMQueryService.hpp`

Create a header with these key structures and classes:

```cpp
// Data structures
struct GeoLocation {
    double latitude, longitude;
    std::string address, city, state, country;
    double radiusKm;  // Search radius

    Wt::Json::Object toJson() const;
    static GeoLocation fromJson(const Wt::Json::Object& json);
    bool isValid() const;  // Validate coordinates
};

struct BusinessInfo {
    std::string name, type, category, address;
    GeoLocation location;
    double rating;
    int reviewCount;
    std::string priceRange;  // "$", "$$", "$$$"
    std::vector<std::string> cuisineTypes;
};

struct BusinessDemographics {
    int totalBusinesses, totalRestaurants, totalRetail, totalServices;
    std::string marketSaturation;  // "low", "medium", "high"
    std::string dominantCuisine, averagePriceRange;
    std::string primaryDemographic, incomeLevel;
    std::vector<BusinessInfo> competitors;
    std::vector<std::string> opportunities, challenges, recommendations;
    std::string summary;
};

struct LLMQueryResult {
    bool success;
    std::string errorMessage;
    BusinessDemographics demographics;
    std::string rawResponse;
    int tokensUsed;
    double queryTimeMs;
};

class LLMQueryService {
public:
    enum class LLMProvider { ANTHROPIC, OPENAI, GOOGLE, LOCAL };
    using QueryCallback = std::function<void(const LLMQueryResult&)>;

    explicit LLMQueryService(std::shared_ptr<EventManager> eventManager);

    bool initialize(const std::string& apiKey, LLMProvider provider, const std::string& baseUrl = "");

    // Async queries
    void queryBusinessDemographics(const GeoLocation& location, QueryCallback callback);
    void queryLocalCompetitors(const GeoLocation& location, const std::string& businessType, QueryCallback callback);
    void queryMarketOpportunities(const GeoLocation& location, QueryCallback callback);

    // Sync queries
    LLMQueryResult queryBusinessDemographicsSync(const GeoLocation& location);

    // Configuration
    void setModel(const std::string& model);
    void setTimeout(int timeoutSeconds);
    void setDebugMode(bool enabled);
    bool isReady() const;

    static std::string providerToString(LLMProvider provider);
    static LLMProvider stringToProvider(const std::string& providerStr);

protected:
    std::string buildDemographicsPrompt(const GeoLocation& location) const;
    std::string getApiEndpoint() const;
    Wt::Json::Object buildRequestBody(const std::string& prompt) const;
    BusinessDemographics parseResponse(const std::string& response) const;

private:
    Logger& logger_;
    std::shared_ptr<EventManager> eventManager_;
    std::unique_ptr<APIClient> apiClient_;
    std::string apiKey_, baseUrl_, model_;
    LLMProvider provider_;
    int timeoutSeconds_;
    bool debugMode_, initialized_;
};
```

### 2. Implementation File: `src/services/LLMQueryService.cpp`

Key implementation details:

**Provider-specific API formatting:**
```cpp
Wt::Json::Object LLMQueryService::buildRequestBody(const std::string& prompt) const {
    Wt::Json::Object body;
    switch (provider_) {
        case LLMProvider::ANTHROPIC:
            body["model"] = Wt::Json::Value(model_);
            body["max_tokens"] = Wt::Json::Value(4096);
            // Create messages array with user message
            break;
        case LLMProvider::OPENAI:
            // Similar but with system message support
            break;
        // ... other providers
    }
    return body;
}
```

**Default endpoints per provider:**
```cpp
std::string LLMQueryService::getDefaultBaseUrl() const {
    switch (provider_) {
        case LLMProvider::ANTHROPIC: return "https://api.anthropic.com";
        case LLMProvider::OPENAI: return "https://api.openai.com";
        case LLMProvider::GOOGLE: return "https://generativelanguage.googleapis.com";
        case LLMProvider::LOCAL: return "http://localhost:8080";
    }
}
```

**Prompt building for demographics:**
```cpp
std::string LLMQueryService::buildDemographicsPrompt(const GeoLocation& location) const {
    std::ostringstream ss;
    ss << "Analyze the local business demographics for:\n";
    ss << "Location: " << location.city << ", " << location.state << "\n";
    ss << "Coordinates: " << location.latitude << ", " << location.longitude << "\n";
    ss << "Search Radius: " << location.radiusKm << " km\n\n";
    ss << "Provide analysis in JSON format with fields: totalBusinesses, marketSaturation, "
       << "opportunities, challenges, recommendations, summary\n";
    return ss.str();
}
```

## Files to Modify

### 3. Configuration Manager Header: `include/core/ConfigurationManager.hpp`

Add LLM configuration methods:
```cpp
// LLM Configuration
bool isLLMEnabled() const;
void setLLMEnabled(bool enabled);
std::string getLLMProvider() const;
std::string getLLMApiKey() const;
std::string getLLMModel() const;
std::string getLLMBaseUrl() const;
int getLLMTimeout() const;
int getLLMMaxTokens() const;
bool isLLMDebugMode() const;
double getLLMDefaultRadius() const;
double getLLMMaxRadius() const;

private:
    void setDefaultLLMConfig();
```

### 4. Configuration Manager Implementation: `src/core/ConfigurationManager.cpp`

Add `#include <cstdlib>` for `std::getenv()`.

Implement default config:
```cpp
void ConfigurationManager::setDefaultLLMConfig() {
    auto& llm = getOrCreateSection("llm");
    llm["enabled"] = false;
    llm["provider"] = std::string("anthropic");
    llm["api_key"] = std::string("");
    llm["model"] = std::string("claude-3-sonnet-20240229");
    llm["base_url"] = std::string("");
    llm["timeout"] = 60;
    llm["max_tokens"] = 4096;
    llm["debug_mode"] = false;
    llm["default_radius_km"] = 5.0;
    llm["max_radius_km"] = 50.0;
}
```

API key getter with environment variable fallback:
```cpp
std::string ConfigurationManager::getLLMApiKey() const {
    std::string key = getValue<std::string>("llm.api_key", "");
    if (key.empty() || key.find("${") != std::string::npos) {
        const char* envKey = std::getenv("LLM_API_KEY");
        if (envKey) return std::string(envKey);
    }
    return key;
}
```

Call `setDefaultLLMConfig()` from `loadDefaults()`.

### 5. XML Configuration: `pos_config.xml`

Add inside `<integrations>`:
```xml
<llm>
  <enabled>false</enabled>
  <provider>anthropic</provider>
  <api-key>${LLM_API_KEY}</api-key>
  <base-url></base-url>
  <model>claude-3-sonnet-20240229</model>
  <timeout>60</timeout>
  <max-tokens>4096</max-tokens>
  <debug-mode>false</debug-mode>
  <cache>
    <enabled>true</enabled>
    <ttl-minutes>30</ttl-minutes>
  </cache>
  <geolocation>
    <default-radius-km>5.0</default-radius-km>
    <max-radius-km>50.0</max-radius-km>
  </geolocation>
</llm>
```

### 6. Main Application Header: `include/core/RestaurantPOSApp.hpp`

Add include and member:
```cpp
#include "../services/LLMQueryService.hpp"

// In private section:
std::shared_ptr<LLMQueryService> llmQueryService_;

// Add method declaration:
void initializeLLMService();
```

### 7. Main Application Implementation: `src/core/RestaurantPOSApp.cpp`

Call `initializeLLMService()` from `initializeServices()`.

Implement initialization:
```cpp
void RestaurantPOSApp::initializeLLMService() {
    if (!configManager_->isLLMEnabled()) {
        logger_.info("LLM service is disabled");
        return;
    }

    llmQueryService_ = std::make_shared<LLMQueryService>(eventManager_);

    std::string apiKey = configManager_->getLLMApiKey();
    if (apiKey.empty()) {
        logger_.warning("LLM API key not configured");
        return;
    }

    auto provider = LLMQueryService::stringToProvider(configManager_->getLLMProvider());
    if (llmQueryService_->initialize(apiKey, provider, configManager_->getLLMBaseUrl())) {
        llmQueryService_->setModel(configManager_->getLLMModel());
        llmQueryService_->setTimeout(configManager_->getLLMTimeout());
        llmQueryService_->setDebugMode(configManager_->isLLMDebugMode());
        logger_.info("LLM Query Service initialized successfully");
    }
}
```

### 8. CMakeLists.txt

Add to SOURCES:
```cmake
src/services/LLMQueryService.cpp
```

Add to HEADERS:
```cmake
include/services/LLMQueryService.hpp
```

## Usage Example

```cpp
// Create geolocation
GeoLocation location(40.7128, -74.0060, 5.0);
location.city = "New York";
location.state = "NY";

// Async query
llmQueryService->queryBusinessDemographics(location, [](const LLMQueryResult& result) {
    if (result.success) {
        std::cout << "Total businesses: " << result.demographics.totalBusinesses << std::endl;
        std::cout << "Market: " << result.demographics.marketSaturation << std::endl;
        for (const auto& opp : result.demographics.opportunities) {
            std::cout << "Opportunity: " << opp << std::endl;
        }
    }
});

// Sync query
auto result = llmQueryService->queryBusinessDemographicsSync(location);
```

## Enabling the Service

1. Set in `pos_config.xml`: `<enabled>true</enabled>`
2. Set API key via environment: `export LLM_API_KEY=your_key`
3. Rebuild and run

## Supported LLM Providers

| Provider | Base URL | Default Model |
|----------|----------|---------------|
| Anthropic (Claude) | https://api.anthropic.com | claude-3-sonnet-20240229 |
| OpenAI (GPT) | https://api.openai.com | gpt-4-turbo-preview |
| Google (Gemini) | https://generativelanguage.googleapis.com | gemini-pro |
| Local | http://localhost:8080 | local-model |

## Data Structures Returned

### BusinessDemographics
- `totalBusinesses`, `totalRestaurants`, `totalRetail`, `totalServices` - counts
- `marketSaturation` - "low", "medium", "high"
- `dominantCuisine` - most common cuisine type in area
- `averagePriceRange` - "$", "$$", "$$$", "$$$$"
- `primaryDemographic` - e.g., "young professionals", "families"
- `incomeLevel` - "low", "middle", "upper-middle", "high"
- `competitors` - vector of BusinessInfo objects
- `opportunities` - market gaps identified
- `challenges` - potential issues
- `recommendations` - AI-generated suggestions
- `summary` - overall market analysis

### GeoLocation
- `latitude`, `longitude` - coordinates (-90 to 90, -180 to 180)
- `radiusKm` - search radius in kilometers
- `city`, `state`, `country` - optional location context
- `address` - human-readable address

## File Summary

| File | Action | Description |
|------|--------|-------------|
| `include/services/LLMQueryService.hpp` | CREATE | Service header with structs and class |
| `src/services/LLMQueryService.cpp` | CREATE | Service implementation |
| `include/core/ConfigurationManager.hpp` | MODIFY | Add LLM config methods |
| `src/core/ConfigurationManager.cpp` | MODIFY | Implement LLM config |
| `pos_config.xml` | MODIFY | Add LLM config section |
| `include/core/RestaurantPOSApp.hpp` | MODIFY | Add service member |
| `src/core/RestaurantPOSApp.cpp` | MODIFY | Initialize service |
| `CMakeLists.txt` | MODIFY | Add source/header files |
