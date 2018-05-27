// By using out own wire library, I cut down the communication
// time by almost 130us
// #include <Wire.h>

// https://www.youtube.com/watch?v=IdL0_ZJ7V2s

const short MPU = 0b1101000;

const float KP = 0;
const float KI = 0;
const float KD = 0;

float pid_i_roll = 0;
float pid_i_pitch = 0;
float pid_i_yaw = 0;

float prev_roll_error = 0;
float prev_pitch_error = 0;
float prev_yaw_error = 0;

bool ch1, ch2, ch3, ch4;
uint16_t recv_ch1, recv_ch2, recv_ch3, recv_ch4;
uint32_t timer1, timer2, timer3, timer4;

float gxo, gyo, gzo;
float anglex, angley, anglez;

float pid_roll, pid_pitch, pid_yaw;

bool cmpAesc, cmpBesc;
uint16_t cmpAoth, cmpBoth;

uint16_t T1_MSB = 0;
uint32_t loop_timer_prev = 0;
float loop_elapsed;


uint32_t debug_cmpA;

void I2CStart() {
	// Clear INT bit, Start the com, and enable the I2CInit
	// The TWEN is only there because we are using = and not |=
	static const uint8_t CTRL = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    TWCR = CTRL;
	// Wait for the hardware to set the INT bit (sent start bit)
    while ((TWCR & (1 << TWINT)) == 0);
}

void I2CStop() {
	static const uint8_t CTRL = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    TWCR = CTRL;
}

void I2CWrite(uint8_t u8data) {
	static const uint8_t CTRL = (1 << TWINT) | (1 << TWEN);
    TWDR = u8data;
    TWCR = CTRL;
    while ((TWCR & (1 << TWINT)) == 0);
}

uint8_t I2CReadACK() {
	// Set ack (TWEA) bit on
	static const uint8_t CTRL = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    TWCR = CTRL;
    while ((TWCR & (1 << TWINT)) == 0);
    return TWDR;
}

uint8_t I2CReadNACK() {
	static const uint8_t CTRL = (1 << TWINT) | (1 << TWEN);
    TWCR = CTRL;
    while ((TWCR & (1 << TWINT)) == 0);
    return TWDR;
}

void I2CWriteReg(uint8_t addr, uint8_t reg, uint8_t data) {
	I2CStart();
	I2CWrite((addr << 1) | 0);
	I2CWrite(reg);
	I2CWrite(data);
	I2CStop();
}

void I2CReadReg(uint8_t addr, uint8_t reg, uint8_t *data) {
	I2CStart();
	I2CWrite((addr << 1) | 0);
	I2CWrite(reg);
	I2CStop();
	I2CStart();
	I2CWrite((addr << 1) | 1);
	*data = I2CReadNACK();
	I2CStop();
}

void I2CReadMulReg(uint8_t addr, uint8_t reg, size_t len, uint8_t *data) {
	--len; // THis is being done so it makes the 
	// algorithms below alot easier to implement
	
	I2CStart();
	I2CWrite((addr << 1) | 0);
	I2CWrite(reg);
	I2CStop();
	
	I2CStart();
	I2CWrite((addr << 1) | 1);
	for (int ii = 0; ii < len; ++ii)
		data[ii] = I2CReadACK();
	data[len] = I2CReadNACK();
	I2CStop();
}

ISR(TIMER1_COMPA_vect) {
	// if cmpAesc is true then we want to bring the front left down
	if (cmpAesc) {
		PORTD &= 0b11011111; // turn off
		debug_cmpA = get_ticks();
	} else {
		PORTD &= 0b11101111;
	}
	
	if (PORTD & 0b00110000 == 0) { // if both are low then turn off this inturupt
		TIMSK1 &= 0b11111101;
	} else { // if we still have the other to deal with then set that inturupt up
		OCR1A = cmpAoth;
		cmpAesc = !cmpAesc;
	}
}

ISR(TIMER1_COMPB_vect) {
	// if cmpBesc is true then we want to bring the back left down
	if (cmpBesc) {
		PORTD &= 0b01111111; // turn off
	} else {
		PORTD &= 0b10111111;
	}
	
	if (PORTD & 0b11000000 == 0) { // if both are low then turn off this inturupt
		TIMSK1 &= 0b11111011;
	} else { // if we still have the other to deal with then set that inturupt up
		OCR1B = cmpBoth;
		cmpBesc = !cmpBesc;
	}
	
	//TIFR1 &= 0b11111101;
}

ISR(TIMER1_OVF_vect) {
	++T1_MSB;
	TIFR1 &= 0b11111110; // turn off the ov flag
}

