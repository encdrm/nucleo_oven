#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "main.h"
#include "OLED.h"
#include "menu.h"




extern SPI_HandleTypeDef hspi2;
uint8_t OLED_row = 1;
uint8_t OLED_col = 1;
uint32_t OLED_charColor = 0xFFFFFF;
uint32_t OLED_bgColor = 0x000000;
uint8_t OLED_charPoint = 2;

//8*5 크기의 Alphabet 배열(총 96*5=480 byte)을 사용하여 LCD의 Alphabet을 구현합니다.
static unsigned char characters[495] = {
		0x00, 0x00, 0x00, 0x00, 0x00, //0x20 sp
		0x00, 0x00, 0xFA, 0x00, 0x00, //0x21 !
		0x00, 0xC0, 0x00, 0xC0, 0x00, //0x22 "
		0x28, 0x7C, 0x28, 0x7C, 0x28, //0x23 #
		0x64, 0x92, 0xFE, 0x92, 0x4C, //0x24 $
		0x44, 0x08, 0x10, 0x20, 0x44, //0x25 %
		0x0C, 0xF2, 0xAA, 0xCA, 0x04, //0x26 &
		0x00, 0x00, 0xC0, 0x00, 0x00, //0x27 '
		0x00, 0x00, 0x7C, 0x82, 0x00, //0x28 (
		0x00, 0x82, 0x7C, 0x00, 0x00, //0x29 )
		0x54, 0x38, 0x7C, 0x38, 0x54, //0x2A *
		0x10, 0x10, 0x7C, 0x10, 0x10, //0x2B +
		0x00, 0x01, 0x02, 0x00, 0x00, //0x2C ,
		0x10, 0x10, 0x10, 0x10, 0x10, //0x2D -
		0x00, 0x00, 0x02, 0x00, 0x00, //0x2E .
		0x04, 0x08, 0x10, 0x20, 0x40, //0x2F /
		0x7C, 0x82, 0x82, 0x82, 0x7C, //0x30 0
		0x00, 0x42, 0xFE, 0x02, 0x00, //0x31 1
		0x42, 0x86, 0x8A, 0x92, 0x62, //0x32 2
		0x44, 0x82, 0x92, 0x92, 0x6C, //0x33 3
		0x18, 0x28, 0x48, 0xFE, 0x08, //0x34 4
		0xE4, 0xA2, 0xA2, 0xA2, 0x9C, //0x35 5
		0x7C, 0x92, 0x92, 0x92, 0x4C, //0x36 6
		0x80, 0x80, 0x8E, 0x90, 0xE0, //0x37 7
		0x6C, 0x92, 0x92, 0x92, 0x6C, //0x38 8
		0x64, 0x92, 0x92, 0x92, 0x7C, //0x39 9
		0x00, 0x00, 0x44, 0x00, 0x00, //0x3A :
		0x00, 0x00, 0x46, 0x00, 0x00, //0x3B ;
		0x00, 0x00, 0x10, 0x28, 0x44, //0x3C <
		0x28, 0x28, 0x28, 0x28, 0x28, //0x3D =
		0x44, 0x28, 0x10, 0x00, 0x00, //0x3E >
		0x40, 0x80, 0x8A, 0x90, 0x60, //0x3F ?
		0x7C, 0x82, 0xBA, 0xAA, 0x7A, //0x40 @
		0x7E, 0x90, 0x90, 0x90, 0x7E, //0x41 A
		0xFE, 0x92, 0x92, 0x92, 0x6C, //0x42 B
		0x7C, 0x82, 0x82, 0x82, 0x44, //0x43 C
		0xFE, 0x82, 0x82, 0x44, 0x38, //0x44 D
		0xFE, 0x92, 0x92, 0x92, 0x82, //0x45 E
		0xFE, 0x90, 0x90, 0x90, 0x80, //0x46 F
		0x7C, 0x82, 0x82, 0x92, 0x5C, //0x47 G
		0xFE, 0x10, 0x10, 0x10, 0xFE, //0x48 H
		0x00, 0x82, 0xFE, 0x82, 0x00, //0x49 I
		0x00, 0x82, 0xFC, 0x80, 0x00, //0x4A J
		0xFE, 0x10, 0x28, 0x44, 0x82, //0x4B K
		0xFE, 0x02, 0x02, 0x02, 0x02, //0x4C L
		0xFE, 0x40, 0x20, 0x40, 0xFE, //0x4D M
		0xFE, 0x20, 0x10, 0x08, 0xFE, //0x4E N
		0x7C, 0x82, 0x82, 0x82, 0x7C, //0x4F 0
		0xFE, 0x90, 0x90, 0x90, 0x60, //0x50 P
		0x78, 0x84, 0x84, 0x86, 0x7C, //0x51 Q
		0xFE, 0x90, 0x98, 0x94, 0x62, //0x52 R
		0x64, 0x92, 0x92, 0x92, 0x4C, //0x53 S
		0x80, 0x80, 0xFE, 0x80, 0x80, //0x54 T
		0xFC, 0x02, 0x02, 0x02, 0xFC, //0x55 U
		0xF0, 0x0C, 0x02, 0x0C, 0xF0, //0x56 V
		0xFE, 0x04, 0x08, 0x04, 0xFE, //0x57 W
		0xC6, 0x28, 0x10, 0x28, 0xC6, //0x58 X
		0xC0, 0x20, 0x1E, 0x20, 0xC0, //0x59 Y
		0x86, 0x8A, 0x92, 0xA2, 0xC2, //0x5A Z
		0x00, 0x00, 0xFE, 0x82, 0x00, //0x5B [
		0x40, 0x20, 0x10, 0x08, 0x04, //0x5C Backslash
		0x00, 0x82, 0xFE, 0x00, 0x00, //0x5D ]
		0x20, 0x40, 0x80, 0x40, 0x20, //0x5E ^
		0x01, 0x01, 0x01, 0x01, 0x01, //0x5F _
		0x00, 0x00, 0xC0, 0x40, 0x00, //0x60 `
		0x04, 0x2A, 0x2A, 0x2A, 0x1E, //0x61 a
		0xFE, 0x12, 0x12, 0x12, 0x0C, //0x62 b
		0x1C, 0x22, 0x22, 0x22, 0x14, //0x63 c
		0x0C, 0x12, 0x12, 0x12, 0xFE, //0x64 d
		0x1C, 0x2A, 0x2A, 0x2A, 0x18, //0x65 e
		0x00, 0x10, 0x7E, 0x90, 0x00, //0x66 f
		0x08, 0x15, 0x15, 0x15, 0x1E, //0x67 g
		0xFE, 0x10, 0x10, 0x10, 0x0E, //0x68 h
		0x00, 0x22, 0xBE, 0x02, 0x00, //0x69 i
		0x00, 0x21, 0xBE, 0x00, 0x00, //0x6A j
		0xFE, 0x08, 0x14, 0x02, 0x00, //0x6B k
		0x00, 0x82, 0xFE, 0x02, 0x00, //0x6C l
		0x1E, 0x10, 0x0C, 0x10, 0x0E, //0x6D m
		0x1E, 0x10, 0x10, 0x10, 0x0E, //0x6E n
		0x1C, 0x22, 0x22, 0x22, 0x1C, //0x6F o
		0x1F, 0x14, 0x14, 0x14, 0x08, //0x70 p
		0x08, 0x14, 0x14, 0x14, 0x1F, //0x71 q
		0x3E, 0x08, 0x10, 0x20, 0x20, //0x72 r
		0x12, 0x2A, 0x2A, 0x2A, 0x24, //0x73 s
		0x00, 0x10, 0x7E, 0x12, 0x00, //0x74 t
		0x1C, 0x02, 0x02, 0x02, 0x1C, //0x75 u
		0x18, 0x04, 0x02, 0x04, 0x18, //0x76 v
		0x1C, 0x02, 0x1C, 0x02, 0x1C, //0x77 w
		0x22, 0x14, 0x08, 0x14, 0x22, //0x78 x
		0x11, 0x09, 0x06, 0x04, 0x18, //0x79 y
		0x22, 0x26, 0x2A, 0x32, 0x22, //0x7A z
		0x00, 0x10, 0x6C, 0x82, 0x00, //0x7B {
		0x00, 0x00, 0xFE, 0x00, 0x00, //0x7C |
		0x00, 0x82, 0x6C, 0x10, 0x00, //0x7D }
		0x18, 0x20, 0x10, 0x08, 0x30, //0x7E ~
		0x0E, 0x12, 0x22, 0x12, 0x0E, //0x7F

		0x7C, 0x7C, 0x7C, 0x7C, 0x7C, //Full Square(/Q)
		0x7C, 0x44, 0x44, 0x44, 0x7C, //Empty Square(/q)
		0x38, 0x7C, 0x7C, 0x7C, 0x38, //Full Circle(/*)
};

