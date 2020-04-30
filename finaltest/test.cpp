#include <iostream>
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <signal.h>
void signal_handler(int signo){
	printf("Got SIGUSR1\n");
}
int main(){
	signal(SIGUSR1,signal_handler);
	int pid = fork();
	if(pid==0){
		for(int i = 0 ; i < 5; i++){
			kill(getppid(),SIGUSR1);
			sleep(1);
		}
	} else {
		wait(0);
	}
}