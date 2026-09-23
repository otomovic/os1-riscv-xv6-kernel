#ifndef PROJECT_BASE_V1_1_SYSCALL_CPP_H
#define PROJECT_BASE_V1_1_SYSCALL_CPP_H

#include "syscall_c.h"

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;

class Thread {
public:
    Thread(void (*body)(void*), void* arg);
    virtual ~Thread();

    int start();
    int join();

    static void dispatch();
    static int sleep(time_t time);

protected:
    Thread();
    virtual void run() {}

private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;

    static void runWrapper(void* thread);
};

class Semaphore {
public:
    Semaphore(unsigned init = 1);
    virtual ~Semaphore();

    int wait();
    int signal();

private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
public:
    void terminate();

protected:
    PeriodicThread(time_t period);
    virtual void periodicActivation() {}

private:
    time_t period;

    void run() override;
};

class Console {
public:
    static char getc();
    static void putc(char chr);
};

#endif //PROJECT_BASE_V1_1_SYSCALL_CPP_H
