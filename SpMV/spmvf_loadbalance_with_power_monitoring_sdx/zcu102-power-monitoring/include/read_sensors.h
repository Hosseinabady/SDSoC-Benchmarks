/* File: read_sensors.cpp
 *
 Copyright (c) [2016] [Mohammad Hosseinabady (mohammad@hosseinabady.com)]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
* This file has been written at University of Bristol
* for the ENPOWER project funded by EPSRC
*
* File name : read_sensors.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 31 August 2018
* blog: https://highlevel-synthesis.com/
*/

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
