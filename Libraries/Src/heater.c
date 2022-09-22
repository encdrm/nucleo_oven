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

//히터 PWM은 10kHz마다 실행됨.

void Heater_Clear(heater_t *heater){

}

void Heater_Init(heater_t *heater, TIM_HandleTypeDef *htim, uint32_t Channel){


}

void Heater_Start(heater_t *heater){
	// 타이머 관련

}

void Heater_Stop(heater_t *heater){

}

void Heater_Controller(int heaterState, tempsensor_t *tempsensor, heater_t *heater){
	float sensorADCRead = tempsensor->read(tempsensor);

	switch (heaterState){
		case OFF:
			duty = 0.f;
			Heater_Stop(heater);
			if (heater->onFlag) {
				Heater_Start(heater);
				heaterState = PREHEATING;
			}
			break;

		case PREHEATING:
			duty = 1.f;
			if (!heater->onFlag) heaterState = OFF;
			else if (heater->current > heater->target - 5.f) heaterState = TRANSIENT;
			break;

		case TRANSIENT:
			duty = Control_PID(sensorADCRead, heater, PIDTransient);
			if (!heater->onFlag) heaterState = OFF;
			else if ((heater->current > heater->prev - DEVIATION) && (heater->current < heater->prev + DEVIATION)) heaterState = STEADY;
			break;

		case STEADY:
			duty = Control_PID(sensorADCRead, heater, PIDSteady);
			if (!heater->onFlag) heaterState = OFF;
			else if ((heater->current <= heater->prev - DEVIATION) || (heater->current >= heater->prev + DEVIATION)) heaterState = TRANSIENT;
			break;

		default:
			heater->onFlag = 0;
			heaterState = OFF;

	}

}
