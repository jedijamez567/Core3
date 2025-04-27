/**
 * Core3 REST API Client Example
 * 
 * This file demonstrates how to interact with the Core3 REST API from a frontend application.
 * It includes examples of authentication, error handling, and making requests to various endpoints.
 */

class Core3APIClient {
  /**
   * Create a new API client instance
   * @param {string} baseUrl - The base URL of the API (e.g., 'https://localhost:44443')
   * @param {string} apiToken - The API token for authentication
   */
  constructor(baseUrl, apiToken) {
    this.baseUrl = baseUrl;
    this.apiToken = apiToken;
  }

  /**
   * Make an authenticated request to the API
   * @param {string} endpoint - The API endpoint (e.g., '/v1/version/')
   * @param {string} method - The HTTP method (GET, POST, PUT, DELETE)
   * @param {object} [body] - The request body (for POST/PUT requests)
   * @returns {Promise<object>} - The API response as a JSON object
   */
  async request(endpoint, method, body = null) {
    const url = `${this.baseUrl}${endpoint}`;
    
    const headers = {
      'Authorization': `Bearer ${this.apiToken}`,
      'Content-Type': 'application/json'
    };

    const options = {
      method,
      headers,
      // Skip SSL verification in development (remove in production)
      // This is only for testing with self-signed certificates
      rejectUnauthorized: false
    };

    if (body && (method === 'POST' || method === 'PUT')) {
      options.body = JSON.stringify(body);
    }

    try {
      const response = await fetch(url, options);
      
      // Handle HTTP errors
      if (!response.ok) {
        const errorData = await response.json().catch(() => ({}));
        throw new APIError(
          `API request failed with status ${response.status}`,
          response.status,
          errorData
        );
      }
      
      return await response.json();
    } catch (error) {
      if (error instanceof APIError) {
        throw error;
      }
      
      // Handle network errors
      throw new APIError(
        `Network error: ${error.message}`,
        0,
        { originalError: error }
      );
    }
  }

  /**
   * Get the API version information
   * @returns {Promise<object>} - Version information
   */
  async getVersion() {
    return this.request('/v1/version/', 'GET');
  }

  /**
   * Look up a character by name
   * @param {string} name - The character name to look up
   * @returns {Promise<object>} - Character information
   */
  async lookupCharacter(name) {
    return this.request(`/v1/lookup/character/?name=${encodeURIComponent(name)}`, 'GET');
  }

  /**
   * Search for characters by name prefix
   * @param {string} searchTerm - The search term (minimum 3 characters)
   * @param {number} [limit=20] - Maximum number of results to return
   * @param {number} [offset=0] - Offset for pagination
   * @returns {Promise<object>} - Search results
   */
  async searchCharacters(searchTerm, limit = 20, offset = 0) {
    if (searchTerm.length < 3) {
      throw new Error('Search term must be at least 3 characters');
    }
    
    return this.request(
      `/v1/find/character/?search=${encodeURIComponent(searchTerm)}&limit=${limit}&offset=${offset}`,
      'GET'
    );
  }

  /**
   * Execute a console command
   * @param {string} command - The command to execute
   * @param {string} [args] - Command arguments
   * @returns {Promise<object>} - Command result
   */
  async executeConsoleCommand(command, args = '') {
    return this.request(
      `/v1/admin/console/${command}/?args=${encodeURIComponent(args)}`,
      'POST'
    );
  }

  /**
   * Send a mail message
   * @param {object} mailData - Mail data
   * @param {string} mailData.sender - Sender name
   * @param {string} mailData.recipient - Recipient name
   * @param {string} mailData.subject - Mail subject
   * @param {string} mailData.body - Mail body
   * @returns {Promise<object>} - Mail send result
   */
  async sendMail(mailData) {
    return this.request('/v1/chat/mail/', 'POST', mailData);
  }

  /**
   * Get an object by ID
   * @param {number} objectId - The object ID
   * @returns {Promise<object>} - Object data
   */
  async getObject(objectId) {
    return this.request(`/v1/object/${objectId}/`, 'GET');
  }

  /**
   * Update an object property
   * @param {number} objectId - The object ID
   * @param {string} className - The object class name
   * @param {string} property - The property to update
   * @param {any} value - The new property value
   * @returns {Promise<object>} - Update result
   */
  async updateObjectProperty(objectId, className, property, value) {
    return this.request(
      `/v1/object/${objectId}/${className}/${property}/`,
      'PUT',
      { value }
    );
  }
}

/**
 * Custom error class for API errors
 */
class APIError extends Error {
  /**
   * Create a new API error
   * @param {string} message - Error message
   * @param {number} status - HTTP status code
   * @param {object} data - Additional error data
   */
  constructor(message, status, data) {
    super(message);
    this.name = 'APIError';
    this.status = status;
    this.data = data;
  }
}

/**
 * Example usage of the API client
 */
async function exampleUsage() {
  try {
    // Create a new API client
    const apiClient = new Core3APIClient(
      'https://localhost:44443',
      'YOUR_API_TOKEN_HERE'
    );

    // Get API version
    const versionInfo = await apiClient.getVersion();
    console.log('API Version:', versionInfo);

    // Look up a character
    const characterInfo = await apiClient.lookupCharacter('Luke');
    console.log('Character Info:', characterInfo);

    // Search for characters
    const searchResults = await apiClient.searchCharacters('Sky');
    console.log('Search Results:', searchResults);

    // Send a mail
    const mailResult = await apiClient.sendMail({
      sender: 'Admin',
      recipient: 'Luke',
      subject: 'Welcome!',
      body: 'Welcome to the server!'
    });
    console.log('Mail Result:', mailResult);

  } catch (error) {
    console.error('API Error:', error);
    
    if (error instanceof APIError) {
      console.error('Status:', error.status);
      console.error('Error Data:', error.data);
    }
  }
}

// Uncomment to run the example
// exampleUsage();

// For use in a module environment
if (typeof module !== 'undefined' && module.exports) {
  module.exports = {
    Core3APIClient,
    APIError
  };
}
