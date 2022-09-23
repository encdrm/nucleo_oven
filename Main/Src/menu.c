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

void testTemp();
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
void testHeat();




void profile(){}
void test();
void DCFan_Set(uint8_t level);
void GraphUITest(){
	float xData[10] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
	float yData[10] = {30.0, 80.0, 50.0, 80.0, 50.0, 80.0, 50.0, 60.0, 80.0, 60.0};
	graph_t * g = Graph_InitEdge(xData, yData, 0.1, 6.0);
	Graph_UI(g);
	Graph_Delete(g);
}

Menu_t menuList[] = {
		{profile, "Profile", COLOR_PINK},
		{testTemp, "TestTemp", COLOR_WHITE},
		{test, "Test", COLOR_RED},
		{GraphUITest, "TestGraph", COLOR_BLUE},
		{testHeat, "TestHeat", COLOR_SKY},

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
					// Convection Fan
					HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, 0);
					OLED_Printf("/1Motor1: ON ");
					break;
				case 1:
					// Rotisserie Motor
					HAL_GPIO_WritePin(Motor2_GPIO_Port, Motor2_Pin, 0);
					OLED_Printf("/2Motor2: ON ");
					break;
				case 2:
					// Lamp
					HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 0);
					OLED_Printf("/3LAMP  : ON ");
					break;
				case 3:
					// Heater Top
					HAL_GPIO_WritePin(Heater_Top_GPIO_Port, Heater_Top_Pin, 0);
					OLED_Printf("/4HU    : ON ");
					break;
				case 4:
					// Heater Bottom
					HAL_GPIO_WritePin(Heater_Bottom_GPIO_Port, Heater_Bottom_Pin, 0);
					OLED_Printf("/5HD    : ON ");
					break;
				case 5:
					// Mainboard Fan
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
					HAL_GPIO_WritePin(Heater_Top_GPIO_Port, Heater_Top_Pin, 1);
					OLED_Printf("/4HU    : OFF");
					break;
				case 4:
					HAL_GPIO_WritePin(Heater_Bottom_GPIO_Port, Heater_Bottom_Pin, 1);
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
	HAL_GPIO_WritePin(Heater_Top_GPIO_Port, Heater_Top_Pin, 1);
	HAL_GPIO_WritePin(Heater_Bottom_GPIO_Port, Heater_Bottom_Pin, 1);
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, 0);

}

//출력 단자가 테스트 내용입니다.
Menu_t testHeatList[] = {
		{NULL, "/s/1/rtempU:", COLOR_SKY},
		{NULL, "/s/2/rtargetU:", COLOR_SKY},
		{NULL, "/s/3/rConvect:", COLOR_SKY},
		{NULL, "/s/4/wdutyU:", COLOR_SKY},
		{NULL, "/s/5/wstateU:", COLOR_SKY},
		{NULL, "/s/6/wEsumU:", COLOR_SKY}
};

// FLAG_TEMPSENSOR_DEBUG가 설정되면 온도를 직접 제어할 수 있음.
// 디버깅을 위한 state to string 저장소
char *heaterStateStr[] = {"OFF", "PREHEATING", "TRANSIENT", "STEADY"};
void testHeat(){
	SwitchLED(COLOR_SKY);
	OLED_MenuUI("TEST HEAT", 0xFF0000, 0x000000, testHeatList, 6, 0xFFFF00);
	OLED_Printf("/s$29/y%3.2f  \r\n", heaterTop->target);
	OLED_Printf("/s$39/y%s\r\n", (Motor1_GPIO_Port->ODR) & Motor1_Pin?"OFF":"ON ");
	OLED_Cursor(0, 0xFF6600);
	int idx = 0;
	heaterTop->start(heaterTop);
	HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_SET);	// Convection 팬 끄기
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, GPIO_PIN_SET);	// 냉각팬 켜기
	uint32_t pTime = HAL_GetTick();
	for(;;){
		uint16_t sw = Switch_Read();

		if(sw==SW_ENTER) break;
		else if (sw==SW_TOP) {
			idx -= (idx>0)?1:0;
			OLED_Cursor(idx, 0xFF6600);
		}
		else if (sw==SW_BOTTOM) {
			idx += (idx<2)?1:0;
			OLED_Cursor(idx, 0xFF6600);
		}
		else if (sw==SW_RIGHT || sw==SW_RIGHT_LONG) {
			switch(idx) {
#ifdef FLAG_TEMPSENSOR_DEBUG
			case 0:
				tempTop->lastTemp += 10.0f;
				break;
#endif
			case 1:
				heaterTop->target += 1.0f;
				OLED_Printf("/s$29/y%3.2f  \r\n", heaterTop->target);
				break;
			case 2:
				HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_RESET);
				OLED_Printf("/s$39/y%s\r\n", (Motor1_GPIO_Port->ODR) & Motor1_Pin?"OFF":"ON ");
				break;
			}
		}
		else if (sw==SW_LEFT || sw==SW_LEFT_LONG) {
			switch(idx) {
#ifdef FLAG_TEMPSENSOR_DEBUG
			case 0:
				tempTop->lastTemp -= 10.0f;
				break;
#endif
			case 1:
				heaterTop->target -= 1.0f;
				OLED_Printf("/s$29/y%3.2f  \r\n", heaterTop->target);
				break;
			case 2:
				HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_SET);
				OLED_Printf("/s$39/y%s\r\n", (Motor1_GPIO_Port->ODR) & Motor1_Pin?"OFF":"ON ");
				break;
			}
		}
		float temp = tempTop->read(tempTop);
		if(HAL_GetTick() - pTime > 50){
			pTime += 50;
			Switch_LED_Temperature(temp);
		}
		OLED_Printf("/s$19/y%3.2f  \r\n", temp);
		OLED_Printf("/s$49/p%3.2f  \r\n", heaterTop->duty);
		OLED_Printf("/s$59/p%s     \r\n", heaterStateStr[heaterTop->state]);
		OLED_Printf("/s$69/p%3.2f  \r\n", heaterTop->errorSum);
	}
	heaterTop->stop(heaterTop);
}


