#include "Thread.h"
#include "Simulator.h"
#include <assert.h>

Thread::Thread(char* threadName){
    
	name = threadName;
	stackTop = NULL;
    stack = NULL;
    for (int i = 0; i < MachineStateSize; i++) {
		machineState[i] = NULL;
    }
}

Thread::~Thread(){  
    assert(this != simulator->currentThread);
    if (stack != NULL)
		delete [] stack;
}


void Thread::Fork(VoidFunctionPtr func, void *arg){
    StackAllocate(func, arg);
    simulator->ReadyToRun(this);	
}    

// this is put at the top of the execution stack, for detecting stack overflows
const int STACK_FENCEPOST = 0xdedbeef;

void Thread::CheckOverflow(){
    if (stack != NULL) {
		assert(*stack == STACK_FENCEPOST);
   }
}


void Thread::Begin (){
    assert(this == simulator->currentThread);
    simulator->CheckToBeDestroyed();
}

void Thread::Finish (){
   
    assert(this == simulator->currentThread);
    Ssleep(true);				
}


void Thread::yield (){
    Thread *nextThread;
	
    assert(this == simulator->currentThread);

    nextThread = simulator->FindNextToRun();

    if (nextThread != NULL) {
		simulator->ReadyToRun(this);
		simulator->Run(nextThread, false);
    }
}

void Thread::Ssleep (bool finishing){

    Thread *nextThread;

	while ((nextThread = simulator->FindNextToRun()) == NULL){}

    // returns when it's time for us to run
    simulator->Run(nextThread, finishing); 
}

void Thread::StackAllocate(VoidFunctionPtr func, void *arg){

    stack = (int *) new char[StackSize * sizeof(int)];

    stackTop = stack + StackSize - 4;	// -4 to be on the safe side!
	stackTop --;
    *stack = STACK_FENCEPOST;

    machineState[PCState] = (void*)ThreadRoot;		//7 pc
    machineState[InitialPCState] = (void*)func;		//5	esi
    machineState[InitialArgState] = (void*)arg;		//3 edi

}


extern "C" {


	//Func线程保存函数地址
	//IniArg保存函数参数地址
	void* IniArg = (void*)0;
	void* Func = (void*)0;

	void ThreadRoot(){
		simulator->currentThread->Begin();
		__asm{	
			push   IniArg       /* 线程函数func的参数入栈 */
			call   Func          /* call线程函数func */
			add    esp,4        /* 释放参数传递的栈空间 */
		}
		simulator->currentThread->Finish();
	}

	unsigned long _eax_save = 0;                          //全局中间变量

	void SWITCH(Thread *oldThread, Thread *newThread){

		__asm{
			//align  2
			pop         edi        /* 恢复edi */
			pop         esi        /* 恢复esi */
			pop         ebx        /* 恢复ebx */
			mov         esp,ebp    /* 释放要函数局部变量空间 */
			pop         ebp        /* 恢复ebp */

			mov    _eax_save,eax   /* 暂存eax, 注意:_eax_save为全局变量 */

			mov    eax, [esp+4]    /* eax 指向oldThread */
			mov    [_EBX+eax],ebx  /* 保存相关寄存器值, 到oldThread的空间中 */
			mov    [_ECX+eax],ecx
			mov    [_EDX+eax],edx
			mov    [_ESI+eax],esi
			mov    [_EDI+eax],edi
			mov    [_EBP+eax],ebp
			mov    [_ESP+eax],esp  /* 保存栈指针 */

			mov     ebx,_eax_save  /* 取暂存的eax，从全局变量 _eax_save中 */
			mov    [_EAX+eax],ebx  /* 保存初始eax的值 */
			mov    ebx,[esp+0]     /* 取返回地址 */
			mov    [_PC +eax],ebx  /* 保存返回地址 */

			mov    eax,[esp+8]     /* eax指向newThread */
			mov    ebx,[_EAX+eax]  /* 取newThread保存的eax值*/
			mov    _eax_save,ebx   /* 暂存到 _eax_save */

			mov    ebx,[_EBX+eax]  /* 恢复newThread保存的寄存器值 */
			mov    ecx,[_ECX+eax]
			mov    edx,[_EDX+eax]
			mov    IniArg,edx      //
			mov    esi,[_ESI+eax]
			mov    Func,esi        //
			mov    edi,[_EDI+eax]
			mov    ebp,[_EBP+eax]
			mov    esp,[_ESP+eax]  //恢复栈指针

			mov    eax,[_PC +eax]  //保存返回地址到eax
			mov    [esp+0],eax     

			mov    eax,[_eax_save]

			ret                    /*直接返回*/
		}
	}

}