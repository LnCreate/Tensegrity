#include "user_queue.h"

#include <string.h>

/**
 * @brief ��ʼ��һ���հ׶���
 * @param *q Ҫ��ʼ���Ķ���
*/
void Init_Empty_Queue(queue_t *q)
{
	memset(q->data, 0, sizeof(q->data));
	q->head = q->tail = 0;
}

/**
 * @brief ��ѯ���г���
 * @param *q Ҫ��ѯ�Ķ���
 * @return ���г���
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
 * @brief ��ѯ�������±�Ϊindex��Ԫ�ص�ֵ
 * @param *q Ҫ��ѯ�Ķ���
 * @param index ��ѯ�±�
 * @return �������±�Ϊindex��Ԫ�ص�ֵ
*/
queue_data_t Value_Of_Queue(queue_t *q, int index)
{
	// ���鳤�Ȳ��㣬ֱ�ӷ���
	if(Size_Of_Queue(q) < index + 1)
		return 0;
	
	index = (q->head + index + 1) % QUEUE_MAX_LENGTH;
	
	return q->data[index];
}

/**
 * @brief ���
 * @param *q Ҫ����Ķ���
 * @param x Ҫ�����Ԫ��
*/
void Push_To_Queue(queue_t *q, queue_data_t x)
{
	// �������˾Ͳ������
	if(Size_Of_Queue(q) == QUEUE_MAX_LENGTH)
		return ;
	
	q->tail = (q->tail + 1) % QUEUE_MAX_LENGTH;
	q->data[q->tail] = x;
}

/**
 * @brief ����
 * @param *q Ҫ���ӵĶ���
 * @return ���ӵ�Ԫ��
*/
queue_data_t Pop_From_Queue(queue_t *q)
{
	// ���г���Ϊ0������
	if(Size_Of_Queue(q) == 0)
		return 0;
	
	q->head = (q->head + 1) % QUEUE_MAX_LENGTH;
	
	return q->data[q->head];
}
