#include "vm_app.h"


int main()
{
	char *p;
    p = (char *) vm_extend(1);
    p[4095] = 'a';

	while(vm_extend(0));

	char* q;
	q = (char *) vm_extend(1);
	q[0] = 'b';


}
