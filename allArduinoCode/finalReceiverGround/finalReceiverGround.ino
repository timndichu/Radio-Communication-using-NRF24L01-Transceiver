#include <ArduinoJson.h>

// Receiving Text (Receiver)
// Library: TMRh20/RF24 (https://github.com/tmrh20/RF24/)

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Arduino.h>
#include "time.h"

const int red =  16; // d0
const int green = 5; // d1
const int blue = 0; // d3
const int led = 15; //d8
// MY RECEIVER IS THE NODEMCU 1.0(ESP 12E)
//CONFIGURE THE CE,CSN PARAMETERS DEPENDING ON YOUR BOARD'S PINS
RF24 radio(2, 4); // CE, CSN
const byte address[10] = "ADDRESS01";
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 10800;   //Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 0;  //Replace with your daylight offset (seconds)


const char* ssid = "Redmi Tim"; //Enter SSID
const char* password = "host2000"; //Enter Password
const char* websockets_server_host = "nanosat.herokuapp.com"; //Enter server adress
const uint16_t websockets_server_port = 80; // Enter server port
//api
const char* serverName = "http://nanosat.herokuapp.com/dashboard/postReadings";

using namespace websockets;

// uniquely identify which address this radio will use to transmit
bool radioNumber = 1;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit

// Used to control whether this node is sending or receiving
bool role = false;  // true = TX role, false = RX role

// For this example, we'll be using a payload containing
// a string & an integer number that will be incremented
// on every successful transmission.
// Make a data structure to store the entire payload of different datatypes
struct PayloadStruct {
  char message[32];  // only using 6 characters for TX & ACK payloads
  uint8_t counter;
};
PayloadStruct payload;

WebsocketsClient client;

#include <string>
using namespace std;

int counter = 0;
String stringToBePrinted = "";

String convertToString(char* a)
{
  String s = a;
  return s;
}

void setup() {
  Serial.begin(115200);
  pinMode(red,   OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue,  OUTPUT);
    pinMode(led,  OUTPUT);
  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    //    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  // print example's introductory prompt
  //  Serial.println(F("Receiver AcknowledgementPayloads"));


  //  char input = Serial.parseInt();
  radioNumber = 1;
  //  Serial.print(F("radioNumber = "));
  //  Serial.println((int)radioNumber);

  // role variable is hardcoded to RX behavior, inform the user of this
  //  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // to use ACK payloads, we need to enable dynamic payload lengths (for all nodes)
  radio.enableDynamicPayloads();  // ACK payloads are dynamically sized

  // Acknowledgement packets have no payloads by default. We need to enable
  // this feature for all nodes (TX & RX) to use ACK payloads.
  radio.enableAckPayload();

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1

  // additional setup specific to the node's role
  if (role) {
    // setup the TX payload

    memcpy(payload.message, "Hello ", 6);  // set the payload message
    radio.stopListening();                 // put radio in TX mode
  } else {
    // setup the ACK payload & load the first response into the FIFO

    memcpy(payload.message, "World ", 6);  // set the payload message

    // load the payload for the first received transmission on pipe 0
    radio.writeAckPayload(1, &payload, sizeof(payload));

    radio.startListening();  // put radio in RX mode


  }
  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    //    Serial.print(".");
    digitalWrite(red, HIGH); // turn the LED on

    delay(300); // wait for a second
    digitalWrite(red, LOW); // turn the LED off by making the voltage LOW
    delay(300); // wait for a second
     digitalWrite(led, HIGH); // turn the LED on
  
    delay(300); // wait for a second
    digitalWrite(led, LOW); // turn the LED off by making the voltage LOW
    delay(300); // wait for a second

  }

  // Check if connected to wifi
  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(red, HIGH); // turn the LED on
    
    delay(300); // wait for a second
    digitalWrite(red, LOW); // turn the LED off by making the voltage LOW
    delay(300); //

     digitalWrite(led, HIGH); // turn the LED on
   
    delay(300); // wait for a second
    digitalWrite(led, LOW); // turn the LED off by making the voltage LOW
    delay(300); // wait for a second


    //    Serial.print ( "No wifi... Trying again" );
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  //
  //  Serial.println("Connected to Wifi, Connecting to server.");
  // try to connect to Websockets server
  bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  if (connected) {
    //    Serial.println("Connecetd!");
    client.send("Hello Server");
  } else {
    //    Serial.println("Not Connected!");
  }

  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message) {
    //    Serial.print("Got Message: ");
    //    Serial.println(message.data());
  });
}

