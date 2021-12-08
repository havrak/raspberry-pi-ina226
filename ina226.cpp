#include "ina226.h"

//int fd;

INA226::INA226() {
	lastTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

bool INA226::attach(int i2cAddress){
	fd = wiringPiI2CSetup(i2cAddress);
	return fd >= 0;
}
unsigned short int INA226::read16(int fd, unsigned char ad){
	unsigned short int result = wiringPiI2CReadReg16(fd,ad);
	// Chip uses different endian
	return  (result<<8) | (result>>8);
}

void INA226::write16(int fd, unsigned char ad, unsigned short int value){
	// Chip uses different endian
	wiringPiI2CWriteReg16(fd,ad,(value<<8)|(value>>8));
}

// R of shunt resistor in ohm. Max current in Amp
void INA226::ina226_calibrate(float r_shunt, float max_current)
{
	current_lsb = max_current / (1 << 15);
	float calib = 0.00512 / (current_lsb * r_shunt);
	unsigned short int calib_reg = (unsigned short int) floorf(calib);
	current_lsb = 0.00512 / (r_shunt * calib_reg);

	//printf("LSB %f\n",current_lsb);
	//printf("Calib %f\n",calib);
	//printf("Calib R%#06x / %d\n",calib_reg,calib_reg);

	write16(fd,INA226_REG_CALIBRATION, calib_reg);
}

void INA226::ina226_configure(unsigned char bus, unsigned char shunt, unsigned char average, unsigned char mode)
{
	config = (average<<9) | (bus<<6) | (shunt<<3) | mode;
	write16(fd,INA226_REG_CONFIGURATION, config);
}

unsigned short int INA226::ina226_conversion_ready()
{
	return read16(fd,INA226_REG_MASK_ENABLE) & INA226_MASK_ENABLE_CVRF;
}

void INA226::ina226_wait(){
	unsigned char average = (config>>9) & 7;
	unsigned char bus = (config>>6) & 7;
	unsigned char shunt = (config>>3) & 7;

	unsigned int total_wait = (wait[bus] + wait[shunt] + (average ? avgwaits[bus>shunt ? bus : shunt] : 0)) * averages[average];

	usleep(total_wait+1000);

	int count=0;
	while(!ina226_conversion_ready()){
		count++;
	}
	//printf("%d\n",count);
}

void INA226::ina226_read(float *voltage, float *current, float *power, float* shunt_voltage)
{
	if (voltage) {
		unsigned short int voltage_reg = read16(fd,INA226_REG_BUS_VOLTAGE);
		*voltage = (float) voltage_reg * 1.25e-3;
	}

	if (current) {
		int16_t current_reg = (int16_t) read16(fd,INA226_REG_CURRENT);
		*current = (float) current_reg * 1000.0 * current_lsb;
	}

	if (power) {
		int16_t power_reg = (int16_t) read16(fd,INA226_REG_POWER);
		*power = (float) power_reg * 25000.0 * current_lsb;
	}

	if (shunt_voltage) {
		int16_t shunt_voltage_reg = (int16_t) read16(fd,INA226_REG_SHUNT_VOLTAGE);
		*shunt_voltage = (float) shunt_voltage_reg * 2.5e-3;
	}
}

 void INA226::ina226_reset()
{
	write16(fd, INA226_REG_CONFIGURATION, config = INA226_RESET);
}

 void INA226::ina226_disable()
{
	write16(fd, INA226_REG_CONFIGURATION, config = INA226_MODE_OFF);
}

