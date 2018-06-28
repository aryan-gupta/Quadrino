

#include "C:/Users/Aryan/Projects/Quadrino/FlightCtrl/FlightCtrl.h"
#include "C:/Users/Aryan/Projects/Quadrino/I2C/I2C.h"
#include "MPU.h"

void setup() {
	Serial.begin(BAUD_RATE);
	Serial.print("\n\n\n\n");
	setup_timer();
	setup_I2C();
	setup_MPU6050();
}


void loop() {
	
}