//============================================================================
// src/services/LLMQueryService.cpp - LLM Query Service Implementation
//============================================================================

#include "../../include/services/LLMQueryService.hpp"
#include "../../include/utils/Logging.hpp"

#include <Wt/Json/Serializer.h>
#include <Wt/Json/Parser.h>

#include <sstream>
#include <chrono>
#include <iomanip>
#include <cmath>

//============================================================================
// GeoLocation Implementation
//============================================================================

Wt::Json::Object GeoLocation::toJson() const {
    Wt::Json::Object json;
    json["latitude"] = Wt::Json::Value(latitude);
    json["longitude"] = Wt::Json::Value(longitude);
    json["radiusKm"] = Wt::Json::Value(radiusKm);

    if (!address.empty()) {
        json["address"] = Wt::Json::Value(address);
    }
    if (!city.empty()) {
        json["city"] = Wt::Json::Value(city);
    }
    if (!state.empty()) {
        json["state"] = Wt::Json::Value(state);
    }
    if (!country.empty()) {
        json["country"] = Wt::Json::Value(country);
    }

    return json;
}

GeoLocation GeoLocation::fromJson(const Wt::Json::Object& json) {
    GeoLocation loc;

    if (json.contains("latitude")) {
        loc.latitude = json.get("latitude").toNumber().orIfNull(0.0);
    }
    if (json.contains("longitude")) {
        loc.longitude = json.get("longitude").toNumber().orIfNull(0.0);
    }
    if (json.contains("radiusKm")) {
        loc.radiusKm = json.get("radiusKm").toNumber().orIfNull(5.0);
    }
    if (json.contains("address")) {
        loc.address = json.get("address").toString().orIfNull("");
    }
    if (json.contains("city")) {
        loc.city = json.get("city").toString().orIfNull("");
    }
    if (json.contains("state")) {
        loc.state = json.get("state").toString().orIfNull("");
    }
    if (json.contains("country")) {
        loc.country = json.get("country").toString().orIfNull("");
    }

    return loc;
}

std::string GeoLocation::toString() const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6);
    ss << "(" << latitude << ", " << longitude << ")";

    if (!city.empty()) {
        ss << " - " << city;
        if (!state.empty()) {
            ss << ", " << state;
        }
    } else if (!address.empty()) {
        ss << " - " << address;
    }

    ss << " [radius: " << radiusKm << "km]";
    return ss.str();
}

bool GeoLocation::isValid() const {
    // Valid latitude: -90 to 90
    // Valid longitude: -180 to 180
    return (latitude >= -90.0 && latitude <= 90.0) &&
           (longitude >= -180.0 && longitude <= 180.0) &&
           (radiusKm > 0.0);
}

//============================================================================
// BusinessInfo Implementation
//============================================================================

Wt::Json::Object BusinessInfo::toJson() const {
    Wt::Json::Object json;
    json["name"] = Wt::Json::Value(name);
    json["type"] = Wt::Json::Value(type);
    json["category"] = Wt::Json::Value(category);
    json["address"] = Wt::Json::Value(address);
    json["location"] = location.toJson();
    json["phone"] = Wt::Json::Value(phone);
    json["website"] = Wt::Json::Value(website);
    json["rating"] = Wt::Json::Value(rating);
    json["reviewCount"] = Wt::Json::Value(reviewCount);
    json["priceRange"] = Wt::Json::Value(priceRange);
    json["isOpen"] = Wt::Json::Value(isOpen);
    json["operatingHours"] = Wt::Json::Value(operatingHours);

    Wt::Json::Array cuisines;
    for (const auto& cuisine : cuisineTypes) {
        cuisines.push_back(Wt::Json::Value(cuisine));
    }
    json["cuisineTypes"] = cuisines;

    return json;
}

