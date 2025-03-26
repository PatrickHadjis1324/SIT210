#include "DHT.h"
#include <WiFiNINA.h>
#include "ThingSpeak.h"
#include "secrets.h" // Contains WiFi and ThingSpeak credentials

#define DHTPIN 2         // Pin where the DHT sensor is connected
#define DHTTYPE DHT11    // Change to DHT22 if you're using DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  dht.begin();

  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  ThingSpeak.begin(client);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Default is Celsius

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C\tHumidity: ");
  Serial.print(h);
  Serial.println(" %");

  // Write to ThingSpeak fields
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

  // Send data to ThingSpeak
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.print("Problem updating channel. HTTP error code: ");
    Serial.println(x);
  }

  delay(60000); // Wait 60 seconds
}
