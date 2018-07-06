

#include "C:/Users/Aryan/Projects/Quadrino/FlightCtrl/FlightCtrl.h"
#include "C:/Users/Aryan/Projects/Quadrino/I2C/I2C.h"
#include "MPU.h"

void setup() {
	Serial.begin(BAUD_RATE);
	//Serial.print("\n\n\n\n=");
	setup_timer();
	setup_I2C();
	setup_MPU6050();
	Serial.print("=");
	calibrate_gyro();
}


void loop() {
	TCNT1 = 0;
	uint16_t loop_start = TCNT1, s, e;
	
	/// ================ PHASE 1 =============================
	s = TCNT1;
	e = s + PHASE1_TICKS;
	while (TCNT1 < e);
	
	/// ================ PHASE 2 =============================
	s = TCNT1;
		update_MPU_data();
	uint16_t f = TCNT1 - s;
	e = s + PHASE2_TICKS;
	while (TCNT1 < e);
	
	/// ================ PHASE 2 =============================
	s = TCNT1;
		//Serial.print(f);
		//Serial.print('\t');
		Serial.print(tmp_gx);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print(tmp_ax);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print(tmp_gy);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print(tmp_ay);
		Serial.println(' ');
	e = s + PHASE3_TICKS;
	while (TCNT1 < e);
	
	loop_elapsed = (TCNT1 - loop_start) / 2000000.0;
}