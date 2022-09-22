/*
 * heater.c
 *
 *  Created on: 2022. 9. 9.
 *      Author: phc72
 */


#include "control.h"

// heater 상태 상수
#define OFF			0
#define PREHEATING		1
#define TRANSIENT		2
#define STEADY			4

// TRANSIENT <-> STEADY 전환 기준이 되는 온도 편차. 타겟 온도와 차이가 더 커지면 TRANSIENT, 작아지면 STEADY 상태로 전환
#define DEVIATION		0.2f

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

extern tempsensor_t *tempTop;
extern tempsensor_t *tempBottom;

void HeaterControl_TIM9_IRQ();
static void Heater_Start(heater_t *heater);
static void Heater_Stop(heater_t *heater);
static void Heater_Set(heater_t *heater);
static void Heater_Controller(tempsensor_t *tempsensor, heater_t *heater);

void HeaterControl_TIM9_IRQ(){
	Heater_Controller(tempTop, &heaterTop);
	Heater_Controller(tempBottom, &heaterBottom);
	if(heaterTop.state)
		Heater_Set(&heaterTop);
	if(heaterBottom.state)
		Heater_Set(&heaterBottom);
}

void HeaterControl_Start(heater_t *heater) {
	if (heater != NULL){
		HAL_TIM_Base_Start(heater->htim);
		HAL_TIM_Base_Start_IT(heater->htim);
		Heater_Start(heater);
	}
}

void HeaterControl_Stop(heater_t *heater) {
	if (heater != NULL){
		HAL_TIM_Base_Stop_IT(heater->htim);
		HAL_TIM_Base_Stop(heater->htim);
		Heater_Stop(heater);
	}
}

void HeaterControl_Init(heater_t *heater, TIM_HandleTypeDef *htim, uint32_t Channel){
	// Initialize heater struct
	heater->channel = Channel;
	heater->current = .0f;
	heater->duty = .0f;
	heater->errorSum = .0f;
	heater->htim = htim;
	heater->onFlag = false;
	heater->prev = .0f;
	heater->state = OFF;
	heater->target = .0f;
}

//히터 PWM은 10kHz마다 실행됨.
static void Heater_Start(heater_t *heater){
	HAL_TIM_PWM_Start(heater->htim, heater->channel);
}

static void Heater_Stop(heater_t *heater){
	HAL_TIM_PWM_Stop(heater->htim, heater->channel);
}

static void Heater_Set(heater_t *heater){
	// Duty ratio to duty cycle conversion
	uint32_t dutycycle = heater->duty * (__HAL_TIM_GET_AUTORELOAD(heater->htim)+1) - 1;
	// Set duty rate of PWM
	__HAL_TIM_SET_COMPARE(heater->htim, heater->channel, dutycycle);
}

static void Heater_Controller(tempsensor_t *tempsensor, heater_t *heater){
	uint32_t heaterState = heater->state;
	float sensorADCRead = tempsensor->read(tempsensor);
	if (sensorADCRead == NAN)
		return;

	switch (heaterState){
		case OFF:
			heater->duty = 0.f;
			Heater_Stop(heater);
			if (heater->onFlag) {
				Heater_Start(heater);
				heaterState = PREHEATING;
			}
			break;

		case PREHEATING:
			heater->duty = 1.f;
			if (!heater->onFlag) heaterState = OFF;
			else if (heater->current > heater->target - 5.f) heaterState = TRANSIENT;
			break;

		case TRANSIENT:
			heater->duty = Control_PID(sensorADCRead, heater, PIDTransient);
			if (!heater->onFlag) heaterState = OFF;
			else if ((heater->current > heater->prev - DEVIATION) && (heater->current < heater->prev + DEVIATION)) heaterState = STEADY;
			break;

		case STEADY:
			heater->duty = Control_PID(sensorADCRead, heater, PIDSteady);
			if (!heater->onFlag) heaterState = OFF;
			else if ((heater->current <= heater->prev - DEVIATION) || (heater->current >= heater->prev + DEVIATION)) heaterState = TRANSIENT;
			break;

		default:
			heater->onFlag = 0;
			heaterState = OFF;
	}
}
