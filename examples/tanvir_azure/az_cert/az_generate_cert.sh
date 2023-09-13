#!/bin/bash

KeyFile="privateKey.pem"
CsrFile="cert_csr.pem"
DeviceID="riot"  # Set your desired DeviceID here
PassPhrase=""		#passphrase 1234 for az dps
CertFile="cert.pem"  # Corrected the variable assignment by removing spaces around '='

openssl genpkey -out "$KeyFile" -algorithm RSA -pkeyopt rsa_keygen_bits:2048

if [ -n "$PassPhrase" ]; then
    openssl req -new -key "$KeyFile" -out "$CsrFile" -subj "/CN=$DeviceID" -passin pass:"$PassPhrase"
else
    openssl req -new -key "$KeyFile" -out "$CsrFile" -subj "/CN=$DeviceID"
fi

# Verify CSR
openssl req -text -in "$CsrFile" -verify -noout

# Generate certificate
openssl x509 -req -days 365 -in "$CsrFile" -signkey "$KeyFile" -out "$CertFile"

# Retrieve the fingerprint
openssl x509 -in "$CertFile" -noout -fingerprint

