#include "Helper.hpp"
#include <chrono>
#include <ctime>
using namespace std;
char cwd[1024];
char pwd[1024];
vector<long long int>backgrounds;
vector<string> status;
vector<string> line;



static void prompt() {
    //stackoverflow
    char text[255];
    FILE *name;
    name = popen("whoami", "r");
    fgets(text, sizeof(text), name);

    string namestr(text);
    namestr.erase(std::remove(namestr.begin(), namestr.end(), '\n'),
            namestr.end());
   // cout << endl;
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end-start;
    time_t end_time = std::chrono::system_clock::to_time_t(end);

    chdir(cwd);
    string time = ctime(&end_time);
    time.erase(std::remove(time.begin(), time.end(), '\n'),
            time.end());
    getcwd(cwd,sizeof(cwd));

    cout << cwd<<"-"<<time<<"-"<<namestr<<"-";
pclose(name);
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
        cout<<pwd<<endl;
        path = pwd;
    }
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
    strncpy(pwd,cwd,sizeof(cwd));
    
    while(true){
        for(int i = 0; i < backgrounds.size();i++){
            int state;
            waitpid(backgrounds[i],&state, WNOHANG);
            if (WIFEXITED(state)){
                status[i] = "Done";

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
            bool run = false;
            for(int j = 0; j < process.size();j++){
                        if(status[j] != "Collected"){
                            run = true;
                            cout<<"["<<j+1<<"]"<<" "<<status[j]<<"\t"<<line[j]<<endl;
                            if(status[j] == "Done"){
                                status[j] = "Collected";
                            }
                        }
                    }
                    if(run == false){
                        backgrounds.clear();
                        status.clear();
                        line.clear();
                    }
            continue;
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
                    status.push_back("Running");
                    line.push_back(inputline);
                    for(int j = 0; j < process.size();j++){
                        if(status[j] != "Collected"){
                            cout<<"["<<j+1<<"]"<<" "<<status[j]<<"\t"<<line[i]<<endl;
                            if(status[j] == "Done"){
                                status[j] = "Collected";
                            }
                        }
                    }

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

