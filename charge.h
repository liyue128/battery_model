#pragma once
#ifndef _CHARGER_H

#define _CHARGER_H

double CellBypass(double voltage, double min);
bool ExtermlyFlag(double voltage[NUM], bool extermly_bit);

#endif // !_MATH_H