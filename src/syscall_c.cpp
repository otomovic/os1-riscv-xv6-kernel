#include "../h/syscall_c.h"
#include "../h/syscall_codes.h"
#include "../lib/console.h"

static uint64 syscall(uint64 code, uint64 arg1 = 0, uint64 arg2 = 0,
                      uint64 arg3 = 0, uint64 arg4 = 0) {
    register uint64 a0 __asm__("a0") = code;
    register uint64 a1 __asm__("a1") = arg1;
    register uint64 a2 __asm__("a2") = arg2;
    register uint64 a3 __asm__("a3") = arg3;
    register uint64 a4 __asm__("a4") = arg4;

    __asm__ volatile ("ecall"
            : "+r"(a0)
            : "r"(a1), "r"(a2), "r"(a3), "r"(a4)
            : "memory");

    return a0;
}

static int createThread(uint64 code, thread_t* handle,
                        void (*start_routine)(void*), void* arg) {
    if (handle == nullptr || start_routine == nullptr) return -1;

    void* stack = mem_alloc(DEFAULT_STACK_SIZE);
    if (stack == nullptr) return -1;

    void* stackTop = (char*) stack + DEFAULT_STACK_SIZE;
    int result = (int) syscall(code, (uint64) handle, (uint64) start_routine,
                              (uint64) arg, (uint64) stackTop);
    if (result != 0) mem_free(stack);
    return result;
}

void* mem_alloc(size_t size) {
    if (size == 0) return nullptr;

    size_t blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return (void*) syscall(MEM_ALLOC, blocks);
}

int mem_free(void* ptr) {
    if (ptr == nullptr) return -1;

    return (int) syscall(MEM_FREE, (uint64) ptr);
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg) {
    return createThread(THREAD_CREATE, handle, start_routine, arg);
}

int thread_exit() {
    return (int) syscall(THREAD_EXIT);
}

void thread_dispatch() {
    syscall(THREAD_DISPATCH);
}

int thread_create_without_start(thread_t* handle, void (*start_routine)(void*), void* arg) {
    return createThread(THREAD_CREATE_WITHOUT_START, handle, start_routine, arg);
}

int thread_start(thread_t handle) {
    return (int) syscall(THREAD_START, (uint64) handle);
}

int thread_join(thread_t handle) {
    return (int) syscall(THREAD_JOIN, (uint64) handle);
}

int thread_destroy(thread_t handle) {
    return (int) syscall(THREAD_DESTROY, (uint64) handle);
}

int sem_open(sem_t* handle, unsigned init) {
    if (handle == nullptr) return -1;

    return (int) syscall(SEM_OPEN, (uint64) handle, init);
}

int sem_close(sem_t handle) {
    return (int) syscall(SEM_CLOSE, (uint64) handle);
}

int sem_wait(sem_t id) {
    return (int) syscall(SEM_WAIT, (uint64) id);
}

int sem_signal(sem_t id) {
    return (int) syscall(SEM_SIGNAL, (uint64) id);
}

int sem_wait_n(sem_t id, unsigned n) {
    return (int) syscall(SEM_WAIT_N, (uint64) id, n);
}

int sem_signal_n(sem_t id, unsigned n) {
    return (int) syscall(SEM_SIGNAL_N, (uint64) id, n);
}

int time_sleep(time_t time) {
    return (int) syscall(TIME_SLEEP, time);
}

char getc() {
    return (char) syscall(GETC);
}

void putc(char chr) {
    syscall(PUTC, (uint64) chr);
}
