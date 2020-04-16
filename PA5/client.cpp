#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"
#include <time.h>
#include <thread>
#include <stdio.h>
#include "MQreqchannel.h"

#include "Reqchannel.h"
using namespace std;


RequestChannel* create_new_channel(RequestChannel* mainChan,string ival,int mb){
    char name[1024];
    MESSAGE_TYPE m = NEWCHANNEL_MSG;
    mainChan->cwrite(&m, sizeof(m));
  //  cout<<"Sent a new channme mesg"<<endl;
    mainChan->cread(name,1024);
      //cout<<"waotomg a new channme mesg"<<endl;
    RequestChannel* newchan = 0;
    if(ival == "f"){
        newchan= new FIFORequestChannel(name,RequestChannel::CLIENT_SIDE);
    }
    else if(ival == "q"){
        newchan= new MQRequestChannel(name,RequestChannel::CLIENT_SIDE,mb);
    }
    return newchan;
}
void patient_thread_function(int n,int pno, BoundedBuffer* request_buffer){
    /* What will the patient threads do? */
    datamsg d(pno, 0.0, 1);
    double resp = 0;
    for(int i = 0; i < n;i++){
        // chan->cwrite(&d, sizeof(datamsg));
        // chan->cread(&resp, sizeof(double));
       
        // hc->update(pno,resp);
        request_buffer->push((char*)&d, sizeof(datamsg));
         d.seconds += 0.004 ;
    }
    
}

void worker_thread_function(RequestChannel* chan,BoundedBuffer* request_buffer,HistogramCollection* hc,int mb){
    /*
		Functionality of the worker threads	
    */
    char buf[1024];
    double resp=0;
    char recevbuf[mb];
    while(true){
        request_buffer->pop(buf,1024);
        MESSAGE_TYPE* m = (MESSAGE_TYPE*) buf;


        if(*m == DATA_MSG){
            cout<<"run"<<endl;
            chan->cwrite(buf,sizeof(datamsg));
            chan->cread(&resp,sizeof(double));
            hc->update(((datamsg*)buf)->person,resp);
        } else if(*m == FILE_MSG){
            filemsg* fm = (filemsg*) buf;
            string fname = (char*)(fm + 1);
            int sz = sizeof(filemsg) + fname.size()+1;

            chan->cwrite(buf,sz);
            chan->cread(recevbuf,mb);
            string recvfname = "recv/" + fname;
            FILE* fp = fopen(recvfname.c_str(),"r+");
            fseek(fp,fm->offset, SEEK_SET);
            fwrite(recevbuf,1,fm->length,fp);
            fclose(fp);
        }else{

            chan->cwrite(m, sizeof(MESSAGE_TYPE));

            delete chan;
            break;
        }
    }
}

void file_thread_function(string fname, BoundedBuffer* request_buffer,RequestChannel * chan, int m){
    string recvfname = "recv/" + fname;
    //FILE* fp = fopen(recvfname.c_str(), "w");
    char buf[1024];
    filemsg f(0,0);
    memcpy(buf,&f,sizeof(f));
    strcpy(buf + sizeof(f),fname.c_str());
    chan->cwrite(buf, sizeof(f) + fname.size() + 1);
    __int64_t filelength;
    chan->cread(&filelength,sizeof(filelength));

    FILE*  fp= fopen(recvfname.c_str(),"w");
    fseek(fp,filelength,SEEK_SET);
    fclose(fp);

    filemsg* fm = (filemsg*) buf;
    __int64_t remlen = filelength;

    while(remlen > 0){
        fm->length = min(remlen,(__int64_t) m);
        request_buffer->push(buf,sizeof(filemsg)+fname.size()+1);
        fm->offset += fm->length;
        remlen -=fm->length;
    }

}





int main(int argc, char *argv[])
{
    int n = 15000;    //default number of requests per "patient"
    int p = 1;     // number of patients [1,15]
    int w = 200;    //default number of worker threads
    int b = 500; 	// default capacity of the request buffer, you should change this default
	int m = MAX_MESSAGE; 	// default capacity of the message buffer
    srand(time_t(NULL));
    string fname = "10.csv";
    int opt = -1;
    bool filetransfer = false;
    string ival = "f";
    while((opt = getopt(argc,argv,"m:n:b:w:p:f:i:"))!= -1){
        switch(opt){
            case 'm':
                m = atoi(optarg);
                break;
            case 'n':
                n = atoi(optarg);
                break;
            case 'p':
                p = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg);
                break;
            case 'f':
                fname = optarg;
                filetransfer = true;
                break;
            case 'i':
                ival = optarg;
                break;

        }
    }
    int pid = fork();
    if (pid == 0){
		// modify this to pass along m
       // execl ("server", "server", (char *)NULL);
         execl("server","server","-m",to_string(m).c_str(),"-i",ival.c_str(),(char *)NULL);
    }
    
	RequestChannel* chan;
    if(ival == "f")
        chan = new FIFORequestChannel("control", RequestChannel::CLIENT_SIDE);
    else if(ival == "q")
        chan = new MQRequestChannel("control", RequestChannel::CLIENT_SIDE,m);
    BoundedBuffer request_buffer(b);
	HistogramCollection hc;

    //making histogram  and adding to the histogram collection hc

    for(int i = 0; i < p; i++){
        Histogram* h = new Histogram(10, -2.0, 2.0 );
        hc.add(h);
    }
	
	//make worker channels
    RequestChannel* wchans[w];
    for(int i = 0; i < w ;i++){
        wchans[i] = create_new_channel(chan, ival,m);
    }
	

    struct timeval start, end;
    gettimeofday (&start, 0);

    /* Start all threads here */
	thread patient[p];
    thread filethread[1];
    if(filetransfer){
    filethread[0] = thread(file_thread_function,fname,&request_buffer,chan,m);
    } else {
     
    for (int i = 0; i < p; i++){
        patient[i] = thread(patient_thread_function,n,i+1,&request_buffer);
    }
    }


    thread workers[w];
    for(int i = 0; i < w; i++){
        workers[i] = thread(worker_thread_function, wchans[i],&request_buffer,&hc , m);
    }



	/* Join all threads here */
    // for(int i = 0; i < p; i++){
    //     patient[i].join();
    // }
    if(filetransfer){
        filethread[0].join();
    } else{
        for(int i = 0; i < p; i++){
         patient[i].join();
     }
    }
    cout<<"Patient or file finished"<<endl;
    for(int i = 0; i < w; i++){
        cout<<"quit in for loop"<<endl;
       MESSAGE_TYPE q = QUIT_MSG;
       request_buffer.push((char*)&q,sizeof(q));
    }
    cout<<"run"<<endl;
    for(int i = 0; i < w; i++){
        cout<<"in join waiting"<<endl;
        workers[i].join();
    }
    cout<<"run end"<<endl;
    gettimeofday (&end, 0);
    // print the results
    if(!filetransfer)
	   hc.print ();
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micro seconds" << endl;

    // clean up worker channels

    // for(int i = 0 ;i < p; i++){
    //     MESSAGE_TYPE q = QUIT_MSG;
    //     wchans[i]->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    //     //cout << "All Done!!!" << endl;
    //     delete wchans[i];

    // }
    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;

    return 0;
    
}
