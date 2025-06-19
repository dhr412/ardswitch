#include <WiFiS3.h>
#include "wifi-config.h"

const char* serverUrl = "http://192.168.1.100:32768/triggered";

// Ultrasonic sensor pins
const int trigPin = 9;
const int echoPin = 10;

// Constants
const unsigned long COOLDOWN = 210000;
const float TRIGGER_DISTANCE = 150.0;

unsigned long lastRequestTime = 0;
bool wasTriggered = false;

WiFiClient wifiClient;

void setup() {
    Serial.begin(9600);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
}

void loop() {
    unsigned long currentTime = millis();
    bool sendRequest = false;

    if (currentTime - lastRequestTime < COOLDOWN) {
        float distance = getDistance();
        bool isTriggered = (distance > 0 && distance < TRIGGER_DISTANCE);

        if (isTriggered && !wasTriggered) {
            sendRequest = true;
            wasTriggered = true;
        } else if (!isTriggered && wasTriggered) {
            if (currentTime - lastRequestTime >= COOLDOWN) {
                wasTriggered = false;
            }
        } else if (isTriggered && wasTriggered && (currentTime - lastRequestTime >= COOLDOWN)) {
            sendRequest = true;
        }
    } else {
        wasTriggered = false;
    }

    if (sendRequest) {
        sendHttpPost();
        lastRequestTime = currentTime;
    }

    delay(100);
}

float getDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    float distance = duration * 0.0343 / 2;

    if (distance > 400 || distance <= 0) {
        return -1;
    }
    return distance;
}

void sendHttpPost() {
    if (WiFi.status() == WL_CONNECTED) {
        if (wifiClient.connect("192.168.1.100", 32768)) {
            wifiClient.println("POST /trigger HTTP/1.1");
            wifiClient.println("Host: 192.168.1.100:32768");
            wifiClient.println("User-Agent: Arduino UNO R4 WiFi");
            wifiClient.println("Connection: close");
            wifiClient.println("Content-Length: 0");
            wifiClient.println();

            unsigned long timeout = millis();
            while (wifiClient.available() == 0) {
                if (millis() - timeout > 5000) {
                    Serial.println(">>> Client Timeout!");
                    wifiClient.stop();
                    return;
                }
            }

            while (wifiClient.available()) {
                String line = wifiClient.readStringUntil('\r');
                Serial.print(line);
            }
            Serial.println();
            wifiClient.stop();
        } else {
            Serial.println("Connection to server failed");
        }
    } else {
        Serial.println("WiFi Disconnected");
    }
}
