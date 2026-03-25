cmake_minimum_required(VERSION 3.13)

# PICO_BOARD should be set to 'pico_w', 'pico2_w' or not set at all.
if (DEFINED PICO_BOARD)
    message(DEBUG, "Using PICO_BOARD ${PICO_BOARD}")
else ()
    message(VERBOSE, "PICO_BOARD not defined. Using 'pico_w'")
    set(PICO_BOARD "pico_w")
endif ()

##### Bluepad32 Stuff #####

set(BLUEPAD32_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/lib/bluepad32)

if (FALSE)
    # To use BTstack from Pico SDK do
    set(BTSTACK_ROOT ${PICO_SDK_PATH}/lib/btstack)
else ()
    # If you want to use latest BTstack (included in Bluepad32 repo),
    set(BTSTACK_ROOT ${BLUEPAD32_ROOT}/external/btstack)
endif ()

# Needed for btstack_config.h / sdkconfig.h
# so that libbluepad32 can include them
include_directories(${PROJECT_NAME} src)

# Needed when using BTstack from our branch
include_directories(${BTSTACK_ROOT}/3rd-party/bluedroid/encoder/include)
include_directories(${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/include)

# Need for certain IDEs, like CLion. Otherwise it won't find btstack
include_directories(${BTSTACK_ROOT}/src)

###########################

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(demo_bot C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

##### My Application #####

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/util/util.h
        src/platform/my_platform.cpp
        src/platform/my_platform.h
        src/btstack_config.h
        src/sdkconfig.h
        src/drive/motor/Motor.cpp
        src/drive/motor/Motor.h
        src/drive/motor/pico_pwm.h
        src/drive/SquareDrive.cpp
        src/drive/SquareDrive.h
        src/main.h
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        pico_stdlib
        pico_cyw43_arch_none
        pico_btstack_classic
        pico_btstack_cyw43
        hardware_pwm
        bluepad32
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# The RP2040 has been certified to run at 200MHz since Pico SDK v2.1.1
target_compile_definitions(${PROJECT_NAME} PRIVATE SYS_CLK_MHZ=200)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

##########################

add_subdirectory(${BLUEPAD32_ROOT}/src/components/bluepad32 libbluepad32)

target_include_directories(${PROJECT_NAME} PRIVATE
        src
        ${BLUEPAD32_ROOT}/src/components/bluepad32/include)

# Useful for debugging
#target_compile_definitions(${PROJECT_NAME} PRIVATE
#        PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=3000
#        WANT_HCI_DUMP=1 # This enables btstack debug
#        ENABLE_SEGGER_RTT=1
#)