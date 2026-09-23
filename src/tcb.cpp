#include "../h/tcb.h"
#include "../h/riscv.h"
#include "../h/syscall_c.h"

TCB* TCB::running = nullptr;

TCB* TCB::createThread(Body body, void* arg, uint64* stackTop) {
    if (body == nullptr || stackTop == nullptr) return nullptr;
    return new TCB(body, arg, stackTop);
}

//placeholder null pointeri za body i stackTop
TCB* TCB::createMainThread() {
    TCB* tcb = new TCB(nullptr, nullptr, nullptr);
    if (tcb != nullptr) tcb->started = true;
    return tcb;
}

void TCB::start(TCB* tcb) {
    if (tcb == nullptr || tcb->started) return;
    tcb->started = true;
    Scheduler::put(tcb);
}

void TCB::exit() {
    running->finished = true;
    running->releaseJoined();
    dispatch();
}

void TCB::dispatch() {
    TCB* old = running;
    if (!old->isFinished() && !old->isBlocked()) Scheduler::put(old);

    running = Scheduler::get();
    if (running->isMain()) Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    else Riscv::mc_sstatus(Riscv::SSTATUS_SPP);

    contextSwitch(&old->context, &running->context);
}

void TCB::join(TCB* tcb) {
    if (tcb == nullptr || tcb == running) return;
    if (!tcb->started || tcb->isFinished()) return;

    running->setBlocked(true);
    tcb->joined.addLast(running);
    dispatch();
}

int TCB::destroy(TCB* tcb) {
    if (tcb == nullptr || tcb == running) return -1;
    if (tcb->started && !tcb->isFinished()) return -1;

    delete tcb;
    return 0;
}

void TCB::releaseJoined() {
    while (!joined.isEmpty()) {
        TCB* tcb = joined.removeFirst();
        tcb->setBlocked(false);
        Scheduler::put(tcb);
    }
}

void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);
    thread_exit();
}

TCB::~TCB() {
    if (stack != nullptr) MemoryAllocator::mem_free((char*) stack - DEFAULT_STACK_SIZE);
}
