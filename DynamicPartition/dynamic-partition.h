#ifndef _DYNAMIC_PARTITION_H_
#define _DYNAMIC_PARTITION_H_
#include "process.h"

extern void new_free_space_tab();

void init_spa_tab();

void first_fit(Process *p, int req_size, char flag);

void print_spa_tab();

void destroy_spa_tab();

#endif // !_DYNAMIC_PARTITION_H_

