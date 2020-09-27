#include "uthreads.h"
#include <iostream>
#include <queue>
#include <sys/time.h>
#include <memory>
#include "Thread.h"
#include "ThreadManager2.h"

#define SECOND 1000000
#define MAX_THREADS 100
#define ERROR "thread library error: "
std::priority_queue <int, std::vector<int>, std::greater<int>> tidPq;
std::map<int, std::shared_ptr<Thread>> lib;
int curRunnerIdx = 0;
struct sigaction sa = {0};
sigset_t currSigBlock;
int * prio;
int prio_size;
struct itimerval timer;
sigjmp_buf zeroEnv;


void method_3() {}


int error(std::string  msg) {
    std:: cerr << ERROR << msg << std::endl;
    return -1;
}


/**
 * This function blocks all alarm signals
 * */
void blockAlrmSig()
{
    sigemptyset(&currSigBlock);
    sigaddset(&currSigBlock, SIGVTALRM);
    sigaddset(&currSigBlock, SIGALRM);
    sigprocmask(SIG_BLOCK, &currSigBlock, NULL);
}

/**
 * This function unblocks all alarm signals
 * */
void unblockAlrmSig()
{
    sigemptyset(&currSigBlock);
    sigaddset(&currSigBlock, SIGVTALRM);
    sigaddset(&currSigBlock, SIGALRM);
    sigprocmask(SIG_UNBLOCK, &currSigBlock, NULL);
}

void setTimer(int qtime) {
    int sec = qtime / SECOND;
    int usec = qtime % SECOND;

    timer.it_value.tv_sec = sec;    // following time intervals, seconds part
    timer.it_value.tv_usec = usec;    // following time intervals, microseconds part

    timer.it_interval.tv_sec = 0;	// following time intervals, seconds part
    timer.it_interval.tv_usec = 0;	// following time intervals, microseconds part
    int stam = setitimer (ITIMER_VIRTUAL, &timer, nullptr);
    if (stam){
        printf("setitimer error.");
    }
}

void changeThread2(int sig) {
    blockAlrmSig();
    int ret_val = sigsetjmp(lib[curRunnerIdx]->get_env(),1);
//    printf("SWITCH: ret_val=%d\n", ret_val); //todo delete
    if (ret_val == 1) {
//        printf("SWITCH: ret_val=%d\n", ret_val);
        return;
    }


    curRunnerIdx = ThreadManager2::getManager()->timeChangeThread();
    std::shared_ptr<Thread> thread = lib[curRunnerIdx]; //todo delete
    unblockAlrmSig();
    setTimer(thread->get_qtime());
    siglongjmp(thread->get_env(),1);

}

int getNextAvailableTid()
{
    int newTid;
    if (tidPq.empty())
    {
        return lib.size();
    }else {
        newTid = tidPq.top();
        tidPq.pop();
    }
    return newTid;
}




int uthread_spawn(void (*f)(void), int priority)
{
    if ((priority >= prio_size) | (priority < 0))
    {
        return error(PRIORITY_ERROR);
    }

    int tid = getNextAvailableTid();
    if (tid >= MAX_THREADS)
    {

        return error(ERROR_MAX_THREADS);
    }


    std::shared_ptr<Thread> thread(new Thread(tid, priority, f, prio[priority]));
    lib.insert({tid,thread});

    if(tid){ThreadManager2::getManager()->addThread(tid);}
    return tid;
}


int uthread_init(int *quantum_usecs, int size) {
    if( size <= 0)
    {
        return error(INVALID_SIZE);
    }
    for (int i = 0; i < size; ++i) {
        if(quantum_usecs[i] <= 0)
        {
            return error(INVALID_USEC);
        }
    }
    prio = quantum_usecs;
    prio_size = size;

    uthread_spawn(nullptr,0);
    ThreadManager2::getManager()->timeChangeThread();
    // Install timer_handler as the signal handler for SIGVTALRM.
    sa.sa_handler = &changeThread2;
    sigsetjmp(zeroEnv,1);
    int stam = sigaction(SIGVTALRM , &sa,nullptr);
    if (stam < 0) {
        printf("sigaction error.");
        std::exit(EXIT_FAILURE); //todo also change you lazy bastard (from guy to future guy)
    }
    setTimer(prio[lib[curRunnerIdx]->getPriority()]);
    return 0;
}

int uthread_change_priority(int tid, int priority)
{
    if(!lib.count(tid))
    {
        return error(INVALID_ID);
    }
    if ((priority >= prio_size) | (priority < 0))
    {
        return error(PRIORITY_ERROR);
    }
    std::shared_ptr<Thread> thread = lib[tid];
    thread->setPriority(priority);
    return 0;
}

int uthread_terminate(int tid) {
    if(!lib.count(tid))
    {
        return error(INVALID_ID);
    }
    if(!tid)
    {
        setTimer(0);
        delete(ThreadManager2::getManager());
        lib.empty();
        //todo delete all the shit
        exit(0);
    }
    ThreadManager2* manager2 = ThreadManager2::getManager();
    bool isRunner = tid == curRunnerIdx;
    blockAlrmSig();
    int newRunner = manager2->deleteThread(tid);
    if(isRunner){
        curRunnerIdx = newRunner;
        setTimer(lib[curRunnerIdx]->get_qtime());
    }
    std::shared_ptr<Thread> thread = lib[tid];
    lib.erase(tid);
    tidPq.push(tid);
    unblockAlrmSig();
    if(isRunner) {
        siglongjmp(lib[curRunnerIdx]->get_env(), 1);
    }
    return 0;

}

int uthread_block(int tid) {
    if(!lib.count(tid) | !tid)
    {
        return error(INVALID_ID);
    }

    bool isCur = tid == curRunnerIdx; //checks if the blocked thread is the running one
    ThreadManager2* manager = ThreadManager2::getManager();
    if(isCur)
    {
        setTimer(0);
        manager->blockThread(tid);
        changeThread2(0);
        return 0;
    }
    manager->blockThread(tid);
    return 0;
}

int uthread_resume(int tid) {
    if(!lib.count(tid))
    {
        return error(INVALID_ID);
    }
    ThreadManager2::getManager()->unblockThread(tid);
    return 0;
}

int uthread_get_tid() {return curRunnerIdx;}

int uthread_get_total_quantums() {
    return ThreadManager2::getManager()->getTotalQuantoms();
}

int uthread_get_quantums(int tid) {
    if(!lib.count(tid))
    {
        return error(INVALID_ID);
    }
    return ThreadManager2::getManager()->getThreadQuantoms(tid);
}

void method_1() {
    for (;;) {
        std::cout << "I'm Guy!" << std::endl;
    }
}

void method_2() {

    for (;;)
    {
        std::cout << "I'm Ido!" << std::endl;
    }
}



int check()
{
    int prio[] = {5 * 100000,1 * 100000,3 * 100000};
    uthread_init(prio, 3);
//    uthread_spawn(method_1, 1);
//    uthread_spawn(method_2, 1);
    uthread_block(1);

    while (true)
    {}
}
