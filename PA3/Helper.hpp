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
#include <string.h>
#include <stdio.h>
#include <vector>


#endif /* Helper_hpp */
using namespace std;

struct Command{
    string cmd;
};
const std::string WHITESPACE = " \n\r\t\f\v";
//https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
std::string ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s)
{
    return rtrim(ltrim(s));
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
        trim(curr);
        commands.push_back(curr);
        curr=strtok(NULL,separator.c_str());
        
    }
    return commands;
}
// vector<string> split (string line, string separator=" "){
//     vector<string> result;
//     while (line.size()){
//         if (line.find("'")==0 && separator==" "){
//             // cout << "Sicong Found" << endl;
//             size_t found = (line.substr(1)).find("'")+1;
//             if (found == string::npos){
//                 cout << "last part" << endl;
//                 string lastpart = line;
//                 if (lastpart.size()>0){
//                     result.push_back(lastpart);
//                 }
//                 break;
//             }
//             string segment = (line.substr(0,found+1));
//             // cout << "line: " << line << "found: " << found << endl;
//             line = line.substr (found+1);
//             // cout << "[" << segment << "]"<< endl;
//             if (segment.size() != 0) 
//                 result.push_back (segment);
//             // cout << line << endl;
//         }else if (line.find("\"")==0 && separator==" "){
//             // cout << "Sicong Found" << endl;
//             size_t found = (line.substr(1)).find("\"")+1;
//             if (found == string::npos){
//                 cout << "last part" << endl;
//                 string lastpart = line;
//                 if (lastpart.size()>0){
//                     result.push_back(lastpart);
//                 }
//                 break;
//             }
//             string segment = (line.substr(0,found+1));
//             // cout << "line: " << line << "found: " << found << endl;
//             line = line.substr (found+1);
//             // cout << "[" << segment << "]"<< endl;
//             if (segment.size() != 0) 
//                 result.push_back (segment);
//             // cout << line << endl;
//         }else {
//             size_t found = line.find(separator);
//             if (found == string::npos){
//                 string lastpart = trim (line);
//                 if (lastpart.size()>0){
//                     result.push_back(lastpart);
//                 }
//                 break;
//             }
//             string segment = trim (line.substr(0, found));
//             // cout << "line: " << line << "found: " << found << endl;
//             line = line.substr (found+1);

//             // cout << "[" << segment << "]"<< endl;
//             if (segment.size() != 0) 
//                 result.push_back (segment);

//             // cout << line << endl;
//         }
//     }
//     return result;
// }

