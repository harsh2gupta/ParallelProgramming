//
// Created by Shweta Sahu on 3/13/18.
//

#ifndef HW1_CONCURRENTDEQUE_H
#define HW1_CONCURRENTDEQUE_H

#include <deque>
#include <mutex>
template <class T>
class ConcurrentDeque{
private:
    std::deque<T> myQueue;
    std::mutex mtx;
    void lock(){
        mtx.lock();
    }
    void unlock(){
        mtx.unlock();
    }

public:
    long work;
    //ConcurrentDeque():mtx(){}
    T pop_front(){
        T val = NULL;
        lock();
        if(!myQueue.empty()) {
            val = myQueue.front();
            myQueue.pop_front();
        }
        unlock();
        return val;
    }
    T pop_back(){
        T val = NULL;
        lock();
        if(!myQueue.empty()) {
            val = myQueue.back();
            myQueue.pop_back();
        }
        unlock();
        return val;
    }
    void push_back(T val){
        lock();
        myQueue.push_back(val);
        unlock();
    }
    int size(){
        return myQueue.size();
    }
};

#endif //HW1_CONCURRENTDEQUE_H
