#include <Wire.h>

// https://www.youtube.com/watch?v=IdL0_ZJ7V2s

const short MPU = 0b1101000;

bool ch1, ch2, ch3, ch4;
int recv_ch1, recv_ch2, recv_ch3, recv_ch4;
uint16_t timer1, timer2, timer3, timer4;

float gxo, gyo, gzo;
float ax, ay, az;
float gx, gy, gz;

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
	Wire.beginTransmission(MPU);
	Wire.write(0x6B); // Power register
	Wire.write(0); // Clear it out so we dont fall asleep
	Wire.endTransmission();

	Wire.beginTransmission(MPU);
	Wire.write(0x1B); // Gyro register
	Wire.write(0); // Set it to the lowest setting	(6.1 pg12)
	Wire.endTransmission();

	Wire.beginTransmission(MPU);
	Wire.write(0x1C);
	Wire.write(0); // Set it to the lowest setting	(6.2 pg13)
}

void calibrateGyro() {
	long long gxa, gya, gza; // We can use double here
	// trade in performance for accuracy because we 
	// are in setup mode 
	gxa = gya = gza = 0;

	const int SAMPLES = 5000;
	for (int ii = 0; ii < SAMPLES; ++ii) {
		Wire.beginTransmission(MPU);
		Wire.write(0x43); //Request gyro data
		Wire.endTransmission();
		
		Wire.requestFrom(MPU,6);
		while(Wire.available() < 6);

		int tmpx, tmpy, tmpz;
		
		tmpx = (Wire.read() << 8) | Wire.read();		
		tmpy = (Wire.read() << 8) | Wire.read();
		tmpz = (Wire.read() << 8) | Wire.read();

		gxa += tmpx;
		gya += tmpy; 
		gza += tmpz;
	}

	gxo = -(gxa / 131.0 / SAMPLES);
	gyo = -(gya / 131.0 / SAMPLES);
	gzo = -(gza / 131.0 / SAMPLES);
}

void setup() {
	Serial.begin(9600);
	// Reset everything to 0
	ch1 = ch2 = ch3 = ch4 = false;
	recv_ch1 = recv_ch2 = recv_ch3 = recv_ch4 = 0;
	timer1 = timer2 = timer3 = timer4 = 0;
	
	PCICR	|= (1 << PCIE0); // enable interupts on pins 7:0 (17.2.4 pg 92) 
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3); // Turn on interupts on pins 0:3 (17.2.8 pg96)

	TCCR1A = 0;
	TCCR1B = (1 << CS10); // turn off the prescaler (20.14.2 pg173)
	TCCR1C = 0;

	Wire.begin();
	TWBR = 0; // We want to max out the I2C speed
	// TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
	
	setupMPU6050();

	calibrateGyro();
}

void read_process_MPU_data() {
	long tmpax, tmpay, tmpaz, tmpgx, tmpgy, tmpgz, tmpt;
	tmpax = tmpay = tmpaz = tmpgx = tmpgy = tmpgz = tmpt = 0;
	
	// The accel data is usually very accurate, no need to average
	Wire.beginTransmission(MPU);
	Wire.write(0x3B); //Request accel register data
	Wire.endTransmission();
	
	Wire.requestFrom(MPU, 6);
	while(Wire.available() < 6);
	
	tmpax += (Wire.read() << 8) | Wire.read();	
	tmpay += (Wire.read() << 8) | Wire.read();	
	tmpaz += (Wire.read() << 8) | Wire.read();
	
	const int SAMPLES = 1;
	for (int ii = 0; ii < SAMPLES; ++ii) {
		Wire.beginTransmission(MPU); //I2C address of the MPU
		Wire.write(0x43); //Starting register for Gyro Readings
		Wire.endTransmission();
	
		Wire.requestFrom(MPU,6); //Request Gyro Registers (43 - 48)
		while(Wire.available() < 6);
	
		tmpgx += (Wire.read() << 8) | Wire.read();	
		tmpgy += (Wire.read() << 8) | Wire.read();
		tmpgz += (Wire.read() << 8) | Wire.read();
	}
	
	const double af = 16384.0; // / SAMPLES;
	ax = tmpax / af;
	ay = tmpay / af; 
	az = tmpaz / af;
	
	const double gf = 131.0 * SAMPLES;
	gx = (tmpgx / gf) + gxo;
	gy = (tmpgy / gf) + gyo; 
	gz = (tmpgz / gf) + gzo;
}

void loop() {
	Serial.print(recv_ch1);
	Serial.print('\t');
	Serial.print(recv_ch2);
	Serial.print('\t'); 
	Serial.print(recv_ch3);
	Serial.print('\t');
	Serial.print(recv_ch4);
	Serial.print('\t');
	 
	int start = micros();
	read_process_MPU_data();
	int elapse = micros() - start;
	
	Serial.print(elapse);	
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
	
	Serial.println(" ");
	delay(250);
}

