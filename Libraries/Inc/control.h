/*
 * control.h
 *
 *  Created on: 2022. 9. 10.
 *      Author: phc72
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#include "main.h"
#include <stdbool.h>

#define PERIOD 1

typedef struct _heater_t {
		TIM_HandleTypeDef *htim;
		uint32_t Channel;
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

float Control_PID(float sensorADCRead, heater_t *heater, PIDConst PIDMode);

#endif /* INC_CONTROL_H_ */
