#include <iostream>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include "Semaphore.h"
using namespace std;

/* This program now only works for NP=1 (i.e., 1 producer and NC consumers). 
 Modify your program such that when you change NP to >1,
 exactly NP producers run first, then exactly NC, and the whole thing
 should repeat in that order */
// to run the program
//g++ -g -w -std=c++11 Q5.cpp -lpthread
#define A 1
#define B 2
#define C 1

int buffer = 0;
Semaphore Cdone (A);
Semaphore Adone (0);
Semaphore Bdone (0);

Semaphore mtx (1); // will use as mutex
int ncdone = 0; // number of consumers done consuming
int npdone = 0;
int cdone =0;
// each producer gets an id, which is pno	
void producer_function (int pno){
	int count = 0; // each producer threads has its own count
	while (true){
		// do necessary wait
		Cdone.P();		// wait for the buffer to be empty

		// after wait is done, do the produce operation
		// you should not need to change this block
		mtx.P();
		buffer ++;
		cout << "A done"  << endl;
		mtx.V();


		// now do whatever that would indicate that the producers are done
		// in this case, the single producer is waking up all NC consumers
		// this will have to change when you have NP producers
        mtx.P();
        npdone ++;
        if (npdone == A) 
        {
        	npdone = 0;
            for (int i=0; i<B; i++)
                Adone.V(); 
            
        }
        mtx.V();
	}
}
// each consumer gets an id cno
void consumer_function (int cno){
	while (true){
		//do necessary wait
		Adone.P();

		// each consumer reads the buffer		
		// you should not need to change this block
		mtx.P();
		cout << "B Done" << endl;
		mtx.V();
		usleep (500000);

		// now do whatever necessary that would indicate that the consumers are all done
		mtx.P();
		ncdone ++;
		if (ncdone == B)
        { 
        	ncdone = 0; 
            for(int i = 0; i < C ; i++)
                Bdone.V(); 
			
		}
		mtx.V();
	}
}

void C_function (int cno){
	while (true){
		//do necessary wait
		Bdone.P();

		// each consumer reads the buffer		
		// you should not need to change this block
		mtx.P();
		cout << "C Done" << endl;
		mtx.V();
		usleep (500000);

		// now do whatever necessary that would indicate that the consumers are all done
		mtx.P();
		cdone ++;
		if (cdone == C)
        {
            for(int i = 0; i < A ; i++)
                Cdone.V(); 
			cdone = 0; 
		mtx.V();
	}
}
int main (){
	vector<thread> producers;
	vector<thread> consumers;
	vector<thread> Cthread;

	for (int i=0; i< 100; i++)
		producers.push_back (thread(producer_function, i+1));

	for (int i=0; i< 100; i++)
		consumers.push_back(thread(consumer_function, i+1));

	for (int i=0; i< 100; i++)
		Cthread.push_back(thread(C_function, i+1));
	
	for (int i=0; i<consumers.size (); i++)
		consumers [i].join();
	
	for (int i=0; i<producers.size (); i++)
		producers [i].join();
	for (int i=0; i<producers.size (); i++)
		Cthread[i].join();
	
}

