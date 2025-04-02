# 蓝桥杯备赛日志

[TOC]



## 一，主观题

### 引言：

​	在此只作为一个学习上的记录，方便我们代码上的查阅，也是知识的一种总汇，屁话不多说了，干吧。

​																		2025年3月28日 日子人

### 0.总说

​	本文档基于蚂蚁工厂的蓝桥杯嵌入式课程整理，延续其大致思路（CodeMx只作为配置工具，将其生成的代码细化拆分再合并为新工程），孰优孰劣，还很难说。如今最好延续。

​	本文档的主要目的是记录一些代码思路和配置信息，比赛确是学习的好方式，期间也多有收获，仅此作为记录。

### 1.最初工程的建立（系统时钟和其他配置）

​	我们最初的模板是基于CobeMX生成的Source工程，然后以Source为源文件打底，复制生成新工程Project，再用CodeMX不断生成配置代码到Source中，再从Source中复制我们需要的代码到Project，最终以Project为最终作品。

​	首先是时钟，我们高速时钟选择外部晶振，这个步骤会使我们使能我们的GPIOF。

![](.\Pic\1.png)

​	外部晶振的大小是24MHZ，记住这一点。我们需要在时钟树界面配置使系统主频达到80MHZ。

![](.\Pic\2.png)

​	选择外部时钟，24MHZ，3分配 *20 /2，选择锁相环控制，得到系统主频80MHZ。

​	然后更改Project Mananger界面，打开Project侧栏。只更改Project Settings。文件路径建议全英文，这样不容易出问题。

![](.\Pic\3.png)

而在Code Generator中我们选定生成独立的.c.h文件。

![](.\Pic\4.png)

​	此时点击Generate Code,生成文档。我们将此文档复制一份，打开新文档，按照CT117E-M4产品手册里的方法配置Keil5软件。配置完成之后，打开文件，对大量的注释块进行删除。

​	此时main.c文件里不仅只有主函数，还有其他三个函数。

```c
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
```

​	为了简洁和优雅，我们将SystemClock_Config() Error_Handler()函数移植到一个新文件里，而assert_failed函数，可以直接删除。此时工程里有gpio.c文件，我们可以将其中的

```c
  __HAL_RCC_GPIOF_CLK_ENABLE();
```

转移到SystemClock_Config中。

​	此时，我们的工程文档的基础已经打好，下面就可以开始进行我们外设的配置。

### 2.三大金刚——LED KEY LCD外设

​	LED KEY LCD这三兄弟是比赛必出，我们把他们放到一起说。有关GPIO的底层原理我们这里不再赘述。这里讲一下配置的要点和代码的思路。

#### ①LED

​	简单看一下操作原理。

![](.\Pic\5.png)

​	GPIO口与LED小灯的连接经过一个锁存器，锁存器由PD2所控制，PD2输出高电平时，PC8-15才能去控制LED，而PD2输出低电平时，我们的小灯会在原来的状态被锁住……亮的保持亮，暗的继续暗。

​	小灯的一端是高电平，故我们将GPIO置低小灯会亮。而且我们一开始希望灯是灭的，那么我们的PD应该为LOW，而PC应该为HIGH，速度为LOW就可以。都选择推挽输出模式。这样生成的void MX_GPIO_Init(void)就可以作为我们的LED_Init(void)，但此时先不要这么做，为了省事，我可以继续将按键的生成代码一并生成再复制。

​	*我个人在此也有些疑问，我们的PD2需不需要上下拉，官方和课程例程中都没有上下拉，但此处我觉着下拉更好，此处存疑。所有的PC引脚都不需要上下拉。*

此处为LED点亮代码：

```c
void Led_Disp(uint8_t ucLed)
{
	//开始保证小灯熄灭
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOC,ucLed<<8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);

}
```

对GPIOC进行统一操作，通过ucLed变量点亮 PC8到PC15，左移八位是因为原变量为uint16_t（0x0000）类型，uint8_t（0x00）左移8为与其GPIO对应。而对PD2的操作相当于对锁存器的激活。16进制转2进制的知识我们在此不再赘述。

#### ②KEY

![](.\Pic\6.png)

​	按键需要注意，当按键按下时，我们的GPIO**读取的电平为低电平**；没有被按下时，我们的电平为高电平。配置上，输入模式（一定注意，不要在和LED一起配置时将其配置为输出）。外部是配置了高电平的，我们不用对其上下拉，浮空即可。

​	我们对键值的扫描读取方式是：

