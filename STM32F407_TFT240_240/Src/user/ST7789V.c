#include "ST7789V.h"
#include "tim.h"
#include "spi.h"
/********stm32f407vet6**********/
#define ST7789V_RESET(val)  (val?HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET)) 
#define ST7789V_CS(val)  		(val?HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET))
#define ST7789V_D_C(val)  	(val?HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET))

#define ST7789V_DataH       HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET)
#define ST7789V_DataL       HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET)

#define ST7789V_ClkH       	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET)
#define ST7789V_ClkL       	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET)

void DelayUs(uint16_t us)
{
	if(us > 60000)
	{
		us = 59999;
	}
	TIM14->CNT = 0;
	HAL_TIM_Base_Start(&htim14);
	while(TIM14->CNT < us);
	HAL_TIM_Base_Stop(&htim14);
}

void ST7789V_SpiWrite(uint8_t  data)
{
	uint8_t i;
//	ST7789V_CS(0);
	ST7789V_ClkL;
	DelayUs(1);
	for(i = 0; i < 8 ; i++)
	{
		if((data & 0x80 ) == 0x80)
		{
			ST7789V_DataH;
		}
		else
		{
			ST7789V_DataL;
		}
		DelayUs(1);
		ST7789V_ClkH;
		DelayUs(1);
		ST7789V_ClkL;
		data <<= 1;
	}
//	ST7789V_CS(1);
}

uint8_t ST7789V_SpiRead(uint8_t Command)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	uint8_t i ,data = 0;
	GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	ST7789V_CS(0);
	ST7789V_D_C(0);
	ST7789V_ClkL;
	DelayUs(1);
	/********************/
	for(i = 0; i < 8 ; i++)
	{
		if((Command & 0x80 )== 0x80)
		{
			ST7789V_DataH;
		}
		else
		{
			ST7789V_DataL;
		}
		DelayUs(1);
		ST7789V_ClkH;
		DelayUs(1);
		ST7789V_ClkL;
		Command <<= 1;
	}
	/***********************/
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	DelayUs(1);
	ST7789V_ClkH;
	DelayUs(1);
	ST7789V_ClkL;
	
	ST7789V_D_C(1);
	for(i = 0; i < 8 ; i++)
	{
		data <<= 1;
		DelayUs(1);
		ST7789V_ClkH;
		data |= HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12);
		DelayUs(1);
		ST7789V_ClkL;
	}
	
	ST7789V_CS(1);
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	return data;
}

void  ST7789V_WriteCommand(uint8_t CommondData)
{
	ST7789V_D_C(0);
	ST7789V_CS(0);
//	ST7789V_SpiWrite(CommondData);
	HAL_SPI_Transmit(&hspi3, (uint8_t *)&CommondData, 1, 5);
	ST7789V_CS(1);
}

void  ST7789V_WriteData(uint8_t Data)
{
	ST7789V_D_C(1);
	ST7789V_CS(0);
//	ST7789V_SpiWrite(Data);
	
	HAL_SPI_Transmit(&hspi3, (uint8_t *)&Data, 1, 5);
	ST7789V_CS(1);
}

void  ST7789V_WriteData16(uint16_t Data)
{
	uint8_t a,b;
	a = Data >> 8;
	b = Data;
	ST7789V_D_C(1);
	
	ST7789V_CS(0);

//	ST7789V_SpiWrite(a);
//	ST7789V_SpiWrite(b);

	HAL_SPI_Transmit(&hspi3, (uint8_t *)&a, 1, 5);
	HAL_SPI_Transmit(&hspi3, (uint8_t *)&b, 1, 5);
	ST7789V_CS(1);
}

void ST7789V_EnterSleep (void)
{
	ST7789V_WriteCommand(0x28);	// Display Off
	HAL_Delay(20);
	ST7789V_WriteCommand(0x10);	// Sleep In (Low power mode)
}

void ST7789V_ExitSleep (void)
{
	ST7789V_WriteCommand(0x11); // Exit Sleep Mode
	
	HAL_Delay(120);
	
	ST7789V_WriteCommand(0x29); // Display on
	ST7789V_WriteCommand(0x2c); // Memory write
	// When this command is accepted, the column register and the page
	// register are reset to the start column/start page positions.
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
  ST7789V_WriteCommand(0x2a); 
  ST7789V_WriteData((Xpos&0xff00)>>8); 
  ST7789V_WriteData((Xpos&0XFF));	   
  ST7789V_WriteCommand(0x2b); 
  ST7789V_WriteData((Ypos&0xff00)>>8); 
  ST7789V_WriteData((Ypos&0XFF));
} 	  

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
	ST7789V_WriteCommand(0x2c);  
}	 

