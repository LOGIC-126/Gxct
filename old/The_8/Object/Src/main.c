#include "main.h"
#include "RCC/bsp_rcc.h"
#include "KED/bsp_ked.h"
#include "LCD/bsp_lcd.h"
#include "TIM/bsp_tim.h"
#include "App_lift.h"



/*子函数声明区*/
void Key_Proc(void);
void Key_Proc1(void);
void Key_Proc2(void);
void Key_Proc3(void);
void Key_Proc4(void);

void Lcd_Proc(void);
void Led_Proc(void);


void Lift_Proc(void);
/*变量声明区*/


//key
__IO uint32_t Key_uwTick_Set;
#define Key_Peroid 100
uint8_t Key_Val =0;
uint8_t Old_Key_Val =0;
uint8_t Key_Up =0;
uint8_t Key_Down =0;


//uint8_t Wait_Flag = 1;	//等待一秒标志位
//uint8_t Start_Flag = 0; //开始计时标志位

//Lift
__IO uint32_t Lift_uwTick_Set;
#define Lift_Peroid 1000
#define MAX_FLOORS 4
uint8_t Lift_Enable = 0x00;
uint8_t Floot_Sta = 0x01;
enum
{
      LT_STOP=0x01, LT_UP, LT_DOWN, LT_OPEN, LT_CLOS, LT_WAIT_KEY, LT_CON

}LT_Sta;
uint8_t ucState;

//lcd
__IO uint32_t Lcd_uwTick_Set;
#define Lcd_Peroid 200
uint8_t Lcd_str[21];


int main(void)
{
  
	HAL_Init();
	SystemClock_Config();
	Ked_Init();
	LCD_Init();

	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	PWM_PA6_Init();
	PWM_PA7_Init();

	ucState = LT_STOP;
	Floot_num = 1;
	sprintf((char *)Lcd_str ,"    Floot_Num:%d",Floot_num);
	LCD_DisplayStringLine(Line4,Lcd_str);
	while (1)
	{
		Key_Proc();
		Lcd_Proc();
		Lift_Proc();
	}

}

void Key_Proc(void)
{
	if((uwTick - Key_uwTick_Set) <= Key_Peroid)
		return;
	Key_uwTick_Set = uwTick;
	

	Key_Val = Key_Scan();
	Key_Down = Key_Val & (Key_Val ^ Old_Key_Val);
	Key_Up = ~Key_Val & (Key_Val ^ Old_Key_Val);
	Old_Key_Val = Key_Val;

	if (Key_Down == 1)
		Key_Proc1();
	else if(Key_Down == 2)
		Key_Proc2();
	else if(Key_Down == 3)
		Key_Proc3();
	else if(Key_Down == 4)
		Key_Proc4();

}

void Key_Proc1(void)
{
	Lift_Enable = Lift_Enable | 0x01;
} 
void Key_Proc2(void)
{
	Lift_Enable = Lift_Enable | 0x02;
} 
void Key_Proc3(void)
{
	Lift_Enable = Lift_Enable | 0x04;
} 
void Key_Proc4(void)
{
	Lift_Enable = Lift_Enable | 0x08;
} 


void Lcd_Proc(void)
{
	if((uwTick - Lcd_uwTick_Set) < Lcd_Peroid)
		return;
	Lcd_uwTick_Set = uwTick;
	
	Led_Disp(Lift_Enable);
}



void Lift_Proc(void)
{
	switch(ucState)
	{
		case LT_STOP:
			if(Key_Up)
			{
				Lift_uwTick_Set = uwTick;
				ucState = LT_WAIT_KEY;
			}
			break;
		case LT_WAIT_KEY:
			if((uwTick - Lift_uwTick_Set) >= Lift_Peroid)
			{
				if(Lift_Enable){
					if(Lift_Enable & Floot_Sta)
					{
						Lift_Enable = Lift_Enable &(~(Lift_Enable & Floot_Sta));
						break;
					}
					if(Lift_Enable > Floot_Sta)
						ucState = LT_UP;
					else if(Lift_Enable < Floot_Sta)
						ucState = LT_DOWN;
				}
				else ucState = LT_STOP;
			}
			else
			{
				if(Key_Down) ucState = LT_STOP;
			}
			break;
		case LT_UP:
			Lift_Up();
			Floot_Sta = Floot_Sta << 1;
			ucState = LT_CON;
			break;
		case LT_DOWN:
			Lift_Down();
			Floot_Sta = Floot_Sta >> 1;
			ucState = LT_CON;
			break;
		case LT_CON:
			if(Lift_Enable & Floot_Sta) //0110 & 0010  = 0010
			{
				Lift_Enable = Lift_Enable &(~(Lift_Enable & Floot_Sta));
				Lift_Open();
				Lift_Clos();
			}
			ucState = LT_WAIT_KEY;
	}
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
