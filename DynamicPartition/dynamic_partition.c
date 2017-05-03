#include "process.h"
#include "space.h"
#include "dynamic-partition.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define INITSPACEBLOCK 6//初始空闲内存块数量

struct FreeSpaceTab {
	Space *head;//表头
	Space *tail;//表尾
	int length;//表长
};
struct FreeSpaceTab *free_space_tab;//空闲分区表

/**
* brief: 创建一个空的空闲分区表
* param: void
* return: void
*/
extern void new_free_space_tab()
{
	free_space_tab = (struct FreeSpaceTab*)malloc(sizeof(struct FreeSpaceTab));
	free_space_tab->head = NULL;
	free_space_tab->tail = NULL;
	free_space_tab->length = 0;
}

//在空闲分区表表头插入表项
static void insertHead(Space *s)
{
	if (s == NULL) {
		printf("Error: 您插入的表项不合法！");
		exit(-1);
	}
	if (free_space_tab->length == 0) {
		free_space_tab->head = s;
		free_space_tab->tail = s;
	}
	else {
		s->next = free_space_tab->head;
		free_space_tab->head->prior = s;
		free_space_tab->head = s;
	}
	(free_space_tab->length)++;
}

/**
* brief: 初始化空闲分区表
* param: void
* return: void
*/
extern void init_spa_tab()
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

