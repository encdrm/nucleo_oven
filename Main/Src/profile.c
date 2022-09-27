#include "profile.h"


extern tempsensor_t *tempTop;
extern tempsensor_t *tempBottom;

graph_t * profile_upper;
graph_t * profile_lower;
uint32_t timer = 90;
float tData[100] = {0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0};
float uData[100] = {30.0, 80.0, 50.0, 80.0, 50.0, 80.0, 50.0, 60.0, 80.0, 30.0};
float dData[100] = {30.0, 100.0, 70.0, 100.0, 70.0, 90.0, 70.0, 60.0, 50.0, 30.0};
void Heat(graph_t * gr1, graph_t * gr2);


Menu_t profileList[] = {
		{NULL, "/yGetProfile/r$1F>", COLOR_PINK},
		{NULL, "/yTimer: /w90min/r$2F>", COLOR_PINK},
		{NULL, "/yProfileSet/r$3F>", COLOR_PINK},
		{NULL, "/oHeat/r$4F>", COLOR_PINK},
};
void profile(){
	graph_t * g1 = profile_upper;
	graph_t * g2 = profile_lower;
	timer = 90;
	SwitchLED(COLOR_SKY);
	OLED_MenuUI("< Profile", 0xFF0000, 0x000000, profileList, 4, 0xFFFF00);
	OLED_Cursor(0, 0xFF6600);
	int idx = 0;
	uint32_t sw = 0;
	uint32_t timerSetting = 0;
	for(;;){
		sw = Switch_Read();
		if(sw == SW_LEFT && !timerSetting) break;
		else if(sw == SW_LEFT && timerSetting){
			timerSetting = !timerSetting;
			OLED_Printf("$27%s%dmin", timerSetting?"/r":"/w", timer);
		}
		else if(sw == SW_TOP && !timerSetting){
			idx += 3;
			idx %= 4;
			OLED_Cursor(idx, 0xFF6600);
		}
		else if(sw == SW_BOTTOM && !timerSetting){
			idx += 1;
			idx %= 4;
			OLED_Cursor(idx, 0xFF6600);
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && timerSetting){
			if(timer < 300){
				timer += 10;
				tData[timer/10] = (float)timer;
				uData[timer/10] = 30.0f;
				dData[timer/10] = 30.0f;
				Graph_Delete(g1);
				Graph_Delete(g2);
				g1 = _Graph_Init(tData, uData, 1 + (timer / 10), 0, 52, (float)timer / 90.0f, 6.0f);
				g2 = _Graph_Init(tData, dData, 1 + (timer / 10), 0, 52, (float)timer / 90.0f, 6.0f);
				OLED_Printf("$27/r%dmin", timer);
			}
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && timerSetting){
			if(timer > 0){
				timer -= 10;
				Graph_Delete(g1);
				Graph_Delete(g2);
				g1 = _Graph_Init(tData, uData, 1 + (timer / 10), 0, 52, (float)timer / 90.0f, 6.0f);
				g2 = _Graph_Init(tData, dData, 1 + (timer / 10), 0, 52, (float)timer / 90.0f, 6.0f);
				OLED_Printf("$27/r%dmin", timer);
			}
		}
		else if(sw == SW_RIGHT){
			if(idx != 1){
				OLED_Clear();
			}
			switch(idx){
			case 0:
				//그래프 데이터 받아오기
				break;
			case 1:
				timerSetting = !timerSetting;
				OLED_Printf("$27%s%dmin", timerSetting?"/r":"/w", timer);
				break;
			case 2:
				Profile_Set(g1, g2);
				break;
			case 3:
				Heat(g1, g2);
				break;
			}
			if(idx != 1){
				OLED_Clear();
				SwitchLED(COLOR_SKY);
				OLED_MenuUI("< Profile", 0xFF0000, 0x000000, profileList, 4, 0xFFFF00);
				OLED_Printf("$27/w%dmin", timer);
				OLED_Cursor(idx, 0xFF6600);
			}
		}
	}
	Graph_Delete(g1);
	Graph_Delete(g2);
}

