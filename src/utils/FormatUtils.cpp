#include "FormatUtils.hpp"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <regex>
#include <cmath>

namespace FormatUtils {

// =================================================================
// Currency and Numeric Formatting
// =================================================================

std::string formatCurrency(double amount, const std::string& currencySymbol, int precision) {
    std::ostringstream oss;
    oss << currencySymbol << std::fixed << std::setprecision(precision) << amount;
    return oss.str();
}

std::string formatPercentage(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << (value * 100.0) << "%";
    return oss.str();
}

std::string formatNumber(double value, const std::string& separator) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << value;
    std::string numStr = oss.str();
    
    // Add thousands separators
    std::string result;
    int count = 0;
    for (auto it = numStr.rbegin(); it != numStr.rend(); ++it) {
        if (count > 0 && count % 3 == 0 && std::isdigit(*it)) {
            result = separator + result;
        }
        result = *it + result;
        if (std::isdigit(*it)) {
            count++;
        }
    }
    
    return result;
}

std::string formatDecimal(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

// =================================================================
// Date and Time Formatting
// =================================================================

std::string formatCurrentDateTime(const std::string& format) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), format.c_str());
    return oss.str();
}

std::string formatCurrentTime(const std::string& format) {
    return formatCurrentDateTime(format);
}

std::string formatCurrentDate(const std::string& format) {
    return formatCurrentDateTime(format);
}

std::string formatDuration(int seconds, bool includeSeconds) {
    if (seconds < 0) {
        return "0s";
    }
    
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int remainingSeconds = seconds % 60;
    
    std::ostringstream oss;
    
    if (hours > 0) {
        oss << hours << "h";
        if (minutes > 0 || (!includeSeconds && remainingSeconds > 0)) {
            oss << " " << minutes << "m";
        }
        if (includeSeconds && remainingSeconds > 0) {
            oss << " " << remainingSeconds << "s";
        }
    } else if (minutes > 0) {
        oss << minutes << "m";
        if (includeSeconds && remainingSeconds > 0) {
            oss << " " << remainingSeconds << "s";
        }
    } else {
        oss << remainingSeconds << "s";
    }
    
    return oss.str();
}

std::string formatElapsedTime(const std::chrono::steady_clock::time_point& timestamp) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count();
    
    if (elapsed < 60) {
        return "just now";
    } else if (elapsed < 3600) {
        int minutes = elapsed / 60;
        return std::to_string(minutes) + " minute" + (minutes > 1 ? "s" : "") + " ago";
    } else if (elapsed < 86400) {
        int hours = elapsed / 3600;
        return std::to_string(hours) + " hour" + (hours > 1 ? "s" : "") + " ago";
    } else {
        int days = elapsed / 86400;
        return std::to_string(days) + " day" + (days > 1 ? "s" : "") + " ago";
    }
}

std::string formatTime(int hour, int minute, bool use24Hour) {
    std::ostringstream oss;
    
    if (use24Hour) {
        oss << std::setfill('0') << std::setw(2) << hour << ":"
            << std::setfill('0') << std::setw(2) << minute;
    } else {
        int displayHour = hour;
        std::string ampm = "AM";
        
        if (hour == 0) {
            displayHour = 12;
        } else if (hour >= 12) {
            ampm = "PM";
            if (hour > 12) {
                displayHour = hour - 12;
            }
        }
        
        oss << displayHour << ":"
            << std::setfill('0') << std::setw(2) << minute << " " << ampm;
    }
    
    return oss.str();
}

// =================================================================
// POS-Specific Formatting
// =================================================================

std::string formatOrderStatus(Order::Status status) {
    switch (status) {
        case Order::Status::PENDING:          return "Pending";
        case Order::Status::SENT_TO_KITCHEN:  return "Sent to Kitchen";
        case Order::Status::IN_PREPARATION:   return "In Preparation";
        case Order::Status::READY_TO_SERVE:   return "Ready to Serve";
        case Order::Status::SERVED:           return "Served";
        case Order::Status::PAID:             return "Paid";
        case Order::Status::COMPLETED:        return "Completed";
        case Order::Status::CANCELLED:        return "Cancelled";
        default:                              return "Unknown";
    }
}

std::string formatKitchenStatus(KitchenInterface::KitchenStatus status) {
    switch (status) {
        case KitchenInterface::ORDER_RECEIVED:  return "Order Received";
        case KitchenInterface::IN_PREPARATION:  return "In Preparation";
        case KitchenInterface::READY_TO_SERVE:  return "Ready to Serve";
        case KitchenInterface::SERVED:          return "Served";
        default:                                return "Unknown";
    }
}

