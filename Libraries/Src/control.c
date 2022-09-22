/*
 * control.c
 *
 *  Created on: 2022. 9. 9.
 *      Author: phc72
 */
#include "control.h"
#include <stdio.h>

PIDConst PIDTransient = {
		5.f,		// kp
		0.f,		// ki
		10.f,		// kd
		10.f,		// filterConst
		5.f			// antiWindUpConst
};

PIDConst PIDSteady = { //80% duty 초당 0.5도 상승, 0% duty 초당 0.5도 하락 목표
		5.f,		// kp
		4.f,		// ki
		1.f,		// kd
		10.f,		// filterConst
		5.f			// antiWindUpConst
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
	heaterobj->errorSum += temperatureError * PERIOD;

	// Low Pass Filtering
	float temperatureDifferentialFilt = PIDMode.filterConst / (1 + PIDMode.filterConst / temperatureDifferential);

	// anti wind-up
	if (heaterobj->errorSum > PIDMode.antiWindUpConst) heaterobj->errorSum = PIDMode.antiWindUpConst;
	else if (heaterobj->errorSum < -PIDMode.antiWindUpConst) heaterobj->errorSum = -PIDMode.antiWindUpConst;

	// PID control
	duty = (temperatureError * PIDMode.kp + temperatureDifferentialFilt * PIDMode.kd + heaterobj->errorSum * PIDMode.ki)/100;

	// output limit
	if (duty < 0.01f) duty = 0.f;
	if (duty > 0.99f) duty = 1.f;
	printf("Te: %f, Td: %f, ES: %f, duty: %f\r\n", temperatureError, temperatureDifferential, heaterobj->errorSum, duty);
	return duty;
}
