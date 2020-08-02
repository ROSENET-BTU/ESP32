#include "BLEDevice.h"
#include <string.h>


//static BLEUUID Node1_Service("4fafc201-1fb5-459e-8fcc-c5c9c331914b"); // The remote service we wish to connect to.
static BLEUUID Node1_Characteristic("f59f1130-1a0f-4596-b7de-7758e131fd14"); // The characteristic of the remote service we are interested in.

//static BLEUUID Node2_Service("4fafc201-1fb5-459e-8fcc-c5c9c331914b"); // The remote service we wish to connect to.
static BLEUUID Node2_Characteristic("beb5483e-36e1-4688-b7f5-ea07361b26a8"); // The characteristic of the remote service we are interested in.

//static BLEUUID Node3_Service("4fafc201-1fb5-459e-8fcc-c5c9c331914b"); // The remote service we wish to connect to.
static BLEUUID Node3_Characteristic("beb5483e-36e1-4688-b7f5-ea07361b26a8"); // The characteristic of the remote service we are interested in.

//static BLEUUID Node4_Service("4fafc201-1fb5-459e-8fcc-c5c9c331914b"); // The remote service we wish to connect to.
static BLEUUID Node4_Characteristic("beb5483e-36e1-4688-b7f5-ea07361b26a8"); // The characteristic of the remote service we are interested in.

//static BLEUUID Node5_Service("4fafc201-1fb5-459e-8fcc-c5c9c331914b"); // The remote service we wish to connect to.
static BLEUUID Node5_Characteristic("beb5483e-36e1-4688-b7f5-ea07361b26a8"); // The characteristic of the remote service we are interested in.


static BLEUUID Node_Service[5] = {BLEUUID ("4fafc201-1fb5-459e-8fcc-c5c9c331914b"),
                                  BLEUUID ("146c31a5-6e34-41d5-ae8f-2edce5eebee6"),
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

    // Obtain a reference to the service we are after in the remote BLE server.
    for(int i =0; i < 6 ; i++)
    {
      pRemoteService = pClient->getService(Node_Service[i]);
      if (pRemoteService == nullptr && i!=5) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(Node_Service[0].toString().c_str());
      continue;
      }
      else if (pRemoteService != nullptr && i!=5)
      {
        break;
      }
      else if(i == 5)
      {
        Serial.println("We couldn't find any Service. Gomen >.<");
        pClient->disconnect();
        return false;
      }
    }
    /*
    BLERemoteService* pRemoteService = pClient->getService(Node_Service[0]);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(Node_Service[0].toString().c_str());
      pClient->disconnect();
      return false;
    }
    */
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(Node1_Characteristic);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(Node1_Characteristic.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

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
  BLEDevice::init("NetCar_1");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
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
          Serial.print(data_received[i]);
          veri[i]=data_received[i];
          if(counter >= 10)
          {
            for(int j=0;i<sizeof(veri);j++)
            {
              Serial.println("Veri : " + veri[j]);
            }
          }
          counter++;
        }
    }
    Serial.println("");
    //Serial.println("String : " + veri );
    //Serial.println("Data received is : " + data_received);
    
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  delay(500); // Delay a second between loops.
} // End of loop
