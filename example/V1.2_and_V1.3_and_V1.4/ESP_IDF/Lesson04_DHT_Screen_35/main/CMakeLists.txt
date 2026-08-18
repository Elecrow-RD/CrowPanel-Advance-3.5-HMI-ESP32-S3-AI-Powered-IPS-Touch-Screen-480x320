file(
    GLOB_RECURSE LIB_SRC
    "../lib/ui/*.c"
    "../lib/LovyanGFX-master/src/*.c"
    "../lib/LovyanGFX-master/src/*.cpp"
    "../lib/Crowbits_DHT20/*.cpp"
)

idf_component_register(
    SRCS
        ${LIB_SRC}
        "main.cpp"
    INCLUDE_DIRS 
        "." 
        "../lib/ui"
        "../lib/LovyanGFX-master/src"
        "../lib/Crowbits_DHT20"

    REQUIRES
        espressif__arduino-esp32
        lvgl
)
