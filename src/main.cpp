#include "../h/memoryAllocator.h"
#include "../h/riscv.h"
#include "../h/syscall_c.h"
#include "../h/tcb.h"

extern void userMain();

static void userMainWrapper(void* arg) {
    userMain();
}

int main() {
    MemoryAllocator::mem_start();
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    TCB::running = TCB::createMainThread();
    if (TCB::running == nullptr) return -1;

    thread_t userMainHandle = nullptr;
    if (thread_create(&userMainHandle, userMainWrapper, nullptr) != 0) return -1;

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    TCB* userMainThread = (TCB*) userMainHandle;
    while (!userMainThread->isFinished()) {
        thread_dispatch();
    }

    thread_destroy(userMainHandle);
    return 0;
}
