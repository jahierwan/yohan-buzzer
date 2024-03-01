/*
Fait par Nino Jahier
Inspiré de http://kio4.com/arduino/160i_Wemos_ESP32_BLE.htm (Juan Antonio Villalpando)
lui-même basé sur
 Neil Kolban example for IDF:
  https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string.h>
#include <stdio.h>

String qu_suivante;
String zone_partagee = "";
bool attente_buzz = true;
bool debug = true;
bool lockJ1 = false;
bool lockJ2 = false;
bool lockJ3 = false;
bool lockJ4 = false;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


// effet de bord : recupere la valuer de qu_suivante via le BT
void recupere_valeur (BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        qu_suivante = "";
        for (int i = 0; i < value.length(); i++){
          // Serial.print(value[i]); // Presenta value.
          qu_suivante = qu_suivante + value[i];
        };
	if (debug) {
	  Serial.print("recupere_valeur : qu_suivante = ");
	  Serial.print(qu_suivante);
	  Serial.print("  zone_partagee = ");
	  Serial.print(zone_partagee);
	  Serial.println(" Fin recupere_valeur");
	};
      }
}

void transferer (BLECharacteristic *pCharacteristic) {
  Serial.print("J'envoie la valeur du bouton appuyé  : ");
  Serial.println(zone_partagee);
  pCharacteristic->setValue(zone_partagee.c_str());
  zone_partagee = "";
}

// appellé quand le bouton "question suivante" est pressé + toutes les secondes (via timer2 dans MIT)
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      Serial.print("MyCallbacks: ");

      if (zone_partagee == "1" || zone_partagee == "2" || zone_partagee == "3" || zone_partagee == "4" ) {
        transferer(pCharacteristic);
        //delay(1000); //????
      };
      recupere_valeur(pCharacteristic);
      if (qu_suivante == "t") {
        Serial.println("qu_suivante = t => je met le mode 'attente du buzz' a true");
        attente_buzz = true;
        lockJ1 = false;
        lockJ2 = false;
        lockJ3 = false;
        lockJ4 = false;
      } else if (qu_suivante == "f"){ //si la reponse donné n'est pas bonne on peut à nouveau buzzer (mais le joueur sera lock)
        attente_buzz = true;
	      Serial.print("qu_suivante n'est pas true. Il vaut : "); Serial.print(qu_suivante);
      };
      Serial.println("Fin de MyCallbacks ");
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

bool off = true;
void loop() {
  int value;

  value = analogRead(PIN);
  Serial.print("intensité sur le pin = ");
  Serial.println(value);

  if (value < 3000 & attente_buzz == true) {

    if ( value < 50  & lockJ1 == false) {
      zone_partagee = "1";
      lockJ1 = true;
      attente_buzz = false;
    } else if ( value < 1000 & value > 300 & lockJ2 == false) {
      zone_partagee = "2";
      lockJ2 = true;
      attente_buzz = false;
    } else if ( value < 1700 & value > 1100 & lockJ3 == false ) {
      zone_partagee = "3";
      lockJ3 = true;
      attente_buzz = false;
    } else if (value < 3000 & value > 1800 & lockJ4 == false) {
      zone_partagee = "4";
      lockJ4 = true;
      attente_buzz = false;
    };
    Serial.print("Numero du bouton appuyé : ");
    Serial.println(zone_partagee);
    //attente_buzz = false;
  };
}
