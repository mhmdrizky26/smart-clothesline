#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6ME5rLb3D"
#define BLYNK_TEMPLATE_NAME "SmartClothesline"
#define BLYNK_AUTH_TOKEN "8HYudJj0yOdktrB_sOMWO5cYdoNBxa__"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "DHTesp.h"
#include <Stepper.h>

const int stepsPerRevolution1 = 2048;



const int stepsPerRevolution2 = 2048;
Stepper myStepper1(stepsPerRevolution1, 18, 12, 19, 13);
Stepper myStepper2(stepsPerRevolution2, 22, 25, 23, 26);

int cahaya = 34;
int hujan = 15;
int buzzer = 4;
int dhtpin = 5;
int ledpin = 32;
DHTesp dhtSensor;

const char* ssid = "LAB-BASIS DATA";
const char* password = "BasdatSpot2022";

String labelhujan = "Tidak Hujan";
String labelcahaya = "Cerah";

void setup() {
  // put your setup code here, to run once:
  myStepper1.setSpeed(10);
  myStepper2.setSpeed(10);
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  pinMode(cahaya, INPUT);
  pinMode(hujan, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ledpin,OUTPUT);
  dhtSensor.setup(dhtpin, DHTesp::DHT11);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");  
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("WiFi : ");
  Serial.println(ssid);
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  tempandhum();
  rainandlight();
}

void tempandhum(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();


  float temperature = data.temperature;
  Serial.print("Suhu : ");
  Serial.println(temperature);
  Blynk.virtualWrite(V0, temperature);

  float humidity = data.humidity;
  Serial.print("Kelembapan : ");
  Serial.println(humidity);
  Blynk.virtualWrite(V1, humidity);
  delay(1000);
}
void rainandlight(){
  int data_hujan = digitalRead(hujan);
  bool kondisi_hujan = (data_hujan == LOW);
  Serial.print("Hujan : ");
  Serial.println(data_hujan);
  delay(1000);
  int data_cahaya = digitalRead(cahaya);
  bool kondisi_cahaya = (data_cahaya == LOW);
  Serial.print("Cahaya : ");
  Serial.println(data_cahaya);
  labelhujan = kondisi_hujan ? "Hujan" : "Tidak Hujan";
  labelcahaya = kondisi_cahaya ? "Cerah" : "Mendung";
  Blynk.virtualWrite(V2, labelhujan);
  Blynk.virtualWrite(V3, labelcahaya);

  // 0 = hujan && 1 = cuaca mendung
  if (data_hujan == 0 && data_cahaya == 1) {
    Blynk.logEvent("hujan","Awas Hujan");
    for (int i = 0; i < 7; i++) {
      digitalWrite(buzzer, HIGH);
      myStepper1.step(stepsPerRevolution1);
      digitalWrite(buzzer, LOW);
    }
    for (int i = 0; i < 5; i++) {
      myStepper2.step(-stepsPerRevolution2);
    }
  } 
  // 1 = tidak hujan && 0 = cuaca cerah
  if (data_hujan == 1 && data_cahaya == 0) {
    Blynk.logEvent("cerah","Cuaca Cerah");
    for (int i = 0; i < 7; i++) {
      digitalWrite(buzzer, HIGH);
      myStepper1.step(-stepsPerRevolution1);
      digitalWrite(buzzer, LOW);
    }
    for (int i = 0; i < 5; i++) {
      myStepper2.step(stepsPerRevolution2);
    } 
  }
}
BLYNK_WRITE(V4){
  if ( param.asInt() == 1){
    digitalWrite(ledpin,HIGH);
    Serial.println("Lampu Nyala");
  }
  else if ( param.asInt() == 0 ){
    digitalWrite(ledpin,LOW);
    Serial.println("Lampu Mati");
  }
}
