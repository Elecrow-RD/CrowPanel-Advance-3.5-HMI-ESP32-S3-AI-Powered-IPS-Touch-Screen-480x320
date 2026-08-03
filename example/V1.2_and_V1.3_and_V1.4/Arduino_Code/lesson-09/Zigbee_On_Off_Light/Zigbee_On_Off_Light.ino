// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @brief Zigbee end-device sketch for a controllable lamp.
 *
 * The board joins a Zigbee network as a light bulb and waits for the
 * coordinator to change its state. A local button can also trigger a
 * factory reset and toggle the lamp.
 */

#include <Arduino.h>
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/*---------------------------------------------------------------
 * Zigbee light bulb configuration
 * The endpoint number must match the coordinator-side binding logic.
 *--------------------------------------------------------------*/
#define ZIGBEE_LIGHT_ENDPOINT 10
uint8_t led = RGB_BUILTIN;
uint8_t button = BOOT_PIN;

// Zigbee light endpoint exposed to the network.
ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);

/*---------------------------------------------------------------
 * RGB LED output
 * The Zigbee stack calls this function whenever the remote switch
 * changes the lamp state.
 *--------------------------------------------------------------*/
void setLED(bool value) {
  digitalWrite(led, value);
}

/*---------------------------------------------------------------
 * Arduino entry points
 * The setup routine joins Zigbee first; loop() handles the local
 * factory-reset button and light toggling.
 *--------------------------------------------------------------*/
void setup() {
  Serial.begin(115200);

  // Start with the lamp off so the effect of remote control is obvious.
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  // The local button supports factory reset and a direct toggle action.
  pinMode(button, INPUT_PULLUP);

  // Give the bulb a readable device identity for serial output.
  zbLight.setManufacturerAndModel("Espressif", "ZBLightBulb");

  // This callback links Zigbee state changes to the GPIO output.
  zbLight.onLightChange(setLED);

  Serial.println("Adding ZigbeeLight endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbLight);

  // Start Zigbee as a light bulb end device.
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
}

void loop() {
  // The local button is used for factory reset and quick lamp toggling.
  if (digitalRead(button) == LOW) {
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // Holding the button long enough restores the network settings.
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    zbLight.setLight(!zbLight.getLightState());
  }
  delay(100);
}
