#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

// WiFi credentials
const char* wifiSSID = "AndroidAP";
const char* wifiPassword = "jlcz7431";

// IFTTT Webhooks details
const char* iftttServer = "ifttt.com";
const String webhookKey = "fw-ZqwXpLE65ACDkBXsaRPTZ-dZcKBAhweRjLfT0MrE";

WiFiSSLClient secureClient;
HttpClient httpClient(secureClient, iftttServer, 443);

const int sensorPin = A0;       // Analog pin connected to the light sensor
const int lightThreshold = 250; // Threshold value for detecting sunlight

bool isBright = false;          // Keeps track of the last known light state

void triggerIFTTTEvent(const String& eventName) {
    String endpoint = "/trigger/" + eventName + "/with/key/" + webhookKey;

    Serial.print("Triggering event: ");
    Serial.println(endpoint);

    httpClient.beginRequest();
    httpClient.get(endpoint);
    httpClient.sendHeader("Host", iftttServer);
    httpClient.sendHeader("Connection", "close");
    httpClient.endRequest();

    Serial.println("IFTTT response:");
    while (httpClient.available()) {
        char ch = httpClient.read();
        Serial.print(ch);
    }

    Serial.println("\nEvent sent successfully.");
}

void connectToWiFi() {
    Serial.print("Connecting to WiFi network: ");
    Serial.println(wifiSSID);

    WiFi.begin(wifiSSID, wifiPassword);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nWiFi connection established!");
}

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial monitor

    connectToWiFi();
}

void loop() {
    int currentLight = analogRead(sensorPin);
    Serial.print("Current Light Reading: ");
    Serial.println(currentLight);

    if (currentLight > lightThreshold && !isBright) {
        Serial.println("Bright light detected. Triggering IFTTT event...");
        triggerIFTTTEvent("sunlight_detected");
        isBright = true;
    }
    else if (currentLight <= lightThreshold && isBright) {
        Serial.println("Light level dropped. Triggering IFTTT event...");
        triggerIFTTTEvent("sunlight_stopped");
        isBright = false;
    }

    delay(5000); // Wait before next reading
}