static void inline OLED_CS_Set(uint8_t state){
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, state);
}

static void inline OLED_DC_Set(uint8_t state){
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, state);
}

static void inline OLED_RS_Set(uint8_t state){
	HAL_GPIO_WritePin(OLED_RS_GPIO_Port, OLED_RS_Pin, state);
}

static void inline OLED_Send(uint8_t data){
	__disable_irq();//인터럽트를 중단하여 데이터 전송을 원활하게 한다.
	OLED_CS_Set(0);//OLED에 데이터를 전송하기 위해 CS를 Low신호로 만들어야 한다.
	HAL_SPI_Transmit(&hspi2, &data, 1, 1);
	OLED_CS_Set(1);//데이터 전송이 끝나면 CS를 High로 만들어 신호가 들어와도 무시하도록 해야 한다.
	__enable_irq();
}

static void inline OLED_WriteCommand(uint8_t data){
	OLED_DC_Set(0);
	OLED_Send(data);
}

static void inline OLED_WriteData(uint8_t data){
	OLED_DC_Set(1);
	OLED_Send(data);
}

void OLED_Clear(){
	OLED_row = 1;
	OLED_col = 1;
	OLED_charColor = 0xFFFFFF;
	OLED_charPoint = 2;
	OLED_AddrWindow(0, 0, 96, 64);
	for(int i=0; i<12288; i++){
		OLED_WriteData(0);
	}
}

