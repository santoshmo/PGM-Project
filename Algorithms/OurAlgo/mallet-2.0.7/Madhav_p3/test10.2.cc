#include <iostream>
#include "vm_app.h"

using namespace std;

//Test of syslog. 
//1) Message length is more than 2 page's worth of bytes and 
//   this pager only has two physical pages.
//2) Message begins before valid portion of arena starts.
//3) Message ends after valid portion of arena ends

int main()
{
    char *p;
    p = (char *) vm_extend(0);
    p[0] = 'a';

		vm_syslog(p, 1);
	/*char* q;
	q = (char *) vm_extend(0);
	for(int i = 0; i < 4096; i++)
		q[i] = 'a';

	char* r;
	r = (char *) vm_extend(0);
	r[0] = 'a';
    vm_syslog(p + 4095, 4098);

	vm_syslog(p - 5, 4098); 	//invalid portion of arena (starts before)

	vm_syslog(r + 5000, 2000);	//invalid portion of arena (ends after)*/
}
