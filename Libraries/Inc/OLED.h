#ifndef __OLED__H__
#define __OLED__H__

#define OLED_SIZEX 96
#define OLED_SIZEY 64
#include "menu.h"

void OLED_Clear();
void OLED_Begin();
void OLED_AddrWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void OLED_Rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int32_t color);
void OLED_Dot(uint8_t x,uint8_t y,uint8_t size, uint32_t color);
void OLED_WriteChar(int16_t ch);
void OLED_Printf(const char * format, ...);
void OLED_Line(uint8_t xi, uint8_t yi, uint8_t xe, uint8_t ye, int32_t color);
void OLED_MenuUI(char * title, uint32_t titleBgColor, uint32_t titleTextColor, Menu_t * menu, uint32_t count, uint32_t textColor);
void OLED_Cursor(uint8_t n, uint32_t color);


#endif
