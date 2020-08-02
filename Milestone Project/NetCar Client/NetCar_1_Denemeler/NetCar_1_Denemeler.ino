#include "BLEDevice.h"



static BLEUUID Node_Characteristic[5] ={BLEUUID ("ceeb443a-0a2c-40dd-866e-2ef043cc3ece"),
                                        BLEUUID ("f59f1130-1a0f-4596-b7de-7758e131fd14"),
                                        BLEUUID ("1d574db9-0fe5-4322-8484-8bca07925921"),
                                        BLEUUID ("75711100-a76a-4d5b-9cd8-a5d58c921d0e"),
                                        BLEUUID ("d38a2f06-d8c2-4a59-8990-378b7238f118")};


static BLEUUID Node_Service[5] = {BLEUUID ("4fafc201-1fb5-459e-8fcc-c5c9c331914b"),
                                  BLEUUID ("091825a1-2af8-495c-85e9-56a59cc9828e"),
                                  BLEUUID ("8ebdc495-40c8-486f-a769-700feceece08"),
                                  BLEUUID ("637e7205-0871-467d-8dd2-bcc8d7fa00d1"),
                                  BLEUUID ("f9fe2080-a056-4dfb-a804-c4dac3823e4c")};

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;



static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    BLERemoteService* pRemoteService;
     for(int i =0; i < 5 ; i++)
    {
      pRemoteService = pClient->getService(Node_Service[i]);
      if (pRemoteService == nullptr) 
      {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(Node_Service[i].toString().c_str());
        delay(20);
        continue;
      }
      else if (pRemoteService != nullptr)
      {
        break;
      }
    }




    
    /*
    // Obtain a reference to the service we are after in the remote BLE server.
    for(int i =0; i < 5 ; i++)
    {
      pRemoteService = pClient->getService(Node_Service[i]);
      if (pRemoteService == nullptr) 
      {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(Node_Service[i].toString().c_str());
        continue;
      }
      else if (pRemoteService != nullptr)
      {
        break;
      }
    }
    */
    /*
    //BLERemoteService* pRemoteService = pClient->getService(Node_Service[1]);
    pRemoteService = pClient->getService(Node_Service[1]);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(Node_Service[1].toString().c_str());
      pClient->disconnect();
      return false;
    }
    */
    Serial.println(" - Found our service");

    for(int i = 0; i < 5; i++)
    {
      pRemoteCharacteristic = pRemoteService->getCharacteristic(Node_Characteristic[i]);
      if(pRemoteCharacteristic == nullptr)
      {
        Serial.print("Failed to find our Characteristic UUID: ");
        Serial.println(Node_Characteristic[i].toString().c_str());
        continue;
      }
      else if(pRemoteCharacteristic != nullptr)
      {
        break;
      }
    }
    
    /*
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(Node1_Characteristic);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(Node1_Characteristic.toString().c_str());
      pClient->disconnect();
      return false;
    }
    */
    
    Serial.println(" - Found our characteristic");
    /*
    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }
    */
    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    for(int i = 0; i < 5 ; i++) {

      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(Node_Service[i])) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

      } // Found our server
    } // for

    /*
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(Node_Service[0])) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
    */
    
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("NetCar_2");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  /*
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  */
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.


// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    std:: string data_received=pRemoteCharacteristic->readValue();
    char veri[data_received.length()];
    int counter = 0;
    
     if(data_received.length() > 0) {
        for(int i=0;i<data_received.length();i++) {
          Serial.print(String(data_received[i]));
        }
    }
    
    Serial.println("");
    
  }/*
  else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }*/
  
  delay(500); // Delay a second between loops.
} // End of loop
