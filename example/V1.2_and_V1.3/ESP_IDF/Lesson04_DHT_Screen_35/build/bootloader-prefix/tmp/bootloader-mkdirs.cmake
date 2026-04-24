# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "J:/wifihalowmodule/esp-idf/v5.1.1/esp-idf/components/bootloader/subproject"
  "J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson04_DHT_Screen_35/build/bootloader"
  "J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson04_DHT_Screen_35/build/bootloader-prefix"
  "J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson04_DHT_Screen_35/build/bootloader-prefix/tmp"
  "J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson04_DHT_Screen_35/build/bootloader-prefix/src/bootloader-stamp"
  "J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson04_DHT_Screen_35/build/bootloader-prefix/src"
  "J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson04_DHT_Screen_35/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson04_DHT_Screen_35/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "J:/Advance_Code_New/git_code_V13/35_V12/IDF_Code/Lesson04_DHT_Screen_35/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
