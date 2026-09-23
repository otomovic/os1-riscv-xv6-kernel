#include "../h/riscv.h"
#include "../h/memoryAllocator.h"
#include "../h/mySemaphore.h"
#include "../h/syscall_codes.h"
#include "../h/tcb.h"
#include "../lib/console.h"


//privilege drop pri prvom ulasku u threadWrapper
void Riscv::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap(uint64* registers) {
    uint64 volatile sepc = r_sepc();
    uint64 volatile sstatus = r_sstatus();
    uint64 scause = r_scause();

    if (scause == SCAUSE_ECALL_USER || scause == SCAUSE_ECALL_SUPERVISOR) {
        sepc = sepc + 4;
        handleSyscall(registers);
    }
    else if (scause == SCAUSE_TIMER_INTERRUPT) {
        handleTimerInterrupt();
    }
    else if (scause == SCAUSE_CONSOLE_INTERRUPT) {
        handleConsoleInterrupt();
    }
    else {

    }

    w_sstatus(sstatus);
    w_sepc(sepc);
}

void Riscv::handleTimerInterrupt() {
    mc_sip(SIP_SSIP);
}

void Riscv::handleConsoleInterrupt() {
    console_handler();
}

void Riscv::handleSyscall(uint64* registers) {
    uint64 code = registers[REG_A0];

    switch (code) {
        case MEM_ALLOC: {
            size_t blocks = (size_t) registers[REG_A1];
            registers[REG_A0] = (uint64) MemoryAllocator::mem_alloc(blocks * MEM_BLOCK_SIZE);
            break;
        }

        case MEM_FREE: {
            void* ptr = (void*) registers[REG_A1];
            registers[REG_A0] = (uint64) MemoryAllocator::mem_free(ptr);
            break;
        }

        case THREAD_CREATE:
        case THREAD_CREATE_WITHOUT_START: {
            TCB** handle = (TCB**) registers[REG_A1];
            TCB::Body body = (TCB::Body) registers[REG_A2];
            void* arg = (void*) registers[REG_A3];
            uint64* stackTop = (uint64*) registers[REG_A4];

            TCB* tcb = TCB::createThread(body, arg, stackTop);
            if (handle != nullptr) *handle = tcb;
            if (tcb == nullptr) {
                registers[REG_A0] = (uint64) -1;
                break;
            }
            if (code == THREAD_CREATE) TCB::start(tcb);
            registers[REG_A0] = 0;
            break;
        }

        case THREAD_EXIT:
            TCB::exit();
            break;

        case THREAD_DISPATCH:
            TCB::dispatch();
            break;

        case THREAD_START: {
            TCB* tcb = (TCB*) registers[REG_A1];
            if (tcb == nullptr) {
                registers[REG_A0] = (uint64) -1;
                break;
            }
            TCB::start(tcb);
            registers[REG_A0] = 0;
            break;
        }

        case THREAD_JOIN: {
            TCB* tcb = (TCB*) registers[REG_A1];
            if (tcb == nullptr) {
                registers[REG_A0] = (uint64) -1;
                break;
            }
            TCB::join(tcb);
            registers[REG_A0] = 0;
            break;
        }

        case THREAD_DESTROY: {
            TCB* tcb = (TCB*) registers[REG_A1];
            registers[REG_A0] = (uint64) TCB::destroy(tcb);
            break;
        }

        case SEM_OPEN: {
            MySemaphore** handle = (MySemaphore**) registers[REG_A1];
            unsigned init = (unsigned) registers[REG_A2];

            MySemaphore* sem = MySemaphore::createSemaphore(init);
            if (handle != nullptr) *handle = sem;
            registers[REG_A0] = (uint64) (sem != nullptr ? 0 : -1);
            break;
        }

        case SEM_CLOSE: {
            MySemaphore* sem = (MySemaphore*) registers[REG_A1];
            if (sem == nullptr) {
                registers[REG_A0] = (uint64) -1;
                break;
            }
            registers[REG_A0] = (uint64) sem->close();
            break;
        }

        case SEM_WAIT:
        case SEM_WAIT_N: {
            MySemaphore* sem = (MySemaphore*) registers[REG_A1];
            unsigned n = (code == SEM_WAIT ? 1 : (unsigned) registers[REG_A2]);
            if (sem == nullptr) {
                registers[REG_A0] = (uint64) -1;
                break;
            }
            registers[REG_A0] = (uint64) sem->wait(n);
            break;
        }

        case SEM_SIGNAL:
        case SEM_SIGNAL_N: {
            MySemaphore* sem = (MySemaphore*) registers[REG_A1];
            unsigned n = (code == SEM_SIGNAL ? 1 : (unsigned) registers[REG_A2]);
            if (sem == nullptr) {
                registers[REG_A0] = (uint64) -1;
                break;
            }
            registers[REG_A0] = (uint64) sem->signal(n);
            break;
        }

        case TIME_SLEEP:
            registers[REG_A0] = 0;
            break;

        case GETC:
            registers[REG_A0] = (uint64) __getc();
            break;

        case PUTC:
            __putc((char) registers[REG_A1]);
            break;

        default:
            registers[REG_A0] = (uint64) -1;
            break;
    }
}
