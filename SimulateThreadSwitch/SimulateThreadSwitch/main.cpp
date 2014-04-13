#include"Simulator.h"
#include"Thread.h"
#include<iostream>
using namespace std;

Simulator* simulator;

//线程要执行的函数
void ThreadTest(int which){
    for(int i = 0; i < which; i++) {
		std::cout << "*** thread " << simulator->currentThread->getName() << " looped " << i << " times\n";
		simulator->currentThread->yield();
    }
}

int main(int argc, char* argv[]){

	simulator = new Simulator;

	Thread* t;
	
	for(int i=0; i<5; i++){
		char* threadName = new char[30];
		sprintf(threadName, "%s%d", "Test Thread ", i);
		t = new Thread(threadName);
		t->Fork((VoidFunctionPtr)ThreadTest, (void*)(i + 2));
	}
	simulator->Start();
	return 0;
}