BusinessInfo BusinessInfo::fromJson(const Wt::Json::Object& json) {
    BusinessInfo info;

    if (json.contains("name")) {
        info.name = json.get("name").toString().orIfNull("");
    }
    if (json.contains("type")) {
        info.type = json.get("type").toString().orIfNull("");
    }
    if (json.contains("category")) {
        info.category = json.get("category").toString().orIfNull("");
    }
    if (json.contains("address")) {
        info.address = json.get("address").toString().orIfNull("");
    }
    if (json.contains("location")) {
        auto locObj = json.get("location");
        if (locObj.type() == Wt::Json::Type::Object) {
            info.location = GeoLocation::fromJson(locObj);
        }
    }
    if (json.contains("phone")) {
        info.phone = json.get("phone").toString().orIfNull("");
    }
    if (json.contains("website")) {
        info.website = json.get("website").toString().orIfNull("");
    }
    if (json.contains("rating")) {
        info.rating = json.get("rating").toNumber().orIfNull(0.0);
    }
    if (json.contains("reviewCount")) {
        info.reviewCount = static_cast<int>(json.get("reviewCount").toNumber().orIfNull(0));
    }
    if (json.contains("priceRange")) {
        info.priceRange = json.get("priceRange").toString().orIfNull("");
    }
    if (json.contains("isOpen")) {
        info.isOpen = json.get("isOpen").toBool().orIfNull(false);
    }
    if (json.contains("operatingHours")) {
        info.operatingHours = json.get("operatingHours").toString().orIfNull("");
    }
    if (json.contains("cuisineTypes")) {
        auto cuisinesArray = json.get("cuisineTypes");
        if (cuisinesArray.type() == Wt::Json::Type::Array) {
            const Wt::Json::Array& arr = cuisinesArray;
            for (size_t i = 0; i < arr.size(); ++i) {
                info.cuisineTypes.push_back(arr[i].toString().orIfNull(""));
            }
        }
    }

    return info;
}

//============================================================================
// BusinessDemographics Implementation
//============================================================================

Wt::Json::Object BusinessDemographics::toJson() const {
    Wt::Json::Object json;

    json["searchLocation"] = searchLocation.toJson();
    json["totalBusinesses"] = Wt::Json::Value(totalBusinesses);
    json["totalRestaurants"] = Wt::Json::Value(totalRestaurants);
    json["totalRetail"] = Wt::Json::Value(totalRetail);
    json["totalServices"] = Wt::Json::Value(totalServices);

    json["marketSaturation"] = Wt::Json::Value(marketSaturation);
    json["dominantCuisine"] = Wt::Json::Value(dominantCuisine);
    json["averagePriceRange"] = Wt::Json::Value(averagePriceRange);
    json["averageRating"] = Wt::Json::Value(averageRating);

    json["peakHours"] = Wt::Json::Value(peakHours);
    json["peakDays"] = Wt::Json::Value(peakDays);
    json["trafficLevel"] = Wt::Json::Value(trafficLevel);

    json["primaryDemographic"] = Wt::Json::Value(primaryDemographic);
    json["incomeLevel"] = Wt::Json::Value(incomeLevel);
    json["estimatedPopulation"] = Wt::Json::Value(estimatedPopulation);

    json["summary"] = Wt::Json::Value(summary);

    // Arrays
    Wt::Json::Array competitorsArr;
    for (const auto& comp : competitors) {
        competitorsArr.push_back(comp.toJson());
    }
    json["competitors"] = competitorsArr;

    Wt::Json::Array underservedArr;
    for (const auto& cat : underservedCategories) {
        underservedArr.push_back(Wt::Json::Value(cat));
    }
    json["underservedCategories"] = underservedArr;

    Wt::Json::Array oversaturatedArr;
    for (const auto& cat : oversaturatedCategories) {
        oversaturatedArr.push_back(Wt::Json::Value(cat));
    }
    json["oversaturatedCategories"] = oversaturatedArr;

    Wt::Json::Array opportunitiesArr;
    for (const auto& opp : opportunities) {
        opportunitiesArr.push_back(Wt::Json::Value(opp));
    }
    json["opportunities"] = opportunitiesArr;

    Wt::Json::Array challengesArr;
    for (const auto& ch : challenges) {
        challengesArr.push_back(Wt::Json::Value(ch));
    }
    json["challenges"] = challengesArr;

    Wt::Json::Array recommendationsArr;
    for (const auto& rec : recommendations) {
        recommendationsArr.push_back(Wt::Json::Value(rec));
    }
    json["recommendations"] = recommendationsArr;

    return json;
}

