//============================================================================
// src/api/APIConfiguration.cpp - API Configuration Implementation
//============================================================================

#include "../../include/api/APIConfiguration.hpp"
#include <sstream>
#include <iomanip>
#include <random>
#include <regex>
#include <chrono>

// =================================================================
// Static constant definitions
// =================================================================

const char* APIConfiguration::Defaults::DEFAULT_BASE_URL = "http://localhost:5656/api";
const char* APIConfiguration::Defaults::DEFAULT_AUTH_ENDPOINT = "/auth/login";
const char* APIConfiguration::Defaults::DEFAULT_VERSION = "v1";

const char* APIConfiguration::Headers::CONTENT_TYPE = "Content-Type";
const char* APIConfiguration::Headers::ACCEPT = "Accept";
const char* APIConfiguration::Headers::AUTHORIZATION = "Authorization";
const char* APIConfiguration::Headers::USER_AGENT = "User-Agent";
const char* APIConfiguration::Headers::X_API_KEY = "X-API-Key";
const char* APIConfiguration::Headers::X_REQUEST_ID = "X-Request-ID";
const char* APIConfiguration::Headers::X_CLIENT_VERSION = "X-Client-Version";

const char* APIConfiguration::ContentTypes::JSON = "application/json";
const char* APIConfiguration::ContentTypes::JSON_API = "application/vnd.api+json";
const char* APIConfiguration::ContentTypes::FORM_URLENCODED = "application/x-www-form-urlencoded";
const char* APIConfiguration::ContentTypes::MULTIPART_FORM = "multipart/form-data";

// =================================================================
// Static method implementations
// =================================================================

std::map<std::string, std::string> APIConfiguration::getDefaultHeaders() {
    std::map<std::string, std::string> headers;
    
    headers[Headers::CONTENT_TYPE] = ContentTypes::JSON_API;
    headers[Headers::ACCEPT] = ContentTypes::JSON_API;
    headers[Headers::USER_AGENT] = buildUserAgent();
    headers[Headers::X_CLIENT_VERSION] = "1.0.0";
    headers[Headers::X_REQUEST_ID] = generateRequestId();
    
    return headers;
}

std::string APIConfiguration::buildAuthHeader(const std::string& token) {
    if (token.empty()) {
        return "";
    }
    
    // Check if token already has "Bearer " prefix
    if (token.substr(0, 7) == "Bearer ") {
        return token;
    }
    
    return "Bearer " + token;
}

std::string APIConfiguration::buildUserAgent(const std::string& clientName, 
                                            const std::string& clientVersion) {
    std::ostringstream oss;
    oss << clientName << "/" << clientVersion << " (Restaurant POS System)";
    return oss.str();
}

bool APIConfiguration::isValidApiUrl(const std::string& url) {
    if (url.empty()) {
        return false;
    }
    
    // Basic URL validation using regex
    std::regex urlRegex(R"(^https?://[^\s/$.?#].[^\s]*$)");
    return std::regex_match(url, urlRegex);
}

std::string APIConfiguration::buildApiUrl(const std::string& baseUrl, const std::string& endpoint) {
    std::string result = baseUrl;
    
    // Remove trailing slash from base URL
    if (!result.empty() && result.back() == '/') {
        result.pop_back();
    }
    
    // Add leading slash to endpoint if missing
    std::string cleanEndpoint = endpoint;
    if (!cleanEndpoint.empty() && cleanEndpoint.front() != '/') {
        cleanEndpoint = "/" + cleanEndpoint;
    }
    
    return result + cleanEndpoint;
}

bool APIConfiguration::isSuccessStatus(int statusCode) {
    return statusCode >= 200 && statusCode < 300;
}

bool APIConfiguration::isClientError(int statusCode) {
    return statusCode >= 400 && statusCode < 500;
}

bool APIConfiguration::isServerError(int statusCode) {
    return statusCode >= 500 && statusCode < 600;
}

std::string APIConfiguration::getStatusDescription(int statusCode) {
    switch (statusCode) {
        case StatusCodes::OK:                    return "OK";
        case StatusCodes::CREATED:               return "Created";
        case StatusCodes::NO_CONTENT:            return "No Content";
        case StatusCodes::BAD_REQUEST:           return "Bad Request";
        case StatusCodes::UNAUTHORIZED:          return "Unauthorized";
        case StatusCodes::FORBIDDEN:             return "Forbidden";
        case StatusCodes::NOT_FOUND:             return "Not Found";
        case StatusCodes::CONFLICT:              return "Conflict";
        case StatusCodes::UNPROCESSABLE_ENTITY:  return "Unprocessable Entity";
        case StatusCodes::INTERNAL_SERVER_ERROR: return "Internal Server Error";
        case StatusCodes::BAD_GATEWAY:           return "Bad Gateway";
        case StatusCodes::SERVICE_UNAVAILABLE:   return "Service Unavailable";
        default:
            if (isSuccessStatus(statusCode))      return "Success";
            if (isClientError(statusCode))        return "Client Error";
            if (isServerError(statusCode))        return "Server Error";
            return "Unknown Status";
    }
}

std::string APIConfiguration::generateRequestId() {
    // Generate a simple UUID-like request ID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::ostringstream oss;
    oss << std::hex;
    
    for (int i = 0; i < 8; ++i) {
        oss << dis(gen);
    }
    oss << "-";
    for (int i = 0; i < 4; ++i) {
        oss << dis(gen);
    }
    oss << "-4"; // Version 4 UUID
    for (int i = 0; i < 3; ++i) {
        oss << dis(gen);
    }
    oss << "-";
    oss << std::hex << (8 + (dis(gen) % 4)); // Variant bits
    for (int i = 0; i < 3; ++i) {
        oss << dis(gen);
    }
    oss << "-";
    for (int i = 0; i < 12; ++i) {
        oss << dis(gen);
    }
    
    return oss.str();
}

std::string APIConfiguration::urlEncode(const std::string& value) {
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;
    
    for (char c : value) {
        // Keep alphanumeric and certain safe characters
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            encoded << '%' << std::setw(2) << static_cast<unsigned char>(c);
        }
    }
    
    return encoded.str();
}

std::string APIConfiguration::urlDecode(const std::string& value) {
    std::ostringstream decoded;
    
    for (size_t i = 0; i < value.length(); ++i) {
        if (value[i] == '%' && i + 2 < value.length()) {
            std::string hex = value.substr(i + 1, 2);
            char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
            decoded << decodedChar;
            i += 2; // Skip the hex digits
        } else if (value[i] == '+') {
            decoded << ' '; // Plus signs decode to spaces
        } else {
            decoded << value[i];
        }
    }
    
    return decoded.str();
}
