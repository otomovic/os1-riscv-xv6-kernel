#include "../h/mySemaphore.h"
#include "../h/scheduler.h"

MySemaphore* MySemaphore::createSemaphore(unsigned init) {
    return new MySemaphore(init);
}

int MySemaphore::wait(unsigned n) {
    if (closed) return -1;

    if (value >= n) {
        value -= n;
        return 0;
    }

    TCB* current = TCB::running;
    current->setSemRequest(n);
    current->setBlocked(true);
    blocked.addLast(current);
    waiters++;

    TCB::dispatch();

    waiters--;
    if (closed) {
        if (waiters == 0) delete this;
        return -1;
    }
    return 0;
}

int MySemaphore::signal(unsigned n) {
    if (closed) return -1;

    value += n;
    unblockReady();
    return 0;
}

int MySemaphore::close() {
    if (closed) return -1;

    closed = true;
    unblockAll();
    if (waiters == 0) delete this;
    return 0;
}

void MySemaphore::unblockReady() {
    while (!blocked.isEmpty() && blocked.peekFirst()->getSemRequest() <= value) {
        TCB* tcb = blocked.removeFirst();
        value -= tcb->getSemRequest();
        tcb->setBlocked(false);
        Scheduler::put(tcb);
    }
}

void MySemaphore::unblockAll() {
    while (!blocked.isEmpty()) {
        TCB* tcb = blocked.removeFirst();
        tcb->setBlocked(false);
        Scheduler::put(tcb);
    }
}
