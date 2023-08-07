#!/bin/bash

# Generate CA key
openssl genpkey -algorithm RSA -out ca.key

# Generate CA certificate (self-signed)
openssl req -x509 -new -key ca.key -out ca.crt -days 365

# Generate server key
openssl genpkey -algorithm RSA -out server.key

# Generate CSR for server certificate
openssl req -new -key server.key -out server.csr

# Sign server CSR with CA
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 365

# Generate client key
openssl genpkey -algorithm RSA -out client.key

# Generate CSR for client certificate
openssl req -new -key client.key -out client.csr

# Sign client CSR with CA
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 365

# Convert CA certificate to PEM format
openssl x509 -in ca.crt -out ca.pem

# Convert server certificate to PEM format
openssl x509 -in server.crt -out server.pem

# Convert client certificate to PEM format
openssl x509 -in client.crt -out client.pem

# Create the certificates subdirectory
mkdir -p certificates

# Move the generated files to the subdirectory
mv ca.key ca.pem server.key server.pem client.key client.pem certificates/

# Cleanup temporary files
rm ca.crt ca.srl client.crt client.csr client.key server.crt server.csr server.key