void OLED_Begin(){
	//15ms 이상 대기
	HAL_Delay(100);

	//초기 작업(리셋)
	OLED_CS_Set(1);
	OLED_RS_Set(0);
	HAL_Delay(3);
	OLED_RS_Set(1);

	//기본 설정 명령어
	OLED_WriteCommand(0xAE);
	OLED_WriteCommand(0xA0);
	OLED_WriteCommand(0x72);
	OLED_WriteCommand(0xA1);
	OLED_WriteCommand(0x00);
	OLED_WriteCommand(0xA2);
	OLED_WriteCommand(0x00);
	OLED_WriteCommand(0xA4);
	OLED_WriteCommand(0xA8);
	OLED_WriteCommand(0x3F);
	OLED_WriteCommand(0xAD);
	OLED_WriteCommand(0x8E);
	OLED_WriteCommand(0xB0);
	OLED_WriteCommand(0x0B);
	OLED_WriteCommand(0xB1);
	OLED_WriteCommand(0x31);
	OLED_WriteCommand(0xB3);
	OLED_WriteCommand(0xF0);
	OLED_WriteCommand(0x8A);
	OLED_WriteCommand(0x64);
	OLED_WriteCommand(0x8B);
	OLED_WriteCommand(0x78);
	OLED_WriteCommand(0x8C);
	OLED_WriteCommand(0x64);
	OLED_WriteCommand(0xBB);
	OLED_WriteCommand(0x3A);
	OLED_WriteCommand(0xBE);
	OLED_WriteCommand(0x3E);
	OLED_WriteCommand(0x87);
	OLED_WriteCommand(0x06);
	OLED_WriteCommand(0x81);
	OLED_WriteCommand(0x91);
	OLED_WriteCommand(0x82);
	OLED_WriteCommand(0x50);
	OLED_WriteCommand(0x83);
	OLED_WriteCommand(0x7D);

	OLED_Clear();
	OLED_WriteCommand(0xAF);
	HAL_Delay(100);

}

void OLED_AddrWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h){
	uint8_t x1 = x;
	uint8_t y1 = y;
	if(x1 > 95){
		x1 = 95;
	}
	if(y1 > 63){
		y1 = 63;
	}

	uint8_t x2 = x + w - 1;
	uint8_t y2 = y + h - 1;
	if(x2>95)
		x2 = 95;
	if(y2>63)
		y2 = 63;

	if(x1>x2){
		uint8_t temp = x2;
		x2=x1;
		x1=temp;
	}
	if(y1>y2){
		uint8_t temp = y2;
		y2=y1;
		y1=temp;
	}

	OLED_WriteCommand(0x15);
	OLED_WriteCommand(x1);
	OLED_WriteCommand(x2);
	OLED_WriteCommand(0x75);
	OLED_WriteCommand(y1);
	OLED_WriteCommand(y2);
}



void OLED_Rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int32_t color){
	OLED_AddrWindow(x, y, w, h);
	uint32_t red = (color >> 19) & 0x1F;
	uint32_t gre = (color >> 10) & 0x3F;
	uint32_t blu = (color >> 3) & 0x1F;
	uint32_t data = (red << 11) | (gre << 5) | blu;
	uint16_t size = (uint32_t)w * (uint32_t)h;
	for(uint16_t i=0; i<size; i++){
		OLED_WriteData(data >> 8);
		OLED_WriteData(data & 0xFF);
	}
}


