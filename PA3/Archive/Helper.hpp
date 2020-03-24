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
