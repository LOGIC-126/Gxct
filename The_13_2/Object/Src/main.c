#include "main.h"
#include "bsp_rcc.h"
#include "bsp_ked.h"
#include "bsp_i2c.h"
#include "bsp_lcd.h"
#include "bsp_tim.h"
#include "bsp_uart.h"

enum
{
	XN=0x00,YN,XM,YM
};

enum
{
	SHOP=0X00,PRICE,REP
};
u8 Lcd_View;

//变量
u8 Key_Val,Key_Old,Key_Up,Key_Down;
u8 ucLed = 0x00;
u8 Lcd_str[21];
u8 Uart_TX[50];
u8 Len;
u8 Uart_Rx[10];
u8 E24T_Str1[4];
u8 E24T_Str2[4];
u8 E24T_Str3[4];
u8 E24T_Str4[4];
u8 E24R_Str[4];
__IO uint32_t Key_uwTick;
__IO uint32_t Lcd_uwTick;
__IO uint32_t Led_uwTick;
__IO uint32_t S5_uwTick;
__IO uint32_t I2c_uwTick;
u8 ShopX,ShopY,RepX,RepY,Old_RepX,Old_RepY;
float PriX,PriY,Sum,Old_PriX,Old_PriY;

//子函数
void Key_Proc(void);
void Key_Proc1(void);
void Key_Proc2(void);
void Key_Proc3(void);
void Key_Proc4(void);
void Lcd_Proc(void);
void Led_Proc(void);
void I2c_Proc(void);

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	KED_GPIO_Init();
	UART1_Init();
	TIM2_Init();
	HAL_UART_Receive_IT(&huart1,Uart_Rx,1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	//lcd
	LCD_Init();
	HAL_Delay(100);
	LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
	LCD_Clear(Black);
	HAL_Delay(100);
//    LCD_DisplayStringLine(Line4, (unsigned char *)"    Hello,world.   ");
//    HAL_Delay(1000);
	I2CInit();
	//-------
//	RepX=RepY=10;
//	PriX=PriY=1.0;
	icc_24C02_Read(XN,E24R_Str,sizeof(ShopX));
	Old_RepX = RepX = E24R_Str[0];
	HAL_Delay(100);
	icc_24C02_Read(YN,E24R_Str,sizeof(ShopY));
	Old_RepY = RepY = E24R_Str[0];
	HAL_Delay(100);
	icc_24C02_Read(XM,E24R_Str,sizeof(E24R_Str));
	memcpy(&PriX,E24R_Str,sizeof(PriX));
	Old_PriX = PriX;
	HAL_Delay(100);
	icc_24C02_Read(YM,E24R_Str,sizeof(E24R_Str));
	memcpy(&PriY,E24R_Str,sizeof(PriY));
	Old_PriY = PriY;
	
	
	while (1)
	{
		Key_Proc();
		Lcd_Proc();
		Led_Proc();
		I2c_Proc();
		if(((uwTick - S5_uwTick) > 5000))
		{
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,25);
			ucLed = ucLed & 0xFE;
		}
		else
		{
			if(S5_uwTick != 0)
			{
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,150);
			ucLed = ucLed | 0x01;
			}
		}
	}
}

void Key_Proc(void)
{
	if((uwTick - Key_uwTick) <= 50)	return;
	Key_uwTick = uwTick;
	
	Key_Val = Key_Scan();
	Key_Down = Key_Val &(Key_Val ^ Key_Old);
	Key_Up = ~Key_Val &(Key_Val ^ Key_Old);
	Key_Old = Key_Val;

	if(Key_Down == 1)
		Key_Proc1();
	if(Key_Down == 2)
		Key_Proc2();
	if(Key_Down == 3)
		Key_Proc3();
	if(Key_Down == 4)
		Key_Proc4();	
	
	
}