BusinessDemographics BusinessDemographics::fromJson(const Wt::Json::Object& json) {
    BusinessDemographics demo;

    if (json.contains("searchLocation")) {
        auto locObj = json.get("searchLocation");
        if (locObj.type() == Wt::Json::Type::Object) {
            demo.searchLocation = GeoLocation::fromJson(locObj);
        }
    }

    if (json.contains("totalBusinesses")) {
        demo.totalBusinesses = static_cast<int>(json.get("totalBusinesses").toNumber().orIfNull(0));
    }
    if (json.contains("totalRestaurants")) {
        demo.totalRestaurants = static_cast<int>(json.get("totalRestaurants").toNumber().orIfNull(0));
    }
    if (json.contains("totalRetail")) {
        demo.totalRetail = static_cast<int>(json.get("totalRetail").toNumber().orIfNull(0));
    }
    if (json.contains("totalServices")) {
        demo.totalServices = static_cast<int>(json.get("totalServices").toNumber().orIfNull(0));
    }

    if (json.contains("marketSaturation")) {
        demo.marketSaturation = json.get("marketSaturation").toString().orIfNull("");
    }
    if (json.contains("dominantCuisine")) {
        demo.dominantCuisine = json.get("dominantCuisine").toString().orIfNull("");
    }
    if (json.contains("averagePriceRange")) {
        demo.averagePriceRange = json.get("averagePriceRange").toString().orIfNull("");
    }
    if (json.contains("averageRating")) {
        demo.averageRating = json.get("averageRating").toNumber().orIfNull(0.0);
    }

    if (json.contains("peakHours")) {
        demo.peakHours = json.get("peakHours").toString().orIfNull("");
    }
    if (json.contains("peakDays")) {
        demo.peakDays = json.get("peakDays").toString().orIfNull("");
    }
    if (json.contains("trafficLevel")) {
        demo.trafficLevel = json.get("trafficLevel").toString().orIfNull("");
    }

    if (json.contains("primaryDemographic")) {
        demo.primaryDemographic = json.get("primaryDemographic").toString().orIfNull("");
    }
    if (json.contains("incomeLevel")) {
        demo.incomeLevel = json.get("incomeLevel").toString().orIfNull("");
    }
    if (json.contains("estimatedPopulation")) {
        demo.estimatedPopulation = static_cast<int>(json.get("estimatedPopulation").toNumber().orIfNull(0));
    }

    if (json.contains("summary")) {
        demo.summary = json.get("summary").toString().orIfNull("");
    }

    // Parse arrays
    auto parseStringArray = [&json](const std::string& key) -> std::vector<std::string> {
        std::vector<std::string> result;
        if (json.contains(key)) {
            auto arrVal = json.get(key);
            if (arrVal.type() == Wt::Json::Type::Array) {
                const Wt::Json::Array& arr = arrVal;
                for (size_t i = 0; i < arr.size(); ++i) {
                    result.push_back(arr[i].toString().orIfNull(""));
                }
            }
        }
        return result;
    };

    demo.underservedCategories = parseStringArray("underservedCategories");
    demo.oversaturatedCategories = parseStringArray("oversaturatedCategories");
    demo.opportunities = parseStringArray("opportunities");
    demo.challenges = parseStringArray("challenges");
    demo.recommendations = parseStringArray("recommendations");

    // Parse competitors
    if (json.contains("competitors")) {
        auto compArr = json.get("competitors");
        if (compArr.type() == Wt::Json::Type::Array) {
            const Wt::Json::Array& arr = compArr;
            for (size_t i = 0; i < arr.size(); ++i) {
                if (arr[i].type() == Wt::Json::Type::Object) {
                    demo.competitors.push_back(BusinessInfo::fromJson(arr[i]));
                }
            }
        }
    }

    return demo;
}

//============================================================================
// LLMQueryService Implementation
//============================================================================

LLMQueryService::LLMQueryService(std::shared_ptr<EventManager> eventManager)
    : logger_(Logger::getInstance())
    , eventManager_(eventManager)
    , provider_(LLMProvider::ANTHROPIC)
    , timeoutSeconds_(60)
    , debugMode_(false)
    , initialized_(false)
    , totalQueries_(0)
    , totalTokensUsed_(0)
    , totalCost_(0.0)
{
    logger_.info("LLMQueryService: Creating LLM Query Service instance");
}

LLMQueryService::~LLMQueryService() {
    logger_.info("LLMQueryService: Destroying LLM Query Service instance");
}

