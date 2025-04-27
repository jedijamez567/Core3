/**
 * Core3 REST API Test Script
 * 
 * This script tests the connection to the Core3 REST API by making a simple
 * request to the version endpoint. It's useful for verifying that the API
 * is properly configured and accessible.
 * 
 * Usage:
 * 1. Make sure the Core3 server is running with the REST API enabled
 * 2. Update the API_URL and API_TOKEN constants below
 * 3. Run the script with Node.js: node test-api-connection.js
 */

const https = require('https');
const fs = require('fs');

// Configuration - Update these values
const API_URL = 'https://localhost:44443';
const API_TOKEN = 'YOUR_API_TOKEN_HERE'; // Replace with your actual API token
const API_ENDPOINT = '/v1/version/';

// Read API token from file if it exists
try {
  const tokenPath = 'MMOCoreORB/bin/conf/api_token.txt';
  if (fs.existsSync(tokenPath)) {
    const token = fs.readFileSync(tokenPath, 'utf8').trim();
    if (token) {
      console.log('Using API token from file');
      API_TOKEN = token;
    }
  }
} catch (error) {
  console.warn('Could not read API token from file:', error.message);
}

// Function to make an API request
function makeApiRequest(url, token) {
  return new Promise((resolve, reject) => {
    const options = {
      headers: {
        'Authorization': `Bearer ${token}`,
        'Content-Type': 'application/json'
      },
      // For development only - disable SSL verification for self-signed certificates
      rejectUnauthorized: false
    };

    https.get(url, options, (res) => {
      let data = '';

      // A chunk of data has been received
      res.on('data', (chunk) => {
        data += chunk;
      });

      // The whole response has been received
      res.on('end', () => {
        if (res.statusCode === 200) {
          try {
            const parsedData = JSON.parse(data);
            resolve({ statusCode: res.statusCode, data: parsedData });
          } catch (error) {
            reject(new Error(`Failed to parse response: ${error.message}`));
          }
        } else {
          reject(new Error(`Request failed with status code: ${res.statusCode}`));
        }
      });
    }).on('error', (error) => {
      reject(new Error(`Request error: ${error.message}`));
    });
  });
}

// Main function to test the API connection
async function testApiConnection() {
  console.log('Testing connection to Core3 REST API...');
  console.log(`URL: ${API_URL}${API_ENDPOINT}`);
  
  try {
    const response = await makeApiRequest(`${API_URL}${API_ENDPOINT}`, API_TOKEN);
    
    console.log('\n✅ Connection successful!');
    console.log('Status code:', response.statusCode);
    console.log('Response data:');
    console.log(JSON.stringify(response.data, null, 2));
    
    if (response.data.api_version) {
      console.log(`\nAPI Version: ${response.data.api_version}`);
    }
    
    if (response.data.core3_version) {
      console.log(`Core3 Version: ${response.data.core3_version}`);
    }
    
    console.log('\nThe Core3 REST API is properly configured and accessible.');
  } catch (error) {
    console.error('\n❌ Connection failed!');
    console.error('Error:', error.message);
    console.error('\nPossible issues:');
    console.error('1. The Core3 server is not running');
    console.error('2. The REST API is not enabled in config-local.lua');
    console.error('3. The API_TOKEN is incorrect');
    console.error('4. SSL certificates are not properly configured');
    console.error('5. There is a network issue preventing the connection');
    console.error('\nPlease check your configuration and try again.');
  }
}

// Run the test
testApiConnection();
