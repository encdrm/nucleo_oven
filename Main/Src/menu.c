#include "menu.h"
#include "main.h"
#include "OLED.h"
#include "Graph.h"
#include "Switch.h"
#include "profile.h"

#include <stdio.h>
#include "max6675.h"
#include "control.h"

extern SPI_HandleTypeDef hspi3;
TIM_HandleTypeDef htim3;

extern tempsensor_t *tempTop;
extern tempsensor_t *tempBottom;

extern heater_t *heaterTop;
extern heater_t *heaterBottom;
extern float tData[100];
extern float uData[100];
extern float dData[100];


extern graph_t * profile_upper;
extern graph_t * profile_lower;

void Heat2();
void test();


extern uint32_t timer;

Menu_t menuList[] = {
		{profile, "/yProfile/r$1F>", COLOR_PINK},
		{Heat2, "/yHeat/r$5F>", COLOR_SKY},
		{test, "/yTest Module/r$3F>", COLOR_RED},
};

const uint8_t menuCnt = sizeof(menuList) / sizeof(Menu_t);

void Menu_Setup(){
	OLED_Begin();
	OLED_MenuUI("STM32 OVEN", 0xFF8800, 0x000000, menuList, menuCnt > 6 ? 6 : menuCnt, 0xFFFF00);
	OLED_Cursor(0, 0xFF0000);
	SwitchLED(menuList[0].color);


	profile_upper = _Graph_Init(tData, uData, 1 + (timer / 10), 0, 52, (float)timer / 90.0f, 6.0f);
	profile_lower = _Graph_Init(tData, dData, 1 + (timer / 10), 0, 52, (float)timer / 90.0f, 6.0f);
}

void Menu(){
	Menu_Setup();
	uint8_t idx = 0;
	for(;;){
		uint16_t sw = Switch_Read();
		if(sw == SW_TOP || sw == SW_TOP_LONG){
			idx+=menuCnt - 1;
			idx %= menuCnt;
			if(menuCnt > 6 && (idx % 6 == 5 || idx == menuCnt - 1)){
				OLED_Clear();
				OLED_MenuUI("STM32 OVEN", 0xFF8800, 0x000000, menuList + (idx / 6) * 6, menuCnt - (idx / 6) * 6 > 6 ? 6 : menuCnt - (idx / 6) * 6, 0xFFFF00);
			}
			OLED_Cursor(idx%6, 0xFF0000);
			SwitchLED(menuList[idx].color);
		}
		else if(sw == SW_BOTTOM || sw == SW_BOTTOM_LONG){
			idx+=1;
			idx %= menuCnt;
			if(menuCnt > 6 && idx % 6 == 0){
				OLED_Clear();
				OLED_MenuUI("STM32 OVEN", 0xFF8800, 0x000000, menuList + idx, menuCnt - idx > 6 ? 6 : menuCnt - idx, 0xFFFF00);
			}
			OLED_Cursor(idx%6, 0xFF0000);
			SwitchLED(menuList[idx].color);
		}
		else if(sw == SW_RIGHT){
			OLED_Clear();//OLED를 지웁니다.
			menuList[idx].function();
			OLED_Clear();//OLED를 지웁니다.
			OLED_MenuUI("STM32 OVEN", 0xFF8800, 0x000000, menuList, menuCnt, 0xFFFF00);
			OLED_Cursor(idx % 6, 0xFF0000);
			SwitchLED(menuList[idx].color);
		}
	}
}


//출력 단자가 테스트 내용입니다.
Menu_t testList[] = {
		{NULL, "/wMOTR /yC:OFF R:OFF", COLOR_RED},
		{NULL, "/wLAMP /yOFF", COLOR_RED},
		{NULL, "/wHEAT /yU:0.0 D:0.0", COLOR_RED},
		{NULL, "/wTU", COLOR_RED},
		{NULL, "/wTD", COLOR_RED},
		{NULL, "/wFAN  /yOFF", COLOR_RED},
};

