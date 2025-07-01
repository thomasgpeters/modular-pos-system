#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include "../Order.hpp"
#include "../ui/factories/UIComponentFactory.hpp"
#include "../ui/components/OrderEntryPanel.hpp"
#include "../ui/components/OrderStatusPanel.hpp"
#include "../ui/components/MenuDisplay.hpp"
#include "../ui/components/CurrentOrderDisplay.hpp"
#include "../ui/components/ActiveOrdersDisplay.hpp"
#include "../ui/components/KitchenStatusDisplay.hpp"
#include "../ui/components/ThemeSelector.hpp"

#include "../services/POSService.hpp"
#include "../events/EventManager.hpp"
#include "../core/ConfigurationManager.hpp"

#include <string>
#include <sstream>
#include <iomanip>

/**
 * @file FormatUtils.hpp
 * @brief Utility functions for formatting display values
 * 
 * This header provides utility functions for formatting various
 * data types for display in the UI consistently.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @namespace FormatUtils
 * @brief Contains utility functions for formatting display values
 */
namespace FormatUtils {
    
    /**
     * @brief Formats an order ID for display
     * @param orderId Order ID to format
     * @return Formatted order ID string
     */
    inline std::string formatOrderId(int orderId) {
        std::stringstream ss;
        ss << "#" << std::setfill('0') << std::setw(4) << orderId;
        return ss.str();
    }
    
    /**
     * @brief Formats a table number for display
     * @param tableNumber Table number to format
     * @return Formatted table number string
     */
    inline std::string formatTableNumber(int tableNumber) {
        return "Table " + std::to_string(tableNumber);
    }
    
    /**
     * @brief Formats a currency amount for display
     * @param amount Amount to format
     * @return Formatted currency string
     */
    inline std::string formatCurrency(double amount) {
        std::stringstream ss;
        ss << "$" << std::fixed << std::setprecision(2) << amount;
        return ss.str();
    }
    
    /**
     * @brief Formats an order status for display
     * @param status Order status to format
     * @return Formatted status string
     */
    inline std::string formatOrderStatus(Order::Status status) {
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
    
    /**
     * @brief Formats a quantity for display
     * @param quantity Quantity to format
     * @return Formatted quantity string
     */
    inline std::string formatQuantity(int quantity) {
        return std::to_string(quantity) + "x";
    }
    
    /**
     * @brief Formats a percentage for display
     * @param percentage Percentage to format (0-1 range)
     * @return Formatted percentage string
     */
    inline std::string formatPercentage(double percentage) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << (percentage * 100) << "%";
        return ss.str();
    }
    
    /**
     * @brief Formats a time duration in minutes
     * @param minutes Duration in minutes
     * @return Formatted duration string
     */
    inline std::string formatDuration(int minutes) {
        if (minutes < 60) {
            return std::to_string(minutes) + " min";
        } else {
            int hours = minutes / 60;
            int remainingMinutes = minutes % 60;
            return std::to_string(hours) + "h " + std::to_string(remainingMinutes) + "m";
        }
    }
    
    /**
     * @brief Formats a date/time for display
     * @param timestamp Timestamp to format
     * @return Formatted date/time string
     */
    inline std::string formatDateTime(const std::chrono::system_clock::time_point& timestamp) {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M");
        return ss.str();
    }
    
    /**
     * @brief Formats a time for display (time only)
     * @param timestamp Timestamp to format
     * @return Formatted time string
     */
    inline std::string formatTime(const std::chrono::system_clock::time_point& timestamp) {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%H:%M");
        return ss.str();
    }
    
    /**
     * @brief Formats a phone number for display
     * @param phoneNumber Phone number to format
     * @return Formatted phone number string
     */
    inline std::string formatPhoneNumber(const std::string& phoneNumber) {
        // Simple formatting for US phone numbers
        if (phoneNumber.length() == 10) {
            return "(" + phoneNumber.substr(0, 3) + ") " + 
                   phoneNumber.substr(3, 3) + "-" + 
                   phoneNumber.substr(6, 4);
        }
        return phoneNumber; // Return as-is if not standard format
    }
    
    /**
     * @brief Truncates text to a maximum length with ellipsis
     * @param text Text to truncate
     * @param maxLength Maximum length before truncation
     * @return Truncated text with ellipsis if needed
     */
    inline std::string truncateText(const std::string& text, size_t maxLength) {
        if (text.length() <= maxLength) {
            return text;
        }
        return text.substr(0, maxLength - 3) + "...";
    }
    
    /**
     * @brief Capitalizes the first letter of each word
     * @param text Text to capitalize
     * @return Capitalized text
     */
    inline std::string capitalizeWords(const std::string& text) {
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

#endif // FORMATUTILS_H
