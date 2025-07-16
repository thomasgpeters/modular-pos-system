//============================================================================
// src/api/APIClient.cpp - Implementation of APIClient
//============================================================================

#include "../../include/api/APIClient.hpp"
#include "../../include/api/APIConfiguration.hpp"
#include <iostream>
#include <sstream>

APIClient::APIClient(const std::string& baseUrl) 
    : baseUrl_(baseUrl), timeoutSeconds_(APIConfiguration::Defaults::API_TIMEOUT), 
      debugMode_(false) {
    
    initializeDefaults();
    httpClient_ = std::make_unique<Wt::Http::Client>();
    
    std::cout << "[APIClient] Initialized with base URL: " << baseUrl_ << std::endl;
}

void APIClient::get(const std::string& endpoint, 
                   const std::map<std::string, std::string>& params,
                   ResponseCallback callback) {
    
    std::string url = buildUrl(endpoint, params);
    debugLog("GET request to: " + url);
    
    if (!httpClient_) {
        APIResponse response;
        response.success = false;
        response.errorMessage = "HTTP client not initialized";
        if (callback) callback(response);
        return;
    }
    
    try {
        // Create HTTP message
        Wt::Http::Message message;
        addAuthHeaders(message);
        
        // For now, we'll implement a simplified version
        // In a real implementation, you'd use Wt::Http::Client::get()
        
        // Simulate API response for demo
        APIResponse response;
        response.success = true;
        response.statusCode = 200;
        
        // Create sample JSON:API response
        Wt::Json::Object sampleData;
        sampleData["type"] = Wt::Json::Value("Order");
        sampleData["id"] = Wt::Json::Value("1");
        
        Wt::Json::Object attributes;
        attributes["order_id"] = Wt::Json::Value(1);
        attributes["table_identifier"] = Wt::Json::Value("table 1");
        attributes["status"] = Wt::Json::Value("pending");
        sampleData["attributes"] = attributes;
        
        response.data = sampleData;
        
        if (callback) callback(response);
        
    } catch (const std::exception& e) {
        // FIXED: Use handleError instead of handleAPIError, and handle return value
        APIResponse errorResponse = handleError(500, std::string("Exception: ") + e.what());
        if (callback) {
            callback(errorResponse);
        }
    }
}

void APIClient::post(const std::string& endpoint,
                    const Wt::Json::Object& data,
                    ResponseCallback callback) {
    
    std::string url = buildUrl(endpoint);
    debugLog("POST request to: " + url);
    
    // Similar implementation to GET but with request body
    // For demo, we'll simulate success
    
    APIResponse response;
    response.success = true;
    response.statusCode = 201;
    response.data = data; // Echo back the data
    
    if (callback) callback(response);
}

void APIClient::put(const std::string& endpoint,
                   const Wt::Json::Object& data,
                   ResponseCallback callback) {
    
    std::string url = buildUrl(endpoint);
    debugLog("PUT request to: " + url);
    
    // Simulate API response for demo
    APIResponse response;
    response.success = true;
    response.statusCode = 200;
    response.data = data;
    
    if (callback) callback(response);
}

void APIClient::patch(const std::string& endpoint,
                     const Wt::Json::Object& data,
                     ResponseCallback callback) {
    
    std::string url = buildUrl(endpoint);
    debugLog("PATCH request to: " + url);
    
    // Simulate API response for demo
    APIResponse response;
    response.success = true;
    response.statusCode = 200;
    response.data = data;
    
    if (callback) callback(response);
}

void APIClient::delete_(const std::string& endpoint,
                       ResponseCallback callback) {
    
    std::string url = buildUrl(endpoint);
    debugLog("DELETE request to: " + url);
    
    // Simulate API response for demo
    APIResponse response;
    response.success = true;
    response.statusCode = 204; // No Content
    
    if (callback) callback(response);
}

APIClient::APIResponse APIClient::getSync(const std::string& endpoint,
                                         const std::map<std::string, std::string>& params) {
    
    std::string url = buildUrl(endpoint, params);
    debugLog("Synchronous GET request to: " + url);
    
    // For demo, return sample data
    APIResponse response;
    response.success = true;
    response.statusCode = 200;
    
    Wt::Json::Object sampleData;
    sampleData["type"] = Wt::Json::Value("Order");
    sampleData["id"] = Wt::Json::Value("1");
    
    response.data = sampleData;
    return response;
}

