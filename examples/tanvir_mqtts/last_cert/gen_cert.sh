#!/bin/bash
mkdir certs
cd certs
openssl genrsa -out server.key 2048
openssl genrsa -out ca.key 2048
openssl req -new -key ca.key -out ca.csr -sha256
openssl x509 -req -in ca.csr -signkey ca.key -out ca.pem -days 365 -sha256
openssl req -new -key server.key -out server.csr -sha256
echo "Wait .......signing now....."
openssl x509 -req -in server.csr -CA ca.pem -CAkey ca.key -CAcreateserial -out server.pem -days 360
sudo chmod 777 ca.pem
sudo chmod 777 server.key
sudo chmod 777 server.pem

sudo cp ca.pem /etc/mosquitto/ca_certificates/
sudo cp server.key /etc/mosquitto/certs/
sudo cp server.pem /etc/mosquitto/certs/



