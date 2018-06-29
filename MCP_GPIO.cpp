/*************************************************** 

 ****************************************************/

#include <memory>
#include <stdexcept>

static inline uint8_t receive_one(uint8_t register_address) {
    std::unique_ptr<uint8_t> buffer(new uint8_t(0));
    if(rc_i2c_read_byte(this->device_address, register_address, buffer.get()))
        throw std::runtime_error("Byte Read Failure");
    
    return buffer.get();
}

static inline long receive_multiple(uint8_t register_address, size_t length) {
    std::unique_ptr<int> buffer(new int(0));
    if(rc_i2c_read_bytes(this->device_address, register_address, length, static_cast<uint8_t *>(buffer.get())))
        throw std::runtime_error("Byte Stream Read Failure");
    
    return buffer.get();
}

static inline void send_address(uint8_t register_address) {
    if(rc_i2c_send_byte(this->device_address, register_address))
        throw std::runtime_error("Address Send Failure");
}

static inline void send_one(uint8_t register_address, uint8_t data) {
    if(rc_i2c_write_byte(this->device_address, register_address, data))
        throw std::runtime_error("Data Send Failure");
}

static inline void send_multiple(uint8_t register_address, size_t length, uint8_t * data) {
    if(rc_i2c_write_bytes(this->device_address, register_address, length, data))
        throw std::runtime_error("Data Stream Send Failure");
}

/**
 * Initializes the MCP23017 given its HW selected address, see datasheet for Address selection.
 */
MCP_GPIO::MCP_GPIO(int bus, uint8_t device_address) :
    pins(0)
{
	uint8_t i2caddr = device_address > 7 ? 7 : device_address;
    this->device_address = MCP23017_ADDRESS | i2caddr;

    if(rc_i2c_init(bus, device_address))
        throw std::runtime_error("Failed to Initialize I2C Bus");

	// Set defaults (all outputs on port A and B)
	write_register(MCP23017_IODIRA, 0x0);
	write_register(MCP23017_IODIRB, 0x0);
}

/**
 * Delegating constructor.
 * Initializes the default MCP23017, with 000 for the configurable part of the address.
 */
MCP_GPIO::MCP_GPIO() :
    MCP_GPIO(0, 0)
{}


/**
 * Bit number associated to a give Pin
 */
uint8_t MCP_GPIO::pin_to_bit(uint8_t pin){
	return pin % 8;
}

/**
 * Register address, port dependent, for a given PIN
 */
uint8_t MCP_GPIO::pin_to_address(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr){
	return(pin < 8) ? portAaddr : portBaddr;
}

/**
 * Reads a given register
 */
uint8_t MCP_GPIO::read_register(uint8_t reg){
    send_address(reg);
    return recieve_one(reg);
}

/**
 * Writes a given register
 */
void MCP_GPIO::write_register(uint8_t reg, uint8_t value){
    send_one(reg, value);
}


/**
 * Helper to update a single bit of an A/B register.
 * - Reads the current register value
 * - Writes the new register value
 */
void MCP_GPIO::update_bit(uint8_t pin, uint8_t value, uint8_t register_A, uint8_t register_B) {
    uint8_t bit = pin_to_bit(pin);
    uint8_t register_address = pin_to_address(pin, register_A, register_B);
    uint8_t register_value = read_register(register_address);
    
    // Set the value for the particular bit
    if((register_value & (1 << bit) != (value << bit)) {
        register_value ^= (value << bit);
        write_register(register_address, register_value);
    }
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Sets the pin mode to either INPUT or OUTPUT
 */
void MCP_GPIO::pin_mode(uint8_t pin, uint8_t direction) {
	update_bit(pin, (direction == INPUT), MCP23017_IODIRA, MCP23017_IODIRB);
}

/**
 * Writes all the pins in one go. This method is very useful if you are implementing a multiplexed matrix and want to get a decent refresh rate.
 */
void MCP_GPIO::write_gpio(uint16_t values) {
    uint8_t * data = static_cast<uint8_t *>(&values);
    send_multiple(MCP23017_GPIOA, 2, data);
}

void MCP_GPIO::digital_write(uint8_t pin, uint8_t value) {
	uint8_t gpio;
	uint8_t bit = pin_to_bit(pin);

	uint8_t register_address = pin_to_address(pin, MCP23017_OLATA,MCP23017_OLATB);
    
    // write the new GPIO
    this->pins[bit] = value;
    write_register(register_address, static_cast<uint8_t>(pins.to_ulong()));
}

void MCP_GPIO::set_pull_up(uint8_t pin, uint8_t direction) {
	update_bit(pin, direction, MCP23017_GPPUA, MCP23017_GPPUB);
}

