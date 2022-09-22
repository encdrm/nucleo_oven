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

typedef struct _heater_t heater_t;

typedef struct _heater_t {
		TIM_HandleTypeDef *htim;
		uint32_t channel;
		uint32_t state;
		float duty;
		float target;
		float current;
		float prev;
		float errorSum;
		bool onFlag;
		void (*start)(heater_t *);
		void (*stop)(heater_t *);
}heater_t;

typedef struct _PIDConst {
		float kp;
		float ki;
		float kd;
		float filterConst;
		float antiWindUpConst;

}PIDConst;

extern heater_t *heaterTop;
extern heater_t *heaterBottom;

extern PIDConst PIDTransient;
extern PIDConst PIDSteady;

heater_t *Custom_HeaterControl(TIM_HandleTypeDef *htim, uint32_t Channel);
void HeaterControl_TIM9_IRQ();

float Control_PID(float sensorADCRead, heater_t *heaterobj, PIDConst PIDMode);
#endif /* INC_CONTROL_H_ */
