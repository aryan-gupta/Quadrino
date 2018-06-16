// Code in her is largely based on 
// https://embedds.com/programming-avr-I2C_-interface/

void I2C_Init() {
    TWSR = 0x00; // no prescaler
    TWBR = 0x00; //set SCL to max
	
    TWCR = (1 << TWEN);
}

void I2C_Start() {
	// Clear INT bit, Start the comm., and enable the I2C_Init
	// The TWEN is only there because we are using = and not |=
	static const uint8_t CTRL = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    TWCR = CTRL;
	// Wait for the hardware to set the INT bit (sent start bit)
    while ((TWCR & (1 << TWINT)) == 0);
}

void I2C_Stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void I2C_Write(uint8_t u8data) {
    TWDR = u8data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0);
}

uint8_t I2C_ReadACK() {
	// Set ack (TWEA) bit on
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while ((TWCR & (1 << TWINT)) == 0);
    return TWDR;
}

uint8_t I2C_ReadNACK() {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0);
    return TWDR;
}

uint8_t I2C_GetStatus() {
	// There was error checking
	// but got removed for speed reasons
	// the time saveings were about 50us
    uint8_t status = TWSR & 0b11111000; // strip prescaler bits
    return status;
}

int I2C_WriteReg(uint8_t addr, uint8_t reg, uint8_t data) {
	I2C_Start();
	I2C_Write((addr << 1) | 0);
	I2C_Write(reg);
	I2C_Write(data);
	I2C_Stop();
	return 0;
}

int I2C_ReadReg(uint8_t addr, uint8_t reg, uint8_t *data) {
	I2C_Start();
	I2C_Write((addr << 1) | 0);
	I2C_Write(reg);
	I2C_Stop();
	I2C_Start();
	I2C_Write((addr << 1) | 1);
	*data = I2C_ReadNACK();
	I2C_Stop();
	return 0;
}

int I2C_ReadMulReg(uint8_t addr, uint8_t reg, size_t len, uint8_t *data) {
	--len; // THis is being done so it makes the 
	// algorithms below alot easier to implement
	
	I2C_Start();
	I2C_Write((addr << 1) | 0);
	I2C_Write(reg);
	I2C_Stop();
	
	I2C_Start();
	I2C_Write((addr << 1) | 1);
	for (int ii = 0; ii < len; ++ii)
		data[ii] = I2C_ReadACK();
	data[len] = I2C_ReadNACK();
	I2C_Stop();
	return 0;
}

int I2C_ReadReg16(uint8_t addr, uint8_t sreg, uint16_t *data) {
	uint8_t tmph, tmpl;
	int tmp;
	
	I2C_ReadReg(addr, sreg++, &tmph);
	I2C_ReadReg(addr, sreg, &tmpl);
	
	*data = (uint16_t(tmph) << 8) | tmpl;
	return 0;
}

#ifndef FLIGHT_CTRL

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

#endif