#include <stdio.h>
#include <roboticscape.h>

void main() {
    // Front Left
    int motor_controller_f = 2;
    int motor_dir_f = 111;
    int limit_switch_f = 1 * 32 + 17;
    int encoder_reader_f = 4;
    

    int motor_controller_r = 1;
    int motor_dir_r = 3 * 32 + 2;
    int limit_switch_r = 3 * 32 + 17;
    int encoder_reader_r = 3;
    
    if(
        rc_initialize() ||
        rc_enable_motors() ||
        rc_set_motor_brake(motor_controller_f) || rc_set_motor_brake(motor_controller_r) ||
        rc_gpio_export(limit_switch_f) || rc_gpio_export(limit_switch_r) || rc_gpio_export(motor_dir_r) || rc_gpio_export(motor_dir_f) ||
        rc_gpio_set_dir(limit_switch_f, INPUT_PIN) || rc_gpio_set_dir(limit_switch_r, INPUT_PIN) ||
	rc_set_pinmux_mode(motor_dir_f, PINMUX_GPIO_PD) || //rc_set_pinmux_mode(motor_dir_r, PINMUX_GPIO_PD) ||
	rc_gpio_set_dir(motor_dir_r, OUTPUT_PIN) || rc_gpio_set_dir(motor_dir_f, OUTPUT_PIN)
    ) {
        printf("[electrical_io] ERROR: failed to run rc_initialize(), are you root?\n");
	return;
    }

    if(

    )

    printf("Initial encoder values - Front: %d\tRear: %d\n", rc_get_encoder_pos(encoder_reader_f), rc_get_encoder_pos(encoder_reader_r));
    
    if(rc_gpio_set_value(motor_dir_f, 1) || rc_gpio_set_value(motor_dir_r, 1)) {
	printf("gpio failure");
	return;
    }

    // Move legs to rear
    while(rc_gpio_get_value(limit_switch_r) != 0) {
        rc_set_motor(motor_controller_f, -0.1);
        rc_set_motor(motor_controller_r, -0.1);
    }

    printf("Rear Limit encoder values - Front: %d\tRear: %d\n", rc_get_encoder_pos(encoder_reader_f), rc_get_encoder_pos(encoder_reader_r));

    if(rc_gpio_set_value(motor_dir_f, 0) || rc_gpio_set_value(motor_dir_r, 0)) {
	printf("gpio failure");
	return;
    }

    while(rc_gpio_get_value(limit_switch_f) != 0) {
        rc_set_motor(motor_controller_f, -0.1);
        rc_set_motor(motor_controller_r, -0.1);
    }

    printf("Front Limit encoder values - Front: %d\tRear %d\n", rc_get_encoder_pos(encoder_reader_f), rc_get_encoder_pos(encoder_reader_r));
    
    rc_cleanup();
}
