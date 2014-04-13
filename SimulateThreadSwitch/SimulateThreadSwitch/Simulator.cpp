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
    
    oldThread->CheckOverflow();		   

	// switch to the next thread
    this->currentThread = nextThread;  
  
    
	SWITCH(oldThread, nextThread);

    // 这里是oldThread在运行
	
    CheckToBeDestroyed();		
}

//检测有没有线程需要销毁
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

