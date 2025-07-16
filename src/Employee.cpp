//============================================================================
// src/Employee.cpp - Employee Domain Object Implementation
//============================================================================

#include "../include/Employee.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <regex>

Employee::Employee() {
    initializeDefaults();
}

Employee::Employee(const std::string& employeeNumber,
                   const std::string& firstName,
                   const std::string& lastName,
                   const std::string& role)
    : employeeNumber_(employeeNumber), firstName_(firstName), 
      lastName_(lastName), role_(role) {
    
    initializeDefaults();
}

Employee::Employee(const std::string& employeeId,
                   const std::string& employeeNumber,
                   const std::string& firstName,
                   const std::string& lastName,
                   const std::string& email,
                   const std::string& phone,
                   const std::string& role,
                   const std::string& locationId,
                   bool active,
                   double hourlyRate)
    : employeeId_(employeeId), employeeNumber_(employeeNumber),
      firstName_(firstName), lastName_(lastName), email_(email),
      phone_(phone), role_(role), locationId_(locationId),
      active_(active), hourlyRate_(hourlyRate) {
    
    // Set timestamps
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    if (createdAt_.empty()) {
        createdAt_ = ss.str();
    }
    if (updatedAt_.empty()) {
        updatedAt_ = ss.str();
    }
    if (hiredDate_.empty()) {
        hiredDate_ = ss.str();
    }
}

// =================================================================
// Business Logic Methods
// =================================================================

std::string Employee::getFullName() const {
    return firstName_ + " " + lastName_;
}

std::string Employee::getDisplayName() const {
    if (firstName_.empty()) return lastName_;
    if (lastName_.empty()) return firstName_;
    
    return firstName_ + " " + lastName_.substr(0, 1) + ".";
}

bool Employee::isManager() const {
    std::string lowerRole = role_;
    std::transform(lowerRole.begin(), lowerRole.end(), lowerRole.begin(), ::tolower);
    return lowerRole == "manager" || lowerRole == "admin";
}

bool Employee::isServer() const {
    std::string lowerRole = role_;
    std::transform(lowerRole.begin(), lowerRole.end(), lowerRole.begin(), ::tolower);
    return lowerRole == "server" || lowerRole == "waiter" || lowerRole == "waitress";
}

bool Employee::isCook() const {
    std::string lowerRole = role_;
    std::transform(lowerRole.begin(), lowerRole.end(), lowerRole.begin(), ::tolower);
    return lowerRole == "cook" || lowerRole == "chef" || lowerRole == "kitchen";
}

bool Employee::isCashier() const {
    std::string lowerRole = role_;
    std::transform(lowerRole.begin(), lowerRole.end(), lowerRole.begin(), ::tolower);
    return lowerRole == "cashier" || lowerRole == "register";
}

bool Employee::hasAdminAccess() const {
    return isManager() || getRoleEnum() == Role::ADMIN;
}

bool Employee::canProcessPayments() const {
    return isCashier() || isManager() || isServer();
}

bool Employee::canModifyOrders() const {
    return isServer() || isManager() || isCashier();
}

bool Employee::isValid() const {
    // Check required fields
    if (employeeNumber_.empty() || firstName_.empty() || lastName_.empty()) {
        return false;
    }
    
    // Validate email format if provided
    if (!email_.empty() && !isValidEmail(email_)) {
        return false;
    }
    
    // Validate phone format if provided
    if (!phone_.empty() && !isValidPhone(phone_)) {
        return false;
    }
    
    // Check hourly rate is non-negative
    if (hourlyRate_ < 0.0) {
        return false;
    }
    
    return true;
}

Employee::Role Employee::getRoleEnum() const {
    return stringToRole(role_);
}

void Employee::setRoleEnum(Role role) {
    role_ = roleToString(role);
}

std::string Employee::roleToString(Role role) {
    switch (role) {
        case Role::MANAGER:  return "manager";
        case Role::SERVER:   return "server";
        case Role::COOK:     return "cook";
        case Role::CASHIER:  return "cashier";
        case Role::HOST:     return "host";
        case Role::BUSSER:   return "busser";
        case Role::ADMIN:    return "admin";
        case Role::OTHER:    return "other";
        default:             return "other";
    }
}

