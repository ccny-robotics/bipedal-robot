#include <stdint.h>
#include <stdio.h>
#include <roboticscape.h>

typedef struct Leg {
    uint8_t motor_controller;
    uint8_t motor_dir;
    uint8_t limit_switch;
    uint8_t encoder_reader;
} leg_t;

uint8_t initialize() {
    return rc_initialize() || rc_enable_motors();
}

uint8_t set_direction_pin(leg_t * leg, uint8_t dir) {
    leg->motor_dir = dir;
    return rc_set_pinmux_mode(dir, PINMUX_GPIO); // || rc_gpio_export(dir) || rc_gpio_set_dir(dir, OUTPUT_PIN);
}

uint8_t set_limit_switch(leg_t * leg, uint8_t bus, uint8_t pin) {
    leg->limit_switch = bus * 32 + pin;
    return rc_gpio_set_dir(bus * 32 + pin, INPUT_PIN);
}

uint8_t set_motor(leg_t * leg, uint8_t controller) {
    leg->motor_controller = controller;
    return rc_set_motor_brake(controller);
}

void main() {
    leg_t front, rear;

    if(initialize() ||
        set_motor(&front, 2) || set_direction_pin(&front, 111) || set_limit_switch(&front, 1, 17) ||
        set_motor(&rear, 1) || set_direction_pin(&rear, 15) || set_limit_switch(&rear, 3, 17)) {
        
        printf("[electrical_io] ERROR: failed to run rc_initialize(), are you root?\n");
	    return;
    } else {
        front.encoder_reader = 4;
        rear.encoder_reader = 3;
    }

    printf("Initial encoder values - Front: %d\tRear: %d\n", rc_get_encoder_pos(front.encoder_reader), rc_get_encoder_pos(rear.encoder_reader));
    
    if(rc_gpio_set_value(front.motor_dir, 1) || rc_gpio_set_value(rear.motor_dir, 1)) {
	    printf("gpio failure");
	    return;
    }

    // Move legs to rear
    while(rc_gpio_get_value(rear.limit_switch) != 0) {
        rc_set_motor(front.motor_controller, -0.1);
        rc_set_motor(rear.motor_controller, -0.1);
    }

    printf("Rear Limit encoder values - Front: %d\tRear: %d\n", rc_get_encoder_pos(front.encoder_reader), rc_get_encoder_pos(rear.encoder_reader));

    if(rc_gpio_set_value(front.motor_dir, 0) || rc_gpio_set_value(rear.motor_dir, 0)) {
	    printf("gpio failure");
	    return;
    }

    while(rc_gpio_get_value(front.limit_switch) != 0) {
        rc_set_motor(front.motor_controller, -0.1);
        rc_set_motor(rear.motor_controller, -0.1);
    }

    printf("Front Limit encoder values - Front: %d\tRear %d\n", rc_get_encoder_pos(front.encoder_reader), rc_get_encoder_pos(rear.encoder_reader));
    
    rc_cleanup();
}