ISR(PCINT0_vect) {
	// https://electronics.stackexchange.com/questions/120654/interrupt-management-large-avr-projects
	// Inturupts while in the ISR wont be skipped as long as it isnt
	// the same inturupt
	
	uint32_t tmp = get_ticks(); // this is a 16bit value
	
	// Channel 1 ========================
	if (ch1 == 0 and PINB & 0b01) {
		ch1 = 1;
		timer1 = tmp;
	} else if (ch1 == 1 and !(PINB & 0b01)) {
		ch1 = 0;
		recv_ch1 = tmp - timer1;
	}
	
	// Channel 2 ========================
	if (ch2 == 0 and PINB & 0b010) {
		ch2 = 1;
		timer2 = tmp;
	} else if (ch2 == 1 and !(PINB & 0b010)) {
		recv_ch2 = tmp - timer2;
		ch2 = 0;
	}

	// Channel 3 ========================
	if (ch3 == 0 and PINB & 0b0100) {
		ch3 = 1;
		timer3 = tmp;
	} else if (ch3 == 1 and !(PINB & 0b0100)) {
		recv_ch3 = tmp - timer3;
		ch3 = 0;
	}

	// Channel 4 ========================
	if (ch4 == 0 and PINB & 0b01000) {
		ch4 = 1;
		timer4 = tmp;
	} else if (ch4 == 1 and !(PINB & 0b01000)) {
		recv_ch4 = tmp - timer4;
		ch4 = 0;
	}
}

uint32_t get_ticks() {
	return (uint32_t(T1_MSB) << 16) | TCNT1;
}

void setupMPU6050() {
	I2CWriteReg(MPU, 0x6B, 0x00); 
	I2CWriteReg(MPU, 0x1B, 0x00); 
	I2CWriteReg(MPU, 0x1C, 0x00);
}

