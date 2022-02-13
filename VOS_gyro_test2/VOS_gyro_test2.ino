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


float x, y, z, sum = 0;
long int timef = 0, timeb = 0, timez = 0;
int plusThreshold = 30, minusThreshold = -30;
boolean flag = false;


void setup() {
  Serial.begin(9600);
  //  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");
}


void loop() {

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);
  }
  if (y > plusThreshold)
  {
    Serial.println("Collision front");
    //    colorWipe(strip.Color(0, 0, 100), 1);

    timef = millis();
  }
  if (y < minusThreshold)
  {
    Serial.println("Collision back");
    //    colorWipe(strip.Color(100, 0, 0), 1);

    timeb = millis();
  }
  if (abs(timef - timeb) < 600) {
    if (timez == 0) {
      timez = millis();
    }
  } else {
    timez = 0;
  }

  if (timez + 1500 > millis()) {
    if (flag) {
      colorWipe(strip.Color(100, 0, 0), 10);
    } else if (!flag) {
      colorWipe(strip.Color(0, 0, 100), 10);
    }
  } else {
    colorWipe(strip.Color(0, 0, 0), 10);
    flag = !flag;
  }
  Serial.print(timez);
  Serial.print('\t');
  Serial.println(abs(timef - timeb));
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 1; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
