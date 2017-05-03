#include "dynamic-partition.h"
#include "process.h"
#include <stdio.h>
#define PROCESSNUM 8

int main(int argc, char *argv)
{
	Process *pro_set[PROCESSNUM];

	for (int i = 0; i < PROCESSNUM; i++) {
		pro_set[i] = new_process("p", 0, 0);
	}
	new_free_space_tab();
	init_spa_tab();
	first_fit(pro_set[0], 130, 'a');
	first_fit(pro_set[1], 60, 'a');
	first_fit(pro_set[2], 100, 'a');
	first_fit(pro_set[1], pro_set[1]->pro_size, 'f');
	first_fit(pro_set[3], 200, 'a');
	first_fit(pro_set[2], pro_set[2]->pro_size, 'f');
	first_fit(pro_set[0], pro_set[0]->pro_size, 'f');
	first_fit(pro_set[4], 140, 'a');
	first_fit(pro_set[5], 60, 'a');
	first_fit(pro_set[6], 50, 'a');
	first_fit(pro_set[7], 60, 'a');

	for (int i = 0; i < PROCESSNUM; i++) {
		destroy_process(pro_set[i]);
	}
	destroy_spa_tab();

	return 0;
}