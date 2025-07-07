//============================================================================
// src/utils/FormatUtils.cpp - FIXED (Clean implementations)
//============================================================================

#include "../../include/utils/FormatUtils.hpp"

#include <sstream>
#include <iomanip>
#include <ctime>
#include <cctype>

namespace FormatUtils {
    
    std::string formatOrderId(int orderId) {
        std::stringstream ss;
        ss << "#" << std::setfill('0') << std::setw(4) << orderId;
        return ss.str();
    }
    
    std::string formatTableNumber(int tableNumber) {
        return "Table " + std::to_string(tableNumber);
    }
    
    std::string formatCurrency(double amount) {
        std::stringstream ss;
        ss << "$" << std::fixed << std::setprecision(2) << amount;
        return ss.str();
    }
    
    std::string formatOrderStatus(Order::Status status) {
        switch (status) {
            case Order::PENDING:         return "Pending";
            case Order::SENT_TO_KITCHEN: return "In Kitchen";
            case Order::PREPARING:       return "Preparing";
            case Order::READY:           return "Ready";
            case Order::SERVED:          return "Served";
            case Order::CANCELLED:       return "Cancelled";
            default:                     return "Unknown";
        }
    }
    
    std::string formatQuantity(int quantity) {
        return std::to_string(quantity) + "x";
    }
    
    std::string formatPercentage(double percentage) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << (percentage * 100) << "%";
        return ss.str();
    }
    
    std::string formatDuration(int minutes) {
        if (minutes < 60) {
            return std::to_string(minutes) + " min";
        } else {
            int hours = minutes / 60;
            int remainingMinutes = minutes % 60;
            return std::to_string(hours) + "h " + std::to_string(remainingMinutes) + "m";
        }
    }
    
    std::string formatDateTime(const std::chrono::system_clock::time_point& timestamp) {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M");
        return ss.str();
    }
    
    std::string formatTime(const std::chrono::system_clock::time_point& timestamp) {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%H:%M");
        return ss.str();
    }
    
    std::string formatPhoneNumber(const std::string& phoneNumber) {
        // Simple formatting for US phone numbers
        if (phoneNumber.length() == 10) {
            return "(" + phoneNumber.substr(0, 3) + ") " + 
                   phoneNumber.substr(3, 3) + "-" + 
                   phoneNumber.substr(6, 4);
        }
        return phoneNumber; // Return as-is if not standard format
    }
    
    std::string truncateText(const std::string& text, size_t maxLength) {
        if (text.length() <= maxLength) {
            return text;
        }
        return text.substr(0, maxLength - 3) + "...";
    }
    
    std::string capitalizeWords(const std::string& text) {
        std::string result = text;
        bool capitalizeNext = true;
        
        for (char& c : result) {
            if (std::isalpha(c)) {
                if (capitalizeNext) {
                    c = std::toupper(c);
                    capitalizeNext = false;
                } else {
                    c = std::tolower(c);
                }
            } else {
                capitalizeNext = true;
            }
        }
        
        return result;
    }
}
