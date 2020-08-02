#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <FirebaseESP32.h>
 
#define FIREBASE_HOST "" 
#define FIREBASE_AUTH ""
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
 
AsyncWebServer server(80);
FirebaseData firebaseData;
 
void setup(){
  Serial.begin(115200);
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    int paramsNr = request->params();
    Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
 
        AsyncWebParameter* p = request->getParam(i);
        Serial.print("Param name: ");
        Serial.println(p->name());
        Serial.print("Param value: ");
        Serial.println(p->value());
        Serial.println("------");
        if (Firebase.set(firebaseData, "/test/" + p->name() , p->value())) {
          Serial.println("PASSED");
        } else {
          Serial.println(firebaseData.errorReason());
        }
    }
 
    request->send(200, "text/plain", "message received");
  });
 
  server.begin();
}
 
void loop(){
}
