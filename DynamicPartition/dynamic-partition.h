#ifndef _DYNAMIC_PARTITION_H_
#define _DYNAMIC_PARTITION_H_
#include "process.h"

/**
* brief: 创建一个空的空闲分区表
* param: void
* return: void
*/
extern void new_free_space_tab();

/**
* brief: 初始化空闲分区表
* param: void
* return: void
*/
extern void init_spa_tab();

/**
* brief: 首次适应算法
* param: p申请内存的作业 req_size申请的内存大小 flag请求标志a为请求分配f为请求释放
* return: void
*/
extern void first_fit(Process *p, int req_size, char flag);

/**
* brief: 最佳适应算法
* param: p申请内存的作业 req_size申请的内存大小 flag请求标志a为请求分配f为请求释放
* return: void
*/
extern void best_fit(Process *p, int req_size, char flag);

/**
* brief: 析构函数(销毁空闲分区表)
* param: void
* return: void
*/
extern void destroy_spa_tab();

#endif // !_DYNAMIC_PARTITION_H_

