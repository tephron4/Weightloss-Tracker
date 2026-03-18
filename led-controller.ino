#include <ArduinoBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Custom UUID
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  pinMode(LED_BUILTIN, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("NanoESP32_Control");
  BLE.setAdvertisedService(ledService);
  ledService.addCharacteristic(switchCharacteristic);
  BLE.addService(ledService);
  
  switchCharacteristic.writeValue(0);
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    
    while (central.connected()) {
      if (switchCharacteristic.written()) {
        byte val = switchCharacteristic.value();
        Serial.print("Data received: ");
        Serial.println(val); // This will tell us if it sees a 1 or 0
        
        if (switchCharacteristic.value()) {   
          digitalWrite(LED_BUILTIN, HIGH); 
        } else {
          digitalWrite(LED_BUILTIN, LOW); 
        }
      }
    }
    Serial.println("Disconnected from central");
  }
}