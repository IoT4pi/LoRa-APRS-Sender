# LoRa-APRS-Sender
Send encoded GPS Position over Lora to an LoRA APRS Gateway 
Der Sender ist nach Prinzip des folgenden Links aufgebaut. 
http://www.kh-gps.de/lora.htm

Copyright of the author Stuart Robinson - 02/07/2015 15:00
These programs may be used free of charge for personal, recreational and educational purposes only.

This program, or parts of it, may not be used for or in connection with any commercial purpose without
the explicit permission of the author Stuart Robinson.

Die Software des Senders wurde geändert, sodass die GPS Daten einerseits in Klartext oder in komprimierter Form (Mic-e) übertragen werden.
Sender arbeitet LoRa APRS Gateway zusammen. 

Ändere die Zeile 43 mit 
String Tcall="OE1KEB-12";   //your Call Sign

und setze Dein Rufzeichen ein. 
