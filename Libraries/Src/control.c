/*
 * control.c
 *
 *  Created on: 2022. 9. 9.
 *      Author: phc72
 */
#include "control.h"

heater_t heaterTop;
heater_t heaterBottom;

PIDConst PIDTransient = {
		5.f,		// kp
		0.f,		// ki
		10.f,		// kd
		10.f,		// filterConst
		5.f			// antiWindUpConst
};

void CONTROL_TIM1_IRQ(){

}

PIDConst PIDSteady = { //80% duty 초당 0.5도 상승, 0% duty 초당 0.5도 하락 목표
		5.f,		// kp
		4.f,		// ki
		1.f,		// kd
		10.f,		// filterConst
		5.f			// antiWindUpConst
};

float Control_PID(float sensorADCRead, heater_t *heater, PIDConst PIDMode){

	float duty;
	heater->prev = heater->current;
	heater->current = sensorADCRead;

	// Proportional term
	int temperatureError = heater->target - heater->current;

	// Derivative term
	int temperatureDifferential = (heater->current - heater->prev) / PERIOD;

	// Integral term
	heater->errorSum += temperatureError * PERIOD;

	// Low Pass Filtering
	float temperatureDifferentialFilt = PIDMode.filterConst / (1 + PIDMode.filterConst / temperatureDifferential);

	// anti wind-up
	if (heater->errorSum > PIDMode.antiWindUpConst) heater->errorSum = PIDMode.antiWindUpConst;
	else if (heater->errorSum < -PIDMode.antiWindUpConst) heater->errorSum = -PIDMode.antiWindUpConst;

	// PID control
	duty = temperatureError * PIDMode.kp + temperatureDifferentialFilt * PIDMode.kd + heater->errorSum * PIDMode.ki;

	// output limit
	if (duty < 0.01f) duty = 0.f;
	if (duty > 0.99f) duty = 1.f;

	return duty;
}
