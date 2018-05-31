#include <iostream>

#include <roboticscape.hpp>

int main() {
    int motor_controller_f = 2;
    int limit_switch_f = 1 * 32 + 17;
    int encoder_reader_f = 4;
    
    int motor_controller_r = 1;
    int limit_switch_r = 3 * 32 + 17;
    int encoder_reader_r = 3;
    
    if(
        rc_initialize() ||
        rc_enable_motors() ||
        rc_set_motor_brake(motor_controller_f) || rc_set_motor_brake(motor_controller_r) ||
        rc_gpio_export(limit_switch_f) || rc_gpio_export(limit_switch_r) ||
        rc_gpio_set_dir(limit_switch, INPUT_PIN)
    ) {
        throw std::runtime_error("[electrical_io] ERROR: failed to run rc_initialize(), are you root?\n");
    }

    std::cout << "Initial encoder values - Front: " << rc_get_encoder_pos(encoder_reader_f) << "\tRear: " rc_get_encoder_pos(encoder_reader_r)

    // Move legs to rear
    while(rc_gpio_get_value(limit_switch_r) != 1) {
        rc_set_motor(motor_controller_f, -0.1);
        rc_set_motor(motor_controller_r, -0.1);
    }

    std::cout << "Rear Limit encoder values - Front: " << rc_get_encoder_pos(encoder_reader_f) << "\tRear: " rc_get_encoder_pos(encoder_reader_r)

    while(rc_gpio_get_value(limit_switch_f) != 1) {
        rc_set_motor(motor_controller_f, 0.1);
        rc_set_motor(motor_controller_r, 0.1);
    }

    std::cout << "Front Limit encoder values - Front: " << rc_get_encoder_pos(encoder_reader_f) << "\tRear: " rc_get_encoder_pos(encoder_reader_r)
    
    rc_cleanup();
    return 0;
}