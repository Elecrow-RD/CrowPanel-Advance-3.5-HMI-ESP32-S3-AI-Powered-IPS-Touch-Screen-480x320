
file(
    GLOB_RECURSE LIB_SRC
    "../lib/ui/*.c"
    "../lib/LovyanGFX-master/src/*.c"
    "../lib/LovyanGFX-master/src/*.cpp"
)

idf_component_register(
    SRCS
        ${LIB_SRC}
        "main.cpp"
    INCLUDE_DIRS
        "."
        "../lib/ui"
        "../lib/LovyanGFX-master/src"
    REQUIRES
        espressif__arduino-esp32
        lvgl
)
