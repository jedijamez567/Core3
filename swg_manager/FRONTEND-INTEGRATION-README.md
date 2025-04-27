# Core3 REST API Frontend Integration

This README provides instructions for integrating a frontend application with the Core3 REST API for Star Wars Galaxies.

## Files Overview

The following files have been created to help with the integration:

1. **DevInformation.md** - Comprehensive documentation about the Core3 REST API and integration plan
2. **MMOCoreORB/bin/conf/config-local.lua** - Configuration file for enabling and configuring the REST API
3. **generate-ssl-certs.sh** - Shell script to generate SSL certificates required by the API
4. **test-api-connection.js** - Node.js script to test the API connection
5. **frontend-api-client-example.js** - JavaScript example of an API client
6. **frontend-api-client-example.ts** - TypeScript version of the API client with type definitions

## Setup Instructions

### 1. Configure the REST API

1. Ensure the `config-local.lua` file is in the correct location: `MMOCoreORB/bin/conf/config-local.lua`
2. Update the API token in the configuration file with a secure token

### 2. Generate SSL Certificates

1. Make the script executable:
   ```bash
   chmod +x generate-ssl-certs.sh
   ```

2. Run the script:
   ```bash
   ./generate-ssl-certs.sh
   ```

3. The script will:
   - Generate SSL key and certificate in `MMOCoreORB/bin/conf/`
   - Generate a secure API token and save it to `MMOCoreORB/bin/conf/api_token.txt`
   - Display the configuration to add to `config-local.lua`

### 3. Restart the Core3 Server

After configuring the API and generating certificates, restart the Core3 server to apply the changes.

### 4. Test the API Connection

1. Update the API token in `test-api-connection.js` if needed (the script will try to read it from the token file)
2. Run the test script:
   ```bash
   node test-api-connection.js
   ```
3. Verify that the connection is successful

### 5. Implement the Frontend API Client

Use the provided example API clients as a starting point for your frontend integration:

- For JavaScript projects: `frontend-api-client-example.js`
- For TypeScript projects: `frontend-api-client-example.ts`

## API Endpoints

The Core3 REST API provides the following endpoints:

- **Version Information**: `GET:/v1/version/`
- **Object Management**: 
  - `GET|DELETE:/v1/object/(?:(\\d*)/|)`
  - `PUT:/v1/object/(\\d+)/(\\w+)/(\\w+)/`
- **Configuration**: `GET|POST|PUT:/v1/admin/config/(?:(.*)/|)`
- **Statistics**: `GET|PUT:/v1/admin/stats/`
- **Console Commands**: `POST:/v1/admin/console/(\\w+)/`
- **Account/Character Management**: 
  - `POST:/v1/admin/account/(\\d+)/galaxy/(\\d+)/character/(\\d+)/`
  - `POST:/v1/admin/account/(\\d+)/`
- **Lookups**:
  - `GET:/v1/(find|lookup)/character/`
  - `GET:/v1/(find|lookup)/guild/`
- **Chat**: `POST:/v1/chat/(mail|message|galaxy)/`

For detailed information about each endpoint, refer to the `DevInformation.md` file.

## Security Considerations

1. **API Token**: Store the API token securely and never expose it in client-side code
2. **HTTPS**: Always use HTTPS for API communication
3. **Input Validation**: Validate all user inputs before sending to the API
4. **Error Handling**: Implement proper error handling for API responses

## Next Steps

1. Implement the API client in your frontend application
2. Create UI components that interact with the API
3. Implement state management for API data
4. Add error handling and loading states
5. Test the integration thoroughly

For a more detailed integration plan, refer to the `DevInformation.md` file.
