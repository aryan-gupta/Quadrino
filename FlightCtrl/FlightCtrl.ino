// By using out own wire library, I cut down the communication
// time by almost 130us
// #include <Wire.h>

// https://www.youtube.com/watch?v=IdL0_ZJ7V2s

const short MPU = 0b1101000;

bool ch1, ch2, ch3, ch4;
int recv_ch1, recv_ch2, recv_ch3, recv_ch4;
uint16_t timer1, timer2, timer3, timer4;

uint16_t escfr, escfl, escbr, escbl;

uint16_t *escodr[4];

float gxo, gyo, gzo;
float ax, ay, az;
float gx, gy, gz;

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

ISR(PCINT0_vect) {
	//unsigned long tmp = micros();
	uint16_t tmp = TCNT1; // this is a 16bit value
	
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
	
	// Timer1
	TCCR1A = 0;
	TCCR1B = (1 << CS11); // turn off the prescaler (20.14.2 pg173)
	TCCR1C = 0;
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
	escfr = escfl = escbr = escbl = 0;
	
	DDRD |= 0b11110000; // set pins 4:7 as output
	
	setupInt();
	setupTimer();
	setupI2C();
	setupMPU6050();
	
	calibrateGyro();
	update_MPU_data(); // We want to update the gyro data once before we enter the loop
}

void update_MPU_data() {
	uint8_t rdata[14];
	int16_t mpu[7];
	
	I2CReadMulReg(MPU, 0x3B, 14, rdata);
	
	for (int ii = 0; ii < 7; ++ii) {
		size_t idx = ii * 2;
		mpu[ii] = (rdata[idx] << 8) | rdata[idx + 1];
	}
	
	const double af = 16384.0;
	ax = mpu[0] / af;
	ay = mpu[1] / af; 
	az = mpu[2] / af;
	
	const double gf = 131.0;
	gx = (mpu[4] / gf) + gxo;
	gy = (mpu[5] / gf) + gyo; 
	gz = (mpu[6] / gf) + gzo;
}

void serial_print_all() {
	Serial.print(recv_ch1);
	Serial.print('\t');
	Serial.print(recv_ch2);
	Serial.print('\t'); 
	Serial.print(recv_ch3);
	Serial.print('\t');
	Serial.print(recv_ch4);
	Serial.print('\t');
	
	// Serial.print(elapse);
	Serial.print('\t');
	Serial.print(gx);
	Serial.print('\t');
	Serial.print(gy);
	Serial.print('\t');
	Serial.print(gz);
	Serial.print('\t');
	
	Serial.print(ax);
	Serial.print('\t');
	Serial.print(ay);
	Serial.print('\t');
	Serial.print(az);
	Serial.print('\t');
}

void loop() {
	uint16_t loop_timer = TCNT1;
	//serial_print_all();

	// update_MPU_data();
	
	escfr = recv_ch1;
	escfl = recv_ch2;
	escbr = recv_ch3;
	escbl = recv_ch4;
	
	// uint16_t* s;
	// uint16_t* ss;
	
	// if (escfr < escfl) {
		// s = &escfr;
	// }
	
	// if ()

	PORTD |= B11110000;
	
	uint16_t curTime = TCNT1;
	uint16_t tch1 = escfr + curTime;
	uint16_t tch2 = escfl + curTime;
	uint16_t tch3 = escbr + curTime;
	uint16_t tch4 = escbl + curTime;
	
	while (TCNT1 - loop_timer < 1900); // wait 2000 ticks (a little less)
	
	while (PORTD >= 16) {
		curTime = TCNT1;
		
		if (tch1 <= TCNT1) PORTD &= 0b11101111;
		if (tch2 <= TCNT1) PORTD &= 0b11011111;
		if (tch3 <= TCNT1) PORTD &= 0b10111111;
		if (tch4 <= TCNT1) PORTD &= 0b01111111;
	}
	
	// set all esc values as high
	// uint16_t current_time = TCNT1;
	
	
	// if (escfr < escfl) {
		// tmpsort[0] = &escfr;
		// tmpsort[1] = &escfl;
	// } else {
		// tmpsort[1] = &escfr;
		// tmpsort[0] = &escfl;
	// }
	
	// if (escbr < escbl) {
		// tmpsort[2] = &escbr;
		// tmpsort[3] = &escbl;
	// } else {
		// tmpsort[3] = &escbr;
		// tmpsort[2] = &escbl;
	// }
	
	// if (*tmpsort[0] < *tmpsort[2]) {
		// escodr[0] = tmpsort[0];
		// if (*tmpsort[])
	// } else {
		// escodr[0] = tmpsort[2];
	// }
	
	Serial.println(" ");
	delay(1);
}