```c
uint8_t Key_Scan(void)
{
	uint8_t Key_Val = 0;
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == GPIO_PIN_RESET)
		Key_Val = 1;
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET)
		Key_Val = 2;
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == GPIO_PIN_RESET)
		Key_Val = 3;
	if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == GPIO_PIN_RESET)
		Key_Val = 4;
	return Key_Val;
}
```

```c
void Key_Proc(void)
{
	if((uwTick - Key_uwTick_Set) < Key_Peroid)
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
```

而对按键进行处理的程序为：

```c
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
```

这个相当于是一个固定程序，我们对按键的功能封装在了独立的Key_Proc’N‘()中，这段代码我非常喜欢，有两个要紧的点。

```c
//定时程序
if((uwTick - Key_uwTick_Set) <= Key_Peroid)
	return;
Key_uwTick_Set = uwTick;
```

​	我们的Key_Proc函数会被放在主循环中，为了实现“多线程”，设计了这样的一个分频程序。uwTick是配合系统滴答定时器（SysTick）的一个全局变量，每过1ms增加一次，类型为 **__IO uint32_t**。我们再定义一个同类型的变量（如Key_uwTick_Set）作为打点，在主循环中不断比较uwTick与Key_uwTick_Set的大小，小于我们规定的周期便跳出程序，大于便重新打点并执行下面的程序。

​	此处要注意，因为运算符有优先级，我们需要用括号将先进行计算的括起来。

​	然后就是键值处理：

```c
Key_Val = Key_Scan();
Key_Down = Key_Val & (Key_Val ^ Old_Key_Val);
Key_Up = ~Key_Val & (Key_Val ^ Old_Key_Val);
Old_Key_Val = Key_Val;
```

​	不得不说位运算经常是个好的思路，这种键值处理的方式也相当好用且优雅。**Key_Scan();**函数我们上文有提及，他将返回一个被按下的键值。先看**Key_Down = Key_Val & (Key_Val ^ Old_Key_Val);** 为什么按键按下的值就是这玩意呢，方便讲述和理解，我们就假设只有一个按钮，按下为1，松开为0。那么**(Key_Val ^ Old_Key_Val)**异或的作用就是检测键值是否发生变化，变化为1，不变为0。**Key_Val & (Key_Val ^ Old_Key_Val)**那么这一段含义便是，如果键值变化 **Key_Val & 1** 就是按下的键值，**~Key_Val & 1**就是松开的键值。如果不懂不妨代入数值自己写写，会发现这很巧妙，即使把**Key_Val**键值换成 2 (0x02) 3 (0x03) 4 (0x04)，结果也是一样的。

​	如此我们便得到了两个巧妙的工具，**Key_Down**和**Key_Up**，这会有妙用。

##### 按键扩展

长按短按：（自己瞎写的，还没试）

```c
void Key_Proc(void)
{
    //扫描和读取……
    
	if(Key_Down)
    	Set_Tick = Tick;
	if(Key_Up)
	{
    	if(Tick - Set_Tick >= Peroid)
    	{
        //在此编写长按程序
    	}
    	else
    	{
        //在此编写短按程序
    	}
    
	}
}
```

程序应该对，在Key_Peroid小的情况下，Key_Proc()里的减速分频对其没有太大影响。

无操作1秒钟后切换状态：

```c
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
```

​	这是蓝桥杯第8届省赛，其中有个一秒钟电梯按钮没有被点击电梯启动。只是放在这里，不多说了。

#### ③LCD

​	LCD的程序在真实比赛中配置相对简单，因为比赛资料会给出我们的LCD驱动程序，我们要写的只有显示逻辑。在显示上需要用到sprintf()函数，这个函数的使用方式我经常忘记。

```c
sprintf((char *)Lcd_str ,"    data:%d",data);
LCD_DisplayStringLine(Line1,Lcd_str);
```

此代码组合可以实现对变量的显示。我们同样把LCD显示的任务放到一个与按钮类似的分频程序中，用于动态显示我们需要显示的量。

```c
void Lcd_Proc(void)
{
	if((uwTick - Lcd_uwTick_Set) < Lcd_Peroid)
		return;
	Lcd_uwTick_Set = uwTick;
	
    //数据显示逻辑
    sprintf((char *)Lcd_str ,"    data:%d",data);
	LCD_DisplayStringLine(Line1,Lcd_str);
}

```

实现具体功能多参考官方给出的示例代码，我还其中发现其中还有不少好玩的东西。以后有空可以试试。