//在空闲分区表表尾插入
static void insertTail(Space *s) {
	if (s == NULL) {
		printf("Error: 您插入的表项不合法！");
		exit(-1);
	}
	s->next = free_space_tab->tail->next;
	s->prior = free_space_tab->tail;
	free_space_tab->tail->next = s;
	free_space_tab->tail = s;
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

//从空闲分区表删除表项
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

//打印空闲分区表
static void print_spa_tab()
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

//根据地址从低到高实现空闲分区表排序
static void sort_addr()
{
	Space *outtmp = free_space_tab->head;

	while (outtmp != NULL) {
		Space *min = outtmp;
		Space *intmp = outtmp->next;
		while (intmp != NULL) {
			if (intmp->spa_addr < min->spa_addr) {
				min = intmp;
			}
			intmp = intmp->next;
		}
		if (min != outtmp) {
			int tmp_size = min->spa_size;
			int tmp_addr = min->spa_addr;
			min->spa_size = outtmp->spa_size;
			min->spa_addr = outtmp->spa_addr;
			outtmp->spa_size = tmp_size;
			outtmp->spa_addr = tmp_addr;
		}
		outtmp = outtmp->next;
	}
}

//根据表项内存从大到小实现空闲分区表排序
static void sort_size()
{
	Space *outtmp = free_space_tab->head;

	while (outtmp != NULL) {
		Space *min = outtmp;
		Space *intmp = outtmp->next;
		while (intmp != NULL) {
			if (intmp->spa_size < min->spa_size) {
				min = intmp;
			}
			intmp = intmp->next;
		}
		if (min != outtmp) {
			int tmp_size = min->spa_size;
			int tmp_addr = min->spa_addr;
			min->spa_size = outtmp->spa_size;
			min->spa_addr = outtmp->spa_addr;
			outtmp->spa_size = tmp_size;
			outtmp->spa_addr = tmp_addr;
		}
		outtmp = outtmp->next;
	}
}

//内存申请
static void mem_alloc(Process *p, int req_size)
{
	printf("申请前的空闲分区表: \n");
	print_spa_tab();
	printf("\n作业%s正在申请%dKB的内存\n", p->pro_name, req_size);
	Space *tmp = free_space_tab->head;
	while (tmp != NULL) {
		if (req_size <= tmp->spa_size) {
			p->pro_size += req_size;
			p->pro_addr = tmp->spa_addr;
			tmp->spa_size -= req_size;
			tmp->spa_addr += req_size;
			printf("内存分配成功! 作业始址为: %d\n\n", p->pro_addr);
			break;
		}
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		printf("Error: 请求分配内存失败！系统里没有足够内存供改程序使用！\n\n");
	}
}


//释放内存, 成功返回true, 失败返回false
static bool mem_free(Process *p, int req_size)
{
	Space *tmp = free_space_tab->head;
	printf("释放前的空闲分区表: \n");
	print_spa_tab();
	sort_addr();//按地址排序

	//如果释放的内存块地址在空闲分区表的队首表项的首地址前
	if (p->pro_addr < tmp->spa_addr) {
		//释放队头前的内存只有两种情况:一种是相邻，一种是不相邻
		if (p->pro_addr + req_size == tmp->spa_addr) {
			//如果相邻,队头的内存大小加上释放的内存，队头的首地址改为被释放内存的首地址
			tmp->spa_size += req_size;
			tmp->spa_addr = p->pro_addr;
			return true;
		}
		else {
			//如果不相邻,在队头前插入新表项
			Space *news = new_space(tmp->spa_id, p->pro_size, p->pro_addr);
			insertBefore(news, tmp);
			return true;
		}
	}
	else if (p->pro_addr > free_space_tab->tail->spa_addr) {
	//如果释放的内存块地址在空闲分区表的队尾表项的首地址后
		Space *tail = free_space_tab->tail;
		if (tail->spa_addr + tail->spa_size == p->pro_addr) {
			//如果与空闲分区表尾部相邻把其和空闲分区表尾部合并
			tail->spa_size += req_size;
			return true;
		}
		else {
			//不与空闲分区表的尾部相邻，创建一个新表项在空闲分区表尾部插入
			Space *news = new_space(tail->spa_id + 1, p->pro_size, p->pro_addr);
			insertTail(news);
			return true;
		}
	}
	else {
	//如果释放的内存块的首地址不在队首表项地址前也不在队尾表项地址后
	//循环遍历整个空闲分区表找到适合的位置
		while (tmp != NULL) {
			if (p->pro_addr < tmp->spa_addr) {
				Space *prior = tmp->prior;
				Space *next = tmp;
				//如果不是队头有四种情况
				if ((p->pro_addr + req_size == next->spa_addr) && (prior->spa_addr + prior->spa_size == p->pro_addr)) {
					//第一种:释放的内存块既与前又与后相邻合并三个空闲内存块
					//首地址等于prior的地址
					//prior的size加上req_size和next的size
					prior->spa_size = prior->spa_size + req_size + next->spa_size;
					//把next从空闲分区表删除
					delete(next);
					return true;
				}
				else if (prior->spa_addr + prior->spa_size == p->pro_addr) {
					//第二种:释放的内存块只与前相邻合并这两个内存块
					//首地址依旧等于prior的首地址
					//prior的size要加上req_size
					prior->spa_size += req_size;
					return true;
				}
				else if (p->pro_addr + req_size == next->spa_addr) {
					//第三种:释放的内存块只与后相邻合并这两个内存块
					//next的首地址改为当前释放进程的首地址
					next->spa_addr = p->pro_addr;
					//next的size要加上req_size
					next->spa_size += p->pro_size;
					return true;
				}
				else {
					//第四种:不与前相邻也不与后相邻新建一个表项插入空闲分区表相对位置
					Space *news = new_space(next->spa_id, p->pro_size, p->pro_addr);
					insertBefore(news, next);
					return true;
				}
			}
			tmp = tmp->next;
		}
	}
	return false;
}

/**
* brief: 首次适应算法
* param: p申请内存的作业 req_size申请的内存大小 flag请求标志a为请求分配f为请求释放
* return: void
*/
extern void first_fit(Process *p, int req_size, char flag)
{
	system("cls");
	if (flag != 'a' && flag != 'f') {
		printf("Error: 请求出错！请求只能是申请内存和释放内存，请检查您的参数\n");
		exit(-1);
	}
	else if (flag == 'a') {
		mem_alloc(p, req_size);
		printf("提出请求后的空闲分区表: \n");
	}
	else {
		if (mem_free(p, req_size)) {
			printf("\n作业%s(始址: %d)释放了%dKB的内存\n\n", p->pro_name, p->pro_addr, req_size);
			printf("释放内存后的空闲分区表: \n");
		}
		else {
			printf("Error: 释放内存失败！\n");
		}
	}
	print_spa_tab();
	system("pause");
}


/**
* brief: 最佳适应算法
* param: p申请内存的作业 req_size申请的内存大小 flag请求标志a为请求分配f为请求释放
* return: void
*/
extern void best_fit(Process *p, int req_size, char flag)
{
	system("cls");
	if (flag != 'a' && flag != 'f') {
		printf("Error: 请求出错！请求只能是申请内存和释放内存，请检查您的参数\n");
		exit(-1);
	}
	else if (flag == 'a') {
		sort_size();//再申请之前对空闲分区表按照内存大小从大到小排序
		mem_alloc(p, req_size);//调用分配内存函数
		printf("提出请求后的空闲分区表: \n");
	}
	else {
		if (mem_free(p, req_size)) {//调用释放内存函数
			printf("\n作业%s(始址: %d)释放了%dKB的内存\n\n", p->pro_name, p->pro_addr, req_size);
			printf("释放内存后的空闲分区表: \n");
		}
		else {
			printf("Error: 释放内存失败！\n");
		}
	}
	sort_size();//对分区表排序后在打印
	print_spa_tab();
	system("pause");
}


/**
* brief: 析构函数(销毁空闲分区表)
* param: void
* return: void
*/
extern void destroy_spa_tab()
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