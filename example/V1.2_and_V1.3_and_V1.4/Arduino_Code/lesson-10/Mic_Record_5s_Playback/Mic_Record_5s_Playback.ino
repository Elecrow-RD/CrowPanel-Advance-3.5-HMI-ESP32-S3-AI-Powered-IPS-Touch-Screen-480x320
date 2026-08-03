#include <Arduino.h>
#include <ESP_I2S.h>
#include <esp_heap_caps.h>

/*---------------------------------------------------------------
 * CrowPanel Advance 3.5-inch audio pin map
 * The microphone and wireless module share a board-level signal
 * path. GPIO45 must remain HIGH while the microphone is in use.
 *--------------------------------------------------------------*/
constexpr int MIC_CLK = 9;
constexpr int MIC_DATA = 10;
constexpr int MIC_SELECT = 45;

constexpr int SPK_BCLK = 13;
constexpr int SPK_LRCLK = 11;
constexpr int SPK_DATA = 12;
constexpr int AMP_CTRL = 21;

/*---------------------------------------------------------------
 * Recording and playback settings
 * Five seconds of 16 kHz, 16-bit mono audio uses about 160 KB.
 * Playback uses a stereo buffer because the amplifier receives a
 * standard two-channel I2S stream.
 *--------------------------------------------------------------*/
constexpr uint32_t SAMPLE_RATE = 16000;
constexpr uint32_t RECORD_SECONDS = 5;
constexpr float PLAYBACK_GAIN = 2.0f;
constexpr size_t WAV_HEADER_SIZE = 44;
constexpr int16_t NOISE_GATE_LEVEL = 220;
constexpr size_t FADE_SAMPLE_COUNT = 320;

// Owns the I2S receive and transmit channels used by this lesson.
I2SClass audio;

struct AudioStats {
  uint16_t peak;
  uint16_t averageAbs;
  uint32_t clippedSamples;
};

/**
 * @brief Enable or mute the on-board audio amplifier.
 *
 * GPIO21 is active LOW on the 3.5-inch board. Muting the amplifier
 * during recording prevents the speaker path from adding noise.
 *
 * @param enabled true to enable playback, false to mute it.
 * @return None.
 * @note Called before recording and immediately around playback.
 */
void setAmplifierEnabled(bool enabled) {
  digitalWrite(AMP_CTRL, enabled ? LOW : HIGH);
}

/**
 * @brief Increase one PCM sample without allowing integer overflow.
 *
 * Samples that exceed the signed 16-bit range are clipped. Reduce
 * PLAYBACK_GAIN if the recording sounds distorted.
 *
 * @param sample Signed 16-bit PCM sample from the microphone.
 * @return Amplified signed 16-bit PCM sample.
 * @note Called while preparing the stereo playback buffer.
 */
int16_t amplify(int16_t sample) {
  int32_t value = static_cast<int32_t>(sample * PLAYBACK_GAIN);
  if (value > INT16_MAX) value = INT16_MAX;
  if (value < INT16_MIN) value = INT16_MIN;
  return static_cast<int16_t>(value);
}

/**
 * @brief Remove microphone DC offset and collect signal statistics.
 *
 * I2S MEMS microphones may carry a small DC bias. Removing the mean
 * value keeps the speaker centered around zero and reduces hiss or
 * low-frequency noise after amplification.
 *
 * @param samples Mono PCM sample buffer after the WAV header.
 * @param sampleCount Number of 16-bit samples in the buffer.
 * @return Peak, average absolute level, and clipping count.
 * @note Called once after each recording completes.
 */
AudioStats normalizeRecording(int16_t *samples, size_t sampleCount) {
  int64_t sum = 0;
  for (size_t i = 0; i < sampleCount; ++i) {
    sum += samples[i];
  }

  int32_t dcOffset = sampleCount > 0 ? static_cast<int32_t>(sum / sampleCount) : 0;
  uint64_t absSum = 0;
  uint16_t peak = 0;
  uint32_t clippedSamples = 0;

  for (size_t i = 0; i < sampleCount; ++i) {
    int32_t corrected = static_cast<int32_t>(samples[i]) - dcOffset;
    if (corrected > INT16_MAX) corrected = INT16_MAX;
    if (corrected < INT16_MIN) corrected = INT16_MIN;

    int16_t sample = static_cast<int16_t>(corrected);
    int32_t magnitude = abs(static_cast<int32_t>(sample));
    if (magnitude < NOISE_GATE_LEVEL) {
      sample = 0;
      magnitude = 0;
    }
    if (magnitude > static_cast<int32_t>(peak)) {
      peak = static_cast<uint16_t>(magnitude);
    }
    if (magnitude > INT16_MAX - 16) {
      ++clippedSamples;
    }

    samples[i] = sample;
    absSum += magnitude;
  }

  AudioStats stats;
  stats.peak = peak;
  stats.averageAbs = sampleCount > 0 ? static_cast<uint16_t>(absSum / sampleCount) : 0;
  stats.clippedSamples = clippedSamples;
  return stats;
}

/**
 * @brief Apply a short fade-in and fade-out to a PCM stream.
 *
 * The amplifier is enabled only during playback, so the first and last
 * samples can click if they start far from zero. A short ramp makes the
 * transition quieter without changing the recorded content.
 *
 * @param samples Mono PCM sample buffer after normalization.
 * @param sampleCount Number of 16-bit samples in the buffer.
 * @return None.
 * @note Called before mono samples are copied to the speaker frame.
 */
