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
		cout << "child" << endl;
		char* child;
		child = (char*) vm_extend(0);
		
		char* child1;
		child1 = (char*) vm_extend(0);
		child1[1] = 'q';
		
	//	vm_yield();
		
	}
	else //parent
	{
		cout << "parent" << endl;
		char* parent;
		parent = (char*) vm_extend(0);
		
		char* parent1;
		parent1 = (char*) vm_extend(0);
		parent1[0] = 'p';
		
		vm_syslog(parent + 4095, 2);
	}
}
