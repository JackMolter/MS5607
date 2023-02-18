// values 
#define MS5607_id 0b1110111 // check as this is determined by the CSB pin 
#define MS5607_BAUD 112800
#define MS5607_PORT i2c0 // check

// pins
#define MS5607_SDA 8
#define MS5607_SCL 9

// commands
#define MS5607_RESET 0b00011110
#define MS5607_PROM 0b10100110

#define MS5607_CONVERT 0b01001000
#define MS5607_ADC 0b00000000


// functions 
void ms5607_init();
void ms5607_reset();
void ms5607_prom_read();
void ms5607_conversion();
