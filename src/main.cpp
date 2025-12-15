#include "../include/core/RestaurantPOSApp.hpp"

#include <Wt/WServer.h>
#include <iostream>

int main(int argc, char** argv) {
    try {
        // Initialize server with command line args only
        Wt::WServer server(argc, argv);

        // Add entry point - this creates our application factory
        server.addEntryPoint(Wt::EntryPointType::Application,
                           createApplication,
                           "/pos",
                           "/favicon.ico");
        
        std::cout << "===========================================================" << std::endl;
        std::cout << "  Restaurant POS System - Modular Architecture v2.0.0" << std::endl;
        std::cout << "===========================================================" << std::endl;
        std::cout << "Starting server on http://localhost:8080/pos" << std::endl;
        std::cout << "Press Ctrl+C to stop the server" << std::endl;
        std::cout << "===========================================================" << std::endl;
        
        if (server.start()) {
            int sig = Wt::WServer::waitForShutdown();
            std::cout << "Shutting down: signal = " << sig << std::endl;
            server.stop();
        }
    } catch (const Wt::WServer::Exception& e) {
        std::cerr << "Server exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
