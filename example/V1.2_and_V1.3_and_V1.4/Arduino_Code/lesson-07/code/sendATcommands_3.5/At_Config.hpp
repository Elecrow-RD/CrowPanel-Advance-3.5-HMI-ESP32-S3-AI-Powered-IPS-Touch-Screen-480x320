#ifndef _AT_CONFIG_H_
#define _AT_CONFIG_H_

#pragma once

#include <Arduino.h>
#include "Cmd_String.h"
#include "Param_Var.h"
#include "Cmd_List.h"
#include "Cmd_Callback.h"

/*---------------------------------------------------------------
 * AT parser facade
 * This class wraps serial reception and command dispatch so the lesson
 * can treat the AT command set as a single subsystem.
 *--------------------------------------------------------------*/
class AT_Config
{
    private:
        static AT_Config* instance_;

    public:
        AT_Config(void);  

        static AT_Config& getInstance();

        void begin( void );

        /**
         * @brief Parse one AT command line.
         *
         * @param cmd NUL-terminated AT command string.
         * @param length Number of received bytes in the command buffer.
         * @return None.
         */
        void parseCmd(const char *cmd, uint16_t length);

        /**
         * @brief Read one command line from Serial into a buffer.
         *
         * @param buf Output buffer that receives the serial bytes.
         * @param size Output length of the received command.
         * @return true if at least one byte was received.
         * @return false if no serial data was waiting.
         */
        bool receiveSerialCmd(uint8_t *buf, uint8_t *size);

    protected:

};

extern AT_Config at_config;

#endif