void loop() {

   if ( WiFi.status() != WL_CONNECTED ) {
       digitalWrite(led, HIGH); // turn the LED on
  
     //    Serial.print ( "No wifi... Trying again" );
  }
  else {
     digitalWrite(led, LOW); 
  }


  if (role) {
    // This device is a TX node

    unsigned long start_timer = micros();                  // start the timer
    bool report = radio.write(&payload, sizeof(payload));  // transmit & save the report
    unsigned long end_timer = micros();                    // end the timer
    digitalWrite(blue, HIGH); // turn the LED on
    
    delay(500); // wait for a second
    digitalWrite(blue, LOW); // turn the LED off by making the voltage LOW
    if (report) {
      //      Serial.print(F("Transmission successful! "));  // payload was delivered
      //      Serial.print(F("Time to transmit = "));
      //      Serial.print(end_timer - start_timer);  // print the timer result
      //      Serial.print(F(" us. Sent: "));
      //      Serial.print(payload.message);  // print the outgoing message
      //      Serial.print(payload.counter);  // print the outgoing counter
      uint8_t pipe;
      if (radio.available(&pipe)) {  // is there an ACK payload? grab the pipe number that received it
        PayloadStruct received;
        radio.read(&received, sizeof(received));  // get incoming ACK payload
        //        Serial.print(F(" Recieved "));
        //        Serial.print(radio.getDynamicPayloadSize());  // print incoming payload size
        //        Serial.print(F(" bytes on pipe "));
        //        Serial.print(pipe);  // print pipe number that received the ACK
        //        Serial.print(F(": "));
        //        Serial.print(received.message);    // print incoming message
        //        Serial.println(received.counter);  // print incoming counter

        // save incoming counter & increment for next outgoing
        payload.counter = received.counter + 1;

      } else {
        //        Serial.println(F(" Recieved: an empty ACK packet"));  // empty ACK packet received
      }


    } else {
      //      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }

    // to make this example readable in the serial monitor
    delay(1000);  // slow transmissions down by 1 second

  } else {
    // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe)) {                     // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getDynamicPayloadSize();  // get the size of the payload
      PayloadStruct received;
      radio.read(&received, sizeof(received));  // get incoming payload
      digitalWrite(green, HIGH); // turn the LED on
     
      delay(500); // wait for a second
      digitalWrite(green, LOW); // turn the LED off by making the voltage LOW
      delay(50);
      // let the websockets client check for incoming messages
      if (client.available()) {
        client.poll();
        client.send(received.message);

      }


      //      check received data and save
      String receivedString = convertToString(received.message);

      char abr = receivedString[0];

      String timeToBePrinted = "";

      if (abr == 'A' && counter == 0) {
        stringToBePrinted = stringToBePrinted + receivedString;
        counter ++;
      }
      else if (abr == 'T' && counter == 1) {
        stringToBePrinted = stringToBePrinted + "," + receivedString;
        counter ++;
      }
      else if (abr == 'G' && counter == 2) {
        stringToBePrinted = stringToBePrinted + "," + receivedString;
        counter ++;
      }

      if (counter == 3) {
        timeToBePrinted = printLocalTime();
        stringToBePrinted = stringToBePrinted + "," + timeToBePrinted;
        Serial.println(stringToBePrinted);
        counter = 0;
        stringToBePrinted = "";
      }


      //      Serial.print(F("Received "));
      //      Serial.print(bytes);  // print the size of the payload
      //      Serial.print(F(" bytes on pipe "));
      //      Serial.print(pipe);  // print the pipe number
      //      Serial.print(F(": "));
      //      Serial.print(received.message);  // print incoming message
      //      Serial.print(received.counter);  // print incoming counter
      //      Serial.print(F(" Sent: "));
      //      Serial.print(payload.message);    // print outgoing message
      //      Serial.println(payload.counter);  // print outgoing counter

      String st = convertToString(received.message);
      // save incoming counter & increment for next outgoing
      payload.counter = received.counter + 1;
      // load the payload for the first received transmission on pipe 0
      radio.writeAckPayload(1, &payload, sizeof(payload));
      if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        // Your Domain name with URL path or IP address with path
        http.begin(client, serverName);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Accept", "application/json");

        int httpResponseCode = http.POST("{\"data\":\"" + st + "\"}");

        //        Serial.print("HTTP Response code: ");
        //        Serial.println(httpResponseCode);

        // Free resources
        http.end();
      }


    }
  }  // role


}

String printLocalTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  //  Serial.println(asctime(timeinfo));
  //  delay(1000);
  String timeString = convertToString(asctime(timeinfo));
  return timeString;
}
