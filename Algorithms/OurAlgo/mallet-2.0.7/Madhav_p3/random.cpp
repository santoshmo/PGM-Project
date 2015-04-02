#include "vm_app.h"
#include <iostream>

using namespace std;

int main()
{
	char* p;
	p = (char*) vm_extend(0);
	cout << "extended p! " << p << " and " << *p << endl;

//	cout << "NOTHING IS HAPPENING!!" << endl;

}
