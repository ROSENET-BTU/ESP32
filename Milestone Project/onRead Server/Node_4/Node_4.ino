#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "637e7205-0871-467d-8dd2-bcc8d7fa00d1"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

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
  delay(2000);
}
