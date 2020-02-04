#include "opretion.h"
#include "charge.h"
#define ABSENT 0
#define EXTREM 1
#define ABNORMAL 2
#define PRECH 3
#define EQ_CUR 4
#define EQ_VOL 5
#define VLOWV 12.4
#define VRECH 16.4
#define D1 0.02
#define D2 0.08
#define INNER_BYP 0.2
#define OUTER_BYP 0.6

double CellBypass(double voltage, double min)
{
	double current_bypass = 0;

	//D1内部放电阈值,D2表外部放电阈值
	if (voltage - min >= D1 / 4 && voltage - min <= D2)
	{
		//subtract the inner bypass current
		current_bypass -= INNER_BYP;
	}  //仅闭合内部放电旁路

	if (voltage - min > D2&& voltage - min <= D1 + D2)
	{
		//subtract the outter bypass current
		current_bypass -= OUTER_BYP;
	}  //仅闭合外部放电旁路

	if (voltage - min > D1 + D2)
	{
		//subtract both inner and outter bypass current
		current_bypass -= INNER_BYP + OUTER_BYP;
	}  //闭合内部和外部旁路
	return current_bypass;
}

bool ExtermlyFlag(double voltage[NUM], bool extermly_bit)
{
	double max_of_cell = Max(voltage);  //求最大值
	double min_of_cell = Min(voltage);  //求最小值
	double sum_of_cell = Sum(voltage);  //求和

	int flag = (sum_of_cell > VLOWV) && (max_of_cell > VRECH / NUM && max_of_cell - min_of_cell >= D1) || \
		(extermly_bit && max_of_cell - min_of_cell >= D1 / 4);

	return flag;
}

int ChargeStage(double voltage[NUM], TypeOfStruct* charge_data)
{
	int charge_stage = 0;
	double sum_of_voltage = Sum(voltage);

	if (sum_of_voltage == 0) {
		charge_stage = ABSENT;
	}
	if (sum_of_voltage >= VLOWV && charge_data->extrem_bit) {
		charge_stage = EXTREM;
	}
	if (charge_data->abnormal_flag) {
		charge_stage = ABNORMAL;
	}
	if (sum_of_voltage < VLOWV && !charge_data->abnormal_flag) {
		charge_stage = PRECH;
	}
	if (sum_of_voltage > VLOWV&& sum_of_voltage < VRECH && !charge_data->extrem_bit && !charge_data->abnormal_flag) {
		if (charge_data->charge_state != EQ_VOL) {
			charge_stage = EQ_CUR;
		}
	}
	if ((sum_of_voltage > VRECH || charge_data->charge_state == EQ_VOL) && !charge_data->extrem_bit && !charge_data->abnormal_flag) {
		charge_stage = EQ_VOL;
	}

	return charge_stage;
}