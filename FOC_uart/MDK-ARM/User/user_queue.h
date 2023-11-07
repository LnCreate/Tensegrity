#ifndef __USER_QUEUE_H
#define __USER_QUEUE_H

#include "main.h"

// 队列中最长长度
#define QUEUE_MAX_LENGTH 50		

// 队列中数据元素的数据类型
typedef uint8_t queue_data_t;			

// 队列结构体定义
typedef struct
{
	queue_data_t data[QUEUE_MAX_LENGTH+1];
	int head, tail;
}queue_t;

/**
 * @brief 初始化一个空白队列
 * @param *q 要初始化的队列
*/
void Init_Empty_Queue(queue_t *q);

/**
 * @brief 查询队列长度
 * @param *q 要查询的队列
 * @return 队列长度
*/
int Size_Of_Queue(queue_t *q);

/**
 * @brief 查询队列中下标为index的元素的值
 * @param *q 要查询的队列
 * @param index 查询下标
 * @return 队列中下标为index的元素的值
*/
queue_data_t Value_Of_Queue(queue_t *q, int index);

/**
 * @brief 入队
 * @param *q 要加入的队列
 * @param x 要加入的元素
*/
void Push_To_Queue(queue_t *q, queue_data_t x);

/**
 * @brief 出队
 * @param *q 要出队的队列
 * @return 出队的元素
*/
queue_data_t Pop_From_Queue(queue_t *q);

#endif
