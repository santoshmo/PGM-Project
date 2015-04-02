#include "vm_app.h"

//tests reading and writing when a page must be evicted. Also tests clock algorithm to 
//make sure p is evicted (rather than q) before r is brought into physmem

int main()
{
	char *p;
    p = (char *) vm_extend(0);
    p[4095] = 'a';
	
	char* q;
	q = (char *) vm_extend(0);
	q[0] = 'b';
	
	char* r;
	r = (char*) vm_extend(0);
	r[0] = 'c';
	
	p[4095] = 'd';


}