### 3.ADC编写

##### 配置

​	我们的开发板上有两个旋钮可变电阻和一个I2C可编程电阻，都可以用ADC的方式来读取其上分压。ADC的工作方式和原理是有些复杂的，我们的代码只会采用最简单的方式就足够了，在配置上：

​	我们先在芯片图示化区激活需要的ADC引脚，而后在侧边找到Analog区域，找到相应ADC，点击后：

![](D:\E\Gxct\Pic\9.png)

选择单端输入，在下方配置界面：

![](.\Pic\7.png)

重点是独立模式，异步时钟除2，分辨率改为最高的12bit。

![](D:\E\Gxct\Pic\8.png)

而后是将采样周期选择最大640.5。此时即可生成代码

##### 代码移植

​	我们Source中会生成一个adc.c文件，从中我们copy除了MspDeInit之外（不会用到）的所有代码，在Object文件中建立相应外设文件，并清除注释。

​	1.在添加库文件时，我们需要将三个库文件都添加过来：**stm32g4xx_hal_adc.c  stm32g4xx_hal_adc_ex.c  stm32g4xx_ll_adc.c**

​	2.找到main.h头文件，一路Open过去，一路找到**stm32g4xx_hal_conf.h**开启其中被注释掉的  **#define HAL_ADC_MODULE_ENABLED**

​	3.我们还需要在系统时钟定义中，我的版本的时钟定义生成到了main.c中，我们将其移植到我们的**SystemClock_Config(void)**函数中。

​	完成这三项工作，我们的ADC就可以使用了

使用方法：

```c
uint16_t getADC1(void)
{
	uint16_t adc_val;
	HAL_ADC_Start(&hadc1);
	adc_val = HAL_ADC_GetValue(&hadc1);
	return adc_val;
}
```

​	此函数读取的值并不是单纯的电压值，根据ADC的原理，我们应该将其/4096 再x3.3。

​	在进行这一步时需要注意，要注意浮点数数制的转换。

​	我们可以：

```c
sprintf((char*)LCD_Str,"Val1:%.3f",((((float)getADC1())/4096)*3.3));
LCD_DisplayStringLine(Line2, LCD_Str);
```

​	也可以

```c
sprintf((char*)LCD_Str,"Val1:%.3f",((getADC1()/4096.0)*3.3));
LCD_DisplayStringLine(Line2, LCD_Str);
```

​	错误示例我这里就不放了……也是犯了很多错误，导致最后的数值有问题。

### 4.PWM输出编写

​	不要好奇整理的顺序，这都是根据比赛考频一一整理的，PWM也是经常考察的点，而PWM的输出得借助定时器，我们这里便说说定时器的配置。

​	同上，为节省时间，我们不先对底层进行过多阐述。只着色于我们CodeMX配置和编写。

​	看了眼教程发现这还是挺乱的，我自己写吧。PWM的输出底层原理上应该是相对复杂，但是在我们实际操作中还算简单。

##### 配置

![](.\Pic\10.png)

在Pinout view中，我们只选择一个PWM生成模式，其余先不必去更改。

这里时钟源的选择默认Display，实际来看和内部时钟没有大的区别。

![](.\Pic\11.png)

时钟预分频器设置为 80-1 这样保证我们的定时器产生的脉冲为1mhz，自动重装载寄存器（ARR）和比较寄存器（Pulse）设置参考我们的需求。我们这里的自动重装载寄存器值为1000-1，产生的pwm信号的频率也就是1KHZ，如果是500，产生的频率是2KHZ。

​	此时即可生成代码，然后移植，移植相对简单。如果没记错的话，我的底层库里自动包含TIM库，PWM输出不依赖中断，系统时钟的初始化函数也不需要修改。

##### PWM操作

​	配置并移植完成以后，我们要启动PWM输出：

```c
//启动pwm输出
HAL_TIM_PWM_Start(&htim,TIM_CHANNEL_1);
//停止pwm输出
HAL_TIM_PWM_Stop(&htim,TIM_CHANNEL_1);
```

​	我们通过操作ARR和Compare值来对PWM方波进行调制。需要用到的HAL库函数为：

```c
//配置PWM频率
__HAL_TIM_SetAutoreload(&htim, (uint16_t)pwm_freq_arr);
//配置PWM占空比    
__HAL_TIM_SetCompare(&htim, Channel, (uint16_t)pwm_duty_pulse);
```

​	这样我们的PWM方波便能够随意调节。

### 5.PWM输入（输入比较）

