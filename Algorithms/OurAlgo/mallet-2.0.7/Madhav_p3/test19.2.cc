#include "vm_app.h"


int main()
{
	char* p;
	unsigned page_num = 0;
	while(p = (char*)vm_extend(0))
	{
		p[page_num * 4096 + 5] = 'a';
	}
	vm_syslog((void*)0x60001000, 8);
}