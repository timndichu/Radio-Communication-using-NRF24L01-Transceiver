/*
  Bandwidth example
  This example shows how different low-pass filter settings are affecting the readings from the
  accelerometer and the gyroscope.
*/

#include <MPU9255.h>//include MPU9255 library

// Sending Text (Transmitter)
// Library: TMRh20/RF24 (https://github.com/tmrh20/RF24/)

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

// MY TRANSMITTER IS THE ESP32
//CONFIGURE THE CE,CSN PARAMETERS DEPENDING ON YOUR BOARD'S PINS
RF24 radio(4, 5); // CE, CSN
const byte address[10] = "ADDRESS01";

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
    /*
    //accelerometer
    Serial.println("Accelerometer: ");
    Serial.print("  AX: ");
    Serial.print(ax);
    Serial.print(" AY: ");
    Serial.print(ay);
    Serial.print(" AZ: ");
    Serial.print(az);
    Serial.println("");
    Serial.println("");

    //gyroscope
    Serial.println("Gyroscope: ");
    Serial.print("  GX: ");
    Serial.print(gx);
    Serial.print(" GY: ");
    Serial.print(gy);
    Serial.print(" GZ: ");
    Serial.print(gz);
    Serial.println("");
    Serial.println("");

    //temperature
    Serial.print("Temp: ");
    Serial.print(dT, 2);
    Serial.print(F(" degrees Celsius"));
    Serial.println(F(""));
    Serial.println("");
    Serial.println("");

    */
    //    send mpu reading to groundstation
    char txt[32];
    if(counter==0){
       sprintf(txt, "A,AX:%d,AY:%d,AZ:%d", ax,ay,az);
       counter++;
    }
    else if(counter==1){
       sprintf(txt, "G,GX:%d,GY:%d,GZ:%d", gx,gy,gz);
       counter++;
    }
    else if(counter==2){
       sprintf(txt, "T,Temp:%0.2f", dT);
       counter=0;
    }
   
    memcpy(payload.message, txt, sizeof(txt));
     
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

      } else {
        Serial.println(F(" Recieved: an empty ACK packet"));  // empty ACK packet received
      }


    } else {
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }

    delay(2000);
  }
}
}


void setup() {
  Serial.begin(9600);//initialize Serial port
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

  // print example's introductory prompt
  Serial.println(F("Transmitter AcknowledgementPayloads"));


  radioNumber = 0;
  Serial.print(F("radioNumber = "));
  Serial.println((int)radioNumber);

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


}

void loop() {

  print_data();//print some data
  Serial.println("");

  delay(2000);

}

