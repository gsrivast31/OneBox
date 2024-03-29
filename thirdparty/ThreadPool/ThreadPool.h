#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <deque>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <exception>
#include "CommonDefines.h"

using namespace NewGroup::Core;

// need this type to "erase" the return type of the packaged task
struct any_packaged_base {
    virtual void execute() = 0;
};

template<class R>
struct any_packaged : public any_packaged_base {
    any_packaged(std::packaged_task<R()> &&t)
     : task(std::move(t))
    {
    }
    void execute()
    {
        task();
        task.get_future().get();
    }
    std::packaged_task<R()> task;
};

class any_packaged_task {
public:
    any_packaged_task(){}
    template<class R>
    any_packaged_task(std::packaged_task<R()> &&task)
     : ptr(new any_packaged<R>(std::move(task)))
    {
    }
    void operator()()
    {
        ptr->execute();
    }
private:
    std::shared_ptr<any_packaged_base> ptr;
};

class ThreadPool;
 
// our worker thread objects
class Worker {
public:
    Worker(ThreadPool &s) : pool(s) { }
    void operator()();
private:
    ThreadPool &pool;
};

// the actual thread pool
class ThreadPool {
public:
    
    
    
    ThreadPool(size_t);
    template<class T, class F>
    void enqueue(F f, TaskPriority priority = kTaskPriority_Normal);
    ~ThreadPool();
private:
    friend class Worker;

    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::deque< any_packaged_task > tasks[3];
    
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};
 
void Worker::operator()()
{
    while(true)
    {
        std::unique_lock<std::mutex> lock(pool.queue_mutex);
        while(!pool.stop && pool.tasks[kTaskPriority_High].empty() && pool.tasks[kTaskPriority_Normal].empty() && pool.tasks[kTaskPriority_Low].empty())
            pool.condition.wait(lock);
        if(pool.stop)
            return;
        
        TaskPriority priority = kTaskPriority_Low;
        any_packaged_task task;
        if(!pool.tasks[kTaskPriority_High].empty())
        {
            priority = kTaskPriority_High;
        }
        else if(!pool.tasks[kTaskPriority_Normal].empty())
        {
            priority = kTaskPriority_Normal;
        }
        task = pool.tasks[priority].front();
        pool.tasks[priority].pop_front();
        lock.unlock();
        try{
            task();
        }
        catch(std::exception& e)
        {
            NSLog(@"Standard exception: %s", e.what());
        }
        catch(...){
            assert(false);
        }
        
    }
}
 
// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads)
    :   stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.push_back(std::thread(Worker(*this)));
}

// add new work item to the pool
template<class T, class F>
void ThreadPool::enqueue(F f, TaskPriority priority)
{
    std::packaged_task<T()> task(f);
    {
        std::unique_lock<std::mutex> lock(queue_mutex);    
        tasks[priority].push_back(any_packaged_task(std::move(task)));
    }
    condition.notify_one();
}
 
// the destructor joins all threads
ThreadPool::~ThreadPool()
{
    stop = true;
    condition.notify_all();
    for(size_t i = 0;i<workers.size();++i)
        workers[i].join();
}

#endif
