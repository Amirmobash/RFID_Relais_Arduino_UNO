#include <SPI.h>
#include <MFRC522.h>

// RFID RC522 Pins
#define SS_PIN 10
#define RST_PIN 9

// Relais Pins
#define RELAY1 7
#define RELAY2 6

// Viele Relaismodule sind LOW-aktiv.
// Falls dein Relais andersherum arbeitet, diese Werte tauschen.
#define RELAY_ON LOW
#define RELAY_OFF HIGH

MFRC522 rfid(SS_PIN, RST_PIN);

// true  = beim nächsten gültigen Scan Relais 1 aktivieren
// false = beim nächsten gültigen Scan Relais 2 aktivieren
bool nextRelayIsOne = true;

// Erlaubte RFID-Karten
// UID: 33 F8 76 B7
byte card1[4] = {0x33, 0xF8, 0x76, 0xB7};

// UID: 0B 38 01 CE
byte card2[4] = {0x0B, 0x38, 0x01, 0xCE};

bool checkCard(byte allowedCard[4]) {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != allowedCard[i]) {
      return false;
    }
  }
  return true;
}

bool isAuthorizedCard() {
  if (rfid.uid.size != 4) {
    return false;
  }

  if (checkCard(card1)) return true;
  if (checkCard(card2)) return true;

  return false;
}

void printCardUID() {
  Serial.print("Karten-UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void activateRelay(byte relayPin) {
  digitalWrite(relayPin, RELAY_ON);
  delay(1000);
  digitalWrite(relayPin, RELAY_OFF);
}

void setup() {
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  digitalWrite(RELAY1, RELAY_OFF);
  digitalWrite(RELAY2, RELAY_OFF);

  Serial.println("RFID Relais Steuerung gestartet");
  Serial.println("Bitte RFID-Karte scannen...");
}

void loop() {
  // Warten, bis eine neue Karte erkannt wird
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // UID der Karte lesen
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  printCardUID();

  if (!isAuthorizedCard()) {
    Serial.println("Zugriff verweigert: Karte nicht erlaubt");
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(1000);
    return;
  }

  Serial.println("Zugriff erlaubt");

  if (nextRelayIsOne) {
    Serial.println("Relais 1 wird fuer 1 Sekunde aktiviert");
    activateRelay(RELAY1);
    nextRelayIsOne = false;
  } else {
    Serial.println("Relais 2 wird fuer 1 Sekunde aktiviert");
    activateRelay(RELAY2);
    nextRelayIsOne = true;
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Kurze Pause, damit dieselbe Karte nicht sofort mehrfach gelesen wird
  delay(1000);
}
