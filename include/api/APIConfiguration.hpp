//============================================================================
// include/api/APIConfiguration.hpp - API Configuration Constants and Utilities
//============================================================================

#ifndef APICONFIGURATION_H
#define APICONFIGURATION_H

#include <string>
#include <map>

/**
 * @class APIConfiguration
 * @brief Configuration constants and utilities for API communication
 * 
 * Provides default values, headers, and utility methods for API clients
 * to ensure consistent configuration across the application.
 */
class APIConfiguration {
public:
    /**
     * @struct Defaults
     * @brief Default configuration values
     */
    struct Defaults {
        static constexpr int API_TIMEOUT = 30;              ///< Default timeout in seconds
        static constexpr int MAX_RETRIES = 3;               ///< Default max retry attempts
        static constexpr int RETRY_DELAY_MS = 1000;         ///< Default retry delay in milliseconds
        static constexpr bool DEBUG_MODE = false;           ///< Default debug mode setting
        static constexpr bool ENABLE_CACHING = true;        ///< Default caching setting
        static constexpr int CACHE_TIMEOUT_MINUTES = 5;     ///< Default cache timeout
        
        // Default URLs
        static const char* DEFAULT_BASE_URL;                 ///< Default API base URL
        static const char* DEFAULT_AUTH_ENDPOINT;            ///< Default authentication endpoint
        static const char* DEFAULT_VERSION;                 ///< Default API version
    };
    
    /**
     * @struct Headers
     * @brief Standard HTTP header names
     */
    struct Headers {
        static const char* CONTENT_TYPE;                     ///< Content-Type header
        static const char* ACCEPT;                          ///< Accept header
        static const char* AUTHORIZATION;                   ///< Authorization header
        static const char* USER_AGENT;                     ///< User-Agent header
        static const char* X_API_KEY;                       ///< API Key header
        static const char* X_REQUEST_ID;                    ///< Request ID header
        static const char* X_CLIENT_VERSION;                ///< Client version header
    };
    
    /**
     * @struct ContentTypes
     * @brief Standard content type values
     */
    struct ContentTypes {
        static const char* JSON;                            ///< JSON content type
        static const char* JSON_API;                        ///< JSON:API content type
        static const char* FORM_URLENCODED;                 ///< Form URL encoded content type
        static const char* MULTIPART_FORM;                  ///< Multipart form content type
    };
    
    /**
     * @struct StatusCodes
     * @brief HTTP status code constants
     */
    struct StatusCodes {
        static constexpr int OK = 200;                       ///< Success
        static constexpr int CREATED = 201;                  ///< Created
        static constexpr int NO_CONTENT = 204;               ///< No Content
        static constexpr int BAD_REQUEST = 400;              ///< Bad Request
        static constexpr int UNAUTHORIZED = 401;             ///< Unauthorized
        static constexpr int FORBIDDEN = 403;                ///< Forbidden
        static constexpr int NOT_FOUND = 404;                ///< Not Found
        static constexpr int CONFLICT = 409;                 ///< Conflict
        static constexpr int UNPROCESSABLE_ENTITY = 422;     ///< Unprocessable Entity
        static constexpr int INTERNAL_SERVER_ERROR = 500;    ///< Internal Server Error
        static constexpr int BAD_GATEWAY = 502;              ///< Bad Gateway
        static constexpr int SERVICE_UNAVAILABLE = 503;      ///< Service Unavailable
    };
    
    // =================================================================
    // Static Utility Methods
    // =================================================================
    
    /**
     * @brief Gets default HTTP headers for API requests
     * @return Map of header name to value
     */
    static std::map<std::string, std::string> getDefaultHeaders();
    
    /**
     * @brief Builds authorization header value
     * @param token Bearer token
     * @return Authorization header value
     */
    static std::string buildAuthHeader(const std::string& token);
    
    /**
     * @brief Builds User-Agent header value
     * @param clientName Client application name
     * @param clientVersion Client version
     * @return User-Agent header value
     */
    static std::string buildUserAgent(const std::string& clientName = "RestaurantPOS", 
                                     const std::string& clientVersion = "1.0.0");
    
    /**
     * @brief Validates API URL format
     * @param url URL to validate
     * @return True if URL is valid
     */
    static bool isValidApiUrl(const std::string& url);
    
    /**
     * @brief Builds full API URL from base URL and endpoint
     * @param baseUrl Base API URL
     * @param endpoint API endpoint
     * @return Complete API URL
     */
    static std::string buildApiUrl(const std::string& baseUrl, const std::string& endpoint);
    
    /**
     * @brief Checks if HTTP status code indicates success
     * @param statusCode HTTP status code
     * @return True if status indicates success (2xx)
     */
    static bool isSuccessStatus(int statusCode);
    
    /**
     * @brief Checks if HTTP status code indicates client error
     * @param statusCode HTTP status code
     * @return True if status indicates client error (4xx)
     */
    static bool isClientError(int statusCode);
    
    /**
     * @brief Checks if HTTP status code indicates server error
     * @param statusCode HTTP status code
     * @return True if status indicates server error (5xx)
     */
    static bool isServerError(int statusCode);
    
    /**
     * @brief Gets human-readable description of HTTP status code
     * @param statusCode HTTP status code
     * @return Status description
     */
    static std::string getStatusDescription(int statusCode);
    
    /**
     * @brief Generates a unique request ID
     * @return Unique request ID string
     */
    static std::string generateRequestId();
    
    /**
     * @brief URL encodes a string for safe use in URLs
     * @param value String to encode
     * @return URL encoded string
     */
    static std::string urlEncode(const std::string& value);
    
    /**
     * @brief URL decodes a string
     * @param value String to decode
     * @return URL decoded string
     */
    static std::string urlDecode(const std::string& value);

private:
    // Private constructor - this is a utility class
    APIConfiguration() = delete;
};

#endif // APICONFIGURATION_H
