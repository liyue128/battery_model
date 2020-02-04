#include <iostream>
#include "opretion.h"
#define VLOWV 12.4
#define VRECH 16.8
#define VRECH0 16.5
#define IPRE 0.203
#define INORMAL 1.5  //3.11
#define ITERM 0.2  //0.203
#define D1 0.02
#define ABSENT 0
#define EXTREM 1
#define ABNORMAL 2
#define PRECH 3
#define EQ_CUR 4
#define EQ_VOL 5

double SOC[21] = { 0,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1 };
double VOC[21] = { 3.1285,3.3569,3.396,3.462,3.521,3.5566,3.585,3.6123,3.643,3.6869,3.7359,3.786,3.8326,3.8766,3.9196,3.969,4.028,4.0848,4.1196,4.1457,4.166 };
double R0[21] = { 0.122239389,0.09410802,0.079650846,0.076279662,0.072013093,0.071019369,0.07011003,0.070668486,0.06937625,0.068206621,\
		0.068655088,0.068376068,0.06792144,0.067836683,0.066842488,0.067557738,0.068285143,0.066842488,0.06847322,0.071371398,0.167451596 };
//double cell_current[16] = { 3.11,2.812,2.511,2.211,1.912,1.61,1.311,1.011,0.7738,0.6495,0.5406,0.47,0.45001,0.327,0.243,0.203 };
double cell_current[16] = { 1.5,1.3667,1.232,1.098,0.964,0.829,0.6955,0.561,0.455,0.3997,0.351,0.3194,0.31046,0.25545,0.2179,0.200 };
//double data_number[16] = { 0,70,140,227,327,564,670,978,1340,1438,1549,1657,1811,2057,2548,2966 };
double data_number[16] = { 0,70,140,227,327,564,670,978,1340,1438,1549,1657,1811,2057,2548,2966 };

double function1(float state_of_charge)
{
	int l = 0, r = 21;
	while (r - l > 1) {
		int mid = (l + r) / 2;
		if (state_of_charge == SOC[mid]) {
			l = mid;
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
double ChargingVoltage(double cell_voltage[NUM], TypeOfStruct* charge_data)
{
	double f, h;
	double y;
	double battery_voltage = Sum(cell_voltage);
	f = function1(charge_data->x);
	h = function2(charge_data->x);

	if (charge_data->charge_state == EQ_CUR || charge_data->charge_state == PRECH) {
		y = f + h * INORMAL;
	}
	else if (charge_data->charge_state == EQ_VOL) {
		y = f + h * charge_data->u;
	}
	else {
		y = f;
	}

	return y;
}

double ChargingCurrent(double voltage[NUM], TypeOfStruct* charge_data)
{
	int n = 1;
	double battery_voltage = Sum(voltage);
	double min = Min(voltage);
	double max = Max(voltage);
	double current;

	//extremly imbalance
	if (charge_data->charge_state == EXTREM || charge_data->charge_state == ABNORMAL) {
		charge_data->count = 0;
		current = 0;
	}

	//precharge
	if (charge_data->charge_state == PRECH) {
		charge_data->count = 0;
		current = IPRE;
	}
	//equal current
	if (charge_data->charge_state == EQ_CUR) {
		charge_data->count = 0;
		current = INORMAL;
	}
	//equal voltage
	if (charge_data->charge_state == EQ_VOL) {
		charge_data->count++;
		if (charge_data->count < data_number[15]) {
			int l = 0, r = 16;
			while (r - l > 1) {
				int mid = (l + r) / 2;
				if (charge_data->count == data_number[mid] / n) {
					l = mid;
					break;
				}
				if (charge_data->count < data_number[mid] / n) {
					r = mid;
				}
				else {
					l = mid;
				}
			}
			if (charge_data->count < data_number[15]) {
				current = cell_current[l] + (cell_current[l + 1] - cell_current[l]) / ((data_number[l + 1] - data_number[l]) / n) * (charge_data->count - data_number[l] / n);
			}
		}
		else {
			current = ITERM;
		}
	}

	//debug
	double debug_var = 0;

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

