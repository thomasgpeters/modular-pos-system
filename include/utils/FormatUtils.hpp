#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include "../Order.hpp"
#include "../MenuItem.hpp"
#include "../PaymentProcessor.hpp"
#include "../KitchenInterface.hpp"

#include <string>
#include <chrono>
#include <vector>

/**
 * @file FormatUtils.hpp
 * @brief Utility functions for formatting data for display
 * 
 * This header provides comprehensive formatting utilities for all
 * data types used in the POS system, ensuring consistent presentation
 * throughout the user interface.
 * 
 * @author Restaurant POS Team
 * @version 2.0.0
 */

/**
 * @namespace FormatUtils
 * @brief Contains utility functions for data formatting
 */
namespace FormatUtils {
    
    // =================================================================
    // Currency and Number Formatting
    // =================================================================
    
    /**
     * @brief Formats a monetary amount as currency
     * @param amount Amount to format
     * @param currencySymbol Currency symbol (default: $)
     * @param showCents Whether to show cents
     * @return Formatted currency string
     */
    std::string formatCurrency(double amount, const std::string& currencySymbol = "$", bool showCents = true);
    
    /**
     * @brief Formats a percentage
     * @param value Percentage value (0.0 - 1.0 or 0 - 100)
     * @param isDecimal Whether value is decimal (0.15) or percentage (15)
     * @param decimals Number of decimal places
     * @return Formatted percentage string
     */
    std::string formatPercentage(double value, bool isDecimal = true, int decimals = 1);
    
    /**
     * @brief Formats a number with thousand separators
     * @param value Number to format
     * @param decimals Number of decimal places
     * @return Formatted number string
     */
    std::string formatNumber(double value, int decimals = 2);
    
    /**
     * @brief Formats an integer with thousand separators
     * @param value Integer to format
     * @return Formatted number string
     */
    std::string formatInteger(int value);
    
    // =================================================================
    // Date and Time Formatting
    // =================================================================
    
    /**
     * @brief Formats a time point as date string
     * @param timePoint Time point to format
     * @param format Date format string
     * @return Formatted date string
     */
    std::string formatDate(const std::chrono::system_clock::time_point& timePoint, const std::string& format = "%Y-%m-%d");
    
    /**
     * @brief Formats a time point as time string
     * @param timePoint Time point to format
     * @param format Time format string
     * @param use24Hour Use 24-hour format
     * @return Formatted time string
     */
    std::string formatTime(const std::chrono::system_clock::time_point& timePoint, const std::string& format = "", bool use24Hour = false);
    
    /**
     * @brief Formats a time point as date and time string
     * @param timePoint Time point to format
     * @param dateFormat Date format string
     * @param timeFormat Time format string
     * @return Formatted date-time string
     */
    std::string formatDateTime(const std::chrono::system_clock::time_point& timePoint, 
                              const std::string& dateFormat = "%Y-%m-%d", 
                              const std::string& timeFormat = "%H:%M:%S");
    
    /**
     * @brief Formats a duration in human-readable form
     * @param duration Duration to format
     * @param showSeconds Whether to include seconds
     * @return Formatted duration string
     */
    std::string formatDuration(const std::chrono::duration<double>& duration, bool showSeconds = true);
    
    /**
     * @brief Formats elapsed time since a time point
     * @param timePoint Starting time point
     * @param showSeconds Whether to include seconds
     * @return Formatted elapsed time string
     */
    std::string formatElapsedTime(const std::chrono::system_clock::time_point& timePoint, bool showSeconds = true);
    
    /**
     * @brief Formats a relative time (e.g., "2 minutes ago")
     * @param timePoint Time point to format
     * @return Formatted relative time string
     */
    std::string formatRelativeTime(const std::chrono::system_clock::time_point& timePoint);
    
    // =================================================================
    // Order Formatting
    // =================================================================
    
    /**
     * @brief Formats an order summary
     * @param order Order to format
     * @param includeItems Whether to include item details
     * @return Formatted order summary
     */
    std::string formatOrderSummary(const Order& order, bool includeItems = false);
    
    /**
     * @brief Formats an order status
     * @param status Order status to format
     * @return Human-readable status string
     */
    std::string formatOrderStatus(Order::Status status);
    
    /**
     * @brief Formats an order ID for display
     * @param orderId Order ID to format
     * @param prefix ID prefix (default: "Order #")
     * @return Formatted order ID
     */
    std::string formatOrderId(int orderId, const std::string& prefix = "Order #");
    
    /**
     * @brief Formats a table number
     * @param tableNumber Table number to format
     * @param prefix Table prefix (default: "Table ")
     * @return Formatted table number
     */
    std::string formatTableNumber(int tableNumber, const std::string& prefix = "Table ");
    
    /**
     * @brief Formats order totals breakdown
     * @param order Order to format totals for
     * @param showBreakdown Whether to show detailed breakdown
     * @return Formatted totals string
     */
    std::string formatOrderTotals(const Order& order, bool showBreakdown = true);
    
    // =================================================================
    // Menu Item Formatting
    // =================================================================
    
    /**
     * @brief Formats a menu item for display
     * @param item Menu item to format
     * @param includeDescription Whether to include description
     * @param includePrice Whether to include price
     * @return Formatted menu item string
     */
    std::string formatMenuItem(const MenuItem& item, bool includeDescription = false, bool includePrice = true);
    
