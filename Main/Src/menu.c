#include "menu.h"
#include "main.h"
#include "OLED.h"
#include "Graph.h"
#include "Switch.h"

#include <stdio.h>
#include "max6675.h"
#include "control.h"

extern SPI_HandleTypeDef hspi3;

extern tempsensor_t *tempTop;
extern tempsensor_t *tempBottom;

extern heater_t *heaterTop;
extern heater_t *heaterBottom;


void testTemp(){
	SwitchLED(COLOR_BLACK);
	for(;;){
		uint16_t sw = Switch_Read();
		if(sw) break;
		if (tempTop->is_readable(tempTop) && tempBottom->is_readable(tempBottom)) {
			float temp1 = tempTop->read(tempTop);
			float temp2 = tempBottom->read(tempBottom);
			OLED_Printf("/s/0/rtempU: /y%f\r\n", temp1);
			OLED_Printf("/s/1/rtempD: /y%f\r\n", temp2);
//			printf("temp: %f\r\n", temp1->read(temp1));
		}
	}
}

// FLAG_TEMPSENSOR_DEBUG가 설정되어 있어야 온도를 직접 제어할 수 있다.
void testHeat(){
	SwitchLED(COLOR_SKY);
	uint16_t idx = 0;
	heaterTop->start(heaterTop);
	for(;;){
		uint16_t sw = Switch_Read();

		if(sw==SW_LEFT_LONG) break;
		else if (sw==SW_LEFT) idx = 0;
		else if (sw==SW_RIGHT) idx = 1;
		else if (sw==SW_TOP || sw==SW_TOP_LONG) {
			if (idx)
				heaterTop->target += 1.0f;
#ifdef FLAG_TEMPSENSOR_DEBUG
			else
				tempTop->lastTemp += 10.0f;
#endif
		}
		else if (sw==SW_BOTTOM || sw==SW_BOTTOM_LONG) {
			if (idx)
				heaterTop->target -= 1.0f;
#ifdef FLAG_TEMPSENSOR_DEBUG
			else
				tempTop->lastTemp -= 10.0f;
#endif
		}
		float temp = tempTop->read(tempTop);
		OLED_Printf("/s/0/rSelect: /p%s\r\n", (idx)?"target":"temp  ");
		OLED_Printf("/s/1/rtempU: /y%f\r\n", temp);
		OLED_Printf("/s/2/rtargetU: /y%f\r\n", heaterTop->target);
		OLED_Printf("/s/3/rdutyU: /y%f\r\n", heaterTop->duty);
	}
	heaterTop->stop(heaterTop);
}



void profile(){}
void test();
void DCFan_Set(uint8_t level);


Menu_t menuList[] = {
		{profile, "Profile", COLOR_PINK},
		{testTemp, "TestTemp", COLOR_WHITE},
		{test, "Test", COLOR_RED},
		{test, "Test2", COLOR_BLUE},
		{testHeat, "TestHeat", COLOR_SKY},
		{test, "Test4", COLOR_BLACK},
		{test, "Test5", COLOR_RED},

};

const uint8_t menuCnt = sizeof(menuList) / sizeof(Menu_t);

void Menu_Setup(){
	OLED_Begin();
	OLED_MenuUI("STM32 OVEN", 0xFF8800, 0x000000, menuList, menuCnt > 6 ? 6 : menuCnt, 0xFFFF00);
	OLED_Cursor(0, 0xFF0000);
	SwitchLED(menuList[0].color);
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
		else if(sw == SW_ENTER){
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
		{NULL, "Motor1: OFF", COLOR_RED},
		{NULL, "Motor2: OFF", COLOR_RED},
		{NULL, "LAMP  : OFF", COLOR_RED},
		{NULL, "HU    : OFF", COLOR_RED},
		{NULL, "HD    : OFF", COLOR_RED},
		{NULL, "FAN   : OFF", COLOR_RED},
};


//출력 단자가 잘 동작하는지 테스트합니다.
void test(){
	uint8_t idx = 0;
	OLED_MenuUI("TEST", 0xFF00FF, 0x000000, testList, 6, 0x6600FF);
	OLED_Cursor(0, 0xFF0000);
	SwitchLED(testList[0].color);
	testList[0].color = COLOR_RED;
	testList[1].color = COLOR_RED;
	testList[2].color = COLOR_RED;
	testList[3].color = COLOR_RED;
	testList[4].color = COLOR_RED;
	testList[5].color = COLOR_RED;
	for(;;){
		uint16_t sw = Switch_Read();
		if(sw == SW_TOP || sw == SW_TOP_LONG){
			idx+=5;
			idx %= 6;
			OLED_Cursor(idx, 0xFF0000);
			SwitchLED(testList[idx].color);
		}
		else if(sw == SW_BOTTOM || sw == SW_BOTTOM_LONG){
			idx+=1;
			idx %= 6;
			OLED_Cursor(idx, 0xFF0000);
			SwitchLED(testList[idx].color);
		}
		else if(sw == SW_ENTER){
			break;
		}
		else if(sw == SW_LEFT || sw == SW_RIGHT){
			if(testList[idx].color == COLOR_RED){
				testList[idx].color = COLOR_WHITE;
				switch(idx){
				case 0:
					HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, 0);
					OLED_Printf("/1Motor1: ON ");
					break;
				case 1:
					HAL_GPIO_WritePin(Motor2_GPIO_Port, Motor2_Pin, 0);
					OLED_Printf("/2Motor2: ON ");
					break;
				case 2:
					HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 0);
					OLED_Printf("/3LAMP  : ON ");
					break;
				case 3:
					HAL_GPIO_WritePin(HU_GPIO_Port, HU_Pin, 0);
					OLED_Printf("/4HU    : ON ");
					break;
				case 4:
					HAL_GPIO_WritePin(HD_GPIO_Port, HD_Pin, 0);
					OLED_Printf("/5HD    : ON ");
					break;
				case 5:
					HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, 1);
					OLED_Printf("/6FAN   : ON ");
					break;
				}
			}
			else if(testList[idx].color == COLOR_WHITE){
				testList[idx].color = COLOR_RED;
				switch(idx){
				case 0:
					HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, 1);
					OLED_Printf("/1Motor1: OFF");
					break;
				case 1:
					HAL_GPIO_WritePin(Motor2_GPIO_Port, Motor2_Pin, 1);
					OLED_Printf("/2Motor2: OFF");
					break;
				case 2:
					HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 1);
					OLED_Printf("/3LAMP  : OFF");
					break;
				case 3:
					HAL_GPIO_WritePin(HU_GPIO_Port, HU_Pin, 1);
					OLED_Printf("/4HU    : OFF");
					break;
				case 4:
					HAL_GPIO_WritePin(HD_GPIO_Port, HD_Pin, 1);
					OLED_Printf("/5HD    : OFF");
					break;
				case 5:
					HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, 0);
					OLED_Printf("/6FAN   : OFF");
					break;
				}
			}

			SwitchLED(testList[idx].color);
		}
	}

	HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, 1);
	HAL_GPIO_WritePin(Motor2_GPIO_Port, Motor2_Pin, 1);
	HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 1);
	HAL_GPIO_WritePin(HU_GPIO_Port, HU_Pin, 1);
	HAL_GPIO_WritePin(HD_GPIO_Port, HD_Pin, 1);
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, 0);

}




