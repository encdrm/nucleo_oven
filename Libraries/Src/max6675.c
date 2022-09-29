/*
 * max6675.c
 *
 *  Created on: 2022. 9. 10.
 *      Author: encdrm
 */

#include "max6675.h"

//============================= Private functions =============================
static void Add_GlobalWaitCountNode(waitcount_node *waitCntNode);
__weak float __sensor_read(tempsensor_t *sensorobj);

//================================== Methods ==================================
static float sensor_read(tempsensor_t *sensorobj);
static bool sensor_is_readable(tempsensor_t *sensorobj);
static void sensor_set_interval(tempsensor_t *sensorobj, uint32_t interval);
static uint32_t sensor_get_interval(tempsensor_t *sensorobj);
static uint32_t sensor_read_waitcount(tempsensor_t *sensorobj);

waitcount waitcount_node_storage;

//================================= functions =================================

/*
 * 카운트 값을 1씩 줄이기 위한 인터럽트 핸들러.
 * HAL_InitTick 함수에서 Systick 인터럽트가 1ms마다 실행되도록 설정되므로,
 * 핸들러의 실행주기도 1ms이다.
 */
void Systick_Sensor_IRQ() {
	// 카운트가 0이 아니면, waitcount의 값을 1 감소시킨다.
	// 현재 카운트 node를 읽으면, next node가 없을 때까지 다음으로 넘어가 읽기를 반복한다.
	if (waitcount_node_storage.size > 0) {
		waitcount_node *cur_node = waitcount_node_storage.frontPtr;
		while (cur_node != NULL) {
			cur_node->data -= (cur_node->data == 0) ? 0 : 1;
			cur_node = cur_node->next;
		}
	}
}

/* 온도 센서 객체를 생성하는 함수 */
tempsensor_t* Custom_Tempsensor(SPI_HandleTypeDef *hspi, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t interval) {
	// Setting methods
	tempsensor_t *sensorobj = (tempsensor_t*) calloc(1, sizeof(tempsensor_t));

	sensorobj->read = sensor_read;
	sensorobj->is_readable = sensor_is_readable;
	sensorobj->set_interval = sensor_set_interval;
	sensorobj->get_interval = sensor_get_interval;
	sensorobj->read_waitcount = sensor_read_waitcount;


	// Setting fields
	sensorobj->hspi = hspi;
	sensorobj->__sensor_interval = interval;

	// Setting CS pin
	sensorobj->csPin = GPIO_Pin;
	sensorobj->csPort = GPIOx;

	// Pulling up CS pin
	HAL_GPIO_WritePin(sensorobj->csPort, sensorobj->csPin, GPIO_PIN_SET);

	// setting waitCount object
	waitcount_node *waitCntNode = (waitcount_node*) calloc(1, sizeof(waitcount_node));
	sensorobj->waitCount = &(waitCntNode->data);
	*(sensorobj->waitCount) = 10;	// for initialize&interrupt working test
	Add_GlobalWaitCountNode(waitCntNode);

	return sensorobj;
}

void Add_GlobalWaitCountNode(waitcount_node *waitCntNode) {
	if (waitCntNode != NULL) {

		if (waitcount_node_storage.size == 0) {
			waitcount_node_storage.frontPtr = waitCntNode;
		} else {
			waitcount_node_storage.backPtr->next = waitCntNode;

		}
		waitcount_node_storage.backPtr = waitCntNode;
		waitcount_node_storage.size++;
	}
}

/* 센서 값을 실제로 읽는 함수 */
__weak float __sensor_read(tempsensor_t *sensorobj) {
	uint8_t pData[2];
	float temp = 0;

	// 16비트 데이터를 MSB부터 차례대로 받는다.
	HAL_GPIO_WritePin(sensorobj->csPort, sensorobj->csPin, GPIO_PIN_RESET);
	HAL_SPI_Receive(sensorobj->hspi, pData, 1, 50);
	HAL_GPIO_WritePin(sensorobj->csPort, sensorobj->csPin, GPIO_PIN_SET);

	if (((pData[0]|(pData[1]<<8))>>2)& 0x0001)
	   return NAN;
	temp = ((((pData[0]|pData[1]<<8)))>>3);

	return temp * 0.25f;
}

/*
 * 센서 값을 읽는 메소드.
 * 센서는 최소 250ms의 간격을 두고 읽어야 한다.
 * 따라서 마지막으로 센서를 읽은 지 250ms가 흐르지 않았다면, 대신 이전에 읽은 센서 값을 반환한다.
 * 실제로 센서 값을 읽는 작업은 __sensor_read가 수행한다.
 */
float sensor_read(tempsensor_t *sensorobj) {
	float data = .0f;
	if (sensorobj->is_readable(sensorobj)) {
		data = __sensor_read(sensorobj);
		*(sensorobj->waitCount) = sensorobj->__sensor_interval;
		sensorobj->lastTemp = data;
	} else {
		data = sensorobj->lastTemp;
	}
	return data;
}

/* 센서 값을 읽을 수 있는지 확인하는 메소드 */
bool sensor_is_readable(tempsensor_t *sensorobj) {
	return !(sensorobj->read_waitcount(sensorobj));
}

/* 센서를 읽는 간격(in ms)을 설정하는 메소드 */
void sensor_set_interval(tempsensor_t *sensorobj, uint32_t interval) {
	sensorobj->__sensor_interval = interval;
}

/* 센서를 읽는 간격(in ms)을 가져오는 메소드*/
uint32_t sensor_get_interval(tempsensor_t *sensorobj) {
	return sensorobj->__sensor_interval;
}

uint32_t sensor_read_waitcount(tempsensor_t *sensorobj) {
	return *(sensorobj->waitCount);
}
