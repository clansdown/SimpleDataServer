#ifndef SIMPLE_DATA_SERVER_STORAGE_FILE_MANAGER_HPP
#define SIMPLE_DATA_SERVER_STORAGE_FILE_MANAGER_HPP

#include <string>
#include <string_view>
#include <vector>
#include <expected.hpp>
#include <nlohmann/json.hpp>

namespace simple_data_server {

/**
 * @brief Error types for file operations.
 */
enum class FileError {
    KeyDirectoryNotFound,
    FileNotFound,
    InvalidJson,
    FileTooLarge,
    InvalidFilename,
    IoError,
    JsonEncodingError
};

/**
 * @brief Manages file storage operations for JSON data files.
 *
 * This class handles all file I/O operations including reading, writing,
 * and listing JSON files within key-specific directories.
 */
class FileManager {
public:
    /**
     * @brief Construct a FileManager with the specified data directory.
     *
     * @param data_directory Path to the data directory (e.g., "data").
     * @pre data_directory must not be empty.
     * @post Creates the data directory if it doesn't exist.
     */
    explicit FileManager(std::string data_directory);

    /**
     * @brief Put JSON data to a file.
     *
     * @param key The user's shared key (determines subdirectory).
     * @param filename The name of the file (will be sanitized, .json added if missing).
     * @param data The JSON data to store.
     * @return std::expected<void, FileError> Success or error.
     * @pre key must not be empty.
     * @pre filename must be a valid filename after sanitization.
     * @pre data must be valid JSON.
     * @post On success, file is written to data/{key}/{filename}.json
     */
    [[nodiscard]] std::expected<void, FileError>
    put_json(std::string_view key,
             std::string_view filename,
             const nlohmann::json& data) noexcept;

    /**
     * @brief Get JSON data from a file.
     *
     * @param key The user's shared key (determines subdirectory).
     * @param filename The name of the file to read.
     * @return std::expected<nlohmann::json, FileError> The JSON data or error.
     * @pre key must not be empty.
     * @pre filename must not be empty.
     * @post On success, returns the parsed JSON data.
     */
    [[nodiscard]] std::expected<nlohmann::json, FileError>
    get_json(std::string_view key, std::string_view filename) const noexcept;

    /**
     * @brief List all JSON files for a key.
     *
     * @param key The user's shared key (determines subdirectory).
     * @return std::expected<std::vector<std::string>, FileError> List of filenames or error.
     * @pre key must not be empty.
     * @post On success, returns list of .json files in data/{key}/.
     */
    [[nodiscard]] std::expected<std::vector<std::string>, FileError>
    list_files(std::string_view key) const noexcept;

    /**
     * @brief Check if a key directory exists.
     *
     * @param key The user's shared key to check.
     * @return true if the directory exists, false otherwise.
     */
    [[nodiscard]] bool key_directory_exists(std::string_view key) const noexcept;

    /**
     * @brief Get the data directory path.
     *
     * @return const std::string& The data directory path.
     */
    [[nodiscard]] const std::string& get_data_directory() const noexcept {
        return data_directory_;
    }

private:
    /**
     * @brief Sanitize a filename by removing path components.
     *
     * @param filename The original filename.
     * @return std::string The sanitized filename.
     * @post Removes all path separators, dots (except final extension), and path components.
     */
    [[nodiscard]] std::string sanitize_filename(std::string_view filename) const noexcept;

    /**
     * @brief Ensure .json extension is present.
     *
     * @param filename The filename to check.
     * @return std::string The filename with .json extension.
     */
    [[nodiscard]] std::string ensure_json_extension(std::string filename) const noexcept;

    /**
     * @brief Get the full path for a key's directory.
     *
     * @param key The user's shared key.
     * @return std::string The full path to the key's directory.
     */
    [[nodiscard]] std::string get_key_directory(std::string_view key) const noexcept;

    /**
     * @brief Get the full path for a file.
     *
     * @param key The user's shared key.
     * @param filename The filename.
     * @return std::string The full path to the file.
     */
    [[nodiscard]] std::string get_file_path(std::string_view key,
                                            std::string_view filename) const noexcept;

    std::string data_directory_;
};

} // namespace simple_data_server

#endif // SIMPLE_DATA_SERVER_STORAGE_FILE_MANAGER_HPP
