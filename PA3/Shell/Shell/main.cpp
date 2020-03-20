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
char** vec_to_char_array (vector<string> parts){
    char ** result = new char * [parts.size() + 1]; // add 1 for the NULL at the end
    for (int i=0; i<parts.size(); i++){
        // allocate a big enough string
        result [i] = new char [parts [i].size() + 1]; // add 1 for the NULL byte
        strcpy (result [i], parts[i].c_str());
    }
    result [parts.size()] = NULL;
    return result;
}

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
void execute(string command){
     if(redirectCheck(inputline)){
                 redirect(inputline);
            }else {
                char** command = parseInput((char*)inputline.c_str(),inputline.length());
                execvp(command[0],command);
             }
	// bool mi = false;
 //    vector<string> argstrings = split (command, " "); // split the command into space-separated parts
 //    int i = 0;
 //    while(argstrings.size()>i){
 //        if (argstrings[i].find("\"")==string::npos && argstrings[i].find("'")==string::npos ){
 //            if (argstrings[i].find(">")!=string::npos){ // redirect output case
 //            if (argstrings[i]==">"){
 //                argstrings.erase(argstrings.begin()+i);
 //            }else{cout << "special case to be handled" << endl;}

 //            int write_to = open(argstrings[i].c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IWUSR);
 //            dup2(write_to,1);
 //            close(write_to);
 //            argstrings.erase(argstrings.begin()+i);
 //            }else if (argstrings[i].find("<")!=string::npos){
 //                if (argstrings[i]=="<"){
 //                    argstrings.erase(argstrings.begin()+i);
 //                }else{cout << "special case to be handled" << endl;}
 //                int read_from = open(argstrings[i].c_str(), O_RDONLY,S_IRUSR);
 //                dup2(read_from,0);
 //                close(read_from);
 //                argstrings.erase(argstrings.begin()+i);
 //            }else {
 //                i++;
 //            }
 //        }else{
 //            i++;
 //        }
 //    }
 //    for (int i=0; i<argstrings.size();i++){
 //        argstrings[i]=trim(argstrings[i]);
 //    }
 //    if (argstrings[0]=="cd"){
 //        char cwd[256];
 //        if (argstrings[1] == "-"){
 //            argstrings[1] = "..";
 //            mi = true;
 //        }
 //        char** args = vec_to_char_array (argstrings);// convert vec<string> into an array of char*
 //        chdir(args[1]);
 //        if (mi){
 //            getcwd(cwd,sizeof(cwd));
 //            cout << cwd << endl;
 //        }
 //        return;
 //    }else {
 //        char** args = vec_to_char_array (argstrings);// convert vec<string> into an array of char*
 //        execvp (args[0], args);
 //    }
}
int main(){
    while(true){
    	  int stdin = dup(0);
        	int stdout = dup(1);
    	vector<int>backgrounds;

        cout << "My Shell$ ";
        string inputline;
        bool background = false;
        getline(cin,inputline);
        if(inputline.find('&') != string::npos){
        	background = true;
        }

        if(inputline == string("exit")){
            cout <<"Bye!! end of shell"<<endl;
            break;
        }
         vector<string> tparts = split(inputline,"|");
        int numProcess = (int)tparts.size();
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
    }

       
    


if(numProcess > 1){
	for (int i=0;  i < tparts.size(); i++){
            // make pipe
            int fd[2];
          
            pipe(fd);
            pid_t pid = fork();
			if (!pid){
                close (fd[0]);
                // redirect output to the next level
                // unless this is the last level
                if (i < tparts.size() - 1){
                    dup2(fd[1],1);// redirect STDOUT to fd[1], so that it can write to the other side
                    close (fd[1]);// STDOUT already points fd[1], which can be closed
                }
                //execute function that can split the command by spaces to 
                // find out all the arguments, see the definition
                execute (tparts [i]); // this is where you execute
            }else{
                if (!background){
                    waitpid(pid,NULL,0);            // wait for the child process // waitpid(pid,0,WNOHANG); works too
                    background = false;
                }else{
                    backgrounds.push_back(pid);
                    background = false;
                }
				// then do other redirects
                dup2(fd[0],0);
                close(fd[1]);
                close(fd[0]);
            }
        }
        dup2(stdin,0);
        dup2(stdout,1);
        close(stdin);
        close(stdout);
        // for(int i = 0; i < process.size(); i++){
        //     int fd[2];
        //     pipe(fd);
        //     if(!fork()){
        //         if(i < process.size() - 1){asd
        //             dup2(fd[1], 1);
        //             close(fd[1]);
        //         }
        //         close(fd[0]);
              
        //         execute(process[i]);

        //     } else {
        //         if(i == process.size() - 1)
        //              wait(0);
        //         dup2(fd[0],0);
        //         close(fd[0]);
        //         close(fd[1]);

        //     }
        // }
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
