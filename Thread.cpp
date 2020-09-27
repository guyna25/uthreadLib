//
// Created by toozig on 18/04/2020.
//


#include <iostream>
#include "Thread.h"
#include "ThreadManager2.h"

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
        "rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif

address_t Thread::translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
                 "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}

Thread::Thread(int tid, int priority, void (* f)(), int qtime) :
        _tid(tid), _priority(priority), _qtime(qtime)
{
    _stack = new char[STACK_SIZE];
    address_t sp, pc;

    sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)f;
    sigsetjmp(_env, 1);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_env->__saved_mask);

}


void Thread::initEnv(void (* f)())
{




}


void Thread::setPriority(int priority)
{
    _priority = priority;

}

Thread::~Thread()
{
    delete[] _stack;
}

Thread::Thread() = default;


