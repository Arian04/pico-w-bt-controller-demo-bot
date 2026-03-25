#pragma once

#include "sdkconfig.h"

// Sanity check
#ifndef CONFIG_BLUEPAD32_PLATFORM_CUSTOM
#error "Pico W must use BLUEPAD32_PLATFORM_CUSTOM"
#endif

// Forward declaration
struct uni_platform;

namespace MyPlatform {
    //
    // Entry Point
    //
    uni_platform* get_platform_struct();
}