void OLED_Dot(uint8_t x,uint8_t y,uint8_t size, uint32_t color){
	OLED_Rectangle(x, y, size, size, color);
}


void OLED_WriteChar(int16_t ch){
	if(ch <= -1 && ch >= -3){
		ch = 131 + ch;
	}
	else if(ch < 32 || ch >= 128){
		ch = 63;
	}

	ch -= 32;
	for(uint8_t i=0; i<5; i++){
		uint8_t byte = characters[ch * 5 + i];
		for(uint8_t j=0; j<8; j++){
			OLED_Dot(OLED_col + i * OLED_charPoint, OLED_row + j * OLED_charPoint, OLED_charPoint, \
					((byte>>(7-j))&0x01) ? OLED_charColor : OLED_bgColor);
		}
	}
	OLED_col += 6 * OLED_charPoint;

}


static int str2hex(char * str){
	int result = 0;
	for(int i=0; str[i]; i++){
		if(str[i] >= '0' && str[i] <= '9'){
			result <<= 4;
			result |= str[i] - '0';
		}
		else if(str[i] >= 'A' && str[i] <= 'F'){
			result <<= 4;
			result |= str[i] - 'A' + 10;
		}
		else if(str[i] >= 'a' && str[i] <= 'f'){
			result <<= 4;
			result |= str[i] - 'a' + 10;
		}
	}
	return result;
}

#define OLED
void OLED_Printf(const char * format, ...){
	/*
	 * buffer를 많이 할당받거나 static으로 선언하면 좋겠지만, 보통 프로세스당 4GB의 메모리를 가지는 일반 컴퓨터와 다르게
	 * STM32F411의 경우 전체 메모리 공간이 오직 128kB의 저장용량만 가지므로 메모리 공간을 아껴 써야 한다.
	 */
	char buffer[256] = { 0 };

	// 가변 인자를 받아서 buffer에 쓴다.
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);


