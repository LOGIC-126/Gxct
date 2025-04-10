/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "RCC/bsp_rcc.h"
#include "LED_KEY/bsp_led_key.h"
#include "UART/bsp_uart.h"
#include "LCD/bsp_lcd.h"
#include "IIC/bsp_i2c.h"
#include "ADC/bsp_adc.h"
#include "Basic_tim/bsp_tim6.h"
#include "PWM_IN/bsp_pwm_in.h"

// 子函数声明
void Key_Proc(void);
void Key1_Proc(void);
void Key2_Proc(void);
void Key3_Proc(void);
void Key4_Proc(void);

void Lcd_Proc(void);

/*变量声明区*/

//按键区
__IO uint32_t Key_Set_uwTick;
#define Key_Time 100

uint8_t Key_Val = 0;
uint8_t Old_Key_Val = 0;
uint8_t Key_Up = 0;
uint8_t Key_Down = 0;

//串口区
__IO uint32_t UART_Set_uwTick;
#define UART_Time 500

//char str[40] ={'A','B','C','D'};
uint8_t rx_buffer;

//LCD区
__IO uint32_t LCD_Set_uwTick;
#define LCD_Time 300
uint8_t LCD_Str[21];

//24C02区
uint8_t Save_buffer[4];
float myFloat = 12.50;
float ReadFloat;
uint8_t Read_buffer[4];

//PWM IC相关变量
uint16_t PWM_Up_Cnt;
uint16_t PWM_Down_Cnt;
float Duty;

//主函数
int main(void)
{
	/*系统初始化*/
	HAL_Init();
	SystemClock_Config();
	
	/*外设初始化*/
	
	//LCD初始化
	LCD_Init();	
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
//	LCD_DisplayStringLine(Line0, (uint8_t *)"                    ");
	

	LED_KEY_Init();
	UART1_Init();
	I2CInit();
	Basic_TIM6_Init();
	PWM_IC_TIM8_Init();
	ADC1_Init();
	ADC2_Init();
	
	HAL_UART_Receive_IT(&huart1, &rx_buffer, 1);
	HAL_TIM_Base_Start_IT(&htim6);
	
	HAL_TIM_IC_Start_IT(&htim8,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim8,TIM_CHANNEL_2);

	
	while (1)
	{
		Key_Proc();
		Lcd_Proc();
	}

}

//子程序
void Key_Proc(void)
{
	if((uwTick - Key_Set_uwTick) < Key_Time) return;
	Key_Set_uwTick = uwTick;
	
	Key_Val = Key_Scan();
	Key_Down =  Key_Val&(Key_Val ^ Old_Key_Val);
	Key_Up = ~Key_Val&(Key_Val ^ Old_Key_Val);
	Old_Key_Val = Key_Val;
	
	if (Key_Down == 1) Key1_Proc();
	if (Key_Down == 2) Key2_Proc();
	if (Key_Down == 3) Key3_Proc();
	if (Key_Down == 4) Key4_Proc();
	
}

void Key1_Proc(void)
{
	memcpy(Save_buffer,&myFloat,sizeof(myFloat));
	iic_24C02_Wirte(0x01,Save_buffer,sizeof(Save_buffer));
	LCD_DisplayStringLine(Line3, (uint8_t *)"24c02 wirte!");
}
void Key2_Proc(void)
{
	iic_24C02_Read(0x02,Read_buffer,sizeof(Read_buffer));
	memcpy(&ReadFloat,Read_buffer,sizeof(ReadFloat));
	sprintf((char*)LCD_Str,"Read! v:%f",ReadFloat);
	LCD_DisplayStringLine(Line4,LCD_Str);	
}
void Key3_Proc(void)
{
	Mpc_Wirte(0x77);
}
void Key4_Proc(void)
{
	Mpc_Wirte(0x22);
}

//LCD程序
void Lcd_Proc(void)
{
	if((uwTick - LCD_Set_uwTick) < LCD_Time)return;
	LCD_Set_uwTick = uwTick;
	
	
	sprintf((char*)LCD_Str,"R:%X",Mpc_Read());
	LCD_DisplayStringLine(Line1, LCD_Str);
//	sprintf((char*)LCD_Str,"Val1:%.3f",((((float)getADC1())/4096)*3.3));
//	LCD_DisplayStringLine(Line2, LCD_Str);
//	sprintf((char*)LCD_Str,"Val2:%.3f",((((float)getADC2())/4096)*3.3));
//	LCD_DisplayStringLine(Line3, LCD_Str);
//	sprintf((char*)LCD_Str,"Pwm_F:%06d",(unsigned int)(1000000/PWM_Up_Cnt));
//	LCD_DisplayStringLine(Line4, LCD_Str);
//	sprintf((char*)LCD_Str,"Pwm_Duty:%3.2f",Duty*100);
//	LCD_DisplayStringLine(Line5, LCD_Str);
	
}

//串口
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	
	
	
	/***********************************************************/
	HAL_UART_Receive_IT(&huart1, &rx_buffer, 1);
	
}

//定时器溢出中断
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{





}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM8)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			PWM_Up_Cnt = HAL_TIM_ReadCapturedValue(&htim8,TIM_CHANNEL_1)+1;
			Duty = (float)PWM_Down_Cnt/PWM_Up_Cnt;
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			PWM_Down_Cnt = HAL_TIM_ReadCapturedValue(&htim8,TIM_CHANNEL_2)+1;
		}
	}


}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