bool LLMQueryService::initialize(const std::string& apiKey,
                                  LLMProvider provider,
                                  const std::string& baseUrl) {
    logger_.info("LLMQueryService: Initializing with provider: " + providerToString(provider));

    apiKey_ = apiKey;
    provider_ = provider;
    baseUrl_ = baseUrl.empty() ? getDefaultBaseUrl() : baseUrl;
    model_ = getDefaultModel();

    if (apiKey_.empty()) {
        logger_.info("LLMQueryService: API key is empty - service will not be functional");
        initialized_ = false;
        return false;
    }

    // Create API client
    apiClient_ = std::make_unique<APIClient>(baseUrl_);
    apiClient_->setAuthToken(apiKey_);
    apiClient_->setTimeout(timeoutSeconds_);
    apiClient_->setDebugMode(debugMode_);

    initialized_ = true;
    logger_.info("LLMQueryService: Initialized successfully with model: " + model_);

    return true;
}

void LLMQueryService::setProvider(LLMProvider provider) {
    provider_ = provider;
    baseUrl_ = getDefaultBaseUrl();
    model_ = getDefaultModel();
    logger_.info("LLMQueryService: Provider changed to: " + providerToString(provider));
}

LLMQueryService::LLMProvider LLMQueryService::getProvider() const {
    return provider_;
}

void LLMQueryService::setModel(const std::string& model) {
    model_ = model;
    logger_.info("LLMQueryService: Model set to: " + model);
}

std::string LLMQueryService::getModel() const {
    return model_;
}

void LLMQueryService::setTimeout(int timeoutSeconds) {
    timeoutSeconds_ = timeoutSeconds;
    if (apiClient_) {
        apiClient_->setTimeout(timeoutSeconds);
    }
}

void LLMQueryService::setDebugMode(bool enabled) {
    debugMode_ = enabled;
    if (apiClient_) {
        apiClient_->setDebugMode(enabled);
    }
}

bool LLMQueryService::isReady() const {
    return initialized_ && !apiKey_.empty();
}

//============================================================================
// Async Query Methods
//============================================================================

void LLMQueryService::queryBusinessDemographics(const GeoLocation& location,
                                                 QueryCallback callback) {
    if (!isReady()) {
        LLMQueryResult result;
        result.success = false;
        result.errorMessage = "LLM service not initialized";
        if (callback) callback(result);
        return;
    }

    if (!location.isValid()) {
        LLMQueryResult result;
        result.success = false;
        result.errorMessage = "Invalid geolocation coordinates";
        if (callback) callback(result);
        return;
    }

    logger_.info("LLMQueryService: Querying business demographics for: " + location.toString());

    std::string prompt = buildDemographicsPrompt(location);
    sendQuery(prompt, callback);
}

void LLMQueryService::queryLocalCompetitors(const GeoLocation& location,
                                             const std::string& businessType,
                                             QueryCallback callback) {
    if (!isReady()) {
        LLMQueryResult result;
        result.success = false;
        result.errorMessage = "LLM service not initialized";
        if (callback) callback(result);
        return;
    }

    logger_.info("LLMQueryService: Querying local competitors for: " + businessType);

    std::string prompt = buildCompetitorPrompt(location, businessType);
    sendQuery(prompt, callback);
}

void LLMQueryService::queryMarketOpportunities(const GeoLocation& location,
                                                QueryCallback callback) {
    if (!isReady()) {
        LLMQueryResult result;
        result.success = false;
        result.errorMessage = "LLM service not initialized";
        if (callback) callback(result);
        return;
    }

    logger_.info("LLMQueryService: Querying market opportunities for: " + location.toString());

    std::string prompt = buildOpportunitiesPrompt(location);
    sendQuery(prompt, callback);
}

void LLMQueryService::queryCustom(const GeoLocation& location,
                                   const std::string& customPrompt,
                                   QueryCallback callback) {
    if (!isReady()) {
        LLMQueryResult result;
        result.success = false;
        result.errorMessage = "LLM service not initialized";
        if (callback) callback(result);
        return;
    }

    std::ostringstream fullPrompt;
    fullPrompt << "Location context: " << location.toString() << "\n\n";
    fullPrompt << customPrompt;

    sendQuery(fullPrompt.str(), callback);
}

//============================================================================
// Sync Query Methods
//============================================================================

LLMQueryResult LLMQueryService::queryBusinessDemographicsSync(const GeoLocation& location) {
    if (!isReady()) {
        LLMQueryResult result;
        result.success = false;
        result.errorMessage = "LLM service not initialized";
        return result;
    }

    if (!location.isValid()) {
        LLMQueryResult result;
        result.success = false;
        result.errorMessage = "Invalid geolocation coordinates";
        return result;
    }

    logger_.info("LLMQueryService: Sync query for business demographics: " + location.toString());

    std::string prompt = buildDemographicsPrompt(location);
    return sendQuerySync(prompt);
}

