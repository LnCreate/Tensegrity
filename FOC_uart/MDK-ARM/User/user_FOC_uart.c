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
��������ϵͳ�����ϵ��stm32�����޷��������ݣ�������λstm32����������գ�����״̬�¿��������գ���������

�����������ģ���ϵ����Զ�����ģ���Լ���Ϣ��stm32���ڽ�����Щ������Ϣʱ���г�ʼ������ܵ����޷���������������

����취������ģ���Ӧ���ڵĳ�ʼ��Ӧ����ģ�鷢�����Լ���Ϣ����У���������
*/

/**
 * @brief FOC��ͨ�ų�ʼ������
**/
void User_CommFOCInit(void)
{
	__HAL_UART_CLEAR_IDLEFLAG(&USART_FOC_HUSART);
	__HAL_UART_ENABLE_IT(&USART_FOC_HUSART, UART_IT_IDLE); //ʹ��IDLE�ж�

//DMA���պ������˾�һ��Ҫ�ӣ����ӽ��ղ�����һ�δ�������ʵ���ݣ��ǿյģ��Ҵ�ʱ���յ������ݳ���Ϊ�����������ݳ���
	HAL_UART_Receive_DMA(&USART_FOC_HUSART,(uint8_t*)FOC.usart_rx_buf,USART_FOC_RX_SIZE);
	FOC.init_ok = 1;
//	FOC.handle.test1 = 0;
//	FOC.handle.test2 = 0;
//	FOC.handle.test3 = 0.0f;
}

/**
 * @brief FOC��DMA�жϴ�������Ӧ�÷���stm32f1xx_it.c������
**/
void User_CommFOCIRQHandler(void)
{	

	if(__HAL_UART_GET_FLAG(&USART_FOC_HUSART,UART_FLAG_IDLE) != RESET)//idle��־����λ
	{ 
		FOC.rx_handle = NOT_COMPLETE;
		__HAL_UART_CLEAR_IDLEFLAG(&USART_FOC_HUSART);
		HAL_UART_DMAStop(&huart1);  
		FOC.rx_data_len =  USART_FOC_RX_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); //�ܼ�����ȥδ��������ݸ������õ��Ѿ����յ����ݸ���
		User_CommFOCDataParas();
		
		HAL_UART_Receive_DMA(&USART_FOC_HUSART, (uint8_t*)FOC.usart_rx_buf, USART_FOC_RX_SIZE);
	 }
}

/**
 * @brief FOC��rx���ݴ���������״̬������ʽ�����ζ�pos��speed���д��������򵥵ľ������
**/
void User_CommFOCDataParas(void)
{
	uint8_t i = 0,j = 0;
	char str1[20] = {0};
	char str2[20] = {0};
	for(i=0;i<FOC.rx_data_len;i++)
	{
		
		if (FOC.usart_rx_buf[i] == 0x0d && FOC.usart_rx_buf[i+1]== 0x0a && i!=0) // ��ʱ������ͣ���һλ��0x0d��ʹ��
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
	FOC.rx_handle = COMPLETE; // ֻҪ������жϣ�������û��ִ�У�����ձ�־λ��������ܻ�ֹͣ����
	return;
}


/**
* @brief FOC�����ݽ���Task����״̬������ʽ�����η���pos��speed����
**/
void User_CommFOCRecTask(void)
{
	static char Motor0_PosCommand_Table[30]={"r axis0.encoder.pos_estimate"}; // Ҫʹ�þ�̬����ȫ�ֱ�������Ȼû���ȶ���ַ�����ͻ��
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
			FOC.rx_handle = COMPLETE; // ����������
		}
		break;
		case POS:
		{
			if(FOC.rx_handle==COMPLETE)
			{
				memset(FOC.usart_rx_buf,0,sizeof(FOC.usart_rx_buf));
				FOC.rx_state = POS; // ������ڷ�����֮ǰ����Ϊ��������˲��Ϳ��Զ�ȡ
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
				FOC.rx_state = SPEED; // ������ڷ�����֮ǰ����Ϊ��������˲��Ϳ��Զ�ȡ
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
* @brief FOC��rx�����Task������λ������
**/
void User_CommFOCSendTask(void)
{

	static int i=0;
	static int dirflag=0; // ����
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
 * @brief atof֮ǰ�ļ�⺯��������б������Ƿ�Ӧ�ý���atof����
 * @param str: ��������б�
 * @return (bool)��Ч����1����Ч����0
**/
int is_valid_atof_format(const char* str) 
{
	// ����ַ����Ƿ�Ϊ��
	if (str == NULL) return 0;

	// ����ǰ���ո�
	while (isspace(*str)) str++;

	// ���������
	if (*str == '+' || *str == '-') str++;

	// �����λ�Ƿ�Ϊ0
	if (*str == '0') {
		str++;
		// �����λΪ0��ֻ����0.xxx�ĸ�ʽ
		if (*str != '.') return 0;
	}

	// ���ʣ����ַ�
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

	// ������еļ�鶼ͨ���ˣ�����1
	return 1;
}

