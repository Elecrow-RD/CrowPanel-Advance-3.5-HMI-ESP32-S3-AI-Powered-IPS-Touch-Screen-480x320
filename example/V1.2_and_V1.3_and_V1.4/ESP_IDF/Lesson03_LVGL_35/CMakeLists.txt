# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

if(WIN32)
    set(CCACHE_ENABLE OFF CACHE BOOL "Disable ccache to avoid Windows command-line length limits" FORCE)
endif()

set(EXTRA_COMPONENT_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/lib/lvgl"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(lesson_3-3.5_lvgl)
