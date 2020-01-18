/*
恒压开始阶段，电压会缓缓下降一会儿,由于逻辑上的原因，当电压下降到4.2以下时会产生瞬时3.11的电流;
然后重复以上过程，直至电流降至终止电流
*/

#include <iostream>
#include "opretion.h"
#include "charge.h"
#define VRECH 16.8
#define NUM 4

int main()
{
	using namespace std;

	double x[NUM] = { 0,0.2,0.4,0.8 }, y[NUM] = { 0 }, u[NUM] = { 0 };
	double b, y0;
	double min_of_soc = 0, max_of_voltage = 0, min_of_voltage = 0, battery_voltage;
	long count[5] = { 0 };
	bool extrem_bit = false, extrem_flag = false;
	b = 1.0 * 0.1 / 3600 * 3.11;

	double debug_var = 0;  //调试变量

	min_of_soc = Min(x);
	while (min_of_soc < 1) {
		cout << count[4]++ << " ";

		//判断极端不平衡状态
		extrem_flag = ExtermlyFlag(y, extrem_bit);
		if (extrem_flag) {
			extrem_bit = true;
		}
		else extrem_bit = false;

		for (int i = 0; i < NUM; i++) {
			//对SOC的范围进行限制
			if (x[i] < 0) { x[i] = 0; }
			if (x[i] > 1) { x[i] = 1; }

			//记录上一个时刻y[i]的值
			y0 = y[i];

			y[i] = ChargingVoltage(x[i], u[i], y, extrem_bit);  //电压计算
			//soc状态更新
			x[i] = x[i] + b * u[i];

			if (y[i] > VRECH / 4) {  //恒压阶段电流控制
				count[i]++;
			}
			else if (y[i] < VRECH / 4 - 0.05) {
				count[i] = 0;
			}
			if (x[i] > 1) {
				y[i] = 4.20;
			}
			u[i] = ChargingCurrent(y, count[i], i, extrem_bit);

			//debug
			/*if (y[i] > 4.25) {
				y[i] = 4.25;
			}*/

			cout << y[i] << " ";  // << u[i] << " ";  // << x[i] << " ";
		}

		max_of_voltage = Max(y);
		min_of_voltage = Min(y);
		min_of_soc = Min(x);
		battery_voltage = Sum(y);

		cout << battery_voltage << endl;
	}

	return 0;
}