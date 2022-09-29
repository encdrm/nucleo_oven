/*
 * feature.h
 *
 *  Created on: 2022. 9. 29.
 *      Author: user01
 */

#ifndef INC_FEATURE_H_
#define INC_FEATURE_H_

#include "main.h"

typedef struct _feature_t feature_t;
typedef struct _feature_t {
	char *name;
	void (*on_function)(feature_t* featureobj);
	void (*off_function)(feature_t* featureobj);
	uint32_t (*get_state_function)(feature_t* featureobj);
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
} feature_t;

//========================= 기능 제어 함수 원형 입력 시작 =========================

void feature_gpio_high(feature_t *feature);
void feature_gpio_low(feature_t *feature);
uint32_t feature_state_on_when_gpio_is_high(feature_t *feature);
uint32_t feature_state_on_when_gpio_is_low(feature_t *feature);

//========================== 기능 제어 함수 원형 입력 끝 ==========================

#endif /* INC_FEATURE_H_ */
