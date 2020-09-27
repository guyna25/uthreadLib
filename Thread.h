#ifndef EX2_RESOURCES_20200412_THREAD_H
#define EX2_RESOURCES_20200412_THREAD_H
#include <setjmp.h>
#include <signal.h>
#define STACK_SIZE 8192
#define JB_SP 6
#define JB_PC 7
#include <unistd.h>







//static const char RUNNING = 'r';
//static const char READY = 'e';
//static const char BLOCKED = 'b';
typedef unsigned long address_t;
void timer_handler1(int sig);
class Thread
{
public:
    Thread(int tid, int priority, void (*f)(), int qtime);
    Thread();
    int getPriority() const {return _priority;};
    sigjmp_buf &get_env(){ return  _env;}
    int get_qtime(){ return _qtime;}
    static address_t translate_address(address_t addr);
    void setPriority(int priority);
    ~Thread();
private:

    int _tid, _priority{},_qtime; //todo set it
    char *_stack;
    // copied from jmp
    sigjmp_buf _env;
    //todo move this to cpp
    void initEnv(void (*f)());
};


#endif //EX2_RESOURCES_20200412_THREAD_H
