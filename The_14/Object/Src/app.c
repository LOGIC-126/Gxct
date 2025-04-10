#include "app.h"
#include "TIM/bsp_tim.h"

//ռ�ձȲ��䣬Ƶ�ʸı�
void Pwm_Out(uint16_t Ferq,uint16_t duty)
{
	__HAL_TIM_SetAutoreload(&htim2,Ferq);		// N/F = ��
//	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,(Ferq * duty/100));
	
	uint16_t compareValue = (uint16_t)(Ferq) * (uint16_t)(duty) / 100;
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, compareValue);
//	HAL_Delay(100);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
}




