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
		char* child;
		child = (char*) vm_extend(0);
		
		char* child1;
		child1 = (char*) vm_extend(0);
		child[1] = 'a';
		
		vm_yield();
		
	}
	else //parent
	{
		cout << "child" << endl;
		char* parent;
		parent = (char*) vm_extend(0);
		parent[1] = 'b';
		char* parent1;
		parent1 = (char*) vm_extend(0);
		parent1[0] = 'p';
		
		vm_syslog(parent + 4095, 2);
	}
}
