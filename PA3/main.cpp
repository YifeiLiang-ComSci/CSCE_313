#include "Helper.hpp"
#include <chrono>
#include <ctime>
using namespace std;
char cwd[1024];
vector<long long int>backgrounds;


static void prompt() {
    //stackoverflow
    char text[255];
    FILE *name;
    name = popen("whoami", "r");
    fgets(text, sizeof(text), name);
   // cout << "Name is : " << text;
    pclose(name);
   // cout << endl;
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end-start;
    time_t end_time = std::chrono::system_clock::to_time_t(end);

    chdir(cwd);
    
    getcwd(cwd,sizeof(cwd));

    cout << cwd<<"-"<<ctime(&end_time)<<"-"<<name<<"-";

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
        prompt();
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

