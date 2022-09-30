/*
 * heater.c
 *
 *  Created on: 2022. 9. 9.
 *      Author: phc72
 *
 */

#include "control.h"
#include "setting.h"

// TRANSIENT <-> STEADY 전환 기준이 되는 온도 편차. 타겟 온도와 차이가 더 커지면 TRANSIENT, 작아지면 STEADY 상태로 전환
#define DEVIATION		2.f

// heater state
enum {
	OFF, PREHEATING, TRANSIENT, STEADY
};

extern setting_t *ovenSetting;

//========================= Type definitions & Structs =========================

typedef struct {
	int pointTime;
	float targetTemperature;
	int holdType;
} profile;

// Methods for heater object
static void heater_start(heater_t *heaterobj);
static void heater_stop(heater_t *heaterobj);
static void heater_set_target_temp(heater_t *heaterobj, float targetTemp);

// Other functions
void HeaterControl_TIM9_IRQ();
void Custom_TransmitTempTBtoBT(void);
__STATIC_INLINE void __Heater_SetDuty(heater_t *heaterobj);
static void Heater_Controller(tempsensor_t *tempsensor, heater_t *heaterobj);

void HeaterControl_TIM9_IRQ() {
	Heater_Controller(thermoTop, heaterTop);
	Heater_Controller(thermoBottom, heaterBottom);
	if (heaterTop->onFlag || heaterBottom->onFlag)	// dirty method
		Custom_TransmitTempTBtoBT();
	if (heaterTop->onFlag)
		__Heater_SetDuty(heaterTop);
	if (heaterBottom->onFlag)
		__Heater_SetDuty(heaterBottom);
}

/* 블루투스로 상하 오븐 온도 전송하는 함수 */
void Custom_TransmitTempTBtoBT() {

	// 아무리 온도 길이가 길어봤자 1000.0도(6word) 이하일 것.
	char buffer[20];
	snprintf(buffer, sizeof(buffer), "%.1f/%.1f/%.1f\n",
			 ((float)ovenSetting->heatruntime) / 60000.0f, thermoTop->read(thermoTop),
			thermoBottom->read(thermoBottom));
	HAL_UART_Transmit(&huart1, (uint8_t*) buffer, strlen(buffer), 10);
}

/* 히터 객체를 생성하는 함수. 입력되는 타이머와 채널은 PWM이 가능해야 한다. */
heater_t* Custom_HeaterControl(TIM_HandleTypeDef *htim, uint32_t Channel) {
	heater_t *heaterobj = (heater_t*) calloc(1, sizeof(heater_t));

	// Setting methods
	heaterobj->channel = Channel;
	heaterobj->current = .0f;
	heaterobj->duty = .0f;
	heaterobj->errorSum = .0f;
	heaterobj->htim = htim;
	heaterobj->onFlag = false;
	heaterobj->prev = .0f;
	heaterobj->prevError = .0f;
	heaterobj->state = OFF;
	heaterobj->target = .0f;

	// Setting fields
	heaterobj->start = heater_start;
	heaterobj->stop = heater_stop;
	heaterobj->set_target_temp = heater_set_target_temp;

	return heaterobj;
}

/* 히터 객체를 생성하는 함수 */
static void Heater_Controller(tempsensor_t *tempsensorobj, heater_t *heaterobj) {
	float sensorADCRead = tempsensorobj->read(tempsensorobj);
	if (sensorADCRead == NAN)
		return;
	heaterobj->prev = heaterobj->current;
	heaterobj->current = sensorADCRead;

	switch (heaterobj->state) {
	case OFF:
		heaterobj->duty = 0.f;
		heaterobj->errorSum = 0.f;
		if (heaterobj->onFlag) {
			heaterobj->state = TRANSIENT;
		}
		break;

	case TRANSIENT:
		heaterobj->duty = Control_PID(heaterobj, PIDTransient);
		if (!heaterobj->onFlag)
			heaterobj->state = OFF;
		else if (abs(heaterobj->target - heaterobj->current) <= DEVIATION) {
			heaterobj->errorSum = .0f;
			heaterobj->state = STEADY;
		}
		break;

	case STEADY:
		heaterobj->duty = Control_PID(heaterobj, PIDSteady);
		if (!heaterobj->onFlag)
			heaterobj->state = OFF;
		else if (abs(heaterobj->target - heaterobj->current) > DEVIATION)
			heaterobj->state = TRANSIENT;
		break;

	default:
		heaterobj->onFlag = 0;
		heaterobj->state = OFF;
	}
}

/* 히터의 On/Off 비율을 설정하는 함수 */
__STATIC_INLINE void __Heater_SetDuty(heater_t *heaterobj) {
	// Duty ratio to duty cycle conversion
	uint32_t dutycycle = heaterobj->duty
			* (__HAL_TIM_GET_AUTORELOAD(heaterobj->htim) + 1) - 1;
	// Set duty rate of PWM
	__HAL_TIM_SET_COMPARE(heaterobj->htim, heaterobj->channel, dutycycle);
}

/* 히터를 켜는 메소드 */
static void heater_start(heater_t *heaterobj) {
	heaterobj->onFlag = true;
	HAL_TIM_PWM_Start(heaterobj->htim, heaterobj->channel);
}

/* 히터를 끄는 메소드 */
static void heater_stop(heater_t *heaterobj) {
	heaterobj->onFlag = false;
	while (heaterobj->state != OFF)	// Heater_Controller가 OFF 상태인지 확인
		HAL_TIM_PWM_Stop(heaterobj->htim, heaterobj->channel);
}

/* 히터의 목표 온도를 설정하는 메소드 */
static void heater_set_target_temp(heater_t *heaterobj, float targetTemp) {
	heaterobj->target = targetTemp;
}