void applyFade(int16_t *samples, size_t sampleCount) {
  size_t fadeCount = min(FADE_SAMPLE_COUNT, sampleCount / 2);
  for (size_t i = 0; i < fadeCount; ++i) {
    samples[i] = static_cast<int16_t>(
        (static_cast<int32_t>(samples[i]) * static_cast<int32_t>(i)) /
        static_cast<int32_t>(fadeCount));

    size_t tail = sampleCount - 1 - i;
    samples[tail] = static_cast<int16_t>(
        (static_cast<int32_t>(samples[tail]) * static_cast<int32_t>(i)) /
        static_cast<int32_t>(fadeCount));
  }
}

/**
 * @brief Record five seconds from the microphone and play it back.
 *
 * The function first configures standard I2S input for the on-board
 * microphone. It then removes the WAV header, converts mono samples
 * to stereo, and reconfigures I2S for the on-board amplifier.
 *
 * @param None.
 * @return true when the entire playback buffer is written.
 * @return false when I2S setup, recording, or allocation fails.
 * @note Called once after reset and whenever the serial command is r.
 */
bool recordAndPlay() {
  size_t wavSize = 0;

  /*---------------------------------------------------------------
   * Record from the on-board I2S microphone
   * GPIO45 selects the microphone instead of the wireless module.
   * The amplifier remains muted so the recording captures only the
   * intended sound near the board.
   *--------------------------------------------------------------*/
  setAmplifierEnabled(false);
  digitalWrite(MIC_SELECT, HIGH);

  audio.setPinsPdmRx(MIC_CLK, MIC_DATA);
  if (!audio.begin(I2S_MODE_PDM_RX, SAMPLE_RATE,
                   I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("ERROR: PDM microphone initialization failed.");
    return false;
  }

  Serial.println("Recording for 5 seconds...");
  uint8_t *wav = audio.recordWAV(RECORD_SECONDS, &wavSize);
  audio.end();

  if (wav == nullptr || wavSize <= WAV_HEADER_SIZE) {
    Serial.println("ERROR: Recording failed (check PSRAM and microphone pins).");
    free(wav);
    return false;
  }
  Serial.printf("Recording complete: %u bytes\n", static_cast<unsigned>(wavSize));

  /*---------------------------------------------------------------
   * Convert mono microphone samples to speaker playback samples
   * recordWAV() places a standard 44-byte header before the PCM
   * payload. The 3.5-inch V1.2/V1.3/V1.4 microphone uses a two-pin
   * PDM path on GPIO9 and GPIO10, so the payload is already mono.
   *--------------------------------------------------------------*/
  uint8_t *monoBytes = wav + WAV_HEADER_SIZE;
  size_t monoSize = wavSize - WAV_HEADER_SIZE;
  size_t sampleCount = monoSize / sizeof(int16_t);
  size_t stereoSize = sampleCount * 2 * sizeof(int16_t);

  int16_t *stereo = static_cast<int16_t *>(
      heap_caps_malloc(stereoSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  if (stereo == nullptr) {
    // Internal RAM provides a fallback when PSRAM is not exposed as a heap.
    stereo = static_cast<int16_t *>(malloc(stereoSize));
  }
  if (stereo == nullptr) {
    Serial.println("ERROR: Playback buffer allocation failed.");
    free(wav);
    return false;
  }

  int16_t *mono = reinterpret_cast<int16_t *>(monoBytes);
  AudioStats stats = normalizeRecording(mono, sampleCount);
  applyFade(mono, sampleCount);
  Serial.printf("Audio level: mic_mode=PDM, peak=%u, avg=%u, clipped=%u\n",
                stats.peak, stats.averageAbs,
                static_cast<unsigned>(stats.clippedSamples));
  if (stats.peak < 600) {
    Serial.println("WARNING: Recorded level is low; speak closer to the microphone.");
  }

  for (size_t i = 0; i < sampleCount; ++i) {
    int16_t sample = amplify(mono[i]);
    stereo[i * 2] = 0;
    stereo[i * 2 + 1] = sample;
  }

  /*---------------------------------------------------------------
   * Play through the on-board I2S amplifier
   * The I2S peripheral is restarted because recording and playback
   * use different pins and channel layouts.
   *--------------------------------------------------------------*/
  audio.setPins(SPK_BCLK, SPK_LRCLK, SPK_DATA);
  if (!audio.begin(I2S_MODE_STD, SAMPLE_RATE,
                   I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("ERROR: I2S speaker initialization failed.");
    free(stereo);
    free(wav);
    return false;
  }

  Serial.println("Playing the recording...");
  setAmplifierEnabled(true);
  size_t bytesWritten = audio.write(reinterpret_cast<uint8_t *>(stereo), stereoSize);
  delay(20);
  setAmplifierEnabled(false);
  audio.end();

  free(stereo);
  free(wav);

  Serial.printf("Playback complete: %u/%u bytes written.\n",
                static_cast<unsigned>(bytesWritten),
                static_cast<unsigned>(stereoSize));
  return bytesWritten == stereoSize;
}

/**
 * @brief Prepare the board and run the first recording cycle.
 *
 * The microphone path is selected before I2S starts, while the
 * active-LOW amplifier is muted until playback begins.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(MIC_SELECT, OUTPUT);
  digitalWrite(MIC_SELECT, HIGH);

  pinMode(AMP_CTRL, OUTPUT);
  setAmplifierEnabled(false);

  Serial.println("\nCrowPanel Advance 3.5-inch microphone record/playback demo");
  Serial.println("Send r in Serial Monitor to run it again.");
  if (!psramFound()) {
    Serial.println("WARNING: PSRAM was not detected; recording may fail.");
  }

  recordAndPlay();
}

/**
 * @brief Wait for a serial command that starts another recording.
 *
 * Both lowercase and uppercase r are accepted. Any remaining serial
 * characters are discarded so one command starts only one cycle.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by the Arduino runtime after setup().
 */
void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'r' || command == 'R') {
      while (Serial.available()) Serial.read();
      recordAndPlay();
    }
  }
  delay(10);
}
