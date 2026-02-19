#ifndef SIMPLE_DATA_SERVER_HANDLERS_API_HANDLER_HPP
#define SIMPLE_DATA_SERVER_HANDLERS_API_HANDLER_HPP

#include <string>
#include <string_view>
#include <memory>
#include "storage/file_manager.hpp"

namespace simple_data_server {

/**
 * @brief HTTP status codes for API responses.
 */
enum class HttpStatus : int {
    Ok = 200,
    BadRequest = 400,
    NotFound = 404,
    PayloadTooLarge = 413,
    InternalServerError = 500
};

/**
 * @brief Result of an API operation.
 */
struct ApiResult {
    HttpStatus status;
    std::string message;
    std::optional<nlohmann::json> data;
};

/**
 * @brief Handles API requests for put, get, and list operations.
 *
 * This class parses incoming JSON requests, validates required fields,
 * and delegates to the FileManager for storage operations.
 */
class ApiHandler {
public:
    /**
     * @brief Construct an ApiHandler with the given FileManager.
     *
     * @param file_manager Shared pointer to the FileManager instance.
     * @pre file_manager must not be nullptr.
     */
    explicit ApiHandler(std::shared_ptr<FileManager> file_manager);

    /**
     * @brief Handle a PUT request to store JSON data.
     *
     * Expected JSON body: {"key": "...", "filename": "...", "data": {...}}
     *
     * @param request_body The raw request body string.
     * @return ApiResult The result of the operation.
     * @pre request_body must be a valid JSON string.
     * @post On success, returns status Ok. On failure, returns appropriate error.
     */
    [[nodiscard]] ApiResult handle_put(std::string_view request_body) const noexcept;

    /**
     * @brief Handle a GET request to retrieve JSON data.
     *
     * Expected JSON body: {"key": "...", "filename": "..."}
     *
     * @param request_body The raw request body string.
     * @return ApiResult The result of the operation.
     * @pre request_body must be a valid JSON string.
     * @post On success, returns status Ok with data field. On failure, returns appropriate error.
     */
    [[nodiscard]] ApiResult handle_get(std::string_view request_body) const noexcept;

    /**
     * @brief Handle a LIST request to list files for a key.
     *
     * Expected JSON body: {"key": "..."}
     *
     * @param request_body The raw request body string.
     * @return ApiResult The result of the operation.
     * @pre request_body must be a valid JSON string.
     * @post On success, returns status Ok with files array. On failure, returns appropriate error.
     */
    [[nodiscard]] ApiResult handle_list(std::string_view request_body) const noexcept;

private:
    /**
     * @brief Parse request body JSON and extract key field.
     *
     * @param request_body The raw request body.
     * @return std::expected<std::string, ApiResult> The key or error.
     */
    [[nodiscard]] std::expected<std::string, ApiResult>
    parse_and_get_key(std::string_view request_body) const noexcept;

    /**
     * @brief Convert FileError to ApiResult.
     *
     * @param error The file error.
     * @return ApiResult The corresponding API result.
     */
    [[nodiscard]] ApiResult file_error_to_api_result(FileError error) const noexcept;

    std::shared_ptr<FileManager> file_manager_;
};

} // namespace simple_data_server

#endif // SIMPLE_DATA_SERVER_HANDLERS_API_HANDLER_HPP
