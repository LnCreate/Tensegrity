#ifndef __USER_QUEUE_H
#define __USER_QUEUE_H

#include "main.h"

// �����������
#define QUEUE_MAX_LENGTH 50		

// ����������Ԫ�ص���������
typedef uint8_t queue_data_t;			

// ���нṹ�嶨��
typedef struct
{
	queue_data_t data[QUEUE_MAX_LENGTH+1];
	int head, tail;
}queue_t;

/**
 * @brief ��ʼ��һ���հ׶���
 * @param *q Ҫ��ʼ���Ķ���
*/
void Init_Empty_Queue(queue_t *q);

/**
 * @brief ��ѯ���г���
 * @param *q Ҫ��ѯ�Ķ���
 * @return ���г���
*/
int Size_Of_Queue(queue_t *q);

/**
 * @brief ��ѯ�������±�Ϊindex��Ԫ�ص�ֵ
 * @param *q Ҫ��ѯ�Ķ���
 * @param index ��ѯ�±�
 * @return �������±�Ϊindex��Ԫ�ص�ֵ
*/
queue_data_t Value_Of_Queue(queue_t *q, int index);

/**
 * @brief ���
 * @param *q Ҫ����Ķ���
 * @param x Ҫ�����Ԫ��
*/
void Push_To_Queue(queue_t *q, queue_data_t x);

/**
 * @brief ����
 * @param *q Ҫ���ӵĶ���
 * @return ���ӵ�Ԫ��
*/
queue_data_t Pop_From_Queue(queue_t *q);

#endif
