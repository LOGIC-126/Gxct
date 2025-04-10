#include "main.h"
#include "RCC/bsp_rcc.h"
#include "KEY_LED/bsp_key_led.h"
#include "LCD/bsp_lcd.h"
#include "UART/bsp_Uart.h"
#include "PWM/bsp_pwm.h"

/*变量声明区*/

//Key
__IO uint32_t Key_uwTick;
#define Key_Period 150
uint8_t Key_Val = 0;
uint8_t Old_Key_Val = 0;
uint8_t Key_Up = 0;
uint8_t Key_Down = 0;

//Lcd
__IO uint32_t Lcd_uwTick;
#define Lcd_Period 300
uint8_t Lcd_str[21];
	
	//状态机参量

#define LCD_PSD 0x01
#define LCD_STA 0x02
uint8_t Lcd_In = LCD_PSD;

//Uart
uint8_t Uart_Tx_Str[21]={'A','B','C'};
uint8_t Uart_Rx_Str[8];

//逻辑参量
uint8_t Psd_View[3] = {'@','@','@'};
uint8_t Psd_Word[3] = {'1','2','3'};
uint8_t Word_Time = 0;

//子函数声明
void Key_Proc(void);
void Key_Proc1(void);
void Key_Proc2(void);
void Key_Proc3(void);
void Key_Proc4(void);
void Lcd_Proc(void);

int main(void)
{
	//系统内核初始化
	HAL_Init();
	SystemClock_Config();
	Rcc_Init();
	//外设初始化
	
	KEY_LED_Init();
	UART1_Init();
	PWM_TIM2_Init();
	
	//LCD初始化
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	
	
	
	
	
//	LCD_DisplayStringLine(Line0, (uint8_t *)"                    ");
//	LCD_DisplayStringLine(Line1, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line2, (uint8_t *)"        PSD         ");
//	LCD_DisplayStringLine(Line3, (uint8_t *)"                    ");
//	LCD_DisplayStringLine(Line4, (uint8_t *)"                    ");
//	
//	LCD_DisplayStringLine(Line5, (uint8_t *)"                    ");
//	LCD_DisplayStringLine(Line6, (uint8_t *)"       HAL LIB      ");
//	LCD_DisplayStringLine(Line7, (uint8_t *)"                    ");
//	LCD_DisplayStringLine(Line8, (uint8_t *)"         @80        ");
//	LCD_DisplayStringLine(Line9, (uint8_t *)"                    ");
	
	HAL_UART_Receive_IT(&huart1,Uart_Rx_Str,7);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	
	while (1)
	{
		Key_Proc();
		Lcd_Proc();
	}

}

void Lcd_Proc(void)
{
	if((uwTick - Lcd_uwTick) < Lcd_Period) return;
	Lcd_uwTick = uwTick;
	
	
	if (Lcd_In == LCD_PSD)
	{
		sprintf((char *)Lcd_str,"    B1:%c",Psd_View[0]);
		LCD_DisplayStringLine(Line4,Lcd_str);
		sprintf((char *)Lcd_str,"    B2:%c",Psd_View[1]);
		LCD_DisplayStringLine(Line5,Lcd_str);
		sprintf((char *)Lcd_str,"    B3:%c",Psd_View[2]);
		LCD_DisplayStringLine(Line6,Lcd_str);
	}
	else if(Lcd_In == LCD_STA)
	{
		
	
	}
}

void Key_Proc(void)
{
	if((uwTick - Key_uwTick) < Key_Period) return;
	Key_uwTick = uwTick;

	Key_Val = Key_Scan();
	Key_Down = Key_Val & (Key_Val ^ Old_Key_Val);
	Key_Up = ~Key_Val & (Key_Val ^ Old_Key_Val);
	Old_Key_Val = Key_Val;

	if (Key_Down == 1) Key_Proc1();
	if (Key_Down == 2) Key_Proc2();
	if (Key_Down == 3) Key_Proc3();
	if (Key_Down == 4) Key_Proc4();

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		if((Uart_Rx_Str[0] == Psd_Word[0])&(Uart_Rx_Str[1] == Psd_Word[1])&
			(Uart_Rx_Str[2] == Psd_Word[2]))
		{
			Psd_Word[0] = Uart_Rx_Str[4];
			Psd_Word[1] = Uart_Rx_Str[5];
			Psd_Word[2] = Uart_Rx_Str[6];
		}

	
	}

	HAL_UART_Receive_IT(&huart1,Uart_Rx_Str,7);
}


void Key_Proc1(void)
{
	if (Psd_View[0] == '@')
	{
		Psd_View[0] = '0';
	}
	else if(Psd_View[0] >= '9')
	{
		Psd_View[0] = '0';
	}
	else
	{
		Psd_View[0]++;
	}

}

void Key_Proc2(void)
{
	if (Psd_View[1] == '@')
	{
		Psd_View[1] = '0';
	}
	else if(Psd_View[1] >= '9')
	{
		Psd_View[1] = '0';
	}
	else
	{
		Psd_View[1]++;
	}


}

void Key_Proc3(void)
{
	if (Psd_View[2] == '@')
	{
		Psd_View[2] = '0';
	}
	else if(Psd_View[2] >= '9')
	{
		Psd_View[2] = '0';
	}
	else
	{
		Psd_View[2]++;
	}


}

void Key_Proc4(void)
{
	if((Psd_View[0] == Psd_Word[0])&(Psd_View[1] == Psd_Word[1])&(Psd_View[2] == Psd_Word[2]))
	{
		Lcd_In = LCD_STA;
		LCD_Clear(Black);
		LCD_DisplayStringLine(Line2, (uint8_t *)"        STA         ");
		LCD_DisplayStringLine(Line4, (uint8_t *)"     F:2000Hz");
		LCD_DisplayStringLine(Line5, (uint8_t *)"     D:10%");
		Led_Disp(0x01);
		HAL_Delay(5000);
		Led_Disp(0x00);
		LCD_Clear(Black);
		LCD_DisplayStringLine(Line2, (uint8_t *)"        PSD         ");
		Psd_View[0] = Psd_View[1] = Psd_View[2] = '@';
		Lcd_In = LCD_PSD;
		Word_Time = 0 ;
	}
	else
	{
		Psd_View[0] = Psd_View[1] = Psd_View[2] = '@';
		Word_Time++;
		if (Word_Time >=3)
		{
			for (int i = 0;i<=25;i++)
			{
				Led_Disp(0x02);
				HAL_Delay(100);
				Led_Disp(0x00);
				HAL_Delay(100);
			}

		}
	
	}
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
