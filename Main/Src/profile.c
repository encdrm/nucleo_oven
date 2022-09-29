#include "profile.h"
#include "setting.h"

extern UART_HandleTypeDef huart1;
extern tempsensor_t *thermoTop;
extern tempsensor_t *thermoBottom;
extern setting_t *ovenSetting;


graph_t * profile_upper;
graph_t * profile_lower;
uint32_t timer = 90;
uint32_t time_interval = 10;//작동 중 오류를 방지하기 위해 임시적으로 10을 interval로 설정하되, 바꿀 수 있게 합니다.

//온도 프로필 권장사항
//tData값은 반드시 오름차순이어야 하며, 보낼 때 이를 고려하여 순서에 맞게 보내야 함.
//tData값은 항상 간격이 일정할 필요는 없지만, 자동 인터벌 설정을 지원받기 위해서는 간격 편차가 일정 값 이하여야 함.
//자동 인터벌 값을 인터벌 값을 평균내서 구하며, 타이머에 추가 시 이를 반영.
void Heat(graph_t * gr1, graph_t * gr2);

Menu_t profileList[] = {
		{NULL, "/yGetProfile/r$1F>", COLOR_PINK},
		{NULL, "/yTimer: /w90min/r$2F>", COLOR_PINK},
		{NULL, "/yIntvl: /w10min/r$3F>", COLOR_PINK},
		{NULL, "/yProfileSet/r$4F>", COLOR_PINK},
		{NULL, "/oHeat/r$5F>", COLOR_PINK},
};
void profile(){
	graph_t * g1 = profile_upper;
	graph_t * g2 = profile_lower;
	SwitchLED(COLOR_SKY);
	OLED_MenuUI("< Profile", 0xFF0000, 0x000000, profileList, 5, 0xFFFF00);
	OLED_Cursor(0, 0xFF6600);
	int idx = 0;
	uint32_t sw = 0;
	uint32_t timerSetting = 0;
	OLED_Printf("$27/w%2dmin", timer);
	OLED_Printf("$37/w%2dmin", time_interval);

	// for BT configuration
	HAL_StatusTypeDef halError;
	enum {
		BTPARSE_IDLE, BTPARSE_READ, BTPARSE_WRITEGRAPH
	};
	uint8_t buffer[3000] = {0};	// UART 데이터
	int bufferIdx = 0;
	uint8_t Data;
	char bufferX[20] = { 0 };
	char bufferY[20] = { 0 };
	int btBufXIdx = 0;
	int btBufYIdx = 0;
	int btParseState = BTPARSE_IDLE;
	int btTransCount = 0;
	bool flag_start = false;
	bool flag_bottom = false;
	bool flag_finished = false;

	for(;;){
		sw = Switch_Read();
		if(sw == SW_LEFT && !timerSetting) break;
		else if(sw == SW_LEFT && timerSetting){
			timerSetting = !timerSetting;
			OLED_Printf("$27%s%2dmin", timerSetting?"/r":"/w", timer);
			OLED_Printf("$37%s%2dmin", timerSetting?"/r":"/w", time_interval);
		}
		else if(sw == SW_TOP && !timerSetting){
			idx += 4;
			idx %= 5;
			OLED_Cursor(idx, 0xFF6600);
		}
		else if(sw == SW_BOTTOM && !timerSetting){
			idx += 1;
			idx %= 5;
			OLED_Cursor(idx, 0xFF6600);
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && timerSetting){
			if(idx == 1){
				timer += time_interval;
				//tData[timer/time_interval] = (float)timer;
				//uData[timer/time_interval] = 30.0f;
				//dData[timer/time_interval] = 30.0f;
				//Graph_Delete(g1);
				//Graph_Delete(g2);
				//g1 = _Graph_Init(tData, uData, 1 + (timer / time_interval), 0, 52, (float)timer / 90.0f, 6.0f);
				//g2 = _Graph_Init(tData, dData, 1 + (timer / time_interval), 0, 52, (float)timer / 90.0f, 6.0f);
				g1->Add(g1, (float)timer, g1->yData[g1->count - 1]);
				g1->ChangeDensity(g1, (float)timer / 90.0f, 6.0f);
				g2->Add(g2, (float)timer, g2->yData[g2->count - 1]);
				g2->ChangeDensity(g2, (float)timer / 90.0f, 6.0f);
				OLED_Printf("$27/r%2dmin", timer);
			}
			else if(idx == 2){//불규칙 간격 타이머도 만들 수 있음. 따라서 그래프 객체는 여기서 건드리지 않습니다.
				time_interval += 1;
				OLED_Printf("$37/r%2dmin", time_interval);
			}
		}
		else if((sw == SW_BOTTOM || sw == SW_BOTTOM_LONG) && timerSetting){
			if(timer > 0 && idx == 1){
				timer = g1->xData[g1->count - 2];//타이머 간격이 일정하지 않을 수 있음을 감안하여 xData값을 바탕으로 타이머 설정.
//				Graph_Delete(g1);
//				Graph_Delete(g2);
//				g1 = _Graph_Init(tData, uData, 1 + (timer / time_interval), 0, 52, (float)timer / 90.0f, 6.0f);
//				g2 = _Graph_Init(tData, dData, 1 + (timer / time_interval), 0, 52, (float)timer / 90.0f, 6.0f);
				g1->Pop(g1, NULL, NULL);
				g2->Pop(g2, NULL, NULL);
				OLED_Printf("$27/r%2dmin", timer);
			}
			else if(time_interval > 1 && idx == 2){
				time_interval -= 1;
				OLED_Printf("$37/r%2dmin", time_interval);
			}
		}
		else if(sw == SW_RIGHT){
			if(idx != 1 && idx != 2){
				OLED_Clear();
			}
			switch(idx){
			case 0:
				Graph_Delete(g1);
				Graph_Delete(g2);
				g1 = Graph_InitNull(0, 52,
									(float) timer / 90.0f, 6.0f);
				g2 = Graph_InitNull(0, 52,
									(float) timer / 90.0f, 6.0f);
				//초기값이 누락되어 있을 수 있어서 추가.
				g1->Add(g1, 0.0f, 30.0f);
				g2->Add(g2, 0.0f, 30.0f);
				timer = 0;//그래프 설정 시 타이머 초기화하여 t값의 최댓값으로 반영.
				bufferIdx = 0;
				memset(bufferX, 0, 20);
				memset(bufferY, 0, 20);
				btBufXIdx = 0;
				btBufYIdx = 0;
				btTransCount = 0;	// '/' 구분자 감지 횟수
				flag_start = false;
				flag_bottom = false;
				flag_finished = false;

				OLED_Printf("/s/r$20Getting Profile from BT...");
				// Read bluetooth

				while (true) {
					halError = HAL_UART_Receive(&huart1, (uint8_t*) buffer, 1, 200);
					if (halError == HAL_OK)
						break;
					if (Switch_Read())
						break;
				}
				if (halError == HAL_OK) { // Received
					while (halError == HAL_OK)
						halError = HAL_UART_Receive(&huart1, (uint8_t*) (buffer+(++bufferIdx)), 1, 200);
					for (int btIdx = 0;btIdx<bufferIdx;btIdx++) {
						Data = *(buffer+btIdx);
						switch (Data) {
						case 'T':	//
							flag_start = true;
							flag_bottom = false;
							btTransCount = 0;
							btParseState = BTPARSE_READ;
							continue;
						case 'B':
							flag_start = true;
							flag_bottom = true;
							btTransCount = 0;
							btParseState = BTPARSE_READ;
							continue;
						case '/':
							if (flag_start) {
								btTransCount++;
								if (btTransCount%2 == 0)
									btParseState = BTPARSE_WRITEGRAPH;
								else
									continue;
							}
							break;
						case 'E':
							flag_finished = true;
							break;
						}

						if (flag_finished)
							break;

						switch (btParseState) {
						case BTPARSE_IDLE:
							break;
						case BTPARSE_READ:
							if (btTransCount%2 == 0)
								bufferX[btBufXIdx++] = Data;
							else
								bufferY[btBufYIdx++] = Data;
							break;
						case BTPARSE_WRITEGRAPH:
							{
								float xdata = atof(bufferX);
								float ydata = atof(bufferY);
								if(xdata < 0.05f){//초기값 설정으로 판별
									if(flag_bottom){
										g2->Pop(g2, NULL, NULL);
									} else {
										g1->Pop(g1, NULL, NULL);
									}
								}
								timer = timer > (int)xdata ? timer : (int)xdata;//타이머 값은 자동 설정.
								g1->ChangeDensity(g1, (float)timer / 90.0f, 6.0);
								g2->ChangeDensity(g2, (float)timer / 90.0f, 6.0);
								if (flag_bottom){
									g2->Add(g2, xdata, ydata);
								} else {
									g1->Add(g1, xdata, ydata);
								}
								memset(bufferX, 0, sizeof(bufferX));
								memset(bufferY, 0, sizeof(bufferY));
								btBufXIdx = 0;
								btBufYIdx = 0;
								btParseState = BTPARSE_READ;
							}
							break;
						}
					}
				}
				OLED_Clear();
				if (flag_finished == true) {
					if(g1->count < g2->count){
						for(uint16_t i = g1->count; i < g2->count; i++){
							g1->Add(g1, g2->xData[i], 20.0f);
						}
					}
					else if(g2->count < g1->count){
						for(uint16_t i = g2->count; i < g1->count; i++){
							g2->Add(g2, g1->xData[i], 20.0f);
						}
					}
					//자동 인터벌 설정 : g1, g2의 길이를 맞췄으므로 g1의 데이터만 갖고 사용한다.
					if(g1->count > 1)
						time_interval = (int)(g1->xData[g1->count-1] / (g1->count - 1));
					OLED_Printf("/s/b$60Finished!");
					flag_finished = false;
					g1->Print(g1, 0x0000FF);
					g2->Print(g2, 0x00FF00);
					while (!Switch_Read());
				} else {
					OLED_Printf("$20/s/bExiting...");
				}
				break;
			case 1:
				timerSetting = !timerSetting;
				OLED_Printf("$27%s%2dmin", timerSetting?"/r":"/w", timer);
				break;
			case 2:
				timerSetting = !timerSetting;
				OLED_Printf("$37%s%2dmin", timerSetting?"/r":"/w", time_interval);
				break;
			case 3:
				Profile_Set(g1, g2);
				break;
			case 4:
				Heat(g1, g2);
				break;
			}
			if(idx != 1 && idx != 2){
				OLED_Clear();
				SwitchLED(COLOR_SKY);
				OLED_MenuUI("< Profile", 0xFF0000, 0x000000, profileList, 5, 0xFFFF00);
				OLED_Printf("$27/w%2dmin", timer);
				OLED_Printf("$37/w%2dmin", time_interval);
				OLED_Cursor(idx, 0xFF6600);
			}
		}
	}
//	g1, g2가 전역변수에서 정의되므로 Graph_Delete 삭제
//	Graph_Delete(g1);
//	Graph_Delete(g2);
	profile_upper = g1;
	profile_lower = g2;
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
		if(HAL_GetTick() - pTime > 100){
			pTime += 100;
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

extern TIM_HandleTypeDef htim2;
extern uint32_t OLED_bgColor;
void Heat(graph_t * gr1, graph_t * gr2){//Graph에 따라 분 단위로 시간 경과에 따라 온도를 설정합니다.
	htim2.Instance->CCR1 = 256;
	HAL_Delay(300);
	htim2.Instance->CCR1 = 0;
	graph_t * grn1 = Graph_InitNull(gr1->xAxisPos, gr1->yAxisPos, gr1->xDensity, gr1->yDensity);
	graph_t * grn2 = Graph_InitNull(gr2->xAxisPos, gr2->yAxisPos, gr2->xDensity, gr2->yDensity);
	OLED_Clear();
	OLED_MenuUI("< HEAT:/bON         ", 0xFF0000, 0x000000, HeatList, 6, 0xFFFF00);
	OLED_Cursor(4, 0xFF0000);
	heaterTop -> target = gr1->yData[0];
	heaterBottom -> target = gr2->yData[0];
	OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
	OLED_Printf("/s$3B/y%3.1f  \r\n", heaterTop->target);
	OLED_Printf("/s$55/yC:%s  \r\n", ovenSetting->feature_state(ovenSetting, "convection") ? "ON " : "OFF");
	OLED_Printf("/s$5B/yR:%s  \r\n", ovenSetting->feature_state(ovenSetting, "rotisserie") ? "ON " : "OFF");
	int idx = 0;
	int curs = 4;
	int heaterOn = 1;
	float interval = (gr1->xData[idx + 1] - gr1->xData[idx]) * 60000.00;
	float target1U = gr1->yData[idx];
	float target2U = gr1->yData[idx + 1];
	float target1D = gr2->yData[idx];
	float target2D = gr2->yData[idx + 1];
	float tempTop = thermoTop->read(thermoTop);
	float tempBottom = thermoBottom->read(thermoBottom);
	heaterTop->start(heaterTop);
	heaterBottom->start(heaterBottom);
	ovenSetting->feature_on(ovenSetting, "lamp");
	HAL_Delay(500);
	heaterTop -> target = target1U;
	heaterBottom -> target = target1D;
	ovenSetting->feature_off(ovenSetting, "convection");	// Convection 팬 끄기
	ovenSetting->feature_on(ovenSetting, "fan");	// 냉각팬 켜기
	uint32_t heatTime = HAL_GetTick();
	uint32_t pTime = HAL_GetTick();
	uint32_t gTime = HAL_GetTick();
	uint32_t pauseTime = HAL_GetTick();
	uint32_t graphmode = 0;
	uint8_t pause = 0;
	grn1->Add(grn1, gr1->xData[idx], tempTop);
	grn2->Add(grn2, gr2->xData[idx], tempBottom);
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
			ovenSetting->feature_off(ovenSetting, "lamp");
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
			OLED_MenuUI("< HEAT:        ", 0xFF0000, 0x000000, HeatList, 6, 0xFFFF00);
			OLED_bgColor = 0xFF0000;
			OLED_Printf("$07/b%s", heaterOn?"ON ":"OFF");
			OLED_bgColor = 0x000000;
			OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
			OLED_Printf("/s$3B/y%3.1f  \r\n", heaterTop->target);
			OLED_Printf("/s$55/yC:%s  \r\n", ovenSetting->feature_state(ovenSetting, "convection") ? "ON " : "OFF");
			OLED_Printf("/s$5B/yR:%s  \r\n", ovenSetting->feature_state(ovenSetting, "rotisserie") ? "ON " : "OFF");
			OLED_Cursor(curs, 0xFF0000);
		}
		else if(sw == SW_ENTER && !graphmode && curs == 4){//콘벡숀 모오터 돌리기!
			ovenSetting->feature_toggle(ovenSetting, "convection");
			OLED_Printf("/s$55/rC:%s  \r\n", ovenSetting->feature_state(ovenSetting, "convection") ? "ON " : "OFF");
			OLED_Printf("/s$5B/yR:%s  \r\n", ovenSetting->feature_state(ovenSetting, "rotisserie") ? "ON " : "OFF");
		}
		else if(sw == SW_RIGHT && !pause){//그래프 띄우기
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
					OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
					OLED_Printf("/s$55/yC:%s  \r\n", ovenSetting->feature_state(ovenSetting, "convection") ? "ON " : "OFF");
					OLED_Printf("/s$5B/yR:%s  \r\n", ovenSetting->feature_state(ovenSetting, "rotisserie") ? "ON " : "OFF");
					OLED_Cursor(curs, 0xFF0000);
				}
			}
			else if(curs == 4){
				ovenSetting->feature_toggle(ovenSetting, "rotisserie");
				OLED_Printf("/s$55/yC:%s  \r\n", ovenSetting->feature_state(ovenSetting, "convection") ? "ON " : "OFF");
				OLED_Printf("/s$5B/rR:%s  \r\n", ovenSetting->feature_state(ovenSetting, "rotisserie") ? "ON " : "OFF");
			}
		}
		else if(sw == SW_TOP && pause){
			pause = !pause;
			OLED_bgColor = 0xFF0000;
			OLED_Printf("$0B/k ");
			OLED_bgColor = 0x000000;
			heatTime += HAL_GetTick() - pauseTime;
			heaterTop->start(heaterTop);
			heaterBottom->start(heaterBottom);
		}
		else if((sw == SW_TOP || sw == SW_TOP_LONG) && !graphmode && !pause){
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
			else if(curs == 5){
				pause = !pause;
				if(pause){
					OLED_bgColor = 0xFF0000;
					OLED_Printf("$0B/kP");
					OLED_bgColor = 0x000000;
					pauseTime = HAL_GetTick();
					heaterTop->stop(heaterTop);
					heaterBottom->stop(heaterBottom);
				}
				else{
					OLED_bgColor = 0xFF0000;
					OLED_Printf("$0B/k ");
					OLED_bgColor = 0x000000;
					heatTime += HAL_GetTick() - pauseTime;
					heaterTop->start(heaterTop);
					heaterBottom->start(heaterBottom);
				}
			}
		}

		tempTop = thermoTop->read(thermoTop);
		tempBottom = thermoBottom->read(thermoBottom);
		if(HAL_GetTick() - heatTime > (uint32_t)(gr1->xData[idx + 1] * 60000.0) && idx < gr1->count - 1){
			idx++;
			grn1->Add(grn1, gr1->xData[idx], tempTop);
			grn2->Add(grn2, gr2->xData[idx], tempBottom);
			if(idx != gr1->count - 1){
				interval = (gr1->xData[idx + 1] - gr1->xData[idx]) * 60000.00;
				target1U = gr1->yData[idx];
				target2U = gr1->yData[idx + 1];
				target1D = gr2->yData[idx];
				target2D = gr2->yData[idx + 1];
			}
		}
		if(HAL_GetTick() - pTime > 200){
			pTime += 200;
			Switch_LED_Temperature((tempTop + tempBottom) / 2.0);
			//온도 프로필에서 설정한 값의 2배 속도로 움직이게 하여 안정적으로 작동시킵니다.
			if(!pause){
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
			}
			if(!graphmode){
				uint32_t tck = HAL_GetTick() - heatTime;
				OLED_Printf("/s$25/y%3.1f  \r\n", tempTop);
				OLED_Printf("/s$2B/y%3.1f  \r\n", tempBottom);
				OLED_Printf("/s$35/y%3.1f  \r\n", heaterTop->target);
				OLED_Printf("/s$3B/y%3.1f  \r\n", heaterBottom->target);
//				OLED_Printf("/s$45/p%3.1f  \r\n", heaterTop->duty);
//				OLED_Printf("/s$4B/p%3.1f  \r\n", heaterBottom->duty);
//				OLED_Printf("/s$55/p%s\r\n", heaterStateStr2[heaterTop->state]);
//				OLED_Printf("/s$5B/p%s\r\n", heaterStateStr2[heaterBottom->state]);
//				OLED_Printf("/s$65/p%3.1f  \r\n", heaterTop->errorSum);
//				OLED_Printf("/s$6B/p%3.1f  \r\n", heaterBottom->errorSum);
				if(!pause)
					OLED_Printf("/s$45/g%01d:%02d:%02d.%01d", (tck / 3600000) % 24,(tck / 60000) % 60,(tck / 1000) % 60, (tck / 100) % 10);
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
					Graph_PrintPoint(grn1, (float) (HAL_GetTick() - heatTime) / 60000.0f, tempTop, 0xFF4444);
				}
				else if(graphmode == 2){
					gr2 ->Print(gr2, 0x00FF00);
					grn2 ->Print(grn2, 0xFF0000);
					Graph_PrintPoint(gr2, (float) (HAL_GetTick() - heatTime) / 60000.0f, heaterBottom->target, 0x44FF44);
					Graph_PrintPoint(grn2, (float) (HAL_GetTick() - heatTime) / 60000.0f, tempBottom, 0xFF4444);
				}
			}

			OLED_Line(0, 53, 95, 53, 0xFFFF00);
//			OLED_Printf("/s$60/g%d:$64/y%d/$68/r%d[\'c]", (HAL_GetTick() - heatTime) / 60000, (int)temp, (int)heaterTop->target);
		}
	}
	heaterTop->stop(heaterTop);
	heaterBottom->stop(heaterBottom);
}
