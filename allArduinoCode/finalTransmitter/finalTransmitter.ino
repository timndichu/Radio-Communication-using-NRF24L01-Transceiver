
#include <MPU9255.h>//include MPU9255 library

// Sending Text (Transmitter)
// Library: TMRh20/RF24 (https://github.com/tmrh20/RF24/)

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

//OBC RX,TX PINS
#define RXp2 16
#define TXp2 17

#define RXp3 9
#define TXp3 10

//RGB PINS
const int red =  25; // GIOP36
const int green = 26; // GIOP39
const int blue = 27; // GIOP34

// MY TRANSMITTER IS THE ESP32
//CONFIGURE THE CE,CSN PARAMETERS DEPENDING ON YOUR BOARD'S PINS
RF24 radio(4, 5); // CE, CSN
const byte address[10] = "ADDRESS01";


int delimiter, delimiter_1;
String oc, ot, ol, ob, os;


MPU9255 mpu;

bool radioNumber = 0;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit

// Used to control whether this node is sending or receiving
bool role = true;  // true = TX role, false = RX role

// For this example, we'll be using a payload containing
// a string & an integer number that will be incremented
// on every successful transmission.
// Make a data structure to store the entire payload of different datatypes
struct PayloadStruct {
  char message[32];  // only using 6 characters for TX & ACK payloads
  uint8_t counter;
};
PayloadStruct payload;


int counter = 0;
int counter2 = 0;



// Task to save OBC serial data
void saveSerialData(void *parameter) {
  while (1) {
    if (Serial.available() > 0) {

      String stringReceived = Serial.readString();
    // String stringReceived = "OC,-0.100,-0.300,\nOT,12.1,13.7,12.3,\nOL,0.90,0.90,\nOB,A\nOS,C";
//          Serial.println(stringReceived);
     
      int delimiter, delimiter_1, delimiter_2, delimiter_3;
      delimiter = stringReceived.indexOf("\n");
      delimiter_1 = stringReceived.indexOf("\n", delimiter + 1);
      delimiter_2 = stringReceived.indexOf("\n", delimiter_1 + 1);
      delimiter_3 = stringReceived.indexOf("\n", delimiter_2 + 1);
      oc = stringReceived.substring(0, delimiter);
      ot = stringReceived.substring(delimiter + 1, delimiter_1);
      ol = stringReceived.substring(delimiter_1 + 1, delimiter_2);
      ob = stringReceived.substring(delimiter_2 + 1, delimiter_3);
      os = stringReceived.substring(delimiter_3 + 1, stringReceived.length() + 1);

 
     

    
      for (int i = 0; i < 5; i++) {
        if (i == 0) {
          sprintData(oc);
          //        continue;
        }
        else if (i == 1) {
          sprintData(ot);
          //        continue;
        }
        else if (i == 2) {
          sprintData(ol);
          //        continue;
        }
        else if (i == 3) {
          sprintData(ob);
          //        continue;
        }
        else if (i == 4) {
          sprintData(os);
          //        continue;
        }
      }
      
      

     
    }
    delay(1000);
  }
}

// Task to save EPS serial data
void saveEPSData(void *parameter) {
  while (1) {
    if (Serial2.available() > 0) {
      String stringReceived = Serial2.readString();
      Serial.println(stringReceived);

      //      int delimiter, delimiter_1;
      //      delimiter = stringReceived.indexOf("\n");
      //      delimiter_1 = stringReceived.indexOf("\n", delimiter + 1);
      //      oc = stringReceived.substring(0, delimiter);
      //      ot = stringReceived.substring(delimiter + 1, delimiter_1);
      //      ol = stringReceived.substring(delimiter_1 + 1, stringReceived.length() + 1);
      //
      //      for (int i = 0; i < 3; i++) {
      //        if (i == 0) {
      //          sprintData(oc);
      //        }
      //        else if (i == 1) {
      //          sprintData(ot);
      //        }
      //        else if (i == 2) {
      //          sprintData(ol);
      //        }
      //      }
      delay(2000);
    }
  }
}



