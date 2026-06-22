# RFID Relais Steuerung mit Arduino UNO

Dieses Projekt steuert zwei Relais mit einem Arduino UNO und einem RFID-RC522-Modul.

## Funktion

- Beim ersten gültigen RFID-Scan wird **Relais 1** für 1 Sekunde aktiviert.
- Beim zweiten gültigen RFID-Scan wird **Relais 2** für 1 Sekunde aktiviert.
- Danach beginnt die Reihenfolge wieder von vorne.
- Nur gespeicherte RFID-Karten dürfen die Relais schalten.

## Verwendete Hardware

- Arduino UNO
- RFID-Modul RC522
- 2-Kanal Relaismodul 5V
- RFID-Karten oder RFID-Tags
- Jumper-Kabel
- Externe Spannungsversorgung für das Schloss, falls benötigt

## Benötigte Arduino-Bibliothek

Installiere in der Arduino IDE:

```text
MFRC522 by GithubCommunity
```

Pfad:

```text
Arduino IDE -> Sketch -> Include Library -> Manage Libraries -> MFRC522 suchen -> Installieren
```

## Anschlussplan

### RFID RC522 an Arduino UNO

| RC522 Pin | Arduino UNO |
|----------|-------------|
| SDA      | D10         |
| SCK      | D13         |
| MOSI     | D11         |
| MISO     | D12         |
| RST      | D9          |
| 3.3V     | 3.3V        |
| GND      | GND         |

**Wichtig:** Das RC522-Modul muss mit **3.3V** betrieben werden, nicht mit 5V.

### Relaismodul an Arduino UNO

| Relaismodul | Arduino UNO |
|------------|-------------|
| IN1        | D7          |
| IN2        | D6          |
| VCC        | 5V          |
| GND        | GND         |

## RFID-Karten im Code

Aktuell sind diese Karten freigeschaltet:

```cpp
33 F8 76 B7
0B 38 01 CE
```

Im Code stehen sie so:

```cpp
byte card1[4] = {0x33, 0xF8, 0x76, 0xB7};
byte card2[4] = {0x0B, 0x38, 0x01, 0xCE};
```

## Weitere Karte hinzufügen

1. UID der Karte mit dem Serial Monitor auslesen.
2. Neue Karte im Code hinzufügen, zum Beispiel:

```cpp
byte card3[4] = {0xAA, 0xBB, 0xCC, 0xDD};
```

3. In der Funktion `isAuthorizedCard()` ergänzen:

```cpp
if (checkCard(card3)) return true;
```

## Relais-Logik

Viele Relaismodule sind **LOW-aktiv**. Deshalb ist im Code eingestellt:

```cpp
#define RELAY_ON LOW
#define RELAY_OFF HIGH
```

Falls dein Relais genau andersherum funktioniert, ändere es zu:

```cpp
#define RELAY_ON HIGH
#define RELAY_OFF LOW
```

## Projekt starten

1. Arduino IDE öffnen.
2. Datei `src/RFID_Relais_Arduino_UNO.ino` öffnen.
3. Board auswählen: **Arduino UNO**.
4. Port auswählen, zum Beispiel **COM8**.
5. Code hochladen.
6. Serial Monitor mit **9600 Baud** öffnen.
7. Karte scannen.

## Sicherheitshinweis

Bei elektrischen Türschlössern oder höheren Spannungen muss eine passende externe Stromversorgung verwendet werden. Keine hohen Lasten direkt über den Arduino betreiben.
