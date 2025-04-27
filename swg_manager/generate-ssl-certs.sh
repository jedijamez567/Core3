#!/bin/bash
# Script to generate SSL certificates for Core3 REST API

# Set variables
CERT_DIR="MMOCoreORB/bin/conf"
KEY_FILE="$CERT_DIR/ssl.key"
CSR_FILE="$CERT_DIR/ssl.csr"
CRT_FILE="$CERT_DIR/ssl.crt"
API_TOKEN_FILE="$CERT_DIR/api_token.txt"

# Create directory if it doesn't exist
mkdir -p "$CERT_DIR"

echo "Generating SSL certificates for Core3 REST API..."

# Generate private key
echo "Generating private key..."
openssl genrsa -out "$KEY_FILE" 2048

# Generate certificate signing request
echo "Generating certificate signing request..."
openssl req -new -key "$KEY_FILE" -out "$CSR_FILE" -subj "/C=US/ST=NY/L=Somewhere/O=Core3/CN=Core3API"

# Generate self-signed certificate
echo "Generating self-signed certificate..."
openssl x509 -req -days 3650 -in "$CSR_FILE" -signkey "$KEY_FILE" -out "$CRT_FILE"

# Remove CSR file as it's no longer needed
rm -f "$CSR_FILE"

# Display certificate information
echo "Certificate information:"
openssl x509 -text -noout -in "$CRT_FILE"

# Generate API token
echo "Generating secure API token..."
API_TOKEN=$(openssl rand -base64 18)
echo "$API_TOKEN" > "$API_TOKEN_FILE"
echo "API token saved to $API_TOKEN_FILE"

echo ""
echo "SSL certificates and API token have been generated successfully."
echo ""
echo "To configure the Core3 REST API, update your config-local.lua file with:"
echo ""
echo "Core3.RESTServerPort = 44443"
echo "Core3.RESTServer = {"
echo "    LogLevel = 4,"
echo "    APIToken = \"$API_TOKEN\","
echo "    SSLKeyFile = \"conf/ssl.key\","
echo "    SSLCertFile = \"conf/ssl.crt\","
echo "}"
echo ""
echo "Remember to restart your server after making these changes."
