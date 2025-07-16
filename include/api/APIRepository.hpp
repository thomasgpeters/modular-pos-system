//============================================================================
// include/api/APIRepository.hpp - Base Repository Pattern
//============================================================================

#ifndef APIREPOSITORY_H
#define APIREPOSITORY_H

#include "APIClient.hpp"
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Value.h>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>

/**
 * @class APIRepository
 * @brief Base class for API-based repositories
 * 
 * Provides common CRUD operations and JSON:API handling for all entities.
 * Uses the repository pattern to abstract API communication details.
 */
template<typename T>
class APIRepository {
public:
    /**
     * @brief Constructs repository
     * @param client API client instance
     * @param endpoint Base endpoint for this entity type
     */
    explicit APIRepository(std::shared_ptr<APIClient> client, const std::string& endpoint)
        : client_(client), endpoint_(endpoint) {}
    
    /**
     * @brief Virtual destructor
     */
    virtual ~APIRepository() = default;
    
    // =================================================================
    // CRUD Operations
    // =================================================================
    
    /**
     * @brief Finds all entities
     * @param params Query parameters for filtering/pagination
     * @param callback Callback with results
     */
    virtual void findAll(const std::map<std::string, std::string>& params = {},
                        std::function<void(std::vector<T>, bool success)> callback = nullptr) {
        
        client_->get(endpoint_, params, [this, callback](const APIClient::APIResponse& response) {
            std::vector<T> entities;
            
            if (response.success) {
                // Check if we have array data
                if (!response.dataArray.empty()) {
                    for (const auto& item : response.dataArray) {
                        auto entity = fromJson(item);
                        if (entity) {
                            entities.push_back(*entity);
                        }
                    }
                }
                // Also check if single data object exists
                else if (!response.data.empty()) {
                    auto entity = fromJson(response.data);
                    if (entity) {
                        entities.push_back(*entity);
                    }
                }
            }
            
            if (callback) {
                callback(entities, response.success);
            }
        });
    }
    
    /**
     * @brief Finds entity by ID
     * @param id Entity ID
     * @param callback Callback with result
     */
    virtual void findById(const std::string& id,
                         std::function<void(std::unique_ptr<T>, bool success)> callback = nullptr) {
        
        std::string url = endpoint_ + "/" + id;
        
        client_->get(url, {}, [this, callback](const APIClient::APIResponse& response) {
            std::unique_ptr<T> entity = nullptr;
            
            if (response.success && !response.data.empty()) {
                entity = fromJson(response.data);
            }
            
            if (callback) {
                callback(std::move(entity), response.success);
            }
        });
    }
    
    /**
     * @brief Finds entity by ID (integer version)
     * @param id Entity ID as integer
     * @param callback Callback with result
     */
    virtual void findById(int id,
                         std::function<void(std::unique_ptr<T>, bool success)> callback = nullptr) {
        findById(std::to_string(id), callback);
    }
    
    /**
     * @brief Creates new entity
     * @param entity Entity to create
     * @param callback Callback with created entity
     */
    virtual void create(const T& entity,
                       std::function<void(std::unique_ptr<T>, bool success)> callback = nullptr) {
        
        auto jsonData = toJson(entity);
        
        client_->post(endpoint_, jsonData, [this, callback](const APIClient::APIResponse& response) {
            std::unique_ptr<T> createdEntity = nullptr;
            
            if (response.success && !response.data.empty()) {
                createdEntity = fromJson(response.data);
            }
            
            if (callback) {
                callback(std::move(createdEntity), response.success);
            }
        });
    }
    
    /**
     * @brief Updates existing entity by string ID
     * @param id Entity ID
     * @param entity Updated entity data
     * @param callback Callback with updated entity
     */
    virtual void update(const std::string& id, const T& entity,
                       std::function<void(std::unique_ptr<T>, bool success)> callback = nullptr) {
        
        std::string url = endpoint_ + "/" + id;
        auto jsonData = toJson(entity);
        
        client_->put(url, jsonData, [this, callback](const APIClient::APIResponse& response) {
            std::unique_ptr<T> updatedEntity = nullptr;
            
            if (response.success && !response.data.empty()) {
                updatedEntity = fromJson(response.data);
            }
            
            if (callback) {
                callback(std::move(updatedEntity), response.success);
            }
        });
    }
    
