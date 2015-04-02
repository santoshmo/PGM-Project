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
    p[5000] = 'a';
    char letter = p[5000];
}