​	首先，我们输入来的PWM信号是从哪来的呢？我们开发板上板载两个方波信号发生器：

![](D:\E\Gxct\Pic\13.png)



​	从PA15,PB4输入两路信号，他们分别为TIM8，TIM3的通道1。

##### 配置

​	配置上我们以PA15为例：

![](.\Pic\12.png)

从模式选择Reset模式（具体原因我还不太明白），边缘选择TI1FP1，时钟也选择内部时钟。

通道1选择输入捕获直接模式，通道2选择输入捕获间接模式。

同时我们需要开启中断：这里需要注意，TIM8和TIM3中断配置界面不一样

![](.\Pic\14.png)

![](.\Pic\15.png)

生成的中断代码也是略有不同，心里要有印象。

代码的移植这里可以和其他TIM相关外设一起配置一起移植，步骤都差不多，这里不再多说。

##### 得到频率和占空比

​	同pwm输出类似，我们的输入捕获也需要代码打开：

```c
	HAL_TIM_IC_Start_IT(&htim8,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim8,TIM_CHANNEL_2);
```

​	开启时要打开两个，且以中断形式打开。不过这里存疑：我们需不需要再启动定时器。*等待测试。*

​	这一段代码也相对固定，而且这一段代码有时候也可能作为简易示波器使用：

```c
//TIM3和TIM8都是这个中断回调函数，将相关量改为相应定时器即可。

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM8)	//定时器判别
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) //通道判别
		{
			PWM_Up_Cnt = HAL_TIM_ReadCapturedValue(&htim8,TIM_CHANNEL_1)+1;
			Duty = (float)PWM_Down_Cnt/PWM_Up_Cnt;
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)	//通道判别
		{
			PWM_Down_Cnt = HAL_TIM_ReadCapturedValue(&htim8,TIM_CHANNEL_2)+1;
		}
	}
}
```

如何去理解等我哪天心情好再写……

​	如果记不住相应结构体怎么写，我们就对着**htim8**一路F12，多少能找到相应的。

### 6.串口编写

​	串口的使用我们仍采用最简单的方式，数据的发送是直接发送，但是接收我们采用中断接收。板载USB转串口，简单来说数据线插在USB1上时就能实现串口通信。（欸？你说USB2咋还没咋用过）我们直接开始配置。

##### 配置

​	有关串口的一些知识也是以后补充，这里仍只说配置要点。根据相应资料，我们选择PA9,PA10两引脚作为USART的TX和RX。要先选择，因为默认的端口并不是这俩。选择之后再进行如下配置：

​	![](.\Pic\16.png)

模式选择异步模式，下面的参数我们大多数采用默认，只有波特率可能会改成9600。

如此即可直接生成代码，此时，我们的源工程中会生成一个usart.c文件，以同上类似的方式将其移植。

注意，系统时钟配置里也会生成一段代码，但是我前几次测试忽略了，结果代码也能用（……）很迷惑。有可能是因为默认配置之类的。

##### 代码编写

串口的发送及其简单：

```c
//串口发送函数
HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
//使用案例
AL_UART_Transmit(&huart, (uint8_t *)str, len, 100);
```

而串口的接收相对复杂，为了不堵塞程序，我们采用中断接收的方式。

```c
//在初始化完串口之后，我们需要开启第一次中断
HAL_UART_Receive_IT(&huart1,Uart_Rx_Str,7);
```

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		HAL_UART_Receive_IT(&huart1,Uart_Rx_Str,7);
	}
}

```

我们编写串口中断回调函数（这里也提醒我们转移it.c里面的文件），在每次中断回调之后都再次开启一次接收中断。也可以在此函数中编写对串口接收到的信息进行进一步处理。

### 7.I2C编写

​	有一个好消息，我们的I2C驱动代码不用我们手动配置；也有个坏消息，我们需要手写两个外设的I2C通讯。同上，我们这里不详细讲述I2C到底是什么，只是代码的编写。

![](.\Pic\17.png)

在底层驱动代码中，有我们需要的HAL库驱动代码。我们将其移植到我们的工程中。

```c
//驱动代码
void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);
```

移植就是简单的复制粘贴，移植完成后就是我们的中间层驱动代码的编写，中间层驱动代码如果说要是完全背下来，应该是相对困难的，不过好在也难在我们有手册，可以基于手册去进行记忆。

还有一点就是，参考文档有一点可能需要更改。

```c
/**
  * @brief I2C等待确认信号
  * @param None
  * @retval None
  */
