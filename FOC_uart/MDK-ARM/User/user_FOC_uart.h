#ifndef _USER_FOC_UART_H
#define _USER_FOC_UART_H


#include "main.h"


#define USART_FOC_TX_SIZE	50
#define USART_FOC_RX_SIZE	100

typedef struct
{
	float pos;
	float speed;
	float force;
}RECIEVE_MES_TYPEDEF;

typedef enum
{
	NONE=0x00,
	POS,
	SPEED,
	FORCE,
}REC_STATE_TYPEDEF;

typedef enum
{
	NOT_COMPLETE = 0x00,
	COMPLETE,
}HANDLE_TYPEDEF;

typedef enum
{
	NOT_BUSY = 0x00,
	BUSY,
}BUSY_TYPEDEF;

typedef struct
{
	char usart_tx_buf[USART_FOC_TX_SIZE];
	char usart_rx_buf[USART_FOC_RX_SIZE];	
	RECIEVE_MES_TYPEDEF rx_mes; // 收到的消息的存储
	REC_STATE_TYPEDEF rx_state; // 接收消息的状态机
	HANDLE_TYPEDEF rx_handle; // 数据是否处理完成
	uint16_t rx_data_len; // 接收到的数据长度
	
	REC_STATE_TYPEDEF tx_state; // 发送消息请求的状态机，不包含发送运动命令
	uint8_t init_ok; // 是否完成初始化
}USART_FOC_TYPEDEF;

extern USART_FOC_TYPEDEF FOC;




void User_CommFOCInit(void);
void User_CommFOCIRQHandler(void);
void User_CommFOCDataParas(void);

int is_valid_atof_format(const char* str);
void User_CommFOCRecTask(void);
void User_CommFOCSendTask(void);

#endif










