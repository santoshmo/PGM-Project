#include <iostream>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

//Test of syslog.
//1) Read message from different processes.

int main()
{	
	int pid = fork();
	if(pid != 0) //child
	{
		cout << "parent" << endl;
		char* parent1;
		parent1 = (char*) vm_extend(1);
		parent1[4095] = 'a';

		char* parent2;
		parent2 = (char*) vm_extend(2);
		parent2[0] = 'b';

		vm_yield();

	}
	else //parent
	{
		cout << "child" << endl;
		char* child1;
		child1 = (char*) vm_extend(0);
		child1[4095] = 'c';

		char* child2;
		child2 = (char*) vm_extend(1);
		child2[0] = 'd';

		vm_syslog(child1 + 4095, 2);
	}
}