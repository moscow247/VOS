/*
  BLE_Central_Device.ino

  This program uses the ArduinoBLE library to set-up an Arduino Nano 33 BLE Sense
  as a central device and looks for a specified service and characteristic in a
  peripheral device. If the specified service and characteristic is found in a
  peripheral device, the last detected value of the on-board gesture sensor of
  the Nano 33 BLE Sense, the APDS9960, is written in the specified characteristic.

  The circuit:
  - Arduino Nano 33 BLE Sense.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define LED_PIN     2
#define LED_COUNT  27
#define BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* device2 = "19b10002-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* deviceC2 = "19b10003-e8f2-537e-4f6c-d104768a1214";


float x, y, z;
int plusThreshold = 30, minusThreshold = -30;
int gesture = -1;
int oldg = -1;
int len, i;
long int timee;
uint32_t CColor;

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);

  Serial.begin(9600);

  if (!BLE.begin()) {
    Serial.println("* Starting BLE module failed!");
    while (1);
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");

  BLE.setLocalName("Nano 33 BLE (Central)");
  BLE.advertise();

  Serial.println("Arduino Nano 33 BLE Sense (Central Device)");
  Serial.println(" ");
}

void loop() {
  len = 2;
  for (int i = 0; i < 2; i++) {
    connectToPeripheral(i);
  }
}


void colorWipe(uint32_t color, int wait) {
  for (int i = 1; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}




void connectToPeripheral(int i) {
  BLEDevice peripheral, peripheral2;

  Serial.println("- Discovering peripheral device...");
  if (i == 0) {
    do {
      BLE.scanForUuid(deviceServiceUuid);
      peripheral = BLE.available();
      colorWipe(strip.Color(90, 0, 0), 1);
    } while (!peripheral);
  } else if (i == 1) {
    do {
      BLE.scanForUuid(device2);
      peripheral2 = BLE.available();
      colorWipe(strip.Color(0, 90, 0), 1);
    } while (!peripheral2);
  }

  if (i == 0) {
    if (peripheral) {
      Serial.println("* Peripheral device found!");
      Serial.print("* Device MAC address: ");
      Serial.println(peripheral.address());
      Serial.print("* Device name: ");
      Serial.println(peripheral.localName());
      Serial.print("* Advertised service UUID: ");
      Serial.println(peripheral.advertisedServiceUuid());
      Serial.println(" ");
      BLE.stopScan();
      controlPeripheral(peripheral, i);
    }
  } else if (i == 1) {
    if (peripheral2) {
      Serial.println("* Peripheral device found!");
      Serial.print("* Device MAC address: ");
      Serial.println(peripheral2.address());
      Serial.print("* Device name: ");
      Serial.println(peripheral2.localName());
      Serial.print("* Advertised service UUID: ");
      Serial.println(peripheral2.advertisedServiceUuid());
      Serial.println(" ");
      BLE.stopScan();
      controlPeripheral(peripheral2, i);
    }
  }

}

void controlPeripheral(BLEDevice peripheral, int num) {
  Serial.println("- Connecting to peripheral device...");

  if (peripheral.connect()) {
    Serial.println("* Connected to peripheral device!");
    Serial.println(" ");
  } else {
    Serial.println("* Connection to peripheral device failed!");
    Serial.println(" ");
    return;
  }

  Serial.println("- Discovering peripheral device attributes...");
  if (peripheral.discoverAttributes()) {
    Serial.println("* Peripheral device attributes discovered!");
    Serial.println(" ");
  } else {
    Serial.println("* Peripheral device attributes discovery failed!");
    Serial.println(" ");
    peripheral.disconnect();
    return;
  }
  BLECharacteristic gestureCharacteristic;
  if (num == 0) {
    gestureCharacteristic = peripheral.characteristic(deviceServiceCharacteristicUuid);
  } else if (num == 1) {
    gestureCharacteristic = peripheral.characteristic(deviceC2);
  }

  if (!gestureCharacteristic) {
    Serial.println("* Peripheral device does not have gesture_type characteristic!");
    peripheral.disconnect();
    return;
  } else if (!gestureCharacteristic.canWrite()) {
    Serial.println("* Peripheral does not have a writable gesture_type characteristic!");
    peripheral.disconnect();
    return;
  }
  timee=millis();
  while (peripheral.connected() and !(millis()>=timee+500)) {
    if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(x, y, z);
    }
    if (y > plusThreshold)
    {
      gesture =  1;
      colorWipe(strip.Color(255,   0,   0), 5); // Red
      delay(50);
    }
    if (y < minusThreshold)
    {
      gesture = 2;
      colorWipe(strip.Color(0,   255,   0), 5);
      delay(50);
    }
    if (x < minusThreshold)
    {
      gesture = 3;
      colorWipe(strip.Color(0,   0,   255), 5);
      delay(50);
    }
    if (x > plusThreshold)
    {
      gesture = 4;
      colorWipe(strip.Color(255, 255, 255), 5);
      delay(50);
    }
    if (gesture != oldg) {
      Serial.print("* Writing value to gesture_type characteristic: ");
      Serial.println(gesture);
      oldg = gesture;
      gestureCharacteristic.writeValue((byte)gesture);

      Serial.println("* Writing value to gesture_type characteristic done!");
      Serial.println(" ");
    }

  }
  Serial.println("- Peripheral device disconnected!");
  if (!peripheral.connected()) {
    colorWipe(strip.Color(255, 0, 255), 10);
    gesture = 10;
  }
}
