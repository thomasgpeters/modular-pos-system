//============================================================================
// include/api/repositories/EmployeeRepository.hpp
//============================================================================

#ifndef EMPLOYEEREPOSITORY_H
#define EMPLOYEEREPOSITORY_H

#include "../APIRepository.hpp"
#include "../../Employee.hpp"
#include <string>
#include <functional>
#include <vector>
#include <map>

/**
 * @class EmployeeRepository
 * @brief Repository for Employee entity API operations
 * 
 * Handles CRUD operations for Employee entities through the API middleware,
 * providing JSON:API compliant data serialization and deserialization.
 */
class EmployeeRepository : public APIRepository<Employee> {
public:
    /**
     * @brief Constructs repository with API client
     * @param client Shared pointer to API client
     */
    explicit EmployeeRepository(std::shared_ptr<APIClient> client) 
        : APIRepository(client, "/Employee") {}
    
    /**
     * @brief Virtual destructor
     */
    virtual ~EmployeeRepository() = default;
    
    // =================================================================
    // Specialized Query Methods
    // =================================================================
    
    /**
     * @brief Finds employees by role
     * @param role Employee role (e.g., "manager", "server", "cook")
     * @param callback Callback with results
     */
    void findByRole(const std::string& role,
                   std::function<void(std::vector<Employee>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[role]"] = role;
        findAll(params, callback);
    }
    
    /**
     * @brief Finds active employees only
     * @param callback Callback with results
     */
    void findActive(std::function<void(std::vector<Employee>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[active]"] = "true";
        findAll(params, callback);
    }
    
    /**
     * @brief Finds employees by location
     * @param locationId Location identifier
     * @param callback Callback with results
     */
    void findByLocation(const std::string& locationId,
                       std::function<void(std::vector<Employee>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[location_id]"] = locationId;
        findAll(params, callback);
    }
    
    /**
     * @brief Finds employee by employee number
     * @param employeeNumber Unique employee number
     * @param callback Callback with result
     */
    void findByEmployeeNumber(const std::string& employeeNumber,
                             std::function<void(std::unique_ptr<Employee>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[employee_number]"] = employeeNumber;
        
        findAll(params, [callback](std::vector<Employee> employees, bool success) {
            if (success && !employees.empty()) {
                auto employee = std::make_unique<Employee>(employees[0]);
                if (callback) callback(std::move(employee), true);
            } else {
                if (callback) callback(nullptr, false);
            }
        });
    }
    
    /**
     * @brief Finds employees with specific hourly rate range
     * @param minRate Minimum hourly rate
     * @param maxRate Maximum hourly rate
     * @param callback Callback with results
     */
    void findByHourlyRateRange(double minRate, double maxRate,
                              std::function<void(std::vector<Employee>, bool)> callback = nullptr) {
        std::map<std::string, std::string> params;
        params["filter[hourly_rate][gte]"] = std::to_string(minRate);
        params["filter[hourly_rate][lte]"] = std::to_string(maxRate);
        findAll(params, callback);
    }

protected:
    /**
     * @brief Converts JSON:API data to Employee object
     * @param json JSON value from API response
     * @return Unique pointer to Employee or nullptr if conversion fails
     */
    std::unique_ptr<Employee> fromJson(const Wt::Json::Value& json) override {
        // FIXED: Use safe JSON handling instead of direct type checking
        if (json.type() != Wt::Json::Type::Object) {
            return nullptr;
        }
        
        auto jsonObj = static_cast<const Wt::Json::Object&>(json);
        
        // Get the ID from the top level
        std::string employeeId = safeGetString(jsonObj, "id");
        
        // Get attributes object
        auto attributesValue = safeGetValue(jsonObj, "attributes");
        if (attributesValue.type() != Wt::Json::Type::Object) {
            return nullptr;
        }
        
        auto attrs = static_cast<const Wt::Json::Object&>(attributesValue);
        
        // Create employee object
        auto employee = std::make_unique<Employee>();
        
        // Set ID from top level
        employee->setEmployeeId(employeeId);
        
        // FIXED: Use safe getter methods from base class
        employee->setEmployeeNumber(safeGetString(attrs, "employee_number"));
        employee->setFirstName(safeGetString(attrs, "first_name"));
        employee->setLastName(safeGetString(attrs, "last_name"));
        employee->setEmail(safeGetString(attrs, "email"));
        employee->setPhone(safeGetString(attrs, "phone"));
        employee->setRole(safeGetString(attrs, "role"));
        employee->setLocationId(safeGetString(attrs, "location_id"));
        employee->setActive(safeGetBool(attrs, "active", true));
        employee->setHiredDate(safeGetString(attrs, "hired_date"));
        employee->setHourlyRate(safeGetDouble(attrs, "hourly_rate", 0.0));
        employee->setCreatedAt(safeGetString(attrs, "created_at"));
        employee->setUpdatedAt(safeGetString(attrs, "updated_at"));
        
        // Validate the employee before returning
        if (!employee->isValid()) {
            std::cerr << "[EmployeeRepository] Invalid employee data from API for ID: " << employeeId << std::endl;
            return nullptr;
        }
        
        return employee;
    }
    
    /**
     * @brief Converts Employee object to JSON:API format
     * @param employee Employee object to convert
     * @return JSON object in JSON:API format
     */
    Wt::Json::Object toJson(const Employee& employee) override {
        Wt::Json::Object jsonObj;
        
        // JSON:API structure
        jsonObj["type"] = Wt::Json::Value("Employee");
        
        // Include ID if present
        if (!employee.getEmployeeId().empty()) {
            jsonObj["id"] = Wt::Json::Value(employee.getEmployeeId());
        }
        
        // Create attributes object
        Wt::Json::Object attributes;
        attributes["employee_number"] = Wt::Json::Value(employee.getEmployeeNumber());
        attributes["first_name"] = Wt::Json::Value(employee.getFirstName());
        attributes["last_name"] = Wt::Json::Value(employee.getLastName());
        attributes["email"] = Wt::Json::Value(employee.getEmail());
        attributes["phone"] = Wt::Json::Value(employee.getPhone());
        attributes["role"] = Wt::Json::Value(employee.getRole());
        attributes["location_id"] = Wt::Json::Value(employee.getLocationId());
        attributes["active"] = Wt::Json::Value(employee.isActive());
        attributes["hired_date"] = Wt::Json::Value(employee.getHiredDate());
        attributes["hourly_rate"] = Wt::Json::Value(employee.getHourlyRate());
        
        // Include timestamps if present
        if (!employee.getCreatedAt().empty()) {
            attributes["created_at"] = Wt::Json::Value(employee.getCreatedAt());
        }
        if (!employee.getUpdatedAt().empty()) {
            attributes["updated_at"] = Wt::Json::Value(employee.getUpdatedAt());
        }
        
        jsonObj["attributes"] = attributes;
        
        return jsonObj;
    }
};

#endif // EMPLOYEEREPOSITORY_H
