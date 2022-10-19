//////////////////////////////////////////////////////////////////////
#include <WebSocketsClient.h>
#include <Arduino.h>

#include <ESP8266WiFi.h>


const char* ssid = "Redmi Tim";
const char* password = "host2000";

const char* server = "nanosat.herokuapp.com";
const int port = 3000;

char path[] = "/";
char host[] = "nanosat.herokuapp.com";

WebSocketsClient webSocket;

unsigned long messageInterval = 5000;
bool connected = false;
 
#define DEBUG_SERIAL Serial
 
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  String text = (char*) payload;
    switch(type) {
        case WStype_DISCONNECTED:
            DEBUG_SERIAL.printf("[WSc] Disconnected!\n");
            connected = false;
           
            break;
        case WStype_CONNECTED: {
            DEBUG_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
            connected = true;
            
            DEBUG_SERIAL.println("[WSc] SENT: Connected");
          
        }
            break;
        case WStype_TEXT: {
            DEBUG_SERIAL.printf("[WSc] RESPONSE: %s\n", payload);
//             Serial.println(&payload);
        }
            break;
         case WStype_BIN:
            DEBUG_SERIAL.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);
            break;
                case WStype_PING:
                        // pong will be send automatically
                        DEBUG_SERIAL.printf("[WSc] get ping\n");
                        break;
                case WStype_PONG:
                        // answer to a ping we send
                        DEBUG_SERIAL.printf("[WSc] get pong\n");
                        break;
    }
 
}


void setup() {
    DEBUG_SERIAL.begin(115200);
   
 
    for(uint8_t t = 4; t > 0; t--) {
        DEBUG_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        DEBUG_SERIAL.flush();
        delay(1000);
    }
 
    WiFi.begin(ssid, password);
 
    while ( WiFi.status() != WL_CONNECTED ) {
      delay ( 500 );
      Serial.print ( "." );
    }
    DEBUG_SERIAL.print("Local IP: "); DEBUG_SERIAL.println(WiFi.localIP());
    // server address, port and URL
    webSocket.begin("ic-project-smart-home.herokuapp.com", 80, "/");
 
    // event handler
    webSocket.onEvent(webSocketEvent);
}
 
unsigned long lastUpdate = millis();
 
 
void loop() {
    webSocket.loop();
    if (connected){
        DEBUG_SERIAL.println("[WSc] SENT: Simple js client message!!");
        delay(1000);
        webSocket.sendTXT("Simple js client message!!");
        lastUpdate = millis();
    }
}
