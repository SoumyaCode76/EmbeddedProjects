#1. Declare minimum CMake version required to build this project
cmake_minimum_required(VERSION 3.20.0)

set(SIMULATION_MODE "OFF" CACHE STRING "Simulation mode: OFF, ON, or EMU")

if(SIMULATION_MODE STREQUAL "EMU")
    set(DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/app_emulation.overlay)
    set(BOARD qemu_cortex_m0)
    set(CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/prj_emulation.conf)
elseif(SIMULATION_MODE STREQUAL "ON")
    set(DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/app_picsimlab.overlay)
    set(BOARD nucleo_f103rb)
    set(CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/prj_picsimlab.conf)
else()
    set(DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/app.overlay)
    set(BOARD stm32f429i_disc1)
    set(CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/prj.conf)
endif()

#2. Locate Zephyr build system as a CMake module
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
#3. Name your project
project(Testing_IRQ)
#4. Tell Zephyr which source files to use
target_sources(app PRIVATE src/main.c)
#5. Add include directories
target_include_directories(app PRIVATE inc)
#6. Set properties for the executable target 'app'