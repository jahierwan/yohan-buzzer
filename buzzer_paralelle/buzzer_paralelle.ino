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

// la communication bluetooth se fait via ce buffer
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

#define BUTTON1 D2
#define BUTTON2 D3
#define BUTTON3 D4
#define BUTTON4 D5

#define LEDJ1 D7
#define LEDJ2 D8
#define LEDJ3 D9
#define LEDJ4 D10

bool LEDJ1on = true;
bool LEDJ2on = true;
bool LEDJ3on = true;
bool LEDJ4on = true;
/*
#define BUTTON5 D6
#define BUTTON6 D7
#define BUTTON7 D8
#define BUTTON8 D9
#define BUTTON9 D10
*/
// effet de bord : recupere la valeur de qu_suivante via le BT
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

// appellé quand le bouton "question suivante" est pressé
// et toutes les secondes (via timer2 dans MIT)
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      Serial.print("MyCallbacks: ");

      // on veut un entier entre 0 et 9 en premiere position
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

        digitalWrite(LEDJ1, HIGH);
        digitalWrite(LEDJ2, HIGH);
        digitalWrite(LEDJ3, HIGH);
        digitalWrite(LEDJ4, HIGH);
        LEDJ1on = true;
        LEDJ2on = true;
        LEDJ3on = true;
        LEDJ4on = true;

      } else if (qu_suivante == "f"){
	//si la reponse donné n'est pas bonne on peut à nouveau buzzer
	// (mais le joueur reste locké)
        attente_buzz = true;
        if (LEDJ1on == true) {
          digitalWrite(LEDJ1, HIGH);
        } else {
          digitalWrite(LEDJ1, LOW);
        }  
        if (LEDJ2on == true) {
          digitalWrite(LEDJ2, HIGH);
        } else {
          digitalWrite(LEDJ2, LOW);
        }
        if (LEDJ3on == true) {
          digitalWrite(LEDJ3, HIGH);
        } else {
          digitalWrite(LEDJ3, LOW);
        }
        if (LEDJ4on == true) {
          digitalWrite(LEDJ4, HIGH);
        } else {
          digitalWrite(LEDJ4, LOW);
        }  
	Serial.print("qu_suivante n'est pas true. Il vaut : "); Serial.print(qu_suivante);
      } else if (qu_suivante == "r") {
	// Bouton redemarrage de la carte arduino
	ESP.restart();
      }
      Serial.println("Fin de MyCallbacks ");
    }
};

void setup_button (Bounce2::Button *b, int pin) {
  b->interval(5); // un bon compromis de ce qu'en disent les gens
  b->attach(pin, INPUT );
  b->setPressedState(HIGH);
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

  // initialisations des boutons
  setup_button(&button1, BUTTON1);
  setup_button(&button2, BUTTON2);
  setup_button(&button3, BUTTON3);
  setup_button(&button4, BUTTON4);
  
  pinMode(LEDJ1, OUTPUT);
  pinMode(LEDJ2, OUTPUT);
  pinMode(LEDJ3, OUTPUT);
  pinMode(LEDJ4, OUTPUT);

  digitalWrite(LEDJ1, HIGH);
  digitalWrite(LEDJ2, HIGH);
  digitalWrite(LEDJ3, HIGH);
  digitalWrite(LEDJ4, HIGH);

}

int ledClignotante;
void button_do (int i, bool* lockJ) {
  *lockJ = true;
  attente_buzz = false;
  zone_partagee = String(i);
  Serial.print("Numero du bouton appuyé : "); Serial.println(zone_partagee);

  digitalWrite(LEDJ1, LOW);
  digitalWrite(LEDJ2, LOW);
  digitalWrite(LEDJ3, LOW);
  digitalWrite(LEDJ4, LOW);
  if (i == 1) {
    LEDJ1on = false;
    ledClignotante = LEDJ1;
  } else if (i == 2) {
    LEDJ2on = false;
    ledClignotante = LEDJ2;
  } else if (i == 3) {
    LEDJ3on = false;
    ledClignotante = LEDJ3;
  } else {
    LEDJ4on = false;
    ledClignotante = LEDJ4;
  }
}

unsigned long currentTime;
boolean ledState = 0;

void loop() {
  button1.update();
  button2.update();
  button3.update();
  button4.update();

  int cpt = 0;
  if (attente_buzz) {
    if (button1.pressed() & lockJ1 == false) { button_do(1, &lockJ1); };
    if (button2.pressed() & lockJ2 == false) { button_do(2, &lockJ2); };
    if (button3.pressed() & lockJ3 == false) { button_do(3, &lockJ3); };
    if (button4.pressed() & lockJ4 == false) { button_do(4, &lockJ4); };


  } else {
    if (millis() - currentTime > 500) {
       currentTime = millis();
       ledState =! ledState;
       digitalWrite(ledClignotante, ledState);
    }
  };
};
