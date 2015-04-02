#include <iostream>
#include <climits>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

//Test of syslog. 
//1) Message length is more than 2 page's worth of bytes and 
//   this pager only has two physical pages.
//2) Message begins before valid portion of arena starts.
//3) Message ends after valid portion of arena ends

int main()
{
    char *p, *q, *r, *s, *t, *u , *v;
    p = (char *) vm_extend(0);
    q = (char *) vm_extend(1);
    char x = q[0];

    vm_syslog(p + 4095, 2);

    int pid = fork();
    if(pid != 0) {
    	vm_yield();

    	u = (char *) vm_extend(2);
    	char z = u[1];
    	vm_syslog(p + 4095, 2);

    	v = (char *) vm_extend(0);
    	v[0] = 'd';
    	vm_syslog(p + 4095, 2);
    	vm_yield();
    }
    else{
    	r = (char *) vm_extend(1);
    	r[1] = 'a';
    	vm_syslog(r + 4095, 2);

    	s = (char *) vm_extend(2);
    	char y = s[0];

		vm_syslog(r + 4095, 2);
    	t = (char *) vm_extend(0);
    	t[0] = 'b';

    	vm_syslog(r + 4095, 4098);
    	vm_yield();
    }
}
