#include "process.h"
#include "space.h"
#include "dynamic-partition.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define INITSPACEBLOCK 6//��ʼ�����ڴ������

struct FreeSpaceTab {
	Space *head;//��ͷ
	Space *tail;//��β
	int length;//��
};
struct FreeSpaceTab *free_space_tab;//���з�����

/**
* brief: ����һ���յĿ��з�����
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

//�ڿ��з������ͷ�������
static void insertHead(Space *s)
{
	if (s == NULL) {
		printf("Error: ������ı���Ϸ���");
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
* brief: ��ʼ�����з�����
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

//�ڿ��з������β����
static void insertTail(Space *s) {
	if (s == NULL) {
		printf("Error: ������ı���Ϸ���");
		exit(-1);
	}
	s->next = free_space_tab->tail->next;
	s->prior = free_space_tab->tail;
	free_space_tab->tail->next = s;
	free_space_tab->tail = s;
}

//�ڿ��з������ĳ������֮ǰ����, sΪ������Ԫ��, nextΪ�������б���
static void insertBefore(Space *s, Space *next)
{
	Space *tmp;
	if (next == NULL && s == NULL) {
		printf("Error: ������ı���Ϸ���");
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
	//����ɹ����޸ķ�����
	s->spa_id = s->next->spa_id;
	tmp = s->next;
	while (tmp != NULL) {
		tmp->spa_id++;
		tmp = tmp->next;
	}
}

//�ӿ��з�����ɾ������
static void delete(Space *s)
{
	if (free_space_tab->length == 0) {
		printf("Error: ���з�����Ϊ��, �޷�ɾ����\n");
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

//��ӡ���з�����
static void print_spa_tab()
{
	Space *tmp = NULL;
	printf("-----------------------\n");
	printf("������ ������С ����ʼַ\n");
	printf("-----------------------\n");
	tmp = free_space_tab->head;
	while (tmp != NULL) {
		printf("%3d %7d %10d\n", tmp->spa_id, tmp->spa_size, tmp->spa_addr);
		tmp = tmp->next;
	}
	printf("-----------------------\n");
}

//���ݵ�ַ�ӵ͵���ʵ�ֿ��з���������
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

//���ݱ����ڴ�Ӵ�Сʵ�ֿ��з���������
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

//�ڴ�����
static void mem_alloc(Process *p, int req_size)
{
	printf("����ǰ�Ŀ��з�����: \n");
	print_spa_tab();
	printf("\n��ҵ%s��������%dKB���ڴ�\n", p->pro_name, req_size);
	Space *tmp = free_space_tab->head;
	while (tmp != NULL) {
		if (req_size <= tmp->spa_size) {
			p->pro_size += req_size;
			p->pro_addr = tmp->spa_addr;
			tmp->spa_size -= req_size;
			tmp->spa_addr += req_size;
			printf("�ڴ����ɹ�! ��ҵʼַΪ: %d\n\n", p->pro_addr);
			break;
		}
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		printf("Error: ��������ڴ�ʧ�ܣ�ϵͳ��û���㹻�ڴ湩�ĳ���ʹ�ã�\n\n");
	}
}


//�ͷ��ڴ�, �ɹ�����true, ʧ�ܷ���false
static bool mem_free(Process *p, int req_size)
{
	Space *tmp = free_space_tab->head;
	printf("�ͷ�ǰ�Ŀ��з�����: \n");
	print_spa_tab();
	sort_addr();//����ַ����

	//����ͷŵ��ڴ���ַ�ڿ��з�����Ķ��ױ�����׵�ַǰ
	if (p->pro_addr < tmp->spa_addr) {
		//�ͷŶ�ͷǰ���ڴ�ֻ���������:һ�������ڣ�һ���ǲ�����
		if (p->pro_addr + req_size == tmp->spa_addr) {
			//�������,��ͷ���ڴ��С�����ͷŵ��ڴ棬��ͷ���׵�ַ��Ϊ���ͷ��ڴ���׵�ַ
			tmp->spa_size += req_size;
			tmp->spa_addr = p->pro_addr;
			return true;
		}
		else {
			//���������,�ڶ�ͷǰ�����±���
			Space *news = new_space(tmp->spa_id, p->pro_size, p->pro_addr);
			insertBefore(news, tmp);
			return true;
		}
	}
	else if (p->pro_addr > free_space_tab->tail->spa_addr) {
	//����ͷŵ��ڴ���ַ�ڿ��з�����Ķ�β������׵�ַ��
		Space *tail = free_space_tab->tail;
		if (tail->spa_addr + tail->spa_size == p->pro_addr) {
			//�������з�����β�����ڰ���Ϳ��з�����β���ϲ�
			tail->spa_size += req_size;
			return true;
		}
		else {
			//������з������β�����ڣ�����һ���±����ڿ��з�����β������
			Space *news = new_space(tail->spa_id + 1, p->pro_size, p->pro_addr);
			insertTail(news);
			return true;
		}
	}
	else {
	//����ͷŵ��ڴ����׵�ַ���ڶ��ױ����ַǰҲ���ڶ�β�����ַ��
	//ѭ�������������з������ҵ��ʺϵ�λ��
		while (tmp != NULL) {
			if (p->pro_addr < tmp->spa_addr) {
				Space *prior = tmp->prior;
				Space *next = tmp;
				//������Ƕ�ͷ���������
				if ((p->pro_addr + req_size == next->spa_addr) && (prior->spa_addr + prior->spa_size == p->pro_addr)) {
					//��һ��:�ͷŵ��ڴ�����ǰ��������ںϲ����������ڴ��
					//�׵�ַ����prior�ĵ�ַ
					//prior��size����req_size��next��size
					prior->spa_size = prior->spa_size + req_size + next->spa_size;
					//��next�ӿ��з�����ɾ��
					delete(next);
					return true;
				}
				else if (prior->spa_addr + prior->spa_size == p->pro_addr) {
					//�ڶ���:�ͷŵ��ڴ��ֻ��ǰ���ںϲ��������ڴ��
					//�׵�ַ���ɵ���prior���׵�ַ
					//prior��sizeҪ����req_size
					prior->spa_size += req_size;
					return true;
				}
				else if (p->pro_addr + req_size == next->spa_addr) {
					//������:�ͷŵ��ڴ��ֻ������ںϲ��������ڴ��
					//next���׵�ַ��Ϊ��ǰ�ͷŽ��̵��׵�ַ
					next->spa_addr = p->pro_addr;
					//next��sizeҪ����req_size
					next->spa_size += p->pro_size;
					return true;
				}
				else {
					//������:����ǰ����Ҳ����������½�һ�����������з��������λ��
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
* brief: �״���Ӧ�㷨
* param: p�����ڴ����ҵ req_size������ڴ��С flag�����־aΪ�������fΪ�����ͷ�
* return: void
*/
extern void first_fit(Process *p, int req_size, char flag)
{
	system("cls");
	if (flag != 'a' && flag != 'f') {
		printf("Error: �����������ֻ���������ڴ���ͷ��ڴ棬�������Ĳ���\n");
		exit(-1);
	}
	else if (flag == 'a') {
		mem_alloc(p, req_size);
		printf("��������Ŀ��з�����: \n");
	}
	else {
		if (mem_free(p, req_size)) {
			printf("\n��ҵ%s(ʼַ: %d)�ͷ���%dKB���ڴ�\n\n", p->pro_name, p->pro_addr, req_size);
			printf("�ͷ��ڴ��Ŀ��з�����: \n");
		}
		else {
			printf("Error: �ͷ��ڴ�ʧ�ܣ�\n");
		}
	}
	print_spa_tab();
	system("pause");
}


