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

//�ڿ��з������ͷ����
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
	printf("������ ������С ����ʼַ\n");
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
	printf("����ǰ�Ŀ��з�����: \n");
	print_spa_tab();
	printf("%s��������%dKB���ڴ�\n", p->pro_name, req_size);
	Space *tmp = free_space_tab->head;
	while (tmp != NULL) {
		if (req_size <= tmp->spa_size) {
			p->pro_size += req_size;
			p->pro_addr = tmp->spa_addr;
			tmp->spa_size -= req_size;
			tmp->spa_addr += req_size;
			printf("�ڴ����ɹ�!\n");
			break;
		}
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		printf("Error: ��������ڴ�ʧ�ܣ�ϵͳ��û���㹻�ڴ湩�ĳ���ʹ�ã�\n");
	}
	printf("��������Ŀ��з�����: \n");
	print_spa_tab();
}

void mem_free(Process *p, int req_size)
{
	Space *tmp = free_space_tab->head;
	printf("�ͷ�ǰ�Ŀ��з�����: \n");
	print_spa_tab();
	while (tmp != NULL) {
		if (p->pro_addr < tmp->spa_addr) {
			Space *prior = tmp->prior;
			Space *next = tmp;
			if ((p->pro_addr + req_size == next->spa_addr) && (prior->spa_addr + prior->spa_size == p->pro_addr)) {
				//�ͷŵ��ڴ�����ǰ��������ںϲ����������ڴ��
				//�׵�ַ����prior�ĵ�ַ
				//prior��size����req_size��next��size
				prior->spa_size = prior->spa_size + req_size + next->spa_size;
				//��next�ӿ��з�����ɾ��
				delete(next);
				printf("%s�ͷ���%dKB���ڴ�\n", p->pro_name, req_size);
				printf("�ͷ��ڴ��Ŀ��з�����: \n");
				print_spa_tab();
				break;
			}
			else if (prior->spa_addr + prior->spa_size == p->pro_addr) {
				//�ͷŵ��ڴ��ֻ��ǰ���ںϲ��������ڴ��
				//�׵�ַ���ɵ���prior���׵�ַ
				//prior��sizeҪ����req_size
				prior->spa_size += req_size;
				printf("%s�ͷ���%dKB���ڴ�\n", p->pro_name, req_size);
				printf("�ͷ��ڴ��Ŀ��з�����: \n");
				print_spa_tab();
				break;
			}
			else if (p->pro_addr + req_size == next->spa_addr) {
				//�ͷŵ��ڴ��ֻ������ںϲ��������ڴ��
				//next���׵�ַ��Ϊ��ǰ�ͷŽ��̵��׵�ַ
				next->spa_addr = p->pro_addr;
				//next��sizeҪ����req_size
				next->spa_size += p->pro_size;
				printf("%s�ͷ���%dKB���ڴ�\n", p->pro_name, req_size);
				printf("�ͷ��ڴ��Ŀ��з�����: \n");
				print_spa_tab();
				break;
			}
			else {
				//����ǰ����Ҳ����������½�һ�����������з��������λ��
				Space *news = new_space(next->spa_id, p->pro_size, p->pro_addr);
				insertBefore(news, next);
				printf("%s�ͷ���%dKB���ڴ�\n", p->pro_name, req_size);
				printf("�ͷ��ڴ��Ŀ��з�����: \n");
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
		printf("Error: �����������ֻ���������ڴ���ͷ��ڴ棬�������Ĳ���\n");
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