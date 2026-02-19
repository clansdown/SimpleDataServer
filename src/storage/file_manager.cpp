#include "storage/file_manager.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace simple_data_server {

namespace {

constexpr std::string_view JSON_EXTENSION = ".json";
constexpr size_t MAX_JSON_SIZE_BYTES = 1024 * 1024; // 1MB

bool is_valid_json_character(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-';
}

} // namespace

FileManager::FileManager(std::string data_directory)
    : data_directory_(std::move(data_directory)) {
    std::filesystem::create_directories(data_directory_);
}

std::expected<void, FileError>
FileManager::put_json(std::string_view key,
                     std::string_view filename,
                     const nlohmann::json& data) noexcept {
    if (key.empty()) {
        return std::unexpected(FileError::InvalidFilename);
    }

    if (!key_directory_exists(key)) {
        return std::unexpected(FileError::KeyDirectoryNotFound);
    }

    const auto sanitized_filename = sanitize_filename(filename);
    if (sanitized_filename.empty()) {
        return std::unexpected(FileError::InvalidFilename);
    }

    const auto filename_with_ext = ensure_json_extension(sanitized_filename);
    const auto file_path = get_file_path(key, filename_with_ext);

    try {
        const auto json_string = data.dump();
        if (json_string.size() > MAX_JSON_SIZE_BYTES) {
            return std::unexpected(FileError::FileTooLarge);
        }

        std::ofstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            return std::unexpected(FileError::IoError);
        }

        file << json_string;
        if (file.fail()) {
            return std::unexpected(FileError::IoError);
        }

        return {};
    } catch (const nlohmann::json::exception&) {
        return std::unexpected(FileError::JsonEncodingError);
    }
}

std::expected<nlohmann::json, FileError>
FileManager::get_json(std::string_view key, std::string_view filename) const noexcept {
    if (key.empty() || filename.empty()) {
        return std::unexpected(FileError::InvalidFilename);
    }

    if (!key_directory_exists(key)) {
        return std::unexpected(FileError::KeyDirectoryNotFound);
    }

    const auto sanitized_filename = sanitize_filename(filename);
    const auto filename_with_ext = ensure_json_extension(sanitized_filename);
    const auto file_path = get_file_path(key, filename_with_ext);

    if (!std::filesystem::exists(file_path)) {
        return std::unexpected(FileError::FileNotFound);
    }

    try {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            return std::unexpected(FileError::IoError);
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

        if (content.size() > MAX_JSON_SIZE_BYTES) {
            return std::unexpected(FileError::FileTooLarge);
        }

        auto data = nlohmann::json::parse(content);
        return data;
    } catch (const nlohmann::json::parse_error&) {
        return std::unexpected(FileError::InvalidJson);
    } catch (const std::exception&) {
        return std::unexpected(FileError::IoError);
    }
}

std::expected<std::vector<std::string>, FileError>
FileManager::list_files(std::string_view key) const noexcept {
    if (key.empty()) {
        return std::unexpected(FileError::InvalidFilename);
    }

    if (!key_directory_exists(key)) {
        return std::unexpected(FileError::KeyDirectoryNotFound);
    }

    const auto key_dir = get_key_directory(key);
    std::vector<std::string> files;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(key_dir)) {
            if (entry.is_regular_file()) {
                const auto filename = entry.path().filename().string();
                if (filename.size() >= JSON_EXTENSION.size() &&
                    filename.substr(filename.size() - JSON_EXTENSION.size()) == JSON_EXTENSION) {
                    files.push_back(filename);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        return std::unexpected(FileError::IoError);
    }

    std::sort(files.begin(), files.end());
    return files;
}

bool FileManager::key_directory_exists(std::string_view key) const noexcept {
    const auto key_dir = get_key_directory(key);
    return std::filesystem::exists(key_dir) && std::filesystem::is_directory(key_dir);
}

std::string FileManager::sanitize_filename(std::string_view filename) const noexcept {
    std::string result;
    result.reserve(filename.size());

    for (char c : filename) {
        if (c == '/' || c == '\\' || c == '\0') {
            continue;
        }
        if (c == '.' && !result.empty() && result.back() == '.') {
            continue;
        }
        if (c == '.') {
            result.push_back('_');
            continue;
        }
        if (is_valid_json_character(c)) {
            result.push_back(c);
        }
    }

    while (!result.empty() && (result.back() == '_' || result.back() == '-')) {
        result.pop_back();
    }

    return result;
}

std::string FileManager::ensure_json_extension(std::string filename) const noexcept {
    if (filename.size() >= JSON_EXTENSION.size()) {
        const auto ext = filename.substr(filename.size() - JSON_EXTENSION.size());
        if (ext == JSON_EXTENSION) {
            return filename;
        }
    }
    return filename + JSON_EXTENSION.data();
}

std::string FileManager::get_key_directory(std::string_view key) const noexcept {
    return std::string(data_directory_) + "/" + std::string(key);
}

std::string FileManager::get_file_path(std::string_view key,
                                        std::string_view filename) const noexcept {
    return get_key_directory(key) + "/" + std::string(filename);
}

} // namespace simple_data_server
