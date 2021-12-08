#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <chrono>

using namespace std::chrono;

using namespace std;

#ifndef INA226_H_
#define INA226_H_

class INA226 {
	private:
  milliseconds lastTime;

	void write16(int fd, unsigned char ad, unsigned short int value);
	unsigned short int read16(int fd, unsigned char ad);

	unsigned long long int config;
	float current_lsb;


	public:
	enum {
		INA226_REG_CONFIGURATION = 0x00,
		INA226_REG_SHUNT_VOLTAGE = 0x01,
		INA226_REG_BUS_VOLTAGE = 0x02,
		INA226_REG_POWER = 0x03,
		INA226_REG_CURRENT = 0x04,
		INA226_REG_CALIBRATION = 0x05,
		INA226_REG_MASK_ENABLE = 0x06,
		INA226_REG_ALERT_LIMIT = 0x07,
		INA226_REG_MANUFACTURER = 0xFE,
		INA226_REG_DIE_ID = 0xFF,
	};

#define INA226_RESET 0x8000
#define INA226_MASK_ENABLE_CVRF 0x0008

	enum {
		INA226_BIT_SHUNT = 0,
		INA226_BIT_BUS = 1,
		INA226_BIT_MODE = 2,
	};

#define INA226_MODE_SHUNT 1
#define INA226_MODE_BUS 2
#define INA226_MODE_TRIGGERED 0
#define INA226_MODE_CONTINUOUS 4

	enum {
		INA226_MODE_OFF = 0,
		INA226_MODE_SHUNT_TRIGGERED = 1,
		INA226_MODE_BUS_TRIGGERED = 2,
		INA226_MODE_SHUNT_BUS_TRIGGERED = 3,
		INA226_MODE_OFF2 = 4,
		INA226_MODE_SHUNT_CONTINUOUS = 5,
		INA226_MODE_BUS_CONTINUOUS = 6,
		INA226_MODE_SHUNT_BUS_CONTINUOUS = 7,
	};

	enum {
		INA226_TIME_01MS = 0, /* 140us */
		INA226_TIME_02MS = 1, /* 204us */
		INA226_TIME_03MS = 2, /* 332us */
		INA226_TIME_05MS = 3, /* 588us */
		INA226_TIME_1MS = 4,	/* 1.1ms */
		INA226_TIME_2MS = 5,	/* 2.115ms */
		INA226_TIME_4MS = 6,	/* 4.156ms */
		INA226_TIME_8MS = 7,	/* 8.244ms */
	};

	enum {
		INA226_AVERAGES_1 = 0,
		INA226_AVERAGES_4 = 1,
		INA226_AVERAGES_16 = 2,
		INA226_AVERAGES_64 = 3,
		INA226_AVERAGES_128 = 4,
		INA226_AVERAGES_256 = 5,
		INA226_AVERAGES_512 = 6,
		INA226_AVERAGES_1024 = 7,
	};

	const unsigned short int averages[8] = { 1, 4, 16, 64, 128, 256, 512, 1024 };

	// Conservative to be done for all averages
	// const uint16_t wait[] = {142,206,336,595,1113,2142,4207,8346};

	// Minimum wait for 1 average
	const unsigned short int wait[8] = { 0, 0, 0, 0, 500, 1500, 3550, 7690 };

	// Time in us per iteration to calculate average for a given measure time
	const unsigned short int avgwaits[8] = { 300, 450, 700, 1200, 1250, 1300, 1300, 1320 };
	int fd;

	INA226();                           // construct function

	bool attach(int i2cAddress);
	void ina226_wait();
	void ina226_read(float *voltage, float *current, float *power, float* shunt_voltage);
	void ina226_reset();
	void ina226_disable();
	void ina226_calibrate(float r_shunt, float max_current);
	void ina226_configure(unsigned char bus, unsigned char shunt, unsigned char average, unsigned char mode);
	unsigned short int ina226_conversion_ready();

};

#endif /* INA226_H_ */
