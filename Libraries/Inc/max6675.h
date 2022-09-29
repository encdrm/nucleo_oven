/*
 * max6675.h
 *
 *  Created on: 2022. 9. 10.
 *      Author: encdrm
 */

#ifndef INC_MAX6675_H_
#define INC_MAX6675_H_

#include "main.h"	// 핀 매크로들 불러오기 위함
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

// 온도센서 출력으로 tempsensorobject->lastTemp를 내놓는 플래그
//#define FLAG_TEMPSENSOR_DEBUG

#ifdef FLAG_TEMPSENSOR_DEBUG
float __sensor_read(tempsensor_t *sensorobj) {
	return sensorobj->lastTemp;
}
#endif

//========================= Type definitions & Structs =========================

typedef enum {
	SENSOR_HIGH,
	SENSOR_LOW
} sensorpos;

typedef struct __waitcount_node waitcount_node;
typedef struct __waitcount waitcount;
typedef struct __tempsensor_t tempsensor_t;

typedef struct __waitcount_node {
	uint32_t data;
	waitcount_node *next;
} waitcount_node;

typedef struct __waitcount {
	waitcount_node *frontPtr;
	waitcount_node *backPtr;
	uint32_t size;
} waitcount;

/* 온도센서 객체 구조체 */
typedef struct __tempsensor_t {
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *csPort;
	uint32_t *waitCount;
	uint32_t __sensor_interval;
	float lastTemp;
	uint16_t csPin;

	float (*read)(tempsensor_t *);
	bool (*is_readable)(tempsensor_t *);

	void (*set_interval)(tempsensor_t *, uint32_t);
	uint32_t (*get_interval)(tempsensor_t *);

	uint32_t (*read_waitcount)(tempsensor_t *);
} tempsensor_t;

// 센서와 통신하는 간격을 최소 250ms로 맞추기 위한 카운트
// 센서값을 읽어들일 때마다 250으로 설정되며, 1ms마다 1씩 줄어든다.
extern waitcount global_waitcount;

//======================== Callback & Public functions ========================

void Systick_Sensor_IRQ();
tempsensor_t* Custom_Tempsensor(SPI_HandleTypeDef *hspi, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t interval);

#endif /* INC_MAX6675_H_ */
