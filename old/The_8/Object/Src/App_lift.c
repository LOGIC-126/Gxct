#include "App_lift.h"


uint8_t Floot_num;

uint8_t Lift_Lcd_str[21];

//电梯上升
void Lift_Up(void)
{
	//PA4高，PA6 80 
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	
	HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
	__HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,800);
	
	//流水灯

	Led_Disp(0x10);
	HAL_Delay(1500);
	Led_Disp(0x20);
	HAL_Delay(1500);
	Led_Disp(0x40);
	HAL_Delay(1500);
	Led_Disp(0x80);
	HAL_Delay(1500);
	Led_Disp(0x00);
	
	HAL_TIM_PWM_Stop(&htim16,TIM_CHANNEL_1);
	Floot_num++;
	sprintf((char *)Lift_Lcd_str ,"    Floot_Num:%d",Floot_num);
	LCD_DisplayStringLine(Line4,Lift_Lcd_str);
}

void Lift_Down(void)
{
	//PA4低，PA6 60 
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	
	HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
	__HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,600);
	
	//流水灯

	Led_Disp(0x80);
	HAL_Delay(1500);
	Led_Disp(0x40);
	HAL_Delay(1500);
	Led_Disp(0x20);
	HAL_Delay(1500);
	Led_Disp(0x10);
	HAL_Delay(1500);
	Led_Disp(0x00);
	
	HAL_TIM_PWM_Stop(&htim16,TIM_CHANNEL_1);
	Floot_num--;
	sprintf((char *)Lift_Lcd_str ,"    Floot_Num:%d",Floot_num);
	LCD_DisplayStringLine(Line4,Lift_Lcd_str);
}

//电梯开关门
void Lift_Open(void)
{
	//PA5 高 ，PA7 60
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
	
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,300);
	
	LCD_DisplayStringLine(Line1,(uint8_t *)"Lift is Opening......");
	HAL_Delay(4000);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
}

void Lift_Clos(void)
{
	//PA5 低 ，PA7 50
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
	
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,250);
	
	LCD_DisplayStringLine(Line1,(uint8_t *)"Lift is closing......");
	HAL_Delay(4000);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);

}

