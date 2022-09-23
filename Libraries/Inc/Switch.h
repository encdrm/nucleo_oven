#ifndef __SWITCH__H__
#define __SWITCH__H__


#define SW_1 0x0001
#define SW_2 0x0002
#define SW_3 0x0004
#define SW_4 0x0008
#define SW_5 0x0010
#define SW_12 0x0003
#define SW_13 0x0005
#define SW_14 0x0009
#define SW_15 0x0011
#define SW_23 0x0006
#define SW_24 0x000A
#define SW_25 0x0012
#define SW_34 0x000C
#define SW_35 0x0014
#define SW_45 0x0018
#define SW_1L 0x0021
#define SW_2L 0x0022
#define SW_3L 0x0024
#define SW_4L 0x0028
#define SW_5L 0x0030
#define SW_12L 0x0023
#define SW_13L 0x0025
#define SW_14L 0x0029
#define SW_15L 0x0031
#define SW_23L 0x0026
#define SW_24L 0x002A
#define SW_25L 0x0032
#define SW_34L 0x002C
#define SW_35L 0x0034
#define SW_45L 0x0038

#define SW_LEFT SW_5
#define SW_RIGHT SW_1
#define SW_TOP SW_2
#define SW_BOTTOM SW_4
#define SW_ENTER SW_3
#define SW_LEFT_LONG SW_5L
#define SW_RIGHT_LONG SW_1L
#define SW_TOP_LONG SW_2L
#define SW_BOTTOM_LONG SW_4L
#define SW_ENTER_LONG SW_3L

#define COLOR_RED 0xA000
#define COLOR_PINK 0xA035
#define COLOR_WHITE 0xA0A0
#define COLOR_SKY 0x35A0
#define COLOR_BLUE 0x00A0
#define COLOR_BLACK 0x0000

#define SwitchLED(col) Switch_LED((((col)>>8)&0xFF)<<2, ((col)&0xFF)<<2)


void Switch_LED(uint16_t red, uint16_t blue);
uint16_t Switch_Read();

#endif
