#include<vector>
#include"Thread.h"
using namespace std;

#ifndef SIMULATOR_H
#define SIMULATOR_H


class Simulator{
public:
    Simulator();		// Initialize list of ready threads 
    ~Simulator();		// De-allocate ready list

    void ReadyToRun(Thread* thread);	
    				// Thread can be dispatched.
    Thread* FindNextToRun();	// Dequeue first thread on the ready 
				// list, if any, and return thread.
    void Run(Thread* nextThread, bool finishing);
    				// Cause nextThread to start running
    void CheckToBeDestroyed();// Check if thread that had been
    				// running needs to be deleted
	void Start();

	Thread* currentThread;
private:  
	vector<Thread*> readyList;		
	Thread *toBeDestroyed;	
};

extern Simulator* simulator; 

#endif //SIMULATOR_H