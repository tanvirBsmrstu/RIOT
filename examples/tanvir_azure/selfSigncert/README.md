DPS [https://learn.microsoft.com/en-us/azure/iot/iot-mqtt-connect-to-iot-dps]
username=0ne00B1BDDA/registrations/riot-registrationID/api-version=2019-03-31
ClientId=riot-registrationID


device registration
publish on topic : $dps/registrations/PUT/iotdps-register/?$rid={request_id}
with payload
======
{
  "registrationId": "riot-registrationID",
  "payload": {
    "iotcModelId": "yourModelId",
    "properties": {
      "desired": {
        "customProperty1": "value1",
        "customProperty2": "value2"
      }
    }
  }
}
======
