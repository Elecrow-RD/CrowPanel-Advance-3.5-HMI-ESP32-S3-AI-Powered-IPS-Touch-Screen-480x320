#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// WiFi credentials, must be in the same subnet as the host
const char* ssid     = "elecrow888";
const char* password = "elecrow2014";
// LED pin
const int LED_PIN = 19; // 7.0 5.0 4.3 inch ---LED pin
// const int LED_PIN = 18; // 3.5 2.8 2.4 inch ---LED pin

// Create a WebServer listening on port 80
WebServer server(80);

void handleOn() {
  digitalWrite(LED_PIN, HIGH);
  server.send(200, "text/plain", "LED ON");
  Serial.println("Light turned ON!");
}

void handleOff() {
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "LED OFF");
  Serial.println("Light turned OFF!");
}

// Independent task: continuously handle client requests
void handleClientTask(void* pvParameters) {
  for (;;) {
    server.handleClient();
    // Delay 1 FreeRTOS tick, about 1 ms (default configTICK_RATE_HZ=1000)
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // Set routes
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);

  server.begin();
  Serial.println("HTTP server started");

  // Create a task to handle network requests, stack size 4096 bytes, priority 1
  xTaskCreate(handleClientTask, "HandleClient", 4096, nullptr, 1, nullptr);
}

void loop() {

}
