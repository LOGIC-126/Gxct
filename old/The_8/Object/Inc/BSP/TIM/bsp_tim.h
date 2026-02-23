#include "main.h"


extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim16;

void PWM_PA6_Init(void);
void PWM_PA7_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

