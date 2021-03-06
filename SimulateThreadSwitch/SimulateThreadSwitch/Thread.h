#ifndef THREAD_H
#define THREAD_H

/* the offsets of the registers from the beginning of the thread object */
/* 寄存器存储的位置对应的线程对象内存地址的偏移量 */
#define _ESP     0
#define _EAX     4
#define _EBX     8
#define _ECX     12
#define _EDX     16
#define _EBP     20
#define _ESI     24
#define _EDI     28
#define _PC      32

/* These definitions are used in Thread::AllocateStack(). */
#define PCState         (_PC/4-1)
#define InitialPCState  (_ESI/4-1)
#define InitialArgState (_EDX/4-1)

#define MachineStateSize 75 

typedef void (*VoidFunctionPtr)(void *arg); 
const int StackSize = (8 * 1024);	// stack size = 8 * 1024 * 4 bytes

class Thread{
private:
	 // the current stack pointer
    int *stackTop;						  
	// all registers except for stackTop
	// 线程切换时存储寄存器的数组
    void *machineState[MachineStateSize];  
											
  public:
    Thread(char* name);		// initialize a Thread 
    ~Thread(); 				// deallocate a Thread

	// Make thread run (*func)(arg)
	void Fork(VoidFunctionPtr func, void *arg); 
    void yield();  		
	// Put the thread to sleep 
    void Ssleep(bool finishing); 
	// thread Startup	
    void Begin();	
	// The thread is done executing
    void Finish();  		
	// Check if thread stack has overflowed
    void CheckOverflow();   	

	char* getName(){return name;}
private:  
    int *stack; 	 	// Bottom of the stack 
    char* name;			
    void StackAllocate(VoidFunctionPtr func, void *arg);
};


extern "C" {
	// First frame on thread execution stack; 
	//  call ThreadBegin
	//	call "func"
	//	(when func returns) call ThreadFinish()
	void ThreadRoot();

	// Stop running oldThread and start running newThread
	void SWITCH(Thread *oldThread, Thread *newThread);
}

#endif //THREAD_H