#ifndef PROJECT_BASE_V1_1_SCHEDULER_H
#define PROJECT_BASE_V1_1_SCHEDULER_H

#include "list.h"

class TCB;

class Scheduler {
public:
    static TCB* get();
    static void put(TCB* tcb);

private:
    static List<TCB> readyThreadQueue;
};

#endif //PROJECT_BASE_V1_1_SCHEDULER_H
