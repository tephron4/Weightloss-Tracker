#include <ArduinoBLE.h>
#include <Preferences.h> // Library for permanent storage

Preferences prefs;

BLEService weightService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Custom UUID

// Define Float Characteristics (Read and Write)
BLEFloatCharacteristic startWeightChar("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic goalWeightChar("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic currentWeightChar("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic progressPercentageChar("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead);
BLEByteCharacteristic resetChar("19B10006-E8F2-537E-4F6C-D104768A1214", BLEWrite);

int latchPin = 10;
int clockPin = 9;
int dataPin = 8;

int leds = 0;

float startWeight;
float goalWeight;
float currWeight;
float lossProgress;

void updateShiftRegister()
{
  Serial.println("Update shift register");
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
}

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  updateShiftRegister();
  Serial.begin(9600);
  while(! Serial); // wait until serial is ready

  Serial.println("Running LED Test...");
  startupAnimation();
  
  prefs.begin("weights", false); // Open "weights" namespace for read/write

  startWeight = prefs.getFloat("start", 0.0);
  goalWeight = prefs.getFloat("goal", 0.0);
  currWeight = prefs.getFloat("current", 0.0);
  calculateLossProgress();

  printStartWeight();
  printGoalWeight();
  printCurrWeight();

  updateLeds();

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("NanoESP32_WeightTracker");
  BLE.setAdvertisedService(weightService);

  weightService.addCharacteristic(startWeightChar);
  weightService.addCharacteristic(goalWeightChar);
  weightService.addCharacteristic(currentWeightChar);
  weightService.addCharacteristic(progressPercentageChar);
  weightService.addCharacteristic(resetChar);

  startWeightChar.writeValue(startWeight);
  goalWeightChar.writeValue(goalWeight);
  currentWeightChar.writeValue(currWeight);
  progressPercentageChar.writeValue(lossProgress);

  BLE.addService(weightService);
  BLE.advertise();
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    while (central.connected()) {
      if (startWeightChar.written()) {
        startWeight = startWeightChar.value();
        Serial.print("Start Weight: ");
        Serial.println(startWeight);
        prefs.putFloat("start", startWeight); // Save to preferences
        updateLeds();
      }
      if (goalWeightChar.written()) {
        goalWeight = goalWeightChar.value();
        Serial.print("Goal Weight: ");
        Serial.println(goalWeight);
        prefs.putFloat("goal", goalWeight); // Save to preferences
        updateLeds();
      }
      if (currentWeightChar.written()) {
        currWeight = currentWeightChar.value();
        Serial.print("Current Weight: ");
        Serial.println(currWeight);
        prefs.putFloat("current", currWeight); // Save to preferences
        updateLeds();
      }
      if (resetChar.written()) {
        Serial.println("Resetting weights...");
        prefs.clear(); // Clear all saved weights

        startWeight = 0.0;
        goalWeight = 0.0;
        currWeight = 0.0;
        startWeightChar.writeValue(0.0);
        goalWeightChar.writeValue(0.0);
        currentWeightChar.writeValue(0.0);
        updateLeds();
      }
    }
  }
}

void startupAnimation() {
  for (int i = 0; i < 8; i++) {
    leds = (1 << i);
    updateShiftRegister();
    delay(100);
  }
  for (int i = 6; i >= 0; i--) {
    leds = (1 << i);
    updateShiftRegister();
    delay(100);
  }
  leds = 0; // Clear them out after test
  updateShiftRegister();
  Serial.println("Hardware test complete.");
}

void updateLeds() {
  Serial.println("Updating LEDs");
  calculateLossProgress();

  // Using round() or adding 0.5 can help the LEDs feel more responsive
  int numLEDSLit = lossProgress / 12.5; 
  if (numLEDSLit > 8) numLEDSLit = 8;
  Serial.print("Number of LEDs lit: ");
  Serial.println(numLEDSLit);

  leds = 0;
  for (int i = 0; i < numLEDSLit; i++) {
    leds |= (1 << i); // Bitwise OR is cleaner for setting bits
  }
  updateShiftRegister();
}

void calculateLossProgress() {
  float weightLost = startWeight - currWeight;
  lossProgress = (weightLost / (startWeight - goalWeight)) * 100;

  // Keep progress within 0-100 range
  if (isnan(lossProgress) || lossProgress < 0) lossProgress = 0;
  if (lossProgress > 100) lossProgress = 100;
  Serial.print("Weight loss progress: ");
  Serial.print(lossProgress);
  Serial.println("%");

  progressPercentageChar.writeValue(lossProgress);
}

void printStartWeight() {
  Serial.print("Your starting weight is: ");
  Serial.print(startWeight);
  Serial.println(" lbs");
}
void printGoalWeight() {
  Serial.print("Your goal weight is: ");
  Serial.print(goalWeight);
  Serial.println(" lbs");
}
void printCurrWeight() {
  Serial.print("Your current weight is: ");
  Serial.print(currWeight);
  Serial.println(" lbs");
}