#include <Arduino.h>
#include "ESP_I2S.h"

// ======================= I2S Pin Definitions =======================
// Microphone
#define I2S_MIC_DATA   10  // PDM data pin
#define I2S_MIC_CLK    9   // PDM clock pin
// Speaker
#define I2S_SPK_DOUT   12  // Speaker data output
#define I2S_SPK_LRC    11  // Channel clock (WS/LRC)
#define I2S_SPK_BCLK   13  // Bit clock (BCLK)
// Control
#define MUTE_PIN       21  // Speaker mute control
#define CTRL_PIN       45  // Microphone signal control

// ======================= Audio Parameters =======================
#define SAMPLE_RATE    16000
#define RECORD_TIME    5       // Recording duration (seconds)
#define AMPLIFY_GAIN   5.0f    // Amplification factor

// ======================= Global Variables =======================
I2SClass i2s;  // Global I2S instance

// ======================= Function Declarations =======================
void setupPins();
uint8_t* recordAudio(size_t *wav_size);
void playAudio(uint8_t *wav_buffer, size_t wav_size);

// ======================= Main Process =======================
void setup() {
  Serial.begin(115200);
  setupPins();
  Serial.println("System initialized, starting recording...");
  
  size_t wav_size = 0;
  uint8_t *wav_buffer = recordAudio(&wav_size);
  
  if (wav_buffer != nullptr) {
    Serial.println("Recording complete, starting playback...");
    playAudio(wav_buffer, wav_size);
    free(wav_buffer);
  }

  Serial.println("Recording and playback process completed.");
}

void loop() {
  // Empty loop, run only once
}

// ======================= Initialize Pins =======================
void setupPins() {
  pinMode(MUTE_PIN, OUTPUT);
  pinMode(CTRL_PIN, OUTPUT);

  digitalWrite(MUTE_PIN, HIGH);  // Mute speaker
  digitalWrite(CTRL_PIN, HIGH);
}

// ======================= Recording Function =======================
uint8_t* recordAudio(size_t *wav_size) {
  // Initialize microphone input
  i2s.setPinsPdmRx(I2S_MIC_CLK, I2S_MIC_DATA);
  if (!i2s.begin(I2S_MODE_PDM_RX, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("❌ I2S microphone initialization failed!");
    return nullptr;
  }

  delay(200);
  Serial.println("🎙️ Recording started...");
  uint8_t *wav_buffer = i2s.recordWAV(RECORD_TIME, wav_size);
  i2s.end();
  Serial.println("✅ Recording finished.");

  return wav_buffer;
}

// ======================= Playback Function =======================
void playAudio(uint8_t *wav_buffer, size_t wav_size) {
  if (wav_buffer == nullptr || wav_size <= 44) {
    Serial.println("❌ Invalid audio data!");
    return;
  }

  // Skip the WAV file header
  uint8_t *audio_data = wav_buffer + 44;
  size_t audio_size = wav_size - 44;

  // Initialize speaker output
  i2s.setPins(I2S_SPK_BCLK, I2S_SPK_LRC, I2S_SPK_DOUT);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("❌ I2S speaker initialization failed!");
    return;
  }

  delay(200);
  digitalWrite(MUTE_PIN, LOW); // Unmute speaker

  // Allocate stereo buffer (left channel muted, right channel plays audio)
  uint8_t *stereo_buffer = (uint8_t *)heap_caps_malloc(audio_size * 2, MALLOC_CAP_SPIRAM);
  if (stereo_buffer == nullptr) {
    Serial.println("❌ Memory allocation failed!");
    i2s.end();
    return;
  }

  // Convert mono to stereo + amplify
  for (size_t i = 0, j = 0; i < audio_size; i += 2, j += 4) {
    stereo_buffer[j] = 0;     // Left channel silent
    stereo_buffer[j + 1] = 0;

    int16_t sample = (int16_t)((audio_data[i + 1] << 8) | audio_data[i]);
    float amplified = sample * AMPLIFY_GAIN;

    if (amplified > 32767) amplified = 32767;
    if (amplified < -32768) amplified = -32768;

    int16_t new_sample = (int16_t)amplified;
    stereo_buffer[j + 2] = new_sample & 0xFF;
    stereo_buffer[j + 3] = (new_sample >> 8) & 0xFF;
  }

  // Play the audio
  Serial.println("🔊 Playing audio...");
  i2s.write(stereo_buffer, audio_size * 2);

  // Clean up resources
  heap_caps_free(stereo_buffer);
  i2s.end();
  digitalWrite(MUTE_PIN, HIGH); // Mute speaker

  Serial.println("✅ Playback finished.");
}
