#include "main.h"
#include "bsp_rcc.h"
#include "bsp_ked.h"
#include "bsp_lcd.h"
#include "bsp_tim.h"
//宏定义


//子函数声明
void Key_Proc(void);
void Key_Proc1(void);
void Key_Proc2(void);
void Key_Proc3(void);
void Key_Proc4(void);
void Led_Proc(void);
void Lcd_Proc(void);

/*变量声明*/
__IO uint32_t Key_uwTick;
__IO uint32_t Led_uwTick;
__IO uint32_t Lcd_uwTick;

uint8_t Key_Val,Key_Old,Key_Up,Key_Down;

int main(void)
{

	HAL_Init();
	SystemClock_Config();
	
	KED_Init();
	LCD_Init();
	TIM2_Init();
	TIM3_Init();
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);

//    LCD_DisplayStringLine(Line4, (unsigned char *)"    Hello,world.   ");
//    HAL_Delay(1000);
	while (1)
	{
		Key_Proc();
		Led_Proc();
		Lcd_Proc();
	}

}

void Key_Proc(void)
{
	if((uwTick - Key_uwTick) <= 50)	return;
	Key_uwTick = uwTick;

	Key_Val = Key_Scan();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;
	
	if(Key_Down == 1) Key_Proc1();
	if(Key_Down == 2) Key_Proc2();
	if(Key_Down == 3) Key_Proc3();
	if(Key_Down == 4) Key_Proc4();
}

void Led_Proc(void)
{
	if((uwTick - Led_uwTick) <= 200)	return;
	Led_uwTick = uwTick;
	
	
	
}

void Lcd_Proc(void)
{
	if((uwTick - Lcd_uwTick) <= 300)	return;
	Lcd_uwTick = uwTick;
	
	
	
}


void Key_Proc1(void)
{
	
}
void Key_Proc2(void)
{

}
void Key_Proc3(void)
{

}
void Key_Proc4(void)
{

}





