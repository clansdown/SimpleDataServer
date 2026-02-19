#ifndef SIMPLE_DATA_SERVER_SERVER_DATA_SERVER_HPP
#define SIMPLE_DATA_SERVER_SERVER_DATA_SERVER_HPP

#include <cstdint>
#include <memory>
#include "handlers/api_handler.hpp"

/**
 * @brief Maximum request body size in bytes (1MB).
 */
constexpr std::size_t MAX_REQUEST_SIZE = 1024 * 1024;

namespace simple_data_server {

/**
 * @brief The main data server class using uWebSockets.
 *
 * This class initializes and runs the HTTP server with the specified
 * API endpoints for put, get, and list operations.
 */
class DataServer {
public:
    /**
     * @brief Construct a DataServer with the specified port and handler.
     *
     * @param port The port number to listen on.
     * @param api_handler Shared pointer to the ApiHandler instance.
     * @pre api_handler must not be nullptr.
     * @post Server is configured but not yet running.
     */
    DataServer(std::uint16_t port, std::shared_ptr<ApiHandler> api_handler);

    /**
     * @brief Start the server and begin listening for connections.
     *
     * @return true if the server started successfully, false otherwise.
     * @post Server is running and accepting connections.
     */
    [[nodiscard]] bool start() noexcept;

    /**
     * @brief Stop the server.
     *
     * @post Server is stopped and no longer accepting connections.
     */
    void stop() noexcept;

    /**
     * @brief Get the port the server is listening on.
     *
     * @return std::uint16_t The port number.
     */
    [[nodiscard]] std::uint16_t get_port() const noexcept {
        return port_;
    }

private:
    /**
     * @brief Process incoming request data.
     *
     * @param response The HTTP response object.
     * @param handler The API handler to use.
     * @param body_buffer Accumulated request body.
     * @param is_last Whether this is the last chunk of data.
     */
    template<typename ResponseType>
    static void process_request(ResponseType* response,
                                ApiHandler* handler,
                                std::string* body_buffer,
                                bool is_last);

    std::uint16_t port_;
    std::shared_ptr<ApiHandler> api_handler_;
    void* listen_socket_;
};

} // namespace simple_data_server

#endif // SIMPLE_DATA_SERVER_SERVER_DATA_SERVER_HPP
