/*
 * control.h
 *
 *  Created on: 2022. 9. 10.
 *      Author: phc72
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#include "main.h"
#include "max6675.h"
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#define PERIOD 1

//========================= Type definitions & Structs =========================

typedef struct _heater_t heater_t;
typedef struct _heater_t {
		TIM_HandleTypeDef *htim;	// PWM 타이머 핸들
		uint32_t channel;			// PWM 타이머 채널
		uint32_t state;
		float time;
		float duty;
		float target;
		float current;
		float prev;
		float prevError;
		float errorSum;
		bool onFlag;
		void (*start)(heater_t *);
		void (*stop)(heater_t *);
		void (*set_target_temp)(heater_t *, float);
} heater_t;

typedef struct _PIDConst {
		float kp;
		float ki;
		float kd;
		float filterConst;
		float antiWindUpConst;
		float deadBandConst;

}PIDConst;

//============================== Exported types ==============================

extern heater_t *heaterTop;
extern heater_t *heaterBottom;

extern PIDConst PIDTransient;
extern PIDConst PIDSteady;

extern TIM_HandleTypeDef htim9;
extern UART_HandleTypeDef huart1;

extern tempsensor_t *thermoTop;
extern tempsensor_t *thermoBottom;


heater_t *Custom_HeaterControl(TIM_HandleTypeDef *htim, uint32_t Channel);
void HeaterControl_TIM9_IRQ();

float Control_PID(heater_t *heaterobj, PIDConst PIDMode);
#endif /* INC_CONTROL_H_ */
