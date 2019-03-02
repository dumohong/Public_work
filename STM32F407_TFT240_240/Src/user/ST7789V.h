#ifndef  __ST7789V_H
#define  __ST7789V_H
#include "stm32f4xx.h"

//»­±ÊÑÕÉ«
#define WHITE         	0xFFFF
#define BLACK         	0x0000	  
#define BLUE         	 	0x001F  
#define BRED            0XF81F
#define GRED 			 			0XFFE0
#define GBLUE			 			0X07FF
#define RED           	0xF800
#define MAGENTA       	0xF81F
#define GREEN         	0x07E0
#define CYAN          	0x7FFF
#define YELLOW        	0xFFE0
#define BROWN 			 		0XBC40 //×ØÉ«
#define BRRED 			 		0XFC07 //×ØºìÉ«
#define GRAY  			 		0X8430 //»ÒÉ«

uint8_t ST7789V_SpiRead(uint8_t Command);
void  DelayUs(uint16_t us);
void  ST7789V_SpiWrite(uint8_t  data);
void  ST7789V_WriteCommand(uint8_t CommondData);
void  ST7789V_WriteData(uint8_t Data);
void  ST7789V_EnterSleep (void);
void  ST7789V_ExitSleep (void);
void  ST7789V_SetDisplayWindow(int x0, int y0, int x1, int y1);	
void  ST7789V_Init(void);
void  ST7789V_ClearScreen(uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
#endif
