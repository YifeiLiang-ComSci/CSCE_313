#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "Helper.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>


using namespace std;

static void redirect(std::string &inputline) {
	int temp1 = (int)inputline.find("<");
	int temp2 = (int)inputline.find(">");
	int min;
	if(temp1 == -1){
		min = temp2;
	} else if(temp2 == -1){
		min = temp1;
	} else {
		min = (temp1 < temp2) ? temp1:temp2;
	}


    if(inputline.find("<") != string::npos){
        int index = (int)inputline.find("<");
        //                    int index1 = (int) inputline.find(" ", index + 1);
        //                    int length = index1 - index + 1;
        string filename = inputline.substr(index + 1,inputline.length() - index);
        
        // cout<<"length"<<length<<endl;
        filename = trim(filename);
        if(filename.find(" ")!=string::npos){
        	filename = filename.substr(0, filename.find(" "));
        }
        
        int fd = open(filename.c_str(),O_RDONLY,S_IRUSR);

        dup2(fd,0);
        close(fd);
        // inputline = inputline.substr(0,index);
        // char** command = parseInput((char*)inputline.c_str(), sizeof(inputline));
        
        // execvp(command[0], command);
        
        
        
    }
     if(inputline.find(">")!=string::npos){
        
        int index = (int)inputline.find(">");
        //                    int index1 = (int) inputline.find(" ", index + 1);
        //                    int length = index1 - index + 1;
        string filename = inputline.substr(index + 1,inputline.length() - index);
        filename = trim(filename);
        if(filename.find(" ")!=string::npos){
        	filename = filename.substr(0, filename.find(" "));
        }
        int fd = open(filename.c_str(),O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);


        dup2(fd,1);
        close(fd);

        //inputline = inputline.substr(0,index);
        // char** command = parseInput((char*)inputline.c_str(), sizeof(inputline));
        // execvp(command[0], command);
        
    } 

   	inputline = inputline.substr(0,min);
   	inputline = trim(inputline);

    char** command = parseInput((char*)inputline.c_str(), sizeof(inputline));
    execvp(command[0], command);
}
void execute(string inputline){
     if(redirectCheck(inputline)){
                 redirect(inputline);
            }else {
                char** command = parseInput((char*)inputline.c_str(),inputline.length());
                execvp(command[0],command);
             }
}
int main(){
    while(true){


        cout << "My Shell$ ";
        string inputline;
        
        getline(cin,inputline);

        if(inputline == string("exit")){
            cout <<"Bye!! end of shell"<<endl;
            break;
        }
         vector<string> process = split(inputline,"|");
        int numProcess = (int)process.size();
        if(numProcess == 1){
        int pid = fork();
        if(pid == 0){
        	if(redirectCheck(inputline)){
        		redirect(inputline);
        	}else {
        		cout<<"run"<<endl;
                char** command = parseInput((char*)inputline.c_str(), sizeof(inputline));
                execvp(command[0], command);
            }
        } else {
        	wait(0);
        }
        cout<<"inputline: "<<inputline<<endl;
        vector<string> process = split(inputline,"|");
        int numProcess = (int)process.size();
    }



        for(int i = 0; i < process.size(); i++){
            int fd[2];
            pipe(fd);
            if(!fork()){
                if(i < process.size() - 1){
                    dup2(fd[1], 1);
                    close(fd[1]);
                }
                close(fd[0]);
                if(fork() == 0){
                execute(process[i]);
            } else {
            	continue;
            }

            } else {
                if(i == process.size() - 1)
                    wait(0);
                dup2(fd[0],0);
                close(fd[0]);
                close(fd[1]);

            }
        }
            
       
//        if(pid == 0){
//            // char* args[] = {(char*) inputline.c_str(),NULL};
//            // execvp(args[0],args);
//
//        } else {
//            wait(0);
//        }
   // }
}
}
