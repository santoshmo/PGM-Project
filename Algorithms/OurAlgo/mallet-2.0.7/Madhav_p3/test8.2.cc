#include <iostream>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

//Test of clock queue and updates between different processes/share ids


int main()
{	
	int pid = fork();

	if(pid == 0) //child
	{
		cout << "child" << endl;
		char* child;
		child = (char*) vm_extend(0);
		child[20] = 'b';				//second page in physmem is evicted for this
		
		char* child1;
		child1 = (char*) vm_extend(1);
		child1[0] = 'p';				//this should still by in physmem
		
		vm_syslog(child + 4095, 2);
		
		char* child2;
		child2 = (char*) vm_extend(0);
		child2[80] = 'c';				//this will evict page for child
		
		char* child3;
		child3 = (char*) vm_extend(0);
		child3[13] = 'd';				//this will evict page with share id = 1
	}
	
	else //parent
	{
		cout << "parent" << endl;
		char* parent;
		parent = (char*) vm_extend(0);
		parent[4095] = 'x';				//this is first page in physmem
		
		char* parent1;
		parent1 = (char*) vm_extend(1);
		parent1[100] = 'y';				//this is second page in physmem
		
		parent[16] = 'a';				//first page being used again
		
		vm_yield();
		
		parent1[300] = 'z';				//this should no longer be in physmem, will have to read from disk	
	}
	
	
}
