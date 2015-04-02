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
    p = (char *) vm_extend(1);
    p[4095] = 'a';

	char* q;
	q = (char *) vm_extend(1);
	for(int i = 0; i < 4096; i++)
		q[i] = 'a';

	char* r;
	r = (char *) vm_extend(0);
	r[0] = 'a';
    vm_syslog(p + 4095, 4096);
}
