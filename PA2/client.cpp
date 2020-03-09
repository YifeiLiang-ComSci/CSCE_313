/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include <sys/time.h>
#include "common.h"
#include "FIFOreqchannel.h"
#include <fstream>
#include <ctime> 

using namespace std;


int main(int argc, char *argv[]){
  
  cout<<"after main"<<endl;
  char *buffer = new char[256];
  int size = 256;
    int n = 100;    // default number of requests per "patient"
  int p = 15;   // number of patients
  int person = 0;
    double time = 0;
    int e = 0;
    int c;
    string strfilename;
    bool isDataPoint = false;
    bool fileReuqest = false;
    bool channelRequest = false;
    bool datapoint_Request = false;
    //cout<<"before get opt"<<endl;
    while ((c = getopt (argc, argv, "p:t:e:f:m:c")) != -1){
    switch (c)
      {
        case 'p':
          person = atoi(optarg);
          isDataPoint = true;

          break;
        case 't':
          time = stod(optarg);
          break;
        case 'e':
          e = atoi(optarg);
          datapoint_Request = true;

          break;
        case 'f':
          strfilename = optarg;
          fileReuqest = true;
          break;
        case 'm':
          size = stoi(optarg);
          buffer = new char[size];
          cout<<"Buffer size is "<<size<<endl;
          break;
        case 'c':
          channelRequest = true;



      }
  }
  if(channelRequest == true){
    isDataPoint = false;
    fileReuqest = false;
    datapoint_Request = false;
  }
   //cout<<"Before fork"<<endl;
  
  int pid_id = fork();
  if ( pid_id == 0 ) {
    string command  =to_string(size);

    char charCommand[command.length()+1];
    strcpy(charCommand,command.c_str());
    char *args[] = { "./server","-m", charCommand, NULL};
    execvp(args[0], args);
    // char *args[] = { "./server", NULL};
    // execvp(args[0], args);
  }
    srand(time_t(NULL));
   // cout<<"before channelRequest "<<endl;
    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
    
 
  if(datapoint_Request){
    cout<<"process_data_request"<<endl;
    datamsg data1(person,time ,e);
    chan.cwrite (&data1, sizeof (datamsg));
      
    double temp1;


    chan.cread ((char*)&temp1,sizeof(double));
    cout<<"data: "<<temp1<<endl;

  }
  else if(isDataPoint){
    cout<<"process_data_request"<<endl;
    ofstream myfile;
    // timeval tp_start, tp_end;
    // gettimeofday(&tp_start, 0); // start timer
    // double t1 = tp_start.tv_usec;
    auto start = std::chrono::system_clock::now();
    myfile.open ("received/x"+to_string(person)+".csv");

    // sending a non-sense message, you need to change this
    for(int i = 0; i < 15000;i++){
      //myfile << i * 0.004<<",";
      if(i == 15000 - 1){
        cout<<i * 0.004<<endl;
      }
      datamsg data1(person,i *0.004 ,1);
      chan.cwrite (&data1, sizeof (datamsg));
      
      double temp1;


      chan.cread ((char*)&temp1,sizeof(double));

      //myfile << temp1;

      datamsg data2(person,i *0.004 ,2);
      chan.cwrite (&data2, sizeof (datamsg));
      double temp2;
      chan.cread ((char*)&temp2,sizeof(double));
      myfile << (i * 0.004)<<","<<temp1<<","<< temp2<<"\n";

  }
myfile.close();
  // gettimeofday(&tp_end, 0);
  //   double t2 = tp_end.tv_usec;
auto end = std::chrono::system_clock::now();
std::chrono::duration<double> elapsed_seconds = end-start;
    cout <<"time(seconds):"<< elapsed_seconds.count() << "s\n";

}
if(fileReuqest){
  cout<<"Process File request: "<<endl;
//file request worked
    filemsg file(0,0);
    char filename[strfilename.length()+1];
    strcpy(filename,strfilename.c_str());
    char* temp = new char[sizeof(filemsg) +strfilename.length()+1];
    memcpy(temp, &file, sizeof(filemsg));

    memcpy(temp + sizeof(filemsg), &filename,strfilename.length()+1 );
    chan.cwrite (temp, sizeof (filemsg) +strfilename.length()+1);
    __int64_t length;
    chan.cread ((char*)&length,sizeof(__int64_t));



    int remaining = length;

    int numOfRequest = ceil(double(length) / size);
    // timeval tp_start, tp_end;
    // gettimeofday(&tp_start, 0); // start timer
    // long long int t1 = tp_start.tv_usec;
    auto start = std::chrono::system_clock::now();
    ofstream myfile("received/"+strfilename);
    cout<<size<<endl;
    delete[] temp;
    
    for (int i = 0; i < numOfRequest ; i++) {

      

      if (remaining >= size){

        filemsg fileT(i * size, size);

        remaining -= size;
        memcpy(buffer, &fileT, sizeof(filemsg));
        memcpy(buffer + sizeof(filemsg), filename, strfilename.length()+1 );
        chan.cwrite (buffer, sizeof (filemsg) + strfilename.length()+1);

        chan.cread (buffer, size);
       // string s = (string) buffer;
        myfile.write(buffer,size);

      } else {
        filemsg fileT(i * size, remaining);

          memcpy(buffer, &fileT, sizeof(filemsg));
          memcpy(buffer + sizeof(filemsg), filename, strfilename.length()+1 );
          chan.cwrite (buffer, sizeof (filemsg) + strfilename.length()+1);
          memset(buffer, 0x00, size );
          chan.cread (buffer, remaining);
          myfile.write(buffer,remaining);
      }


      


  }
  myfile.close();
  // gettimeofday(&tp_end, 0);
  //   long long int t2 = tp_end.tv_usec;
  auto end = std::chrono::system_clock::now();
std::chrono::duration<double> elapsed_seconds = end-start;
    cout <<"time(seconds):"<< elapsed_seconds.count() << "s\n";




}
//requesting new channel
if (channelRequest){
  MESSAGE_TYPE mtype = NEWCHANNEL_MSG;
  chan.cwrite(&mtype, sizeof(mtype));
  char channelName[30];
  chan.cread(&channelName,sizeof(channelName));
  cout<<channelName<<endl;
  FIFORequestChannel chan1 (channelName, FIFORequestChannel::CLIENT_SIDE);
  cout << "chan1 connected"<<endl;
  cout<<"Using new channel to requst data point from person "<<person<<" at time ("<<time<<")"<< " ecg1  ecg2"<<endl;
  datamsg e1(person,time ,1);
  chan1.cwrite(&e1,sizeof(filemsg));
  double re1;
  chan1.cread(&re1,sizeof(double));
  cout<<"Person "<<person<<" at "<<time<<" ecg1: "<< re1<<endl;
  datamsg e2(person,time ,2);
  chan1.cwrite(&e2,sizeof(filemsg));
  double re2;
  chan1.cread(&re2,sizeof(double));
  cout<<"Person "<<person<<" at "<<time<<" ecg2: "<< re2<<endl;
  MESSAGE_TYPE m = QUIT_MSG;
  chan1.cwrite (&m, sizeof (MESSAGE_TYPE));

}


    //cout << temp[0] << " " << temp[1] << endl;
    // closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
   chan.cwrite (&m, sizeof (MESSAGE_TYPE));
   wait(0);
    cout<<"client Terminated"<<endl;


}
