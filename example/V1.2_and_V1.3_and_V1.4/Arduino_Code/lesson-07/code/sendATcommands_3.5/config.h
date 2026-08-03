#ifndef _RADIOLIB_EX_LORAWAN_CONFIG_H
#define _RADIOLIB_EX_LORAWAN_CONFIG_H

#include <RadioLib.h>
#include <SPI.h>

/*---------------------------------------------------------------
 * Board and radio configuration
 * The lesson centralizes pin mapping here so the command parser and
 * radio setup share the same hardware assumptions.
 *--------------------------------------------------------------*/
#define HSPI_MISO  9
#define HSPI_MOSI  3
#define HSPI_SCLK  10
#define HSPI_SS    0
// SX1262 wiring used by this lesson board.
SX1262 radio = new Module(0, 1, 2, 46, SPI);

// Regional presets supported by the lesson command set.
const LoRaWANBand_t Region_915 = US915;
const LoRaWANBand_t Region_868 = EU868;
const uint8_t subBand = 1;

// LoRaWAN node object used by the AT command handlers.
LoRaWANNode node(&radio, &Region_868, subBand);

/**
 * @brief Convert a RadioLib result code to human-readable text.
 *
 * @param result RadioLib status code.
 * @return Short string that describes the code.
 */
String stateDecode(const int16_t result) {
  switch (result) {
    case RADIOLIB_ERR_NONE:
      return "ERR_NONE";
    case RADIOLIB_ERR_CHIP_NOT_FOUND:
      return "ERR_CHIP_NOT_FOUND";
    case RADIOLIB_ERR_PACKET_TOO_LONG:
      return "ERR_PACKET_TOO_LONG";
    case RADIOLIB_ERR_RX_TIMEOUT:
      return "ERR_RX_TIMEOUT";
    case RADIOLIB_ERR_CRC_MISMATCH:
      return "ERR_CRC_MISMATCH";
    case RADIOLIB_ERR_INVALID_BANDWIDTH:
      return "ERR_INVALID_BANDWIDTH";
    case RADIOLIB_ERR_INVALID_SPREADING_FACTOR:
      return "ERR_INVALID_SPREADING_FACTOR";
    case RADIOLIB_ERR_INVALID_CODING_RATE:
      return "ERR_INVALID_CODING_RATE";
    case RADIOLIB_ERR_INVALID_FREQUENCY:
      return "ERR_INVALID_FREQUENCY";
    case RADIOLIB_ERR_INVALID_OUTPUT_POWER:
      return "ERR_INVALID_OUTPUT_POWER";
    case RADIOLIB_ERR_NETWORK_NOT_JOINED:
      return "RADIOLIB_ERR_NETWORK_NOT_JOINED";
    case RADIOLIB_ERR_DOWNLINK_MALFORMED:
      return "RADIOLIB_ERR_DOWNLINK_MALFORMED";
    case RADIOLIB_ERR_INVALID_REVISION:
      return "RADIOLIB_ERR_INVALID_REVISION";
    case RADIOLIB_ERR_INVALID_PORT:
      return "RADIOLIB_ERR_INVALID_PORT";
    case RADIOLIB_ERR_NO_RX_WINDOW:
      return "RADIOLIB_ERR_NO_RX_WINDOW";
    case RADIOLIB_ERR_INVALID_CID:
      return "RADIOLIB_ERR_INVALID_CID";
    case RADIOLIB_ERR_UPLINK_UNAVAILABLE:
      return "RADIOLIB_ERR_UPLINK_UNAVAILABLE";
    case RADIOLIB_ERR_COMMAND_QUEUE_FULL:
      return "RADIOLIB_ERR_COMMAND_QUEUE_FULL";
    case RADIOLIB_ERR_COMMAND_QUEUE_ITEM_NOT_FOUND:
      return "RADIOLIB_ERR_COMMAND_QUEUE_ITEM_NOT_FOUND";
    case RADIOLIB_ERR_JOIN_NONCE_INVALID:
      return "RADIOLIB_ERR_JOIN_NONCE_INVALID";
#ifdef RADIOLIB_ERR_N_FCNT_DOWN_INVALID
    case RADIOLIB_ERR_N_FCNT_DOWN_INVALID:
      return "RADIOLIB_ERR_N_FCNT_DOWN_INVALID";
#endif
#ifdef RADIOLIB_ERR_A_FCNT_DOWN_INVALID
    case RADIOLIB_ERR_A_FCNT_DOWN_INVALID:
      return "RADIOLIB_ERR_A_FCNT_DOWN_INVALID";
#endif
    case RADIOLIB_ERR_DWELL_TIME_EXCEEDED:
      return "RADIOLIB_ERR_DWELL_TIME_EXCEEDED";
    case RADIOLIB_ERR_CHECKSUM_MISMATCH:
      return "RADIOLIB_ERR_CHECKSUM_MISMATCH";
    case RADIOLIB_ERR_NO_JOIN_ACCEPT:
      return "RADIOLIB_ERR_NO_JOIN_ACCEPT";
    case RADIOLIB_LORAWAN_SESSION_RESTORED:
      return "RADIOLIB_LORAWAN_SESSION_RESTORED";
    case RADIOLIB_LORAWAN_NEW_SESSION:
      return "RADIOLIB_LORAWAN_NEW_SESSION";
    case RADIOLIB_ERR_NONCES_DISCARDED:
      return "RADIOLIB_ERR_NONCES_DISCARDED";
    case RADIOLIB_ERR_SESSION_DISCARDED:
      return "RADIOLIB_ERR_SESSION_DISCARDED";
  }
  return "See https://jgromes.github.io/RadioLib/group__status__codes.html";
}

/**
 * @brief Print an error message and optionally halt the program.
 *
 * @param failed True when the caller detected a failure.
 * @param message Status message to print.
 * @param state RadioLib result code associated with the failure.
 * @param halt True to remain in the error loop.
 * @return None.
 */
void debug(bool failed, const __FlashStringHelper* message, int state, bool halt) {
  if (failed) {
    Serial.print(message);
    Serial.print(" - ");
    Serial.print(stateDecode(state));
    Serial.print(" (");
    Serial.print(state);
    Serial.println(")");
    while (halt) {
      delay(1);
    }
  }
}

/**
 * @brief Print a byte array as hexadecimal pairs.
 *
 * @param buffer Byte array to print.
 * @param len Number of bytes in the buffer.
 * @return None.
 */
void arrayDump(uint8_t *buffer, uint16_t len) {
  for (uint16_t c = 0; c < len; c++) {
    char b = buffer[c];
    if (b < 0x10) {
      Serial.print('0');
    }
    Serial.print(b, HEX);
  }
  Serial.println();
}

#endif
