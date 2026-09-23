#ifndef PROJECT_BASE_V1_1_TCB_H
#define PROJECT_BASE_V1_1_TCB_H

#include "../lib/hw.h"
#include "list.h"
#include "memoryAllocator.h"
#include "scheduler.h"

class TCB {
public:
    using Body = void (*)(void*);

    static TCB* createThread(Body body, void* arg, uint64* stackTop);
    static TCB* createMainThread();

    static void start(TCB* tcb);
    static void exit();
    static void dispatch();
    static void join(TCB* tcb);
    static int destroy(TCB* tcb);

    bool isFinished() const { return finished; }

    bool isBlocked() const { return blocked; }

    void setBlocked(bool value) { blocked = value; }

    unsigned getSemRequest() const { return semRequest; }

    void setSemRequest(unsigned value) { semRequest = value; }

    static TCB* running;

    void* operator new(size_t size) noexcept { return MemoryAllocator::mem_alloc(size); }

    void operator delete(void* ptr) noexcept { MemoryAllocator::mem_free(ptr); }

private:
    struct Context {
        uint64 ra;
        uint64 sp;
        uint64 s[12];
    };

    static uint64 constexpr STACK_ALIGNMENT = 16;

    TCB(Body body, void* arg, uint64* stackTop) :
            body(body),
            arg(arg),
            stack(stackTop),
            context({(uint64) &threadWrapper,
                     (uint64) stackTop & ~(STACK_ALIGNMENT - 1)}),  //poravnanje steka
            finished(false),
            blocked(false),
            started(false),
            main(body == nullptr),
            semRequest(0) {}

    ~TCB();

    bool isMain() const { return main; }

    void releaseJoined();

    static void threadWrapper();

    static void contextSwitch(Context* oldContext, Context* runningContext);

    Body body;
    void* arg;
    uint64* stack;
    Context context;
    bool finished;
    bool blocked;
    bool started;
    bool main;
    unsigned semRequest;
    List<TCB> joined;
};

#endif //PROJECT_BASE_V1_1_TCB_H
