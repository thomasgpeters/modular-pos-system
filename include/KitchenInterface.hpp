#ifndef KITCHENINTERFACE_H
#define KITCHENINTERFACE_H

#include "../include/Order.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Value.h>

/**
 * @file KitchenInterface.h
 * @brief Kitchen communication system for the Restaurant POS
 * 
 * This file contains the KitchenInterface class which manages real-time
 * communication with kitchen display systems and staff. Part of the three-legged
 * foundation of the POS system (Order Management, Payment Processing, Kitchen Interface).
 * 
 * @author Restaurant POS Team
 * @version 1.0.0
 */

/**
 * @class KitchenInterface
 * @brief Manages real-time communication with kitchen systems (LEG 3 of POS Foundation)
 * 
 * The KitchenInterface class handles communication between the POS system and kitchen
 * display systems, order tracking, and kitchen workflow management. It provides
 * real-time updates, queue management, and integration points for kitchen equipment.
 */
class KitchenInterface {
public:
    /**
     * @enum KitchenStatus
     * @brief Kitchen-specific status updates for orders
     */
    enum KitchenStatus { 
        ORDER_RECEIVED,     ///< Order received in kitchen
        PREP_STARTED,       ///< Kitchen started preparing order
        READY_FOR_PICKUP,   ///< Order ready for server pickup
        SERVED              ///< Order served to customer
    };
    
    /**
     * @struct KitchenTicket
     * @brief Represents a kitchen ticket with order information
     */
    struct KitchenTicket {
        int orderId;                ///< Associated order ID
        int tableNumber;            ///< Table number for the order
        std::vector<std::string> items; ///< List of items to prepare
        std::string specialInstructions; ///< Combined special instructions
        std::chrono::system_clock::time_point timestamp; ///< Ticket creation time
        KitchenStatus status;       ///< Current kitchen status
        int estimatedPrepTime;      ///< Estimated preparation time in minutes
        
        KitchenTicket() : orderId(0), tableNumber(0), status(ORDER_RECEIVED), estimatedPrepTime(0) {}
    };
    
    /**
     * @brief Constructs a new KitchenInterface
     */
    KitchenInterface();
    
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~KitchenInterface() = default;
    
    /**
     * @brief Sends order to kitchen display systems
     * @param order Order to send to kitchen
     * @return True if successfully sent, false otherwise
     */
    bool sendOrderToKitchen(std::shared_ptr<Order> order);
    
    /**
     * @brief Updates kitchen status for an order
     * @param orderId Order ID to update
     * @param status New kitchen status
     * @return True if successfully updated, false otherwise
     */
    bool updateKitchenStatus(int orderId, KitchenStatus status);
    
    /**
     * @brief Gets current kitchen queue status
     * @return JSON object with queue information
     */
    Wt::Json::Object getKitchenQueueStatus();
    
    /**
     * @brief Gets all active kitchen tickets
     * @return Vector of active kitchen tickets
     */
    std::vector<KitchenTicket> getActiveTickets();
    
    /**
     * @brief Gets kitchen ticket for a specific order
     * @param orderId Order ID to look up
     * @return Pointer to kitchen ticket, or nullptr if not found
     */
    const KitchenTicket* getTicketByOrderId(int orderId);
    
    /**
     * @brief Removes a completed ticket from the kitchen queue
     * @param orderId Order ID to remove
     * @return True if ticket was found and removed, false otherwise
     */
    bool removeTicket(int orderId);
    
    /**
     * @brief Gets estimated wait time for new orders
     * @return Estimated wait time in minutes
     */
    int getEstimatedWaitTime();
    
    /**
     * @brief Gets the number of orders currently in kitchen queue
     * @return Number of active kitchen tickets
     */
    size_t getQueueLength() const { return activeTickets_.size(); }
    
    /**
     * @brief Checks if kitchen is currently busy
     * @param threshold Maximum queue length before considered busy (default: 5)
     * @return True if kitchen queue exceeds threshold
     */
    bool isKitchenBusy(size_t threshold = 5) const { 
        return activeTickets_.size() > threshold; 
    }
    
    /**
     * @brief Gets the string representation of a kitchen status
     * @param status Kitchen status to convert
     * @return String representation of the status
     */
    static std::string kitchenStatusToString(KitchenStatus status);
    
    /**
     * @brief Broadcasts a message to all kitchen displays
     * @param message Message to broadcast
     * @return True if successfully sent, false otherwise
     */
    bool broadcastMessage(const std::string& message);
    
    // Extension points for kitchen workflow optimization
    /**
     * @brief Called when an order is sent to kitchen
     * Override in derived classes for custom behavior
     * @param order Order sent to kitchen
     * @param ticket Generated kitchen ticket
     */
    virtual void onOrderSentToKitchen(std::shared_ptr<Order> order, 
                                     const KitchenTicket& ticket) {}
    
    /**
     * @brief Called when kitchen status is updated
     * Override in derived classes for custom behavior
     * @param orderId Order ID with updated status
     * @param oldStatus Previous kitchen status
     * @param newStatus New kitchen status
     */
    virtual void onKitchenStatusUpdated(int orderId, 
                                       KitchenStatus oldStatus, 
                                       KitchenStatus newStatus) {}
    
    /**
     * @brief Called when kitchen queue becomes busy
     * Override in derived classes for alerts or workflow adjustments
     * @param queueLength Current queue length
     */
    virtual void onKitchenBusy(size_t queueLength) {}
    
    /**
     * @brief Called when kitchen queue becomes free
     * Override in derived classes for notifications
     * @param queueLength Current queue length
     */
    virtual void onKitchenFree(size_t queueLength) {}

protected:
    /**
     * @brief Creates a kitchen ticket from an order
     * @param order Order to create ticket for
     * @return Generated kitchen ticket
     */
    virtual KitchenTicket createKitchenTicket(std::shared_ptr<Order> order);
    
    /**
     * @brief Broadcasts JSON message to kitchen displays
     * @param message JSON message to broadcast
     * @return True if successfully sent, false otherwise
     */
    virtual bool broadcastToKitchen(const Wt::Json::Object& message);
    
    /**
     * @brief Estimates preparation time for an order
     * @param order Order to estimate preparation time for
     * @return Estimated preparation time in minutes
     */
    virtual int estimatePreparationTime(std::shared_ptr<Order> order);

private:
    /**
     * @brief Gets current timestamp as string
     * @return ISO format timestamp string
     */
    std::string getCurrentTimestamp();
    
    /**
     * @brief Finds ticket index by order ID
     * @param orderId Order ID to search for
     * @return Index of ticket, or -1 if not found
     */
    int findTicketIndex(int orderId);
    
    std::vector<KitchenTicket> activeTickets_;  ///< Active kitchen tickets
    bool wasKitchenBusy_;                       ///< Track kitchen busy state for notifications
};

#endif // KITCHENINTERFACE_H