std::string formatMenuCategory(MenuItem::Category category) {
    switch (category) {
        case MenuItem::APPETIZER:   return "Appetizers";
        case MenuItem::MAIN_COURSE: return "Main Courses";
        case MenuItem::DESSERT:     return "Desserts";
        case MenuItem::BEVERAGE:    return "Beverages";
        case MenuItem::SPECIAL:     return "Specials";
        default:                    return "Other";
    }
}

std::string formatOrderSummary(const std::shared_ptr<Order>& order, bool includeItems) {
    if (!order) {
        return "Invalid order";
    }
    
    std::ostringstream oss;
    oss << "Order #" << order->getId() 
        << " for Table " << order->getTableNumber()
        << " - " << formatCurrency(order->getTotal())
        << " (" << formatOrderStatus(order->getStatus()) << ")";
    
    if (includeItems && !order->getItems().empty()) {
        oss << "\nItems: " << formatOrderItems(order);
    }
    
    return oss.str();
}

std::string formatOrderItems(const std::shared_ptr<Order>& order, const std::string& separator) {
    if (!order) {
        return "";
    }
    
    const auto& items = order->getItems();
    std::vector<std::string> itemStrings;
    
    for (const auto& item : items) {
        std::ostringstream itemOss;
        itemOss << item.quantity << "x " << item.menuItem->getName();
        itemStrings.push_back(itemOss.str());
    }
    
    return joinStrings(itemStrings, separator);
}

std::string formatTableNumber(int tableNumber, const std::string& prefix) {
    return prefix + std::to_string(tableNumber);
}

std::string formatOrderId(int orderId, const std::string& prefix) {
    return prefix + std::to_string(orderId);
}

// =================================================================
// Text and String Formatting
// =================================================================

std::string truncateText(const std::string& text, size_t maxLength, const std::string& ellipsis) {
    if (text.length() <= maxLength) {
        return text;
    }
    
    if (maxLength <= ellipsis.length()) {
        return ellipsis.substr(0, maxLength);
    }
    
    return text.substr(0, maxLength - ellipsis.length()) + ellipsis;
}

std::string capitalizeWords(const std::string& text) {
    std::string result = text;
    bool capitalizeNext = true;
    
    for (char& c : result) {
        if (std::isspace(c)) {
            capitalizeNext = true;
        } else if (capitalizeNext) {
            c = std::toupper(c);
            capitalizeNext = false;
        } else {
            c = std::tolower(c);
        }
    }
    
    return result;
}

std::string toTitleCase(const std::string& text) {
    return capitalizeWords(text);
}

