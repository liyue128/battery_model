#include "opretion.h"
#include "charge.h"
#define VLOWV 12.4
#define VRECH 16.8
#define D1 0.02
#define D2 0.08
#define INNER_BYP 0.3
#define OUTER_BYP 1.2

double CellBypass(double voltage, double min)
{
	double current_bypass = 0;

	//D1内部放电阈值,D2表外部放电阈值
	if (voltage - min >= D1 / 2 && voltage - min <= D2)
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

	int flag = (sum_of_cell > VLOWV) && ((max_of_cell >= VRECH / 4 && (min_of_cell < VRECH / 4 - 0.05) && max_of_cell - min_of_cell >= D1) || \
		(extermly_bit && max_of_cell - min_of_cell >= D1 / 2));

	return flag;
}

