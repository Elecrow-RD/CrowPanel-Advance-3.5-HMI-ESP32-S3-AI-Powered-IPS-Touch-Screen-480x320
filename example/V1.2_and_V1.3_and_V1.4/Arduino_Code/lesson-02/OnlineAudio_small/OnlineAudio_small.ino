#include <Wire.h>
#include "Arduino.h"
#include "WiFiMulti.h"
#include "Audio.h"

/*---------------------------------------------------------------
 * I2S audio pin map
 * The built-in amplifier receives decoded audio through these
 * three I2S signals.
 *--------------------------------------------------------------*/
#define I2S_DOUT  12
#define I2S_BCLK  13
#define I2S_LRC   11

// Handles MP3 decoding, buffering, and I2S output.
Audio audio;

// Tries the configured access point and maintains the station link.
WiFiMulti wifiMulti;

// Wi-Fi credentials used by this lesson. Replace them before publishing.
String ssid     = "elecrow888";
String password = "elecrow2014";

/**
 * @brief Connect Wi-Fi and start online MP3 playback.
 *
 * The amplifier enable pin is prepared before playback, then the
 * ESP32 joins the network and streams an MP3 URL through the I2S
 * audio library.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup() {
  Serial.begin(115200);

  /*---------------------------------------------------------------
   * Enable the audio amplifier path
   * GPIO21 controls the amplifier shutdown path on this board. The
   * initial HIGH level keeps the circuit in a known state while Wi-Fi
   * is connecting.
   *--------------------------------------------------------------*/
  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);

  delay(50);
  Serial.printf("[LINE--%d]\n", __LINE__);

  /*---------------------------------------------------------------
   * Join the wireless network
   * Station mode lets the board connect to an existing router. If the
   * first attempt fails, the code resets the Wi-Fi state and retries.
   *--------------------------------------------------------------*/
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(ssid.c_str(), password.c_str());
  wifiMulti.run();
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect(true);
    wifiMulti.run();
  }

  Serial.printf("[LINE--%d]\n", __LINE__);
  Serial.println("----- WIFI_CONNECTED -----");

  /*---------------------------------------------------------------
   * Start the audio stream
   * The library decodes the remote MP3 stream and sends samples to
   * the amplifier through the configured I2S pins.
   *--------------------------------------------------------------*/
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(20);
  digitalWrite(21, LOW);
  
  audio.connecttohost("http://music.163.com/song/media/outer/url?id=2086327879.mp3"); // flower

  // audio.connecttohost("http://music.163.com/song/media/outer/url?id=5103312.mp3"); // Empire state of mine.mp3
  Serial.printf("[LINE--%d]\t ready to play!!\n", __LINE__);
}

/**
 * @brief Keep audio playback running and accept a new stream URL.
 *
 * The audio library must be serviced frequently. When a URL is typed
 * into the serial monitor, playback stops and the library attempts to
 * connect to the new address.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by the Arduino runtime.
 */
void loop() {
  audio.loop();

  if (Serial.available()) {
    audio.stopSong();
    String r = Serial.readString();

    // Trim whitespace so the length check and URL parser use only the
    // meaningful characters typed by the student.
    r.trim();
    if (r.length() > 5) audio.connecttohost(r.c_str()); // Try connecting to the next song URL
    log_i("free heap=%i", ESP.getFreeHeap());
  }
}
