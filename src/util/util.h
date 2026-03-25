#pragma once

#include <pico/cyw43_arch.h>
#include <concepts>
#include <iostream>
#include <format>

namespace Util {
    inline void set_builtin_led(const bool value) {
        return cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, value);
    }

    template <std::integral T, T min, T max, bool enable_logging>
    [[nodiscard]] static T constrain_helper(const T value) {
        static_assert(min < max, "min must be less than max");

        // Constrain values
        if (value < min) {
            if (enable_logging) {
                const std::string value_str = std::to_string(value);
                const std::string min_str = std::to_string(min);
                printf("[Warning] Constrained value of '%s' to '%s'\n", value_str.c_str(), min_str.c_str());
            }
            return min;
        } else if (value > max) {
            if (enable_logging) {
                const std::string value_str = std::to_string(value);
                const std::string max_str = std::to_string(max);
                printf("[Warning] Constrained value of '%s' to '%s'\n", value_str.c_str(), max_str.c_str());
            }
            return max;
        } else {
            return value;
        }
    }

    template <std::integral T, T min, T max>
    [[nodiscard]] static T constrain(const T value) {
        return constrain_helper<T, min, max, false>(value);
    }

    template <std::integral T, T min, T max>
    [[nodiscard]] static T constrain_log(const T value) {
        return constrain_helper<T, min, max, true>(value);
    }
}
