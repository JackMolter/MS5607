#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/i2c.h"
#include "barometer.h"
#include "math.h"

static int addr = MS5607_id;
// Variables
uint16_t C1, C2, C3, C4, C5, C6;
uint32_t D1, D2;
int32_t dT, TEMP;
int64_t OFF, SENS, P;

void ms5607_init() {
    // I2C pins setup
    i2c_init(MS5607_PORT, MS5607_BAUD);
    gpio_set_function(MS5607_SDA, GPIO_FUNC_I2C);
    gpio_set_function(MS5607_SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(MS5607_SDA);
    //gpio_pull_up(MS5607_SCL);


}
void ms5607_reset() {
    uint8_t com = MS5607_CMD_RESET;
    i2c_write_blocking(MS5607_PORT, addr, &com, 1, true);
}

void ms5607_prom_read() { // return 128 bit
    // Address 0 contains factory data and the setup, addresses 1-6 calibration 
    // coefficients and address 7 contains the serial code and CRC
    uint8_t pom[6];
    pom[0] = MS5607_CMD_PROM_READ_C1;
    pom[1] = MS5607_CMD_PROM_READ_C2;
    pom[2] = MS5607_CMD_PROM_READ_C3;
    pom[3] = MS5607_CMD_PROM_READ_C4;
    pom[4] = MS5607_CMD_PROM_READ_C5;
    pom[5] = MS5607_CMD_PROM_READ_C6;

    uint8_t data[2];
    uint16_t callibration_values[6];
    printf("test");
    for(int i = 0; i < 6; i++) {
        i2c_write_blocking(MS5607_PORT, addr, &pom[i], 1, true);
        i2c_read_blocking(MS5607_PORT, addr, data, 2, false);
        callibration_values[i] = ((uint16_t)data[0] << 8) | data[1];    // bit wise operation to combine 2 8 bit into 1 16 bit
        printf("Cal= %u\n", callibration_values[i]);
        sleep_ms(2);
    }
    C1 = callibration_values[0]; // Presure sensitivity
    C2 = callibration_values[1]; // pressure offset
    C3 = callibration_values[2]; // Temperature coefficient of pressure sensitivity
    C4 = callibration_values[3]; // Temperature coefficient of pressure offset
    C5 = callibration_values[4]; // Reference temperature
    C6 = callibration_values[5]; // Temperature coefficient of the temperature |
    
}
void ms5607_D1_conversion(){
    uint8_t init = MS5607_CONVERT_D1; //uncompensated pressure
    uint8_t cmd = MS5607_CMD_ADC_READ;
    uint8_t data[3];  
    i2c_write_blocking(MS5607_PORT, addr, &init, 1, true);
    sleep_ms(10);
    i2c_write_blocking(MS5607_PORT, addr, &cmd, 1, true);
    i2c_read_blocking(MS5607_PORT, addr, data, 3, false);
    //D1 = (uint32_t) data[0] + (uint32_t) data[1] + (uint32_t) data[2];
    D1 = ((uint32_t)data[0] << 16) | (data[1] << 8) | data[2];
}

void ms5607_D2_conversion(){
    uint8_t init = MS5607_CONVERT_D2; //uncompensated temperature
    uint8_t cmd = MS5607_CMD_ADC_READ;
    uint8_t data[3];
    i2c_write_blocking(MS5607_PORT, addr, &init, 1, true);
    sleep_ms(10);
    i2c_write_blocking(MS5607_PORT, addr, &cmd, 1, true);
    i2c_read_blocking(MS5607_PORT, addr, data, 3, false);
    D2 = ((uint32_t)data[0] << 16) | (data[1] << 8) | data[2];
    //printf("D2= %d\n", D2);
}
void ms5607_convert() {
    //Temp Calculations
    //dT = (int32_t)D2 - ((int32_t)C5*2^8);
    //TEMP = 2000 + dT*((int32_t)C6)/2^23;
    dT = D2 - (C5* pow(2,8));
    TEMP = 2000 + dT * (C6/ pow(2,23));
    printf("DT= %d\n", dT);
    //printf("TEMP= %d\n", TEMP);

    //Compensated temp calcululations
    OFF = C2*pow(2,17) + (C4*dT)/ pow(2,6);
    SENS = C1 * pow(2,16) + ((C3 * dT)/ pow(2,7));

    //P = D1 * (SENS/pow(2,31) - OFF)/ pow(2,15);
    //printf("OFF= %lld\n", OFF);
    //printf("D1= %u\n", D1);
    //printf("SENS= %lld\n", SENS);
    P = ((((int64_t) D1 * SENS) >> 21) - OFF) >> 15;
    printf("Temperature: %d Pressure: %d \n",TEMP,P);

}
/*
void ms5607_second_order_conversion() {
    int64_t OFF2, SENS2;
    if (TEMP < 20) {
        T2 = (dT^2)/(2^31); // T2
        // int32_t T2 = ((int64_t) dT * (int64_t) dT) >> 31;
        OFF2 = 61*((int64_t)TEMP - 2000)^2/(2^4);
        SENS2 = 2* ((int64_t)TEMP -2000)^2;
        if (TEMP < -15) {
            OFF2 = OFF2 + 15*((int64_t)TEMP+1500)^2;
            SENS2 = SENS2 + 8*((int64_t)TEMP +1500)^2;
        }
    } else {
        T2 = 0;
        OFF2 = 0;
        SENS2 = 0;
    }
    TEMP = TEMP - T2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;
    printf("TEMP: %d | Y axis: %d | Z axis: %d |\r\n", TEMP, OFF, SENS); // %d might be wrong
    sleep_ms(300);
}
*/
void main() {
    stdio_init_all();
    //sleep_ms(25000);
    ms5607_init();
    ms5607_reset();
    sleep_ms(5);
    ms5607_prom_read();
    
    while(1) {
        //ms5607_reset();
        /*
        i2c_write_blocking(MS5607_PORT,addr,0b11111100,1,false);
        sleep_ms(1000);
        */
        sleep_ms(500);
        ms5607_D1_conversion();
        ms5607_D2_conversion();
        ms5607_convert(); 
    }

}