void Profile_Set(graph_t * gr1, graph_t * gr2){
	uint16_t idx = 0;
	OLED_Line(0, 53, 95, 53, 0xFF00FF);
	gr1 -> Print(gr1, 0x0000FF);
	gr2 -> Print(gr2, 0x00FF00);
	_Graph_PrintPoint(gr1, idx, 0xFF8800);
	_Graph_PrintPoint(gr2, idx, 0xFF8800);
	OLED_Printf("/s/k$60<$6F>");
	OLED_Printf("/s$62/rt%03d$66/yu%03d$6A/yd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
	uint32_t pTime = HAL_GetTick();
	uint32_t state = 0;
	for(;;){
		uint16_t sw = Switch_Read();
		if((sw == SW_TOP || sw == SW_TOP_LONG) && idx < gr1->count - 1 && state == 0){
			idx ++;
			OLED_Clear();
			OLED_Line(0, 53, 95, 53, 0xFF00FF);
			gr1 -> Print(gr1, 0x0000FF);
			gr2 -> Print(gr2, 0x00FF00);
			_Graph_PrintPoint(gr1, idx, 0xFF8800);
			_Graph_PrintPoint(gr2, idx, 0xFF8800);
			OLED_Printf("/s/k$60<$6F>");
			OLED_Printf("/s$62/rt%03d$66/yu%03d$6A/yd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			pTime = HAL_GetTick();
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && idx > 0 && state == 0){
			idx --;
			OLED_Clear();
			OLED_Line(0, 53, 95, 53, 0xFF00FF);
			gr1 -> Print(gr1, 0x0000FF);
			gr2 -> Print(gr2, 0x00FF00);
			_Graph_PrintPoint(gr1, idx, 0xFF8800);
			_Graph_PrintPoint(gr2, idx, 0xFF8800);
			OLED_Printf("/s/p$60<$6F>");
			OLED_Printf("/s$62/rt%03d$66/yu%03d$6A/yd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			pTime = HAL_GetTick();
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && gr1->yData[idx] < 299.0f && state == 1){
			gr1->yData[idx] += 5.0f;
			OLED_Clear();
			OLED_Line(0, 53, 95, 53, 0xFF00FF);
			gr2 -> Print(gr2, 0x00FF00);
			gr1 -> Print(gr1, 0x0000FF);
			_Graph_PrintPoint(gr1, idx, 0xFF8800);
			_Graph_PrintPoint(gr2, idx, 0xFF8800);
			OLED_Printf("/s/p$60<$6F>");
			OLED_Printf("/s$62/yt%03d$66/ru%03d$6A/yd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			pTime = HAL_GetTick();
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && gr1->yData[idx] > 4.0f && state == 1){
			gr1->yData[idx] -= 5.0f;
			OLED_Clear();
			OLED_Line(0, 53, 95, 53, 0xFF00FF);
			gr2 -> Print(gr2, 0x00FF00);
			gr1 -> Print(gr1, 0x0000FF);
			_Graph_PrintPoint(gr1, idx, 0xFF8800);
			_Graph_PrintPoint(gr2, idx, 0xFF8800);
			OLED_Printf("/s/p$60<$6F>");
			OLED_Printf("/s$62/yt%03d$66/ru%03d$6A/yd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			pTime = HAL_GetTick();
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && gr2->yData[idx] < 299.0f && state == 2){
			gr2->yData[idx] += 5.0f;
			OLED_Clear();
			OLED_Line(0, 53, 95, 53, 0xFF00FF);
			gr1 -> Print(gr1, 0x0000FF);
			gr2 -> Print(gr2, 0x00FF00);
			_Graph_PrintPoint(gr1, idx, 0xFF8800);
			_Graph_PrintPoint(gr2, idx, 0xFF8800);
			OLED_Printf("/s/p$60<$6F>");
			OLED_Printf("/s$62/yt%03d$66/yu%03d$6A/rd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			pTime = HAL_GetTick();
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && gr2->yData[idx] > 4.0f && state == 2){
			gr2->yData[idx] -= 5.0f;
			OLED_Clear();
			OLED_Line(0, 53, 95, 53, 0xFF00FF);
			gr1 -> Print(gr1, 0x0000FF);
			gr2 -> Print(gr2, 0x00FF00);
			_Graph_PrintPoint(gr1, idx, 0xFF8800);
			_Graph_PrintPoint(gr2, idx, 0xFF8800);
			OLED_Printf("/s/p$60<$6F>");
			OLED_Printf("/s$62/yt%03d$66/yu%03d$6A/rd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			pTime = HAL_GetTick();
		}
		else if(sw == SW_ENTER){
			state = (state + 1) % 3;
			OLED_Printf("/s/p$60<$6F>");
			if(state==2){
				OLED_Printf("/s$62/yt%03d$66/yu%03d$6A/rd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			}
			else if(state==1){
				OLED_Printf("/s$62/yt%03d$66/ru%03d$6A/yd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			}
			else{
				OLED_Printf("/s$62/rt%03d$66/yu%03d$6A/yd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			}
		}
		else if(sw == SW_LEFT){
			break;
		}
		/*else if(sw == SW_RIGHT){
			Heat2(gr1, gr2);

			OLED_Clear();
			OLED_Line(0, 53, 95, 53, 0xFF00FF);
			gr1 -> Print(gr1, 0x0000FF);
			gr2 -> Print(gr2, 0x00FF00);
			_Graph_PrintPoint(gr1, idx, 0xFF8800);
			_Graph_PrintPoint(gr2, idx, 0xFF8800);
			OLED_Printf("/s/p$60<$6F>");
			OLED_Printf("/s$62/rt%03d$66/yu%03d$6A/yd%03d", (int)gr1->xData[idx], (int)gr1->yData[idx], (int)gr2->yData[idx]);
			pTime = HAL_GetTick();
			state = 0;
		}*/
		if(HAL_GetTick() - pTime > 10){
			pTime += 10;
			Switch_LED_Temperature((gr1->yData[idx] + gr2->yData[idx])/2.0);
		}

	}


}


Menu_t HeatList[] = {
		{NULL, "/s/1/R       U     D  ", COLOR_SKY},
		{NULL, "/s/2/wTEMP", COLOR_SKY},
		{NULL, "/s/3/wTARG", COLOR_SKY},
		{NULL, "/s/4/wTIME", COLOR_SKY},
		{NULL, "/s/5/wMOTR", COLOR_SKY},
		{NULL, "/s/6/wShow Graph     >", COLOR_SKY}
};

extern uint32_t OLED_bgColor;
void Heat(graph_t * gr1, graph_t * gr2){//Graph에 따라 분 단위로 시간 경과에 따라 온도를 설정합니다.
	graph_t * grn1 = Graph_InitNull(gr1->xAxisPos, gr1->yAxisPos, gr1->xDensity, gr1->yDensity);
	graph_t * grn2 = Graph_InitNull(gr2->xAxisPos, gr2->yAxisPos, gr2->xDensity, gr2->yDensity);
	OLED_Clear();
	OLED_MenuUI("< HEAT:/bON         ", 0xFF0000, 0x000000, HeatList, 6, 0xFFFF00);
	OLED_Cursor(4, 0xFF0000);
	heaterTop -> target = gr1->yData[0];
	heaterBottom -> target = gr2->yData[0];
	OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
	OLED_Printf("/s$3B/y%3.1f  \r\n", heaterTop->target);
	OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
	OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
	int idx = 0;
	int curs = 4;
	int heaterOn = 1;
	float interval = (gr1->xData[idx + 1] - gr1->xData[idx]) * 60000.00;
	float target1U = gr1->yData[idx];
	float target2U = gr1->yData[idx + 1];
	float target1D = gr2->yData[idx];
	float target2D = gr2->yData[idx + 1];
	float tempU = tempTop->read(tempTop);
	float tempD = tempBottom->read(tempBottom);
	heaterTop->start(heaterTop);
	heaterBottom->start(heaterBottom);
	HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 0);
	HAL_Delay(500);
	heaterTop -> target = target1U;
	heaterBottom -> target = target1D;
	HAL_GPIO_WritePin(Motor1_GPIO_Port, Motor1_Pin, GPIO_PIN_SET);	// Convection 팬 끄기
	HAL_GPIO_WritePin(DCFAN_GPIO_Port, DCFAN_Pin, GPIO_PIN_SET);	// 냉각팬 켜기
	uint32_t heatTime = HAL_GetTick();
	uint32_t pTime = HAL_GetTick();
	uint32_t gTime = HAL_GetTick();
	uint32_t graphmode = 0;
	grn1->Add(grn1, gr1->xData[idx], tempU);
	grn2->Add(grn2, gr2->xData[idx], tempD);
	for(;;){
		if(HAL_GetTick() - heatTime > timer * 60000 && heaterOn){
			HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, 1);
			heaterTop->stop(heaterTop);
			heaterBottom->stop(heaterBottom);
			heaterOn = 0;
		}
		uint16_t sw = Switch_Read();
		if(sw==SW_LEFT && graphmode == 0) break;
		else if(sw == SW_LEFT && graphmode){
			graphmode = 0;
			OLED_Clear();
			OLED_MenuUI("< HEAT:        ", 0xFF0000, 0x000000, HeatList, 6, 0xFFFF00);
			OLED_bgColor = 0xFF0000;
			OLED_Printf("$07/b%s", heaterOn?"ON ":"OFF");
			OLED_bgColor = 0x000000;
			OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
			OLED_Printf("/s$3B/y%3.1f  \r\n", heaterTop->target);
			OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
			OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
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
					OLED_MenuUI("< HEAT:         ", 0xFF0000, 0x000000, HeatList, 6, 0xFFFF00);
					OLED_bgColor = 0xFF0000;
					OLED_Printf("$07/b%s", heaterOn?"ON ":"OFF");
					OLED_bgColor = 0x000000;

					OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterTop->target);
					OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
					OLED_Printf("/s$5B/yR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
					OLED_Cursor(curs, 0xFF0000);
				}
			}
			else if(curs == 4){
				HAL_GPIO_WritePin(Motor2_GPIO_Port, Motor2_Pin, (Motor2_GPIO_Port->ODR) & Motor2_Pin?0:1);
				OLED_Printf("/s$55/yC:%s  \r\n", (Motor1_GPIO_Port -> ODR) & Motor1_Pin ? "OFF" : "ON ");
				OLED_Printf("/s$5B/rR:%s  \r\n", (Motor2_GPIO_Port -> ODR) & Motor2_Pin ? "OFF" : "ON ");
			}
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && !graphmode){
			if(curs > 4){
				curs --;
				OLED_Cursor(curs, 0xFF0000);
			}
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && !graphmode){
			if(curs < 5){
				curs ++;
				OLED_Cursor(curs, 0xFF0000);
			}
		}

		tempU = tempTop->read(tempTop);
		tempD = tempBottom->read(tempBottom);
		if(HAL_GetTick() - heatTime > (uint32_t)(gr1->xData[idx + 1] * 60000.0) && idx < gr1->count - 1){
			idx++;
			grn1->Add(grn1, gr1->xData[idx], tempU);
			grn2->Add(grn2, gr2->xData[idx], tempD);
			if(idx != gr1->count - 1){
				interval = (gr1->xData[idx + 1] - gr1->xData[idx]) * 60000.00;
				target1U = gr1->yData[idx];
				target2U = gr1->yData[idx + 1];
				target1D = gr2->yData[idx];
				target2D = gr2->yData[idx + 1];
			}
		}
		if(HAL_GetTick() - pTime > 100){
			pTime += 100;
			Switch_LED_Temperature((tempU + tempD) / 2.0);
			//온도 프로필에서 설정한 값의 2배 속도로 움직이게 하여 안정적으로 작동시킵니다.
			if(heaterTop->target < target2U){
				heaterTop->target += 200.0 * ((target2U - target1U) > 0? (target2U - target1U) : (target1U - target2U)) / interval;
				if(heaterTop -> target > target2U){
					heaterTop->target = target2U;
				}
			}
			else if(heaterTop->target > target2U){
				heaterTop->target -= 200.0 * ((target2U - target1U) > 0? (target2U - target1U) : (target1U - target2U)) / interval;
				if(heaterTop -> target < target2U){
					heaterTop->target = target2U;
				}
			}
			if(heaterBottom->target < target2D){
				heaterBottom->target += 200.0 * ((target2D - target1D) > 0? (target2D - target1D) : (target1D - target2D)) / interval;
				if(heaterBottom -> target > target2D){
					heaterBottom->target = target2D;
				}
			}
			else if(heaterBottom->target > target2D){
				heaterBottom->target -= 200.0 * ((target2D - target1D) > 0? (target2D - target1D) : (target1D - target2D)) / interval;
				if(heaterBottom -> target < target2D){
					heaterBottom->target = target2D;
				}
			}
			if(!graphmode){
				uint32_t tck = HAL_GetTick() - heatTime;
				OLED_Printf("/s$25/y%3.1f  \r\n", tempU);
				OLED_Printf("/s$2B/y%3.1f  \r\n", tempD);
				OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
				OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
//				OLED_Printf("/s$45/p%3.1f  \r\n", heaterTop->duty);
//				OLED_Printf("/s$4B/p%3.1f  \r\n", heaterBottom->duty);
//				OLED_Printf("/s$55/p%s\r\n", heaterStateStr2[heaterTop->state]);
//				OLED_Printf("/s$5B/p%s\r\n", heaterStateStr2[heaterBottom->state]);
//				OLED_Printf("/s$65/p%3.1f  \r\n", heaterTop->errorSum);
//				OLED_Printf("/s$6B/p%3.1f  \r\n", heaterBottom->errorSum);
				OLED_Printf("/s$45/g%01d:%02d:%02d.%01d", (tck / 3600000) % 24,(tck / 60000) % 60,(tck / 1000) % 100, (tck / 100) % 10);
			}
		}
		else if(graphmode){
			if(HAL_GetTick() - gTime > 500){
				gTime += 500;
				OLED_Clear();
				if(graphmode == 1){
					gr1 ->Print(gr1, 0x0000FF);
					grn1 ->Print(grn1, 0xFF0000);
					Graph_PrintPoint(gr1, (float) (HAL_GetTick() - heatTime) / 60000.0f, heaterTop->target, 0x4444FF);
					Graph_PrintPoint(grn1, (float) (HAL_GetTick() - heatTime) / 60000.0f, tempU, 0xFF4444);
				}
				else if(graphmode == 2){
					gr2 ->Print(gr2, 0x00FF00);
					grn2 ->Print(grn2, 0xFF0000);
					Graph_PrintPoint(gr2, (float) (HAL_GetTick() - heatTime) / 60000.0f, heaterBottom->target, 0x44FF44);
					Graph_PrintPoint(grn2, (float) (HAL_GetTick() - heatTime) / 60000.0f, tempD, 0xFF4444);
				}
			}

			OLED_Line(0, 53, 95, 53, 0xFFFF00);
//			OLED_Printf("/s$60/g%d:$64/y%d/$68/r%d[\'c]", (HAL_GetTick() - heatTime) / 60000, (int)temp, (int)heaterTop->target);
		}
	}
	heaterTop->stop(heaterTop);
	heaterBottom->stop(heaterBottom);
}
