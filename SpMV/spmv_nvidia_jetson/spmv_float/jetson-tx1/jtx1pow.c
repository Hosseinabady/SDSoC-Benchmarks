/**
 * @file jtx1pow.C
 * @author cs
 * @brief This file contains implementation of the function 
 * for accessing on-board and on-module INA3221's values.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>    
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <byteswap.h>

#include "jtx1pow.h"

/**
 * @brief Voltage shunt monitors' registers
 */
static __u8 vshuntReg[] = {0x01, 0x03, 0x05};

/**
 * @brief Voltage bus monitors' registers
 */
static __u8 vbusReg[] = {0x02, 0x04, 0x06};

/**
 * @brief Resistors' values in milli-ohms
 */
static int rshunt[] = {20, 10, 10};

/**
 * @brief Read shunt voltage value
 * @param file e.g., located under path "/dev/i2c-%d" where "%d" is 1
 * @param reg e.g., vshuntReg[rail] where rail is indexed 
 * by ::jtx1_rail
 */
static int convShuntVol(int file, int reg){
  uint16_t v = __bswap_16(i2c_smbus_read_word_data(file, reg));
  return (int)((v >> 3) * 40);
}

/**
 * @brief Read bus voltage value
 * @param file e.g., located under path "/dev/i2c-%d" where "%d" is 1
 * @param reg e.g., vbusReg[rail] where rail is indexed by ::jtx1_rail
 */
static int convBusVol(int file, int reg){
  uint16_t v = __bswap_16(i2c_smbus_read_word_data(file, reg));
  return (int)((v >> 3) * 8);
}

/**
 * @brief Calculate current using voltage and resistor values
 * @param v e.g., convShuntVol(file, vshuntReg[rail])
 * @param r e.g., rshunt[rail] where rail is indexed by ::jtx1_rail
 */
static int calcCurr(int v, int r) {
  return (int)(v/(float)r);
}

/**
 * @brief Calculate power using voltage and current values
 * @param v e.g., vbusReg[rail])
 * @param i e.g., calcCurr(convShuntVol(file, vshuntReg[rail]), rshunt[rail])
 */ 
static int calcPow(int v, int i) {
  return (int)(v*i/1000);
}

/**
 * @brief Read INA3221's values from 0x42 and 0x43 addresses using sysfs files
 * 
 * Use sysf files to access on-board INA3221 sensor 
 * and read power, current, and voltage information.
 *
 * @param rail Indexed by ::jtx1_rail
 * @param measure Either VOLTAGE, POWER or CURRENT. See ::jtx1pow_ina3321_measure
 * @param *val Output's reference
 */
static void jtx1_get_ina3221_sysf(jtx1_rail rail,
				  jtx1_rail_type measure,
				  unsigned int *val)
{
  FILE *fp;
  int addr;
  int ans;
  char buff[MAX_BUFF];
  char *mea = "voltage";
    
  if (rail >= 0 && rail <= 2) {
    addr = 0;
  } else if (rail >= 3 && rail <= 5) {
    addr = 2;
  } else if (rail >= 6 && rail <= 8) {
    addr = 3;
  } else {
    addr = 2;
  }

  switch (measure) {
  case 0: {
    mea = "voltage";
    break;
  }
  case 1: {
    mea = "power";
    break;
  }
  case 2: {
    mea = "current";
    break;
  }
  default:
    break;
  }

  snprintf(buff, sizeof(buff),
	   SYSFS_INA3321_PATH "/1-004%d/iio_device/in_%s" "%d" "_input",
	   addr, mea, rail % 2);

//  printf("buff = %s\n", buff);
  fp = fopen(buff, "r");

  if (fp == NULL) {
    fprintf(stderr, "Error opening file: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  } else if (!fscanf(fp, "%d", &ans)) {
    fprintf(stderr, "Error scanning the file: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  } else {
    fclose(fp);
  }

  *val = ans;

}

/**
 * @brief Read INA3221's values from 0x40 using userspace i2c communication
 * 
 * Use userspace I2C to access on-module INA3221 sensor
 * and read power, current, and voltage information.
 *
 * @param rail Indexed by ::jtx1_rail
 * @param *val Output's reference
 */
static int jtx1_get_ina3221_userspace_i2c(int rail, jtx1_rail_type measure, unsigned int *val)
{
  int file;
  int adapter_nr = 1;
  char filename[20];

  int addr = 0x40; 


  int vshunt;
  int vbus;
  int curr;
  int power;

  snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
 // printf("filename = %s ", filename);
  file = open(filename, O_RDWR);
  if (file < 0) {
    fprintf(stderr, "Error opening file: %s\n", strerror(errno));
    printf("Error opening file: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  
  if (ioctl(file, I2C_SLAVE, addr) < 0) {
    fprintf(stderr, "IOCTL error: %s\n", strerror(errno));
    printf("Error opening file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  /* sleep(1); */
//  printf(" rail = %d", rail);
//  printf(" vshunt = %d", vshuntReg[rail]);
  vshunt = convShuntVol(file, vshuntReg[rail]);
//  printf(" vbus = %d", vbusReg[rail]);
//	if (rail == 0)
		vbus = convBusVol(file, vbusReg[rail]);
//	else
//		vbus = 1000;
  
//  printf(" curr = %d\n", rshunt[rail]);
  curr = calcCurr(vshunt, rshunt[rail]);
  power = calcPow(vbus, curr);

  switch (measure) {
  case 0: {
    *val = vbus;
    break;
  }
  case 1: {
    *val = power;
    break;
  }
  case 2: {
    *val = curr;
    break;
  }
  default:
    break;
  }
//  *val = power;

  close(file);

  return 0;
}

/**
 * @brief Read on-board and on-module INA3221's values 
 * from 0x40, 0x42 and 0x43 addresses
 * 
 * Use sysf files to access on-board INA3221 sensor 
 * and userspace I2C to access on-module INA3221 sensor
 * and read power, current, and voltage information.
 *
 * @param rail Indexed by ::jtx1_rail
 * @param measure Either VOLTAGE, POWER or CURRENT. See ::jtx1pow_ina3321_measure
 * @param *val Output's reference
 */
void jtx1_get_ina3221(jtx1_rail rail,
		      jtx1_rail_type measure,
		      unsigned int *val)
{

  if (rail >= 0 && rail <= 2) {
    jtx1_get_ina3221_userspace_i2c(rail, measure, val);
  } else if (rail >= 3 && rail <= 8) {
    jtx1_get_ina3221_sysf(rail, measure, val);
  } else {}
  
}