    /**
     * @brief Formats a menu category
     * @param category Category to format
     * @return Human-readable category string
     */
    std::string formatMenuCategory(MenuItem::Category category);
    
    /**
     * @brief Formats menu item availability
     * @param available Whether item is available
     * @return Formatted availability string
     */
    std::string formatAvailability(bool available);
    
    // =================================================================
    // Payment Formatting
    // =================================================================
    
    /**
     * @brief Formats a payment method
     * @param method Payment method to format
     * @return Human-readable payment method string
     */
    std::string formatPaymentMethod(PaymentProcessor::PaymentMethod method);
    
    /**
     * @brief Formats a payment result
     * @param result Payment result to format
     * @param includeDetails Whether to include transaction details
     * @return Formatted payment result string
     */
    std::string formatPaymentResult(const PaymentProcessor::PaymentResult& result, bool includeDetails = false);
    
    /**
     * @brief Formats a transaction ID
     * @param transactionId Transaction ID to format
     * @param prefix ID prefix (default: "TXN-")
     * @return Formatted transaction ID
     */
    std::string formatTransactionId(const std::string& transactionId, const std::string& prefix = "");
    
    // =================================================================
    // Kitchen Formatting
    // =================================================================
    
    /**
     * @brief Formats kitchen status
     * @param status Kitchen status to format
     * @return Human-readable status string
     */
    std::string formatKitchenStatus(KitchenInterface::KitchenStatus status);
    
    /**
     * @brief Formats a kitchen ticket summary
     * @param ticket Kitchen ticket to format
     * @return Formatted ticket summary
     */
    std::string formatKitchenTicket(const KitchenInterface::KitchenTicket& ticket);
    
    /**
     * @brief Formats estimated wait time
     * @param minutes Wait time in minutes
     * @return Formatted wait time string
     */
    std::string formatWaitTime(int minutes);
    
    // =================================================================
    // List and Collection Formatting
    // =================================================================
    
    /**
     * @brief Formats a list of strings with separators
     * @param items List of items to format
     * @param separator Separator between items
     * @param lastSeparator Separator before last item
     * @return Formatted list string
     */
    std::string formatList(const std::vector<std::string>& items, 
                          const std::string& separator = ", ", 
                          const std::string& lastSeparator = " and ");
    
    /**
     * @brief Formats a count with singular/plural
     * @param count Count value
     * @param singular Singular form
     * @param plural Plural form (if empty, adds 's' to singular)
     * @return Formatted count string
     */
    std::string formatCount(int count, const std::string& singular, const std::string& plural = "");
    
    // =================================================================
    // Address and Contact Formatting
    // =================================================================
    
    /**
     * @brief Formats a phone number
     * @param phoneNumber Phone number to format
     * @param format Phone format (US, international, etc.)
     * @return Formatted phone number
     */
    std::string formatPhoneNumber(const std::string& phoneNumber, const std::string& format = "US");
    
    /**
     * @brief Formats an address
     * @param street Street address
     * @param city City
     * @param state State or province
     * @param zipCode ZIP or postal code
     * @param country Country (optional)
     * @return Formatted address
     */
    std::string formatAddress(const std::string& street, const std::string& city, 
                             const std::string& state, const std::string& zipCode, 
                             const std::string& country = "");
    
    // =================================================================
    // Text Formatting Utilities
    // =================================================================
    
    /**
     * @brief Converts string to title case
     * @param text Text to convert
     * @return Title case string
     */
    std::string toTitleCase(const std::string& text);
    
    /**
     * @brief Converts string to upper case
     * @param text Text to convert
     * @return Upper case string
     */
    std::string toUpperCase(const std::string& text);
    
    /**
     * @brief Converts string to lower case
     * @param text Text to convert
     * @return Lower case string
     */
    std::string toLowerCase(const std::string& text);
    
    /**
     * @brief Truncates text to a maximum length
     * @param text Text to truncate
     * @param maxLength Maximum length
     * @param ellipsis Ellipsis string to append
     * @return Truncated text
     */
    std::string truncateText(const std::string& text, size_t maxLength, const std::string& ellipsis = "...");
    
    /**
     * @brief Pads text to a specific width
     * @param text Text to pad
     * @param width Target width
     * @param padChar Padding character
     * @param alignLeft Align text to left (true) or right (false)
     * @return Padded text
     */
    std::string padText(const std::string& text, size_t width, char padChar = ' ', bool alignLeft = true);
    
    /**
     * @brief Wraps text to fit within a specific width
     * @param text Text to wrap
     * @param width Maximum line width
     * @return Vector of wrapped lines
     */
    std::vector<std::string> wrapText(const std::string& text, size_t width);
    
    // =================================================================
    // Validation and Sanitization
    // =================================================================
    
    /**
     * @brief Sanitizes text for HTML display
     * @param text Text to sanitize
     * @return Sanitized text
     */
    std::string sanitizeHtml(const std::string& text);
    
    /**
     * @brief Escapes special characters for CSV
     * @param text Text to escape
     * @return Escaped text
     */
    std::string escapeCsv(const std::string& text);
    
    /**
     * @brief Removes non-printable characters
     * @param text Text to clean
     * @return Cleaned text
     */
    std::string removeNonPrintable(const std::string& text);
    
    /**
     * @brief Trims whitespace from both ends
     * @param text Text to trim
     * @return Trimmed text
     */
    std::string trim(const std::string& text);
}

#endif // FORMATUTILS_H

