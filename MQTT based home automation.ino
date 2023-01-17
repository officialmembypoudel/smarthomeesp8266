// #include <ESP32HTTPUpdateServer.h>
#include <Servo.h>
#include <EspMQTTClient.h>

Servo servo;

// #include "EspMQTTClient.h"
const int led = 16;
const int wifiLed = 4;
const int powerS = 2;
const int testLed = 12;
const int pingPin = 15;  // Trigger Pin of Ultrasonic Sensor
const int echoPin = 14;  // Echo Pin of Ultrasonic Sensor
boolean doorState = false;



EspMQTTClient client(
  "Rock on",
  "Demon@slayer",
  "109.205.181.241",  // MQTT Broker server ip
  // "MQTTUsername",   // Can be omitted if not needed
  // "MQTTPassword",   // Can be omitted if not needed
  "autochalitmqtt"  // Client name that uniquely identify your device
);

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(wifiLed, OUTPUT);
  pinMode(testLed, OUTPUT);
  pinMode(powerS, OUTPUT);
  digitalWrite(led, LOW);
  digitalWrite(powerS, LOW);
  digitalWrite(wifiLed, LOW);
  //   pinMode(echoPin, INPUT);
  // pinMode(pingPin, OUTPUT);
  servo.attach(13);
  servo.write(180);
  delay(500);
}

void onConnectionEstablished() {

  client.subscribe("groundLight", [](const String &payload) {
    Serial.println(payload);
    if (payload == "on") {
      digitalWrite(led, HIGH);
    } else if (payload == "off") {
      digitalWrite(led, LOW);
    }
  });
  client.subscribe("fan", [](const String &payload) {
    Serial.println(payload);
    if (payload == "on") {
      digitalWrite(powerS, HIGH);
    } else if (payload == "off") {
      digitalWrite(powerS, LOW);
    }
  });
  client.subscribe("level1Light", [](const String &payload) {
    Serial.println(payload);
    if (payload == "on") {
      digitalWrite(wifiLed, HIGH);
    } else if (payload == "off") {
      digitalWrite(wifiLed, LOW);
    }
  });
  client.subscribe("door", [](const String &payload) {
    Serial.println(payload);
    if (payload == "on") {
      doorState = true;
      servo.write(10);
      delay(1000);
    } else if (payload == "off") {
      doorState = false;
      servo.write(180);
      delay(1000);
    }
  });

  client.publish("mytopic/test", "This is a message");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(testLed, HIGH);
    Serial.print("connected");
  } else {
    digitalWrite(testLed, LOW);
    Serial.print("Disconnected");
  }
  long duration, inches, cm;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(100);


  client.loop();

  if (doorState == false && cm < 20) {
    servo.write(0);
    delay(1000);
  } else if (doorState == false && cm > 20) {
    servo.write(180);
    delay(1000);
  }
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}