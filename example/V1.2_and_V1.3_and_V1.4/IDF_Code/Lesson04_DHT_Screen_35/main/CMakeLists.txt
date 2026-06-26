file(
    GLOB_RECURSE LIB_SRC
    "../lib/ui/*.c"
    "../lib/LovyanGFX-master/src/*.c"
    "../lib/LovyanGFX-master/src/*.cpp"
    "../lib/lvgl/src/*.cpp"
    "../lib/lvgl/src/*.c"
    "../lib/TAMC_GT911/*.cpp"
    "../lib/Crowbits_DHT20/*.cpp"
)

# LVGL examples/demos/tests are not needed for this project and may fail under -Werror
list(FILTER LIB_SRC EXCLUDE REGEX ".*/(examples|demos|tests)/.*")
list(FILTER LIB_SRC EXCLUDE REGEX ".*/lv_.*_templ\\.c$")

idf_component_register(
    SRCS
        ${LIB_SRC}
        "main.cpp"
    INCLUDE_DIRS 
        "." 
        ".."
        "../lib/ui"
        "../lib/lvgl"
        "../lib/LovyanGFX-master/src"
        "../lib/TAMC_GT911"
        "../lib/Crowbits_DHT20"

    REQUIRES
        arduino-esp32
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_CONF_INCLUDE_SIMPLE)