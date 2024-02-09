/*
    Based on Neil Kolban example for IDF:
https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
   Modificado por Juan Antonio Villalpando.
   http://kio4.com/arduino/160i_Wemos_ESP32_BLE.htm
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string.h>
#include <stdio.h>

String from_tel;
String transfer = "";
bool actif = true;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      pCharacteristic->setValue(transfer.c_str());
      transfer = "";
      
      if (value.length() > 0) {
        from_tel = "";
        for (int i = 0; i < value.length(); i++){
          // Serial.print(value[i]); // Presenta value.
          from_tel = from_tel + value[i];
        } 
        if (from_tel == "true") {
          actif = true;
        }
      }
    }
};

// branché sur le pin A0
int PIN = A0;

void setup() {
  // bt
  Serial.begin(115200);
  BLEDevice::init("buzzer");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  // boutons
  pinMode(PIN, INPUT);
}

void loop() {
  int value;

  value = analogRead(PIN);
  //Serial.print("intensité sur le pin = ");
  //Serial.println(value);

  if (value < 3000 & actif == true) {
    if ( value < 100 ) {
      transfer = "1";
    } else if ( value < 600 ) {
      transfer = "2";
    } else if ( value < 1500 ) {
      transfer = "3";
    } else if (value < 3000) {
      transfer = "4";
    };
    Serial.print("Numero du bouton appuyé : ");
    Serial.println(transfer);
    actif = false;
  }
  //delay(40);  

}
