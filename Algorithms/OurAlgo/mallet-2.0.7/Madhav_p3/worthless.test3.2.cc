#include <iostream>
#include "vm_app.h"

using namespace std;

//Test of extend. 
//Call extend then read from new data. (Should be initialized to 0.)

int main()
{
    char *p;
    p = (char *) vm_extend(0);
	char a = p[0];
	cout << "read " << a << " from new page!" << endl;
}
