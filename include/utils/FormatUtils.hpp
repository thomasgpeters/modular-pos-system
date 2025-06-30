#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include "../Order.hpp"
#include "../MenuItem.hpp"
#include "../KitchenInterface.hpp"

#include <string>
#include <chrono>
#include <vector>
#include <memory>

/**
 * @file FormatUtils.hpp
 * @brief Formatting utility functions for the Restaurant POS System
 * 
 * This file contains utility functions for formatting various data types
 * including currency, dates, times, and POS-specific data.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @namespace FormatUtils
 * @brief Utility functions for data formatting
 */
namespace FormatUtils {
    
    // =================================================================
    // Currency and Numeric Formatting
    // =================================================================
    
    /**
     * @brief Formats a double value as currency
     * @param amount Amount to format
     * @param currencySymbol Currency symbol (default: "$")
     * @param precision Decimal precision (default: 2)
     * @return Formatted currency string
     */
    std::string formatCurrency(double amount, 
                              const std::string& currencySymbol = "$", 
                              int precision = 2);
    
    /**
     * @brief Formats a percentage value
     * @param value Percentage value (0.15 for 15%)
     * @param precision Decimal precision (default: 1)
     * @return Formatted percentage string
     */
    std::string formatPercentage(double value, int precision = 1);
    
    /**
     * @brief Formats a number with thousands separators
     * @param value Number to format
     * @param separator Thousands separator (default: ",")
     * @return Formatted number string
     */
    std::string formatNumber(double value, const std::string& separator = ",");
    
    /**
     * @brief Formats a number with specified precision
     * @param value Number to format
     * @param precision Decimal places
     * @return Formatted number string
     */
    std::string formatDecimal(double value, int precision);
    
    // =================================================================
    // Date and Time Formatting
    // =================================================================
    
    /**
     * @brief Formats current date and time
     * @param format Format string (e.g., "%Y-%m-%d %H:%M:%S")
     * @return Formatted date/time string
     */
    std::string formatCurrentDateTime(const std::string& format = "%Y-%m-%d %H:%M:%S");
    
    /**
     * @brief Formats current time only
     * @param format Format string (default: "%H:%M:%S")
     * @return Formatted time string
     */
    std::string formatCurrentTime(const std::string& format = "%H:%M:%S");
    
    /**
     * @brief Formats current date only
     * @param format Format string (default: "%Y-%m-%d")
     * @return Formatted date string
     */
    std::string formatCurrentDate(const std::string& format = "%Y-%m-%d");
    
    /**
     * @brief Formats a duration in seconds to human-readable format
     * @param seconds Duration in seconds
     * @param includeSeconds Whether to include seconds in output
     * @return Formatted duration string (e.g., "2h 30m" or "15m 30s")
     */
    std::string formatDuration(int seconds, bool includeSeconds = false);
    
    /**
     * @brief Formats elapsed time since a timestamp
     * @param timestamp Timestamp to calculate from
     * @return Formatted elapsed time string (e.g., "5 minutes ago")
     */
    std::string formatElapsedTime(const std::chrono::steady_clock::time_point& timestamp);
    
    /**
     * @brief Formats time for display (12-hour or 24-hour format)
     * @param hour Hour (0-23)
     * @param minute Minute (0-59)
     * @param use24Hour Whether to use 24-hour format
     * @return Formatted time string
     */
    std::string formatTime(int hour, int minute, bool use24Hour = false);
    
    // =================================================================
    // POS-Specific Formatting
    // =================================================================
    
    /**
     * @brief Formats an order status as display text
     * @param status Order status enum
     * @return Human-readable status string
     */
    std::string formatOrderStatus(Order::Status status);
    
    /**
     * @brief Formats a kitchen status as display text
     * @param status Kitchen status enum
     * @return Human-readable kitchen status string
     */
    std::string formatKitchenStatus(KitchenInterface::KitchenStatus status);
    
    /**
     * @brief Formats a menu category as display text
     * @param category Menu category enum
     * @return Human-readable category string
     */
    std::string formatMenuCategory(MenuItem::Category category);
    
    /**
     * @brief Formats an order summary for display
     * @param order Order to format
     * @param includeItems Whether to include item details
     * @return Formatted order summary
     */
    std::string formatOrderSummary(const std::shared_ptr<Order>& order, 
                                  bool includeItems = false);
    
    /**
     * @brief Formats order items as a list
     * @param order Order containing items
     * @param separator Item separator (default: ", ")
     * @return Formatted items list
     */
    std::string formatOrderItems(const std::shared_ptr<Order>& order,
                                const std::string& separator = ", ");
    
    /**
     * @brief Formats table number with prefix
     * @param tableNumber Table number
     * @param prefix Prefix text (default: "Table ")
     * @return Formatted table string
     */
    std::string formatTableNumber(int tableNumber, const std::string& prefix = "Table ");
    
    /**
     * @brief Formats order ID with prefix
     * @param orderId Order ID
     * @param prefix Prefix text (default: "Order #")
     * @return Formatted order ID string
     */
    std::string formatOrderId(int orderId, const std::string& prefix = "Order #");
    
    // =================================================================
    // Text and String Formatting
    // =================================================================
    
    /**
     * @brief Truncates text to specified length with ellipsis
     * @param text Text to truncate
     * @param maxLength Maximum length
     * @param ellipsis Ellipsis string (default: "...")
     * @return Truncated text
     */
    std::string truncateText(const std::string& text, 
                            size_t maxLength, 
                            const std::string& ellipsis = "...");
    