void ST7789V_Init(void)
{
	ST7789V_RESET(0);
	HAL_Delay(50);
	ST7789V_RESET(1);
	HAL_Delay(120);

//	ST7789V_WriteCommand( 0x01);
//	HAL_Delay(120);
	ST7789V_WriteCommand( 0x11);
	HAL_Delay(120);
	ST7789V_WriteCommand(0x36);
	ST7789V_WriteData(0x00);
	ST7789V_WriteCommand(0x3a);
	ST7789V_WriteData(0x05);
	
	ST7789V_WriteCommand(0xB2);     
	ST7789V_WriteData(0x0C);   
	ST7789V_WriteData(0x0C);   
	ST7789V_WriteData(0x00);   
	ST7789V_WriteData(0x33);   
	ST7789V_WriteData(0x33);
	
	ST7789V_WriteCommand(0xB7);     
	ST7789V_WriteData( 0x35);   

	ST7789V_WriteCommand(0xBB);   //VCOM  
	ST7789V_WriteData( 0x1C);   

	ST7789V_WriteCommand(0xC0);     
	ST7789V_WriteData(0x2C);   

	ST7789V_WriteCommand(0xC2);     
	ST7789V_WriteData(0x01);   

	ST7789V_WriteCommand(0xC3);   //GVDD  
	ST7789V_WriteData( 0x0B); //   

	ST7789V_WriteCommand(0xC4);     
	ST7789V_WriteData(0x20);   

	ST7789V_WriteCommand(0xC6);     
	ST7789V_WriteData(0x0F);   

	ST7789V_WriteCommand(0xD0);     
	ST7789V_WriteData( 0xA4);   
	ST7789V_WriteData( 0xA1);   

	ST7789V_WriteCommand(0xE0);     
	ST7789V_WriteData(0xD0);   
	ST7789V_WriteData(0x06);   
	ST7789V_WriteData(0x0B);   
	ST7789V_WriteData(0x0A);   
	ST7789V_WriteData(0x09);   
	ST7789V_WriteData(0x06);   
	ST7789V_WriteData(0x2F);   
	ST7789V_WriteData(0x44);   
	ST7789V_WriteData(0x45);   
	ST7789V_WriteData(0x18);   
	ST7789V_WriteData(0x14);   
	ST7789V_WriteData(0x14);   
	ST7789V_WriteData(0x27);   
	ST7789V_WriteData(0x2D);   

	ST7789V_WriteCommand(0xE1);     
	ST7789V_WriteData(0xD0);   
	ST7789V_WriteData(0x06);   
	ST7789V_WriteData(0x0B);   
	ST7789V_WriteData(0x0A);   
	ST7789V_WriteData(0x09);   
	ST7789V_WriteData(0x05);   
	ST7789V_WriteData(0x2E);   
	ST7789V_WriteData(0x43);   
	ST7789V_WriteData(0x45);   
	ST7789V_WriteData(0x18);   
	ST7789V_WriteData(0x14);   
	ST7789V_WriteData(0x14);   
	ST7789V_WriteData(0x27);   
	ST7789V_WriteData(0x2D);   

	ST7789V_WriteCommand(0x29);   

	HAL_Delay(120); //ms

	ST7789V_WriteCommand(0x21);  

	ST7789V_WriteCommand(0x2a); //Column Address Set
	ST7789V_WriteData(0x00);
	ST7789V_WriteData(0x00); //0
	ST7789V_WriteData(0x00);
	ST7789V_WriteData(0xf0); //240
	ST7789V_WriteCommand(0x2b); //Row Address Set
	ST7789V_WriteData(0x00);
	ST7789V_WriteData(0x00); //0
	ST7789V_WriteData(0x00);
	ST7789V_WriteData(0xf0); //240
	
	HAL_Delay(20);
	ST7789V_ClearScreen(BLUE);
	
}

void ST7789V_ClearScreen(uint16_t color)
{
	unsigned int i;
	LCD_SetCursor(0x00,0x0000);	//设置光标位置
	LCD_WriteRAM_Prepare();
	for(i=0;i < 240 * 240;i++)
	{
		ST7789V_WriteData16(color);
	}
}
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	ST7789V_WriteData16(color); 
} 

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)
		incx=1; //设置单步方向 
	else if(delta_x==0)
		incx=0;//垂直线 
	else 
	{
		incx=-1;
		delta_x=-delta_x;
	} 
	if(delta_y>0)
		incy=1; 
	else if(delta_y==0)
		incy=0;//水平线 
	else
	{
		incy=-1;
		delta_y=-delta_y;
	} 
	if( delta_x>delta_y)
		distance=delta_x; //选取基本增量坐标轴 
	else 
		distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
