#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

#define RELAY1 7
#define RELAY2 6

#define RELAY_ON LOW
#define RELAY_OFF HIGH

MFRC522 rfid(SS_PIN, RST_PIN);

bool nextRelayIsOne = true;

byte card1[4] = {0x33, 0xF8, 0x76, 0xB7};
byte card2[4] = {0x0B, 0x38, 0x01, 0xCE};

void relaysOff() {
  digitalWrite(RELAY1, RELAY_OFF);
  digitalWrite(RELAY2, RELAY_OFF);
}

bool checkCard(byte allowedCard[4]) {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != allowedCard[i]) return false;
  }
  return true;
}

bool isAuthorizedCard() {
  if (rfid.uid.size != 4) return false;
  if (checkCard(card1)) return true;
  if (checkCard(card2)) return true;
  return false;
}

void printCardUID() {
  Serial.print("Karten-UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void activateRelay(byte relayPin) {
  relaysOff();

  digitalWrite(relayPin, RELAY_ON);
  delay(1000);
  digitalWrite(relayPin, RELAY_OFF);

  relaysOff();
}

void setup() {
  // خیلی مهم: قبل از OUTPUT کردن، وضعیت OFF را بنویس
  digitalWrite(RELAY1, RELAY_OFF);
  digitalWrite(RELAY2, RELAY_OFF);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  relaysOff();

  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  relaysOff();

  Serial.println("RFID Relais Steuerung gestartet");
  Serial.println("Relais sind AUS");
}

void loop() {
  relaysOff();  // حالت پیش‌فرض امن

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  printCardUID();

  if (!isAuthorizedCard()) {
    Serial.println("Zugriff verweigert");
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(1000);
    return;
  }

  Serial.println("Zugriff erlaubt");

  if (nextRelayIsOne) {
    Serial.println("Relais 1 fuer 1 Sekunde aktiv");
    activateRelay(RELAY1);
    nextRelayIsOne = false;
  } else {
    Serial.println("Relais 2 fuer 1 Sekunde aktiv");
    activateRelay(RELAY2);
    nextRelayIsOne = true;
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000);
}
