#ifndef PROJECT_BASE_V1_1_LIST_H
#define PROJECT_BASE_V1_1_LIST_H

#include "memoryAllocator.h"

template<typename T>
class List {
public:
    List() : head(nullptr), tail(nullptr) {}

    List(const List<T>&) = delete;

    List<T>& operator=(const List<T>&) = delete;

    bool isEmpty() const { return head == nullptr; }

    void addLast(T* data) {
        Elem* elem = new Elem(data, nullptr);
        if (elem == nullptr) return;
        if (tail != nullptr) {
            tail->next = elem;
            tail = elem;
        }
        else {
            head = tail = elem;
        }
    }

    T* removeFirst() {
        if (head == nullptr) return nullptr;

        Elem* elem = head;
        head = head->next;
        if (head == nullptr) tail = nullptr;

        T* data = elem->data;
        delete elem;
        return data;
    }

    T* peekFirst() const {
        if (head == nullptr) return nullptr;
        return head->data;
    }

private:
    struct Elem {
        T* data;
        Elem* next;

        Elem(T* data, Elem* next) : data(data), next(next) {}

        void* operator new(size_t size) noexcept { return MemoryAllocator::mem_alloc(size); }

        void operator delete(void* ptr) noexcept { MemoryAllocator::mem_free(ptr); }
    };

    Elem* head;
    Elem* tail;
};

#endif //PROJECT_BASE_V1_1_LIST_H
