#ifndef _DYNAMIC_PARTITION_H_
#define _DYNAMIC_PARTITION_H_
#include "process.h"

/**
* brief: ����һ���յĿ��з�����
* param: void
* return: void
*/
extern void new_free_space_tab();

/**
* brief: ��ʼ�����з�����
* param: void
* return: void
*/
extern void init_spa_tab();

/**
* brief: �״���Ӧ�㷨
* param: p�����ڴ����ҵ req_size������ڴ��С flag�����־aΪ�������fΪ�����ͷ�
* return: void
*/
extern void first_fit(Process *p, int req_size, char flag);

/**
* brief: �����Ӧ�㷨
* param: p�����ڴ����ҵ req_size������ڴ��С flag�����־aΪ�������fΪ�����ͷ�
* return: void
*/
extern void best_fit(Process *p, int req_size, char flag);

/**
* brief: ��������(���ٿ��з�����)
* param: void
* return: void
*/
extern void destroy_spa_tab();

#endif // !_DYNAMIC_PARTITION_H_

