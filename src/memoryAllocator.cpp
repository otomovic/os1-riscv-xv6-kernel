//
// Created by os on 8/15/26.
//
#include "../h/memoryAllocator.h"
#include "../lib/console.h"

MemBlock* MemoryAllocator::freeBlocks = nullptr;
MemBlock* MemoryAllocator::allocatedBlocks = nullptr;

void MemoryAllocator::mem_start() {
    MemoryAllocator::freeBlocks = (MemBlock*)HEAP_START_ADDR;
    MemoryAllocator::freeBlocks->prev = nullptr;
    MemoryAllocator::freeBlocks->next = nullptr;
    MemoryAllocator::freeBlocks->size = (size_t)((char *) HEAP_END_ADDR - (size_t) HEAP_START_ADDR + 1);
    MemoryAllocator::allocatedBlocks = nullptr;
}

void* MemoryAllocator::mem_alloc(size_t size) {
    //poravnanje (velicina + zaglavlje na velicinu bloka)
    size_t blockSize = size + sizeof(MemBlock);
    if (blockSize % MEM_BLOCK_SIZE != 0)
        blockSize = (blockSize / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE + MEM_BLOCK_SIZE;

    MemBlock* temp = freeBlocks;
    while (temp != nullptr) {
        if (temp->size > blockSize) {
            temp->size -= blockSize;
            MemBlock* temp1 = temp;
            temp = reinterpret_cast<MemBlock*>(reinterpret_cast<char*>(temp1) + blockSize);
            temp->size = temp1->size;
            temp->prev = temp1->prev;
            temp->next = temp1->next;
            if (temp->prev == nullptr) freeBlocks = temp;
            if (temp->prev != nullptr) temp->prev->next = temp;
            if (temp->next != nullptr) temp->next->prev = temp;
            temp1->size = blockSize;
            if (allocatedBlocks != nullptr) {
                //ako je blok pre allocatedBlocks
                if (temp1 < allocatedBlocks) {
                    temp1->prev = nullptr;
                    temp1->next = allocatedBlocks;
                    allocatedBlocks->prev = temp1;
                    allocatedBlocks = temp1;
                    return reinterpret_cast<char *>(allocatedBlocks) + sizeof(MemBlock);
                }
                //ako je izmedju dva bloka ili posle svih
                for (MemBlock* tmp = allocatedBlocks; tmp != nullptr; tmp = tmp->next) {
                    if (temp1 > tmp) {
                        if (tmp->next != nullptr && temp1 < tmp->next) {
                            temp1->prev = tmp;
                            temp1->next = tmp->next;
                            tmp->next->prev = temp1;   // prvo iskoristi staru vrednost
                            tmp->next = temp1;
                            return reinterpret_cast<char *>(temp1) + sizeof(MemBlock);
                        }
                        else if (tmp->next == nullptr) {
                            temp1->prev = tmp;
                            temp1->next = nullptr;
                            tmp->next = temp1;
                            return reinterpret_cast<char *>(temp1) + sizeof(MemBlock);
                        }
                    }
                }
            }
            else {
                allocatedBlocks = temp1;
                temp1->prev = nullptr;
                temp1->next = nullptr;
                return reinterpret_cast<char *>(temp1) + sizeof(MemBlock);
            }
        }
        else if (temp->size >= blockSize) {
            if (temp->prev != nullptr) temp->prev->next = temp->next;
            if (temp->next != nullptr) temp->next->prev = temp->prev;
            MemBlock* temp1 = temp;
            if (temp->prev == nullptr) freeBlocks = temp->next;
            if (allocatedBlocks != nullptr) {
                //ako je blok pre allocatedBlocks
                if (temp1 < allocatedBlocks) {
                    temp1->prev = nullptr;
                    temp1->next = allocatedBlocks;
                    allocatedBlocks->prev = temp1;
                    allocatedBlocks = temp1;
                    return reinterpret_cast<char *>(allocatedBlocks) + sizeof(MemBlock);
                }
                //ako je izmedju dva bloka ili posle svih
                for (MemBlock* tmp = allocatedBlocks; tmp != nullptr; tmp = tmp->next) {
                    if (temp1 > tmp) {
                        if (tmp->next != nullptr && temp1 < tmp->next) {
                            temp1->prev = tmp;
                            temp1->next = tmp->next;
                            tmp->next->prev = temp1;   // prvo iskoristi staru vrednost
                            tmp->next = temp1;
                            return reinterpret_cast<char *>(temp1) + sizeof(MemBlock);
                        }
                        else if (tmp->next == nullptr) {
                            temp1->prev = tmp;
                            temp1->next = nullptr;
                            tmp->next = temp1;
                            return reinterpret_cast<char *>(temp1) + sizeof(MemBlock);
                        }
                    }
                }
            }
            else {
                allocatedBlocks = temp1;
                temp1->prev = nullptr;
                temp1->next = nullptr;
                return reinterpret_cast<char *>(temp1) + sizeof(MemBlock);
            }
        }
        temp = temp->next;
    }
    return nullptr;
}

int MemoryAllocator::mem_free(void *ptr) {
    if (!ptr) return -1;
    MemBlock* oldBlock = reinterpret_cast<MemBlock *>(static_cast<char *>(ptr) - sizeof(MemBlock));
    if (oldBlock == nullptr) return -1;
    if (oldBlock->prev != nullptr) oldBlock->prev->next = oldBlock->next;
    else allocatedBlocks = oldBlock->next;
    if (oldBlock->next != nullptr) oldBlock->next->prev = oldBlock->prev;
    if (freeBlocks == nullptr) {
        oldBlock->prev = nullptr;
        oldBlock->next = nullptr;
        freeBlocks = oldBlock;
        return 0;
    }
    //ako je pre prvog free bloka
    if (oldBlock < freeBlocks) {
        MemBlock* temp = freeBlocks;
        oldBlock -> prev = nullptr;
        oldBlock -> next = freeBlocks;
        freeBlocks->prev = oldBlock;
        freeBlocks = oldBlock;
        if (reinterpret_cast<MemBlock *>(reinterpret_cast<char *>(freeBlocks) + freeBlocks->size) == temp) {
            freeBlocks->next = temp->next;
            freeBlocks->size += temp->size;
            if (temp->next != nullptr) temp->next->prev = freeBlocks;
        }
        return 0;
    }
    else {
        for (MemBlock* tmp = freeBlocks; tmp != nullptr; tmp = tmp->next) {
            MemBlock* nextNode = tmp->next;   // <-- sačuvaj PRE mutacije
            if (oldBlock > tmp && nextNode != nullptr && oldBlock < nextNode) {
                oldBlock->prev = tmp;
                oldBlock->next = nextNode;
                if (oldBlock->next != nullptr)
                    oldBlock->next->prev = oldBlock;

                tmp->next = oldBlock;
                if (reinterpret_cast<MemBlock *>(reinterpret_cast<char *>(oldBlock) + oldBlock->size) == nextNode) {
                    MemBlock* r = nextNode;
                    oldBlock->size += r->size;
                    oldBlock->next = r->next;
                    if (r->next != nullptr) r->next->prev = oldBlock;
                    r->prev = nullptr;
                    r->next = nullptr;
                    r = nullptr;
                }
                if (reinterpret_cast<MemBlock *>(reinterpret_cast<char *>(tmp) + tmp->size) == oldBlock) {
                    tmp->next = oldBlock->next;
                    if (oldBlock->next) oldBlock->next->prev = tmp;
                    tmp->size += oldBlock->size;
                    oldBlock->next = nullptr;
                    oldBlock->prev = nullptr;
                    oldBlock = nullptr;
                }
                return 0;
            }
            else if (oldBlock > tmp && nextNode == nullptr) {
                oldBlock->prev = tmp;
                oldBlock->next = nullptr;
                tmp->next = oldBlock;
                if (reinterpret_cast<MemBlock *>(reinterpret_cast<char *>(tmp) + tmp->size) == oldBlock) {
                    tmp->size += oldBlock->size;
                    tmp->next = nullptr;
                    oldBlock->next = nullptr;
                    oldBlock->prev = nullptr;
                    oldBlock = nullptr;
                }
                return 0;
            }
        }
    }
    return -2;
}