    /**
     * @brief Updates existing entity by integer ID
     * @param id Entity ID as integer
     * @param entity Updated entity data
     * @param callback Callback with updated entity
     */
    virtual void update(int id, const T& entity,
                       std::function<void(std::unique_ptr<T>, bool success)> callback = nullptr) {
        update(std::to_string(id), entity, callback);
    }
    
    /**
     * @brief Deletes entity by string ID
     * @param id Entity ID
     * @param callback Callback with success status
     */
    virtual void delete_(const std::string& id,
                        std::function<void(bool success)> callback = nullptr) {
        
        std::string url = endpoint_ + "/" + id;
        
        client_->delete_(url, [callback](const APIClient::APIResponse& response) {
            if (callback) {
                callback(response.success);
            }
        });
    }
    
    /**
     * @brief Deletes entity by integer ID
     * @param id Entity ID as integer
     * @param callback Callback with success status
     */
    virtual void delete_(int id,
                        std::function<void(bool success)> callback = nullptr) {
        delete_(std::to_string(id), callback);
    }
    
    // =================================================================
    // Utility Methods
    // =================================================================
    
    /**
     * @brief Gets the endpoint for this repository
     * @return Endpoint string
     */
    const std::string& getEndpoint() const { return endpoint_; }
    
    /**
     * @brief Gets the API client
     * @return Shared pointer to API client
     */
    std::shared_ptr<APIClient> getClient() const { return client_; }
    
    /**
     * @brief Checks if the repository is properly initialized
     * @return True if client is available
     */
    bool isInitialized() const { return client_ != nullptr; }

protected:
    /**
     * @brief Converts JSON:API object to entity
     * @param json JSON object to convert (from API response)
     * @return Unique pointer to entity, nullptr if conversion fails
     */
    virtual std::unique_ptr<T> fromJson(const Wt::Json::Value& json) = 0;
    
    /**
     * @brief Converts entity to JSON:API object
     * @param entity Entity to convert
     * @return JSON object in JSON:API format
     */
    virtual Wt::Json::Object toJson(const T& entity) = 0;
    
    /**
     * @brief Helper method to safely get JSON object value
     * @param obj JSON object
     * @param key Key to look for
     * @return JSON value or null value if not found
     */
    Wt::Json::Value safeGetValue(const Wt::Json::Object& obj, const std::string& key) const {
        if (obj.contains(key)) {
            return obj.get(key);
        }
        return Wt::Json::Value();
    }
    
    /**
     * @brief Helper method to safely get string from JSON object
     * @param obj JSON object
     * @param key Key to look for
     * @param defaultValue Default value if key not found
     * @return String value
     */
    std::string safeGetString(const Wt::Json::Object& obj, const std::string& key, 
                             const std::string& defaultValue = "") const {
        auto value = safeGetValue(obj, key);
        if (value.type() == Wt::Json::Type::String) {
            return static_cast<std::string>(value);
        }
        return defaultValue;
    }
    
    /**
     * @brief Helper method to safely get integer from JSON object
     * @param obj JSON object
     * @param key Key to look for
     * @param defaultValue Default value if key not found
     * @return Integer value
     */
    int safeGetInt(const Wt::Json::Object& obj, const std::string& key, 
                   int defaultValue = 0) const {
        auto value = safeGetValue(obj, key);
        if (value.type() == Wt::Json::Type::Number) {
            return static_cast<int>(value);
        }
        return defaultValue;
    }
    
    /**
     * @brief Helper method to safely get double from JSON object
     * @param obj JSON object
     * @param key Key to look for
     * @param defaultValue Default value if key not found
     * @return Double value
     */
    double safeGetDouble(const Wt::Json::Object& obj, const std::string& key, 
                        double defaultValue = 0.0) const {
        auto value = safeGetValue(obj, key);
        if (value.type() == Wt::Json::Type::Number) {
            return static_cast<double>(value);
        }
        return defaultValue;
    }
    
    /**
     * @brief Helper method to safely get boolean from JSON object
     * @param obj JSON object
     * @param key Key to look for
     * @param defaultValue Default value if key not found
     * @return Boolean value
     */
    bool safeGetBool(const Wt::Json::Object& obj, const std::string& key, 
                     bool defaultValue = false) const {
        auto value = safeGetValue(obj, key);
        if (value.type() == Wt::Json::Type::Bool) {
            return static_cast<bool>(value);
        }
        return defaultValue;
    }

private:
    std::shared_ptr<APIClient> client_;
    std::string endpoint_;
};

#endif // APIREPOSITORY_H
