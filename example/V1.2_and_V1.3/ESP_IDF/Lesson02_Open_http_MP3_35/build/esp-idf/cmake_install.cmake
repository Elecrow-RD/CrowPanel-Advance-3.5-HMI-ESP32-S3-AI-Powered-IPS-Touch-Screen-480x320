# Install script for directory: J:/wifihalowmodule/esp-idf/v5.1.1/esp-idf

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/lesson_2-open_http_mp3_small")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "J:/wifihalowmodule/tools/tools/xtensa-esp32s3-elf/esp-12.2.0_20230208/xtensa-esp32s3-elf/bin/xtensa-esp32s3-elf-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/xtensa/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_ringbuf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/efuse/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/driver/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_pm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_app_format/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/bootloader_support/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/bootloader/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esptool_py/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/partition_table/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_partition/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/app_update/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_mm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/spi_flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/pthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_system/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_rom/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/hal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/log/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/heap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_hw_support/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/freertos/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/newlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/cxx/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_common/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_timer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/app_trace/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_event/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/nvs_flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_phy/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/vfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/lwip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_netif_stack/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_netif/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/wpa_supplicant/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_coex/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/bt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/unity/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/cmock/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/console/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/http_parser/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp-tls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_adc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_eth/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_gdbstub/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_hid/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/tcp_transport/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_http_client/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_http_server/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_https_ota/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_https_server/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_psram/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_lcd/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/protobuf-c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/protocomm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/esp_local_ctrl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espcoredump/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/wear_levelling/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/sdmmc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/idf_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/ieee802154/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/json/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/mqtt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/perfmon/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/spiffs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/touch_element/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/ulp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/usb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/wifi_provisioning/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__network_provisioning/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/chmorgan__esp-libhelix-mp3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp-modbus/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__libsodium/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__mdns/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/joltwallet__littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp-sr/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp-zboss-lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp-zigbee-lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__rmaker_common/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp_diagnostics/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__cbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp_diag_data_store/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp_insights/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp_modem/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__jsmn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__json_parser/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__json_generator/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp_schedule/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp-serial-flasher/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp_rcp_update/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp_secure_cert_mgr/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__esp_rainmaker/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/espressif__qrcode/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/arduino-esp32/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson02_Open_http_MP3_35/build/esp-idf/main/cmake_install.cmake")
endif()