APIClient::APIResponse APIClient::postSync(const std::string& endpoint,
                                          const Wt::Json::Object& data) {
    
    std::string url = buildUrl(endpoint);
    debugLog("Synchronous POST request to: " + url);
    
    // For demo, return success
    APIResponse response;
    response.success = true;
    response.statusCode = 201;
    response.data = data;
    
    return response;
}

std::string APIClient::buildUrl(const std::string& endpoint,
                               const std::map<std::string, std::string>& params) {
    
    std::string url = baseUrl_;
    if (url.back() == '/' && endpoint.front() == '/') {
        url.pop_back();
    } else if (url.back() != '/' && endpoint.front() != '/') {
        url += '/';
    }
    url += endpoint;
    
    if (!params.empty()) {
        url += "?" + encodeQueryParams(params);
    }
    
    return url;
}

void APIClient::addAuthHeaders(Wt::Http::Message& message) {
    // FIXED: Use APIConfiguration static methods
    auto defaultHeaders = APIConfiguration::getDefaultHeaders();
    for (const auto& [key, value] : defaultHeaders) {
        message.addHeader(key, value);
    }
    
    // Add custom headers
    for (const auto& [key, value] : defaultHeaders_) {
        message.addHeader(key, value);
    }
    
    // Add authentication header if token is set
    if (!authToken_.empty()) {
        message.addHeader(APIConfiguration::Headers::AUTHORIZATION,
                         APIConfiguration::buildAuthHeader(authToken_));
    }
}

APIClient::APIResponse APIClient::parseResponse(const Wt::Http::Message& response) {
    APIResponse result;
    
    // Get status code (this would need to be implemented based on Wt::Http::Message API)
    // result.statusCode = response.status();
    
    // Parse JSON response body
    std::string body = response.body();
    
    if (isValidJson(body)) {
        try {
            Wt::Json::Object jsonResponse;
            Wt::Json::parse(body, jsonResponse);
            
            // Parse JSON:API format
            if (jsonResponse.contains("data")) {
                auto dataValue = jsonResponse.get("data");
                if (dataValue.type() == Wt::Json::Type::Object) {
                    result.data = static_cast<Wt::Json::Object>(dataValue);
                } else if (dataValue.type() == Wt::Json::Type::Array) {
                    result.dataArray = static_cast<Wt::Json::Array>(dataValue);
                }
            }
            
            if (jsonResponse.contains("meta")) {
                result.meta = static_cast<Wt::Json::Object>(jsonResponse.get("meta"));
            }
            
            if (jsonResponse.contains("included")) {
                result.included = static_cast<Wt::Json::Array>(jsonResponse.get("included"));
            }
            
            result.success = true;
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = "JSON parsing error: " + std::string(e.what());
        }
    } else {
        result.success = false;
        result.errorMessage = "Invalid JSON response";
    }
    
    return result;
}

APIClient::APIResponse APIClient::handleError(int statusCode, const std::string& body) {
    APIResponse response;
    response.success = false;
    response.statusCode = statusCode;
    response.errorMessage = body;
    
    debugLog("API Error [" + std::to_string(statusCode) + "]: " + body);
    
    return response;
}

void APIClient::setAuthToken(const std::string& token) {
    authToken_ = token;
    debugLog("Auth token updated");
}

void APIClient::setTimeout(int seconds) {
    timeoutSeconds_ = seconds;
    debugLog("Timeout set to " + std::to_string(seconds) + " seconds");
}

void APIClient::setHeaders(const std::map<std::string, std::string>& headers) {
    defaultHeaders_ = headers;
    debugLog("Custom headers updated");
}

void APIClient::setDebugMode(bool enabled) {
    debugMode_ = enabled;
    if (enabled) {
        std::cout << "[APIClient] Debug mode enabled" << std::endl;
    }
}

void APIClient::debugLog(const std::string& message) {
    if (debugMode_) {
        std::cout << "[APIClient DEBUG] " << message << std::endl;
    }
}

void APIClient::initializeDefaults() {
    // FIXED: Use APIConfiguration static method
    defaultHeaders_ = APIConfiguration::getDefaultHeaders();
}

std::string APIClient::encodeQueryParams(const std::map<std::string, std::string>& params) {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& [key, value] : params) {
        if (!first) oss << "&";
        // FIXED: Use APIConfiguration URL encoding
        oss << APIConfiguration::urlEncode(key) << "=" << APIConfiguration::urlEncode(value);
        first = false;
    }
    
    return oss.str();
}

bool APIClient::isValidJson(const std::string& json) {
    try {
        Wt::Json::Value value;
        Wt::Json::parse(json, value);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