LLMQueryResult LLMQueryService::queryLocalCompetitorsSync(const GeoLocation& location,
                                                          const std::string& businessType) {
    if (!isReady()) {
        LLMQueryResult result;
        result.success = false;
        result.errorMessage = "LLM service not initialized";
        return result;
    }

    std::string prompt = buildCompetitorPrompt(location, businessType);
    return sendQuerySync(prompt);
}

LLMQueryResult LLMQueryService::queryMarketOpportunitiesSync(const GeoLocation& location) {
    if (!isReady()) {
        LLMQueryResult result;
        result.success = false;
        result.errorMessage = "LLM service not initialized";
        return result;
    }

    std::string prompt = buildOpportunitiesPrompt(location);
    return sendQuerySync(prompt);
}

//============================================================================
// Utility Methods
//============================================================================

bool LLMQueryService::validateCredentials() {
    if (apiKey_.empty()) {
        return false;
    }

    // Simple validation - try a minimal API call
    // In production, this would make a test request
    return true;
}

Wt::Json::Object LLMQueryService::getUsageStats() const {
    Wt::Json::Object stats;
    stats["totalQueries"] = Wt::Json::Value(totalQueries_);
    stats["totalTokensUsed"] = Wt::Json::Value(totalTokensUsed_);
    stats["totalCost"] = Wt::Json::Value(totalCost_);
    stats["provider"] = Wt::Json::Value(providerToString(provider_));
    stats["model"] = Wt::Json::Value(model_);
    stats["cacheSize"] = Wt::Json::Value(static_cast<int>(queryCache_.size()));
    return stats;
}

void LLMQueryService::clearCache() {
    queryCache_.clear();
    logger_.info("LLMQueryService: Cache cleared");
}

std::string LLMQueryService::providerToString(LLMProvider provider) {
    switch (provider) {
        case LLMProvider::ANTHROPIC: return "anthropic";
        case LLMProvider::OPENAI: return "openai";
        case LLMProvider::GOOGLE: return "google";
        case LLMProvider::LOCAL: return "local";
        default: return "unknown";
    }
}

LLMQueryService::LLMProvider LLMQueryService::stringToProvider(const std::string& providerStr) {
    std::string lower = providerStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "anthropic" || lower == "claude") return LLMProvider::ANTHROPIC;
    if (lower == "openai" || lower == "gpt") return LLMProvider::OPENAI;
    if (lower == "google" || lower == "gemini") return LLMProvider::GOOGLE;
    if (lower == "local") return LLMProvider::LOCAL;

    return LLMProvider::ANTHROPIC;  // Default
}

//============================================================================
// Protected Methods - Prompt Building
//============================================================================

std::string LLMQueryService::buildDemographicsPrompt(const GeoLocation& location) const {
    std::ostringstream ss;

    ss << "Analyze the local business demographics and market conditions for the following location:\n\n";

    ss << "Location: ";
    if (!location.address.empty()) {
        ss << location.address << "\n";
    }
    if (!location.city.empty()) {
        ss << "City: " << location.city;
        if (!location.state.empty()) {
            ss << ", " << location.state;
        }
        ss << "\n";
    }
    ss << "Coordinates: " << std::fixed << std::setprecision(6)
       << location.latitude << ", " << location.longitude << "\n";
    ss << "Search Radius: " << location.radiusKm << " km\n\n";

    ss << "Please provide a comprehensive analysis in JSON format with the following structure:\n";
    ss << R"({
  "totalBusinesses": <number>,
  "totalRestaurants": <number>,
  "totalRetail": <number>,
  "totalServices": <number>,
  "marketSaturation": "<low|medium|high>",
  "dominantCuisine": "<most common cuisine type>",
  "averagePriceRange": "<$|$$|$$$|$$$$>",
  "averageRating": <number 0-5>,
  "peakHours": "<description of peak hours>",
  "peakDays": "<busiest days>",
  "trafficLevel": "<low|medium|high>",
  "primaryDemographic": "<main customer demographic>",
  "incomeLevel": "<low|middle|upper-middle|high>",
  "estimatedPopulation": <number>,
  "underservedCategories": ["<category1>", "<category2>"],
  "oversaturatedCategories": ["<category1>", "<category2>"],
  "opportunities": ["<opportunity1>", "<opportunity2>"],
  "challenges": ["<challenge1>", "<challenge2>"],
  "recommendations": ["<recommendation1>", "<recommendation2>"],
  "summary": "<brief summary of the market analysis>"
})";

    ss << "\n\nProvide realistic estimates based on typical patterns for this type of area.";

    return ss.str();
}

