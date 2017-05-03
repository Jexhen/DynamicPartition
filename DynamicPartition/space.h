#ifndef _SPACE_H_
#define _SPACE_H_

struct Space_ {
	int spa_id;
	int spa_size;
	int spa_addr;
	struct Space_ *prior;
	struct Space_ *next;
};
typedef struct Space_ Space;

extern Space *new_space(int id, int size, int addr);

extern void set_spa_id(Space *s, int id);

extern void set_spa_size(Space *s, int size);

extern void set_spa_addr(Space *s, int addr);

extern int get_spa_id(Space *s);

extern int get_spa_size(Space *s);

extern int get_spa_addr(Space *s);

extern void destroy_space(Space *s);

#endif // !_SPACE_H_