    /**
     * @brief Capitalizes the first letter of each word
     * @param text Text to capitalize
     * @return Capitalized text
     */
    std::string capitalizeWords(const std::string& text);
    
    /**
     * @brief Converts text to title case
     * @param text Text to convert
     * @return Title case text
     */
    std::string toTitleCase(const std::string& text);
    
    /**
     * @brief Converts text to upper case
     * @param text Text to convert
     * @return Upper case text
     */
    std::string toUpperCase(const std::string& text);
    
    /**
     * @brief Converts text to lower case
     * @param text Text to convert
     * @return Lower case text
     */
    std::string toLowerCase(const std::string& text);
    
    /**
     * @brief Removes leading and trailing whitespace
     * @param text Text to trim
     * @return Trimmed text
     */
    std::string trimWhitespace(const std::string& text);
    
    /**
     * @brief Pluralizes a word based on count
     * @param word Word to pluralize
     * @param count Count to check
     * @param pluralForm Custom plural form (optional)
     * @return Pluralized word
     */
    std::string pluralize(const std::string& word, 
                         int count, 
                         const std::string& pluralForm = "");
    
    // =================================================================
    // List and Collection Formatting
    // =================================================================
    
    /**
     * @brief Joins a vector of strings with separator
     * @param items Items to join
     * @param separator Separator string
     * @return Joined string
     */
    std::string joinStrings(const std::vector<std::string>& items,
                           const std::string& separator = ", ");
    
    /**
     * @brief Formats a list with "and" conjunction
     * @param items Items to format
     * @param oxfordComma Whether to use Oxford comma
     * @return Formatted list (e.g., "A, B, and C")
     */
    std::string formatList(const std::vector<std::string>& items,
                          bool oxfordComma = true);
    
    /**
     * @brief Formats a count with item name
     * @param count Count value
     * @param itemName Item name (singular)
     * @param pluralName Plural form (optional)
     * @return Formatted count string (e.g., "5 items")
     */
    std::string formatCount(int count,
                           const std::string& itemName,
                           const std::string& pluralName = "");
    
    // =================================================================
    // Phone and Address Formatting
    // =================================================================
    
    /**
     * @brief Formats a phone number
     * @param phoneNumber Raw phone number
     * @param format Format pattern (default: "(XXX) XXX-XXXX")
     * @return Formatted phone number
     */
    std::string formatPhoneNumber(const std::string& phoneNumber,
                                 const std::string& format = "(XXX) XXX-XXXX");
    
    /**
     * @brief Formats an address for display
     * @param street Street address
     * @param city City name
     * @param state State/province
     * @param zipCode ZIP/postal code
     * @param country Country (optional)
     * @return Formatted address string
     */
    std::string formatAddress(const std::string& street,
                             const std::string& city,
                             const std::string& state,
                             const std::string& zipCode,
                             const std::string& country = "");
    
    // =================================================================
    // Size and Measurement Formatting
    // =================================================================
    
    /**
     * @brief Formats file size in human-readable format
     * @param bytes Size in bytes
     * @param binary Whether to use binary (1024) or decimal (1000) units
     * @return Formatted size string (e.g., "1.5 MB")
     */
    std::string formatFileSize(long long bytes, bool binary = true);
    
    /**
     * @brief Formats a percentage as a progress indicator
     * @param percentage Percentage (0.0 to 1.0)
     * @param width Width of progress bar in characters
     * @param fillChar Character to use for filled portion
     * @param emptyChar Character to use for empty portion
     * @return ASCII progress bar string
     */
    std::string formatProgressBar(double percentage,
                                 int width = 20,
                                 char fillChar = '█',
                                 char emptyChar = '░');
    
    // =================================================================
    // Validation and Parsing Helpers
    // =================================================================
    
    /**
     * @brief Validates and formats a currency string
     * @param input Input string
     * @param defaultValue Default value if parsing fails
     * @return Parsed and formatted currency value
     */
    double parseCurrency(const std::string& input, double defaultValue = 0.0);
    
    /**
     * @brief Validates if a string is a valid currency format
     * @param input Input string to validate
     * @return True if valid currency format
     */
    bool isValidCurrency(const std::string& input);
    
    /**
     * @brief Validates if a string is a valid phone number
     * @param input Input string to validate
     * @return True if valid phone number format
     */
    bool isValidPhoneNumber(const std::string& input);
    
    // =================================================================
    // HTML and Display Formatting
    // =================================================================
    
    /**
     * @brief Escapes HTML special characters
     * @param text Text to escape
     * @return HTML-escaped text
     */
    std::string escapeHtml(const std::string& text);
    
    /**
     * @brief Converts newlines to HTML line breaks
     * @param text Text to convert
     * @return Text with HTML line breaks
     */
    std::string newlinesToBr(const std::string& text);
    
    /**
     * @brief Formats text for tooltip display
     * @param text Text to format
     * @param maxLineLength Maximum characters per line
     * @return Formatted tooltip text
     */
    std::string formatTooltip(const std::string& text, int maxLineLength = 50);
    
    /**
     * @brief Creates a formatted receipt-style line
     * @param leftText Left-aligned text
     * @param rightText Right-aligned text
     * @param totalWidth Total line width
     * @param fillChar Fill character (default: ' ')
     * @return Formatted receipt line
     */
    std::string formatReceiptLine(const std::string& leftText,
                                 const std::string& rightText,
                                 int totalWidth = 40,
                                 char fillChar = ' ');
}

#endif // FORMATUTILS_H
