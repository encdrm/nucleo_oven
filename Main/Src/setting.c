/*
 * settings.c
 *
 *  Created on: 2022. 9. 29.
 *      Author: user01
 *  오븐의 모든 기능 동작과 설정값을 저장하는 setting 객체 관련 함수/메소드 모음
 *
 */
#include "setting.h"
#include "config.h"
#include <stdbool.h>

//============================= Private functions =============================

__STATIC_INLINE feature_t *__get_feature(feature_t featList[], int featCount, const char *featureName);
__STATIC_INLINE bool __assert_feature(feature_t *feature);

//================================== Methods ==================================

static void setting_feature_on(setting_t * settingobj, char *featureName);
static void setting_feature_off(setting_t * settingobj, char *featureName);
static void setting_feature_toggle(setting_t * settingobj, char *featureName);
static uint32_t setting_feature_state(setting_t * settingobj, char *featureName);



/* Setting 객체를 생성하는 함수 */
setting_t *Custom_Setting(feature_t featureList[], uint32_t feature_count) {
	setting_t *settingobj = (setting_t*) calloc(1, sizeof(setting_t));

	// Setting methods
	settingobj->feature_on = setting_feature_on;
	settingobj->feature_off = setting_feature_off;
	settingobj->feature_toggle = setting_feature_toggle;
	settingobj->feature_state = setting_feature_state;

	// Setting private fields
	settingobj->_feature_list = globalFeatureList;
	settingobj->_feature_count = feature_count;

	// Setting fields

	return settingobj;
}

/* featureName 문자열과 같은 이름의 기능을 찾는 함수 */
__STATIC_INLINE feature_t *__get_feature(feature_t featList[], int featCount, const char *featureName) {
	int idx;
	for (idx=0; idx<featCount; idx++) {
		if (!strcmp(featureName, featList[idx].name)) // strcmp는 같으면 0을 반환함.
			break;
	}
	if (idx == featCount)	// Cannot find feature
		printf("Cannot find feature: %s\r\n", featureName);
	else
		return &(featList[idx]);
	return NULL;
}

/* feature 및 feature의 하위 메소드가 NULL을 가리키지는 않는지 확인하는 함수 */
__STATIC_INLINE bool __assert_feature(feature_t *feature) {
	if (feature != NULL)
		if (feature->off_function != NULL)
			if (feature->on_function != NULL)
				if (feature->get_state_function != NULL)
					// Passed
					return true;
				else printf("%s->get_state_function is NULL.\r\n", feature->name);
			else printf("%s->on_function is NULL.\r\n", feature->name);
		else printf("%s->off_function is NULL.\r\n", feature->name);
	else printf("%s not exist.\r\n", feature->name);
	// Failed
	return false;
}

/* featureName 문자열과 이름이 같은 기능을 켜는 메소드 */
static void setting_feature_on(setting_t * settingobj, char *featureName) {
	feature_t *feature = __get_feature(settingobj->_feature_list, settingobj->_feature_count, featureName);
	if (__assert_feature(feature))
		feature->on_function(feature);
}

/* featureName 문자열과 이름이 같은 기능을 끄는 메소드 */
static void setting_feature_off(setting_t * settingobj, char *featureName) {
	feature_t *feature = __get_feature(settingobj->_feature_list, settingobj->_feature_count, featureName);
		if (__assert_feature(feature))
			feature->off_function(feature);
}

/* featureName 문자열과 이름이 같은 기능을 toggle하는 메소드 */
static void setting_feature_toggle(setting_t * settingobj, char *featureName) {
	feature_t *feature = __get_feature(settingobj->_feature_list, settingobj->_feature_count, featureName);
		if (__assert_feature(feature)) {
			if (feature->get_state_function(feature))
				feature->off_function(feature);
			else
				feature->on_function(feature);
		}
}

/* featureName 문자열과 이름이 같은 기능의 상태를 반환하는 메소드 */
static uint32_t setting_feature_state(setting_t * settingobj, char *featureName) {
	feature_t *feature = __get_feature(settingobj->_feature_list, settingobj->_feature_count, featureName);
		if (__assert_feature(feature))
			return feature->get_state_function(feature);
		return 0;
}

