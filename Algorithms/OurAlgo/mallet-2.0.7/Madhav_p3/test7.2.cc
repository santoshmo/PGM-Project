#include <iostream>
#include <climits>
#include <unistd.h>
#include "vm_app.h"

using namespace std;


int main()
{
	int pid = fork();
	if(pid == 0) //child
	{
		cout << "child" << endl;
		char* child;
		child = (char*) vm_extend(1);
		
		char* child1;
		child1 = (char*) vm_extend(1);
		
		vm_yield();

		char* child2, *child3;
		child2 = (char*) vm_extend(0);
		child3 = (char*) vm_extend(0);
		
	}
	else //parent
	{
		cout << "parent" << endl;
		char* parent;
		parent = (char*) vm_extend(1);
		
		char* parent1;
		parent1 = (char*) vm_extend(1);
		parent1[0] = 'p';
		
		vm_syslog(parent + 4095, 2);

		int pid2 = fork();
		if(pid2 == 0) //child
		{
			cout << "child" << endl;
			char* child;
			child = (char*) vm_extend(1);
			
			char* child1;
			child1 = (char*) vm_extend(1);
			
			vm_yield();
			
		}
		else //parent
		{
			cout << "parent" << endl;
			char* parent;
			parent = (char*) vm_extend(1);
			
			char* parent1;
			parent1 = (char*) vm_extend(1);
			parent1[0] = 'p';
			
			vm_syslog(parent + 4095, 2);

			
		}
	}

}
