#include <DHT.h>
#include <PulseSensorPlayground.h>
#include <SoftwareSerial.h>

#define DHTPIN 2          // Broche DHT11
#define DHTTYPE DHT11
#define MIC_PIN A2        // Broche microphone
#define PULSE_PIN A3      // Broche Pulse Sensor

// Configuration Pulse Sensor
const int Threshold = 900;  // Ajusté à votre plage de valeurs
PulseSensorPlayground pulseSensor;
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial bluetooth(10, 11); // RX, TX pour Bluetooth

unsigned long previousMillis = 0;
const long interval = 2000;  // Intervalle de 2s pour DHT/micro
unsigned long lastBeatTime = 0;
int minBeatInterval = 500; // Au moins 500ms entre battements

void setup() {
  Serial.begin(9600);     // Debug sur le moniteur série
  bluetooth.begin(9600);  // Communication série Bluetooth
  
  // Initialisation Pulse Sensor
  pulseSensor.analogInput(PULSE_PIN);
  pulseSensor.setThreshold(Threshold);
  pulseSensor.begin();
  
  // Initialisation DHT11
  dht.begin();
}

void loop() {
  // Lecture du Pulse Sensor
  int signal = analogRead(PULSE_PIN);
  
  // Lecture périodique DHT11 et microphone
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Lecture température
    float temp = dht.readTemperature();
    Serial.print("TEMP:");
    Serial.print(temp);
    Serial.println("|");
    
    // Lecture microphone
    int sound = analogRead(MIC_PIN);
    Serial.print("SOUND:");
    Serial.print(sound);
    Serial.println("|");
    
    // Affichage valeur brute du capteur de pouls pour débogage
    Serial.print("RAW_PULSE:");
    Serial.print(signal);
    Serial.println("|");
  }
  
  // Détection de battement de cœur
  if (pulseSensor.sawStartOfBeat()) {
    unsigned long now = millis();
    if (now - lastBeatTime > minBeatInterval) {
      lastBeatTime = now;
      int bpm = pulseSensor.getBeatsPerMinute();
      
      Serial.println("⚡ Battement détecté !");
      Serial.print("BPM:");
      Serial.print(bpm);
      Serial.println("|");
      
      // Envoyer au Bluetooth également
      bluetooth.print("BPM:");
      bluetooth.print(bpm);
      bluetooth.println("|");
    }
  }
  
  // Court délai sans bloquer la détection
  delay(10);
}