unsigned char I2CWaitAck(void)
{
    unsigned short cErrTime = 5;
    SDA_Input_Mode();
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    while(SDA_Input())
    {
        cErrTime--;
        delay1(DELAY_TIME);
        if (0 == cErrTime)
        {
            SDA_Output_Mode();
            I2CStop();
            return ERROR;
        }
    }
    //*************************************************//
    //SDA_Output_Mode();
    //SCL_Output(0);
    //delay1(DELAY_TIME);
    //return SUCCESS;
    //*************************************************//
    /*	此处代码应该把SDA_Output_Mode();移到最下面
    */
    SCL_Output(0);
    delay1(DELAY_TIME);
	SDA_Output_Mode();
    return SUCCESS;
}
```

如何记得这件事呢，我们看到这个等待应答函数就应该想到：等一等！然后把**SDA_Output_Mode();**移到最下面。

##### 24C02

​	24C02是一个存储器，能实现离电后数据的存储。下面是手册的原文。

![](.\Pic\i2c.jpg)

1 ，设备地址有7位（0xAn），但我们看作8位, 最后一位标志着读写 (写为0，读为1，0xA0和0xA1)。
2 ，设备发数据等芯片应答, 设备收数据给芯片发应答 。
3 ，读取时假写, 假写时没有停止信号 。

据此，我们可以进行代码的封装。

```c
//24C02相关代码
void iic_24C02_Wirte(uint8_t ucAddr,uint8_t *pucBuf,uint8_t ucNum)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	
	I2CSendByte(ucAddr);
	I2CWaitAck();
	
	while(ucNum--)
	{
		I2CSendByte(*pucBuf++);
		I2CWaitAck();
	}
	I2CStop();
	delay1(500);				//为了保证通讯，我们的写操作需要一定延时
	
}

void iic_24C02_Read(uint8_t ucAddr,uint8_t *pucBuf,uint8_t ucNum)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	
	I2CSendByte(ucAddr);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xa1);
	I2CWaitAck();
	
	while(ucNum--)
	{
		*pucBuf++ = I2CReceiveByte();
		if(ucNum)
			I2CSendAck();
		else
			I2CSendNotAck();
	}
	
	I2CStop();
}

```

这里我们就可以操作24C02去进行写入读取了。

##### MCP4017

​	这玩意压根没见用过，但来都来了，搞吧。写入读取相比24C02更加简单。

![](.\Pic\18.png)

手册上给的并不好看，直接记忆也无妨：

```c
//MPC代码
void Mpc_Wirte(uint8_t pucBuf)
{
	
	I2CStart();
	I2CSendByte(0x5E);
	I2CWaitAck();
	
	I2CSendByte(pucBuf);
	I2CWaitAck();
	I2CStop();
	
}

uint8_t Mpc_Read(void)
{
	uint8_t val;
	I2CStart();
	I2CSendByte(0x5F);
	I2CWaitAck();
	
	val = I2CReceiveByte();
	I2CSendNotAck();		//同样在读时，发送无应答（实际上无应答这个说法啊……不知道外国人咋想）
	I2CStop();
	
	return val;
}
```

### The End

​	我们对我们配置的外设进行一定的总结：

| 外设/需要修改处 | Drivers Hal库 | stm32g4xx_hal_conf.h | stm32g4xx_it.c | System Clock Configuration |
| --------------- | ------------- | -------------------- | -------------- | -------------------------- |
| LED&LCD&KEY     | 无            | 无                   | 无             | 无                         |
| ADC             | 需（包含LL）  | 需                   | 需             | 需                         |
| PWM_OUT         | 无            | 需                   | 无             | 无                         |
| PWM_IN          | 无            | 需                   | 需             | 无                         |
| UART            | 需            | 需                   | 需             | 需                         |
| I2C             | 无            | 无                   | 无             | 无                         |

这个表是根据我个人的CodeMX做的，更换版本就很有可能不同……比赛又更换了版本，我得再试试。

​	主观题现在就总结到这个程度。早春的月光清冷地随凉风透过半开的窗户，窗户的玻璃上又随灯光反射着我和电脑、键盘的影子。虽说我常质疑意义这个东西是否有意义，此时又不免思考：我做这一切的意义是什么，究竟想要的是什么。大抵，比赛的目的也只是为了知识。浮名会散去，荣誉会散去，唯有知识是留存在自己身上的，这哗啦哗啦一长篇下来啊，你估计也明白了，我们此行的目的几乎已经达到了。