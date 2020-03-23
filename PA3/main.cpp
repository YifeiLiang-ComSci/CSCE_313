#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include "Helper.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <algorithm>

using namespace std;
char cwd[1024];
vector<long long int>backgrounds;


int main(){
    getcwd(cwd,sizeof(cwd));
    
    while(true){
        for(int i = 0; i < backgrounds.size();i++){
            int status;
            waitpid(backgrounds[i],&status, WNOHANG);
            if (WIFEXITED(status)){
                backgrounds.erase(backgrounds.begin()+i);
                cout<<"background erased"<<endl;
            }
        }
        chdir(cwd);

        getcwd(cwd,sizeof(cwd));
        cout << cwd<<"-";
        int stdin = dup(0);
        int stdout = dup(1);


        cout << "My Shell$ ";
        string inputline;
        bool background = false;
        getline(cin,inputline);
        if(inputline.find('&') != string::npos){
        	background = true;
            cout<<"background: is true"<<endl;
            inputline = inputline.substr(0,inputline.find("&"));
        }


        if(inputline == string("exit")){
            cout <<"Bye!! end of shell"<<endl;
            break;
        }
        vector<string> process;
        if(((int)inputline.find("echo")) == 0){
            process.push_back(inputline);
        }else{
            process = split(inputline,"|");


        }

        if((int)inputline.find("jobs") == 0){
            cout<<"vector size: "<<backgrounds.size()<<endl;
            for(int i = 0; i < backgrounds.size();i++){
                cout<<"job "<<i+1<<": "<<backgrounds[i]<<endl;
            }
            continue;
        } else {
            cout<<"input is not job"<<inputline<<endl;
        }
        for(int i = 0; i < process.size(); i++){
            int fd[2];
            pipe(fd);
            int pid = fork();
            if(pid==0){
                if(i < process.size() - 1){
                    dup2(fd[1], 1);
                    close(fd[1]);
                }
                close(fd[0]);

                execute(process[i]);

            } else {
                if(background && i == process.size() - 1){
                    backgrounds.push_back(pid);

                }
                else if(i == process.size() - 1){
                   waitpid(pid,0,0); 

               }
               dup2(fd[0],0);
               close(fd[0]);
               close(fd[1]);

           }
       }
       dup2(stdin,0);
       dup2(stdout,1);
       close(stdin);
       close(stdout);
   }   


}

