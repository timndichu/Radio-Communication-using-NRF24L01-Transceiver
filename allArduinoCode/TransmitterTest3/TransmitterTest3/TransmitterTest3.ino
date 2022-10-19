// Sending Text (Transmitter)
// Library: TMRh20/RF24 (https://github.com/tmrh20/RF24/)

#include <SPI.h>
//NRF TRANSMITTER LIBRARIES
#include <RF24.h>
#include <nRF24L01.h>
//time library RTC
#include <ESP32Time.h>

//ESP32Time rtc;
//Initialize RTC
ESP32Time rtc(10800);  // offset in seconds GMT+3 (KENYA)

// MY TRANSMITTER IS THE ESP32
//CONFIGURE THE CE,CSN PARAMETERS DEPENDING ON YOUR BOARD'S PINS
RF24 radio(4, 5); // CE, CSN
const byte address[10] = "ADDRESS01";


void setup() {
  Serial.begin(9600);
//  SET RTC TIME
  rtc.setTime(30, 23, 11, 18, 10, 2022);  //SS:MM:HH DD/MM/YYYY
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
    //    GET CURRENT TIME
    String date = rtc.getDateTime();

    String txt = " Config.. V1.01. Tafiti";
    String dateTime = date + txt;
  
    char tab2[240];
    strncpy(tab2, dateTime.c_str(), sizeof(tab2));
    tab2[sizeof(tab2) - 1] = 0;
    Serial.println(tab2);  

    radio.write(&tab2, sizeof(tab2));
    delay(1500);
};
