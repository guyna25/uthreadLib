//
// Created by toozig on 25/04/2020.
//

#include "ThreadManager2.h"

ThreadManager2 * ThreadManager2::_manager2 = nullptr;

int ThreadManager2::changeRunnerThread(char reason)
{
    if(!_cur->_tid && !_cur->_next) //no threads in line
    {
        _cur->_quantoms++;
        _totalQuantoms++;
        return 0;
    }
    Node * perv = _cur;
    _cur = _cur->_next;
    perv->_next = nullptr;
    _cur->_quantoms++;
    _totalQuantoms++;
    _cur->_prev = nullptr;
    if (reason == DELETE)
    {
        _idMap.erase(perv->_tid);
        delete (perv);
    }
    else if (reason == TIME)
    {
        addToReady(perv);
    }
    else if (reason == BLOCKED)
    {
        perv->_blocked = true;
    }

    return _cur->_tid;
}

void ThreadManager2::addThread(int tid)
{
    Node * node = new Node(tid, _readyTail, nullptr);
    _idMap[tid] = node;
    addToReady(node);
}

void ThreadManager2::addToReady(ThreadManager2::Node * node)
{
    if(node->_blocked){return;}
    Node* tail = _readyTail? _readyTail: _cur;
    node->_prev = tail;
    node->_next = nullptr;
    tail->_next = node;
    _readyTail = node;
}

ThreadManager2 * ThreadManager2::getManager()
{
    if (_manager2 == nullptr)
    {
        _manager2 = new ThreadManager2();
    }
    return _manager2;
}

int ThreadManager2::deleteThread(int tid)
{ //index of the currnt runthread - deleted, -1  - didnt delete
    if (_idMap.count(tid))
    {
        Node * thread = _idMap[tid];
        //thread is the running thread
        if (thread == _cur)
        {
            return changeRunnerThread(DELETE);
        }
        removeMiddleNode(tid, thread);
        return _cur->_tid;
    }
    return -1;
}

int ThreadManager2::blockThread(int tid)
{  //index of the currnt runthread - deleted, -1  - didnt delete
    // thread is blocked
    if (_idMap[tid]->_blocked)
    {
        return _cur->_tid;
    }
    Node * thread = _idMap[tid];
    //thread is the running thread
    if (thread != _cur)
    {
        removeMiddleNode(tid, thread);

    }
    thread->_blocked = true;
    return _cur->_tid;


}

void ThreadManager2::removeMiddleNode(int tid, ThreadManager2::Node * thread)
{
    if (thread->_prev != nullptr)
    {
        thread->_prev->_next = thread->_next;
    }
    if (thread->_next != nullptr)
    {
        thread->_next->_prev = thread->_prev;
    }
    thread->_prev = nullptr;
    thread->_next = nullptr;
}

int ThreadManager2::timeChangeThread()
{
    if (!_cur->_next)
    {
        _cur->_quantoms++;
        _totalQuantoms++;
        return _cur->_tid;
    }
    return changeRunnerThread(TIME);
}

void ThreadManager2::unblockThread(int tid)
{
    if (_idMap[tid]->_blocked)
    {
        _idMap[tid]->_blocked = false;
        addToReady(_idMap[tid]);
    }
}

ThreadManager2::ThreadManager2() :_readyTail(nullptr), _totalQuantoms(0)
{
    Node * zero = new Node(0, nullptr, nullptr);
    _idMap[0] = zero;
    _cur = zero;

}

int ThreadManager2::getThreadQuantoms(int tid)
{
    return _idMap[tid]->_quantoms;
}

ThreadManager2::~ThreadManager2()
{
    _idMap.empty();
}


