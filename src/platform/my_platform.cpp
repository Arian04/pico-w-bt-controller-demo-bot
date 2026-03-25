#include "my_platform.h"

#include <ios>
#include <uni.h>

#include "main.h"
#include "drive/SquareDrive.h"
#include "util/util.h"


/**
 * Implementation of the struct `uni_platform` in `uni_platform.h`
 */
namespace MyPlatform::Impl {
    // ReSharper disable once CppUseAuto
    // Manually specifying the type, because this needs to match the struct we're implementing
    constexpr char const* PLATFORM_NAME = "My Platform";

    //
    // Helpers
    //
    inline void play_dual_rumble(
        uni_hid_device_s* d,
        const uint16_t start_delay_ms,
        const uint16_t duration_ms,
        const uint8_t weak_magnitude,
        const uint8_t strong_magnitude
    ) {
        [[likely]] if (d->report_parser.play_dual_rumble != nullptr) {
            d->report_parser.play_dual_rumble(d, start_delay_ms, duration_ms, weak_magnitude, strong_magnitude);
        } else {
            loge("d->report_parser.play_dual_rumble is nullptr\n");
        }
    }

    static void handle_gamepad_system_button_pressed(uni_hid_device_t* d) {
        play_dual_rumble(
            d,
            0, 50,
            128, 40
        );
    }

    static void handle_controller_data(uni_hid_device_t* device, const uni_controller_t* controller) {
        const uni_gamepad_t* gamepad = &controller->gamepad;

        if ((gamepad->buttons & BUTTON_A)) {
            play_dual_rumble(device,
                             0, 250,
                             128, 0);
        }

        if ((gamepad->buttons & BUTTON_B)) {
            play_dual_rumble(device, 0, 250,
                             0, 128);
        }

        // Triggers: Variable rumble
        if (gamepad->buttons & BUTTON_TRIGGER_L) {
            const uint32_t raw_val = gamepad->brake; // value is known to be in [0, 1023];
            const uint8_t val = raw_val / 4;

            xboxone_play_quad_rumble(
                device,
                0, 50,
                val, 0,
                0, 0
            );

            if (Globals::drive == nullptr) {
                printf("ERROR: drive is nullptr\n");
            } else {
                Globals::drive->brake();
                return;
            }
        }
        if (gamepad->buttons & BUTTON_TRIGGER_R) {
            const uint32_t raw_val = gamepad->throttle; // value is known to be in [0, 1023];
            const uint8_t val = raw_val / 4;

            xboxone_play_quad_rumble(
                device,
                0, 50,
                0, val,
                0, 0
            );
        }

        auto left_x = Util::constrain<int16_t, -500, 500>(gamepad->axis_x);
        auto left_y = Util::constrain<int16_t, -500, 500>(gamepad->axis_y);
        auto right_x = Util::constrain<int16_t, -500, 500>(gamepad->axis_rx);
        auto right_y = Util::constrain<int16_t, -500, 500>(gamepad->axis_ry);

        if (left_x < 50 && left_x > -50) {
            left_x = 0;
        }
        if (left_y < 50 && left_y > -50) {
            left_y = 0;
        }
        if (right_x < 50 && right_x > -50) {
            right_x = 0;
        }
        if (right_y < 50 && right_y > -50) {
            right_y = 0;
        }

        printf("lx, ly, rx, ry: %d, %d, %d, %d\n", left_x, left_y, right_x, right_y);

        if (Globals::drive == nullptr) {
            printf("ERROR: drive is nullptr\n");
        } else {
            Globals::drive->setSpeed(left_x, left_y, right_x);
        }
    }

    //
    // Platform Overrides
    //
    static void init(int argc, const char** argv) {
        logi("my_platform: init()\n");
    }

    static void on_init_complete() {
        logi("my_platform: on_init_complete()\n");

        // Safe to call "unsafe" functions since they are called from BT thread

        // Start scanning and autoconnect to supported controllers.
        uni_bt_start_scanning_and_autoconnect_unsafe();

        // list the stored BT keys.
        uni_bt_list_keys_unsafe();

        // Turn off LED once init is done.
        Util::set_builtin_led(false);

        // uni_bt_service_set_enabled(true);

        uni_property_dump_all();
    }

