#include <stdlib.h>
struct Process_ {
	char *pro_name;
	int pro_size;
	int pro_addr;
};
typedef struct Process_ Process;

extern Process *new_process(char *name, int size, int addr)
{
	Process *p = (Process*)malloc(sizeof(Process));
	p->pro_name = name;
	p->pro_size = size;
	p->pro_addr = addr;

	return p;
}

extern char *get_pro_name(Process *pro)
{
	return pro->pro_name;
}

extern int get_pro_size(Process  *pro)
{
	return pro->pro_size;
}

extern int get_pro_addr(Process *pro)
{
	return pro->pro_addr;
}

extern void set_pro_size(Process *p, int size)
{
	p->pro_size = size;
}

extern void set_pro_addr(Process *p, int addr)
{
	p->pro_addr = addr;
}

extern void destroy_process(Process *pro)
{
	free(pro);
}