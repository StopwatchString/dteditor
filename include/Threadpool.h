#ifndef DTEDITOR_THREADPOOL_H
#define DTEDITOR_THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <atomic>
#include <iostream>

//-----------------------------------------
// class Threadpool
//-----------------------------------------
class Threadpool
{
    class Thread {
    public:
        Thread(std::function<bool(std::function<void()>&)> getJob) {
            shouldExit = false;
            working = false;
            thread = std::thread(
                [this, &getJob]() {
                    std::function<void()> job;
                    while (!shouldExit) {
                        if (getJob(job)) {
                            {
                                std::lock_guard<std::mutex> lock(workingMutex);
                                working = true;
                            }
                            job();
                            {
                                std::lock_guard<std::mutex> lock(workingMutex);
                                working = false;
                            }
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                }
            );
        }

        Thread(Thread&& other) noexcept {
            std::cout << "move" << std::endl;
        }

        bool active() {
            std::lock_guard<std::mutex> lock(workingMutex);
            return working;
        }

        void exitAndWait() {
            shouldExit = true;
            thread.join();
        }

    private:
        std::mutex workingMutex;

        bool working;
        bool shouldExit;
        std::thread thread;
    };

public:
    Threadpool(uint32_t threadCount);
    ~Threadpool();

    Threadpool(const Threadpool& other) = delete;
    Threadpool& operator=(const Threadpool& other) = delete;

    Threadpool(Threadpool&& other) = delete;
    Threadpool& operator=(Threadpool&& other) = delete;

    uint32_t addJob(const std::function<void()>& job);
    bool getJob(std::function<void()>& outJob);

    void waitForClearQueue();

private:

    std::mutex jobCountMutex;
    bool threadpoolExiting;
    bool writerWaiting;
    std::mutex jobMutex;
    std::vector<Thread> threads;
    std::queue<std::function<void()>> jobs;
};

#endif