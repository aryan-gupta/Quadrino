const uint16_t PHASE1_TICKS = 4000;
const uint16_t PHASE2_TICKS = 4000 + 50;
const uint16_t PHASE3_TICKS = 4000 + 50;

const unsigned long BAUD_RATE = 115200;

const float KP = 1;
const float KI = 1;
const float KD = 1;

enum RECV_CHANNELS : uint8_t {
	START,
	ROLL,
	PITCH,
	THROTTLE,
	YAW,
	VRA,
	VRB,
	SWA,
	SWB,
	SWC,
	SWD,
	// D1,
	// D2,
	// D3,
	// D4,
	CSUM = 15
};

uint16_t escfr = 0, escfl = 0, escbr = 0, escbl = 0;

float pid_i_roll = 0, pid_i_pitch = 0, pid_i_yaw = 0;
float prev_roll_error = 0, prev_pitch_error = 0, prev_yaw_error = 0;

float pid_roll = 0, pid_pitch = 0, pid_yaw = 0;

float loop_elapsed = 0;

void setup_timer() {
	// https://www.youtube.com/watch?v=IdL0_ZJ7V2s

	// Timer0
	TCCR0B = 0x0; // disable Timer0
	TIMSK0 = 0x0;
	TCNT0 = 0;
	
	// Timer1
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B = (1 << CS11); // turn the prescaler to 8 (20.14.2 pg173)
	TCCR1C = 0;
	TCNT1 = 0;
	
	// Timer2
	TCCR2B = 0x0; // disable Timer2
	TIMSK2 = 0x0;
	TCNT2 = 0;
}

void setup_pins() {
	// setup output pins
	DDRD |= 0b11110000; // set pins 4:7 as output
}

/*
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
	
	// ROLL
	ch_value = recv[ROLL] - 3000; // map the values from -1000 to 1000
	ch_value *= 0.04; // this will give us the degrees that the reciver should get
	
	error = anglex - ch_value;
	
	pid_p = KP * error;
	pid_i_roll += KI * error;
	pid_d = KD * (error - prev_roll_error)/loop_elapsed;
	
	pid_roll = pid_p + pid_i_roll + pid_d;
	
	prev_roll_error = error;
	
	// PITCH
	ch_value = recv_ch2 - 3000;
	ch_value *= 0.04;
	
	error = angley - ch_value;
	
	pid_p = KP * error;
	pid_i_pitch += KI * error;
	pid_d = KD * (error - prev_pitch_error)/loop_elapsed;
	
	pid_pitch = pid_p + pid_i_pitch + pid_d;
	
	prev_pitch_error = error;
	
	// YAW
	ch_value = recv_ch3 - 3000;
	ch_value *= 0.04;
	
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

*/