std::string toUpperCase(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string toLowerCase(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string trimWhitespace(const std::string& text) {
    size_t start = text.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = text.find_last_not_of(" \t\n\r\f\v");
    return text.substr(start, end - start + 1);
}

std::string pluralize(const std::string& word, int count, const std::string& pluralForm) {
    if (count == 1) {
        return word;
    }
    
    if (!pluralForm.empty()) {
        return pluralForm;
    }
    
    // Simple pluralization rules
    std::string lower = toLowerCase(word);
    
    if (lower.ends_with("y") && lower.length() > 1 && 
        std::string("aeiou").find(lower[lower.length()-2]) == std::string::npos) {
        return word.substr(0, word.length()-1) + "ies";
    } else if (lower.ends_with("s") || lower.ends_with("x") || 
               lower.ends_with("z") || lower.ends_with("ch") || 
               lower.ends_with("sh")) {
        return word + "es";
    } else {
        return word + "s";
    }
}

// =================================================================
// List and Collection Formatting
// =================================================================

std::string joinStrings(const std::vector<std::string>& items, const std::string& separator) {
    if (items.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) {
            oss << separator;
        }
        oss << items[i];
    }
    
    return oss.str();
}

std::string formatList(const std::vector<std::string>& items, bool oxfordComma) {
    if (items.empty()) {
        return "";
    } else if (items.size() == 1) {
        return items[0];
    } else if (items.size() == 2) {
        return items[0] + " and " + items[1];
    } else {
        std::ostringstream oss;
        for (size_t i = 0; i < items.size(); ++i) {
            if (i > 0) {
                if (i == items.size() - 1) {
                    if (oxfordComma) {
                        oss << ", and ";
                    } else {
                        oss << " and ";
                    }
                } else {
                    oss << ", ";
                }
            }
            oss << items[i];
        }
        return oss.str();
    }
}

std::string formatCount(int count, const std::string& itemName, const std::string& pluralName) {
    std::string plural = pluralName.empty() ? pluralize(itemName, count) : pluralName;
    return std::to_string(count) + " " + (count == 1 ? itemName : plural);
}

// =================================================================
// Phone and Address Formatting
// =================================================================

std::string formatPhoneNumber(const std::string& phoneNumber, const std::string& format) {
    // Remove all non-digit characters
    std::string digits;
    for (char c : phoneNumber) {
        if (std::isdigit(c)) {
            digits += c;
        }
    }
    
    // Handle US format (XXX) XXX-XXXX
    if (format == "(XXX) XXX-XXXX" && digits.length() == 10) {
        return "(" + digits.substr(0, 3) + ") " + 
               digits.substr(3, 3) + "-" + digits.substr(6, 4);
    }
    
    // Return original if can't format
    return phoneNumber;
}

std::string formatAddress(const std::string& street, const std::string& city,
                         const std::string& state, const std::string& zipCode,
                         const std::string& country) {
    std::ostringstream oss;
    oss << street;
    
    if (!city.empty()) {
        oss << ", " << city;
    }
    
    if (!state.empty()) {
        oss << ", " << state;
    }
    
    if (!zipCode.empty()) {
        oss << " " << zipCode;
    }
    
    if (!country.empty()) {
        oss << ", " << country;
    }
    
    return oss.str();
}

// =================================================================
// Size and Measurement Formatting
// =================================================================

std::string formatFileSize(long long bytes, bool binary) {
    const double base = binary ? 1024.0 : 1000.0;
    const std::vector<std::string> units = binary ? 
        std::vector<std::string>{"B", "KiB", "MiB", "GiB", "TiB"} :
        std::vector<std::string>{"B", "KB", "MB", "GB", "TB"};
    
    if (bytes == 0) {
        return "0 " + units[0];
    }
    
    double size = static_cast<double>(bytes);
    int unitIndex = 0;
    
    while (size >= base && unitIndex < units.size() - 1) {
        size /= base;
        unitIndex++;
    }
    
    std::ostringstream oss;
    if (unitIndex == 0) {
        oss << static_cast<long long>(size) << " " << units[unitIndex];
    } else {
        oss << std::fixed << std::setprecision(1) << size << " " << units[unitIndex];
    }
    
    return oss.str();
}

std::string formatProgressBar(double percentage, int width, char fillChar, char emptyChar) {
    int filled = static_cast<int>(std::round(percentage * width));
    filled = std::max(0, std::min(width, filled));
    
    std::string result;
    for (int i = 0; i < width; ++i) {
        result += (i < filled) ? fillChar : emptyChar;
    }
    
    return result;
}

// =================================================================
// Validation and Parsing Helpers
// =================================================================

double parseCurrency(const std::string& input, double defaultValue) {
    std::string cleaned = input;
    
    // Remove currency symbols and whitespace
    cleaned = std::regex_replace(cleaned, std::regex("[^0-9.-]"), "");
    
    try {
        return std::stod(cleaned);
    } catch (...) {
        return defaultValue;
    }
}

bool isValidCurrency(const std::string& input) {
    std::regex currencyRegex(R"(^\$?\d+(\.\d{1,2})?$)");
    return std::regex_match(input, currencyRegex);
}

bool isValidPhoneNumber(const std::string& input) {
    std::regex phoneRegex(R"(^\(?([0-9]{3})\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$)");
    return std::regex_match(input, phoneRegex);
}

// =================================================================
// HTML and Display Formatting
// =================================================================

std::string escapeHtml(const std::string& text) {
    std::string result = text;
    
    // Replace HTML special characters
    std::vector<std::pair<std::string, std::string>> replacements = {
        {"&", "&amp;"},
        {"<", "&lt;"},
        {">", "&gt;"},
        {"\"", "&quot;"},
        {"'", "&#x27;"}
    };
    
    for (const auto& replacement : replacements) {
        size_t pos = 0;
        while ((pos = result.find(replacement.first, pos)) != std::string::npos) {
            result.replace(pos, replacement.first.length(), replacement.second);
            pos += replacement.second.length();
        }
    }
    
    return result;
}

std::string newlinesToBr(const std::string& text) {
    std::string result = text;
    size_t pos = 0;
    
    while ((pos = result.find('\n', pos)) != std::string::npos) {
        result.replace(pos, 1, "<br>");
        pos += 4; // Length of "<br>"
    }
    
    return result;
}

std::string formatTooltip(const std::string& text, int maxLineLength) {
    std::string result;
    std::istringstream words(text);
    std::string word;
    int currentLineLength = 0;
    
    while (words >> word) {
        if (currentLineLength + word.length() + 1 > maxLineLength && currentLineLength > 0) {
            result += "\n";
            currentLineLength = 0;
        }
        
        if (currentLineLength > 0) {
            result += " ";
            currentLineLength++;
        }
        
        result += word;
        currentLineLength += word.length();
    }
    
    return result;
}

std::string formatReceiptLine(const std::string& leftText, const std::string& rightText,
                             int totalWidth, char fillChar) {
    int textLength = leftText.length() + rightText.length();
    int fillLength = std::max(0, totalWidth - textLength);
    
    return leftText + std::string(fillLength, fillChar) + rightText;
}

} // namespace FormatUtils