//출력 단자가 잘 동작하는지 테스트합니다.
void test(){
	SwitchLED(COLOR_BLACK);
	uint8_t idx = 0;
	OLED_MenuUI("< TEST", 0xFF00FF, 0x000000, testList, 6, 0x6600FF);
	OLED_Cursor(0, 0xFF0000);
	SwitchLED(testList[0].color);
	uint8_t setting = 0;
	HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 1);
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, 0);
	uint32_t heatTime = HAL_GetTick();
	for(;;){
		if(HAL_GetTick() - heatTime > 100){
			heatTime += 100;
			float tempU = tempTop->read(tempTop);
			float tempD = tempBottom->read(tempBottom);
			OLED_Printf("$45%3.2f", tempU);//민기야!
			OLED_Printf("$55%3.2f", tempD);//온도 읽어줘!
			Switch_LED_Temperature((tempU + tempD) / 2.0);//온도 읽어서 여기다가 넣어야 함.
		}
		uint16_t sw = Switch_Read();
		if((sw == SW_TOP || sw == SW_TOP_LONG) && setting){
			idx+=5;
			idx %= 6;
			OLED_Cursor(idx, 0xFF0000);
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && setting){
			idx+=1;
			idx %= 6;
			OLED_Cursor(idx, 0xFF0000);
		}
		else if(sw == SW_LEFT){
			break;
		}
		else if(sw == SW_RIGHT){
			switch(idx){
			case 0:
				setting ++;
				setting %= 3;
				switch(setting){
				case 0:
					OLED_Printf("/y$15C:%s$1BR:%s", Motor1_GPIO_Port->ODR & Motor1_Pin ? "OFF" : "ON ", Motor2_GPIO_Port->ODR & Motor2_Pin ? "OFF" : "ON ");
					break;
				case 1:
					OLED_Printf("/r$15C:%s/y$1BR:%s", Motor1_GPIO_Port->ODR & Motor1_Pin ? "OFF" : "ON ", Motor2_GPIO_Port->ODR & Motor2_Pin ? "OFF" : "ON ");
					break;
				case 2:
					OLED_Printf("/y$15C:%s/r$1BR:%s", Motor1_GPIO_Port->ODR & Motor1_Pin ? "OFF" : "ON ", Motor2_GPIO_Port->ODR & Motor2_Pin ? "OFF" : "ON ");
					break;
				}
				break;
			case 2:
				setting ++;
				setting %= 3;
				switch(setting){
				case 0:
					OLED_Printf("/y$15C:%1.1f$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				case 1:
					OLED_Printf("/r$15C:%1.1f/y$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				case 2:
					OLED_Printf("/y$15C:%1.1f/r$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				}
				break;
			case 1:
				HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, LAMP_GPIO_Port->ODR & LAMP_Pin ? 0 : 1);
				OLED_Printf("$25/y%s", LAMP_GPIO_Port->ODR & LAMP_Pin ? "OFF" : "ON");
				break;
			case 5:
				HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, DCFAN_GPIO_Port->ODR & LAMP_Pin ? 0 : 1);
				OLED_Printf("$25/y%s", DCFAN_GPIO_Port->ODR & DCFAN_Pin ? "ON " : "OFF");
				break;
			}
		}
		else if(sw == SW_ENTER){
			switch(idx){
			case 0:
				switch(setting){
				case 1:
					HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, Motor1_GPIO_Port->ODR & Motor1_Pin ? 0 : 1);
					OLED_Printf("/r$15C:%s/y$1BR:%s", Motor1_GPIO_Port->ODR & Motor1_Pin ? "OFF" : "ON ", Motor2_GPIO_Port->ODR & Motor2_Pin ? "OFF" : "ON ");
					break;
				case 2:
					HAL_GPIO_WritePin(Motor2_GPIO_Port, Motor2_Pin, Motor2_GPIO_Port->ODR & Motor2_Pin ? 0 : 1);
					OLED_Printf("/y$15C:%s/r$1BR:%s", Motor1_GPIO_Port->ODR & Motor1_Pin ? "OFF" : "ON ", Motor2_GPIO_Port->ODR & Motor2_Pin ? "OFF" : "ON ");
					break;
				}
				break;
			case 2:
				setting ++;
				setting %= 3;
				switch(setting){
				case 0:
					OLED_Printf("/y$15C:%1.1f$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				case 1:
					OLED_Printf("/r$15C:%1.1f/y$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				case 2:
					OLED_Printf("/y$15C:%1.1f/r$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				}
				break;
			case 1:
				HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, LAMP_GPIO_Port->ODR & LAMP_Pin ? 0 : 1);
				OLED_Printf("$25/y%s", LAMP_GPIO_Port->ODR & LAMP_Pin ? "OFF" : "ON");
				break;
			case 5:
				HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, DCFAN_GPIO_Port->ODR & LAMP_Pin ? 0 : 1);
				OLED_Printf("$25/y%s", DCFAN_GPIO_Port->ODR & DCFAN_Pin ? "ON " : "OFF");
				break;
			}
		}
		else if((sw == SW_TOP||sw == SW_TOP_LONG) && setting && idx == 2){
			switch(setting){
			case 1:
				if(htim3.Instance->CCR2 < 10000)
					htim3.Instance -> CCR2 += 1000;
				OLED_Printf("/r$15C:%1.1f/y$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
				break;
			case 2:
				if(htim3.Instance->CCR3 < 10000)
					htim3.Instance -> CCR3 += 1000;
				OLED_Printf("/y$15C:%1.1f/r$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
				break;
			}
		}
		else if((sw == SW_BOTTOM||sw == SW_BOTTOM_LONG) && setting && idx == 2){
			switch(setting){
			case 1:
				if(htim3.Instance->CCR2 > 0)
					htim3.Instance -> CCR2 -= 1000;
				OLED_Printf("/r$15C:%1.1f/y$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
				break;
			case 2:
				if(htim3.Instance->CCR3 > 0)
					htim3.Instance -> CCR3 -= 1000;
				OLED_Printf("/y$15C:%1.1f/r$1BR:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
				break;
			}
		}
	}

	HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, 1);
	HAL_GPIO_WritePin(Motor2_GPIO_Port, Motor2_Pin, 1);
	HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 1);
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, 0);
	htim3.Instance->CCR2 = 0;
	htim3.Instance->CCR3 = 0;

}

//출력 단자가 테스트 내용입니다.
Menu_t Heat2List[] = {
		{NULL, "/s/1/wtempU:", COLOR_SKY},
		{NULL, "/s/2/wtargetU:", COLOR_SKY},
		{NULL, "/s/3/wConvect:", COLOR_SKY},
		{NULL, "/s/4/wdutyU:", COLOR_SKY},
		{NULL, "/s/5/wstateU:", COLOR_SKY},
		{NULL, "/s/6/wEsumU:", COLOR_SKY}
};

// FLAG_TEMPSENSOR_DEBUG가 설정되면 온도를 직접 제어할 수 있음.
// 디버깅을 위한 state to string 저장소
//char *heaterStateStr[] = {"OFF", "PREHEATING", "TRANSIENT", "STEADY"};
//char *heaterStateStr2[] = {"OFF  ", "PREHT", "TRANS", "STEAD"};
void Heat2(){
	SwitchLED(COLOR_SKY);
	OLED_MenuUI("< TEST HEAT", 0xFF0000, 0x000000, Heat2List, 6, 0xFFFF00);
	OLED_Printf("/s$29/y%3.2f  \r\n", heaterTop->target);
	OLED_Printf("/s$39/y%s\r\n", (Motor1_GPIO_Port->ODR) & Motor1_Pin?"OFF":"ON ");
	OLED_Cursor(0, 0xFF6600);
	int idx = 0;
	heaterTop->start(heaterTop);
	HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_SET);	// Convection 팬 끄기
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, GPIO_PIN_SET);	// 냉각팬 켜기
	uint32_t pTime = HAL_GetTick();
	uint32_t adjust = 0;
	for(;;){
		uint16_t sw = Switch_Read();

		if(sw==SW_LEFT) break;
		else if (sw==SW_TOP && !adjust) {
			idx -= (idx>0)?1:0;
			OLED_Cursor(idx, 0xFF6600);
		}
		else if (sw==SW_BOTTOM && !adjust) {
			idx += (idx<2)?1:0;
			OLED_Cursor(idx, 0xFF6600);
		}
		else if ((sw==SW_TOP || sw==SW_TOP_LONG) && adjust) {
			switch(idx) {
#ifdef FLAG_TEMPSENSOR_DEBUG
			case 0:
				tempTop->lastTemp += 10.0f;
				break;
#endif
			case 1:
				heaterTop->target += 1.0f;
				OLED_Printf("/s$29/r%3.2f  \r\n", heaterTop->target);
				break;
			}
		}
		else if ((sw==SW_BOTTOM || sw==SW_BOTTOM_LONG) && adjust) {
			switch(idx) {
#ifdef FLAG_TEMPSENSOR_DEBUG
			case 0:
				tempTop->lastTemp -= 10.0f;
				break;
#endif
			case 1:
				heaterTop->target -= 1.0f;
				OLED_Printf("/s$29/r%3.2f  \r\n", heaterTop->target);
				break;
			}
		}
		else if(sw == SW_ENTER){
			switch(idx){
			case 1:
				adjust = !adjust;
				OLED_Printf("/s$29%s%3.2f  \r\n", adjust?"/r":"/y", heaterTop->target);
				break;
			case 2:
				HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, (Motor1_GPIO_Port->ODR) & Motor1_Pin?0:1);
				OLED_Printf("/s$39/r%s\r\n", (Motor1_GPIO_Port->ODR) & Motor1_Pin?"OFF":"ON ");
				break;
			}
		}
		float temp = tempTop->read(tempTop);
		if(HAL_GetTick() - pTime > 50){
			pTime += 50;
			Switch_LED_Temperature(temp);
		}
		OLED_Printf("/s$19/y%3.2f  \r\n", temp);
//		OLED_Printf("/s$49/p%3.2f  \r\n", heaterTop->duty);
//		OLED_Printf("/s$59/p%s     \r\n", heaterStateStr[heaterTop->state]);
//		OLED_Printf("/s$69/p%3.2f  \r\n", heaterTop->errorSum);
	}
	heaterTop->stop(heaterTop);
}












