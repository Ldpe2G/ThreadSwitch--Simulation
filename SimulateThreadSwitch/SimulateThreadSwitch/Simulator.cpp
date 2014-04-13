#include "Simulator.h"
#include <assert.h>
#include <iostream>
using namespace std;

Simulator::Simulator(){ 
	Thread* t = new Thread("main Thread");
	currentThread = t;
    toBeDestroyed = NULL;
} 


Simulator::~Simulator(){ 
  	if(currentThread != NULL)
		delete currentThread;
} 

void Simulator::ReadyToRun (Thread *thread){
    readyList.push_back(thread);
}

Thread * Simulator::FindNextToRun (){
    if (readyList.empty()) {
		return NULL;
    } else {
		int n = rand() % readyList.size();
		Thread* randomThread = readyList[n];
		swap(readyList[n], readyList.back());
		readyList.pop_back();
    	return randomThread;
    }
}

void Simulator::Run (Thread *nextThread, bool finishing){
    Thread *oldThread = this->currentThread;
    
    if (finishing) {	// mark that we need to delete current thread
         assert(toBeDestroyed == NULL);
		 toBeDestroyed = oldThread;
    }
    
    oldThread->CheckOverflow();		    // check if the old thread
					    // had an undetected stack overflow

    this->currentThread = nextThread;  // switch to the next thread
  
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".	
	//if(oldThread != nextThread)		/* Liang DePeng + */
	SWITCH(oldThread, nextThread);
    // we're back, running oldThread
	
    CheckToBeDestroyed();		// check if thread we were running
					// before this one has finished
					// and needs to be cleaned up
}

void Simulator::CheckToBeDestroyed(){
    if (toBeDestroyed != NULL) {
        delete toBeDestroyed;
		toBeDestroyed = NULL;
    }
}
 

void Simulator::Start(){
	cout <<"simulation start!\n";
	while(!readyList.empty()){
		cout <<"now in "<< currentThread->getName() <<endl;
		currentThread->yield();
	}
	cout <<"simulation complete!\n";
}

