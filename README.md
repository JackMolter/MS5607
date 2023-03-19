# MS5607
Code for the MS5607 barometer on a breakout board that will be used during the BPP launch of SatFabs cubesat. This breakout board is replacing the populated barometer that isnt working. Code is written for the I2C peripheral and assumes that CS pin is held low and PS pin is held high. SDO pin is left open. Code is currently set to the maximum precision which allows for temp up to .01 C and pressure .01 mbar. 

Some info about the chip
The chip is very sensitive so be nice to it.
The PROM values are factory set and do not change
