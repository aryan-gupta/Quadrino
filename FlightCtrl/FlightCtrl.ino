
#include "C:/Users/Aryan/Projects/Quadrino/FlightCtrl/FlightCtrl.h"
// #include "C:/Users/Aryan/Projects/Quadrino/I2C/I2C.h"
#include "C:/Users/Aryan/Projects/Quadrino/iBus/iBus.h"
#include "C:/Users/Aryan/Projects/Quadrino/PWMo/PWMo.h"


void setup() {
	Serial.begin(115200);
	Serial.print("\n\n\n\n");
	setup_pins();
	setup_timer();
	// setup_I2C();
	// setup_MPU6050();
	setup_recv(115200);
	calibrate_escs();
	output_empty_pulse();
}

void loop() {
	// Phase -- P1   >> P2   >> P3
	// Ticks -- 4500 >> 4500 >> 4500
	// Int   -- iBus >> ESC1 >> ESC2
	// Exec  -- iBus >> MPU  >> PID
	uint16_t loop_start = TCNT1, s, e;
	
	/// ================ PHASE 1 =============================
	s = TCNT1;
	e = s + PHASE1_TICKS;
		process_usart_data();
	while (TCNT1 < e);
	
	/// ================ PHASE 2 =============================
	s = TCNT1;
	e = s + PHASE2_TICKS;
	output_esc_pulse(ESC_FL_DOWN, recv[3]*2, ESC_FR_DOWN, recv[3]*2);
	//	update_MPU_data();
	finish_esc_pulse();
	while (TCNT1 < e);
	
	/// ================ PHASE 2 =============================
	s = TCNT1;
	output_esc_pulse(ESC_BL_DOWN, recv[3]*2, ESC_BR_DOWN, recv[3]*2);
	//	update_pid_calc();
	finish_esc_pulse();
	e = s + PHASE3_TICKS;
	while (TCNT1 < e);
	
	loop_elapsed = (TCNT1 - loop_start) / 2000000.0;
}