#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include "../Order.hpp"

#include <string>
#include <chrono>

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
    std::string formatOrderId(int orderId);
    
    /**
     * @brief Formats a table number for display
     * @param tableNumber Table number to format
     * @return Formatted table number string
     */
    std::string formatTableNumber(int tableNumber);
    
    /**
     * @brief Formats a currency amount for display
     * @param amount Amount to format
     * @return Formatted currency string
     */
    std::string formatCurrency(double amount);
    
    /**
     * @brief Formats an order status for display
     * @param status Order status to format
     * @return Formatted status string
     */
    std::string formatOrderStatus(Order::Status status);
    
    /**
     * @brief Formats a quantity for display
     * @param quantity Quantity to format
     * @return Formatted quantity string
     */
    std::string formatQuantity(int quantity);
    
    /**
     * @brief Formats a percentage for display
     * @param percentage Percentage to format (0-1 range)
     * @return Formatted percentage string
     */
    std::string formatPercentage(double percentage);
    
    /**
     * @brief Formats a time duration in minutes
     * @param minutes Duration in minutes
     * @return Formatted duration string
     */
    std::string formatDuration(int minutes);
    
    /**
     * @brief Formats a date/time for display
     * @param timestamp Timestamp to format
     * @return Formatted date/time string
     */
    std::string formatDateTime(const std::chrono::system_clock::time_point& timestamp);
    
    /**
     * @brief Formats a time for display (time only)
     * @param timestamp Timestamp to format
     * @return Formatted time string
     */
    std::string formatTime(const std::chrono::system_clock::time_point& timestamp);
    
    /**
     * @brief Formats a phone number for display
     * @param phoneNumber Phone number to format
     * @return Formatted phone number string
     */
    std::string formatPhoneNumber(const std::string& phoneNumber);
    
    /**
     * @brief Truncates text to a maximum length with ellipsis
     * @param text Text to truncate
     * @param maxLength Maximum length before truncation
     * @return Truncated text with ellipsis if needed
     */
    std::string truncateText(const std::string& text, size_t maxLength);
    
    /**
     * @brief Capitalizes the first letter of each word
     * @param text Text to capitalize
     * @return Capitalized text
     */
    std::string capitalizeWords(const std::string& text);
}

#endif // FORMATUTILS_H