/*
 * feature.c
 *
 *  Created on: 2022. 9. 29.
 *      Author: encdrm
 *
 *  기능을 켜고 끄거나, 상태를 확인하기 위한 함수를 정의하는 파일.
 *  대부분은 단순히 GPIO의 상태를 변경해 제어할 수 있으나, 일부 추가 작업이 필요한 기능을 위해 제작함.
 */
#include <stdio.h>
#include "feature.h"

//========================= 기능 제어 함수 입력 시작 =========================

void feature_gpio_high(feature_t *feature) {
	/*
	 * GPIO를 HIGH로 만드는 함수.
	 */
	printf("%s HIGH\r\n",feature->name);
	HAL_GPIO_WritePin(feature->GPIOx, feature->GPIO_Pin, GPIO_PIN_SET);
}

void feature_gpio_low(feature_t *feature) {
	/*
	 * GPIO를 LOW로 만드는 함수.
	 */
	printf("%s LOW\r\n",feature->name);
	HAL_GPIO_WritePin(feature->GPIOx, feature->GPIO_Pin, GPIO_PIN_RESET);
}
uint32_t feature_state_on_when_gpio_is_high(feature_t *feature) {
	/*
	 * GPIO가 HIGH인 상태를 ON으로 처리하는 함수.
	 * return: 1(ON), 0(OFF)
	 */
	printf("%s state check\r\n",feature->name);
	return HAL_GPIO_ReadPin(feature->GPIOx, feature->GPIO_Pin);
}
uint32_t feature_state_on_when_gpio_is_low(feature_t *feature) {
	/*
		 * GPIO가 LOW인 상태를 ON으로 처리하는 함수.
		 * return: 1(ON), 0(OFF)
	 */
	printf("%s state check\r\n",feature->name);
	return !HAL_GPIO_ReadPin(feature->GPIOx, feature->GPIO_Pin);
}
//========================= 기능 제어 함수 입력 끝 =========================


