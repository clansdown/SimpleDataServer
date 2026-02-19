#include "handlers/api_handler.hpp"

#include <optional>

namespace simple_data_server {

ApiHandler::ApiHandler(std::shared_ptr<FileManager> file_manager)
    : file_manager_(std::move(file_manager)) {
}

ApiResult ApiHandler::handle_put(std::string_view request_body) const noexcept {
    try {
        auto request = nlohmann::json::parse(request_body);

        if (!request.contains("key") || !request["key"].is_string()) {
            return {HttpStatus::BadRequest, "Missing or invalid 'key' field", std::nullopt};
        }

        if (!request.contains("filename") || !request["filename"].is_string()) {
            return {HttpStatus::BadRequest, "Missing or invalid 'filename' field", std::nullopt};
        }

        if (!request.contains("data")) {
            return {HttpStatus::BadRequest, "Missing 'data' field", std::nullopt};
        }

        const auto key = request["key"].get<std::string>();
        const auto filename = request["filename"].get<std::string>();
        const auto& data = request["data"];

        const auto result = file_manager_->put_json(key, filename, data);
        if (!result) {
            return file_error_to_api_result(result.error());
        }

        return {HttpStatus::Ok, "success", std::nullopt};

    } catch (const nlohmann::json::parse_error&) {
        return {HttpStatus::BadRequest, "Invalid JSON", std::nullopt};
    } catch (const std::exception& e) {
        return {HttpStatus::InternalServerError, e.what(), std::nullopt};
    }
}

ApiResult ApiHandler::handle_get(std::string_view request_body) const noexcept {
    try {
        auto request = nlohmann::json::parse(request_body);

        if (!request.contains("key") || !request["key"].is_string()) {
            return {HttpStatus::BadRequest, "Missing or invalid 'key' field", std::nullopt};
        }

        if (!request.contains("filename") || !request["filename"].is_string()) {
            return {HttpStatus::BadRequest, "Missing or invalid 'filename' field", std::nullopt};
        }

        const auto key = request["key"].get<std::string>();
        const auto filename = request["filename"].get<std::string>();

        const auto result = file_manager_->get_json(key, filename);
        if (!result) {
            return file_error_to_api_result(result.error());
        }

        nlohmann::json response_data;
        response_data["data"] = result.value();
        return {HttpStatus::Ok, "success", response_data};

    } catch (const nlohmann::json::parse_error&) {
        return {HttpStatus::BadRequest, "Invalid JSON", std::nullopt};
    } catch (const std::exception& e) {
        return {HttpStatus::InternalServerError, e.what(), std::nullopt};
    }
}

ApiResult ApiHandler::handle_list(std::string_view request_body) const noexcept {
    try {
        auto request = nlohmann::json::parse(request_body);

        if (!request.contains("key") || !request["key"].is_string()) {
            return {HttpStatus::BadRequest, "Missing or invalid 'key' field", std::nullopt};
        }

        const auto key = request["key"].get<std::string>();

        const auto result = file_manager_->list_files(key);
        if (!result) {
            return file_error_to_api_result(result.error());
        }

        nlohmann::json response_data;
        response_data["files"] = result.value();
        return {HttpStatus::Ok, "success", response_data};

    } catch (const nlohmann::json::parse_error&) {
        return {HttpStatus::BadRequest, "Invalid JSON", std::nullopt};
    } catch (const std::exception& e) {
        return {HttpStatus::InternalServerError, e.what(), std::nullopt};
    }
}

ApiResult ApiHandler::file_error_to_api_result(FileError error) const noexcept {
    switch (error) {
        case FileError::KeyDirectoryNotFound:
            return {HttpStatus::NotFound, "Key directory not found", std::nullopt};
        case FileError::FileNotFound:
            return {HttpStatus::NotFound, "File not found", std::nullopt};
        case FileError::InvalidJson:
            return {HttpStatus::BadRequest, "Invalid JSON data", std::nullopt};
        case FileError::FileTooLarge:
            return {HttpStatus::PayloadTooLarge, "File exceeds maximum size (1MB)", std::nullopt};
        case FileError::InvalidFilename:
            return {HttpStatus::BadRequest, "Invalid filename", std::nullopt};
        case FileError::IoError:
            return {HttpStatus::InternalServerError, "File I/O error", std::nullopt};
        case FileError::JsonEncodingError:
            return {HttpStatus::InternalServerError, "JSON encoding error", std::nullopt};
    }
    return {HttpStatus::InternalServerError, "Unknown error", std::nullopt};
}

} // namespace simple_data_server
