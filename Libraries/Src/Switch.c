#include "main.h"
#include "Switch.h"



#define SWITCH_STATE_IDLE 0
#define SWITCH_STATE_PRE_DOWN 1
#define SWITCH_STATE_DOWN 2
#define SWITCH_STATE_PRE_IDLE 3
#define SWITCH_STATE_LONG 4
#define SWITCH_STATE_PRE_IDLE_LONG 5
#define LONG_FLAG 0x20

#define LINE_TRACER_SWITCH
extern TIM_HandleTypeDef htim4;

void Switch_LED(uint16_t red, uint16_t blue){
	htim4.Instance -> CCR1 = red;
	htim4.Instance -> CCR2 = blue;
}

void Switch_LED_Temperature(float temp){
	//250도일 때 빨강
	//50도일 때 흰색
	//0도일 때 파랑
	static float temp_v;
	if(temp_v < temp){
		temp_v += 1.0f;
		if(temp_v > temp){
			temp_v = temp;
		}
	}
	else if(temp_v > temp){
		temp_v -= 1.0f;
		if(temp_v < temp){
			temp_v = temp;
		}
	}


	if(temp_v > 250.00f){
		Switch_LED(1023, 0);
	}
	else if(temp_v > 50.00f){
		Switch_LED(1023, (250.00f - temp_v) * (250.00f - temp_v) * 1023.00f / 40000.0f);
	}
	else if(temp_v > 0){
		Switch_LED(temp_v * temp_v * 1023.00f / 2500.0f, 1023);
	}
	else{
		Switch_LED(0, 1023);
	}
}




uint16_t Switch_Read(){
	static uint8_t Switch_state = 0;
	static uint16_t Switch_sum = 0;
	static uint16_t Switch_input = 0;
	static uint32_t Switch_time = 0;
	static uint32_t Switch_lowTime = 0;
#ifdef LINE_TRACER_SWITCH
	uint8_t sw1 = !HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
	uint8_t sw2 = !HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
	uint8_t sw3 = !HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);
	uint8_t sw4 = !HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);
	uint8_t sw5 = !HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin);
	Switch_input = sw1 | (sw2 << 1) | (sw3 << 2) | (sw4 << 3) | (sw5 << 4);
#endif
	switch(Switch_state){
	case SWITCH_STATE_IDLE:
		if(Switch_input){
			Switch_time = HAL_GetTick();
			Switch_state = SWITCH_STATE_PRE_DOWN;
			Switch_sum |= Switch_input;
		}
		break;
	case SWITCH_STATE_PRE_DOWN:
		if(Switch_input){
			if(HAL_GetTick() - Switch_time > 80){
				Switch_state = SWITCH_STATE_DOWN;
				return Switch_sum;
			}
			Switch_sum |= Switch_input;
		}
		else{
			Switch_state = SWITCH_STATE_IDLE;
			Switch_sum = 0;
		}
		break;
	case SWITCH_STATE_DOWN:
		if(!Switch_input){
			Switch_lowTime = HAL_GetTick();
			Switch_state = SWITCH_STATE_PRE_IDLE;
		}
		else if(HAL_GetTick() - Switch_time > 400){
			Switch_time = HAL_GetTick();
			Switch_state = SWITCH_STATE_LONG;
			return Switch_sum | LONG_FLAG;
		}
		break;
	case SWITCH_STATE_PRE_IDLE:
		if(!Switch_input){
			if(HAL_GetTick() - Switch_lowTime > 80){
				Switch_time = HAL_GetTick();
				Switch_state = SWITCH_STATE_IDLE;
				Switch_sum = 0;
			}
		}
		else{
			Switch_state = SWITCH_STATE_DOWN;
		}
		break;
	case SWITCH_STATE_LONG:
		if(!Switch_input){
			Switch_lowTime = HAL_GetTick();
			Switch_state = SWITCH_STATE_PRE_IDLE_LONG;
		}
		else if(HAL_GetTick() - Switch_time > 100){
			Switch_time = HAL_GetTick();
			return Switch_sum | LONG_FLAG;
		}
		break;
	case SWITCH_STATE_PRE_IDLE_LONG:
		if(!Switch_input){
			if(HAL_GetTick() - Switch_lowTime > 80){
				Switch_time = HAL_GetTick();
				Switch_state = SWITCH_STATE_IDLE;
				Switch_sum = 0;
			}
		}
		else{
			Switch_state = SWITCH_STATE_LONG;
		}
		break;
	}
	return 0;
}
