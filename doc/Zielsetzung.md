###Grobziele:
- Fahrbahnmarkierung selbstständig folgen
- Vermeidung der Kollision mit Hindernissen

###Umsetzung - Mittel und Verhältnisse:
- Mithilfe von Daten der RGB-Frontkamera und Ultraschallsensoren
- Fahrbahn mittig und durchgehend mit blauem Klebeband markiert
- Lichtverhältnisse  müssen nicht konstant sein
  - Auswertung der Kamerabilder soll aber jederzeit möglich sein
- Hindernisse so beschaffen, dass sie von den Ultraschallsensoren
detektiert werden können
  - Mindestgröße, Schallharte Oberfläche, usw. sind gewährleistet
- Bei nahendem Hindernis
  - Auto soll langsamer werden und vor ihm anhalten
  - Fährt erst weiter, wenn die Fahrbahn wieder frei ist


###Zielsetzung - Textuelle Beschreibung

Ziel ist es, das Modellauto in die Lage zu versetzen,
einer Fahrbahnmarkierung selbstständig folgen zu können,
ohne dabei mit Hindernissen zu kollidieren.
Dafür wird auf die eingehenden Daten der RGB-Frontkamera
sowie der Ultraschallsensoren zugegriffen. 
Die Fahrbahn wird mittig und durchgehend mit blauem Klebeband markiert.
Die Lichtverhältnisse  müssen nicht konstant sein; die Auswertung
der Kamerabilder soll aber jederzeit möglich sein. Hindernisse
sind derart beschaffen, dass sie von den Ultraschallsensoren
detektiert werden können (Mindestgröße, Schallharte Oberfläche,
usw. sind gewährleistet).
Ist ein Hindernis zu nah an bzw. auf der Fahrbahn, soll das
Modellauto langsamer werden und vor dem Hindernis anhalten.
Das Modellauto fährt erst weiter, wenn die Ultraschallsensoren
signalisieren, dass die Fahrbahn wieder frei ist.