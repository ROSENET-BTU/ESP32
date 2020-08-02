#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "091825a1-2af8-495c-85e9-56a59cc9828e"
#define CHARACTERISTIC_UUID "f59f1130-1a0f-4596-b7de-7758e131fd14"

int number=0;
String str_number;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
      //std::string value = pCharacteristic->getValue();
      //std:: string str_number = String(number);
      str_number = String(number);
      pCharacteristic->setValue(str_number.c_str());
      Serial.println("Number sent was :" + String(number));
      number++;
      delay(500);
    }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init("Node_1");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Nothing");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
}
