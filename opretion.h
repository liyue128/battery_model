#pragma once
#ifndef _MATH_H

#define _MATH_H
#define NUM 4
double function1(float state_of_charge);
double function2(float state_of_charge);
double ChargingVoltage(double state_of_charge, double u, double cell_voltage[NUM], bool flag);
double ChargingCurrent(double cell_voltage[NUM], int count, int index, bool flag);
double Min(double data_member[NUM]);
double Max(double data_member[NUM]);
double Sum(double data_member[NUM]);

#endif // !_MATH_H

