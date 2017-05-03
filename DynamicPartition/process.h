#ifndef _PROCESS_H_
#define _PROCESS_H_

struct Process_ {
	char *pro_name;
	int pro_size;
	int pro_addr;
};
typedef struct Process_ Process;

extern Process *new_process(char *name, int size, int addr);

extern char *get_pro_name(Process *pro);

extern int get_pro_size(Process  *pro);

extern int get_pro_addr(Process *pro);

extern void set_pro_size(Process *p, int size);

extern void set_pro_addr(Process *p, int addr);

extern void destroy_process(Process *pro);

#endif // !_PROCESS_H_
