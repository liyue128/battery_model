#pragma once
#ifndef _MATH_H

#include "charge.h"

#define _MATH_H
#define NUM 4

double function1(float state_of_charge);
double function2(float state_of_charge);
double ChargingVoltage(double cell_voltage[NUM], TypeOfStruct* charge_data);
double ChargingCurrent(double voltage[NUM], TypeOfStruct* charge_data);
double Min(double data_member[NUM]);
double Max(double data_member[NUM]);
double Sum(double data_member[NUM]);

#endif // !_MATH_H

