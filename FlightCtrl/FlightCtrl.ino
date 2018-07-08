
#include "C:/Users/Aryan/Projects/Quadrino/FlightCtrl/FlightCtrl.h"
#include "C:/Users/Aryan/Projects/Quadrino/I2C/I2C.h"
#include "C:/Users/Aryan/Projects/Quadrino/MPU/MPU.h"
#include "C:/Users/Aryan/Projects/Quadrino/iBus/iBus.h"
#include "C:/Users/Aryan/Projects/Quadrino/PWMo/PWMo.h"

// Reserved Registers
// r2 - sreg for ISR
// r3 - iBus.h | process_usart_data() | sreg for atomic change
// r4 - PWMo.h | ISR(TIMER1_COMPA_vect, ISR_NAKED)
// r5 - PWMo.h | ISR(TIMER1_COMPA_vect, ISR_NAKED)

void function_a() {
	setup_I2C();
}

void setup() {
	//Serial.begin(BAUD_RATE);
	// ================= BASIC SETUP =======================
	setup_pins();
	setup_timer();
	//calibrate_escs_fl();
	//output_empty_pulse_fl();
	//calibrate_escs(function_a, setup_MPU6050, calibrate_gyro);
	//output_empty_pulse(enable_usart_int, finish_gyro_cal, setup_angle_vals);
	// calibrate_escs(dummy, dummy, dummy);
	// output_empty_pulse(dummy, dummy, dummy);
	// function_a();
	// setup_MPU6050();
	// for (uint16_t ii = 0; ii < 5000; ++ii) {
		// calibrate_gyro();
	// }
	// finish_gyro_cal();
	// setup_angle_vals();
	//enable_usart_int();
	// ================= Calibration =====================
	escfl = escfr = escbl = escbr = 4000;
	do_all_phases(setup_I2C, setup_MPU6050, calibrate_gyro);
	for (uint16_t ii = 0; ii < 750; ++ii) {
		do_all_phases(dummy, calibrate_gyro, calibrate_gyro);
	}
	for (uint16_t ii = 4000; ii >= 2000; ii -= 5) {
		escfl = escfr = escbl = escbr = ii;
		do_all_phases(dummy, calibrate_gyro, calibrate_gyro);
	}
	escfl = escfr = escbl = escbr = 2000;
	do_all_phases(dummy, finish_gyro_cal, setup_angle_vals);
	do_all_phases(dummy, dummy, []() { setup_recv(BAUD_RATE); });
	for (uint16_t ii = 0; ii < 750; ++ii) {
		do_all_phases(dummy, dummy, dummy);
	}
}

void loop_phase1() {
	process_usart_data();
	
	if (recv[THROTTLE] < 1000) 
		recv[THROTTLE] = 1000;
	else if (recv[THROTTLE] > 2000)
		recv[THROTTLE] = 2000;
	
	escfl = escfr = escbl = escbr = recv[THROTTLE] * 2;
}

void loop() {
	do_all_phases(loop_phase1, update_MPU_data, update_pid_calc);
}

void calibrate_escs_fl() {
	uint16_t e, s;
	TCNT1 = 0;
	
	s = TCNT1;
	e = s + PHASE1_TICKS;
	while (TCNT1 < e);
	
	s = TCNT1;
		output_esc_pulse(ESC_FL_DOWN, 4000, ESC_FR_DOWN, 4000);
			setup_recv(BAUD_RATE);
			setup_I2C();
		finish_esc_pulse();
	e = s + PHASE2_TICKS;
	while (TCNT1 < e);
	
	s = TCNT1;
	// Serial.println(s);
		output_esc_pulse(ESC_BL_DOWN, 4000, ESC_BR_DOWN, 4000);
			setup_MPU6050();
		finish_esc_pulse();
	// Serial.println(TCNT1);
	e = s + PHASE3_TICKS;
	while (TCNT1 < e);
	
	for (uint16_t ii = 0; ii < 750; ++ii) {
		TCNT1 = 0;
		s = TCNT1;
		e = s + PHASE1_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
			output_esc_pulse(ESC_FL_DOWN, 4000, ESC_FR_DOWN, 4000);
				calibrate_gyro();
			finish_esc_pulse();
		e = s + PHASE2_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
			output_esc_pulse(ESC_BL_DOWN, 4000, ESC_BR_DOWN, 4000);
				calibrate_gyro();
			finish_esc_pulse();
		e = s + PHASE3_TICKS;
		while (TCNT1 < e);
	}
	
	for (uint16_t ii = 4000; ii >= 2000; ii -= 5) {
		TCNT1 = 0;
		s = TCNT1;
		e = s + PHASE1_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
			output_esc_pulse(ESC_FL_DOWN, ii, ESC_FR_DOWN, ii);
				//fc();
			finish_esc_pulse();
		e = s + PHASE2_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
			output_esc_pulse(ESC_BL_DOWN, ii, ESC_BR_DOWN, ii);
				//fc();
			finish_esc_pulse();
		e = s + PHASE3_TICKS;
		while (TCNT1 < e);
	}
}

