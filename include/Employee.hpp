//============================================================================
// include/Employee.hpp - Employee Domain Object
//============================================================================

#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include <chrono>
#include <iostream>

/**
 * @class Employee
 * @brief Represents an employee in the restaurant POS system
 * 
 * This class encapsulates all employee-related data and business logic,
 * matching the data structure used in the API middleware.
 */
class Employee {
public:
    /**
     * @brief Employee roles enumeration
     */
    enum class Role {
        MANAGER,
        SERVER,
        COOK,
        CASHIER,
        HOST,
        BUSSER,
        ADMIN,
        OTHER
    };
    
    /**
     * @brief Default constructor
     */
    Employee();
    
    /**
     * @brief Constructs employee with basic information
     * @param employeeNumber Unique employee number
     * @param firstName Employee's first name
     * @param lastName Employee's last name
     * @param role Employee's role
     */
    Employee(const std::string& employeeNumber,
             const std::string& firstName,
             const std::string& lastName,
             const std::string& role);
    
    /**
     * @brief Full constructor
     * @param employeeId Unique employee ID
     * @param employeeNumber Employee number
     * @param firstName First name
     * @param lastName Last name
     * @param email Email address
     * @param phone Phone number
     * @param role Employee role
     * @param locationId Location ID
     * @param active Whether employee is active
     * @param hourlyRate Hourly wage rate
     */
    Employee(const std::string& employeeId,
             const std::string& employeeNumber,
             const std::string& firstName,
             const std::string& lastName,
             const std::string& email,
             const std::string& phone,
             const std::string& role,
             const std::string& locationId,
             bool active,
             double hourlyRate);
    
    /**
     * @brief Copy constructor
     */
    Employee(const Employee& other) = default;
    
    /**
     * @brief Assignment operator
     */
    Employee& operator=(const Employee& other) = default;
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Employee() = default;
    
    // =================================================================
    // Getters
    // =================================================================
    
    const std::string& getEmployeeId() const { return employeeId_; }
    const std::string& getEmployeeNumber() const { return employeeNumber_; }
    const std::string& getFirstName() const { return firstName_; }
    const std::string& getLastName() const { return lastName_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getRole() const { return role_; }
    const std::string& getLocationId() const { return locationId_; }
    bool isActive() const { return active_; }
    const std::string& getHiredDate() const { return hiredDate_; }
    double getHourlyRate() const { return hourlyRate_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getUpdatedAt() const { return updatedAt_; }
    
    // =================================================================
    // Setters
    // =================================================================
    
    void setEmployeeId(const std::string& employeeId) { employeeId_ = employeeId; }
    void setEmployeeNumber(const std::string& employeeNumber) { employeeNumber_ = employeeNumber; }
    void setFirstName(const std::string& firstName) { firstName_ = firstName; }
    void setLastName(const std::string& lastName) { lastName_ = lastName; }
    void setEmail(const std::string& email) { email_ = email; }
    void setPhone(const std::string& phone) { phone_ = phone; }
    void setRole(const std::string& role) { role_ = role; }
    void setLocationId(const std::string& locationId) { locationId_ = locationId; }
    void setActive(bool active) { active_ = active; }
    void setHiredDate(const std::string& hiredDate) { hiredDate_ = hiredDate; }
    void setHourlyRate(double hourlyRate) { hourlyRate_ = hourlyRate; }
    void setCreatedAt(const std::string& createdAt) { createdAt_ = createdAt; }
    void setUpdatedAt(const std::string& updatedAt) { updatedAt_ = updatedAt; }
    
    // =================================================================
    // Business Logic Methods
    // =================================================================
    
    /**
     * @brief Gets full name (first + last)
     * @return Full name as string
     */
    std::string getFullName() const;
    
    /**
     * @brief Gets display name for UI (first name + last initial)
     * @return Display name
     */
    std::string getDisplayName() const;
    
    /**
     * @brief Checks if employee has manager role
     * @return True if manager
     */
    bool isManager() const;
    
    /**
     * @brief Checks if employee has server role
     * @return True if server
     */
    bool isServer() const;
    
    /**
     * @brief Checks if employee has cook role
     * @return True if cook
     */
    bool isCook() const;
    
    /**
     * @brief Checks if employee has cashier role
     * @return True if cashier
     */
    bool isCashier() const;
    
    /**
     * @brief Checks if employee can access admin functions
     * @return True if has admin access
     */
    bool hasAdminAccess() const;
    
    /**
     * @brief Checks if employee can process payments
     * @return True if can process payments
     */
    bool canProcessPayments() const;
    
    /**
     * @brief Checks if employee can modify orders
     * @return True if can modify orders
     */
    bool canModifyOrders() const;
    
    /**
     * @brief Validates employee data
     * @return True if all required fields are valid
     */
    bool isValid() const;
    
    /**
     * @brief Gets role as enum
     * @return Role enum value
     */
    Role getRoleEnum() const;
    
    /**
     * @brief Sets role from enum
     * @param role Role enum value
     */
    void setRoleEnum(Role role);
    
    /**
     * @brief Converts role enum to string
     * @param role Role enum
     * @return Role as string
     */
    static std::string roleToString(Role role);
    
    /**
     * @brief Converts string to role enum
     * @param roleStr Role as string
     * @return Role enum
     */
    static Role stringToRole(const std::string& roleStr);
    
    /**
     * @brief Gets all available roles as strings
     * @return Vector of role strings
     */
    static std::vector<std::string> getAllRoles();
    
    // =================================================================
    // Operators
    // =================================================================
    
    /**
     * @brief Equality operator
     */
    bool operator==(const Employee& other) const;
    
    /**
     * @brief Inequality operator
     */
    bool operator!=(const Employee& other) const;
    
    /**
     * @brief Less than operator (for sorting)
     */
    bool operator<(const Employee& other) const;
    
    // =================================================================
    // Utility Methods
    // =================================================================
    
    /**
     * @brief Prints employee information to console (debug)
     */
    void printInfo() const;
    
    /**
     * @brief Converts employee to string representation
     * @return String representation
     */
    std::string toString() const;
    
    /**
     * @brief Calculates years of service
     * @return Years since hired date
     */
    double getYearsOfService() const;
    
    /**
     * @brief Formats phone number for display
     * @return Formatted phone number
     */
    std::string getFormattedPhone() const;
    
    /**
     * @brief Gets initials (first letter of first and last name)
     * @return Initials string
     */
    std::string getInitials() const;

private:
    // Core employee data (matches EmployeeRepository structure)
    std::string employeeId_;
    std::string employeeNumber_;
    std::string firstName_;
    std::string lastName_;
    std::string email_;
    std::string phone_;
    std::string role_;
    std::string locationId_;
    bool active_;
    std::string hiredDate_;
    double hourlyRate_;
    std::string createdAt_;
    std::string updatedAt_;
    
    // Helper methods
    void initializeDefaults();
    bool isValidEmail(const std::string& email) const;
    bool isValidPhone(const std::string& phone) const;
};

// =================================================================
// Stream operator for easy output
// =================================================================

std::ostream& operator<<(std::ostream& os, const Employee& employee);

#endif // EMPLOYEE_H
