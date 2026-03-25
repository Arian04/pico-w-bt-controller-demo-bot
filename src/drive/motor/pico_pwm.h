#pragma once

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

class PinPWMConfig {
private:
    explicit PinPWMConfig(const uint gpio_pin)
        : gpio_pin(gpio_pin),
          slice_num(pwm_gpio_to_slice_num(gpio_pin)),
          chan(pwm_gpio_to_channel(gpio_pin)),
          wrapCounter(pwm_set_freq(this->slice_num, FREQUENCY_HZ)) {}

    static uint32_t pwm_set_calculated_freq(const uint slice_num, const uint32_t freq, const uint32_t clk_sys_freq_hz);

    static uint32_t pwm_set_freq(const uint pwm_slice_num, const uint32_t freq);

public:
    static constexpr uint32_t FREQUENCY_HZ = 50;

    const uint gpio_pin;
    const uint slice_num;
    const uint chan;

    const uint16_t wrapCounter;

    static PinPWMConfig attach(const uint gpio_pin) {
        gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
        return PinPWMConfig{gpio_pin};
    }

    ~PinPWMConfig() {
        // pwm_set_enabled(this->slice_num, false);

        // TODO:
        //  is this the right GPIO_FUNC to reset the pin to? maybe call gpio_get_function(gpio_pin)
        //  and see what it returns beforehand
        // gpio_set_function(gpio_pin, GPIO_FUNC_NULL);
    }

    void write_microseconds(const uint16_t pulse_length_counter) const {
        // FIXME: this logic relies on the counter decrementing once per microsecond
        pwm_set_chan_level(slice_num, chan, pulse_length_counter);
    }
};

inline uint32_t PinPWMConfig::pwm_set_calculated_freq(
    const uint slice_num,
    const uint32_t freq,
    const uint32_t clk_sys_freq_hz
) {
    uint32_t divider16 = clk_sys_freq_hz / freq / 4096 +
        (clk_sys_freq_hz % (freq * 4096) != 0);

    if (divider16 < 16) {
        divider16 = 16;
    }

    const uint32_t wrap = (((clk_sys_freq_hz * 16) / divider16) / freq) - 1;
    pwm_set_clkdiv_int_frac4(slice_num, divider16 / 16, divider16 & 0xF);

    return wrap;
}

inline uint32_t PinPWMConfig::pwm_set_freq(const uint pwm_slice_num, const uint32_t freq) {
    const uint32_t clk_sys_freq_hz = clock_get_hz(clk_sys);

    // Use more precise precomputed values for common clock frequencies, but fall back to computing them at runtime.
    uint32_t wrap;
    switch (clk_sys_freq_hz) {
    case 200'000'000:
        /* Scratch math:
               desired freq = 50Hz = repeat every 20ms
               desired pulse widths = [0, 2] ms = [0, 2000] us
               minimum resolution = 1 us
                                  => 20ms/1us = 20000us/1us 20000 steps
            */
        pwm_set_clkdiv_int_frac4(pwm_slice_num, 200, 0);
        wrap = 20000;
        break;
    case 125'000'000:
        // TODO:
        // [[fallthrough]]
    default:
        wrap = pwm_set_calculated_freq(pwm_slice_num, freq, clk_sys_freq_hz);;
    }

    pwm_set_wrap(pwm_slice_num, wrap);

    pwm_set_enabled(pwm_slice_num, true);

    return wrap;
}
