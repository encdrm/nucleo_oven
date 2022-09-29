/*
 * setting.h
 *
 *  Created on: 2022. 9. 30.
 *      Author: user01
 */

#ifndef INC_SETTING_H_
#define INC_SETTING_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "feature.h"


//========================= Type definitions & Structs =========================

typedef struct _setting_t setting_t;
typedef struct _setting_t {
	// Methods
	void (*feature_on)(setting_t * settingobj, char *featureName);			// 기능을 켜기 위한 객체
	void (*feature_off)(setting_t * settingobj, char *featureName);  		// 기능을 끄기 위한 객체
	void (*feature_toggle)(setting_t * settingobj, char *featureName);  	// 기능을 toggle
	uint32_t (*feature_state)(setting_t * settingobj, char *featureName);	// 기능 상태 반환하는 객체

	// Private fields
	feature_t *_feature_list;	// 기능 정의를 담은 리스트
	uint32_t _feature_count;	// 기능 개수

	// Fields
	uint32_t heatruntime;		// 현재 히터 가동 시간(ms)(임시)
} setting_t;

//============================== Public functions ==============================
setting_t *Custom_Setting(feature_t featureList[], uint32_t feature_count);

#endif /* INC_SETTING_H_ */