void Lcd_Proc(void)
{
	if((uwTick - Lcd_uwTick) <= 300)	return;
	Lcd_uwTick = uwTick;
	switch(Lcd_View)
	{
		case SHOP:
			sprintf((char*)Lcd_str,"        SHOP");
			LCD_DisplayStringLine(Line2,Lcd_str);
			sprintf((char*)Lcd_str,"     X:%2d",ShopX);
			LCD_DisplayStringLine(Line4,Lcd_str);
			sprintf((char*)Lcd_str,"     Y:%2d",ShopY);
			LCD_DisplayStringLine(Line5,Lcd_str);
			break;
		case PRICE:
			sprintf((char*)Lcd_str,"        PRICE");
			LCD_DisplayStringLine(Line2,Lcd_str);
			sprintf((char*)Lcd_str,"     X:%.1f",PriX);
			LCD_DisplayStringLine(Line4,Lcd_str);
			sprintf((char*)Lcd_str,"     Y:%.1f",PriY);
			LCD_DisplayStringLine(Line5,Lcd_str);
			break;
		case REP:
			sprintf((char*)Lcd_str,"        REP");
			LCD_DisplayStringLine(Line2,Lcd_str);
			sprintf((char*)Lcd_str,"     X:%2d",RepX);
			LCD_DisplayStringLine(Line4,Lcd_str);
			sprintf((char*)Lcd_str,"     Y:%2d",RepY);
			LCD_DisplayStringLine(Line5,Lcd_str);
			break;
	}


}
void Led_Proc(void)
{
	if((uwTick - Led_uwTick) <= 100)	return;
	Led_uwTick = uwTick;
	
	if((RepX==0) && (RepY == 0))
	{
		ucLed = ucLed ^ 0x02;
	}
	
	Led_Disp(ucLed);

}

void Key_Proc1(void)
{
	Lcd_View++;
	if(Lcd_View >2)
		Lcd_View = 0;
	LCD_Clear(Black);
	
}

void Key_Proc2(void)
{
	switch(Lcd_View)
	{
		case SHOP:
			ShopX++;
			if(ShopX > RepX)
				ShopX = 0;
			break;
		case PRICE:
			PriX = PriX + 0.1;
			if(PriX >= 2.0)
				PriX = 0;
			break;
		case REP:
			RepX++;
			break;
	}

}

void Key_Proc3(void)
{
	switch(Lcd_View)
	{
		case SHOP:
			ShopY++;
			if(ShopY > RepY)
				ShopY = 0;
			break;
		case PRICE:
			PriY = PriY + 0.1;
			if(PriY >= 2.0)
				PriY = 0;
			break;
		case REP:
			RepY++;
			break;
	}

}

void Key_Proc4(void)
{
	switch(Lcd_View)
	{
		case SHOP://5s 30%
			RepX = RepX - ShopX;
			RepY = RepY - ShopY;
			ShopX = 0;
			ShopY = 0;
			S5_uwTick = uwTick;
			Sum = ((float)RepX*PriX + (float)RepY*PriY);
			Len = sprintf((char*)Uart_TX,"X:%d,Y:%d,Z:%.1f\n",RepX,RepY,Sum);
			HAL_UART_Transmit(&huart1,Uart_TX,Len,20);
			break;
		case PRICE:

			break;
		case REP:

			break;
	}
}

void I2c_Proc(void)
{
	if((uwTick - I2c_uwTick) <= 700)	return;
	I2c_uwTick = uwTick;
	if(Old_RepX != RepX)
	{
		icc_24C02_Write(XN,&RepX,sizeof(RepX));
		Old_RepX = RepX;
	}
	else if(Old_RepY != RepY)
	{
		icc_24C02_Write(YN,&RepY,sizeof(RepY));
		Old_RepY = RepY;
	}
	else if(Old_PriX != PriX)
	{
		memcpy(E24T_Str1,&PriX,sizeof(PriX));
		icc_24C02_Write(XM,E24T_Str1,sizeof(E24T_Str1));
		Old_PriX = PriX;
	}
	else if(Old_PriY != PriY)
	{
		memcpy(E24T_Str1,&PriY,sizeof(PriY));
		icc_24C02_Write(YM,E24T_Str1,sizeof(E24T_Str1));
		Old_PriY = PriY;
	}




}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(Uart_Rx[0] == '?')
	{
		sprintf((char*)Uart_TX,"X:%.1f,Y:%.1f \n",PriX,PriY);
		HAL_UART_Transmit(&huart1,Uart_TX,14,10);
	}
	
	HAL_UART_Receive_IT(&huart1,Uart_Rx,1);
}



