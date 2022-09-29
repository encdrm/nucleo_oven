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
extern TIM_HandleTypeDef htim3;

extern tempsensor_t *thermoTop;
extern tempsensor_t *thermoBottom;

extern heater_t *heaterTop;
extern heater_t *heaterBottom;
extern float tData[100];
extern float uData[100];
extern float dData[100];

extern graph_t * profile_upper;
extern graph_t * profile_lower;

void Heat2();
void Heat3();
void test();

extern uint32_t timer;
extern uint32_t time_interval;

Menu_t menuList[] = {
		{profile, "/yProfile/r$1F>", COLOR_PINK},
		{Heat2, "/yHeat/r$2F>", COLOR_SKY},
		{test, "/yTest Module/r$3F>", COLOR_RED},
		{Heat3, "/yTest PID Gain/r$4F>", COLOR_WHITE},
};

const uint8_t menuCnt = sizeof(menuList) / sizeof(Menu_t);
extern TIM_HandleTypeDef htim2;

void Menu_Setup(){
	OLED_Begin();
	OLED_MenuUI("STM32 OVEN", 0xFF8800, 0x000000, menuList, menuCnt > 6 ? 6 : menuCnt, 0xFFFF00);
	OLED_Cursor(0, 0xFF0000);
	SwitchLED(menuList[0].color);


	float tData[100] = {0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0};
	float uData[100] = {30.0, 80.0, 50.0, 80.0, 50.0, 80.0, 50.0, 60.0, 80.0, 30.0};
	float dData[100] = {30.0, 100.0, 70.0, 100.0, 70.0, 90.0, 70.0, 60.0, 50.0, 30.0};
	profile_upper = _Graph_Init(tData, uData, 1 + (timer / time_interval), 0, 52, (float)timer / 90.0f, 6.0f);
	profile_lower = _Graph_Init(tData, dData, 1 + (timer / time_interval), 0, 52, (float)timer / 90.0f, 6.0f);


	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	htim2.Instance->CCR1 = 256;
	HAL_Delay(50);
	htim2.Instance->CCR1 = 128;
	HAL_Delay(50);
	htim2.Instance->CCR1 = 64;
	HAL_Delay(50);
	htim2.Instance->CCR1 = 32;
	HAL_Delay(50);
	htim2.Instance->CCR1 = 0;

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
//수동 제어로 온도 변화를 확인할 수 있습니다.
//오븐을 끈지 얼마 지나지 않은 상태에서도 test를 통해 온도를 확인할 수 있습니다.
void test(){
	SwitchLED(COLOR_BLACK);
	uint8_t idx = 0;
	OLED_MenuUI("< TEST", 0xFF00FF, 0x000000, testList, 6, 0x6600FF);
	OLED_Cursor(0, 0xFF0000);
	SwitchLED(testList[0].color);
	uint8_t setting = 0;
	HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 1);
	htim3.Instance->CCR2 = 0;
	htim3.Instance->CCR3 = 0;
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, 0);
	uint32_t heatTime = HAL_GetTick();
	float tempTop, tempBottom;
	for(;;){
		if(HAL_GetTick() - heatTime > 100){
			heatTime += 100;
			tempTop = thermoTop->read(thermoTop);
			tempBottom = thermoBottom->read(thermoBottom);
			OLED_Printf("$45%3.2f", tempTop);
			OLED_Printf("$55%3.2f", tempBottom);
			Switch_LED_Temperature((tempTop + tempBottom) / 2.0);
		}
		uint16_t sw = Switch_Read();
		if((sw == SW_TOP || sw == SW_TOP_LONG) && !setting){
			idx+=5;
			idx %= 6;
			OLED_Cursor(idx, 0xFF0000);
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && !setting){
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
					OLED_Printf("/y$15C:%s$1BR:%s", \
							ovenSetting->feature_state(ovenSetting, "convection") ? "ON" : "OFF ", \
									ovenSetting->feature_state(ovenSetting, "rotisserie") ? "ON" : "OFF ");
					break;
				case 1:
					OLED_Printf("/r$15C:%s/y$1BR:%s", \
							ovenSetting->feature_state(ovenSetting, "convection") ? "ON" : "OFF ", \
									ovenSetting->feature_state(ovenSetting, "rotisserie") ? "ON" : "OFF ");
					break;
				case 2:
					OLED_Printf("/y$15C:%s/r$1BR:%s/y", \
							ovenSetting->feature_state(ovenSetting, "convection") ? "ON" : "OFF ", \
									ovenSetting->feature_state(ovenSetting, "rotisserie") ? "ON" : "OFF ");
					break;
				}
				break;
			case 2:
				setting ++;
				setting %= 3;
				switch(setting){
				case 0:
					OLED_Printf("/y$35U:%1.1f$3BD:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				case 1:
					OLED_Printf("/r$35U:%1.1f/y$3BD:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				case 2:
					OLED_Printf("/y$35U:%1.1f/r$3BD:%1.1f/y", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				}
				break;
			case 1:
				HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, LAMP_GPIO_Port->ODR & LAMP_Pin ? 0 : 1);
				OLED_Printf("$25/y%s", LAMP_GPIO_Port->ODR & LAMP_Pin ? "OFF" : "ON ");
				break;
			case 5:
				HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, DCFAN_GPIO_Port->ODR & DCFAN_Pin ? 0 : 1);
				OLED_Printf("$65/y%s", DCFAN_GPIO_Port->ODR & DCFAN_Pin ? "ON " : "OFF");
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
					OLED_Printf("/y$15C:%s/r$1BR:%s/y", Motor1_GPIO_Port->ODR & Motor1_Pin ? "OFF" : "ON ", Motor2_GPIO_Port->ODR & Motor2_Pin ? "OFF" : "ON ");
					break;
				}
				break;
			case 2:
				setting ++;
				setting %= 3;
				switch(setting){
				case 0:
					OLED_Printf("/y$35U:%1.1f$3BD:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				case 1:
					OLED_Printf("/r$35U:%1.1f/y$3BD:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				case 2:
					OLED_Printf("/y$35U:%1.1f/r$3BD:%1.1f/y", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
					break;
				}
				break;
			case 1:
				HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, LAMP_GPIO_Port->ODR & LAMP_Pin ? 0 : 1);
				OLED_Printf("$25/y%s", LAMP_GPIO_Port->ODR & LAMP_Pin ? "OFF" : "ON ");
				break;
			case 5:
				HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, DCFAN_GPIO_Port->ODR & DCFAN_Pin ? 0 : 1);
				OLED_Printf("$65/y%s", DCFAN_GPIO_Port->ODR & DCFAN_Pin ? "ON " : "OFF");
				break;
			}
		}
		else if((sw == SW_TOP||sw == SW_TOP_LONG) && setting && idx == 2){
			switch(setting){
			case 1:
				if(htim3.Instance->CCR2 < 10000)
					htim3.Instance -> CCR2 += 1000;
				OLED_Printf("/r$35U:%1.1f/y$3BD:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
				break;
			case 2:
				if(htim3.Instance->CCR3 < 10000)
					htim3.Instance -> CCR3 += 1000;
				OLED_Printf("/y$35U:%1.1f/r$3BD:%1.1f/y", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
				break;
			}
		}
		else if((sw == SW_BOTTOM||sw == SW_BOTTOM_LONG) && setting && idx == 2){
			switch(setting){
			case 1:
				if(htim3.Instance->CCR2 > 0)
					htim3.Instance -> CCR2 -= 1000;
				OLED_Printf("/r$35U:%1.1f/y$3BD:%1.1f", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
				break;
			case 2:
				if(htim3.Instance->CCR3 > 0)
					htim3.Instance -> CCR3 -= 1000;
				OLED_Printf("/y$35U:%1.1f/r$3BD:%1.1f/y", (float)(htim3.Instance->CCR2) / 10000, (float)(htim3.Instance->CCR3) / 10000);
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
		{NULL, "/s/1/R       U     D  ", COLOR_SKY},
		{NULL, "/s/2/wTEMP", COLOR_SKY},
		{NULL, "/s/3/wTARG", COLOR_SKY},
		{NULL, "/s/4/wTIME", COLOR_SKY},
		{NULL, "/s/5/wMOTR", COLOR_SKY},
		{NULL, "/s/6/wShow Graph     >", COLOR_SKY}
};

Menu_t HeatTimerList[] = {
		{NULL, "/s/1/RHeat!", COLOR_SKY},
		{NULL, "/s/2/wTimer:", COLOR_SKY},
		{NULL, "/s/3/wIntvl:", COLOR_SKY},

};




extern uint32_t OLED_bgColor;
void Heat2(){//Graph에 따라 분 단위로 시간 경과에 따라 온도를 설정합니다.
	OLED_Clear();
	//인터벌 및 타이머 설정 창입니다.
	OLED_MenuUI("< HEAT:/bOFF       ", 0xFF0000, 0x000000, HeatTimerList, 3, 0xFFFF00);
	int idx = 0;
	int curs = 0;
	uint8_t timersetting = 0;
	OLED_Cursor(0, 0xFF0000);
	OLED_Printf("$27/y%2d", timer);
	OLED_Printf("$37/y%2d", time_interval);
	for(;;){
		uint16_t sw = Switch_Read();
		if(sw == SW_RIGHT && idx == 0){
			break;
		}
		else if(sw == SW_RIGHT && idx == 1){
			timersetting = !timersetting;
			OLED_Printf("$27%s%2d", timersetting?"/r":"/y", timer);
		}
		else if(sw == SW_RIGHT && idx == 2){
			timersetting = !timersetting;
			OLED_Printf("$37%s%2d", timersetting?"/r":"/y", time_interval);
		}
		else if(sw == SW_LEFT && !timersetting){
			return;
		}
		else if(sw == SW_LEFT && timersetting){
			timersetting = 0;
			OLED_Printf("$27/y%2d", timer);
			OLED_Printf("$37/y%2d", time_interval);
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && timersetting){
			switch(idx){
			case 1:
				timer += time_interval;
				OLED_Printf("$27/r%2d", timer);
				break;
			case 2:
				time_interval += 1;
				OLED_Printf("$37/r%2d", time_interval);
				break;
			}
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && timersetting){
			switch(idx){
			case 1:
				if(timer > time_interval)
					timer -= time_interval;
				OLED_Printf("$27/r%2d", timer);
				break;
			case 2:
				if(time_interval > 1)
					time_interval -= 1;
				OLED_Printf("$37/r%2d", time_interval);
				break;
			}
		}
		else if(sw == SW_TOP && !timersetting){
			idx += 2;
			idx %= 3;
			OLED_Cursor(idx, 0xFF0000);
		}
		else if(sw == SW_BOTTOM && !timersetting){
			idx += 1;
			idx %= 3;
			OLED_Cursor(idx, 0xFF0000);
		}
	}
	htim2.Instance->CCR1 = 256;
	HAL_Delay(300);
	htim2.Instance->CCR1 = 0;
	Graph_Delete(profile_upper);
	Graph_Delete(profile_lower);
	graph_t * grn1 = Graph_InitNull(0, 52, timer/90.0, 6.0);
	graph_t * grn2 = Graph_InitNull(0, 52, timer/90.0, 6.0);
	OLED_Clear();
	OLED_MenuUI("< HEAT:/bON       ", 0xFF0000, 0x000000, Heat2List, 6, 0xFFFF00);
	OLED_Cursor(2, 0xFF0000);
	heaterTop -> target = 30.0f;
	heaterBottom -> target = 30.0f;
	OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
	OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
	OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
	OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
	OLED_Printf("/s$4C/y<%03d", timer);
	idx = 0;
	curs = 2;
	int heaterOn = 1;
	float tempTop = thermoTop->read(thermoTop);
	float tempBottom = thermoBottom->read(thermoBottom);
	heaterTop->start(heaterTop);
	heaterBottom->start(heaterBottom);
	HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 0);
	HAL_Delay(500);
	heaterTop -> target = 30.0f;
	heaterBottom -> target = 30.0f;
	HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_SET);	// Convection 팬 끄기
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, GPIO_PIN_SET);	// 냉각팬 켜기
	uint32_t heatTime = HAL_GetTick();
	uint32_t pTime = HAL_GetTick();
	uint32_t gTime = HAL_GetTick();
	uint32_t graphmode = 0;
	grn1->Add(grn1, 0.0f, heaterTop -> target);
	grn2->Add(grn2, 0.0f, heaterTop -> target);
	uint32_t threshold_time = 60000 * time_interval;
	uint8_t timerset = 0;
	for(;;){
		if(HAL_GetTick() - heatTime > timer * 60000 && heaterOn){
			htim2.Instance->CCR1 = 256;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 0;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 256;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 0;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 256;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 0;
			HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 1);
			heaterTop->stop(heaterTop);
			heaterBottom->stop(heaterBottom);
			heaterOn = 0;
			OLED_bgColor = 0xFF0000;
			OLED_Printf("$07/bOFF");
			OLED_bgColor = 0x000000;
		}
		uint16_t sw = Switch_Read();
		if(sw==SW_LEFT && graphmode == 0) break;
		else if(sw == SW_LEFT && graphmode){
			graphmode = 0;
			OLED_Clear();
			OLED_MenuUI("< HEAT:         ", 0xFF0000, 0x000000, Heat2List, 6, 0xFFFF00);
			OLED_bgColor = 0xFF0000;
			OLED_Printf("$07/b%s", heaterOn?"ON ":"OFF");
			OLED_bgColor = 0x000000;

			OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
			OLED_Printf("/s$3B/y%3.1f  \r\n", heaterTop->target);
			OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
			OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
			OLED_Printf("/s$4C/r<%03d", timer);
			OLED_Cursor(curs, 0xFF0000);
		}
		else if(sw == SW_ENTER && !graphmode && curs == 4){//콘벡숀 모오터 돌리기!
			HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, (Motor1_GPIO_Port->ODR) & Motor1_Pin?0:1);
			OLED_Printf("/s$55/rC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
			OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
		}
		else if(sw == SW_RIGHT){//그래프 띄우기
			if(curs == 5){
				graphmode ++;
				graphmode %= 3;
				OLED_Clear();
				gTime = HAL_GetTick();
				if(!graphmode){
					OLED_Clear();
					OLED_MenuUI("< HEAT:         ", 0xFF0000, 0x000000, Heat2List, 6, 0xFFFF00);
					OLED_bgColor = 0xFF0000;
					OLED_Printf("$07/b%s", heaterOn?"ON ":"OFF");
					OLED_bgColor = 0x000000;

					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
					OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
					OLED_Printf("/s$4C/y<%03d", timer);
					OLED_Cursor(curs, 0xFF0000);
				}
			}
			else if(curs == 4){
				HAL_GPIO_WritePin(Motor2_GPIO_Port, Motor2_Pin, (Motor2_GPIO_Port->ODR) & Motor2_Pin?0:1);
				OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
				OLED_Printf("/s$5B/rR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
			}
			else if(curs == 3){
				timerset = !timerset;
				OLED_Printf("/s$4C%s<%3d", timerset ? "/r" : "/y",timer);
			}
			else if(curs == 2){
				timerset ++;
				timerset %= 3;
				switch(timerset){
				case 0:
					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					break;
				case 1:
					OLED_Printf("/s$35/r%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					break;
				case 2:
					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/r%3.1f  \r\n", heaterBottom->target);
					break;
				}
			}
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && !graphmode && !timerset){
			if(curs > 2){
				curs --;
				OLED_Cursor(curs, 0xFF0000);
			}
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && !graphmode && !timerset){
			if(curs < 5){
				curs ++;
				OLED_Cursor(curs, 0xFF0000);
			}
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && !graphmode && timerset){
			if(curs == 3){
				timer += time_interval;
				grn1->ChangeDensity(grn1, timer / 90.0f, 6.0f);
				grn2->ChangeDensity(grn2, timer / 90.0f, 6.0f);
				OLED_Printf("/s$4C/r<%03d", timer);
			}
			else if(curs == 2){
				switch(timerset){
				case 1:
					heaterTop->target += 1.0f;
					OLED_Printf("/s$35/r%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					break;
				case 2:
					heaterBottom->target += 1.0f;
					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/r%3.1f  \r\n", heaterBottom->target);
					break;
				}
			}
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && !graphmode && timerset){
			if(curs == 3){
				timer -= time_interval;
				grn1->ChangeDensity(grn1, timer / 90.0f, 6.0f);
				grn2->ChangeDensity(grn2, timer / 90.0f, 6.0f);
				OLED_Printf("/s$4C/r<%03d", timer);
			}
			else if(curs == 2){
				switch(timerset){
				case 1:
					heaterTop->target -= 1.0f;
					OLED_Printf("/s$35/r%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					break;
				case 2:
					heaterBottom->target -= 1.0f;
					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/r%3.1f  \r\n", heaterBottom->target);
					break;
				}
			}
		}

		tempTop = thermoTop->read(thermoTop);
		tempBottom = thermoBottom->read(thermoBottom);
		if(HAL_GetTick() - heatTime > threshold_time && idx <= timer / 10 - 1){
			idx++;
			grn1->Add(grn1, idx * (float)time_interval, heaterTop->target);
			grn2->Add(grn2, idx * (float)time_interval, heaterBottom->target);
			threshold_time += time_interval * 60000UL;
		}
		if(HAL_GetTick() - pTime > 100){
			pTime += 100;
			Switch_LED_Temperature((tempTop + tempBottom) / 2.0);
			//온도 프로필에서 설정한 값의 2배 속도로 움직이게 하여 안정적으로 작동시킵니다.
			if(!graphmode){
				uint32_t tck = HAL_GetTick() - heatTime;
				OLED_Printf("/s$25/y%3.1f  \r\n", tempTop);
				OLED_Printf("/s$2B/y%3.1f  \r\n", tempBottom);
				OLED_Printf("/s$45/g%03d:%02d", tck / 60000,(tck / 1000) % 60);
			}
		}
		else if(graphmode){
			if(HAL_GetTick() - gTime > 500){
				gTime += 500;
				OLED_Clear();
				if(graphmode == 1){
					grn1 ->Print(grn1, 0x0000FF);
					Graph_PrintPoint(grn1, (float) (HAL_GetTick() - heatTime) / 60000.0f, tempTop, 0xFF4444);
				}
				else if(graphmode == 2){
					grn2 ->Print(grn2, 0x00FF00);
					Graph_PrintPoint(grn2, (float) (HAL_GetTick() - heatTime) / 60000.0f, tempBottom, 0xFF4444);
				}
			}

			OLED_Line(0, 53, 95, 53, 0xFFFF00);
		}
	}
	heaterTop->stop(heaterTop);
	heaterBottom->stop(heaterBottom);
	timer = (grn1->count - 1) * time_interval;
	profile_upper = grn1;
	profile_lower = grn2;
}




Menu_t Heat3List[] = {
		{NULL, "/s/1/R       U     D  ", COLOR_SKY},
		{NULL, "/s/2/wTEMP", COLOR_SKY},
		{NULL, "/s/3/wTARG", COLOR_SKY},
		{NULL, "/s/4/wK/gP.000I.000D.000", COLOR_SKY},
		{NULL, "/s/5/wMOTR", COLOR_SKY},
		{NULL, "/s/6/wShow Graph     >", COLOR_SKY}
};

extern uint32_t OLED_bgColor;
void Heat3(){//Graph에 따라 분 단위로 시간 경과에 따라 온도를 설정합니다.
	PIDConst PIDTransient_temp = PIDTransient;
	PIDConst PIDSteady_temp = PIDSteady;
	PIDSteady = PIDTransient;
	OLED_Clear();
	//인터벌 및 타이머 설정 창입니다.
	OLED_MenuUI("< HEAT:/bOFF       ", 0xFF0000, 0x000000, HeatTimerList, 3, 0xFFFF00);
	int idx = 0;
	int curs = 0;
	uint8_t timersetting = 0;
	OLED_Cursor(0, 0xFF0000);
	OLED_Printf("$27/y%2d", timer);
	OLED_Printf("$37/y%2d", time_interval);
	for(;;){
		uint16_t sw = Switch_Read();
		if(sw == SW_RIGHT && idx == 0){
			break;
		}
		else if(sw == SW_RIGHT && idx == 1){
			timersetting = !timersetting;
			OLED_Printf("$27%s%2d", timersetting?"/r":"/y", timer);
		}
		else if(sw == SW_RIGHT && idx == 2){
			timersetting = !timersetting;
			OLED_Printf("$37%s%2d", timersetting?"/r":"/y", time_interval);
		}
		else if(sw == SW_LEFT && !timersetting){
			return;
		}
		else if(sw == SW_LEFT && timersetting){
			timersetting = 0;
			OLED_Printf("$27/y%2d", timer);
			OLED_Printf("$37/y%2d", time_interval);
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && timersetting){
			switch(idx){
			case 1:
				timer += time_interval;
				OLED_Printf("$27/r%2d", timer);
				break;
			case 2:
				time_interval += 1;
				OLED_Printf("$37/r%2d", time_interval);
				break;
			}
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && timersetting){
			switch(idx){
			case 1:
				if(timer > time_interval)
					timer -= time_interval;
				OLED_Printf("$27/r%2d", timer);
				break;
			case 2:
				if(time_interval > 1)
					time_interval -= 1;
				OLED_Printf("$37/r%2d", time_interval);
				break;
			}
		}
		else if(sw == SW_TOP && !timersetting){
			idx += 2;
			idx %= 3;
			OLED_Cursor(idx, 0xFF0000);
		}
		else if(sw == SW_BOTTOM && !timersetting){
			idx += 1;
			idx %= 3;
			OLED_Cursor(idx, 0xFF0000);
		}
	}
	htim2.Instance->CCR1 = 256;
	HAL_Delay(300);
	htim2.Instance->CCR1 = 0;
	Graph_Delete(profile_upper);
	Graph_Delete(profile_lower);
	graph_t * grn1 = Graph_InitNull(0, 52, timer/90.0, 6.0);
	graph_t * grn2 = Graph_InitNull(0, 52, timer/90.0, 6.0);
	OLED_Clear();
	OLED_MenuUI("< HEAT:/bON       ", 0xFF0000, 0x000000, Heat3List, 6, 0xFFFF00);
	OLED_Cursor(2, 0xFF0000);
	heaterTop -> target = 30.0f;
	heaterBottom -> target = 30.0f;
	OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
	OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
	OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
	OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
	OLED_Printf("/s/y$43%03d/y$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
	idx = 0;
	curs = 2;
	int heaterOn = 1;
	float tempTop = thermoTop->read(thermoTop);
	float tempBottom = thermoBottom->read(thermoBottom);
	heaterTop->start(heaterTop);
	heaterBottom->start(heaterBottom);
	HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 0);
	HAL_Delay(500);
	heaterTop -> target = 30.0f;
	heaterBottom -> target = 30.0f;
	HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_SET);	// Convection 팬 끄기
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, GPIO_PIN_SET);	// 냉각팬 켜기
	uint32_t heatTime = HAL_GetTick();
	uint32_t pTime = HAL_GetTick();
	uint32_t gTime = HAL_GetTick();
	uint32_t graphmode = 0;
	grn1->Add(grn1, 0.0f, heaterTop -> target);
	grn2->Add(grn2, 0.0f, heaterTop -> target);
	uint32_t threshold_time = 60000 * time_interval;
	uint8_t timerset = 0;
	for(;;){
		if(HAL_GetTick() - heatTime > timer * 60000 && heaterOn){
			htim2.Instance->CCR1 = 256;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 0;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 256;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 0;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 256;
			HAL_Delay(50);
			htim2.Instance->CCR1 = 0;
			HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 1);
			heaterTop->stop(heaterTop);
			heaterBottom->stop(heaterBottom);
			heaterOn = 0;
			OLED_bgColor = 0xFF0000;
			OLED_Printf("$07/bOFF");
			OLED_bgColor = 0x000000;
		}
		uint16_t sw = Switch_Read();
		if(sw==SW_LEFT && graphmode == 0) break;
		else if(sw == SW_LEFT && graphmode){
			graphmode = 0;
			OLED_Clear();
			OLED_MenuUI("< HEAT:         ", 0xFF0000, 0x000000, Heat3List, 6, 0xFFFF00);
			OLED_bgColor = 0xFF0000;
			OLED_Printf("$07/b%s", heaterOn?"ON ":"OFF");
			OLED_bgColor = 0x000000;

			OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
			OLED_Printf("/s$3B/y%3.1f  \r\n", heaterTop->target);
			OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
			OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
			OLED_Printf("/s/y$43%03d/y$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
			OLED_Cursor(curs, 0xFF0000);
		}
		else if(sw == SW_ENTER && !graphmode && curs == 4){//콘벡숀 모오터 돌리기!
			HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, (Motor1_GPIO_Port->ODR) & Motor1_Pin?0:1);
			OLED_Printf("/s$55/rC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
			OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
		}
		else if(sw == SW_RIGHT){//그래프 띄우기
			if(curs == 5){
				graphmode ++;
				graphmode %= 3;
				OLED_Clear();
				gTime = HAL_GetTick();
				if(!graphmode){
					OLED_Clear();
					OLED_MenuUI("< HEAT:         ", 0xFF0000, 0x000000, Heat3List, 6, 0xFFFF00);
					OLED_bgColor = 0xFF0000;
					OLED_Printf("$07/b%s", heaterOn?"ON ":"OFF");
					OLED_bgColor = 0x000000;

					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
					OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
					OLED_Printf("/s/y$43%03d/y$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					OLED_Cursor(curs, 0xFF0000);
				}
			}
			else if(curs == 4){
				HAL_GPIO_WritePin(Motor2_GPIO_Port, Motor2_Pin, (Motor2_GPIO_Port->ODR) & Motor2_Pin?0:1);
				OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
				OLED_Printf("/s$5B/rR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
			}
			else if(curs == 3){
				timerset ++;
				timerset %= 4;
				switch(timerset){
				case 1:
					OLED_Printf("/s/r$43%03d/y$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				case 2:
					OLED_Printf("/s/y$43%03d/r$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				case 3:
					OLED_Printf("/s/y$43%03d/y$48%03d/r$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				default:
					OLED_Printf("/s/y$43%03d/y$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				}
			}
			else if(curs == 2){
				timerset ++;
				timerset %= 3;
				switch(timerset){
				case 0:
					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					break;
				case 1:
					OLED_Printf("/s$35/r%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					break;
				case 2:
					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/r%3.1f  \r\n", heaterBottom->target);
					break;
				}
			}
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && !graphmode && !timerset){
			if(curs > 2){
				curs --;
				OLED_Cursor(curs, 0xFF0000);
			}
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && !graphmode && !timerset){
			if(curs < 5){
				curs ++;
				OLED_Cursor(curs, 0xFF0000);
			}
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && !graphmode && timerset){
			if(curs == 3){
				switch(timerset){
				case 1:
					PIDTransient.kp += 0.005;
					PIDSteady.kp += 0.005;
					OLED_Printf("/s/r$43%03d/y$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				case 2:
					PIDTransient.ki += 0.005;
					PIDSteady.ki += 0.005;
					OLED_Printf("/s/y$43%03d/r$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				case 3:
					PIDTransient.kd += 0.005;
					PIDSteady.kd += 0.005;
					OLED_Printf("/s/y$43%03d/y$48%03d/r$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				}
			}
			else if(curs == 2){
				switch(timerset){
				case 1:
					heaterTop->target += 1.0f;
					OLED_Printf("/s$35/r%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					break;
				case 2:
					heaterBottom->target += 1.0f;
					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/r%3.1f  \r\n", heaterBottom->target);
					break;
				}
			}
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && !graphmode && timerset){
			if(curs == 3){
				switch(timerset){
				case 1:
					PIDTransient.kp -= 0.005;
					PIDSteady.kp -= 0.005;
					OLED_Printf("/s/r$43%03d/y$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				case 2:
					PIDTransient.ki -= 0.005;
					PIDSteady.ki -= 0.005;
					OLED_Printf("/s/y$43%03d/r$48%03d/y$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				case 3:
					PIDTransient.kd -= 0.005;
					PIDSteady.kd -= 0.005;
					OLED_Printf("/s/y$43%03d/y$48%03d/r$4D%03d", (int)(1000*PIDTransient.kp), (int)(1000*PIDTransient.ki), (int)(1000*PIDTransient.kd));
					break;
				}
			}
			else if(curs == 2){
				switch(timerset){
				case 1:
					heaterTop->target -= 1.0f;
					OLED_Printf("/s$35/r%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					break;
				case 2:
					heaterBottom->target -= 1.0f;
					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/r%3.1f  \r\n", heaterBottom->target);
					break;
				}
			}
		}

		tempTop = thermoTop->read(thermoTop);
		tempBottom = thermoBottom->read(thermoBottom);
		if(HAL_GetTick() - heatTime > threshold_time && idx <= timer / 10 - 1){
			idx++;
			grn1->Add(grn1, idx * (float)time_interval, tempTop);
			grn2->Add(grn2, idx * (float)time_interval, tempBottom);
			threshold_time += time_interval * 60000UL;
		}
		if(HAL_GetTick() - pTime > 200){
			pTime += 200;
			Switch_LED_Temperature((tempTop + tempBottom) / 2.0);
			//온도 프로필에서 설정한 값의 2배 속도로 움직이게 하여 안정적으로 작동시킵니다.
			if(!graphmode){
				OLED_Printf("/s$25/y%3.1f  \r\n", tempTop);
				OLED_Printf("/s$2B/y%3.1f  \r\n", tempBottom);
				OLED_Printf("/s$10/yDUTY$15%1.2f$1B%1.2f", heaterTop->duty, heaterBottom->duty);
			}
		}
		else if(graphmode){
			if(HAL_GetTick() - gTime > 500){
				gTime += 500;
				OLED_Clear();
				if(graphmode == 1){
					grn1 ->Print(grn1, 0x0000FF);
					Graph_PrintPoint(grn1, (float) (HAL_GetTick() - heatTime) / 60000.0f, tempTop, 0xFF4444);
				}
				else if(graphmode == 2){
					grn2 ->Print(grn2, 0x00FF00);
					Graph_PrintPoint(grn2, (float) (HAL_GetTick() - heatTime) / 60000.0f, tempBottom, 0xFF4444);
				}
			}

			OLED_Line(0, 53, 95, 53, 0xFFFF00);
		}
	}
	heaterTop->stop(heaterTop);
	heaterBottom->stop(heaterBottom);
	timer = (grn1->count - 1) * time_interval;
	profile_upper = grn1;
	profile_lower = grn2;

	PIDTransient = PIDTransient_temp;
	PIDSteady = PIDSteady_temp;
}


