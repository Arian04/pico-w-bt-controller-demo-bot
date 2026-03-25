#pragma once

#include "pico_pwm.h"
#include "pico/types.h"

// This class represents a connection to a motor via a Vex Motor Controller 29
class Motor {
private:
    PinPWMConfig pin_pwm_config;

    const bool is_reversed;

    void write_microseconds_helper(uint16_t micros) const;

public:
    explicit Motor(const uint pin, const bool is_reversed);

    /**
     *
     * @param speed int in the range [-500, 500]
     */
    void setSpeed(int16_t speed) const;

    void brake() const;
};
