#include "main.h"

#include <btstack_run_loop.h>
#include <pico/stdlib.h>

#include "platform/my_platform.h"

#include "util/util.h"
#include <uni.h>

#include "drive/SquareDrive.h"
#include "drive/motor/Motor.h"

void test_motor(const Motor* motor) {
    motor->setSpeed(250);
    sleep_ms(1000);
    motor->setSpeed(-250);
    sleep_ms(1000);

    motor->setSpeed(0);
    sleep_ms(1000);
}

void test_all_motors(const Motor* motor1, const Motor* motor2, const Motor* motor3, const Motor* motor4) {
    test_motor(motor1);
    test_motor(motor2);
    test_motor(motor3);
    test_motor(motor4);
}

void test_square_drive(const SquareDrive* drive) {
    // Spin
    drive->setSpeed(0, 0, 250);
    sleep_ms(3000);

    // +X
    drive->setSpeed(250, 0, 0);
    sleep_ms(3000);

    // -X
    drive->setSpeed(-250, 0, 0);
    sleep_ms(3000);

    // +Y
    drive->setSpeed(0, 250, 0);
    sleep_ms(3000);

    // -Y
    drive->setSpeed(0, -250, 0);
    sleep_ms(3000);

    // Stop moving
    drive->setSpeed(0, 0, 0);
    sleep_ms(3000);
}

int main() {
    stdio_init_all();

    // while (!stdio_usb_connected()) {
    //     printf("USB serial connection not connected, waiting...");
    //     sleep_ms(1000);
    // }

    // initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        loge("failed to initialise cyw43_arch\n");
        return -1;
    }

    // Init motors
    auto* motor_pin18 = new Motor{18, false};
    auto* motor_pin19 = new Motor{19, true};
    auto* motor_pin20 = new Motor{20, true};
    auto* motor_pin21 = new Motor{21, false};

    test_all_motors(motor_pin18, motor_pin19, motor_pin20, motor_pin21);

    auto* drive = new SquareDrive{motor_pin18, motor_pin19, motor_pin20, motor_pin21};

    Globals::drive = drive;

    // test_square_drive(&drive);

    // Turn-on LED. Turn it off once init is done.
    Util::set_builtin_led(true);

    // Must be called before uni_init()
    uni_platform_set_custom(MyPlatform::get_platform_struct());

    // Initialize BP32
    uni_init(0, nullptr);

    // Does not return.
    btstack_run_loop_execute();

    return 0;
}
