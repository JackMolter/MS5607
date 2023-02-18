#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/i2c.h"

#include "barometer.h"

static int addr = MS5607_id;

void ms5607_init() {
    // set i2c pins
    i2c_init(MS5607_PORT, MS5607_BAUD);
    gpio_set_function(MS5607_SDA, GPIO_FUNC_I2C);
    gpio_set_function(MS5607_SCL, GPIO_FUNC_I2C);
}


void ms5607_reset() {
    uint8_t com = MS5607_RESET;

    i2c_write_blocking(MS5607_PORT, addr, &com, 1, true);

}

void ms5607_prom_read() {
    uint8_t pom = MS5607_PROM;
    uint8_t buf[2];     // may be longer 

    i2c_write_blocking(MS5607_PORT, addr, &pom, 1, true);
    i2c_read_blocking(MS5607_PORT, addr, buf, 2, false);
}

void ms5607_conversion() {
    uint8_t init = MS5607_CONVERT;
    uint8_t cmd = MS5607_ADC;
    uint8_t data[3];

    i2c_write_blocking(MS5607_PORT, addr, &init, 1, true);
    i2c_write_blocking(MS5607_PORT, addr, &cmd, 1, true);
    i2c_read_blocking(MS5607_PORT, addr, data, 3, false);


}


void main() {
    stdio_init_all();


}