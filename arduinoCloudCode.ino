#include "thingProperties.h"
#include "DHT.h"
#include <WiFiNINA.h>
#include "ThingSpeak.h"

// DHT Sensor Settings
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// WiFi credentials (used for ThingSpeak only if ArduinoCloud doesn't expose them)
const char* ssid = SSID;
const char* password = PASS;

// ThingSpeak settings
WiFiClient client;
unsigned long myChannelNumber = 3025006;          // Replace with your channel ID
const char* myWriteAPIKey = "93GTPO5XAQD5JUVE";  // Replace with your API key

// Timer
unsigned long lastReadTime = 0;
const unsigned long interval = 30000; // 30 seconds

void setup() {
  Serial.begin(9600);
  delay(1500);

  dht.begin();

  // Connect to Wi-Fi for ThingSpeak
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected to ThingSpeak");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Initialize Arduino IoT Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();

  unsigned long currentMillis = millis();

  if (currentMillis - lastReadTime >= interval) {
    lastReadTime = currentMillis;

    float t = dht.readTemperature();

    if (isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      temp = t;  // Update Arduino IoT Cloud variable

      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.println(" C");

      // Send data to ThingSpeak
      ThingSpeak.setField(1, temp); // Send to Field 1
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      if (x == 200) {
        Serial.println("ThingSpeak update successful.");
      } else {
        Serial.print("ThingSpeak error: ");
        Serial.println(x);
      }
    }
  }
}

void onTempChange() {
  // Optional: Actions when temp is changed from dashboard
}
