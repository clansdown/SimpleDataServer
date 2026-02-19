# SimpleDataServer

A simple C++23 backend server for sharing JSON data files between users using a shared key (no account system).

## Quick Start

```bash
# Download dependencies
chmod +x download_deps.sh
./download_deps.sh

# Build
mkdir -p build && cd build
cmake -DCMAKE_CXX_STANDARD=23 ..
cmake --build . -j$(nproc)

# Run the server
./simpledataserver -p 8080 -d data
```

## API Documentation

All endpoints accept POST requests with JSON bodies. All responses are in JSON format.

### Base URL Format

```
http(s)://your-server/api/<endpoint>
```

### Endpoints

#### 1. Store JSON Data - `/api/put`

Stores a JSON file in the key's directory.

**Request:**

```bash
POST /api/put
Content-Type: application/json

{
  "key": "mykey123",
  "filename": "data.json",
  "data": {
    "name": "Example",
    "value": 42,
    "items": ["a", "b", "c"]
  }
}
```

**Success Response (200 OK):**

```json
{
  "status": "success"
}
```

**Error Responses:**

- **400 Bad Request**: Missing fields, invalid JSON
- **404 Not Found**: Key directory doesn't exist
- **413 Payload Too Large**: Data exceeds 1MB limit

---

#### 2. Retrieve JSON Data - `/api/get`

Retrieves a JSON file from the key's directory.

**Request:**

```bash
POST /api/get
Content-Type: application/json

{
  "key": "mykey123",
  "filename": "data.json"
}
```

**Success Response (200 OK):**

```json
{
  "status": "success",
  "data": {
    "name": "Example",
    "value": 42,
    "items": ["a", "b", "c"]
  }
}
```

**Error Responses:**

- **400 Bad Request**: Missing fields
- **404 Not Found**: Key directory or file doesn't exist
- **400 Bad Request**: Invalid JSON in file

---

#### 3. List Files - `/api/list`

Lists all JSON files in the key's directory.

**Request:**

```bash
POST /api/list
Content-Type: application/json

{
  "key": "mykey123"
}
```

**Success Response (200 OK):**

```json
{
  "status": "success",
  "files": ["config.json", "data.json", "settings.json"]
}
```

**Error Responses:**

- **400 Bad Request**: Missing key field
- **404 Not Found**: Key directory doesn't exist

---

## Important Notes

### Key Directories

- Key directories must be **manually created** in the data folder before use
- The server will **not auto-create** key directories
- Example: For key `"mykey123"`, create directory `data/mykey123/`

### File Naming

- Filenames are **sanitized** to remove path separators (`/`, `\`) and dots
- `.json` extension is **automatically appended** if not present
- Only alphanumeric characters, underscores, and hyphens are allowed

### Data Limits

- **Maximum file size: 1MB** (enforced for both storage and retrieval)
- Maximum request body size: 1MB

### Error Response Format

All error responses follow this format:

```json
{
  "status": "<error message>",
  "error": "<detailed error>"
}
```

## Example Usage with cURL

```bash
# Store data
curl -X POST http://localhost:8080/api/put -H "Content-Type: application/json" \
  -d '{"key":"mykey123","filename":"config.json","data":{"theme":"dark","lang":"en"}}'

# Retrieve data
curl -X POST http://localhost:8080/api/get -H "Content-Type: application/json" \
  -d '{"key":"mykey123","filename":"config.json"}'

# List files
curl -X POST http://localhost:8080/api/list -H "Content-Type: application/json" \
  -d '{"key":"mykey123"}'
```

## Command-Line Options

```
simpledataserver [options]

Options:
  -p, --port PORT    Port to listen on (default: 8080)
  -d, --dir DIR      Data directory (default: data)
  -h, --help         Show help message
```

## Deployment

The project is designed to run behind nginx for production use:

- SimpleDataServer handles application logic
- nginx manages TLS termination
- nginx handles slow/misbehaving clients
- nginx provides HTTP/2 and connection pooling

## Requirements

- C++23 compiler (GCC 13+, Clang 16+)
- CMake 3.20+
- OpenSSL (optional, for TLS via nginx)
- C++23 features: `std::expected`, `std::string_view`, `std::optional`

## License

MIT