void output_empty_pulse_fl() {
	uint16_t e, s;
	TCNT1 = 0;
	
	s = TCNT1;
		//enable_usart_int();
	e = s + PHASE1_TICKS;
	while (TCNT1 < e);
	
	s = TCNT1;
		output_esc_pulse(ESC_FL_DOWN, 2000, ESC_FR_DOWN, 2000);
			finish_gyro_cal();
		finish_esc_pulse();
	e = s + PHASE2_TICKS;
	while (TCNT1 < e);
	
	s = TCNT1;
		output_esc_pulse(ESC_BL_DOWN, 2000, ESC_BR_DOWN, 2000);
			setup_angle_vals();
		finish_esc_pulse();
	e = s + PHASE3_TICKS;
	while (TCNT1 < e);
	
	for (uint16_t ii = 0; ii < 750; ++ii) {
		TCNT1 = 0;
		
		s = TCNT1;
		e = s + PHASE1_TICKS;
		// Do somthing productive here
		while (TCNT1 < e);
		
		s = TCNT1;
		output_esc_pulse(ESC_FL_DOWN, 2000, ESC_FR_DOWN, 2000);
		// Do somthing productive here
		finish_esc_pulse();
		e = s + PHASE2_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
		output_esc_pulse(ESC_BL_DOWN, 2000, ESC_BR_DOWN, 2000);
		// Do somthing productive here
		finish_esc_pulse();
		e = s + PHASE3_TICKS;
		while (TCNT1 < e);
	}
}

// Im keeping this here now, will be moved later
void update_pid_calc() {
	// the recv_ch? calues are between 2000 and 4000
	// where 3000 is the center stick value
	// Channel1 = Roll
	// Channel2 = Pitch
	// Channel3 = Yaw
	// Channel4 = Throttle
	// The total region we want to map is from -40 degrees below the axis
	// and 40 degrees above the axis. this is 80 degrees we need to map 
	// to the value from -1000 to 1000 
	// 80 / 2000 = 0.04 this means that each tick of the reciver tick is equal
	// to 0.04 degrees of tilt
	
	float ch_value, error, pid_p, pid_d;
	const float ANGLE_FACTOR = 0.02;
	
	// ROLL
	ch_value = recv[ROLL] - 1500; // map the values from -500 to 500
	ch_value *= ANGLE_FACTOR; // this will give us the degrees that the reciver should get
	
	error = anglex - ch_value;
	
	pid_p = KP * error;
	pid_i_roll += KI * error;
	pid_d = KD * (error - prev_roll_error)/loop_elapsed;
	
	pid_roll = pid_p + pid_i_roll + pid_d;
	
	prev_roll_error = error;
	
	// PITCH
	ch_value = recv[PITCH] - 1500;
	ch_value *= ANGLE_FACTOR;
	
	error = angley - ch_value;
	
	pid_p = KP * error;
	pid_i_pitch += KI * error;
	pid_d = KD * (error - prev_pitch_error)/loop_elapsed;
	
	pid_pitch = pid_p + pid_i_pitch + pid_d;
	
	prev_pitch_error = error;
	
	// YAW
	ch_value = recv[YAW] - 1500;
	ch_value *= ANGLE_FACTOR;
	
	error = anglez - ch_value;
	
	pid_p = KP * error;
	pid_i_yaw += KI * error;
	pid_d = KD * (error - prev_yaw_error)/loop_elapsed;
	
	pid_yaw = pid_p + pid_i_yaw + pid_d;
	
	prev_yaw_error = error;
	
	// Final
	uint16_t throttle = recv[THROTTLE] * 2;
	
	escfr = throttle + pid_roll - pid_pitch - pid_yaw; // (front right CCW)
	escfl = throttle - pid_roll - pid_pitch + pid_yaw; // (font left CW)
	escbr = throttle + pid_roll + pid_pitch + pid_yaw; // (back right CW)
	escbl = throttle - pid_roll + pid_pitch - pid_yaw; // (back left CCW)
	
	if      (escfr < 2000) escfr = 2000;
	else if (escfr > 4000) escfr = 4000;
	
	if      (escfl < 2000) escfl = 2000;
	else if (escfl > 4000) escfl = 4000;
	
	if      (escbr < 2000) escbr = 2000;
	else if (escbr > 4000) escbr = 4000;
	
	if      (escbl < 2000) escbl = 2000;
	else if (escbl > 4000) escbl = 4000;
}