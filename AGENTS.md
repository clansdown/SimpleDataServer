# AGENTS.md - SimpleDataServer Development Guide

This document provides guidance for agentic coding agents working on the SimpleDataServer project.

## Project Overview

SimpleDataServer is a C++23 backend server for sharing JSON data files between users using a shared key (no account system).

---

## Build Commands

### CMake Build System

The project uses CMake. Build commands:

```bash
# Create build directory and configure
mkdir -p build && cd build
cmake -DCMAKE_CXX_STANDARD=23 ..

# Build the project
cmake --build . -j$(nproc)

# Build with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### Running a Single Test

When tests are implemented, use:

```bash
# Using ctest with filter
ctest -R test_name_here -V

# Or if using a test runner directly
./build/tests/test_executable --test-case=TestName
./build/tests/test_executable --run-specific-test=TestName
```

---

## Code Style Guidelines

### General Principles

- **C++23**: Use modern C++23 features (std::expected, std::optional, ranges, concepts)
- **RAII**: Always use RAII for resource management (lock guards, file handles, connections)
- **No stubs**: Never stub out functions unless explicitly requested by the user
- **Fully working code**: Produce complete, production-ready implementations

### Naming Conventions

- **Functions**: snake_case, highly descriptive (e.g., `get_user_data_by_key`, `validate_shared_key`)
- **Variables**: snake_case, descriptive (e.g., `json_file_path`, `connection_timeout_ms`)
- **Classes/Types**: PascalCase (e.g., `DataServer`, `JsonFileHandler`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_REQUEST_SIZE`, `DEFAULT_PORT`)
- **Private members**: snake_case with trailing underscore (e.g., `server_socket_`)

### Code Formatting

- Use 4 spaces for indentation (no tabs)
- Max line length: 100 characters
- Opening brace on same line for functions/classes, new line for control statements
- Use `clang-format` with the following config:

```bash
# Format code
clang-format -i --style=file src/**/*.cpp src/**/*.hpp

# Check formatting
clang-format --style=file --dry-run src/**/*.cpp src/**/*.hpp
```

### Import Organization

Order includes (one blank line between groups):
1. Corresponding header (for .cpp files)
2. Standard library headers (`<iostream>`, `<string>`, etc.)
3. Third-party library headers (`<nlohmann/json.hpp>`, etc.)
4. Project local headers (`"handler/user_handler.hpp"`, etc.)

### Documentation

**Every function must have a comment block** documenting:
- Purpose of the function
- Parameters (name and purpose)
- Return value
- Preconditions/postconditions
- Error handling behavior

Example:
```cpp
/**
 * @brief Retrieves JSON data associated with a shared key.
 * 
 * @param shared_key The user's shared key for authentication.
 * @param data_identifier The identifier for the requested data.
 * @return std::expected<nlohmann::json, ServerError> The requested data or error.
 * @pre shared_key must not be empty.
 * @post On success, returns the JSON data. On failure, returns error.
 */
[[nodiscard]] std::expected<nlohmann::json, ServerError>
get_data_by_key(const std::string& shared_key,
                const std::string& data_identifier) noexcept;
```

### Error Handling

- Use `std::expected<T, Error>` for functions that may fail (C++23)
- Avoid exceptions unless for truly unrecoverable errors
- Provide meaningful error messages with context
- Log errors appropriately using a structured logger
- Define error codes/enums for different failure modes

### RAII Principles

- Wrap all resources in RAII classes (files, sockets, locks, memory)
- Use `std::unique_ptr` for exclusive ownership
- Use `std::shared_ptr` for shared ownership
- Use `std::lock_guard` or `std::scoped_lock` for mutexes
- Avoid raw `new`/`delete`; use smart pointers or containers

### Function Design

- Separate logic into focused functions where practical
- Pass complex objects by const reference (`const std::string&`)
- Use pass-by-value for cheap-to-copy types (primitives, small views)
- Avoid excessive parameters; consider a configuration struct if >4 params
- Mark functions `noexcept` when they cannot throw
- Mark functions `[[nodiscard]]` when return value must be used

### Memory and Performance

- Prefer stack allocation over heap when possible
- Use `std::string_view` instead of `std::string` for read-only strings
- Use `std::span` for passing arrays/buffers
- Avoid unnecessary copies; use move semantics (`std::move`)
- Profile before optimizing; write clear code first

### Concurrency

- Use `std::jthread` for cancellable threads (C++20/23)
- Protect shared data with mutexes or atomic operations
- Avoid deadlocks: always lock in consistent order
- Use `std::atomic` for simple shared state
- Consider `std::future`/`std::promise` for async operations

### Testing

- Use a header-only test framework (Catch2 or doctest recommended)
- Test public interfaces, not internal implementation details
- Follow AAA pattern: Arrange, Act, Assert
- Test both success and failure paths
- Use descriptive test names: `test_get_data_with_valid_key_succeeds`

---

## Project Structure Recommendation

```
SimpleDataServer/
├── CMakeLists.txt
├── .clang-format
├── .clang-tidy
├── src/
│   ├── main.cpp
│   ├── server/
│   │   ├── server.hpp
│   │   └── server.cpp
│   ├── handlers/
│   │   ├── data_handler.hpp
│   │   └── data_handler.cpp
│   ├── auth/
│   │   ├── key_validator.hpp
│   │   └── key_validator.cpp
│   ├── storage/
│   │   ├── json_store.hpp
│   │   └── json_store.cpp
│   └── common/
│       ├── errors.hpp
│       └── types.hpp
├── include/              # Public headers
├── tests/
│   ├── CMakeLists.txt
│   ├── test_data_handler.cpp
│   └── test_key_validator.cpp
└── build/
```

---

## Linting

Use clang-tidy for static analysis:

```bash
# Run clang-tidy on all source files
clang-tidy src/**/*.cpp src/**/*.hpp -- -DCMAKE_CXX_STANDARD=23

# Fix common issues
clang-tidy src/**/*.cpp src/**/*.hpp --fix --fix-errors
```

---

## Pre-Commit Checks

Before marking work complete, verify:

1. **Build succeeds**: `cmake --build .`
2. **Tests pass**: `ctest --output-on-failure`
3. **Formatting**: `clang-format --style=file --dry-run src/**/*.cpp src/**/*.hpp`
4. **No warnings**: Compile with `-Wall -Wextra -Wpedantic`
5. **Memory safety**: Run with `-fsanitize=address,undefined` during testing

---

## Key Dependencies

- **JSON**: nlohmann/json (header-only)
- **Testing**: Catch2 or doctest (header-only)
- **Networking**: Boost.Asio or standard library `<format>`/`<coroutine>`
- **Logging**: spdlog (recommended) or custom implementation

---

## Notes for Agents

- Always produce complete, working code - no placeholder implementations
- Document every function with a comment block
- Use meaningful, descriptive names for all identifiers
- Prefer explicit error handling over exceptions
- Keep functions focused and single-purpose
- Ask the user if requirements are unclear