void sendPayload() {

  if (role) {
    // This device is a TX node

    unsigned long start_timer = micros();                  // start the timer
    bool report = radio.write(&payload, sizeof(payload));  // transmit & save the report
    unsigned long end_timer = micros();                    // end the timer

    if (report) {
      Serial.print(F("Transmission successful! "));  // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer);  // print the timer result
      Serial.print(F(" us. Sent: "));
      Serial.print(payload.message);  // print the outgoing message
      Serial.print(payload.counter);  // print the outgoing counter
      digitalWrite(green, HIGH); // turn the LED on
      
      delay(500); // wait for a second
      digitalWrite(green, LOW); // turn the LED off by making the voltage LOW
      delay(50);
      uint8_t pipe;
      if (radio.available(&pipe)) {  // is there an ACK payload? grab the pipe number that received it
        PayloadStruct received;
        radio.read(&received, sizeof(received));  // get incoming ACK payload
        Serial.print(F(" Recieved "));
        Serial.print(radio.getDynamicPayloadSize());  // print incoming payload size
        Serial.print(F(" bytes on pipe "));
        Serial.print(pipe);  // print pipe number that received the ACK
        Serial.print(F(": "));
        Serial.print(received.message);    // print incoming message
        Serial.println(received.counter);  // print incoming counter

        // save incoming counter & increment for next outgoing
        payload.counter = received.counter + 1;

        digitalWrite(blue, HIGH); // turn the LED on
       ;
        delay(500); // wait for a second 
        digitalWrite(blue, LOW); // turn the LED off by making the voltage LOW
        delay(50); // wait for a second

      } else {
        Serial.println(F(" Recieved: an empty ACK packet"));  // empty ACK packet received
      }


    } else {
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
      digitalWrite(red, HIGH); // turn the LED on
     
      delay(500); // wait for a second
      digitalWrite(red, LOW); // turn the LED off by making the voltage LOW
      delay(50); // wait for a second

    }

    delay(500);
  }
}


void sprintData(String str) {
  char txt2[32];
  sprintf(txt2, "%s", str.c_str());
  memcpy(payload.message, txt2, sizeof(txt2));
  Serial.print("OBC Successfully transmitted: ");
  Serial.println(txt2);
  sendPayload();
}

void print_data()//take some raw readings and print them in serial monitor
{

  for (int i = 0; i <= 500; i++)
  {
    mpu.read_acc();
    mpu.read_gyro();

    double dT;
    int ax = mpu.ax;
    int ay = mpu.ay;
    int az = mpu.az;

    int gx = mpu.gx;
    int gy = mpu.gy;
    int gz = mpu.gz;
    dT = ( (double) mpu.read_temp()) / 333.87 + 21.0;

    //    send mpu reading to groundstation
    char txt[32];
    if (counter == 0) {
      sprintf(txt, "A,%d,%d,%d", ax, ay, az);
      counter++;
    }
    else if (counter == 1) {
      sprintf(txt, "G,%d,%d,%d", gx, gy, gz);
      counter++;
    }
    else if (counter == 2) {
      sprintf(txt, "T,%0.2f", dT);
      counter = 0;
    }

    memcpy(payload.message, txt, sizeof(txt));
    Serial.print("MPU Successfully transmitted: ");
    Serial.println(txt);
    sendPayload();
    delay(500);

  }
}


void setup() {
  Serial.begin(9600);//initialize Serial port
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);


  pinMode(red,   OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue,  OUTPUT);
  Wire.begin();

  if (mpu.init())
  {
    Serial.println("initialization failed");
  }
  else
  {
    Serial.println("initialization successful!");
  }
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }




  radioNumber = 0;
 

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


  xTaskCreatePinnedToCore(
    saveSerialData,    // Function to be called
    "SaveSerialData", // Name of the task
    6000,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task Priority, the higher the number, the higher the priority (0 to 25)
    NULL,         // Task Handle
    app_cpu       // CPU Core the task should run in
  );


  //  xTaskCreatePinnedToCore(
  //    saveEPSData,    // Function to be called
  //    "saveEPSData", // Name of the task
  //    6000,         // Stack size (bytes in ESP32, words in FreeRTOS)
  //    NULL,         // Parameter to pass to function
  //    1,            // Task Priority, the higher the number, the higher the priority (0 to 25)
  //    NULL,         // Task Handle
  //    app_cpu       // CPU Core the task should run in
  //  );

}






void loop() {

    print_data();//print some data

}
