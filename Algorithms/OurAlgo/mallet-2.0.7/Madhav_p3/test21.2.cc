#include <iostream>
#include <climits>
#include <unistd.h>
#include "vm_app.h"

using namespace std;


int main()
{
	cout << "parent1" << endl;
	char* parent1;
	parent1 = (char*) vm_extend(0);
	
	char* parent2;
	parent2 = (char*) vm_extend(0);
	parent2[0] = 'p';
	
	vm_syslog(parent1 + 4095, 2);
	
	int pid2 = fork();
	if(pid2 != 0) //child
	{
		cout << "child1" << endl;
		char* child1;
		child1 = (char*) vm_extend(1);
		
		char* child2;
		child2 = (char*) vm_extend(1);
		
		vm_yield();
	
	}
	else //parent
	{
		cout << "child2" << endl;
		char* child3;
		child3 = (char*) vm_extend(1);
		
		char* child4;
		child4 = (char*) vm_extend(1);
		child4[0] = 'p';
			
		vm_syslog(child3 + 4095, 2);	
	}
}