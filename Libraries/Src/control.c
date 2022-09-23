/*
 * control.c
 *
 *  Created on: 2022. 9. 9.
 *      Author: phc72
 */
#include "control.h"
#include <stdio.h>

PIDConst PIDTransient = {
		.5f,		// kp
		0.f,		// ki
		.2f,		// kd
		10.f,		// filterConst
		5.f,		// antiWindUpConst
		.5f			// deadBandConst
};

PIDConst PIDSteady = { //80% duty 초당 0.5도 상승, 0% duty 초당 0.5도 하락 목표
		.05f,		// kp
		.2f,		// ki
		.01f,		// kd
		10.f,		// filterConst
		5.f,		// antiWindUpConst
		.5f			// deadBandConst
};

float Control_PID(float sensorADCRead, heater_t *heaterobj, PIDConst PIDMode){

	float duty;
	heaterobj->prev = heaterobj->current;
	heaterobj->current = sensorADCRead;

	// Proportional term
	float temperatureError = heaterobj->target - heaterobj->current;

	// Derivative term
	float temperatureDifferential = (heaterobj->current - heaterobj->prev) / PERIOD;

	// Integral term
	if (temperatureError < -PIDMode.deadBandConst) heaterobj->errorSum += (temperatureError + PIDMode.deadBandConst) * PERIOD;
	else if (temperatureError > PIDMode.deadBandConst) heaterobj->errorSum += (temperatureError - PIDMode.deadBandConst) * PERIOD;


	// anti wind-up
	if (heaterobj->errorSum > PIDMode.antiWindUpConst) heaterobj->errorSum = PIDMode.antiWindUpConst;
	else if (heaterobj->errorSum < -PIDMode.antiWindUpConst) heaterobj->errorSum = -PIDMode.antiWindUpConst;

	// PID control
	duty = (temperatureError * PIDMode.kp - temperatureDifferential * PIDMode.kd + heaterobj->errorSum * PIDMode.ki);

	// output limit
	if (duty < 0.01f) duty = 0.f;
	if (duty > 0.99f) duty = 1.f;
	return duty;
}
