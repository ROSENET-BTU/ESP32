#include <Wire.h>
char c;
void setup() {
 Wire.begin(1);                /* join i2c bus with address 8 */
 Wire.onReceive(receiveEvent); /* register receive event */
 Wire.onRequest(requestEvent); /* register request event */
 Serial.begin(9600);           /* start serial for debug */
 pinMode(2,OUTPUT);
}

void loop() {
 delay(100);
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
 while (0 <Wire.available()) {
    c = Wire.read();      /* receive byte as a character */
    Serial.print(c);           /* print the character */
  }
 Serial.println();             /* to newline */
 if(c=='H')
  digitalWrite(2,HIGH);
 if(c=='L')
  digitalWrite(2,LOW) 
}

// function that executes whenever data is requested from master
