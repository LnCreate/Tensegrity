#include "user_tasks.h"
#include "user_FOC_uart.h"
#include <stdbool.h>
#include <math.h>
#include "stdio.h"
#include "usart.h"

#define Pi 3.1415926535f



/**
Ӳ�����衢ģ���ʼ������
**/
void User_Init(void)
{	
	
//	osDelay(300);
	__disable_irq();
	User_CommFOCInit();
	__enable_irq();
}

/**
����ϵͳ�����������������ȼ�
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
����ϵͳ������������θ����ȼ�
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
����ϵͳ�����������������ȼ�
**/

void StartTaskLow(void *argument)
{
	for(;;)
  {	
		osDelay(20);//20ms
  }
}



