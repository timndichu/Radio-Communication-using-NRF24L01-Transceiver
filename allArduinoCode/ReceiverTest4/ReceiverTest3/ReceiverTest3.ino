
// Receiving Text (Receiver)
// Library: TMRh20/RF24 (https://github.com/tmrh20/RF24/)

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>


// MY RECEIVER IS THE NODEMCU 1.0(ESP 12E)
//CONFIGURE THE CE,CSN PARAMETERS DEPENDING ON YOUR BOARD'S PINS
RF24 radio(2, 4); // CE, CSN
const byte address[10] = "ADDRESS01";
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>

const char* ssid = "Redmi Tim"; //Enter SSID
const char* password = "host2000"; //Enter Password
const char* websockets_server_host = "nanosat.herokuapp.com"; //Enter server adress
const uint16_t websockets_server_port = 80; // Enter server port

using namespace websockets;

WebsocketsClient client;
void setup() {
    Serial.begin(115200);
    radio.begin();
    radio.setAutoAck(false);
    radio.printDetails();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
    // Connect to wifi
    WiFi.begin(ssid, password);

    // Wait some time to connect to wifi
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    // Check if connected to wifi
    while ( WiFi.status() != WL_CONNECTED ) {
      
      delay ( 500 );
      Serial.print ( "No wifi... Trying again" );
    }

    Serial.println("Connected to Wifi, Connecting to server.");
    // try to connect to Websockets server
    bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
    if(connected) {
        Serial.println("Connecetd!");
        client.send("Hello Server");
    } else {
        Serial.println("Not Connected!");
    }
    
    // run callback when messages are received
    client.onMessage([&](WebsocketsMessage message) {
        Serial.print("Got Message: ");
        Serial.println(message.data());
    });
}

void loop() {
    // let the websockets client check for incoming messages
    if(client.available()) {
        client.poll();
        if (radio.available()) {
          char txt[240] = "";
          radio.read(&txt, sizeof(txt));
          Serial.println(txt);
          client.send(txt);
         }
    }
    delay(500);
}
