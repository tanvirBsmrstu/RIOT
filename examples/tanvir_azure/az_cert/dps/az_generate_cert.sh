#!/bin/bash


ROOT_CA_KEY="root_ca_key.pem"
ROOT_CA_CERT="root_ca_cert.pem" 

Verification_Cert_Key="verification_cert_key.pem"
Verification_Cert_CERT="verification_cert.pem"
Verification_Cert_CSR="verification_cert_csr.pem"

Device_Cert_Key="iVdevice_cert_key.pem"
Device_Cert_CSR="iVdevice_cert_csr.pem"
Device_Cert_CERT="iVdevice_cert_cert.pem"

KeyFile="privateKey.pem"
CsrFile="cert_csr.pem"
Device_ID="riot"  # Set your desired DeviceID here
PassPhrase="1234"		#passphrase 1234 for az dps
CertFile="cert.pem"  # Corrected the variable assignment by removing spaces around '='



if [ "$1" == "get_root_cert" ]; then
    echo "You specified 'get_root_cert' as the first parameter."
    # Generate the root key
    openssl genpkey -algorithm RSA -out "$ROOT_CA_KEY" -pkeyopt rsa_keygen_bits:2048

    # Create a root certificate
    openssl req -new -x509 -key "$ROOT_CA_KEY" -out "$ROOT_CA_CERT" -subj "/CN=ROOT_CA_CERT"
    sudo chmod 774 "$ROOT_CA_CERT"
    
elif [ "$1" == "verify_cert" ]; then
    echo "You specified 'verify_cert' as the first parameter."
    #get the verification code
    echo "please enter the verification code : "
    read VerificationCode
    openssl genpkey -out "$Verification_Cert_Key" -algorithm RSA -pkeyopt rsa_keygen_bits:2048
    if [ -n "$PassPhrase" ]; then
        openssl req -new -key "$Verification_Cert_Key" -out "$Verification_Cert_CSR" -subj "/CN=$VerificationCode" -passin pass:"$PassPhrase"
    else
        openssl req -new -key "$Verification_Cert_Key" -out "$Verification_Cert_CSR" -subj "/CN=$VerificationCode"
    fi
    #sign verification certificate
    openssl x509 -req -in "$Verification_Cert_CSR" -CA "$ROOT_CA_CERT" -CAkey "$ROOT_CA_KEY" -CAcreateserial -out "$Verification_Cert_CERT" -days 365
    sudo chmod 774 "$Verification_Cert_CERT"
elif [ "$1" == "device_cert" ]; then
    ### generate device certificate
    echo "You specified 'device_cert' as the first parameter."
    #get the verification code
    echo "please enter the enrollment id : "
    read Device_ID
    openssl genpkey -out "$Device_Cert_Key" -algorithm RSA -pkeyopt rsa_keygen_bits:2048
    if [ -n "$PassPhrase" ]; then
        openssl req -new -key "$Device_Cert_Key" -out "$Device_Cert_CSR" -subj "/CN=$Device_ID" -passin pass:"$PassPhrase"
    else
        openssl req -new -key "$Device_Cert_Key" -out "$Device_Cert_CSR" -subj "/CN=$Device_ID"
    fi

    #sign verification certificate
    #openssl x509 -req -in "$Device_Cert_CSR" -CA "$ROOT_CA_CERT" -CAkey "$ROOT_CA_KEY" -CAcreateserial -out "$Device_Cert_CERT" -days 365
    openssl x509 -req -in "$Device_Cert_CSR" -CA "$Verification_Cert_CERT" -CAkey "$Verification_Cert_Key" -CAcreateserial -out "$Device_Cert_CERT" -days 365
    #sudo chmod 774 "$Device_Cert_CERT"
else
    echo "The first parameter is neither 'get_root_cert' nor 'another_command'."
    # Add code here for handling other cases
fi


#Generating root ca key and certificate
# Generate the root key
#openssl genpkey -algorithm RSA -out "$ROOT_CA_KEY" -pkeyopt rsa_keygen_bits:2048

# Create a root certificate
#openssl req -new -x509 -key "$ROOT_CA_KEY" -out "$ROOT_CA_CERT" -subj "/CN=ROOT_CA_CERT"

#openssl genpkey -out "$ROOT_CA_KEY" -algorithm RSA -pkeyopt rsa_keygen_bits:2048
#openssl req -new -key "$ROOT_CA_KEY" -out "$ROOT_CA_CERT" -subj "/CN=ROOT_CA_CERT" 



# Retrieve the fingerprint
#openssl x509 -in "$CertFile" -noout -fingerprint






