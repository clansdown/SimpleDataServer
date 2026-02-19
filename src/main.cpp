#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>

#include "server/data_server.hpp"
#include "handlers/api_handler.hpp"
#include "storage/file_manager.hpp"

namespace {

constexpr std::uint16_t DEFAULT_PORT = 8080;
constexpr std::string_view DEFAULT_DATA_DIR = "data";

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  -p, --port PORT    Port to listen on (default: " << DEFAULT_PORT << ")\n"
              << "  -d, --dir DIR      Data directory (default: " << DEFAULT_DATA_DIR << ")\n"
              << "  -h, --help         Show this help message\n";
}

} // namespace

int main(int argc, char* argv[]) {
    std::uint16_t port = DEFAULT_PORT;
    std::string data_dir(DEFAULT_DATA_DIR);

    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);

        if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                try {
                    port = static_cast<std::uint16_t>(std::stoi(argv[++i]));
                } catch (const std::exception&) {
                    std::cerr << "Invalid port number: " << argv[i] << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Option -p/--port requires an argument\n";
                return 1;
            }
        } else if (arg == "-d" || arg == "--dir") {
            if (i + 1 < argc) {
                data_dir = argv[++i];
            } else {
                std::cerr << "Option -d/--dir requires an argument\n";
                return 1;
            }
        } else if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    std::cout << "SimpleDataServer starting...\n"
              << "  Port: " << port << "\n"
              << "  Data directory: " << data_dir << "\n";

    auto file_manager = std::make_shared<simple_data_server::FileManager>(data_dir);
    auto api_handler = std::make_shared<simple_data_server::ApiHandler>(file_manager);
    simple_data_server::DataServer server(port, api_handler);

    if (!server.start()) {
        std::cerr << "Failed to start server\n";
        return 1;
    }

    return 0;
}