void calibrateGyro() {
	long long gxa, gya, gza;
	gxa = gya = gza = 0;

	const int SAMPLES = 5000;
	for (int ii = 0; ii < SAMPLES; ++ii) {
		uint8_t rdata[6];
		int16_t mpu[3];
		
		I2CReadMulReg(MPU, 0x43, 6, rdata);
		
		for (int jj = 0; jj < 3; ++jj) {
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

void setupInt() {
	PCICR  |= (1 << PCIE0); // enable interupts on pins 7:0 (17.2.4 pg 92) 
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3); // Turn on interupts on pins 0:3 (17.2.8 pg96)
}

void setupTimer() {
	// Timer0
	//TCCR0B = 0x0; // disable Timer0
	//TIMSK0 = 0x0;
	//TCNT0 = 0;
	
	// Timer1
	TCCR1A = 0;
	TCCR1B = (1 << CS11); // turn the prescaler to 8 (20.14.2 pg173)
	TCCR1C = 0;
	TIMSK1 |= 0b1;
	TCNT1 = 0;
	
	// Timer2
	// TCCR2B = 0x0; // disable Timer2
	// TIMSK2 = 0x0;
	// TCNT2 = 0;
}

void setupI2C() {
    TWSR = 0x00; // no prescaler
    TWBR = 0x00; //set SCL to max
	
    TWCR = (1 << TWEN); // Enable TWI
}

void setup() {
	Serial.begin(9600);
	
	// Reset everything to 0
	ch1 = ch2 = ch3 = ch4 = false;
	recv_ch1 = recv_ch2 = recv_ch3 = recv_ch4 = 0;
	timer1 = timer2 = timer3 = timer4 = 0;
	
	DDRD |= 0b11110000; // set pins 4:7 as output
	
	setupTimer();
	setupI2C();
	setupMPU6050();
	setupInt();
	
	
	calibrateGyro();
	//update_MPU_data(0); // We want to update the gyro data once before we enter the loop
}

void update_MPU_data() {
	const float AF = 0.4;
	const float OVF = 0.8;
	
	const float GF = 1 - AF;
	const float NVF = 1 - 0.7;
	
	uint8_t rdata[14];
	int16_t mpu[7];
	
	I2CReadMulReg(MPU, 0x3B, 14, rdata);
	
	for (int ii = 0; ii < 7; ++ii) {
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

void serial_print_all() {
	// Serial.print(recv_ch1);
	// Serial.print('\t');
	// Serial.print(recv_ch2);
	// Serial.print('\t'); 
	// Serial.print(recv_ch3);
	// Serial.print('\t');
	// Serial.print(recv_ch4);
	// Serial.print('\t');
	
	// // Serial.print(elapse);
	// Serial.print('\t');
	// Serial.print(gx);
	// Serial.print('\t');
	// Serial.print(gy);
	// Serial.print('\t');
	// Serial.print(gz);
	// Serial.print('\t');
	
	// Serial.print(ax);
	// Serial.print('\t');
	// Serial.print(ay);
	// Serial.print('\t');
	// Serial.print(az);
	// Serial.print('\t');
	
	Serial.print(anglex);
	Serial.print('\t');
	Serial.print(angley);
	Serial.print('\t');
	Serial.print(anglez);
	Serial.print('\t');
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

void update_loop_timer() {
	uint32_t loop_start = get_ticks();
	loop_elapsed = (loop_start - loop_timer_prev) / 2000000.0;
	loop_timer_prev = loop_start;
}

void post_esc_vals() {
	uint16_t throttle = recv_ch4;
	if (throttle > 3900) throttle = 3900; // we want a little it of leeway so our PID calculation
	// can still work
	
	// roll is calulated by left/right escs
	// pitch is calculate by the front/back escs
	// yaw is controlled by the CW/CCW escs
	uint16_t escfr = throttle + pid_roll - pid_pitch - pid_yaw; // (front right CCW)
	uint16_t escfl = throttle - pid_roll - pid_pitch + pid_yaw; // (font left CW)
	uint16_t escbr = throttle + pid_roll + pid_pitch + pid_yaw; // (back right CW)
	uint16_t escbl = throttle - pid_roll + pid_pitch - pid_yaw; // (back left CCW)
	
	//// @TODO battery calculations
	
	// we dont want to turn off any of the props
	if (escfr < 2050) escfr = 2050;
	if (escfl < 2050) escfr = 2050;
	if (escbr < 2050) escfr = 2050;
	if (escbl < 2050) escfr = 2050;
	
	// we also want to make sure that we dont esceed the PWM output limit
	if (escfr > 4000) escfr = 2050;
	if (escfl > 4000) escfr = 2050;
	if (escbr > 4000) escfr = 2050;
	if (escbl > 4000) escfr = 2050;
	
	// escfr = 3892;
	// escfl = 2312;
	// escbr = 3120;
	// escbl = 2030;
	
	escfr = 2700;
	escfl = 2100;
	escbr = 3500;
	escbl = 3900;
	
	uint32_t curTime = get_ticks();
	PORTD |= 0b11110000;
	uint32_t escfr_time = escfr + curTime;
	uint32_t escfl_time = escfl + curTime;
	uint32_t escbr_time = escbr + curTime;
	uint32_t escbl_time = escbl + curTime;
	
	/// Run a profiler here
	
	// The 16bit timer will loop back every 0.0327 seconds
	// (16M / 8) / 2^16 = overflows per second
	// There are 16 million ticks per second, the prescaler is 8
	// so there will be 2 million counts every second. this divided by 
	// the max timer1 is overflows per second
	
	// if the left esc will be pulled down first then cmpAesc is true
	// if right then cmpAesc is false
	
	// Compare A will handle the front l/r escs
	if (escfr_time < escfl_time) {
		cmpAesc = false;
		cmpAoth = escfl_time & 0xFFFF;
		OCR1A   = escfr_time & 0xFFFF; // We want the lower 16bits
	} else {
		cmpAesc = true;
		cmpAoth = escfr_time & 0xFFFF;
		OCR1A   = escfl_time & 0xFFFF; // We want the lower 16bits
	}
	
	// Compare B will hangle the back l/r escs
	if (escbr_time < escbl_time) {
		cmpBesc = false;
		cmpBoth = escbl_time & 0xFFFF;
		OCR1B   = escbr_time & 0xFFFF; // We want the lower 16bits
	} else {
		cmpBesc = true;
		cmpBoth = escbr_time & 0xFFFF;
		OCR1B   = escbl_time & 0xFFFF; // We want the lower 16bits
	}
	
	delay(1);
	
	// We want to enable compare interupt here
	TIFR1 &= 0b11111001;
	TIMSK1 |= 0b110;
	
	// Serial.println('H');
	// We want to do somthing useful here
	
	while (PORTD >= 16);
	
	// {
		// auto time = get_ticks();
		// if (escfr_time < time) PORTD &= 0b11101111;
		// if (escfl_time < time) PORTD &= 0b11011111;
		// if (escbr_time < time) PORTD &= 0b10111111;
		// if (escbl_time < time) PORTD &= 0b01111111;

	// } // wait for thhe signals to finish sending
	// We want to turn off the compare inturupt in case 
	// we loop back and hit it in while doing somthing else
	TIMSK1 &= 0b11111001;
	// Serial.println(get_ticks() - debug_cmpA);
}

void loop() {
	//auto start_micro = micros();
	
	uint32_t start;
	update_loop_timer();
	
	//start = get_ticks();
	update_MPU_data();
	//Serial.print((get_ticks() - start) / 2);
	//Serial.print('\t');
	
	//start = get_ticks();
	update_pid_calc();
	//Serial.print((get_ticks() - start) / 2);
	//Serial.print('\t');
	
	// start = get_ticks();
	// update_esc_val();
	// Serial.print(get_ticks() - start);
	// Serial.print('\t');
	
	//start = get_ticks();
	post_esc_vals();
	//Serial.print((get_ticks() - start) / 2);
	//Serial.print('\t');
	
	// Serial.print(loop_elapsed);
	// Serial.print('\t');
	
	//Serial.println("D");
	// uint32_t stoptime = get_ticks() + 1000;
	// while (get_ticks() < stoptime);
	delay(100);
}

