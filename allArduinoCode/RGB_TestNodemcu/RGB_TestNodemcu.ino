/*
   This ESP32 code is created by esp32io.com

   This ESP32 code is released in the public domain

   For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-rgb-led
*/

const int red =  16; // d0
const int green = 5; // d1
const int blue = 0; // d3

void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  pinMode(red,   OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue,  OUTPUT);
}

void loop() {
  Serial.println("work");
  delay(1000);
  digitalWrite(red, HIGH); // turn the LED on
  Serial.print("red on");
  delay(1000); // wait for a second
  digitalWrite(red, LOW); // turn the LED off by making the voltage LOW
  delay(1000); // wait for a second
  digitalWrite(green, HIGH); // turn the LED on
  Serial.print("green on");
  delay(1000); // wait for a second
  digitalWrite(green, LOW); // turn the LED off by making the voltage LOW
  delay(1000); // wait for a second
  digitalWrite(blue, HIGH); // turn the LED on
  Serial.print("blue on");
  delay(1000); // wait for a second
  digitalWrite(blue, LOW); // turn the LED off by making the voltage LOW
  delay(1000); // wait for a second
}
