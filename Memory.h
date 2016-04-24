//
// Created by wonicon on 16-3-31.
//

#ifndef CACHESIM_MEMORY_H
#define CACHESIM_MEMORY_H


#include "Storage.h"

/**
 * @brief 理想存储器
 */
class Memory : public Storage {
public:
    Memory(int latency);
    virtual int read(int addr, int size, CacheLine *block = nullptr);
    virtual int write(int addr, int size);
    int getAccessTimes(void) const { return read_counter + write_counter; }
private:
    int latency;
    int read_counter;
    int write_counter;
};


#endif //CACHESIM_MEMORY_H
