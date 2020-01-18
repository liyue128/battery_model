#include <iostream>
#include "opretion.h"
#include "charge.h"
#define VLOWV 12.4
#define VRECH 16.8
#define IPRE 0.203
#define INORMAL 3.11
#define ITERM 0.203
#define D1 0.02

double SOC[21] = { 0,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1 };
double VOC[21] = { 3.1285,3.3569,3.396,3.462,3.521,3.5566,3.585,3.6123,3.643,3.6869,3.7359,3.786,3.8326,3.8766,3.9196,3.969,4.028,4.0848,4.1196,4.1457,4.166 };
double R0[21] = { 0.122239389,0.09410802,0.079650846,0.076279662,0.072013093,0.071019369,0.07011003,0.070668486,0.06937625,0.068206621,\
		0.068655088,0.068376068,0.06792144,0.067836683,0.066842488,0.067557738,0.068285143,0.066842488,0.06847322,0.071371398,0.167451596 };
double cell_current[16] = { 3.11,2.812,2.511,2.211,1.912,1.61,1.311,1.011,0.7738,0.6495,0.5406,0.47,0.45001,0.327,0.243,0.203 };
double data_number[16] = { 0,70,140,227,327,564,670,978,1340,1438,1549,1657,1811,2057,2548,2966 };

double function1(float state_of_charge)
{
	int l = 0, r = 21;
	while (r - l > 1) {
		int mid = (l + r) / 2;
		if (state_of_charge == SOC[mid]) {
			break;
		}
		if (state_of_charge < SOC[mid]) {
			r = mid;
		}
		else {
			l = mid;
		}
	}
	double po = VOC[l] + (VOC[l + 1] - VOC[l]) / 0.05 * (state_of_charge - SOC[l]);
	return VOC[l] + (VOC[l + 1] - VOC[l]) / 0.05 * (state_of_charge - SOC[l]);
}

double function2(float state_of_charge)
{
	int l = 0, r = 21;
	while (r - l > 1) {
		int mid = (l + r) / 2;
		if (state_of_charge == SOC[mid]) {
			break;
		}
		if (state_of_charge < SOC[mid]) {
			r = mid;
		}
		else {
			l = mid;
		}
	}
	return R0[l] + (R0[l + 1] - R0[l]) / 0.05 * (state_of_charge - SOC[l]);
}

//计算充电电压,u表示充电电流
double ChargingVoltage(double state_of_charge, double u, double cell_voltage[NUM], bool flag)
{
	double f, h;
	double y, yd;
	double battery_voltage = Sum(cell_voltage);
	f = function1(state_of_charge);
	h = function2(state_of_charge);

	if (flag) {  //极端不平衡,检测电压需关闭所有旁路
		y = f + h * 0;
	}
	if (battery_voltage < VRECH) {  //恒流
		y = f + h * INORMAL;
	}
	else {  //恒压
		y = f + h * u;
	}

	return y;
}

double ChargingCurrent(double cell_voltage[NUM], int count, int index, bool flag)
{
	int n = 1;
	double battery_voltage = Sum(cell_voltage);
	double min = Min(cell_voltage);
	double max = Max(cell_voltage);
	double current;

	//extremly imbalance
	if (flag) {
		return CellBypass(cell_voltage[index], min);
	}

	//precharge
	if (battery_voltage < VLOWV) {
		current = IPRE;
	}
	//equal current
	else if (battery_voltage < VRECH) {
		if (max - min > D1) {
			current = INORMAL + CellBypass(cell_voltage[index], min);
		}
		else {
			current = INORMAL;
		}
	}
	//equal voltage
	else {
		//		count++;
		if (count < data_number[15]) {
			int l = 0, r = 16;
			while (r - l > 1) {
				int mid = (l + r) / 2;
				if (count == data_number[mid] / n) {
					l = mid;
					break;
				}
				if (count < data_number[mid] / n) {
					r = mid;
				}
				else {
					l = mid;
				}
			}
			if (count < data_number[15]) {
				current = cell_current[l] + (cell_current[l + 1] - cell_current[l]) / ((data_number[l + 1] - data_number[l]) / n) * (count - data_number[l] / n);
			}
		}
		else {
			current = ITERM;
		}
	}

	//debug
	double debug_var = current;
	if (debug_var <= 0.3) {
		debug_var = 3.11;
	}

	return current;
}

double Min(double data_member[NUM])
{
	float min = 5;
	for (int i = 0; i < NUM; i++) {
		if (data_member[i] < min) min = data_member[i];
	}
	return min;
}

double Max(double data_member[NUM])
{
	double max = 0;
	for (int i = 0; i < NUM; i++) {
		if (data_member[i] > max) max = data_member[i];
	}
	return max;
}

double Sum(double data_member[NUM])
{
	double sum = 0.0;
	for (int i = 0; i < NUM; i++) {
		sum += data_member[i];
	}
	return sum;
}

