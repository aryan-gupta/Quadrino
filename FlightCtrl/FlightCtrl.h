const uint16_t PHASE1_TICKS = 100;
const uint16_t PHASE2_TICKS = 4000 + 50;
const uint16_t PHASE3_TICKS = 4000 + 50;

const unsigned long BAUD_RATE = 115200;

const float KP = 0;
const float KI = 0;
const float KD = 0;

float pid_i_roll = 0, pid_i_pitch = 0, pid_i_yaw = 0;
float prev_roll_error = 0, prev_pitch_error = 0, prev_yaw_error = 0;

float gxo = 0, gyo = 0, gzo = 0;
float anglex = 0, angley = 0, anglez = 0;

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
	// TIMSK1 |= 0b1; // enable interrupt
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
void setup_MPU6050() {
	I2CWriteReg(MPU, 0x6B, 0x00); 
	I2CWriteReg(MPU, 0x1B, 0x00); 
	I2CWriteReg(MPU, 0x1C, 0x00);
}

void calibrate_gyro() {
	int64_t gxa, gya, gza;
	gxa = gya = gza = 0;

	const uint16_t SAMPLES = 5000;
	for (uint16_t ii = 0; ii < SAMPLES; ++ii) {
		uint8_t rdata[6];
		int16_t mpu[3];
		
		I2CReadMulReg(MPU, 0x43, 6, rdata);
		
		for (uint8_t jj = 0; jj < 3; ++jj) {
			size_t idx = jj * 2;
			mpu[jj] = (rdata[idx] << 8) | rdata[idx + 1];
		}

		gxa += mpu[0];
		gya += mpu[1]; 
		gza += mpu[2];
	}
	
	const float gf = 131.0 * SAMPLES;
	gxo = -(gxa / gf);
	gyo = -(gya / gf);
	gzo = -(gza / gf);
}

void setup_I2C() {
    TWSR = 0x00; // no prescaler
    TWBR = 0x00; //set SCL to max
	
    TWCR = (1 << TWEN); // Enable TWI
}

void update_MPU_data() {
	const float AF = 0.4;
	const float OVF = 0.8;
	
	const float GF = 1 - AF;
	const float NVF = 1 - 0.7;
	
	uint8_t rdata[14];
	int16_t mpu[7];
	
	I2CReadMulReg(MPU, 0x3B, 14, rdata);
	
	for (uint8_t ii = 0; ii < 7; ++ii) {
		size_t idx = ii * 2;
		mpu[ii] = (rdata[idx] << 8) | rdata[idx + 1];
	}
	
	const float af = 16384.0;
	float ax = mpu[0] / af;
	float ay = mpu[1] / af;
	float az = mpu[2] / af;
	
	const float gf = 131.0;
	float gx = (mpu[4] / gf) + gxo;
	float gy = (mpu[5] / gf) + gyo;
	float gz = (mpu[6] / gf) + gzo;
	
	gx = GF * gx * loop_elapsed;
	gy = GF * gy * loop_elapsed;
	gz = GF * gz * loop_elapsed;
	
	ax = AF * atan(ax / sqrt((ay * ay) + (az * az)));
	ay = AF * atan(ay / sqrt((ax * ax) + (az * az)));
	az = AF * atan(sqrt((ax * ax) + (az * az)) / az);
	
	anglex = (OVF * anglex) + (NVF * gx + ax);
	angley = (OVF * angley) + (NVF * gy + ay);
	anglez = (OVF * anglez) + (NVF * gz + az);
}

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
	ch_value = recv_ch1 - 3000; // map the values from -1000 to 1000
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
}
*/