#ifndef __READ_SENSORS_H__
#define __READ_SENSORS_H__

typedef enum {
  VCCPSINTFP = 0,
  VCCPSINTLP,
  VCCPSAUX,
  VCCPSPLL,
  MGTRAVCC,
  MGTRAVTT,
  V2_0046,
  VCCOPS,
  VCCOPS3,
  VCCPSDDRPLL ,
 // PL
  VCCINT,
  VCCBRAM,
  VCCAUX,
  VCC1V2,
  VCC3V3,
  VADJ_FMC,
  MGTAVCC,
  MGTAVTT
} rails;


//int zcu102_open_device();
int zcu102_get_power(rails r, unsigned long int *power_value);
//int zcu102_close_device();

#endif //__READ_SENSORS_H__
