#include "Motor.h"

#include <cstdint>

#include "util/util.h"


void Motor::write_microseconds_helper(uint16_t micros) const {
    micros = Util::constrain_log<uint16_t, 200, 2000>(micros);

    printf("[Debug] Motor: write_microseconds_helper = %dus\n\n", micros);

    this->pin_pwm_config.write_microseconds(micros);
}

Motor::Motor(const uint pin, const bool is_reversed) : pin_pwm_config(PinPWMConfig::attach(pin)),
                                                       is_reversed(is_reversed) {}

void Motor::setSpeed(int16_t speed) const {
    speed = Util::constrain_log<int16_t, -500, 500>(speed);

    if (is_reversed) {
        speed = -speed;
    }

    // 1ms - 2ms will give full reverse to full forward, 1.5ms is neutral.
    // Source: Vex Motor Controller 29 docs
    const uint16_t micros = 1500 + speed;

    printf("[Debug] Motor: setSpeed = %d = %dus\n", speed, micros);

    this->write_microseconds_helper(micros);
}

void Motor::brake() const {
    // "A non-standard 200 us pulse will engage an active brake."
    // Source: https://wiki.purduesigbots.com/vex-electronics/legacy/motor-controller
    constexpr uint16_t micros = 200;

    this->write_microseconds_helper(micros);
}
