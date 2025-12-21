#include "../include/core/RestaurantPOSApp.hpp"
#include <Wt/WServer.h>
#include <iostream>
#include <vector>
#include <string>

/**
 * @brief Prints usage information for the Restaurant POS System
 * @param programName Name of the executable
 */
void printUsage(const std::string& programName) {
    std::cout << "\n===========================================================" << std::endl;
    std::cout << "  Restaurant POS System - Configuration Options" << std::endl;
    std::cout << "===========================================================" << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -p, --port PORT         HTTP port (default: 8082)" << std::endl;
    std::cout << "  -a, --address ADDRESS   Bind address (default: 127.0.0.1)" << std::endl;
    std::cout << "  -d, --docroot PATH      Document root (default: .)" << std::endl;
    std::cout << "  -c, --config FILE       Config file (default: wt_config.xml)" << std::endl;
    std::cout << "  --no-config             Disable config file" << std::endl;
    std::cout << "  --debug                 Enable debug mode" << std::endl;
    std::cout << "  --no-compression        Disable HTTP compression" << std::endl;
    std::cout << "  --session-timeout SEC   Session timeout in seconds (default: 3600)" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << programName << "                    # Use defaults" << std::endl;
    std::cout << "  " << programName << " -p 9000            # Use port 9000" << std::endl;
    std::cout << "  " << programName << " --no-config        # No config file" << std::endl;
    std::cout << "  " << programName << " -p 8082 --debug    # Port 8082 with debug" << std::endl;
    std::cout << "===========================================================" << std::endl;
}

/**
 * @brief Configuration structure for server settings
 */
struct ServerConfig {
    std::string port = "8082";
    std::string address = "127.0.0.1";
    std::string docroot = ".";
    std::string configFile = "wt_config.xml";
    bool useConfig = true;
    bool debug = false;
    bool compression = true;
    std::string sessionTimeout = "3600";
    
    /**
     * @brief Converts configuration to command line arguments
     * @return Vector of argument strings
     */
    std::vector<std::string> toArgs(const std::string& programName) const {
        std::vector<std::string> args;
        args.push_back(programName);
        
        // HTTP settings
        args.push_back("--http-address");
        args.push_back(address);
        args.push_back("--http-port");
        args.push_back(port);
        args.push_back("--docroot");
        args.push_back(docroot);
        
        // Config file
        if (useConfig) {
            args.push_back("--config");
            args.push_back(configFile);
        } else {
            args.push_back("--config");
            args.push_back("");  // Empty config
        }
        
        // Optional settings
        if (debug) {
            args.push_back("--debug");
        }
        
        if (!compression) {
            args.push_back("--no-compression");
        }
        
        // Session settings
        args.push_back("--session-timeout");
        args.push_back(sessionTimeout);
        
        // Disable problematic features
        args.push_back("--ajax-puzzle");
        args.push_back("false");
        args.push_back("--progressive-bootstrap");
        args.push_back("false");
        args.push_back("--send-xhtml-mime-type");
        args.push_back("false");
        
        return args;
    }
};

/**
 * @brief Parses command line arguments into configuration
 * @param argc Argument count
 * @param argv Argument values
 * @return Parsed configuration
 */
ServerConfig parseArguments(int argc, char** argv) {
    ServerConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        }
        else if ((arg == "-p" || arg == "--port") && i + 1 < argc) {
            config.port = argv[++i];
        }
        else if ((arg == "-a" || arg == "--address") && i + 1 < argc) {
            config.address = argv[++i];
        }
        else if ((arg == "-d" || arg == "--docroot") && i + 1 < argc) {
            config.docroot = argv[++i];
        }
        else if ((arg == "-c" || arg == "--config") && i + 1 < argc) {
            config.configFile = argv[++i];
        }
        else if (arg == "--no-config") {
            config.useConfig = false;
        }
        else if (arg == "--debug") {
            config.debug = true;
        }
        else if (arg == "--no-compression") {
            config.compression = false;
        }
        else if (arg == "--session-timeout" && i + 1 < argc) {
            config.sessionTimeout = argv[++i];
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            exit(1);
        }
    }
    
    return config;
}

