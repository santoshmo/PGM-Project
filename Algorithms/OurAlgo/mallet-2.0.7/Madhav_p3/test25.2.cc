#include <iostream>
#include "vm_app.h"
using namespace std;

int main()
{

   char *p1;

   p1 = (char *) vm_extend(0);
   char *p2;

   p2 = (char *) vm_extend(0);

   p2[0] = 'a';
   vm_syslog(p1 + 4095, 2);
}
