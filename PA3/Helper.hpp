//
//  Helper.hpp
//  Shell
//
//  Created by Yifei Liang on 3/13/20.
//  Copyright © 2020 Yifei Liang. All rights reserved.
//

#ifndef Helper_hpp
#define Helper_hpp
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <algorithm> 
#include <cctype>
#include <locale>


#endif /* Helper_hpp */
using namespace std;

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <regex>

// trim from start
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
void execute(string inputline){
    if(redirectCheck(inputline)){
       // string temp1 = inputline;//don't want to mess with pointer
       inputline = redirect(inputline);
   }


   inputline = trim(inputline);


   if((int)inputline.find("awk") == 0){
    int index1 = inputline.find("\"");
    int index2 = inputline.find("\'");
    int index = (index1 < index2)? index1 : index2;
    if(index1 == -1){
        index = index2;
    } else if(index2 == -1){
        index = index1;
    } else{
        index = (index1 < index2)? index1 : index2;
    }
    inputline = inputline.substr(index+1);
    inputline = inputline.substr(0,inputline.length()-1);
    inputline = trim(inputline);
    inputline = "awk " + inputline; 

    char** command = parseInput((char*)inputline.c_str(), sizeof(inputline));
    execvp(command[0],command);
} else if(int(inputline.find("echo") == 0)){
    int index1 = inputline.find("\"");
    int index2 = inputline.find("\'");
    int index = (index1 < index2)? index1 : index2;
    if(index1 == -1){
        index = index2;
    } else if(index2 == -1){
        index = index1;
    } else{
        index = (index1 < index2)? index1 : index2;
    }
    inputline = inputline.substr(index+1);
    inputline = inputline.substr(0,inputline.length()-1);
    inputline = trim(inputline);
    inputline = "echo " + inputline; 

    char** command = parseInput((char*)inputline.c_str(), sizeof(inputline));
    execvp(command[0],command);
} else if((int)(inputline.find("cd")) == 0 ){
    int index = inputline.find("cd");
    string path = inputline.substr(index + 2);
    path = trim(path);
    if(((int)path.find("-")) == 0){
        path = "..";
    } 
    char currentpath[512];
    getcwd(currentpath,sizeof(currentpath));
    string curr(currentpath);
    chdir(path.c_str());
    getcwd(cwd,sizeof(cwd));
    return;
}








else {
    string temp1= inputline;
    char** command = parseInput((char*)temp1.c_str(),inputline.length());
    execvp(command[0],command);
}


}