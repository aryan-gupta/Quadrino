// Code in her is largely based on 
// https://embedds.com/programming-avr-i2c-interface/

void I2CInit() {
    TWSR = 0x00; // no prescaler
    TWBR = 0x00; //set SCL to max
	
    TWCR = (1 << TWEN);
}

void I2CStart() {
	// Clear INT bit, Start the comm., and enable the I2CInit
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
    TWDR = u8data;
	static const uint8_t CTRL = (1 << TWINT) | (1 << TWEN);
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

uint8_t I2CGetStatus() {
    uint8_t status = TWSR & 0b11111000; // strip prescaler bits
    return status;
}

int I2CWriteReg(uint8_t addr, uint8_t reg, uint8_t data) {
	I2CStart();
	I2CWrite((addr << 1) | 0);
	I2CWrite(reg);
	I2CWrite(data);
	I2CStop();
	return 0;
}

int I2CReadReg(uint8_t addr, uint8_t reg, uint8_t *data) {
	I2CStart();
	I2CWrite((addr << 1) | 0);
	I2CWrite(reg);
	I2CStop();
	I2CStart();
	I2CWrite((addr << 1) | 1);
	*data = I2CReadNACK();
	I2CStop();
	return 0;
}

int I2CReadMulReg(uint8_t addr, uint8_t reg, size_t len, uint8_t *data) {
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
	return 0;
}

int I2CReadReg16(uint8_t addr, uint8_t sreg, uint16_t *data) {
	uint8_t tmph, tmpl;
	int tmp;
	
	I2CReadReg(addr, sreg++, &tmph);
	I2CReadReg(addr, sreg, &tmpl);
	
	*data = (uint16_t(tmph) << 8) | tmpl;
	return 0;
}

void setup() {
	Serial.begin(9600);
	const short MPU = 0b1101000;
	// Setup MPU
	I2CInit();
	
	int tmp = 0;
	
	tmp = I2CWriteReg(MPU, 0x6B, 0x00);
	if (tmp != 0) Serial.println(I2CGetStatus()); 
	tmp = I2CWriteReg(MPU, 0x1B, 0x00);
	if (tmp != 0) Serial.println(I2CGetStatus()); 
	tmp = I2CWriteReg(MPU, 0x1C, 0x00);
	if (tmp != 0) Serial.println(I2CGetStatus());
	// Serial.println("\nSetup Done");
}

void loop() {
	const short MPU = 0b1101000;
	uint8_t rdata[14];
	int16_t mpu[7];
	
	unsigned long start = micros();
	
	I2CReadMulReg(MPU, 0x3B, 14, rdata);
	
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