#include "I2C.h"
// By using out own wire library, I cut down the communication
// time by almost 130us
const short MPU = 0b1101000;

void setup() {
	Serial.begin(9600);
	// Setup MPU
	I2C_Init();
	
	int tmp = 0;
	
	tmp = I2C_WriteReg(MPU, 0x6B, 0x00);
	if (tmp != 0) Serial.println(I2C_GetStatus()); 
	tmp = I2C_WriteReg(MPU, 0x1B, 0x00);
	if (tmp != 0) Serial.println(I2C_GetStatus()); 
	tmp = I2C_WriteReg(MPU, 0x1C, 0x00);
	if (tmp != 0) Serial.println(I2C_GetStatus());
	// Serial.println("\nSetup Done");
}

void loop() {
	uint8_t rdata[14];
	int16_t mpu[7];
	
	unsigned long start = micros();
	
	I2C_ReadMulReg(MPU, 0x3B, 14, rdata);
	
	for (int ii = 0; ii < 7; ++ii) {
		size_t idx = ii * 2;
		mpu[ii] = (rdata[idx] << 8) | rdata[idx + 1];
	}
	
	float ax = mpu[0] / 16384.0;
	float ay = mpu[1] / 16384.0;
	float az = mpu[2] / 16384.0;
	float gx = mpu[4] / 131.0;
	float gy = mpu[5] / 131.0;
	float gz = mpu[6] / 131.0;
	
	unsigned long elapse = micros() - start;
	
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
	delay(1000);
}