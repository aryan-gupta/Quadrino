

float gxo = 0, gyo = 0, gzo = 0;
float anglex = 0, angley = 0, anglez = 0;

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
