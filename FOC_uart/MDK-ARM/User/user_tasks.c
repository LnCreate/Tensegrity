#include "user_tasks.h"
#include "user_FOC_uart.h"
#include <stdbool.h>
#include <math.h>
#include "stdio.h"
#include "usart.h"

#define Pi 3.1415926535f



/**
硬件外设、模块初始化配置
**/
void User_Init(void)
{	
	
//	osDelay(300);
	__disable_irq();
	User_CommFOCInit();
	__enable_irq();
}

/**
操作系统任务服务函数，最高优先级
**/
int cnt;
void StartTaskHigh(void *argument)
{
	User_Init();
	
	for(;;)
  {		
		cnt++;
		if(cnt>=250 && FOC.init_ok == 1)
		{
			User_CommFOCSendTask();
			cnt = 0;
		}
		osDelay(2);
  } 
}

/**
操作系统任务服务函数，次高优先级
**/
void StartTaskNormal(void *argument)
{

	for(;;)
  {
		User_CommFOCRecTask();
		osDelay(10);
  }
}
/**
操作系统任务服务函数，最底优先级
**/

void StartTaskLow(void *argument)
{
	for(;;)
  {	
		osDelay(20);//20ms
  }
}



