/*
Fait par Nino Jahier
Inspiré de http://kio4.com/arduino/160i_Wemos_ESP32_BLE.htm (Juan Antonio Villalpando)
lui-même basé sur
 Neil Kolban example for IDF:
  https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
   Ported to Arduino ESP32 by Evandro Copercini

https://github.com/thomasfredericks/Bounce2
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string.h>
#include <stdio.h>
#include <Bounce2.h>

String qu_suivante;
String zone_partagee = "";
bool attente_buzz = true;
bool debug = true;
bool lockJ1 = false;
bool lockJ2 = false;
bool lockJ3 = false;
bool lockJ4 = false;

Bounce2::Button button1 = Bounce2::Button();
Bounce2::Button button2 = Bounce2::Button();
Bounce2::Button button3 = Bounce2::Button();
Bounce2::Button button4 = Bounce2::Button();



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


#define BUTTON1 D2
#define BUTTON2 D3
#define BUTTON3 D4
#define BUTTON4 D5

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
  int delay=5;
  button1.interval(delay);
  button2.interval(delay);
  button3.interval(delay);
  button4.interval(delay);

  button1.attach( BUTTON1, INPUT );
  button2.attach( BUTTON2, INPUT );
  button3.attach( BUTTON3, INPUT );
  button4.attach( BUTTON4, INPUT );

  button1.setPressedState(LOW);
  button2.setPressedState(LOW);
  button3.setPressedState(LOW);
  button4.setPressedState(LOW);

}


void button_do (int i, bool* lockJ) {
  zone_partagee = String(i);
      *lockJ = true;
      Serial.print("Numero du bouton appuyé : ");
      Serial.println(zone_partagee);
}

void loop() {
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  int cpt =0;
  if (attente_buzz) {

    if (button1.pressed() & lockJ1 == false) { button_do(1, &lockJ1); };
    if (button2.pressed() & lockJ2 == false) { button_do(2, &lockJ2); };
    if (button3.pressed() & lockJ3 == false) { button_do(3, &lockJ3); };
    if (button4.pressed() & lockJ4 == false) { button_do(4, &lockJ4); };

    if (button1.pressed()) { cpt++; };
    if (button2.pressed()) { cpt++; };
    if (button3.pressed()) { cpt++; };
    if (button4.pressed()) { cpt++; };
    if ( cpt > 1 ) {
      Serial.print(" ZZZ DOUBLE APPUI DE BOUTONS !!!!!!! ");
    };
    if  ( cpt == 1 ) { attente_buzz = false; };

  };
    //attente_buzz = false;
};
