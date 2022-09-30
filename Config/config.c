/*
 * config.c
 *
 *  Created on: 2022. 9. 30.
 *      Author: user01
 *
 *  오븐의 기능을 관리하는 파일.
 *  기능 추가 과정은 다음과 같다.
 *  1. feature.c에 기능 켜는 함수, 끄는 함수, 상태 불러오는 함수 정의(optional)
 *  2. feature.h에 위 함수 원형 정의
 *  3. config.h의 globalFeatureList[]에 기능 추가
 *  예시는 아래 globalFeatureList 참조.
 *
 *  정의해야 할 함수의 원형은 다음과 같다.
 *
 *  void 기능_켜는_함수(feature_t*)
 *	void 기능_끄는_함수(feature_t*)
 *	uint32_t 기능_상태_확인_함수(feature_t*)
 *
 *	개조하고자 하는 기성품 오븐의 기능에 맞춰 재조정만 해주면 된다.
 */

#include "config.h"

//========================= 오븐 기능 추가 영역 시작 =========================

/*
 * 오븐의 기능을 설정하는 리스트.
 * 순서대로 {이름, 켤 때 사용할 함수, 끌 때 사용할 함수, 상태 불러올 때 사용할 함수, GPIO 뱅크, GPIO 핀}을 요소로 가진다.
 */
feature_t globalFeatureList[] = {
		// 컨벡션 모터
		{"convection", feature_gpio_low, feature_gpio_high, \
			feature_state_on_when_gpio_is_low, Motor1_GPIO_Port, Motor1_Pin},
		// 로티셰리 모터
		{"rotisserie", feature_gpio_low, feature_gpio_high, \
			feature_state_on_when_gpio_is_low, Motor2_GPIO_Port, Motor2_Pin},
		// 램프
		{"lamp", feature_gpio_low, feature_gpio_high, \
			feature_state_on_when_gpio_is_low, LAMP_GPIO_Port, LAMP_Pin},
		// 메인보드 쿨러
		{"fan", feature_gpio_high, feature_gpio_low, \
			feature_state_on_when_gpio_is_high, DCFAN_GPIO_Port, DCFAN_Pin}
};

const uint32_t globalFeatureListSize = sizeof(globalFeatureList)/sizeof(feature_t);
//========================= 오븐 기능 추가 영역 끝 =========================
