/*
 * control.h
 *
 *  Created on: 2022. 9. 10.
 *      Author: phc72
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

# define PERIOD 10

typedef struct _temperatureVariable {
		float target;
		float current;
		float prev;
		float errorSum;

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

float Control_PID(float target, float sensorADCRead, heater_t *temperature, PIDConst PIDMode);

#endif /* INC_CONTROL_H_ */
