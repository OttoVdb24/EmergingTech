#include <ArduinoBLE.h>
BLEService sensorService("12345678-1234-1234-1234-123456789012");
BLEFloatCharacteristic sensorChar(  "12345678-1234-1234-1234-123456789013",  BLERead | BLENotify);

void BLE_setup(){
    if (!BLE.begin()) {
    Serial.println("BLE starten mislukt!");
    while (1);
  }

  BLE.setLocalName("Arduino-Sensor");
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(sensorChar);
  BLE.addService(sensorService);

  sensorChar.writeValue(0.0f);
  BLE.advertise();
  Serial.println("BLE actief, wachten op verbinding...");

}

void setup() {
  Serial.begin(9600);
  BLE_setup();


}

void loop() {
  BLE_send(3);
}



void BLE_send( int value){
    BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Verbonden met: ");
    Serial.println(central.address());

    while (central.connected()) {

      sensorChar.writeValue(value);
      Serial.print("Verstuurd: ");
      Serial.println(value);

      delay(500);
    }

    Serial.println("Verbinding verbroken.");
  }
}