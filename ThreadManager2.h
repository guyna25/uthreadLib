//
// Created by toozig on 25/04/2020.
//

#include <map>
#include "Thread.h"

#ifndef EX2_OS_THREADMANAGER2_H
#define EX2_OS_THREADMANAGER2_H

static const char RUNNING = 'r';
static const char READY = 'e';
static const char BLOCKED = 'b';
static const char TIME = 't';
static const char DELETE = 'd';



class ThreadManager2 {


public:
    int isExist(const int tid){return _idMap.count(tid);};
    ThreadManager2(const ThreadManager2 &) = delete;
    ThreadManager2 &operator=(const ThreadManager2 &) = delete;
    static ThreadManager2 *getManager();
    int get_cur() {return _cur == nullptr ? 0: _cur->_tid;};
    void addThread(int tid);
    int deleteThread(int tid);
    int blockThread(int tid);
    int getTotalQuantoms() const{return _totalQuantoms;}
    int getThreadQuantoms(int tid);
    int timeChangeThread();
    void unblockThread(int tid);
    ~ThreadManager2();
private:

    static ThreadManager2 *_manager2;



    //inner class
    class Node{ ;
    public:
        int _tid;
        Node *_prev;
        Node *_next;
        int _quantoms;
        bool _blocked;
        ~Node()= default;;

        Node(int tid, Node * prev, Node* next):_tid(tid), _prev(prev), _next(next),
                                                        _quantoms(0), _blocked(false){}
    };

    //methods
    void removeMiddleNode(int tid, Node *thread);
    void addToReady(Node* node);

    int changeRunnerThread(char reason); //todo maybe move to private
    //fields
    Node *_cur;
    Node *_readyTail;
    std::map<int, Node* > _idMap;
    int _totalQuantoms;
    //Ctor
    ThreadManager2();
};

#endif //EX2_OS_THREADMANAGER2_H
