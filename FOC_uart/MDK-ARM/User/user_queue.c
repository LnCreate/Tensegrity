#include "user_queue.h"

#include <string.h>

/**
 * @brief 初始化一个空白队列
 * @param *q 要初始化的队列
*/
void Init_Empty_Queue(queue_t *q)
{
	memset(q->data, 0, sizeof(q->data));
	q->head = q->tail = 0;
}

/**
 * @brief 查询队列长度
 * @param *q 要查询的队列
 * @return 队列长度
*/
int Size_Of_Queue(queue_t *q)
{
	if(q->tail >= q->head)
	{
		int size = q->tail - q->head;
		return size;
	}
	else
	{
		int size = q->tail + 1 + QUEUE_MAX_LENGTH - q->head;
		return size;
	}
}

/**
 * @brief 查询队列中下标为index的元素的值
 * @param *q 要查询的队列
 * @param index 查询下标
 * @return 队列中下标为index的元素的值
*/
queue_data_t Value_Of_Queue(queue_t *q, int index)
{
	// 队伍长度不足，直接返回
	if(Size_Of_Queue(q) < index + 1)
		return 0;
	
	index = (q->head + index + 1) % QUEUE_MAX_LENGTH;
	
	return q->data[index];
}

/**
 * @brief 入队
 * @param *q 要加入的队列
 * @param x 要加入的元素
*/
void Push_To_Queue(queue_t *q, queue_data_t x)
{
	// 队列满了就不添加了
	if(Size_Of_Queue(q) == QUEUE_MAX_LENGTH)
		return ;
	
	q->tail = (q->tail + 1) % QUEUE_MAX_LENGTH;
	q->data[q->tail] = x;
}

/**
 * @brief 出队
 * @param *q 要出队的队列
 * @return 出队的元素
*/
queue_data_t Pop_From_Queue(queue_t *q)
{
	// 队列长度为0，错误
	if(Size_Of_Queue(q) == 0)
		return 0;
	
	q->head = (q->head + 1) % QUEUE_MAX_LENGTH;
	
	return q->data[q->head];
}
