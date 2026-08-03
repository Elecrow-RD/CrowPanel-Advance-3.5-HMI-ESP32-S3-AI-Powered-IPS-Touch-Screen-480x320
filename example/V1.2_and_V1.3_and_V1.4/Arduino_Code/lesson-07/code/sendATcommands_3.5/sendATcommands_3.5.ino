#include "config.h"
#include <Arduino.h>
#include "At_Config.hpp"

/*---------------------------------------------------------------
 * Command buffers
 * Serial data is copied into a fixed-size buffer so the AT parser can
 * process one complete line at a time.
 *--------------------------------------------------------------*/
uint8_t cmd_data_buf[244] = { 0 };

// Number of bytes currently stored in the command buffer.
uint8_t cmd_data_size = 0;

/**
 * @brief Initialize serial, radio, and the AT parser.
 *
 * The lesson waits for the USB serial port, then starts the radio
 * stack and prints a short banner so students know the firmware is
 * ready for AT commands.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup() {
  Serial.begin(115200);
  pinMode(45, OUTPUT);  
  digitalWrite(45, LOW);
  while (!Serial)
    ;                                                   // Wait for serial to be initialised
  delay(2000);                                          // Give time to switch to the serial monitor
  SPI.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);  //SPI init

  at_config.begin();
  int16_t state = 0;
  state = radio.begin();
  debug(state != RADIOLIB_ERR_NONE, F("Initialise radio failed"), state, false);
  radio.setCurrentLimit(140.0);
  radio.setTCXO(3.3);
  Serial.println(F("\nAT Begin"));
}

/**
 * @brief Read one AT command line and dispatch it to the parser.
 *
 * The loop also applies runtime parameter changes that were requested
 * by the most recent AT command.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by the Arduino runtime.
 */
void loop() {
  if (at_config.receiveSerialCmd(cmd_data_buf, &cmd_data_size)) {
    at_config.parseCmd((char *)cmd_data_buf, cmd_data_size);

    memset(cmd_data_buf, 0, cmd_data_size);
    cmd_data_size = 0;
  }
  ParameterSetting();
}

/**
 * @brief Apply the runtime flags raised by AT commands.
 *
 * Each flag corresponds to one user action such as joining the
 * network, sending a packet, or changing region and data rate.
 *
 * @param None.
 * @return None.
 */
void ParameterSetting() {
  if (ABP_Join_Flag == 1) {
    ABP_Join_Flag = 0;
    Serial.println("");
    Serial.println("Join ABP");
    int16_t state = 0;
    uint8_t joinDR = 0;

    node.beginABP(app_param.lora_info.DevAddr, NULL, NULL, app_param.lora_info.NwkSKey, app_param.lora_info.AppSKey);
    node.activateABP(joinDR);
    debug(state != RADIOLIB_ERR_NONE, F("Activate ABP failed"), state, false);
  }
  if (OTAA_Join_Flag == 1) {
    OTAA_Join_Flag = 0;
    Serial.println("");
    Serial.println("Join OTAA");
    int16_t state = 0;

    uint8_t joinDR = 0;
    if (app_param.lora_info.ActiveRegion == 0) {
      joinDR = 2;
    } else {
      joinDR = 0;
    }

    node.beginOTAA(app_param.lora_info.JoinEui, app_param.lora_info.DevEui, NULL, app_param.lora_info.AppKey);
    Serial.println("Join ('login') the LoRaWAN Network");
    state = node.activateOTAA(joinDR);
    debug(state != RADIOLIB_LORAWAN_NEW_SESSION, F("Join failed"), state, false);
    Serial.print("[LoRaWAN] DevAddr: ");
    Serial.println((unsigned long)node.getDevAddr(), HEX);
    if (app_param.lora_info.ADR == 1) {
      node.setADR(true);
    } else {
      node.setADR(false);
    }
    node.setDatarate(app_param.lora_info.DR);
    if (app_param.lora_info.dutyCycleEnabled == 1) {
      node.setDutyCycle(true, app_param.lora_info.msPerHour);
    } else {
      node.setDutyCycle(false, app_param.lora_info.msPerHour);
    }
    node.setDwellTime(true, 400);
  }

  if (Send_Flag == 1) {
    Send_Flag = 0;
    if (app_param.lora_info.ActivationType == 1)  //ABP
    {
      Serial.println("Send ABP OK");
      int state = node.sendReceive(MsgTemp, MsgTemp_size);
      debug(state < RADIOLIB_ERR_NONE, F("Error in sendReceive"), state, false);

      // Check if a downlink was received
      // (state 0 = no downlink, state 1/2 = downlink in window Rx1/Rx2)
      if (state > 0) {
        Serial.println(F("Received a downlink"));
      } else {
        Serial.println(F("No downlink received"));
      }
    }
    if (app_param.lora_info.ActivationType == 2)  //OTAA
    {
      int16_t state = RADIOLIB_ERR_NONE;
      Serial.println("Send OTAA OK");
      uint8_t downlinkPayload[10];
      size_t downlinkSize;
      LoRaWANEvent_t uplinkDetails;
      LoRaWANEvent_t downlinkDetails;
      if (Ack_State == 1) {
        Serial.println(F("and requesting LinkCheck and DeviceTime"));
        node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_LINK_CHECK);
        node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_DEVICE_TIME);

        state = node.sendReceive(MsgTemp, MsgTemp_size, Port_Number, downlinkPayload, &downlinkSize, true, &uplinkDetails, &downlinkDetails);
      } else {

        state = node.sendReceive(MsgTemp, MsgTemp_size, Port_Number, downlinkPayload, &downlinkSize, true, &uplinkDetails, &downlinkDetails);
      }
      debug(state < RADIOLIB_ERR_NONE, F("Error in sendReceive"), state, false);
      if (state > 0) {
        Serial.println(F("Received a downlink"));
        if (downlinkSize > 0) {
          Serial.println(F("Downlink data: "));
          arrayDump(downlinkPayload, downlinkSize);
        } else {
          Serial.println(F("<MAC commands only>"));
        }
      } else {
        Serial.println(F("[LoRaWAN] No downlink received"));
      }
    }
  }
  if (ADR_set_Flag == 1) {
    ADR_set_Flag = 0;
    if (app_param.lora_info.ADR == 1) {
      node.setADR(true);
    } else {
      node.setADR(false);
    }
  }
  if (DR_set_Flag == 1) {
    DR_set_Flag = 0;
    node.setDatarate(app_param.lora_info.DR);
  }
  if (DCS_set_Flag == 1) {
    DCS_set_Flag = 0;
    if (app_param.lora_info.dutyCycleEnabled) {
      node.setDutyCycle(true, app_param.lora_info.msPerHour);
    } else {
      node.setDutyCycle(false, app_param.lora_info.msPerHour);
    }
  }
  if (TXP_set_Flag == 1) {
    TXP_set_Flag = 0;
    node.setTxPower(app_param.lora_info.txPower);
  }
  if (RX2DR_set_Flag == 1) {
    RX2DR_set_Flag = 0;
    node.setRx2Dr(app_param.lora_info.rx2dr);
  }
  if (Band_set_Flag == 1) {
    Band_set_Flag = 0;
    if (app_param.lora_info.ActiveRegion == 0) {
      Serial.println(F("Set Regional Choices EU868"));
      node = LoRaWANNode(&radio, &Region_868, subBand);
    } else if (app_param.lora_info.ActiveRegion == 1) {
      Serial.println(F("Set Regional Choices US915"));
      node = LoRaWANNode(&radio, &Region_915, subBand);
    }
  }
}
