#include <SPI.h>
#include <MFRC522.h>

const byte ssPin = 10;
const byte rstPin = 9;

const byte relayOnePin = 7;
const byte relayTwoPin = 6;

const byte relayOn = LOW;
const byte relayOff = HIGH;

MFRC522 rfid(ssPin, rstPin);

bool useRelayOneNext = true;
unsigned long lastCardTime = 0;

byte firstCard[4] = {0x33, 0xF8, 0x76, 0xB7};
byte secondCard[4] = {0x0B, 0x38, 0x01, 0xCE};

void setup() {
  digitalWrite(relayOnePin, relayOff);
  digitalWrite(relayTwoPin, relayOff);

  pinMode(relayOnePin, OUTPUT);
  pinMode(relayTwoPin, OUTPUT);

  turnRelaysOff();

  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  turnRelaysOff();

  Serial.println("RFID relay system started");
  Serial.println("Relays are OFF");
  Serial.println("Scan an authorized card");
}

void loop() {
  turnRelaysOff();

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  if (millis() - lastCardTime < 1500) {
    stopCard();
    return;
  }

  lastCardTime = millis();

  printCardUid();

  if (!cardIsAllowed()) {
    Serial.println("Access denied");
    stopCard();
    return;
  }

  Serial.println("Access allowed");

  if (useRelayOneNext) {
    Serial.println("Relay 1 is active for 1 second");
    pulseRelay(relayOnePin);
    useRelayOneNext = false;
  } else {
    Serial.println("Relay 2 is active for 1 second");
    pulseRelay(relayTwoPin);
    useRelayOneNext = true;
  }

  stopCard();
}

void turnRelaysOff() {
  digitalWrite(relayOnePin, relayOff);
  digitalWrite(relayTwoPin, relayOff);
}

void pulseRelay(byte relayPin) {
  turnRelaysOff();

  digitalWrite(relayPin, relayOn);
  delay(1000);

  digitalWrite(relayPin, relayOff);
  delay(100);

  turnRelaysOff();
}

bool cardIsAllowed() {
  if (rfid.uid.size != 4) {
    return false;
  }

  if (cardMatches(firstCard)) {
    return true;
  }

  if (cardMatches(secondCard)) {
    return true;
  }

  return false;
}

bool cardMatches(byte allowedCard[4]) {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != allowedCard[i]) {
      return false;
    }
  }

  return true;
}

void printCardUid() {
  Serial.print("Card UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }

  Serial.println();
}

void stopCard() {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
