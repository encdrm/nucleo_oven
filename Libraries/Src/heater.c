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

extern TIM_HandleTypeDef htim9;

extern tempsensor_t *tempTop;
extern tempsensor_t *tempBottom;


void HeaterControl_TIM9_IRQ();
static void Heater_Start(heater_t *heaterobj);
static void Heater_Stop(heater_t *heaterobj);
static void Heater_Set(heater_t *heaterobj);
static void Heater_Controller(tempsensor_t *tempsensor, heater_t *heaterobj);

void HeaterControl_TIM9_IRQ(){
	Heater_Controller(tempTop, heaterTop);
	Heater_Controller(tempBottom, heaterBottom);
	if(heaterTop->state)
		Heater_Set(heaterTop);
	if(heaterBottom->state)
		Heater_Set(heaterBottom);
}

heater_t *Custom_HeaterControl(TIM_HandleTypeDef *htim, uint32_t Channel){
	//
	heater_t *heaterobj = (heater_t*) calloc(1, sizeof(heater_t));

	// Setting methods
	heaterobj->channel = Channel;
	heaterobj->current = .0f;
	heaterobj->duty = .0f;
	heaterobj->errorSum = .0f;
	heaterobj->htim = htim;
	heaterobj->onFlag = false;
	heaterobj->prev = .0f;
	heaterobj->state = OFF;
	heaterobj->target = .0f;

	// Setting fields
	heaterobj->start = Heater_Start;
	heaterobj->stop = Heater_Stop;

	return heaterobj;
}

static void Heater_Start(heater_t *heaterobj){
	heaterobj->onFlag = true;
	HAL_TIM_PWM_Start(heaterobj->htim, heaterobj->channel);
}

static void Heater_Stop(heater_t *heaterobj){
	heaterobj->onFlag = false;
	while (heaterobj->state != OFF)	// Heater_Controller가 OFF 상태인지 확인
	HAL_TIM_PWM_Stop(heaterobj->htim, heaterobj->channel);
}

static void Heater_Set(heater_t *heaterobj){
	// Duty ratio to duty cycle conversion
	uint32_t dutycycle = heaterobj->duty * (__HAL_TIM_GET_AUTORELOAD(heaterobj->htim)+1) - 1;
	// Set duty rate of PWM
	__HAL_TIM_SET_COMPARE(heaterobj->htim, heaterobj->channel, dutycycle);
}

static void Heater_Controller(tempsensor_t *tempsensorobj, heater_t *heaterobj){
	float sensorADCRead = tempsensorobj->read(tempsensorobj);
	if (sensorADCRead == NAN)
		return;

	switch (heaterobj->state){
		case OFF:
			heaterobj->duty = 0.f;
			heaterobj->errorSum = 0.f;
			heaterobj->prev = 0.f;
			heaterobj->current = 0.f;
			if (heaterobj->onFlag) {
				heaterobj->start(heaterobj);
				heaterobj->state = PREHEATING;
			}
			break;

		case PREHEATING:
			heaterobj->duty = 1.f;
			if (!heaterobj->onFlag) heaterobj->state = OFF;
			else if (heaterobj->current > heaterobj->target - 5.f) heaterobj->state = TRANSIENT;
			break;

		case TRANSIENT:
			heaterobj->duty = Control_PID(sensorADCRead, heaterobj, PIDTransient);
			if (!heaterobj->onFlag) heaterobj->state = OFF;
			else if ((heaterobj->current > heaterobj->prev - DEVIATION) && (heaterobj->current < heaterobj->prev + DEVIATION)) heaterobj->state = STEADY;
			break;

		case STEADY:
			heaterobj->duty = Control_PID(sensorADCRead, heaterobj, PIDSteady);
			if (!heaterobj->onFlag) heaterobj->state = OFF;
			else if ((heaterobj->current <= heaterobj->prev - DEVIATION) || (heaterobj->current >= heaterobj->prev + DEVIATION)) heaterobj->state = TRANSIENT;
			break;

		default:
			heaterobj->onFlag = 0;
			heaterobj->state = OFF;
	}
}
