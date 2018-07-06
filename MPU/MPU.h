
const float GF = 0.98;
const float AF = 1 - GF;

const float gf = 131.0;
	
const uint8_t MPU = 0b1101000;

float gxo = 0, gyo = 0, gzo = 0;
float anglex = 0, angley = 0, anglez = 0;

void setup_MPU6050() {
	I2C_WriteReg(MPU, 0x6B, 0x00); 
	I2C_WriteReg(MPU, 0x1B, 0x00); 
	I2C_WriteReg(MPU, 0x1C, 0x00);
}

void setup_angle_vals() {
	uint8_t rdata[6];
	I2C_ReadMulReg(MPU, 0x3B, 6, rdata);
	
	float ax = ((rdata[0] << 8) | rdata[1]);
	float ay = ((rdata[2] << 8) | rdata[3]);
	float az = ((rdata[4] << 8) | rdata[5]);
	
	anglex = atan(ax / sqrt((ay * ay) + (az * az))) * (180/3.14159);
	angley = atan(ay / sqrt((ax * ax) + (az * az))) * (180/3.14159);
	// az = atan(sqrt((ax * ax) + (az * az)) / az) * (180/3.14159);
	anglez = 0;
}

void calibrate_gyro() {
	int64_t gxa, gya, gza;
	gxa = gya = gza = 0;

	const uint16_t SAMPLES = 5000;
	for (uint16_t ii = 0; ii < SAMPLES; ++ii) {
		uint8_t rdata[6];
		int16_t mpu[3];
		
		I2C_ReadMulReg(MPU, 0x43, 6, rdata);
		
		for (uint8_t jj = 0; jj < 3; ++jj) {
			size_t idx = jj * 2;
			mpu[jj] = (rdata[idx] << 8) | rdata[idx + 1];
		}

		gxa += mpu[0];
		gya += mpu[1]; 
		gza += mpu[2];
	}
	
	const float gf = SAMPLES; // * 131.0;
	gxo = -(gxa / gf);
	gyo = -(gya / gf);
	gzo = -(gza / gf);
	
	setup_angle_vals();
}

void update_MPU_data() { // This takes about 1983 ticks (max)
	uint8_t rdata[14];
	int16_t mpu[7];
	
	I2C_ReadMulReg(MPU, 0x3B, 14, rdata);
	
	for (uint8_t ii = 0; ii < 7; ++ii) {
		size_t idx = ii * 2;
		mpu[ii] = (rdata[idx] << 8) | rdata[idx + 1];
	}
	
	float ax_r = mpu[0]; // we dont need to convert to degrees because when
	float ay_r = mpu[1]; // we divide this later it will cross out
	float az_r = mpu[2];
	float gx_r = (mpu[4] + gxo) / gf;
	float gy_r = (mpu[5] + gyo) / gf;
	float gz_r = (mpu[6] + gzo) / gf;
	
	float gx = anglex + (gx_r * loop_elapsed);
	float gy = angley + (gy_r * loop_elapsed);
	float gz = anglez + (gz_r * loop_elapsed);
	
	float gxz_fix = gy * sin(gz_r * loop_elapsed * (3.14159/180));
	float gyz_fix = gx * sin(gz_r * loop_elapsed * (3.14159/180));
	
	gx += gxz_fix;
	gy -= gyz_fix;
	
	float ax = -atan(ay_r / sqrt((ax_r * ax_r) + (az_r * az_r))) * (180/3.14159);
	float ay =  atan(ax_r / sqrt((ay_r * ay_r) + (az_r * az_r))) * (180/3.14159);
	float az = 0; // atan(sqrt((ax * ax) + (az * az)) / az) * (180/3.14159);
	
	if (isnan(ay)) { ay = 0; }
	if (isnan(ax)) { ax = 0; }
	// if (isnan(az)) { az = 0; }
	
	anglex = (GF * gx) + (AF * ax);
	angley = (GF * gy) + (AF * ay);
	anglez = (GF * gz) + (AF * az);
}
