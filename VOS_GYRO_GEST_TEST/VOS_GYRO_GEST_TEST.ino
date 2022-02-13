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
float xx, yy, zz, s;
int flag1 = 0;
long int t = 0;
long int timef = 0, timeb = 0, timez = 0;
int plusThreshold = 30, minusThreshold = -30;
boolean flag = false;
uint32_t color;

void setup() {
  Serial.begin(9600);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}


void loop() {

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);
  }
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(xx, yy, zz);
  }


  if (y > plusThreshold)
  {
    Serial.println("Collision front");
    timef = millis();
  }
  if (y < minusThreshold)
  {
    Serial.println("Collision back");
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
      color=strip.Color(0, 0, 100);
    } else if (!flag) {
      color=strip.Color(100, 0, 0);
    }
  } else {

    flag = !flag;
  }

  if (xx<0.5 and xx> -1.1 and yy<1 and yy> -1.1 and  zz > -0.5 and zz < 1.3) {
    if (flag1 != 1) {
      t = millis();
    }
    flag1 = 1;
    delay(100);
  } else {
    flag1 = 0;
    strip.setBrightness(50);
    colorWipe(color, 10);
  }
  if (flag1 == 1 and t + 10000 <= millis()) {
    if (s != 1) {
      colorWipe(strip.Color(0, 0, 0), 50);
      s = 1;
    }
    strip.setBrightness(0);
  } else {
    s = 0;
  }

}

void colorWipe(uint32_t color, int wait) {
  for (int i = 1; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
