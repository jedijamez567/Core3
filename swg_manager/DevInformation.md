# Core3 REST API Integration Documentation

## Overview

This document provides comprehensive information about the Core3 REST API for Star Wars Galaxies and outlines a plan for integrating a frontend application with this API backend.

## Core3 REST API Architecture

The Core3 REST API is built using [cpprestsdk](https://github.com/microsoft/cpprestsdk) and follows a proxy pattern to isolate game logic from API server logic. The key components of the architecture include:

1. **RESTServer**: The main server component that handles HTTP requests, authentication, and routing to the appropriate endpoints.

2. **RESTEndpoint**: Defines API endpoints with regex patterns for URI matching and handlers for processing requests.

3. **APIRequest**: Encapsulates a single API request/response cycle, providing methods for accessing path fields, query parameters, and request body data.

4. **BaseAPIProxy**: Base class for all API proxies, providing common functionality for interacting with the game server.

5. **Specialized API Proxies**: Implementations for different game subsystems:
   - APIProxyPlayerManager: Player and account management
   - APIProxyChatManager: Chat and messaging functionality
   - APIProxyObjectManager: Game object manipulation
   - APIProxyGuildManager: Guild-related operations
   - APIProxyConfigManager: Server configuration management
   - APIProxyStatisticsManager: Server statistics

## Current Configuration Status

The Core3 REST API is **disabled by default** in the main configuration file (`MMOCoreORB/bin/conf/config.lua`). The relevant configuration is:

```lua
------REST Server Config------
RESTServerPort = 0,
```

To enable the API, a `config-local.lua` file needs to be created with the following configuration:

```lua
Core3.RESTServerPort = 44443
Core3.RESTServer = {
    LogLevel = 4, -- -1 NONE, 0 FATAL, 1 ERROR, 2 WARNING, 3 LOG, 4 INFO, 5 DEBUG
    APIToken = "token",
    SSLKeyFile = "conf/ssl.key",
    SSLCertFile = "conf/ssl.crt",
}
```

## SSL Certificate Requirements

The API requires SSL certificates for secure communication. Self-signed certificates can be generated using:

```bash
$ cd bin/conf
$ openssl genrsa -out ssl.key 2048
$ openssl req -new -key secrets/ssl.key -out ssl.csr -subj "/C=US/ST=NY/L=Somewhere/O=MyOrg/CN=Core3API"
$ openssl x509 -req -days 3650 -in ssl.csr -signkey secrets/ssl.key -out ssl.crt
$ rm -f ssl.csr
$ openssl x509 -text -noout -in ssl.crt
```

## Authentication

The API uses Bearer token authentication. A secure token should be generated and configured in `config-local.lua`. A simple way to generate a token is:

```bash
openssl rand -base64 18
```

All API requests must include the token in the Authorization header:

```
Authorization: Bearer {tokenhere}
```

## Available API Endpoints

The Core3 REST API provides the following endpoints:

### Version Information
- `GET:/v1/version/`: Returns API version and Core3 version information

### Object Management
- `GET|DELETE:/v1/object/(?:(\\d*)/|)`: Get or delete objects by ID
- `PUT:/v1/object/(\\d+)/(\\w+)/(\\w+)/`: Update object properties

### Configuration Management
- `GET|POST|PUT:/v1/admin/config/(?:(.*)/|)`: Get or update server configuration

### Statistics Management
- `GET|PUT:/v1/admin/stats/`: Get or update server statistics

### Console Command Execution
- `POST:/v1/admin/console/(\\w+)/`: Execute console commands

### Account and Character Management
- `POST:/v1/admin/account/(\\d+)/galaxy/(\\d+)/character/(\\d+)/`: Character management
- `POST:/v1/admin/account/(\\d+)/`: Account management

### Lookup Services
- `GET:/v1/(find|lookup)/character/`: Character lookup with optional search parameters
- `GET:/v1/(find|lookup)/guild/`: Guild lookup with optional search parameters

### Chat and Messaging
- `POST:/v1/chat/(mail|message|galaxy)/`: Send mail, messages, or galaxy-wide announcements

## API Response Format

API responses are in JSON format and typically include:
- Status information
- Requested data or confirmation of actions
- Metadata about the request/response

Error responses include:
- Error message for the user
- Detailed error information for debugging
- HTTP status code indicating the type of error

## Frontend Integration Plan

### 1. API Configuration Setup

Before integrating the frontend, the API must be properly configured:

1. Create `config-local.lua` in `MMOCoreORB/bin/conf/` with appropriate settings
2. Generate SSL certificates
3. Configure a secure API token
4. Restart the server to apply changes
5. Verify API is working with a simple test request

### 2. Frontend API Client Implementation

#### 2.1 API Client Architecture

Create a modular API client with the following components:

1. **Core API Client**:
   - Handles authentication
   - Manages request/response lifecycle
   - Implements error handling
   - Provides logging and debugging

2. **Resource-specific Services**:
   - Player/Account Service
   - Object Service
   - Chat Service
   - Configuration Service
   - Statistics Service
   - Guild Service

3. **Type Definitions**:
   - Define TypeScript/JavaScript interfaces for all API requests and responses
   - Implement validation for request data

#### 2.2 Authentication Management

Implement secure token storage and management:
- Store the API token securely (not in client-side code)
- Use environment variables or a secure backend service
- Implement token refresh mechanism if needed
- Add authentication headers to all requests

#### 2.3 Error Handling

Implement comprehensive error handling:
- HTTP error handling (4xx, 5xx responses)
- Network error handling (timeouts, connection issues)
- API-specific error handling (based on error codes)
- User-friendly error messages
- Logging for debugging

### 3. Frontend Feature Implementation

Map frontend features to API endpoints:

#### 3.1 User Management
- Login/authentication system
- User profile management
- Character creation and management

#### 3.2 Game World Interaction
- Object interaction
- Character movement and actions
- Environment rendering

#### 3.3 Communication
- Chat system
- Messaging
- Mail system

#### 3.4 Administration (if applicable)
- Server configuration
- User management
- Statistics and monitoring

### 4. Performance Optimization

Implement strategies to optimize API usage:
- Caching frequently accessed data
- Pagination for large data sets
- Optimistic UI updates
- Batching requests where possible
- Implementing retry logic for failed requests

## Technical Considerations

### Security
- Use HTTPS for all API communication
- Implement proper token management
- Consider rate limiting to prevent abuse
- Validate all user inputs
- Implement proper error handling to prevent information leakage

### Cross-Origin Resource Sharing (CORS)
- Configure CORS if the frontend is hosted on a different domain
- Limit CORS to specific origins

### State Management
- Implement a robust state management solution (Redux, MobX, etc.)
- Consider using a normalized store for efficient data access
- Implement optimistic updates for better user experience

### Testing
- Create unit tests for API client functions
- Implement integration tests for API interactions
- Consider mocking the API for frontend development

## Implementation Roadmap

### Phase 1: API Configuration and Testing
- Set up API configuration
- Generate SSL certificates
- Test API connectivity
- Document available endpoints

### Phase 2: Core API Client Development
- Implement authentication
- Create base API client
- Develop error handling
- Implement logging

### Phase 3: Resource-specific Services
- Implement Player/Account service
- Implement Object service
- Implement Chat service
- Implement other services as needed

### Phase 4: Frontend Integration
- Connect frontend components to API services
- Implement state management
- Develop UI components
- Test end-to-end functionality

### Phase 5: Optimization and Refinement
- Performance optimization
- Security hardening
- User experience improvements
- Documentation and maintenance

## Conclusion

The Core3 REST API provides a comprehensive set of endpoints for interacting with the Star Wars Galaxies server. By following the integration plan outlined in this document, a frontend application can be successfully integrated with the API to provide a complete user experience.

The key to successful integration will be proper configuration of the API, secure authentication, comprehensive error handling, and efficient state management in the frontend application.
