//============================================================================
// include/api/APIClient.hpp - HTTP Client for Middleware Communication
//============================================================================

#ifndef APICLIENT_H
#define APICLIENT_H

#include "APIConfiguration.hpp"

#include <Wt/Http/Client.h>
#include <Wt/Http/Message.h>
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Value.h>
#include <Wt/Json/Parser.h>

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <future>

/**
 * @class APIClient
 * @brief HTTP client for communicating with the middleware API
 * 
 * Handles all HTTP communication with the JSON:API middleware including
 * authentication, request/response formatting, and error handling.
 */
class APIClient {
public:
    /**
     * @struct APIResponse
     * @brief Response structure for API calls
     */
    struct APIResponse {
        bool success;
        int statusCode;
        std::string errorMessage;
        Wt::Json::Object data;
        Wt::Json::Array dataArray;
        Wt::Json::Object meta;
        Wt::Json::Array included;
        
        APIResponse() : success(false), statusCode(0) {}
    };
    
    /**
     * @brief Response callback type
     */
    using ResponseCallback = std::function<void(const APIResponse&)>;
    
    /**
     * @brief Constructs API client
     * @param baseUrl Base URL for the API (e.g., "http://localhost:5656/api")
     */
    explicit APIClient(const std::string& baseUrl);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~APIClient() = default;
    
    // =================================================================
    // HTTP Methods
    // =================================================================
    
    /**
     * @brief Performs GET request
     * @param endpoint API endpoint (e.g., "/Employee")
     * @param params Query parameters
     * @param callback Response callback
     */
    void get(const std::string& endpoint, 
             const std::map<std::string, std::string>& params = {},
             ResponseCallback callback = nullptr);
    
    /**
     * @brief Performs POST request
     * @param endpoint API endpoint
     * @param data Request body data
     * @param callback Response callback
     */
    void post(const std::string& endpoint,
              const Wt::Json::Object& data,
              ResponseCallback callback = nullptr);
    
    /**
     * @brief Performs PUT request
     * @param endpoint API endpoint
     * @param data Request body data
     * @param callback Response callback
     */
    void put(const std::string& endpoint,
             const Wt::Json::Object& data,
             ResponseCallback callback = nullptr);
    
    /**
     * @brief Performs PATCH request
     * @param endpoint API endpoint
     * @param data Request body data
     * @param callback Response callback
     */
    void patch(const std::string& endpoint,
               const Wt::Json::Object& data,
               ResponseCallback callback = nullptr);
    
    /**
     * @brief Performs DELETE request
     * @param endpoint API endpoint
     * @param callback Response callback
     */
    void delete_(const std::string& endpoint,
                ResponseCallback callback = nullptr);
    
    // =================================================================
    // Synchronous Methods (for backward compatibility)
    // =================================================================
    
    /**
     * @brief Synchronous GET request
     * @param endpoint API endpoint
     * @param params Query parameters
     * @return API response
     */
    APIResponse getSync(const std::string& endpoint,
                       const std::map<std::string, std::string>& params = {});
    
    /**
     * @brief Synchronous POST request
     * @param endpoint API endpoint
     * @param data Request body data
     * @return API response
     */
    APIResponse postSync(const std::string& endpoint,
                        const Wt::Json::Object& data);
    
    // =================================================================
    // Configuration
    // =================================================================
    
    /**
     * @brief Sets authentication token
     * @param token Bearer token for API authentication
     */
    void setAuthToken(const std::string& token);
    
    /**
     * @brief Sets request timeout
     * @param seconds Timeout in seconds
     */
    void setTimeout(int seconds);
    
    /**
     * @brief Sets custom headers
     * @param headers Map of header name to value
     */
    void setHeaders(const std::map<std::string, std::string>& headers);
    
    /**
     * @brief Enables/disables debug logging
     * @param enabled True to enable debug output
     */
    void setDebugMode(bool enabled);

protected:
    /**
     * @brief Builds full URL from endpoint and parameters
     * @param endpoint API endpoint
     * @param params Query parameters
     * @return Complete URL
     */
    std::string buildUrl(const std::string& endpoint,
                        const std::map<std::string, std::string>& params = {});
    
    /**
     * @brief Adds authentication headers to message
     * @param message HTTP message to modify
     */
    void addAuthHeaders(Wt::Http::Message& message);
    
    /**
     * @brief Parses JSON:API response
     * @param response Raw HTTP response
     * @return Parsed API response
     */
    APIResponse parseResponse(const Wt::Http::Message& response);
    
    /**
     * @brief Handles HTTP errors
     * @param statusCode HTTP status code
     * @param body Response body
     * @return Error response
     */
    APIResponse handleError(int statusCode, const std::string& body);
    
    /**
     * @brief Logs debug information
     * @param message Debug message
     */
    void debugLog(const std::string& message);

private:
    std::string baseUrl_;
    std::string authToken_;
    int timeoutSeconds_;
    std::map<std::string, std::string> defaultHeaders_;
    bool debugMode_;
    
    std::unique_ptr<Wt::Http::Client> httpClient_;
    
    // Helper methods
    void initializeDefaults();
    std::string encodeQueryParams(const std::map<std::string, std::string>& params);
    bool isValidJson(const std::string& json);
};

#endif // APICLIENT_H
