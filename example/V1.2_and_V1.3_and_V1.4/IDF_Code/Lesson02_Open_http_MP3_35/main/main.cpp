#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"

const char *ssid      = "yanfa1";
const char *password  = "1223334444yanfa";

const char *audioUrl = "http://music.163.com/song/media/outer/url?id=2086327879.mp3";

//2.4 2.8 3.5 pins init
#define BCLK  13
#define LRC   11
#define DOUT  12

Audio audio;

//--------methond-----------------------------------------------------------------
void connent_wifi()
{
  // Connect to WiFi
  Serial.printf("connect to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

// Small size operation necessary to pull down the GPIO 21 pin functions
void pull_gpios_low()
{
  // Define the GPIO pin to operate on
  const int GPIO_OUTPUT_IO = 21;
  // Set the pin to output mode
  pinMode(GPIO_OUTPUT_IO, OUTPUT);
  //Pull down pins of GPIO 21
  digitalWrite(GPIO_OUTPUT_IO, LOW);
}

//play httpmp3
void play_http_mp3()
{
  audio.setPinout(BCLK,LRC,DOUT); // Adjust pins according to your setup
  audio.setVolume(21);            // Set the volume level
  audio.connecttohost(audioUrl);  // Connect to the audio URL and start playback
  Serial.println("Playing audio from URL: " + String(audioUrl));
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Connect to WiFi
  connent_wifi();

  // Small size operation necessary to pull down the GPIO 21 pin functions
  pull_gpios_low();

  //play httpmp3
  play_http_mp3();
}

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
