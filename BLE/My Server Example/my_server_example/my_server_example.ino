#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std:: string ledValue = pCharacteristic->getValue();

      if(ledValue.length() > 0) {
        for(int i=0;i<ledValue.length();i++) {
          Serial.print(ledValue[i]);
        }
        Serial.println("");
        if(ledValue.find("H") != -1) {
          digitalWrite(LED_BUILTIN,HIGH);
        }
        else if (ledValue.find("L") != -1) {
          digitalWrite(LED_BUILTIN,LOW);
        }
      }
    }
};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  BLEDevice::init("BURAK");

  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                     CHARACTERISTIC_UUID,
                                     BLECharacteristic::PROPERTY_WRITE
                                     );

 pCharacteristic->setCallbacks(new MyCallbacks());

 pService->start();

 BLEAdvertising *pAdvertising = pServer->getAdvertising();
 pAdvertising->addServiceUUID(SERVICE_UUID);
 pAdvertising->setScanResponse(true);
 BLEDevice::startAdvertising();

}

void loop() {
  // put your main code here, to run repeatedly:

}
