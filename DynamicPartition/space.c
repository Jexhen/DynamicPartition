#include <stdlib.h>
struct Space_ {
	int spa_id;
	int spa_size;
	int spa_addr;
	struct Space_ *prior;
	struct Space_ *next;
};
typedef struct Space_ Space;

extern Space *new_space(int id, int size, int addr)
{
	Space *s = (Space*)malloc(sizeof(Space));
	s->spa_id = id;
	s->spa_size = size;
	s->spa_addr = addr;
	s->prior = NULL;
	s->next = NULL;

	return s;
}

extern void set_spa_id(Space *s, int id)
{
	s->spa_id = id;
}

extern void set_spa_size(Space *s, int size)
{
	s->spa_size = size;
}

extern void set_spa_addr(Space *s, int addr)
{
	s->spa_addr = addr;
}

extern int get_spa_id(Space *s)
{
	return s->spa_id;
}

extern int get_spa_size(Space *s)
{
	return s->spa_size;
}

extern int get_spa_addr(Space *s)
{
	return s->spa_addr;
}

extern void destroy_space(Space *s)
{
	free(s);
}