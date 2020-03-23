#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <algorithm>
#include <regex>

using namespace std;

// trim from start(From stack overflow)
std::string ltrim( std::string str ) {
    return std::regex_replace( str, std::regex("^\\s+"), std::string("") );
}

std::string rtrim( std::string str ) {
    return std::regex_replace( str, std::regex("\\s+$"), std::string("") );
}

std::string trim( std::string str ) {
    return ltrim( rtrim( str ) );
}
char** parseInput(char *line,int size){
    char **command = (char**)malloc(size * sizeof(char *));
    char* parsed = strtok(line, " ");
    int index = 0;
    while(parsed != NULL){
        command[index] = parsed;
        index++;
        parsed = strtok(NULL, " ");
    }
    command[index] = parsed;
    return command;
}
bool redirectCheck(string input){
    return (input.find("<")!=string::npos or input.find(">")!=string::npos);
}
//https://stackoverflow.com/questions/16286095/similar-function-to-javas-string-split-in-c
vector<string> split(string str,string separator){
    char* c_str=const_cast<char*>(str.c_str());
    char* curr;
    vector<string> commands;
    curr = strtok(c_str, separator.c_str());
    while(curr!=NULL){
        
        commands.push_back(curr);
        curr=strtok(NULL,separator.c_str());
        
    }
    for(int i = 0 ; i < commands.size();i++){
        commands[i] = trim(commands[i]);
    }
    return commands;
}
static string redirect(std::string &inputline) {
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
        string filename = inputline.substr(index + 1,inputline.length() - index);
        
        // cout<<"length"<<length<<endl;
        filename = trim(filename);
        if(filename.find(" ")!=string::npos){
            filename = filename.substr(0, filename.find(" "));
        }
        
        int fd = open(filename.c_str(),O_RDONLY,S_IRUSR);

        dup2(fd,0);
        close(fd);
        inputline = inputline.substr(0,index);
        return inputline;

        
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

    }

    inputline = inputline.substr(0,min);
    inputline = trim(inputline);
    return inputline;

}