/**
* brief: �����Ӧ�㷨
* param: p�����ڴ����ҵ req_size������ڴ��С flag�����־aΪ�������fΪ�����ͷ�
* return: void
*/
extern void best_fit(Process *p, int req_size, char flag)
{
	system("cls");
	if (flag != 'a' && flag != 'f') {
		printf("Error: �����������ֻ���������ڴ���ͷ��ڴ棬�������Ĳ���\n");
		exit(-1);
	}
	else if (flag == 'a') {
		sort_size();//������֮ǰ�Կ��з��������ڴ��С�Ӵ�С����
		mem_alloc(p, req_size);//���÷����ڴ溯��
		printf("��������Ŀ��з�����: \n");
	}
	else {
		if (mem_free(p, req_size)) {//�����ͷ��ڴ溯��
			printf("\n��ҵ%s(ʼַ: %d)�ͷ���%dKB���ڴ�\n\n", p->pro_name, p->pro_addr, req_size);
			printf("�ͷ��ڴ��Ŀ��з�����: \n");
		}
		else {
			printf("Error: �ͷ��ڴ�ʧ�ܣ�\n");
		}
	}
	sort_size();//�Է�����������ڴ�ӡ
	print_spa_tab();
	system("pause");
}


/**
* brief: ��������(���ٿ��з�����)
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