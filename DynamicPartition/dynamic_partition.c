#include "process.h"
#include "space.h"
#include <stdio.h>
#include <stdlib.h>
#define INITSPACEBLOCK 6

struct FreeSpaceTab {
	Space *head;
	int length;
};

struct FreeSpaceTab *free_space_tab;

extern void new_free_space_tab()
{
	free_space_tab = (struct FreeSpaceTab*)malloc(sizeof(struct FreeSpaceTab));
	free_space_tab->head = NULL;
	free_space_tab->length = 0;
}

//在空闲分区表表头插入
static void insertHead(Space *s)
{
	if (free_space_tab->length == 0) {
		free_space_tab->head = s;
	}
	else {
		s->next = free_space_tab->head;
		free_space_tab->head->prior = s;
		free_space_tab->head = s;
	}
	(free_space_tab->length)++;
}

//在空闲分区表的某个表项之前插入, s为待插入元素, next为表中已有表项
static void insertBefore(Space *s, Space *next)
{
	Space *tmp;
	if (next == NULL && s == NULL) {
		printf("Error: 您插入的表项不合法！");
		exit(-1);
	}
	if (next == free_space_tab->head) {
		insertHead(s);
	}
	else {
		s->next = next;
		s->prior = next->prior;
		s->prior->next = s;
		s->next->prior = s;
		(free_space_tab->length)++;
	}
	//插入成功后修改分区号
	s->spa_id = s->next->spa_id;
	tmp = s->next;
	while (tmp != NULL) {
		tmp->spa_id++;
		tmp = tmp->next;
	}
}

static void delete(Space *s)
{
	if (free_space_tab->length == 0) {
		printf("Error: 空闲分区表为空, 无法删除！\n");
		exit(-1);
	}
	if (s == free_space_tab->head) {
		free_space_tab->head = free_space_tab->head->next;
		free(s);
	}
	else {
		s->prior->next = s->next;
		s->next->prior = s->prior;
		free(s);
	}
	(free_space_tab->length)--;
}

void init_spa_tab()
{
	int size = rand() % 16 + 35;
	int init_mem = 640;
	for (int i = INITSPACEBLOCK; i > 0; i--) {
		if (i == 1) {
			size = init_mem;
		}
		int addr = rand() % (50 * i + 1) + 250 * i;
		insertHead(new_space(i, size, addr));
		init_mem -= size;
		size = rand() % 151 + 50;
	}
}

void print_spa_tab()
{
	Space *tmp = NULL;
	printf("-----------------------\n");
	printf("分区号 分区大小 分区始址\n");
	printf("-----------------------\n");
	tmp = free_space_tab->head;
	while (tmp != NULL) {
		printf("%3d %7d %10d\n", tmp->spa_id, tmp->spa_size, tmp->spa_addr);
		tmp = tmp->next;
	}
	printf("-----------------------\n");
}

void mem_alloc(Process *p, int req_size)
{
	printf("申请前的空闲分区表: \n");
	print_spa_tab();
	printf("%s正在申请%dKB的内存\n", p->pro_name, req_size);
	Space *tmp = free_space_tab->head;
	while (tmp != NULL) {
		if (req_size <= tmp->spa_size) {
			p->pro_size += req_size;
			p->pro_addr = tmp->spa_addr;
			tmp->spa_size -= req_size;
			tmp->spa_addr += req_size;
			printf("内存分配成功!\n");
			break;
		}
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		printf("Error: 请求分配内存失败！系统里没有足够内存供改程序使用！\n");
	}
	printf("提出请求后的空闲分区表: \n");
	print_spa_tab();
}

void mem_free(Process *p, int req_size)
{
	Space *tmp = free_space_tab->head;
	printf("释放前的空闲分区表: \n");
	print_spa_tab();
	while (tmp != NULL) {
		if (p->pro_addr < tmp->spa_addr) {
			Space *prior = tmp->prior;
			Space *next = tmp;
			if ((p->pro_addr + req_size == next->spa_addr) && (prior->spa_addr + prior->spa_size == p->pro_addr)) {
				//释放的内存块既与前又与后相邻合并三个空闲内存块
				//首地址等于prior的地址
				//prior的size加上req_size和next的size
				prior->spa_size = prior->spa_size + req_size + next->spa_size;
				//把next从空闲分区表删除
				delete(next);
				printf("%s释放了%dKB的内存\n", p->pro_name, req_size);
				printf("释放内存后的空闲分区表: \n");
				print_spa_tab();
				break;
			}
			else if (prior->spa_addr + prior->spa_size == p->pro_addr) {
				//释放的内存块只与前相邻合并这两个内存块
				//首地址依旧等于prior的首地址
				//prior的size要加上req_size
				prior->spa_size += req_size;
				printf("%s释放了%dKB的内存\n", p->pro_name, req_size);
				printf("释放内存后的空闲分区表: \n");
				print_spa_tab();
				break;
			}
			else if (p->pro_addr + req_size == next->spa_addr) {
				//释放的内存块只与后相邻合并这两个内存块
				//next的首地址改为当前释放进程的首地址
				next->spa_addr = p->pro_addr;
				//next的size要加上req_size
				next->spa_size += p->pro_size;
				printf("%s释放了%dKB的内存\n", p->pro_name, req_size);
				printf("释放内存后的空闲分区表: \n");
				print_spa_tab();
				break;
			}
			else {
				//不与前相邻也不与后相邻新建一个表项插入空闲分区表相对位置
				Space *news = new_space(next->spa_id, p->pro_size, p->pro_addr);
				insertBefore(news, next);
				printf("%s释放了%dKB的内存\n", p->pro_name, req_size);
				printf("释放内存后的空闲分区表: \n");
				print_spa_tab();
				break;
			}
		}
		tmp = tmp->next;
	}
}

void first_fit(Process *p, int req_size, char flag)
{
	system("cls");
	if (flag != 'a' && flag != 'f') {
		printf("Error: 请求出错！请求只能是申请内存和释放内存，请检查您的参数\n");
		exit(-1);
	}
	else if (flag == 'a') {
		mem_alloc(p, req_size);
	}
	else {
		mem_free(p, req_size);
	}
	system("pause");
}

void destroy_spa_tab()
{
	Space *current = free_space_tab->head;
	Space *next;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	free(free_space_tab);
}