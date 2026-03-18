#include <ArduinoBLE.h>

BLEService weightService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Custom UUID

// Define Float Characteristics (Read and Write)
BLEFloatCharacteristic startWeightChar("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic goalWeightChar("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic currentWeightChar("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  pinMode(LED_BUILTIN, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("NanoESP32_WeightTracker");
  BLE.setAdvertisedService(weightService);

  weightService.addCharacteristic(startWeightChar);
  weightService.addCharacteristic(goalWeightChar);
  weightService.addCharacteristic(currentWeightChar);

  BLE.addService(weightService);
  BLE.advertise();
  Serial.println("Waiting for weight data...");
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    while (central.connected()) {
      if (startWeightChar.written()) {
        float startWeight = startWeightChar.value();
        Serial.print("Start Weight: ");
        Serial.println(startWeight);
      }
      if (goalWeightChar.written()) {
        float goalWeight = goalWeightChar.value();
        Serial.print("Goal Weight: ");
        Serial.println(goalWeight);
      }
      if (currentWeightChar.written()) {
        float currentWeight = currentWeightChar.value();
        Serial.print("Current Weight: ");
        Serial.println(currentWeight);
      }
    }
  }
}