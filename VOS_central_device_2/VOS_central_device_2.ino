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
#define BRIGHTNESS 125 // Set BRIGHTNESS to about 1/5 (max = 255)

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

const char* deviceServiceUuid2 = "19b10000-e8f2-537e-4f6c-d104768a1215";
const char* deviceServiceCharacteristicUuid2 = "19b10001-e8f2-537e-4f6c-d104768a1215";

BLEService Service(deviceServiceUuid2);
BLEByteCharacteristic chara(deviceServiceCharacteristicUuid2, BLERead | BLEWrite);

float x, y, z;
int plusThreshold = 30, minusThreshold = -30;
int gesture = -1;
int oldg = -1;
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
  BLE.setAdvertisedService(Service);
  Service.addCharacteristic(chara);
  BLE.addService(Service);
  chara.writeValue(-1);
  BLE.advertise();

  Serial.println("Arduino Nano 33 BLE Sense (Central Device)");
  Serial.println(" ");
}

void loop() {
//  connectToPeripheral();
colorWipe(strip.Color(255, 0, 100), 5);
}

void connectToPeripheral() {
  BLEDevice peripheral;

  Serial.println("- Discovering peripheral device...");

  do
  {
    BLE.scanForUuid(deviceServiceUuid);
    peripheral = BLE.available();
  } while (!peripheral);

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
    controlPeripheral(peripheral);
  }
}

void controlPeripheral(BLEDevice peripheral) {
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

  BLECharacteristic gestureCharacteristic = peripheral.characteristic(deviceServiceCharacteristicUuid);

  if (!gestureCharacteristic) {
    Serial.println("* Peripheral device does not have gesture_type characteristic!");
    peripheral.disconnect();
    return;
  } else if (!gestureCharacteristic.canWrite()) {
    Serial.println("* Peripheral does not have a writable gesture_type characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
    if (gestureCharacteristic.written()) {
      colorWipe(strip.Color(100, 100, 100), 50);
    }
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
  }
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 1; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
