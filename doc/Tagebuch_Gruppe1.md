# Gruppe 1 - Grün

Teilnehmer:

* Julian Finck
* Reneé Keller
* Lea Morschel
* Adrian Sorge
* Julian Weihe

---

### Zielsetzung:
* Kollisionsvermeidung

* Fahrlinienerkennung und -verfolgung
* (optional) Straßenschilderkennung / QR-Codes zur Richtungsentscheidung an Kreuzungen
* (optional) Umgebungskarte


Kleinschrittiger:
* Motorsteuerung (1-2 Wochen)
* Linienerkennung (1-2 Wochen)
* Filter/ Messwertoptimierung
* Zusammenführen

---
# Tagebuch

## 1. Woche 20. - 26.11.2017

#### Überlegen der Zielsetzung

#### Erarbeiten der groben Projekt-Anfangs-Struktur
- *doc* für Dokumentation; *src* für Quellcode
- Branches für einzelne Features angelegt
  - kalman : Ultrasonic Datenfilterung
  - linedetection : blaue Linienerkennung und Berechnung der Mittelpunktabweichung
  - motorcontrol : Ansteuerung des Arduino-Kommunikationsblockes

#### Einrichten der VM:
- Zunächst .raw zu .vmdk für boxes, aber keine Hardwarbeschleunigung möglich
- *.raw* to *.vdi* für Virtualbox
- Partition erweitern, Größe anpassen
  
#### Einrichten der Entwicklungsumgebung
- C-Lion installieren
- Umgebungsvariable setzen zur Autovervollständigung der ADTF-Funktionen

---
## 2. Woche 27.11 - 3.12.2017

#### Einfindung in ADTF
- Grobe ADTF-Ordnerstruktur nachvollziehen (u.a. anhand der Dokumentation)
- Einfinden in Projektstrukturen und Abhängigkeiten
- Template-Filter nachvollzogen und in eigenem Projekt angepasst implementiert
  - Einfinden in visuelle Repräsentation der Pins und setzen von *propertys*
  - Einarbeiten in Daten-Kommunikations-Schnittstellen (Daten, Video, Arduino)
    - Erstellen der Schnittstelle Kommunikationsschnittstelle *Protocol.h*
    - Einheitliche Definitionen der Pins verschiedener Bausteine
    - Definition der zu sendenen und empfangenen Daten
    - Funktion zum Senden und Empfangen beliebiger Daten, mit Ausnahme von
        Bildern/Video und Daten zum Arduinokommunikation Baustein

#### Arbeit an Branches
- Motorsteuerung
  - TODO
- Linienerkennung
  - TODO
- Filter/ Messwertoptimierung
  - Erstellung eines Median-Filters mit variabler Listenlänge
  - Erstellung eines Kalman-Filters, genaue Justierung der parameter fehlt noch

#### Mergen der Branches zur weiteren gemeinsamen Arbeit auf dem Master-Zweig
- Begründung:
  - Stete Weiterentwicklung übergreifender Kommunikation über gemeinsame Funktionen/ Schnittstellen
  - Aufwändige kontinuierliche Synchronisation der Branches mit dem Master

---
## 3. Woche 4.12 - 10.12.2017

#### Erstellung des VideoToFile Bausteins
Da in der VM Ausgabevideos aufgrund fehlender Hardwarebeschleunigung nicht angezeigt werden kann,
ist es mit dem Baustein möglich die Daten in einer *.avi* Videodatei zu speichern.

#### Protocol.h um Schnittstelle für Images erweitert
Es lassen sich nun auch direkt *cv::Mat* Typen zwischen Bausteinen versenden.

#### Verknüpfung der Motorsteuerung mit dem Ultrasonic Sensoren über Filter und Logik
Stoppen der Motoren bei eingestellter Näherung

#### Motorsteuerung
- TODO

#### Linienerkennung
- TODO

#### Filter/ Messwertoptimierung
- Erstellung des *UltrasonicLogik*-Bausteins
