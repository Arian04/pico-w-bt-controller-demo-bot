#pragma once
#include <btstack_run_loop.h>
#include <uni.h>

inline void init_btstack_and_bluepad() {
    // Initialize BP32
    uni_init(0, nullptr);

    // Does not return.
    btstack_run_loop_execute();
}

inline void set_builtin_led(const bool value) {
    return cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, value);
}

