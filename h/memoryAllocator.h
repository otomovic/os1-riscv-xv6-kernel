//
// Created by os on 8/15/26.
//
#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
#include "../lib/hw.h"

struct MemBlock {
    MemBlock* prev;
    MemBlock* next;
    size_t size;
};

class MemoryAllocator {
private:
public:
    static MemBlock* freeBlocks;
    static MemBlock* allocatedBlocks;
    static void* mem_alloc(size_t size);
    static int mem_free(void* ptr);
    static void mem_start();
};


#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_H