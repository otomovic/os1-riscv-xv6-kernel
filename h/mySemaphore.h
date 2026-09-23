#ifndef PROJECT_BASE_V1_1_MY_SEMAPHORE_H
#define PROJECT_BASE_V1_1_MY_SEMAPHORE_H

#include "../lib/hw.h"
#include "list.h"
#include "memoryAllocator.h"
#include "tcb.h"

class MySemaphore {
public:
    static MySemaphore* createSemaphore(unsigned init);

    int wait(unsigned n);
    int signal(unsigned n);
    int close();

    void* operator new(size_t size) noexcept { return MemoryAllocator::mem_alloc(size); }

    void operator delete(void* ptr) noexcept { MemoryAllocator::mem_free(ptr); }

private:
    explicit MySemaphore(unsigned init) :
            value(init),
            closed(false),
            waiters(0) {}

    ~MySemaphore() {}

    void unblockReady();

    void unblockAll();

    unsigned value;
    bool closed;
    unsigned waiters;
    List<TCB> blocked;
};

#endif //PROJECT_BASE_V1_1_MY_SEMAPHORE_H
