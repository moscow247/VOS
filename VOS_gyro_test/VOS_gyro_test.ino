/*
  Arduino LSM9DS1 - Gyroscope Application

  This example reads the gyroscope values from the LSM9DS1 sensor
  and prints them to the Serial Monitor or Serial Plotter, as a directional detection of
  an axis' angular velocity.

  The circuit:
  - Arduino Nano 33 BLE

  Created by Riccardo Rizzo

  Modified by Benjamin Dannegård
  30 Nov 2020

  This example code is in the public domain.
*/

#include <Arduino_LSM9DS1.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define LED_PIN     2
#define LED_COUNT  27
#define BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

float x, y, z, s;
int flag = 0;
long int t = 0;
int plusThreshold = 30, minusThreshold = -30;


void setup() {
  Serial.begin(9600);
  //  while (!Serial);
  //  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  //  pinMode(3, OUTPUT);
  Serial.print("accelerationSampleRate sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("accelerationSampleRate in degrees/second");
}


void loop() {

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.print(flag);
    Serial.print('\t');
    Serial.println(z);
  }

  if (x<0.5 and x> -1.1 and y<1 and y> -1.1 and  z > -0.5 and z < 1.3) {
    //    colorWipe(strip.Color(100, 0, 100), 5);
    if (flag != 1) {
      t = millis();
    }
    flag = 1;
    //    s = 1;
    delay(100);
  } else {
    flag = 0;
    strip.setBrightness(50);
    colorWipe(strip.Color(255, 0, 255), 10);
  }
  if (flag == 1 and t + 10000 <= millis()) {
    if (s != 1) {
      colorWipe(strip.Color(0, 0, 0), 50);
      s = 1;
    }
    strip.setBrightness(0);
  } else {
    s = 0;
  }
//апрапрапр
}
void colorWipe(uint32_t color, int wait) {
  for (int i = 1; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
