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