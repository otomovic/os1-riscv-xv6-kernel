#include "../h/syscall_cpp.h"

void* operator new(size_t size) {
    return mem_alloc(size);
}

void* operator new[](size_t size) {
    return mem_alloc(size);
}

void operator delete(void* ptr) noexcept {
    mem_free(ptr);
}

void operator delete[](void* ptr) noexcept {
    mem_free(ptr);
}

Thread::Thread(void (*body)(void*), void* arg) :
        myHandle(nullptr),
        body(body),
        arg(arg) {
    thread_create_without_start(&myHandle, body, arg);
}

Thread::Thread() :
        myHandle(nullptr),
        body(nullptr),
        arg(nullptr) {
    thread_create_without_start(&myHandle, runWrapper, this);
}

Thread::~Thread() {
    thread_join(myHandle);
    thread_destroy(myHandle);
    myHandle = nullptr;
}

int Thread::start() {
    return thread_start(myHandle);
}

int Thread::join() {
    return thread_join(myHandle);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

void Thread::runWrapper(void* thread) {
    Thread* self = (Thread*) thread;
    if (self != nullptr) self->run();
}

Semaphore::Semaphore(unsigned init) :
        myHandle(nullptr) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
    myHandle = nullptr;
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

PeriodicThread::PeriodicThread(time_t period) :
        Thread(),
        period(period) {}

void PeriodicThread::terminate() {
    period = 0;
}

void PeriodicThread::run() {
    while (period != 0) {
        periodicActivation();
        sleep(period);
    }
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char chr) {
    ::putc(chr);
}
