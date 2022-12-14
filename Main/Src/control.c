/*
 * control.c
 *
 *  Created on: 2022. 9. 9.
 *      Author: phc72
 */
#include "control.h"
#include <stdio.h>

PIDConst PIDTransient = {
		.1f,		// kp
		.01f,		// ki
		.2f,		// kd
		20.f,		// filterConst
		20.f,		// antiWindUpConst
		.25f		// deadBandConst
};

PIDConst PIDSteady = { //80% duty 초당 0.5도 상승, 0% duty 초당 0.5도 하락 목표
		.015f,		// kp
		.04f,		// ki
		.4f,		// kd
		20.f,		// filterConst
		20.f,		// antiWindUpConst
		.25f		// deadBandConst
};


float Control_PID(heater_t *heaterobj, PIDConst PIDMode){
	float duty;

	// Proportional term
	float temperatureError = heaterobj->target - heaterobj->current;

	// Derivative term
	float temperatureDifferential = (temperatureError - heaterobj->prevError) / PERIOD;
	heaterobj->prevError = temperatureError;

	// Integral term
	if (temperatureError < -PIDMode.deadBandConst) heaterobj->errorSum += (temperatureError + PIDMode.deadBandConst) * PERIOD;
	else if (temperatureError > PIDMode.deadBandConst) heaterobj->errorSum += (temperatureError - PIDMode.deadBandConst) * PERIOD;

	// anti wind-up
	if (heaterobj->errorSum > PIDMode.antiWindUpConst) heaterobj->errorSum = PIDMode.antiWindUpConst;
	else if (heaterobj->errorSum < -PIDMode.antiWindUpConst) heaterobj->errorSum = -PIDMode.antiWindUpConst;

	// PID control
	duty = (temperatureError * PIDMode.kp + temperatureDifferential * PIDMode.kd + heaterobj->errorSum * PIDMode.ki);

	// output limit
	if (duty < 0.01f) duty = 0.f;
	if (duty > 0.99f) duty = 1.f;
	return duty;
}
