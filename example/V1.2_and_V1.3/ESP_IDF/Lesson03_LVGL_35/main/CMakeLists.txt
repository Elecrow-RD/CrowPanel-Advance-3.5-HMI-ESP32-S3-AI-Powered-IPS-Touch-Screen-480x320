
file(
    GLOB_RECURSE LIB_SRC
    "../lib/ui/*.c"
    "../lib/LovyanGFX-master/src/*.c"
    "../lib/LovyanGFX-master/src/*.cpp"
    "../lib/lvgl/src/*.cpp"
    "../lib/lvgl/src/*.c"
    "../lib/TAMC_GT911/*.cpp"




)




idf_component_register(
    SRCS
    ${LIB_SRC}
    "main.cpp"
    INCLUDE_DIRS 
    "." 
    "../lib/ui"
    "../lib/lvgl"
    "../lib/LovyanGFX-master/src"
    "../lib/TAMC_GT911"



    REQUIRES
    arduino-esp32
    )