#include <ArduinoBLE.h>
#include <Preferences.h> // Library for permanent storage

Preferences prefs;

BLEService weightService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Custom UUID

// Define Float Characteristics (Read and Write)
BLEFloatCharacteristic startWeightChar("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic goalWeightChar("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic currentWeightChar("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic resetChar("19B10005-E8F2-537E-4F6C-D104768A1214", BLEWrite);

void setup() {
  Serial.begin(9600);
  
  prefs.begin("weights", false); // Open "weights" namespace for read/write

  float savedStartWeight = prefs.getFloat("start", 0.0);
  float savedGoalWeight = prefs.getFloat("goal", 0.0);
  float savedCurrentWeight = prefs.getFloat("current", 0.0);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("NanoESP32_WeightTracker");
  BLE.setAdvertisedService(weightService);

  weightService.addCharacteristic(startWeightChar);
  weightService.addCharacteristic(goalWeightChar);
  weightService.addCharacteristic(currentWeightChar);
  weightService.addCharacteristic(resetChar);

  startWeightChar.writeValue(savedStartWeight);
  goalWeightChar.writeValue(savedGoalWeight);
  currentWeightChar.writeValue(savedCurrentWeight);

  BLE.addService(weightService);
  BLE.advertise();
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    while (central.connected()) {
      if (startWeightChar.written()) {
        float startWeight = startWeightChar.value();
        Serial.print("Start Weight: ");
        Serial.println(startWeight);
        prefs.putFloat("start", startWeight); // Save to preferences
      }
      if (goalWeightChar.written()) {
        float goalWeight = goalWeightChar.value();
        Serial.print("Goal Weight: ");
        Serial.println(goalWeight);
        prefs.putFloat("goal", goalWeight); // Save to preferences
      }
      if (currentWeightChar.written()) {
        float currentWeight = currentWeightChar.value();
        Serial.print("Current Weight: ");
        Serial.println(currentWeight);
        prefs.putFloat("current", currentWeight); // Save to preferences
      }
      if (resetChar.written()) {
        Serial.println("Resetting weights...");
        prefs.clear(); // Clear all saved weights

        startWeightChar.writeValue(0.0);
        goalWeightChar.writeValue(0.0);
        currentWeightChar.writeValue(0.0);
      }
    }
  }
}