/**
 * @brief Converts string vector to char* array for Wt
 * @param args String arguments
 * @return Vector of char* pointers
 */
std::vector<char*> stringVectorToCharArray(const std::vector<std::string>& args) {
    std::vector<char*> charArgs;
    for (const auto& arg : args) {
        charArgs.push_back(const_cast<char*>(arg.c_str()));
    }
    return charArgs;
}

/**
 * @brief Main entry point for Restaurant POS System
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code
 */
int main(int argc, char** argv) {
    try {
<<<<<<< HEAD
        // Parse command line arguments
        ServerConfig config = parseArguments(argc, argv);
        
        // Print startup banner
        std::cout << "\n===========================================================" << std::endl;
        std::cout << "🍽️  Restaurant POS System - Starting Server" << std::endl;
        std::cout << "===========================================================" << std::endl;
        std::cout << "📍 Address: " << config.address << std::endl;
        std::cout << "🚪 Port: " << config.port << std::endl;
        std::cout << "📁 Document Root: " << config.docroot << std::endl;
        std::cout << "⚙️  Config File: " << (config.useConfig ? config.configFile : "DISABLED") << std::endl;
        std::cout << "🐛 Debug Mode: " << (config.debug ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "🗜️  Compression: " << (config.compression ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "⏱️  Session Timeout: " << config.sessionTimeout << " seconds" << std::endl;
        std::cout << "===========================================================" << std::endl;
        
        // Convert configuration to Wt arguments
        std::vector<std::string> stringArgs = config.toArgs(argv[0]);
        std::vector<char*> charArgs = stringVectorToCharArray(stringArgs);
        
        // Create and configure Wt server
        Wt::WServer server(static_cast<int>(charArgs.size()), charArgs.data(), WTHTTP_CONFIGURATION);
        
        // Add application entry point
        server.addEntryPoint(Wt::EntryPointType::Application, 
=======
        // Initialize server with command line args only
        Wt::WServer server(argc, argv);

        // Add entry point - this creates our application factory
        server.addEntryPoint(Wt::EntryPointType::Application,
>>>>>>> origin/claude/evaluate-pos-system-llm-01
                           createApplication,
                           "/pos",
                           "/favicon.ico");
        
        std::cout << "🚀 Starting server..." << std::endl;
        
        // Start the server
        if (server.start()) {
            std::cout << "\n===========================================================" << std::endl;
            std::cout << "✅ SERVER STARTED SUCCESSFULLY!" << std::endl;
            std::cout << "===========================================================" << std::endl;
            std::cout << "🌐 URL: http://" << config.address << ":" << config.port << "/pos" << std::endl;
            std::cout << "📋 Application: Restaurant POS System" << std::endl;
            std::cout << "🛑 Press Ctrl+C to stop the server" << std::endl;
            std::cout << "===========================================================" << std::endl;
            
            // Wait for shutdown signal
            int sig = Wt::WServer::waitForShutdown();
            std::cout << "\n🛑 Received shutdown signal (" << sig << ")" << std::endl;
            std::cout << "🔄 Stopping server..." << std::endl;
            server.stop();
            
            std::cout << "✅ Server stopped successfully" << std::endl;
        } else {
            std::cerr << "❌ Failed to start server" << std::endl;
            std::cerr << "💡 Try using a different port with: " << argv[0] << " -p 9000" << std::endl;
            return 1;
        }
        
    } catch (const Wt::WServer::Exception& e) {
        std::cerr << "❌ Wt Server Exception: " << e.what() << std::endl;
        std::cerr << "💡 Check your configuration file or try: " << argv[0] << " --no-config" << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "🏁 Restaurant POS System shutdown complete" << std::endl;
    return 0;
}
