#include "server/data_server.hpp"

#include <App.h>

#include <iostream>
#include <memory>
#include <string>

namespace simple_data_server {

namespace {

constexpr int HTTP_OK = 200;
constexpr int HTTP_BAD_REQUEST = 400;
constexpr int HTTP_NOT_FOUND = 404;
constexpr int HTTP_PAYLOAD_TOO_LARGE = 413;
constexpr int HTTP_INTERNAL_SERVER_ERROR = 500;

std::string status_to_string(HttpStatus status) {
    switch (static_cast<int>(status)) {
        case 200:
            return "200 OK";
        case 400:
            return "400 Bad Request";
        case 404:
            return "404 Not Found";
        case 413:
            return "413 Payload Too Large";
        case 500:
            return "500 Internal Server Error";
    }
    return "500 Internal Server Error";
}

int status_to_int(HttpStatus status) {
    return static_cast<int>(status);
}

void send_response(auto* res, const ApiResult& result) {
    const auto status_code = status_to_int(result.status);

    nlohmann::json response_json;
    response_json["status"] = result.message;

    if (result.data.has_value()) {
        response_json.merge_patch(result.data.value());
    }

    const auto response_str = response_json.dump();

    res->writeStatus(status_to_string(result.status))
        ->writeHeader("Content-Type", "application/json")
        ->writeHeader("Content-Length", std::to_string(response_str.length()))
        ->end(response_str);
}

} // namespace

DataServer::DataServer(std::uint16_t port, std::shared_ptr<ApiHandler> api_handler)
    : port_(port), api_handler_(std::move(api_handler)), listen_socket_(nullptr) {
}

bool DataServer::start() noexcept {
    uWS::App app;

    auto* handler = api_handler_.get();

    app.post("/api/put", [handler]<auto res> {
        auto* response = res;
        auto body_buffer = std::make_shared<std::string>();

        response->onData([response, body_buffer, handler]<auto chunk, bool is_last> mutable {
            body_buffer->append(chunk.data(), chunk.length());

            if (body_buffer->size() > MAX_REQUEST_SIZE) {
                nlohmann::json error_response;
                error_response["error"] = "Request body too large";
                const auto error_str = error_response.dump();
                response->writeStatus("413 Payload Too Large")
                    ->writeHeader("Content-Type", "application/json")
                    ->end(error_str);
                return;
            }

            if (is_last) {
                const auto result = handler->handle_put(*body_buffer);
                send_response(response, result);
            }
        });

        response->onAborted([]<auto> {
            std::cerr << "Request aborted" << std::endl;
        });
    });

    app.post("/api/get", [handler]<auto res> {
        auto* response = res;
        auto body_buffer = std::make_shared<std::string>();

        response->onData([response, body_buffer, handler]<auto chunk, bool is_last> mutable {
            body_buffer->append(chunk.data(), chunk.length());

            if (body_buffer->size() > MAX_REQUEST_SIZE) {
                nlohmann::json error_response;
                error_response["error"] = "Request body too large";
                const auto error_str = error_response.dump();
                response->writeStatus("413 Payload Too Large")
                    ->writeHeader("Content-Type", "application/json")
                    ->end(error_str);
                return;
            }

            if (is_last) {
                const auto result = handler->handle_get(*body_buffer);
                send_response(response, result);
            }
        });

        response->onAborted([]<auto> {
            std::cerr << "Request aborted" << std::endl;
        });
    });

    app.post("/api/list", [handler]<auto res> {
        auto* response = res;
        auto body_buffer = std::make_shared<std::string>();

        response->onData([response, body_buffer, handler]<auto chunk, bool is_last> mutable {
            body_buffer->append(chunk.data(), chunk.length());

            if (body_buffer->size() > MAX_REQUEST_SIZE) {
                nlohmann::json error_response;
                error_response["error"] = "Request body too large";
                const auto error_str = error_response.dump();
                response->writeStatus("413 Payload Too Large")
                    ->writeHeader("Content-Type", "application/json")
                    ->end(error_str);
                return;
            }

            if (is_last) {
                const auto result = handler->handle_list(*body_buffer);
                send_response(response, result);
            }
        });

        response->onAborted([]<auto> {
            std::cerr << "Request aborted" << std::endl;
        });
    });

    app.get("/*", []<auto res> {
        nlohmann::json error_response;
        error_response["error"] = "Not found";
        const auto error_str = error_response.dump();
        res->writeStatus("404 Not Found")
            ->writeHeader("Content-Type", "application/json")
            ->end(error_str);
    });

    const bool success = app.listen(port_, [this]<auto listen_socket> {
        if (listen_socket) {
            listen_socket_ = listen_socket;
            std::cout << "Server listening on port " << port_ << std::endl;
        } else {
            std::cerr << "Failed to listen on port " << port_ << std::endl;
        }
    });

    if (!success) {
        return false;
    }

    app.run();
    return true;
}

void DataServer::stop() noexcept {
    if (listen_socket_) {
        us_listen_socket_close(0, listen_socket_);
        listen_socket_ = nullptr;
    }
}

} // namespace simple_data_server
