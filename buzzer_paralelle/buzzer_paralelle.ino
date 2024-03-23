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
#include <Esp.h>

String qu_suivante;
String zone_partagee = "";
bool attente_buzz = true;
bool debug = true;
bool lockJ1 = false;
bool lockJ2 = false;
bool lockJ3 = false;
bool lockJ4 = false;
bool lockJ5 = false;
bool lockJ6 = false;
bool lockJ7 = false;
bool lockJ8 = false;
bool lockJ9 = false;

Bounce2::Button button1 = Bounce2::Button();
Bounce2::Button button2 = Bounce2::Button();
Bounce2::Button button3 = Bounce2::Button();
Bounce2::Button button4 = Bounce2::Button();
Bounce2::Button button5 = Bounce2::Button();
Bounce2::Button button6 = Bounce2::Button();
Bounce2::Button button7 = Bounce2::Button();
Bounce2::Button button8 = Bounce2::Button();
Bounce2::Button button9 = Bounce2::Button();

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


// effet de bord : recupere la valuer de qu_suivante via le BT
void recupere_valeur (BLECharacteristic *pCharacteristic) {
  Serial.print("recupere_valeur : debut\n");

      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
	Serial.print("recupere_valeur : value.length() > 0\n");
        qu_suivante = "";
        for (int i = 0; i < value.length(); i++){
          // Serial.print(value[i]);
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

      // on veut un entier entre 0 et 9 en premiere position

      //if (zone_partagee == "1" || zone_partagee == "2" || zone_partagee == "3" || zone_partagee == "4" ) {
     if (zone_partagee.charAt(0) > int('0') &&  zone_partagee.charAt(0) <= int('9')) {
	transferer(pCharacteristic);
      };
      recupere_valeur(pCharacteristic);
      if (qu_suivante == "t") {
        Serial.println("qu_suivante = t => je met le mode 'attente du buzz' a true");
        attente_buzz = true;
        lockJ1 = false;
        lockJ2 = false;
        lockJ3 = false;
        lockJ4 = false;
        lockJ5 = false;
        lockJ6 = false;
        lockJ7 = false;
        lockJ8 = false;
        lockJ9 = false;
      } else if (qu_suivante == "f"){
	//si la reponse donné n'est pas bonne on peut à nouveau buzzer
	// (mais le joueur sera locké)
        attente_buzz = true;
	Serial.print("qu_suivante n'est pas true. Il vaut : "); Serial.print(qu_suivante);
      } else if (qu_suivante == "r") { // Bouton redemarrage de la carte arduino
	ESP.restart();
      }
      Serial.println("Fin de MyCallbacks ");
    }
};


#define BUTTON1 D2
#define BUTTON2 D3
#define BUTTON3 D4
#define BUTTON4 D5
#define BUTTON5 D6
#define BUTTON6 D7
#define BUTTON7 D8
#define BUTTON8 D9
#define BUTTON9 D10

void setup_button (Bounce2::Button *b, int pin) {
 b->interval(5);
 b->attach(pin, INPUT );
 b->setPressedState(LOW);
}

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
  setup_button(&button1, BUTTON1);
  setup_button(&button2, BUTTON2);
  setup_button(&button3, BUTTON3);
  setup_button(&button4, BUTTON4);
  /* Pour 9 boutons */
  /*
  setup_button(&button5, BUTTON5);
  setup_button(&button6, BUTTON6);
  setup_button(&button7, BUTTON7);
  setup_button(&button8, BUTTON8);
  setup_button(&button9, BUTTON9);
  */
}


void button_do (int i, bool* lockJ) {
  *lockJ = true;
  attente_buzz = false;
  zone_partagee = String(i);
  Serial.print("Numero du bouton appuyé : ");
  Serial.println(zone_partagee);
}

void loop() {
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  /* Pour 9 boutons */
  /*
  button5.update();
  button6.update();
  button7.update();
  button8.update();
  button9.update();
  */
  int cpt = 0;
  if (attente_buzz) {

    if (button1.pressed() & lockJ1 == false) { button_do(1, &lockJ1); };
    if (button2.pressed() & lockJ2 == false) { button_do(2, &lockJ2); };
    if (button3.pressed() & lockJ3 == false) { button_do(3, &lockJ3); };
    if (button4.pressed() & lockJ4 == false) { button_do(4, &lockJ4); };
    /* Pour 9 boutons */
    /*
    if (button5.pressed() & lockJ5 == false) { button_do(5, &lockJ5); };
    if (button6.pressed() & lockJ6 == false) { button_do(6, &lockJ6); };
    if (button7.pressed() & lockJ7 == false) { button_do(7, &lockJ7); };
    if (button8.pressed() & lockJ8 == false) { button_do(8, &lockJ8); };
    if (button9.pressed() & lockJ9 == false) { button_do(9, &lockJ9); };
    */
    // un petit test pour voir si c'est possible (on n'a pas réussit à faire de
    // double appui!)
    if (button1.pressed()) { cpt++; };
    if (button2.pressed()) { cpt++; };
    if (button3.pressed()) { cpt++; };
    if (button4.pressed()) { cpt++; };
    /* Pour 9 boutons */
    /*
    if (button5.pressed()) { cpt++; };
    if (button6.pressed()) { cpt++; };
    if (button7.pressed()) { cpt++; };
    if (button8.pressed()) { cpt++; };
    if (button9.pressed()) { cpt++; };
    */
    if ( cpt > 1 ) {
      Serial.print(" ZZZ DOUBLE APPUI DE BOUTONS !!!!!!! ");
    };


  };
};
