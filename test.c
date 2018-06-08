#include <stdint.h>
#include <stdio.h>
#include <roboticscape.h>

typedef struct Leg {
    uint8_t motor_controller;
    uint8_t motor_dir;
    uint8_t limit_switch_bus;
    uint8_t limit_switch_pin;
    uint8_t encoder_reader;
} leg_t;

uint8_t initialize() {
    return rc_motor_init() || rc_encoder_init();
}

uint8_t set_direction_pin(leg_t * leg, uint8_t dir) {
    leg->motor_dir = dir;
    return rc_pinmux_set(dir, PINMUX_GPIO); // || rc_gpio_export(dir) || rc_gpio_set_dir(dir, OUTPUT_PIN);
}

uint8_t set_limit_switch(leg_t * leg, uint8_t bus, uint8_t pin) {
    leg->limit_switch_bus = bus;
    leg->limit_switch_pin = pin;
    return rc_gpio_init(bus, pin, GPIOHANDLE_REQUEST_INPUT);
}

uint8_t set_motor(leg_t * leg, uint8_t controller) {
    leg->motor_controller = controller;
    return rc_motor_brake(controller);
}

void main() {
    leg_t front, rear;

    if(initialize() ||
        set_motor(&front, 2) || set_direction_pin(&front, SPI_HEADER_PIN_4) || set_limit_switch(&front, 1, 17) ||
        set_motor(&rear, 1) || set_direction_pin(&rear, UART1_HEADER_PIN_4) || set_limit_switch(&rear, 3, 17)) {
        
        printf("[electrical_io] ERROR: failed to run rc_initialize(), are you root?\n");
	    return;
    } else {
        front.encoder_reader = 4;
        rear.encoder_reader = 3;
    }

    printf("Initial encoder values - Front: %d\tRear: %d\n", rc_encoder_read(front.encoder_reader), rc_encoder_read(rear.encoder_reader));
    
    if(rc_gpio_set_value(front.motor_dir, 1) || rc_gpio_set_value(rear.motor_dir, 1)) {
	    printf("gpio failure");
	    return;
    }

    // Move legs to rear
    while(rc_gpio_get_value(rear.limit_switch_bus, rear.limit_switch_pin) != 0) {
        rc_motor_set(front.motor_controller, -0.1);
        rc_motor_set(rear.motor_controller, -0.1);
    }

    printf("Rear Limit encoder values - Front: %d\tRear: %d\n", rc_encoder_read(front.encoder_reader), rc_encoder_read(rear.encoder_reader));

    if(rc_gpio_set_value(front.motor_dir, 0) || rc_gpio_set_value(rear.motor_dir, 0)) {
	    printf("gpio failure");
	    return;
    }

    while(rc_gpio_get_value(front.limit_switch_bus, front.limit_switch_pin) != 0) {
        rc_motor_set(front.motor_controller, -0.1);
        rc_motor_set(rear.motor_controller, -0.1);
    }

    printf("Front Limit encoder values - Front: %d\tRear %d\n", rc_encoder_read(front.encoder_reader), rc_encoder_read(rear.encoder_reader));
    
    rc_motor_cleanup();
    rc_encoder_cleanup();
}
