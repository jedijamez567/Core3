/**
 * Core3 REST API Client Example (TypeScript Version)
 * 
 * This file demonstrates how to interact with the Core3 REST API from a frontend application
 * with TypeScript type safety.
 */

// API Response Types
interface APIResponse {
  status: string;
  status_code: number;
  [key: string]: any;
}

interface VersionResponse extends APIResponse {
  api_version: number;
  core3_version: string;
}

interface CharacterLookupResponse extends APIResponse {
  metadata: {
    exportTime: string;
    objectCount: number;
    maxDepth: number;
    recursive: boolean;
    msSearch: number;
    msExport: number;
    offset: number;
    limit: number;
    total: number;
    resultsRemaining: number;
  };
  names: Record<string, number>; // Character name to object ID mapping
  objects?: Record<string, any>; // Only present in find mode
}

interface MailData {
  sender: string;
  recipient: string;
  subject: string;
  body: string;
}

interface MailResponse extends APIResponse {
  action: string;
  recipient: string;
  success: boolean;
}

interface ConsoleCommandResponse extends APIResponse {
  console_command: string;
}

interface ObjectResponse extends APIResponse {
  object: {
    _oid: number;
    _className: string;
    [key: string]: any;
  };
}

interface ObjectUpdateResponse extends APIResponse {
  object: {
    _oid: number;
    _className: string;
    property: string;
    value: any;
  };
}

/**
 * Custom error class for API errors
 */
class APIError extends Error {
  status: number;
  data: any;

  /**
   * Create a new API error
   * @param message - Error message
   * @param status - HTTP status code
   * @param data - Additional error data
   */
  constructor(message: string, status: number, data: any) {
    super(message);
    this.name = 'APIError';
    this.status = status;
    this.data = data;
  }
}

/**
 * Core3 REST API Client
 */
class Core3APIClient {
  private baseUrl: string;
  private apiToken: string;

  /**
   * Create a new API client instance
   * @param baseUrl - The base URL of the API (e.g., 'https://localhost:44443')
   * @param apiToken - The API token for authentication
   */
  constructor(baseUrl: string, apiToken: string) {
    this.baseUrl = baseUrl;
    this.apiToken = apiToken;
  }

  /**
   * Make an authenticated request to the API
   * @param endpoint - The API endpoint (e.g., '/v1/version/')
   * @param method - The HTTP method (GET, POST, PUT, DELETE)
   * @param body - The request body (for POST/PUT requests)
   * @returns The API response as a JSON object
   */
  async request<T extends APIResponse>(
    endpoint: string, 
    method: string, 
    body: any = null
  ): Promise<T> {
    const url = `${this.baseUrl}${endpoint}`;
    
    const headers: HeadersInit = {
      'Authorization': `Bearer ${this.apiToken}`,
      'Content-Type': 'application/json'
    };

    const options: RequestInit = {
      method,
      headers,
      // In a real implementation, you'd handle SSL verification properly
      // This is just a placeholder comment as TypeScript fetch doesn't have this option
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
      
      return await response.json() as T;
    } catch (error) {
      if (error instanceof APIError) {
        throw error;
      }
      
      // Handle network errors
      throw new APIError(
        `Network error: ${(error as Error).message}`,
        0,
        { originalError: error }
      );
    }
  }

  /**
   * Get the API version information
   * @returns Version information
   */
  async getVersion(): Promise<VersionResponse> {
    return this.request<VersionResponse>('/v1/version/', 'GET');
  }

  /**
   * Look up a character by name
   * @param name - The character name to look up
   * @returns Character information
   */
  async lookupCharacter(name: string): Promise<CharacterLookupResponse> {
    return this.request<CharacterLookupResponse>(
      `/v1/lookup/character/?name=${encodeURIComponent(name)}`, 
      'GET'
    );
  }

  /**
   * Search for characters by name prefix
   * @param searchTerm - The search term (minimum 3 characters)
   * @param limit - Maximum number of results to return
   * @param offset - Offset for pagination
   * @returns Search results
   */
  async searchCharacters(
    searchTerm: string, 
    limit: number = 20, 
    offset: number = 0
  ): Promise<CharacterLookupResponse> {
    if (searchTerm.length < 3) {
      throw new Error('Search term must be at least 3 characters');
    }
    
    return this.request<CharacterLookupResponse>(
      `/v1/find/character/?search=${encodeURIComponent(searchTerm)}&limit=${limit}&offset=${offset}`,
      'GET'
    );
  }

  /**
   * Execute a console command
   * @param command - The command to execute
   * @param args - Command arguments
   * @returns Command result
   */
  async executeConsoleCommand(
    command: string, 
    args: string = ''
  ): Promise<ConsoleCommandResponse> {
    return this.request<ConsoleCommandResponse>(
      `/v1/admin/console/${command}/?args=${encodeURIComponent(args)}`,
      'POST'
    );
  }

  /**
   * Send a mail message
   * @param mailData - Mail data
   * @returns Mail send result
   */
  async sendMail(mailData: MailData): Promise<MailResponse> {
    return this.request<MailResponse>('/v1/chat/mail/', 'POST', mailData);
  }

  /**
   * Get an object by ID
   * @param objectId - The object ID
   * @returns Object data
   */
  async getObject(objectId: number): Promise<ObjectResponse> {
    return this.request<ObjectResponse>(`/v1/object/${objectId}/`, 'GET');
  }

  /**
   * Update an object property
   * @param objectId - The object ID
   * @param className - The object class name
   * @param property - The property to update
   * @param value - The new property value
   * @returns Update result
   */
  async updateObjectProperty(
    objectId: number, 
    className: string, 
    property: string, 
    value: any
  ): Promise<ObjectUpdateResponse> {
    return this.request<ObjectUpdateResponse>(
      `/v1/object/${objectId}/${className}/${property}/`,
      'PUT',
      { value }
    );
  }
}

/**
 * Example usage of the API client
 */
async function exampleUsage(): Promise<void> {
  try {
    // Create a new API client
    const apiClient = new Core3APIClient(
      'https://localhost:44443',
      'YOUR_API_TOKEN_HERE'
    );

    // Get API version
    const versionInfo = await apiClient.getVersion();
    console.log('API Version:', versionInfo.api_version);
    console.log('Core3 Version:', versionInfo.core3_version);

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

// Export for use in a module environment
export {
  Core3APIClient,
  APIError,
  APIResponse,
  VersionResponse,
  CharacterLookupResponse,
  MailData,
  MailResponse,
  ConsoleCommandResponse,
  ObjectResponse,
  ObjectUpdateResponse
};
