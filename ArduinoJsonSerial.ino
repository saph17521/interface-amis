#include <ArduinoJson.h>
#include <Wire.h>

//Parameters
const int escPin = 3;
int min_throttle = 1000;
int max_throttle = 2000;
unsigned long currentMillis, previousMillis;

unsigned long lastSendTime = 0;  // Pour l'envoi régulier des données
float AccX, AccY, AccZ, AngleRoll, AnglePitch, AngleYaw;

// Fonction pour récupérer les données du MPU6050
void lireMPU6050() {
  Wire.beginTransmission(0x68); // Adresse I2C du MPU6050
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  
  int16_t AccXLSB = Wire.read() << 8 | Wire.read();
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();
  
  AccX = (float)AccXLSB / 4096;
  AccY = (float)AccYLSB / 4096;
  AccZ = (float)AccZLSB / 4096;
  
  AngleRoll = atan(AccY / sqrt(AccX * AccX + AccZ * AccZ)) * (180.0 / 3.14159);
  AnglePitch = -atan(AccX / sqrt(AccY * AccY + AccZ * AccZ)) * (180.0 / 3.14159);
  AngleYaw = atan2(AccZ, AccX) * (180.0 / 3.14159); // Approximation de l'angle Yaw
}

// Fonction pour envoyer des données JSON
void envoyerReponse(DynamicJsonDocument &doc) {
  serializeJson(doc, Serial);
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Wire.begin();
  delay(250);
  
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  if (Wire.endTransmission() != 0) {
    Serial.println("Erreur MPU6050");
  } else {
    Serial.println("MPU6050 OK");
  }
  //Init ESC
  pinMode(escPin, OUTPUT);
  //initProcedure();
}

void loop() {
  // Lecture du MPU6050
  lireMPU6050();

  // Envoi des données
  DynamicJsonDocument dataOut(256);
  dataOut["AccX"] = AccX;
  dataOut["AccY"] = AccY;
  dataOut["AccZ"] = AccZ;
  dataOut["AngleRoll"] = AngleRoll;
  dataOut["AnglePitch"] = AnglePitch;
  dataOut["AngleYaw"] = AngleYaw;
  dataOut["Pression"] = 1;
  envoyerReponse(dataOut);

  // 🔹 3. Lecture des commandes JSON sans bloquer
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    
    DynamicJsonDocument doc(200);
    DeserializationError error = deserializeJson(doc, input);
    if (error) {
      DynamicJsonDocument err(100);
      err["erreur"] = error.c_str();
      envoyerReponse(err);
      return;
    }

    DynamicJsonDocument reponse(200);
    reponse["status"] = "OK";
    
    for (JsonPair kv : doc.as<JsonObject>()) {
      if (String(kv.key().c_str()) == "led") {
        reponse["led"] = (kv.value() == "on") ? "allumée" : "éteinte";
      } else {
        reponse[kv.key()] = kv.value();
      }
    }
    
    envoyerReponse(reponse);
  }
}


void runBrushless() { /* function runBrushless */
  //// Test Brushless routine
  
  Serial.println("running");
  currentMillis = 0;
  previousMillis = millis();
  while (currentMillis < 2000) {
    currentMillis = millis() - previousMillis;
    digitalWrite(escPin, HIGH);
    delayMicroseconds(1350);
    digitalWrite(escPin, LOW);
    delay(20);
  }
  Serial.println("stop");
  currentMillis = 0;
  previousMillis = millis();
  while (currentMillis < 2000) {
    currentMillis = millis() - previousMillis;
    digitalWrite(escPin, HIGH);
    delayMicroseconds(min_throttle);
    digitalWrite(escPin, LOW);
    delay(20);
  }
}

void runMotor(int throttle){
  throttle = map(throttle, 0, 255, min_throttle, max_throttle); // Mise à l'échelle de 0-255 à 1000-2000
  digitalWrite(escPin, HIGH);
  delayMicroseconds(throttle);
  digitalWrite(escPin, LOW);
  delay(20);
}
void initProcedure() { /* function initProcedure */
  //// ESC inittialisation process
  previousMillis = millis();
  Serial.println("throttle up");
  while (currentMillis < 3000) {
    currentMillis = millis() - previousMillis;
    Serial.println(currentMillis);
    digitalWrite(escPin, HIGH);
    delayMicroseconds(max_throttle);
    digitalWrite(escPin, LOW);
    delay(20);
  } //beep- beep-
  currentMillis = 0;
  previousMillis = millis();
  Serial.println("throttle down");
  while (currentMillis < 4500) {
    currentMillis = millis() - previousMillis;
    Serial.println(currentMillis);
    digitalWrite(escPin, HIGH);
    delayMicroseconds(min_throttle);
    digitalWrite(escPin, LOW);
    delay(20);
  } // beep--
  // 1 2 3
}
