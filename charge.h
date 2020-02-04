#pragma once
#ifndef _CHARGER_H

#define _CHARGER_H

#include <string.h>
#define NUM 4

volatile struct TypeOfStruct
{
	double x;
	double u;
	double voltage;
	int charge_state;
	int count;
	bool extrem_bit;
	bool abnormal_flag;
	TypeOfStruct()
	{
		memset(this, 0, sizeof(TypeOfStruct));
	}
};

double CellBypass(double voltage, double min);
bool ExtermlyFlag(double voltage[NUM], bool extermly_bit);
int ChargeStage(double voltage[NUM], TypeOfStruct *charge_data);

#endif //_CHARGER_H