//#ifdef OLED
//	oled_row = 0;
//	oled_col = 0;
//#endif
	// 줄바꿈 기호인 /0, /1을 만나면 커서 위치를 설정해주는 부분.
	for (char *c = buffer; *c; c++) {
		if (*c == '/' && (*(c + 1) >= '0' && *(c + 1) <= '6')) {//작은글씨 최대 7줄 / 큰글씨 최대 3줄
			OLED_row = 1 + 9 * OLED_charPoint * (*(c + 1) - '0');
			if(OLED_row > 60 || (OLED_row >= 54 && OLED_charPoint == 2)){
				OLED_row = 18 * OLED_charPoint;
			}
			OLED_col = 1;
			c += 1;
			continue;
		}
		else if (*c == '$' && (*(c + 1) >= '0' && *(c + 1) <= '6') && ((*(c + 2) >= '0' && *(c + 2) <= '9')||(*(c + 2) >= 'A' && *(c + 2) <= 'F'))) {//작은글씨 최대 7줄 / 큰글씨 최대 3줄
			OLED_row = 1 + 9 * OLED_charPoint * (*(c + 1) - '0');
			OLED_col = 1 + 6 * OLED_charPoint * (*(c + 2) >= 'A' ? (*(c + 2) - 'A' + 10) :(*(c + 2) - '0'));
			if(OLED_row > 60 || (OLED_row >= 54 && OLED_charPoint == 2)){
				OLED_row = 18 * OLED_charPoint;
			}
			c += 2;
			continue;
		}
		else if (*c == '/' && *(c + 1) == '#'){
			char str[7] = {0,0,0,0,0,0,0};
			strncpy(str, c+2, 6);
			OLED_charColor = str2hex(str);
			c += 7;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'S'){
			//Size Big
			OLED_charPoint = 2;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 's'){
			//Size Big
			OLED_charPoint = 1;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'r'){//RGBCMY는 오직 2색 이상 OLED만 지원(예외로 노랑색 라인이 있는 파랑 OLED는 조금 다르게 취급)
			OLED_charColor = 0xFF0000;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'g'){
			OLED_charColor = 0x00FF00;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'b'){
			OLED_charColor = 0x0000FF;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'y'){
			OLED_charColor = 0xFFFF00;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'c'){
			OLED_charColor = 0x00FFFF;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'm'){
			OLED_charColor = 0xFF00FF;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'w'){//White는 단색 OLED에서는 켜짐을 의미
			OLED_charColor = 0xFFFFFF;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'k'){//key color는 단색 OLED에서도 지원
			OLED_charColor = 0xFFFFFF;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'R'){
			OLED_charColor = 0xFF7777;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'G'){
			OLED_charColor = 0x77FF77;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'B'){
			OLED_charColor = 0x7777FF;
			c += 1;
			continue;
		}

		//Additional Color : 비표준 색상입니다.
		else if (*c == '/' && *(c + 1) == 'o'){
			OLED_charColor = 0xFF7700;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'l'){
			OLED_charColor = 0x77FF00;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 't'){
			OLED_charColor = 0x00FF77;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'e'){
			OLED_charColor = 0x0077FF;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'p'){
			OLED_charColor = 0x7700FF;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'M'){
			OLED_charColor = 0xFF0077;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'A'){
			OLED_charColor = 0x888888;
			c += 1;
			continue;
		}
		else if (*c == '/' && *(c + 1) == 'a'){
			OLED_charColor = 0x444444;
			c += 1;
			continue;
		}

		else if (*c == '/' && *(c + 1) == '/'){
			c += 1;
		}
		else if (*c == '/' && *(c + 1) == '#'){
			*c = '#';
			c += 1;
		}

		// \n, \r 등은 깨져서 출력되므로 무시한다.
		if (*c == '\r') continue;
		if (*c == '\n') {
			OLED_row += 9 * OLED_charPoint;
			OLED_col = 1;
			continue;
		}
		if (*c == '\t') {
			if(OLED_col < 25 && OLED_charPoint == 1){
				OLED_col = 25;
			}
			else if(OLED_col < 49){
				OLED_col = 49;
			}
			else if(OLED_col < 73 && OLED_charPoint == 1){
				OLED_col = 75;
			}
			else{
				OLED_col = 1;
				OLED_row += 9 * OLED_charPoint;
			}
			continue;
		}
		if (*c == '/' && *(c+1) == '*'){
			c += 1;
			OLED_WriteChar(-1);
			continue;
		}
		if (*c == '/' && *(c+1) == 'q'){
			c += 1;
			OLED_WriteChar(-2);
			continue;
		}
		if (*c == '/' && *(c+1) == 'Q'){
			c += 1;
			OLED_WriteChar(-3);
			continue;
		}
		OLED_WriteChar(*c);
	}
}

void OLED_Line(uint8_t xi, uint8_t yi, uint8_t xe, uint8_t ye, int32_t color){
	uint8_t red = (color >> 16) & 0xF8;
	uint8_t gre = (color >> 8) & 0xFC;
	uint8_t blu = color & 0xF8;

	//선을 그리기 위해서는 0x21 신호가 필요합니다.
	OLED_WriteCommand(0x21);
	OLED_WriteCommand(xi);
	OLED_WriteCommand(yi);
	OLED_WriteCommand(xe);
	OLED_WriteCommand(ye);
	OLED_WriteCommand(red>>2);
	OLED_WriteCommand(gre>>2);
	OLED_WriteCommand(blu>>2);
}

void OLED_MenuUI(char * title, uint32_t titleBgColor, uint32_t titleTextColor, Menu_t * menu, uint32_t count, uint32_t textColor){
	OLED_Rectangle(0, 0, 96, 9, titleBgColor);
	OLED_row = 1;
	OLED_col = 1;
	OLED_charColor = titleTextColor;
	OLED_bgColor = titleBgColor;
	OLED_Printf("/s%s\n", title);
	OLED_charColor = textColor;
	OLED_bgColor = 0;
	for(uint8_t i = 0; i < count; i++){
		OLED_Printf("/s%s\n", menu[i].message);
	}
}

void OLED_Cursor(uint8_t n, uint32_t color){
	//Line Erase
	for(uint8_t i = 9; i < 64; i+=9){
		OLED_Line(0, i, 95, i, 0);
	}
	OLED_Line(0, 9, 0, 63, 0);
	OLED_Line(95, 9, 95, 63, 0);

	//Line Drawing
	OLED_Line(0, 9 + 9 * n, 95, 9 + 9 * n, color);
	OLED_Line(0, 18 + 9 * n, 95, 18 + 9 * n, color);
	OLED_Line(0, 9 + 9 * n, 0, 18 + 9 * n, color);
	OLED_Line(95, 9 + 9 * n, 95, 18 + 9 * n, color);
}
