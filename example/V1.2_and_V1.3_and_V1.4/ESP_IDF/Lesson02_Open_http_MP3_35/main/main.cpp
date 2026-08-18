#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"

const char *ssid      = "yanfa1";
const char *password  = "1223334444yanfa";

const char *audioUrl = "http://music.163.com/song/media/outer/url?id=2086327879.mp3";

/*---------------------------------------------------------------
 * I2S audio pin map
 * The same sketch structure is used across the display sizes, so the
 * board-specific pins stay near the top of the file.
 *--------------------------------------------------------------*/
#define BCLK  13
#define LRC   11
#define DOUT  12

// MP3 decoding and playback engine.
Audio audio;

/**
 * @brief Connect the board to the Wi-Fi network used for streaming.
 *
 * The lesson waits until the station mode link is ready before the
 * audio library starts the HTTP MP3 stream.
 *
 * @param None.
 * @return None.
 */
void connent_wifi()
{
  Serial.printf("connect to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

/**
 * @brief Hold the amplifier control pin in the active state.
 *
 * Some boards need the GPIO to be driven low before audio output is
 * enabled, otherwise the amplifier remains muted.
 *
 * @param None.
 * @return None.
 */
void pull_gpios_low()
{
  const int GPIO_OUTPUT_IO = 21;
  pinMode(GPIO_OUTPUT_IO, OUTPUT);
  digitalWrite(GPIO_OUTPUT_IO, LOW);
}

/**
 * @brief Configure the audio pinout and start playback from the URL.
 *
 * The audio library handles the MP3 decoder and I2S output once the
 * stream is reachable.
 *
 * @param None.
 * @return None.
 */
void play_http_mp3()
{
  audio.setPinout(BCLK,LRC,DOUT);
  audio.setVolume(21);
  audio.connecttohost(audioUrl);
  Serial.println("Playing audio from URL: " + String(audioUrl));
}

/**
 * @brief Initialize serial, Wi-Fi, power control, and MP3 playback.
 *
 * The setup sequence is deliberately linear so students can match each
 * serial message with a visible stage of the startup process.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime through app_main().
 */
void setup()
{
  Serial.begin(115200);
  connent_wifi();

  pull_gpios_low();

  play_http_mp3();
}

/**
 * @brief Keep the audio decoder fed with data from the network stream.
 *
 * The audio library must be serviced frequently for uninterrupted
 * playback.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by app_main().
 */
void loop()
{
  audio.loop();
  delay(10);
}

extern "C" void app_main(void)
{
    initArduino();

    setup();

    while (true)
    {
        loop();
        delay(1);
    }
}
