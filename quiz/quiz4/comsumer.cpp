#include "KernelSemaphore.h"
int main (){
	KernelSemaphore ks ("/my_kernel_sema", 0);
    ks.P();
    cout << "I can tell the producer is done"<< endl;
}