std::string LLMQueryService::buildCompetitorPrompt(const GeoLocation& location,
                                                    const std::string& businessType) const {
    std::ostringstream ss;

    ss << "Identify and analyze local competitors for a " << businessType << " business at:\n\n";

    if (!location.city.empty()) {
        ss << "Location: " << location.city;
        if (!location.state.empty()) {
            ss << ", " << location.state;
        }
        ss << "\n";
    }
    ss << "Coordinates: " << std::fixed << std::setprecision(6)
       << location.latitude << ", " << location.longitude << "\n";
    ss << "Search Radius: " << location.radiusKm << " km\n\n";

    ss << "Please provide competitor analysis in JSON format:\n";
    ss << R"({
  "competitors": [
    {
      "name": "<business name>",
      "type": "<business type>",
      "category": "<specific category>",
      "address": "<address>",
      "rating": <number 0-5>,
      "reviewCount": <number>,
      "priceRange": "<$|$$|$$$|$$$$>",
      "cuisineTypes": ["<type1>", "<type2>"],
      "operatingHours": "<hours description>"
    }
  ],
  "marketSaturation": "<low|medium|high>",
  "summary": "<competitive landscape summary>",
  "opportunities": ["<gap in market>"],
  "recommendations": ["<how to differentiate>"]
})";

    return ss.str();
}

std::string LLMQueryService::buildOpportunitiesPrompt(const GeoLocation& location) const {
    std::ostringstream ss;

    ss << "Analyze market opportunities for new businesses at:\n\n";

    if (!location.city.empty()) {
        ss << "Location: " << location.city;
        if (!location.state.empty()) {
            ss << ", " << location.state;
        }
        ss << "\n";
    }
    ss << "Coordinates: " << std::fixed << std::setprecision(6)
       << location.latitude << ", " << location.longitude << "\n";
    ss << "Search Radius: " << location.radiusKm << " km\n\n";

    ss << "Provide market opportunity analysis in JSON format:\n";
    ss << R"({
  "underservedCategories": ["<category needing more businesses>"],
  "opportunities": [
    {
      "type": "<business type>",
      "reason": "<why this is an opportunity>",
      "targetDemographic": "<who would be served>",
      "estimatedDemand": "<low|medium|high>",
      "competitionLevel": "<low|medium|high>"
    }
  ],
  "challenges": ["<potential challenge>"],
  "recommendations": ["<specific recommendation>"],
  "summary": "<overall opportunity assessment>"
})";

    return ss.str();
}

//============================================================================
// Protected Methods - API Communication
//============================================================================

