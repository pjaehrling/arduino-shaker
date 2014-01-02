/*
 Arduino connected to a ADXL335 (3 Axis Accelerometer) and a Speaker

 The circuit:
 analog 0: accelerometer self test
 analog 1: z-axis
 analog 2: y-axis
 analog 3: x-axis
 analog 4: ground
 analog 5: vcc
 
 created 29.12.2013
 by Philipp Jährling
*/

// the pins
const int xPin = A3;  // x-Achse
const int yPin = A2;  // y-Achse
const int zPin = A1;  // z-Achse
const int ledPin = 13;
const int audioPin = 3;

const int readDelay = 100;
const int threshold = 40;
const int responseTimeIter = 20;

boolean ledOn = false;
int ledOnIter = 0;
int lastShake = 0; // delays addieren
int lastX = 0;
int lastY = 0;
int lastZ = 0;

int melody[] = {262, 196, 220, 196, 0, 247, 262};
int melodyLength = 7;
int melodyIter = 0;


// Ausgabe des Analogsignals in Digital  0 - 1023
// 0g = 512 (1,65V bei 3.3V Eingangsspannung) --> ca. 103 pro weiterem g (Aus Datenblatt berechnet)
void setup()
{
  // initialisiert die Seriele (USB) Verbindung zum PC, über welche die Logdaten geschrieben werden
  // Serial.begin(9600);  
  
  // Referenzspannung auf AREF Input (EXTERNAL) setzten. Dort liegen 3.3V an.
  // Da bei Uno die Referenzspannung wohl 5V (default) sind, wir aber für den Accelerometer 3.3V verwenden (Laut Datenblatt sollten wir dort nicht mehr als 3.6V anlegen).
  // Sonst verkleinern wir uns quasi vom Start weg den Wertebereich.
  analogReference(EXTERNAL);
  
  // Pin-Modes
  pinMode(ledPin, OUTPUT);   // setze digitalen Pin 13 als LED-Output
  pinMode(audioPin, OUTPUT); // setze digitalen Pin 3 als Audio-Output
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(zPin, INPUT);
}


void loop() 
{
  // Wartezustand - Blinken <--> Geschüttelt dauerhaft an
  if (ledOn && ledOnIter <= 0) {
    digitalWrite(ledPin, LOW); // schalte LED aus
    melodyIter = 0;
    ledOn = false;
  } else {
    digitalWrite(ledPin, HIGH); // schalte LED ein
    ledOn = true;
    if (ledOnIter > 0) {
      tone(audioPin, melody[melodyIter], readDelay);  // spielt einen Ton ab (Pin, Frequenz, Dauer)
      melodyIter = (melodyIter >= melodyLength-1) ? 0 : melodyIter+1;
      ledOnIter--;
    }
  }
  
  int x = analogRead(xPin);
  int y = analogRead(yPin);
  int z = analogRead(zPin);
  
  if ( gotShaked(x,y,z) ) {
    ledOnIter = responseTimeIter;
    // Serial.print("Shaked!!");
    // Serial.println();
  }
  
  // Warten vor nächsten Durchgang
  delay(readDelay);
}

boolean gotShaked(int x, int y, int z) 
{
  int deltaX = 0;
  int deltaY = 0;
  int deltaZ = 0;

  // Initialer Fall
  if ( (lastX == 0)  && (lastY == 0) && (lastZ == 0)) {
    setLast(x,y,z);
    return false;
  }

  // Unterschied setzen
  deltaX = abs(lastX - x);
  deltaY = abs(lastY - y);
  deltaZ = abs(lastZ - z);

  // Testen ob mindest Zwei Achesen den Schwellwert übertretten
  if (  ((deltaX > threshold) && (deltaY > threshold)) || 
        ((deltaX > threshold) && (deltaZ > threshold)) || 
        ((deltaY > threshold) && (deltaZ > threshold))  ) {
          
    setLast(x,y,z);
    
    // Wenn der Schwellwert 2 mal hintereinander übertroffen wurde, ist es ein Schütteln, falls nur ein mal sehen wir weiter
    if (lastShake >= readDelay + readDelay) {
      lastShake = 0;
      return true;
      
    } else {
      lastShake += readDelay;
      return false;
    }
    
  } else {
    lastShake = 0;
    setLast(x,y,z);
    return false; 
  }
}

void setLast(int x, int y, int z) 
{
  lastX = x;
  lastY = y;
  lastZ = z;
}
