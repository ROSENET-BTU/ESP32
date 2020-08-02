#include <Wire.h>

void setup() {
 Serial.begin(9600); /* begin serial for debug */
 Wire.begin(1); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
 Wire.onRequest(requestEvent);
 pinMode(2,INPUT);
}

void loop() {
 delay(100);
}
void requestEvent() {
 if(digitalRead(2) == HIGH )
 {
  Wire.write("H");
  Serial.println("H");
 }
 else if(digitalRead(2) == LOW)
 {
  Wire.write("L");
  Serial.println("L");
 }
}
