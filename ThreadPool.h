#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <iostream>
#include <functional>
#include <queue>

using namespace std;
using Task =  std::function<void()>;



class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads) {
        start(numThreads);
    }
    ~ThreadPool() {
        stop();
    }
    void enqueue(Task task) {
        {
            std::unique_lock <std::mutex> lock(mEventMutex);
            mTasks.emplace(std::move(task));
        }
        mEventVar.notify_one();
    }

private:
    vector<thread> mThreads;
    condition_variable mEventVar;
    mutex mEventMutex;
    bool mStopping = false;
    std::queue<Task> mTasks;

    void start(size_t numThreads) {
        for (auto i=0u; i<numThreads; i++) {
            mThreads.emplace_back([=] {
                while (true) {
                    Task task;
                    {
                        unique_lock<mutex> lock(mEventMutex);
//                        sync_cout()<< i << "lock" << this_thread::get_id() << endl;

                        mEventVar.wait(lock, [=] { return mStopping || !mTasks.empty(); });

                        if (mStopping && mTasks.empty())
                            break;

                        task = std::move(mTasks.front());
                        mTasks.pop();
                    }
                    task();
                }
            });
        }
    }
    void stop() noexcept {
        {
            unique_lock<mutex> lock(mEventMutex);
            mStopping = true;
        }

        mEventVar.notify_all();

        for (auto &thread : mThreads){
//            sync_cout()<< "stopped" << thread.get_id() << endl;
            thread.join();
        }
    }


};

#endif // THREADPOOL_H
