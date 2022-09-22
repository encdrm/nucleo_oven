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

#define PERIOD 1

typedef struct _heater_t {
		TIM_HandleTypeDef *htim;
		uint32_t channel;
		uint32_t state;
		uint32_t duty;
		float target;
		float current;
		float prev;
		float errorSum;
		bool onFlag;
}heater_t;

typedef struct _PIDConst {
		float kp;
		float ki;
		float kd;
		float filterConst;
		float antiWindUpConst;

}PIDConst;

extern heater_t heaterTop;
extern heater_t heaterBottom;

extern PIDConst PIDTransient;
extern PIDConst PIDSteady;

void HeaterControl_Init(heater_t *heater, TIM_HandleTypeDef *htim, uint32_t Channel);
void HeaterControl_Start(heater_t *heater);
void HeaterControl_Stop(heater_t *heater);
void HeaterControl_TIM9_IRQ();

float Control_PID(float sensorADCRead, heater_t *heater, PIDConst PIDMode);
#endif /* INC_CONTROL_H_ */
