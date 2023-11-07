#include "user_FOC_uart.h"
#include "user_queue.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
	
#define USART_FOC_HUSART huart1
#define USART_FOC_HDMA_RX hdma_usart1_rx

USART_FOC_TYPEDEF FOC;

/*
问题现象：系统重新上电后stm32串口无法接收数据，单独复位stm32后可正常接收（调试状态下可正常接收）？？？？

问题分析：该模块上电后会自动发送模块自检信息，stm32若在接收这些串口信息时进行初始化则可能导致无法正常工作！！！

解决办法：将该模块对应串口的初始化应放在模块发送完自检信息后进行！！！！！
*/

/**
 * @brief FOC的通信初始化函数
**/
void User_CommFOCInit(void)
{
	__HAL_UART_CLEAR_IDLEFLAG(&USART_FOC_HUSART);
	__HAL_UART_ENABLE_IT(&USART_FOC_HUSART, UART_IT_IDLE); //使能IDLE中断

//DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
	HAL_UART_Receive_DMA(&USART_FOC_HUSART,(uint8_t*)FOC.usart_rx_buf,USART_FOC_RX_SIZE);
	FOC.init_ok = 1;
//	FOC.handle.test1 = 0;
//	FOC.handle.test2 = 0;
//	FOC.handle.test3 = 0.0f;
}

/**
 * @brief FOC的DMA中断处理函数，应该放在stm32f1xx_it.c中运行
**/
void User_CommFOCIRQHandler(void)
{	

	if(__HAL_UART_GET_FLAG(&USART_FOC_HUSART,UART_FLAG_IDLE) != RESET)//idle标志被置位
	{ 
		FOC.rx_handle = NOT_COMPLETE;
		__HAL_UART_CLEAR_IDLEFLAG(&USART_FOC_HUSART);
		HAL_UART_DMAStop(&huart1);  
		FOC.rx_data_len =  USART_FOC_RX_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); //总计数减去未传输的数据个数，得到已经接收的数据个数
		User_CommFOCDataParas();
		
		HAL_UART_Receive_DMA(&USART_FOC_HUSART, (uint8_t*)FOC.usart_rx_buf, USART_FOC_RX_SIZE);
	 }
}

/**
 * @brief FOC的rx数据处理函数，以状态机的形式，依次对pos、speed进行处理，包含简单的纠错机制
**/
void User_CommFOCDataParas(void)
{
	uint8_t i = 0,j = 0;
	char str1[20] = {0};
	char str2[20] = {0};
	for(i=0;i<FOC.rx_data_len;i++)
	{
		
		if (FOC.usart_rx_buf[i] == 0x0d && FOC.usart_rx_buf[i+1]== 0x0a && i!=0) // 有时候会误发送，第一位是0x0d，痛苦
		{
				for (int j = 0; j < i; j++) str1[j] = FOC.usart_rx_buf[j];
				if(is_valid_atof_format(str1))
				{
					switch(FOC.rx_state) 
					{
						case NONE:break;
						case POS:FOC.rx_mes.pos = atof(str1);break;
						case SPEED:FOC.rx_mes.speed = atof(str1);break;
						default:break;
					}
				}
				break;
		}
	}
	FOC.rx_state = NONE;
	FOC.rx_handle = COMPLETE; // 只要进入过中断，不管有没有执行，都清空标志位，否则可能会停止运行
	return;
}


/**
* @brief FOC的数据接收Task，以状态机的形式，依次发送pos、speed请求
**/
void User_CommFOCRecTask(void)
{
	static char Motor0_PosCommand_Table[30]={"r axis0.encoder.pos_estimate"}; // 要使用静态或者全局变量，不然没有稳定地址，发送会崩
	Motor0_PosCommand_Table[28]=0x0D;
	Motor0_PosCommand_Table[29]=0x0A;
	static char Motor0_VelCommand_Table[30]={"r axis0.encoder.vel_estimate"};
	Motor0_VelCommand_Table[28]=0x0D;
	Motor0_VelCommand_Table[29]=0x0A;
	
	switch(FOC.tx_state)
	{
		case NONE:
		{
			FOC.tx_state = POS;
			FOC.rx_handle = COMPLETE; // 先正常进入
		}
		break;
		case POS:
		{
			if(FOC.rx_handle==COMPLETE)
			{
				memset(FOC.usart_rx_buf,0,sizeof(FOC.usart_rx_buf));
				FOC.rx_state = POS; // 必须放在发命令之前，因为发完命令瞬间就可以读取
				FOC.rx_handle = NOT_COMPLETE;
				HAL_UART_Transmit_DMA(&USART_FOC_HUSART,(uint8_t*)Motor0_PosCommand_Table,sizeof(Motor0_PosCommand_Table));
				FOC.tx_state = SPEED;
			}
		}
		break;
		case SPEED:
		{
			if(FOC.rx_handle==COMPLETE)
			{
				memset(FOC.usart_rx_buf,0,sizeof(FOC.usart_rx_buf));
				FOC.rx_state = SPEED; // 必须放在发命令之前，因为发完命令瞬间就可以读取
				FOC.rx_handle = NOT_COMPLETE;
				HAL_UART_Transmit_DMA(&USART_FOC_HUSART,(uint8_t*)Motor0_VelCommand_Table,sizeof(Motor0_VelCommand_Table));
				FOC.tx_state = POS;
			}
		}
		break;
		default:break;
	}
}

/**
* @brief FOC的rx命令发送Task，发送位置命令
**/
void User_CommFOCSendTask(void)
{

	static int i=0;
	static int dirflag=0; // 正向
	static char pos_buf[20]; 
	sprintf(pos_buf,"%s%d","p 0 ",i);
	pos_buf[18] = 0x0D;
	pos_buf[19] = 0x0A;

	HAL_UART_Transmit_DMA(&USART_FOC_HUSART,(uint8_t*)pos_buf,sizeof(pos_buf));


	if(!dirflag) i++;
	else i--;
	if(i==10 || i==-10) dirflag=!dirflag;
	
}


/**
 * @brief atof之前的检测函数，检测列表内容是否应该进行atof处理
 * @param str: 待处理的列表
 * @return (bool)有效返回1，无效返回0
**/
int is_valid_atof_format(const char* str) 
{
	// 检查字符串是否为空
	if (str == NULL) return 0;

	// 跳过前导空格
	while (isspace(*str)) str++;

	// 检查正负号
	if (*str == '+' || *str == '-') str++;

	// 检查首位是否为0
	if (*str == '0') {
		str++;
		// 如果首位为0，只接受0.xxx的格式
		if (*str != '.') return 0;
	}

	// 检查剩余的字符
	int dot_count = 0;
	while (*str)
	{
		if (*str == '.') 
		{
			dot_count++;
			if (dot_count > 1) return 0;
		}
		else if (!isdigit(*str))
		{
			return 0;
		}
		str++;
	}

	// 如果所有的检查都通过了，返回1
	return 1;
}

