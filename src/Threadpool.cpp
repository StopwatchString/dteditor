#include "Threadpool.h"

//-----------------------------------------
// Constructor
//-----------------------------------------
Threadpool::Threadpool(uint32_t threadCount)
{
    threadpoolExiting = false;
    writerWaiting = false;

    for (uint32_t i = 0; i < threadCount; i++) {
        // threads.emplace_back(getJob);
    }
}

//-----------------------------------------
// Destructor
//-----------------------------------------
Threadpool::~Threadpool()
{
    threadpoolExiting = true;
    for (Thread& t : threads) {
        t.exitAndWait();
    }
}

//-----------------------------------------
// addJob()
//-----------------------------------------
uint32_t Threadpool::addJob(const std::function<void()>& job)
{
    writerWaiting = true;
    std::unique_lock<std::mutex> lock(jobMutex);
    writerWaiting = false;

    jobs.push(job);

    return 0;
}

//-----------------------------------------
// getJob()
//-----------------------------------------
bool Threadpool::getJob(std::function<void()>& outJob)
{
    bool retVal = false;
    if (!writerWaiting) {
        std::unique_lock<std::mutex> lock(jobMutex, std::try_to_lock);
        if (lock.owns_lock()) {
            if (jobs.size() > 0) {
                outJob = jobs.front();
                jobs.pop();
                retVal = true;
            }
        }
    }
    return retVal;
}

#include <iostream>
//-----------------------------------------
// waitForClearQueue()
//-----------------------------------------
void Threadpool::waitForClearQueue()
{
    while (jobs.size() != 0) {
        std::this_thread::yield();
    }
}