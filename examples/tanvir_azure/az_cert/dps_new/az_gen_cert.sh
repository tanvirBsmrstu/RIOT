#!/bin/bash

mkdir certs csr newcerts private
touch index.txt
openssl rand -hex 16 > serial

DeviceID="riot-thesis"

openssl genrsa -aes256 -passout pass:1234 -out ./private/azure-iot-test-only.root.ca.key.pem 4096
openssl req -new -x509 -config ./openssl_root_ca.cnf -passin pass:1234 -key ./private/azure-iot-test-only.root.ca.key.pem -subj '/CN=Azure IoT Hub CA Cert Test Only' -days 30 -sha256 -extensions v3_ca -out ./certs/azure-iot-test-only.root.ca.cert.pem
echo "-------------------------------"
openssl x509 -noout -text -in ./certs/azure-iot-test-only.root.ca.cert.pem
echo "------------Intermediate cert-------------------"
openssl genrsa -aes256 -passout pass:1234 -out ./private/azure-iot-test-only.intermediate.key.pem 4096
openssl req -new -sha256 -passin pass:1234 -config ./openssl_device_intermediate_ca.cnf -subj '/CN=Azure IoT Hub Intermediate Cert Test Only' -key ./private/azure-iot-test-only.intermediate.key.pem -out ./csr/azure-iot-test-only.intermediate.csr.pem
openssl ca -batch -config ./openssl_root_ca.cnf -passin pass:1234 -extensions v3_intermediate_ca -days 30 -notext -md sha256 -in ./csr/azure-iot-test-only.intermediate.csr.pem -out ./certs/azure-iot-test-only.intermediate.cert.pem
openssl x509 -noout -text -in ./certs/azure-iot-test-only.intermediate.cert.pem
echo "-------------Device cert-----------------"
openssl genrsa -out ./private/device-01.key.pem 4096
openssl req -config ./openssl_device_intermediate_ca.cnf -key ./private/device-01.key.pem -subj "/CN=$DeviceID" -new -sha256 -out ./csr/device-01.csr.pem
openssl ca -batch -config ./openssl_device_intermediate_ca.cnf -passin pass:1234 -extensions usr_cert -days 30 -notext -md sha256 -in ./csr/device-01.csr.pem -out ./certs/device-01.cert.pem
openssl x509 -noout -text -in ./certs/device-01.cert.pem
echo "---------------Chain----------------"
cat ./certs/device-01.cert.pem ./certs/azure-iot-test-only.intermediate.cert.pem ./certs/azure-iot-test-only.root.ca.cert.pem > ./certs/device-01-full-chain.cert.pem