void LLMQueryService::sendQuery(const std::string& prompt, QueryCallback callback) {
    auto startTime = std::chrono::high_resolution_clock::now();

    Wt::Json::Object requestBody = buildRequestBody(prompt);
    std::string endpoint = getApiEndpoint();

    logDebug("Sending query to: " + endpoint);

    apiClient_->post(endpoint, requestBody, [this, callback, startTime](const APIClient::APIResponse& response) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        LLMQueryResult result;
        result.queryTimeMs = static_cast<double>(duration.count());
        result.model = model_;

        if (response.success) {
            try {
                // Extract the response content based on provider
                std::string content;

                if (provider_ == LLMProvider::ANTHROPIC) {
                    // Claude API response format
                    if (response.data.contains("content")) {
                        auto contentArr = response.data.get("content");
                        if (contentArr.type() == Wt::Json::Type::Array) {
                            const Wt::Json::Array& arr = contentArr;
                            if (arr.size() > 0) {
                                auto firstContent = arr[0];
                                if (firstContent.type() == Wt::Json::Type::Object) {
                                    const Wt::Json::Object& obj = firstContent;
                                    if (obj.contains("text")) {
                                        content = obj.get("text").toString().orIfNull("");
                                    }
                                }
                            }
                        }
                    }
                    // Extract usage info
                    if (response.data.contains("usage")) {
                        auto usage = response.data.get("usage");
                        if (usage.type() == Wt::Json::Type::Object) {
                            const Wt::Json::Object& usageObj = usage;
                            if (usageObj.contains("output_tokens")) {
                                result.tokensUsed = static_cast<int>(
                                    usageObj.get("output_tokens").toNumber().orIfNull(0));
                            }
                        }
                    }
                } else if (provider_ == LLMProvider::OPENAI) {
                    // OpenAI API response format
                    if (response.data.contains("choices")) {
                        auto choices = response.data.get("choices");
                        if (choices.type() == Wt::Json::Type::Array) {
                            const Wt::Json::Array& arr = choices;
                            if (arr.size() > 0) {
                                auto firstChoice = arr[0];
                                if (firstChoice.type() == Wt::Json::Type::Object) {
                                    const Wt::Json::Object& choiceObj = firstChoice;
                                    if (choiceObj.contains("message")) {
                                        auto msg = choiceObj.get("message");
                                        if (msg.type() == Wt::Json::Type::Object) {
                                            const Wt::Json::Object& msgObj = msg;
                                            content = msgObj.get("content").toString().orIfNull("");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                result.rawResponse = content;
                result.demographics = parseResponse(content);
                result.success = true;

                totalQueries_++;
                totalTokensUsed_ += result.tokensUsed;

                logger_.info("LLMQueryService: Query successful, tokens: " +
                            std::to_string(result.tokensUsed));

            } catch (const std::exception& e) {
                result.success = false;
                result.errorMessage = std::string("LLMQueryService: Failed to parse response: ") + e.what();
                logger_.error(result.errorMessage);
            }
        } else {
            result.success = false;
            result.errorMessage = response.errorMessage;
            logger_.error("LLMQueryService: Query failed: " + response.errorMessage);
        }

        if (callback) {
            callback(result);
        }
    });
}

LLMQueryResult LLMQueryService::sendQuerySync(const std::string& prompt) {
    auto startTime = std::chrono::high_resolution_clock::now();

    Wt::Json::Object requestBody = buildRequestBody(prompt);
    std::string endpoint = getApiEndpoint();

    logDebug("Sending sync query to: " + endpoint);

    APIClient::APIResponse response = apiClient_->postSync(endpoint, requestBody);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    LLMQueryResult result;
    result.queryTimeMs = static_cast<double>(duration.count());
    result.model = model_;

    if (response.success) {
        try {
            std::string content;

            // Extract content based on provider (same logic as async)
            if (provider_ == LLMProvider::ANTHROPIC) {
                if (response.data.contains("content")) {
                    auto contentArr = response.data.get("content");
                    if (contentArr.type() == Wt::Json::Type::Array) {
                        const Wt::Json::Array& arr = contentArr;
                        if (arr.size() > 0) {
                            auto firstContent = arr[0];
                            if (firstContent.type() == Wt::Json::Type::Object) {
                                const Wt::Json::Object& obj = firstContent;
                                if (obj.contains("text")) {
                                    content = obj.get("text").toString().orIfNull("");
                                }
                            }
                        }
                    }
                }
            }

            result.rawResponse = content;
            result.demographics = parseResponse(content);
            result.success = true;

            totalQueries_++;
            totalTokensUsed_ += result.tokensUsed;

        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = std::string("Failed to parse response: ") + e.what();
        }
    } else {
        result.success = false;
        result.errorMessage = response.errorMessage;
    }

    return result;
}

BusinessDemographics LLMQueryService::parseResponse(const std::string& response) const {
    BusinessDemographics demo;

    // Try to find JSON in the response
    size_t jsonStart = response.find('{');
    size_t jsonEnd = response.rfind('}');

    if (jsonStart != std::string::npos && jsonEnd != std::string::npos && jsonEnd > jsonStart) {
        std::string jsonStr = response.substr(jsonStart, jsonEnd - jsonStart + 1);

        try {
            Wt::Json::Object json;
            Wt::Json::parse(jsonStr, json);
            demo = BusinessDemographics::fromJson(json);
        } catch (const std::exception& e) {
            logger_.info("LLMQueryService: Failed to parse JSON response: " + std::string(e.what()));
            // Set raw summary if JSON parsing fails
            demo.summary = response;
        }
    } else {
        // No JSON found, use raw response as summary
        demo.summary = response;
    }

    return demo;
}

std::string LLMQueryService::getApiEndpoint() const {
    switch (provider_) {
        case LLMProvider::ANTHROPIC:
            return "/v1/messages";
        case LLMProvider::OPENAI:
            return "/v1/chat/completions";
        case LLMProvider::GOOGLE:
            return "/v1/models/" + model_ + ":generateContent";
        case LLMProvider::LOCAL:
            return "/v1/completions";
        default:
            return "/v1/messages";
    }
}

std::map<std::string, std::string> LLMQueryService::buildHeaders() const {
    std::map<std::string, std::string> headers;

    headers["Content-Type"] = "application/json";

    switch (provider_) {
        case LLMProvider::ANTHROPIC:
            headers["x-api-key"] = apiKey_;
            headers["anthropic-version"] = "2023-06-01";
            break;
        case LLMProvider::OPENAI:
            headers["Authorization"] = "Bearer " + apiKey_;
            break;
        case LLMProvider::GOOGLE:
            headers["x-goog-api-key"] = apiKey_;
            break;
        default:
            headers["Authorization"] = "Bearer " + apiKey_;
            break;
    }

    return headers;
}

Wt::Json::Object LLMQueryService::buildRequestBody(const std::string& prompt) const {
    Wt::Json::Object body;

    switch (provider_) {
        case LLMProvider::ANTHROPIC: {
            body["model"] = Wt::Json::Value(model_);
            body["max_tokens"] = Wt::Json::Value(4096);

            Wt::Json::Array messages;
            Wt::Json::Object userMessage;
            userMessage["role"] = Wt::Json::Value("user");
            userMessage["content"] = Wt::Json::Value(prompt);
            messages.push_back(userMessage);

            body["messages"] = messages;
            break;
        }
        case LLMProvider::OPENAI: {
            body["model"] = Wt::Json::Value(model_);
            body["max_tokens"] = Wt::Json::Value(4096);

            Wt::Json::Array messages;
            Wt::Json::Object systemMessage;
            systemMessage["role"] = Wt::Json::Value("system");
            systemMessage["content"] = Wt::Json::Value(
                "You are a business demographics analyst. Provide accurate, data-driven insights about local markets.");
            messages.push_back(systemMessage);

            Wt::Json::Object userMessage;
            userMessage["role"] = Wt::Json::Value("user");
            userMessage["content"] = Wt::Json::Value(prompt);
            messages.push_back(userMessage);

            body["messages"] = messages;
            break;
        }
        case LLMProvider::GOOGLE: {
            Wt::Json::Object content;
            Wt::Json::Array parts;
            Wt::Json::Object textPart;
            textPart["text"] = Wt::Json::Value(prompt);
            parts.push_back(textPart);
            content["parts"] = parts;

            Wt::Json::Array contents;
            contents.push_back(content);
            body["contents"] = contents;
            break;
        }
        case LLMProvider::LOCAL:
        default: {
            body["model"] = Wt::Json::Value(model_);
            body["prompt"] = Wt::Json::Value(prompt);
            body["max_tokens"] = Wt::Json::Value(4096);
            break;
        }
    }

    return body;
}

//============================================================================
// Private Helper Methods
//============================================================================

void LLMQueryService::logDebug(const std::string& message) {
    if (debugMode_) {
        logger_.debug("LLMQueryService" + message);
    }
}

void LLMQueryService::logError(const std::string& message) {
    logger_.error("LLMQueryService" + message);
}

void LLMQueryService::publishEvent(const std::string& eventType, const Wt::Json::Object& data) {
    if (eventManager_) {
        eventManager_->publish(eventType, data, "LLMQueryService");
    }
}

std::string LLMQueryService::generateCacheKey(const GeoLocation& location,
                                               const std::string& queryType) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4);
    ss << queryType << "_" << location.latitude << "_" << location.longitude
       << "_" << location.radiusKm;
    return ss.str();
}

std::string LLMQueryService::getDefaultModel() const {
    switch (provider_) {
        case LLMProvider::ANTHROPIC:
            return "claude-3-sonnet-20240229";
        case LLMProvider::OPENAI:
            return "gpt-4-turbo-preview";
        case LLMProvider::GOOGLE:
            return "gemini-pro";
        case LLMProvider::LOCAL:
            return "local-model";
        default:
            return "claude-3-sonnet-20240229";
    }
}

std::string LLMQueryService::getDefaultBaseUrl() const {
    switch (provider_) {
        case LLMProvider::ANTHROPIC:
            return "https://api.anthropic.com";
        case LLMProvider::OPENAI:
            return "https://api.openai.com";
        case LLMProvider::GOOGLE:
            return "https://generativelanguage.googleapis.com";
        case LLMProvider::LOCAL:
            return "http://localhost:8080";
        default:
            return "https://api.anthropic.com";
    }
}
