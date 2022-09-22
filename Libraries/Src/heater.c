/*
 * heater.c
 *
 *  Created on: 2022. 9. 9.
 *      Author: phc72
 */


# include "control.h"

// heater 상태 상수
# define OFF			0
# define PREHEATING		1
# define TRANSIENT		2
# define STEADY			4

// TRANSIENT <-> STEADY 전환 기준이 되는 온도 편차. 타겟 온도와 차이가 더 커지면 TRANSIENT, 작아지면 STEADY 상태로 전환
# define DEVIATION		0.2f

//
typedef struct{
	int pointTime;
	float targetTemperature;
	int holdType;
} profile;

//profile reflow[]

//제어주기마다의 목표온도 어레이
//typedef struct{
//	int operationTime;
//	int targetTemperature;
//} profileArray;

//float reflowArr[];
float temperatureTestArr[] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
							  100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
							  100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
							  120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
							  130, 140, 150, 150, 150, 150, 155, 160, 165, 170, 175, 180,
							  180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180,
							  150, 150, 150, 150, 150, 150, 100, 100, 100, 100, 100, 100,};

int heaterState = 0;
float duty = 0.f;
float target = 0.f;

void Heater_Clear(heater_t *heater){
	heater->target = 0.f;
	heater->current = 0.f;
	heater->prev = 0.f;
	heater->errorSum = 0.f;
	duty = 0.f;
	target = 0.f;
}

void Heater_Init(heater_t temperature){

}

void Heater_Start(heater_t temperature){
	// 타이머 관련
}

void Heater_Stop(heater_t temperature){

}

void Heater_Controller(float sensorADCRead, heater_t *temperature){

	switch (heaterState){
		case OFF:
			duty = 0.f;
			Heater_Clear(temperature);
			if (heaterOnFlag) {
				Heater_Init(temperature);
				heaterState = PREHEATING;
			}
			break;

		case PREHEATING:
			duty = 1.f;
			if (!heaterOnFlag) heaterState = OFF;
			else if (heater->current > target - 5.f) heaterState = TRANSIENT;
			break;

		case TRANSIENT:
			duty = Control_PID(target, sensorADCRead, temperature, PIDTransient);
			if (!heaterOnFlag) heaterState = OFF;
			else if ((heater->current > heater->prev - DEVIATION) && (heater->current < heater->prev + DEVIATION)) heaterState = STEADY;
			break;

		case STEADY:
			duty = Control_PID(target, sensorADCRead, temperature, PIDSteady);
			if (!heaterOnFlag) heaterState = OFF;
			else if ((heater->current <= heater->prev - DEVIATION) || (heater->current >= heater->prev + DEVIATION)) heaterState = TRANSIENT;
			break;

		default:
			heaterOnFlag = 0;
			heaterState = OFF;

	}

}
