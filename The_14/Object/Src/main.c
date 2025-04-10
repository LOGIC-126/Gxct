#include "main.h"
#include "RCC/bsp_rcc.h"
#include "KED/bsp_ked.h"
#include "LCD/bsp_lcd.h"
#include "ADC/bsp_adc.h"
#include "TIM/bsp_tim.h"
#include "app.h"

#define LED_PEROID 100
#define LCD_PEROID 500
#define KEY_PEROID 50

//变量声明
uint8_t Led_dis = 0x33;
__IO uint32_t Led_uwTick = 0;
__IO uint32_t Key_uwTick = 0;
__IO uint32_t Lcd_uwTick = 0;
__IO uint32_t Mode_uwTick = 0;
__IO uint32_t App_uwTick = 0;
__IO uint32_t Set_Tick = 0;
uint8_t Key_Val;
uint8_t Key_Down;
uint8_t Key_Up;
uint8_t Old_Key_Val;


u8 Val_M,Val_P,Val_R,Val_K,Val_N,Val_Duty;
u8 Fq = 250;
u8 lock_Flag = 1;
float Val_V,Val_MH,Val_ML;
enum
{
	DATA = 0X01,PARA,RECD
};
enum
{
	HIGH = 0X01,LOW
};
u8 Lcd_stas = DATA;
u8 Out_Mode = 0;
uint8_t Lcd_str[21];

//PWM IC相关变量
uint16_t PWM_Up_Cnt;
uint16_t PWM_Down_Cnt;
float Duty;
float Va;


//子函数
void Led_Proc(void);

void Key_Proc(void);
void Key1_Proc(void);
void Key2_Proc(void);
void Key3_Proc(void);
void Key4_Proc(void);

void Lcd_Proc(void);
void App_Fun(void);






int main(void)
{

	HAL_Init();
	SystemClock_Config();
	
	KED_Init();
    LCD_Init();
	HAL_Delay(100);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
	LCD_Clear(Black);
	HAL_Delay(100);
	ADC1_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
	
	Val_R = 1;
	Val_K = 1;
	Val_N = 0;
	Val_M = 'L';

	while (1)
	{
		Key_Proc();
		Led_Proc();
		Lcd_Proc();
		App_Fun();
	}

}

void Key_Proc(void)
{
	if((uwTick - Key_uwTick) < KEY_PEROID) return;
	Key_uwTick = uwTick;
	
	Key_Val = Key_Scan();
	Key_Down = Key_Val & (Key_Val ^ Old_Key_Val);
	Key_Up = ~Key_Val & (Key_Val ^ Old_Key_Val);
	Old_Key_Val = Key_Val;

	if (Key_Down == 1)
		Key1_Proc();
	else if(Key_Down == 2)
		Key2_Proc();
	else if(Key_Down == 3)
		Key3_Proc();
	else if(Key_Down == 4)
		Key4_Proc();
		
		if(Key_Up == 4)
		{
			if(uwTick - Set_Tick >= 2000)
			{
			//在此编写长按程序
				lock_Flag = 0;
				
			}
			else
			{
			//在此编写短按程序
				lock_Flag = 1;
			}
		}
}

void Led_Proc(void)
{
	if((uwTick - Led_uwTick) < LED_PEROID) return;
	Led_uwTick = uwTick;
	
	Led_dis = ~Led_dis;
	Led_Disp(Led_dis);

}

void Lcd_Proc(void)
{
	if((uwTick - Lcd_uwTick) < LCD_PEROID) return;
	Lcd_uwTick = uwTick;
	switch(Lcd_stas)
	{
		case DATA:
			sprintf((char *)Lcd_str,"        DATA");
			LCD_DisplayStringLine(Line2 ,Lcd_str);
			sprintf((char *)Lcd_str,"     M = %c",Val_M);
			LCD_DisplayStringLine(Line4 ,Lcd_str);
			sprintf((char *)Lcd_str,"     P = %d%%",Val_P);
			LCD_DisplayStringLine(Line5 ,Lcd_str);
			sprintf((char *)Lcd_str,"     V = %.1f",Val_V);
			LCD_DisplayStringLine(Line6 ,Lcd_str);
			break;
		case PARA:
			sprintf((char *)Lcd_str,"        PARA");
			LCD_DisplayStringLine(Line2 ,Lcd_str);
			sprintf((char *)Lcd_str,"     R = %d",Val_R);
			LCD_DisplayStringLine(Line4 ,Lcd_str);
			sprintf((char *)Lcd_str,"     K = %d",Val_K);
			LCD_DisplayStringLine(Line5 ,Lcd_str);
			break;
		case RECD:
			sprintf((char *)Lcd_str,"        RECD");
			LCD_DisplayStringLine(Line2 ,Lcd_str);
			sprintf((char *)Lcd_str,"     N = %d",Val_N);
			LCD_DisplayStringLine(Line4 ,Lcd_str);
			sprintf((char *)Lcd_str,"     MH = %.1f",Val_MH);
			LCD_DisplayStringLine(Line5 ,Lcd_str);
			sprintf((char *)Lcd_str,"     ML = %.1f",Val_ML);
			LCD_DisplayStringLine(Line6 ,Lcd_str);
			break;
		
	}
//	sprintf((char *)Lcd_str,"Va:%.3f",Va);
//	LCD_DisplayStringLine(Line8 ,Lcd_str);
//	sprintf((char *)Lcd_str,"Duty:%d",Val_Duty);
//	LCD_DisplayStringLine(Line9 ,Lcd_str);
}

void Key1_Proc(void)
{
	Lcd_stas++;
	if(Lcd_stas > 3)
		Lcd_stas = 1;
	LCD_Clear(Black);
}

void Key2_Proc(void)
{
	switch(Lcd_stas)
	{
		
		case DATA:
			if((uwTick - Mode_uwTick) > 5000)
			{
				Out_Mode = !Out_Mode;	//切换模式
				Mode_uwTick = uwTick;
			}
			break;
		case PARA:


			break;
		case RECD:

			break;
	
	}
}

void Key3_Proc(void)
{

}

void Key4_Proc(void)
{
	Set_Tick = uwTick;
}

void App_Fun(void)
{
	if((uwTick - App_uwTick) < 160) return;
	App_uwTick = uwTick;
	
	
	Va = (getADC()/4096.0)*3.3;
	
	if((Va < 3) && (Va > 1))
	{
		Val_Duty = (Va * 37.5) - 27.5;
	}
	else if(Va > 3)
		Val_Duty = 85;
	else if(Va < 1)
		Val_Duty = 10;
	
	if(lock_Flag)
		Val_P = Val_Duty;
	if(!Out_Mode)//低频
	{
		Val_M = 'L';
		if (Fq < 250)
			Fq += 5;
		else if (Fq > 250)
			Fq -= 5;
//		Fq = 250;
		Pwm_Out(Fq - 1,Val_P);
	}
	else
	{
		Val_M = 'H';
		if (Fq < 125)
			Fq += 5;
		else if (Fq > 125)
			Fq -= 5;
//		Fq = 125;
		Pwm_Out(Fq - 1,Val_P);
	}
	



}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			PWM_Up_Cnt = HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2)+1;
			Duty = (float)PWM_Down_Cnt/PWM_Up_Cnt;
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			PWM_Down_Cnt = HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1)+1;
			
		}
	}
}