Menu_t HeatList[] = {
		{NULL, "/s/1/rtempU:", COLOR_SKY},
		{NULL, "/s/2/rtargetU:", COLOR_SKY},
		{NULL, "/s/3/rConvect:", COLOR_SKY},
		{NULL, "/s/4/wdutyU:", COLOR_SKY},
		{NULL, "/s/5/wstateU:", COLOR_SKY},
		{NULL, "/s/6/wEsumU:", COLOR_SKY}
};
void Heat(graph_t * gr){//Graph에 따라 분 단위로 시간 경과에 따라 온도를 설정합니다.
	OLED_Clear();
	OLED_MenuUI("HEAT", 0xFF0000, 0x000000, testHeatList, 6, 0xFFFF00);
	heaterTop -> target = gr->yData[0];
	OLED_Printf("/s$29/y%3.2f  \r\n", heaterTop->target);
	OLED_Printf("/s$39/y%s\r\n", (Motor1_GPIO_Port->ODR) & Motor1_Pin?"OFF":"ON ");
	OLED_Cursor(2, 0xFF6600);
	int idx = 0;
	float interval = (gr->xData[idx + 1] - gr->xData[idx]) * 60000.00;
	float target1 = gr->yData[idx];
	float target2 = gr->yData[idx + 1];
	heaterTop->start(heaterTop);
	HAL_Delay(500);
	heaterTop -> target = target1;
	HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_SET);	// Convection 팬 끄기
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, GPIO_PIN_SET);	// 냉각팬 켜기
	uint32_t heatTime = HAL_GetTick();
	uint32_t pTime = HAL_GetTick();
	uint32_t gTime = HAL_GetTick();
	uint32_t graphmode = 0;
	for(;;){
		uint16_t sw = Switch_Read();
		if(sw==SW_ENTER) break;
		else if(sw == SW_LEFT && !graphmode){
			HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_SET);
			OLED_Printf("/s$39/y%s\r\n", (Motor1_GPIO_Port->ODR) & Motor1_Pin?"OFF":"ON ");
		}
		else if(sw == SW_RIGHT && !graphmode){
			HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_RESET);
			OLED_Printf("/s$39/y%s\r\n", (Motor1_GPIO_Port->ODR) & Motor1_Pin?"OFF":"ON ");
		}
		else if(sw == SW_TOP){
			graphmode = !graphmode;
			OLED_Clear();
			gTime = HAL_GetTick();
		}

		float temp = tempTop->read(tempTop);
		if(HAL_GetTick() - heatTime > gr->xData[idx + 1] && idx < gr->count - 2){
			idx++;
			interval = (gr->xData[idx + 1] - gr->xData[idx]) * 60000.00;
			target1 = gr->yData[idx];
			target2 = gr->yData[idx + 1];
		}
		if(HAL_GetTick() - pTime > 50){
			pTime += 50;
			Switch_LED_Temperature(temp);
			if(heaterTop->target < target2){
				heaterTop->target += 100.0 * ((target2 - target1) > 0? (target2 - target1) : (target1 - target2)) / interval;
				if(heaterTop -> target > target2){
					heaterTop->target = target2;
				}
			}
			else if(heaterTop->target > target2){
				heaterTop->target -= 100.0 * ((target2 - target1) > 0? (target2 - target1) : (target1 - target2)) / interval;
				if(heaterTop -> target < target2){
					heaterTop->target = target2;
				}
			}
			if(!graphmode)
			OLED_Printf("/s$29/y%3.2f  \r\n", heaterTop->target);
		}
		if(!graphmode){
			OLED_Printf("/s$19/y%3.2f  \r\n", temp);
			OLED_Printf("/s$49/p%3.2f  \r\n", heaterTop->duty);
			OLED_Printf("/s$59/p%s     \r\n", heaterStateStr[heaterTop->state]);
			OLED_Printf("/s$69/p%3.2f  \r\n", heaterTop->errorSum);
		}
		else if(graphmode){
			if(HAL_GetTick() - gTime > 500){
				gTime += 500;
				OLED_Clear();
				gr ->Print(gr, 0xFF0000);
				Graph_PrintPoint(gr, (float) (HAL_GetTick() - heatTime) / 60000.0f, heaterTop->target, 0x00FF00);
			}
		}
	}
	heaterTop->stop(heaterTop);
}

