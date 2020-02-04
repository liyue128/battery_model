#include <iostream>
#include <time.h>
#include <Windows.h>
#include "opretion.h"
#include "charge.h"
#define VRECH 16.8
#define ABN_SAMPLE 100000
#define EXTREM_TIMEROUT 6000
#define MIN_VOLTAGE 1
#define CYCLE_TIME 20
#define ABSENT 0
#define EXTREM 1
#define ABNORMAL 2
#define PRECH 3
#define EQ_CUR 4
#define EQ_VOL 5
clock_t start_time, end_time;

int main()
{
	using namespace std;

	TypeOfStruct* data_structure;
	data_structure = new TypeOfStruct;

	double x[NUM] = { 0.05,0.05,0.05,0.2 }, y[NUM] = { 0 }, u[NUM] = { 0 };
	double input_current, b, time_left;
	double min_of_soc = 0, max_of_voltage = 0, min_of_voltage = 0, battery_voltage;
	long sample_count = 0, extrem_count = 0;
	bool extrem_flag = false;  //ģ�⼫�˲�ƽ��״̬
	b = 1.0 * 0.1 / 3600 * 1.5;

	double debug_var = 0;  //���Ա���

	min_of_soc = Min(x);
	while (min_of_soc < 1) {
		//ѭ����ʼʱ��
		start_time = clock();

		cout << sample_count++ << " ";

		//update
		max_of_voltage = Max(y);
		min_of_voltage = Min(y);
		min_of_soc = Min(x);
		battery_voltage = Sum(y);


		//�жϼ��˲�ƽ��״̬  �жϳ��״̬
		extrem_flag = ExtermlyFlag(y, data_structure->extrem_bit);
		if (extrem_flag) {
			data_structure->extrem_bit = true;
		}
		else { data_structure->extrem_bit = false; }
		data_structure->charge_state = ChargeStage(y, data_structure);
		if (sample_count == 1) data_structure->charge_state = 1;  //����ʼ���״̬��Ϊ���˲�ƽ��

		//calculate current
		input_current = ChargingCurrent(y, data_structure);
		
		for (int i = 0; i < NUM; i++) {
			//����soc��Χ
			if (x[i] > 1) {
				x[i] = 1;
			}
			if (x[i] < 0) {
				x[i] = 0;
			}

			//update
			data_structure->x = x[i];
			data_structure->u = u[i];
			data_structure->voltage = battery_voltage;

			//debug
			debug_var = y[i];

			y[i] = ChargingVoltage(y, data_structure);  //��ѹ����
			//soc״̬����
			x[i] = x[i] + b * u[i];

			if (data_structure->charge_state == EQ_CUR || data_structure->charge_state == EXTREM) {
				u[i] = input_current + CellBypass(y[i], min_of_voltage);
			}
			else {
				u[i] = input_current;
			}

			//debug
			if (debug_var > 4.1 && debug_var - y[i] > 0.01) {
				debug_var = u[i];
			}

			cout << y[i] << " " << u[i] << " ";  // << x[i] << " ";
		}

		cout << battery_voltage << " " << data_structure->abnormal_flag << endl;

		//ģ����оƬ�ܹ���⵽���쳣
		if (sample_count == ABN_SAMPLE) {  //�����10000��������ʱ�������оƬ�ܹ���⵽���쳣
			data_structure->abnormal_flag = true;
		}
		if (sample_count - ABN_SAMPLE >= 100) {  //�����쳣��ĵ�100���������˳�ѭ��
			break;
		}

		if(data_structure->charge_state == EXTREM) {  //ģ�⼫�˲�ƽ��״̬��ʱ�쳣
			extrem_count++;
			if (extrem_count >= EXTREM_TIMEROUT) {  //ģ����оƬ�ļ�ʱ����ʱ
				data_structure->extrem_bit = false;
				data_structure->abnormal_flag = true;
			}
		}
		if (data_structure->charge_state == ABNORMAL && extrem_count >= EXTREM_TIMEROUT) {  //�����쳣��ĵ�100���������˳�ѭ��
			extrem_count++;
			if (extrem_count >= EXTREM_TIMEROUT + 100) {
				break;
			}
		}

		//��������ѭ����ʱ
		end_time = clock();
		time_left = CYCLE_TIME - (end_time - start_time) * 1.0 / CLOCKS_PER_SEC;
//		if(time_left > 0) Sleep(time_left);
	}

	delete data_structure;

	return 0;
}