    /**
     * You can filter discovered devices here. Return any value different from UNI_ERROR_SUCCESS;
     * @param addr: the Bluetooth address
     * @param name: could be NULL, could be zero-length, or might contain the name.
     * @param class_of_device: Class of Device. See "uni_bt_defines.h" for possible values.
     * @param rssi: Received Signal Strength Indicator (RSSI) measured in dBms. The higher (255) the better.
     * @return
     */
    static uni_error_t on_device_discovered(
        bd_addr_t addr,
        const char* name,
        const uint16_t class_of_device,
        uint8_t rssi
    ) {
        // As an example, if you want to filter out keyboards, do:
        if (((class_of_device & UNI_BT_COD_MINOR_MASK) & UNI_BT_COD_MINOR_KEYBOARD) == UNI_BT_COD_MINOR_KEYBOARD) {
            logi("Ignoring keyboard\n");
            return UNI_ERROR_IGNORE_DEVICE;
        }

        return UNI_ERROR_SUCCESS;
    }

    static void on_device_connected(uni_hid_device_t* d) {
        logi("my_platform: device connected: %p\n", d);
    }

    static void on_device_disconnected(uni_hid_device_t* d) {
        logi("my_platform: device disconnected: %p\n", d);

        if (Globals::drive == nullptr) {
            printf("ERROR: drive is nullptr\n");
        } else {
            Globals::drive->setSpeed(0, 0, 0);
            for (int i = 0; i < 4; ++i) {
                Util::set_builtin_led(true);
                sleep_ms(250);
                Util::set_builtin_led(false);
                sleep_ms(250);
            }
        }
    }

    static uni_error_t on_device_ready(uni_hid_device_t* d) {
        logi("my_platform: device ready: %p\n", d);

        // You can reject the connection by returning an error.
        return UNI_ERROR_SUCCESS;
    }

    static void on_controller_data(uni_hid_device_t* device, uni_controller_t* controller) {
        // Print device ID before dumping gamepad.
        logi("(%p) id=%d ", device, uni_hid_device_get_idx_for_instance(device));
        uni_controller_dump(controller);

        [[likely]] if (controller->klass == UNI_CONTROLLER_CLASS_GAMEPAD) {
            handle_controller_data(device, controller);
        } else {
            loge("Controller class that isn't a Gamepad was detected: %d", controller->klass);

            switch (controller->klass) {
            case UNI_CONTROLLER_CLASS_BALANCE_BOARD:
                uni_balance_board_dump(&controller->balance_board);
                break;
            case UNI_CONTROLLER_CLASS_MOUSE:
                uni_mouse_dump(&controller->mouse);
                break;
            case UNI_CONTROLLER_CLASS_KEYBOARD:
                uni_keyboard_dump(&controller->keyboard);
                break;
            default:
                loge("Unsupported controller class: %d\n", controller->klass);
                break;
            }
        }
    }

    static const uni_property_t* get_property(__unused uni_property_idx_t idx) {
        return nullptr;
    }

    static void on_oob_event(const uni_platform_oob_event_t event, void* data) {
        switch (event) {
        case UNI_PLATFORM_OOB_GAMEPAD_SYSTEM_BUTTON:
            // Optional: do something when "system" button gets pressed.
            handle_gamepad_system_button_pressed(static_cast<uni_hid_device_t*>(data));
            break;

        case UNI_PLATFORM_OOB_BLUETOOTH_ENABLED:
            // When the "bt scanning" is on / off. Could be triggered by different events
            // Useful to notify the user
            logi("my_platform_on_oob_event: Bluetooth enabled: %d\n", static_cast<bool>(data));
            break;

        default:
            logi("my_platform_on_oob_event: unsupported event: 0x%04x\n", event);
        }
    }
}

//
// Entry Point
//
uni_platform* MyPlatform::get_platform_struct() {
    static uni_platform plat = {
        .name = Impl::PLATFORM_NAME,
        .init = Impl::init,
        .on_init_complete = Impl::on_init_complete,
        .on_device_discovered = Impl::on_device_discovered,
        .on_device_connected = Impl::on_device_connected,
        .on_device_disconnected = Impl::on_device_disconnected,
        .on_device_ready = Impl::on_device_ready,
        .on_controller_data = Impl::on_controller_data,
        .get_property = Impl::get_property,
        .on_oob_event = Impl::on_oob_event,
    };

    return &plat;
}
