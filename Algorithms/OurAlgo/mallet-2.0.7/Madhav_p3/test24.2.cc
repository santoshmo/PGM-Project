#include <iostream>
#include "vm_app.h"
#include <algorithm>
#include <unistd.h>
using namespace std;

int main()
{

   
   char *block0 = (char *) vm_extend(0);
   char *block1 = (char *) vm_extend(0);
  char *block2 = (char *) vm_extend(0);



//Assuming 2 ppages available

   //FAULT read (log zero)
   char x = block0[0];
   //Read
   //cout << "block0: " << block0[0] << endl;
   //FAULT write
   block1[0] = 'b';
   //FAULT write, page out block 0, but does not send block 0 to disk (log zero)
   block2[0] = 'c';
 
   //Read from block 1, keep ref bit updated, clock points to block 2
   x = block1[0];
   //FAULT read, page out block 2 | DISK WRITE block 2
   x = block0[0];
   //FAULT write & DISK READ block 2 | DISK WRITE & page out block 1
    block2[0] = 'c';
   //FAULT write & DISK READ block 1
   block1[0] = 'b';
   //FAULT write block 0 | DISK WRITE block2
   block0[0] = 'a';
   //FAULT read block | DISK WRITE block0 | DISK READ block2
   cout << block1[0] << block2[0] << endl;
  }