Employee::Role Employee::stringToRole(const std::string& roleStr) {
    std::string lowerRole = roleStr;
    std::transform(lowerRole.begin(), lowerRole.end(), lowerRole.begin(), ::tolower);
    
    if (lowerRole == "manager")  return Role::MANAGER;
    if (lowerRole == "server" || lowerRole == "waiter" || lowerRole == "waitress") return Role::SERVER;
    if (lowerRole == "cook" || lowerRole == "chef")     return Role::COOK;
    if (lowerRole == "cashier")  return Role::CASHIER;
    if (lowerRole == "host")     return Role::HOST;
    if (lowerRole == "busser")   return Role::BUSSER;
    if (lowerRole == "admin")    return Role::ADMIN;
    
    return Role::OTHER;
}

std::vector<std::string> Employee::getAllRoles() {
    return {
        "manager", "server", "cook", "cashier", 
        "host", "busser", "admin", "other"
    };
}

// =================================================================
// Operators
// =================================================================

bool Employee::operator==(const Employee& other) const {
    return employeeId_ == other.employeeId_ || 
           employeeNumber_ == other.employeeNumber_;
}

bool Employee::operator!=(const Employee& other) const {
    return !(*this == other);
}

bool Employee::operator<(const Employee& other) const {
    // Sort by last name, then first name
    if (lastName_ != other.lastName_) {
        return lastName_ < other.lastName_;
    }
    return firstName_ < other.firstName_;
}

// =================================================================
// Utility Methods
// =================================================================

void Employee::printInfo() const {
    std::cout << "Employee Information:" << std::endl;
    std::cout << "  ID: " << employeeId_ << std::endl;
    std::cout << "  Number: " << employeeNumber_ << std::endl;
    std::cout << "  Name: " << getFullName() << std::endl;
    std::cout << "  Email: " << email_ << std::endl;
    std::cout << "  Phone: " << getFormattedPhone() << std::endl;
    std::cout << "  Role: " << role_ << std::endl;
    std::cout << "  Location: " << locationId_ << std::endl;
    std::cout << "  Active: " << (active_ ? "Yes" : "No") << std::endl;
    std::cout << "  Hourly Rate: $" << std::fixed << std::setprecision(2) << hourlyRate_ << std::endl;
    std::cout << "  Hired: " << hiredDate_ << std::endl;
}

std::string Employee::toString() const {
    std::ostringstream oss;
    oss << "Employee{id=" << employeeId_ 
        << ", number=" << employeeNumber_
        << ", name=" << getFullName()
        << ", role=" << role_
        << ", active=" << (active_ ? "true" : "false")
        << "}";
    return oss.str();
}

double Employee::getYearsOfService() const {
    if (hiredDate_.empty()) return 0.0;
    
    // Simple calculation - in real app, parse the date properly
    
    // unused variables
    // auto now = std::chrono::system_clock::now();
    // auto nowTime = std::chrono::system_clock::to_time_t(now);
    
    // For demo, return a placeholder calculation
    // In real implementation, parse hiredDate_ and calculate difference
    return 1.0; // Placeholder
}

std::string Employee::getFormattedPhone() const {
    if (phone_.empty()) return "";
    
    // Remove all non-digits
    std::string digits;
    for (char c : phone_) {
        if (std::isdigit(c)) {
            digits += c;
        }
    }
    
    // Format as (XXX) XXX-XXXX if 10 digits
    if (digits.length() == 10) {
        return "(" + digits.substr(0, 3) + ") " + 
               digits.substr(3, 3) + "-" + 
               digits.substr(6, 4);
    }
    
    // Return original if not standard format
    return phone_;
}

std::string Employee::getInitials() const {
    std::string initials;
    
    if (!firstName_.empty()) {
        initials += std::toupper(firstName_[0]);
    }
    
    if (!lastName_.empty()) {
        initials += std::toupper(lastName_[0]);
    }
    
    return initials;
}

// =================================================================
// Private Helper Methods
// =================================================================

void Employee::initializeDefaults() {
    active_ = true;
    hourlyRate_ = 0.0;
    
    // Set current timestamp if not already set
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    if (createdAt_.empty()) {
        createdAt_ = ss.str();
    }
    if (updatedAt_.empty()) {
        updatedAt_ = ss.str();
    }
    if (hiredDate_.empty()) {
        hiredDate_ = ss.str();
    }
}

bool Employee::isValidEmail(const std::string& email) const {
    // Simple email validation using regex
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

bool Employee::isValidPhone(const std::string& phone) const {
    // Allow various phone formats - just check for digits
    std::string digits;
    for (char c : phone) {
        if (std::isdigit(c)) {
            digits += c;
        }
    }
    
    // US phone numbers have 10 digits
    return digits.length() == 10;
}

// =================================================================
// Stream Operator
// =================================================================

std::ostream& operator<<(std::ostream& os, const Employee& employee) {
    os << employee.toString();
    return os;
}
