#include <iostream>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h> 

using namespace std;
class KernelSemaphore{
    string name;
    sem_t* pointer;
public:

    KernelSemaphore (string _name, int _init_value):name(_name){
    	pointer = sem_open(name.c_str(),O_CREAT,S_IRUSR,_init_value);
    };
    void P(){
    	sem_wait(pointer);
    }
    void V(){
    	sem_post(pointer);
    }
    ~KernelSemaphore (){
    	sem_unlink(name.c_str());
    }
};
