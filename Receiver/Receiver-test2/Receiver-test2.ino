
// Receiving Text (Receiver)
// Library: TMRh20/RF24 (https://github.com/tmrh20/RF24/)

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>


// MY RECEIVER IS THE NODEMCU 1.0(ESP 12E)
//CONFIGURE THE CE,CSN PARAMETERS DEPENDING ON YOUR BOARD'S PINS
RF24 radio(2, 4); // CE, CSN
const byte address[10] = "ADDRESS01";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(false);
  radio.printDetails();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
};

void loop() {
  if (radio.available()) {
    char txt[32] = "";
    radio.read(&txt, sizeof(txt));
    Serial.println(txt);
  }
//  else {
//    Serial.println("No radio available");
//    delay(1000);
//  }
};
