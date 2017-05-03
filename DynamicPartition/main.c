#include "dynamic-partition.h"
#include "process.h"
#include <stdio.h>
#define PROCESSNUM 8//进程数

void menu();
void test_FF();
void test_BF();

int main(int argc, char *argv)
{
	menu();

	return 0;
}

void menu() 
{
	char c;
	printf("******************************************\n");
	printf("*************** 菜 单 ********************\n");
	printf("********** 1 首次适应算法 ****************\n");
	printf("********** 2 最佳适应算法 ****************\n");
	printf("******************************************\n");
	printf("\n");
	printf("请输入您的选项: \n");
	
	c = getchar();
	if (c != '1' && c != '2') {
		printf("菜单没有该选项！\n");
	}
	else if (c == '1') {
		test_FF();
	}
	else {
		test_BF();
	}
}

void test_FF()
{
	char *pname[PROCESSNUM] = { "P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8" };
	Process *pro_set[PROCESSNUM];

	for (int i = 0; i < PROCESSNUM; i++) {
		pro_set[i] = new_process(pname[i], 0, 0);
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
}

void test_BF()
{
	char *pname[PROCESSNUM] = { "P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8" };
	Process *pro_set[PROCESSNUM];

	for (int i = 0; i < PROCESSNUM; i++) {
		pro_set[i] = new_process("p", 0, 0);
	}
	new_free_space_tab();
	init_spa_tab();
	best_fit(pro_set[0], 130, 'a');
	best_fit(pro_set[1], 60, 'a');
	best_fit(pro_set[2], 100, 'a');
	best_fit(pro_set[1], pro_set[1]->pro_size, 'f');
	best_fit(pro_set[3], 200, 'a');
	best_fit(pro_set[2], pro_set[2]->pro_size, 'f');
	best_fit(pro_set[0], pro_set[0]->pro_size, 'f');
	best_fit(pro_set[4], 140, 'a');
	best_fit(pro_set[5], 60, 'a');
	best_fit(pro_set[6], 50, 'a');
	best_fit(pro_set[7], 60, 'a');
	for (int i = 0; i < PROCESSNUM; i++) {
		destroy_process(pro_set[i]);
	}
	destroy_spa_tab();
}