
// Sending Text (Transmitter)
// Library: TMRh20/RF24 (https://github.com/tmrh20/RF24/)

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

// MY TRANSMITTER IS THE ESP32
//CONFIGURE THE CE,CSN PARAMETERS DEPENDING ON YOUR BOARD'S PINS
RF24 radio(4, 5); // CE, CSN
const byte address[10] = "ADDRESS01";

void setup() {
  Serial.begin(9600);
   if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  radio.setAutoAck(false);
  radio.printDetails();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  
};

void loop() {
  const char txt[] = "Hello World";
  radio.write(&txt, sizeof(txt));
  delay(1000);
};
