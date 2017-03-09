// -*- C++ -*-

#ifndef SYNC_H
#define SYNC_H

#include <mutex>
#include <condition_variable>

class Synchronizer {
public:
    Synchronizer() {}

    void wait() {
        std::unique_lock<std::mutex> ulck(lck);
        cv.wait(ulck);
    }
    void notify() {
        std::unique_lock<std::mutex> ulck(lck);
        cv.notify_all();
    }
private:
    std::mutex lck;
    std::condition_variable cv;
};

#endif
