/**
 * @file main.cpp
 * @brief Entry point for the Restaurant POS System
 * 
 * This file contains the main function and server initialization for the
 * Restaurant Point of Sale system. The application uses the Wt web framework
 * to provide a modern, responsive web interface for restaurant operations.
 * 
 * Core Features:
 * - Order Management: Create, modify, and track customer orders
 * - Payment Processing: Handle multiple payment methods and transactions
 * - Kitchen Interface: Real-time communication with kitchen display systems
 * 
 * @author Restaurant POS Team
 * @version 1.0.0
 * @date 2025
 */

#include "../include/RestaurantPOSApp.hpp"
#include <Wt/WServer.h>
#include <iostream>
#include <exception>

/**
 * @brief Main entry point for the Restaurant POS application
 * 
 * Initializes the Wt web server with the POS application and starts
 * listening for HTTP connections. The server handles multiple concurrent
 * users and provides real-time updates across all connected sessions.
 * 
 * @param argc Command line argument count
 * @param argv Command line arguments
 * @return 0 on successful completion, 1 on error
 * 
 * Command line options:
 * --http-port 8081           Set HTTP port (default: 8081)
 * --docroot ./www           Set document root directory
 * --http-address 0.0.0.0    Bind to all network interfaces
 * --config wt_config.xml    Use configuration file
 */
int main(int argc, char** argv) {
    try {
        std::cout << "==================================================" << std::endl;
        std::cout << "    Restaurant POS System - Starting Server" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << "Version: 1.0.0" << std::endl;
        std::cout << "Framework: Wt (C++ Web Toolkit)" << std::endl;
        std::cout << "Architecture: Three-Legged Foundation" << std::endl;
        std::cout << "  1. Order Management" << std::endl;
        std::cout << "  2. Payment Processing" << std::endl;
        std::cout << "  3. Kitchen Interface" << std::endl;
        std::cout << "==================================================" << std::endl;
        
        // Create and configure the Wt server
        Wt::WServer server(argc, argv);
        
        // Register the application factory
        server.addEntryPoint(Wt::EntryPointType::Application, createApplication);
        
        // Configure server settings
        server.setServerConfiguration(argc, argv);
        
        // Start the server
        if (server.start()) {
            std::cout << std::endl;
            std::cout << "✓ Server started successfully!" << std::endl;
            std::cout << "✓ POS System is ready for operations" << std::endl;
            std::cout << std::endl;
            std::cout << "Access the POS system at:" << std::endl;
            std::cout << "  Local:    http://localhost:8081" << std::endl;
            std::cout << "  Network:  http://<your-ip>:8081" << std::endl;
            std::cout << std::endl;
            std::cout << "Features available:" << std::endl;
            std::cout << "  • Order creation and management" << std::endl;
            std::cout << "  • Menu item selection and customization" << std::endl;
            std::cout << "  • Kitchen order transmission and tracking" << std::endl;
            std::cout << "  • Payment processing (multiple methods)" << std::endl;
            std::cout << "  • Real-time status updates" << std::endl;
            std::cout << "  • Multi-table management" << std::endl;
            std::cout << std::endl;
            std::cout << "Press Ctrl+C to stop the server" << std::endl;
            std::cout << "==================================================" << std::endl;
            
            // Wait for shutdown signal
            Wt::WServer::waitForShutdown();
            
            std::cout << std::endl;
            std::cout << "Shutting down POS server..." << std::endl;
            server.stop();
            std::cout << "✓ Server stopped successfully" << std::endl;
        } else {
            std::cerr << "✗ Failed to start server" << std::endl;
            std::cerr << "Check that:" << std::endl;
            std::cerr << "  • Port 8081 is available (or specify different port with --http-port)" << std::endl;
            std::cerr << "  • You have permission to bind to the specified port" << std::endl;
            std::cerr << "  • Wt framework is properly installed" << std::endl;
            return 1;
        }
        
    } catch (const Wt::WServer::Exception& e) {
        std::cerr << "✗ Wt Server Exception: " << e.what() << std::endl;
        std::cerr << "This usually indicates:" << std::endl;
        std::cerr << "  • Configuration error" << std::endl;
        std::cerr << "  • Invalid command line arguments" << std::endl;
        std::cerr << "  • Missing or corrupted Wt installation" << std::endl;
        return 1;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Unexpected Exception: " << e.what() << std::endl;
        std::cerr << "Please check:" << std::endl;
        std::cerr << "  • System resources (memory, disk space)" << std::endl;
        std::cerr << "  • File permissions" << std::endl;
        std::cerr << "  • Network configuration" << std::endl;
        return 1;
        
    } catch (...) {
        std::cerr << "✗ Unknown error occurred during startup" << std::endl;
        std::cerr << "Please check system logs for more information" << std::endl;
        return 1;
    }
    
    return 0;
}
