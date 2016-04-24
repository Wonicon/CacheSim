//
// Created by wonicon on 16-3-31.
//

#include "Memory.h"
#include <iostream>
#include <cassert>

Memory::Memory(int latency) :
latency(latency),
read_counter(0),
write_counter(0)
{}

int Memory::read(int addr, int size, CacheLine *block)
{
    this->read_counter++;
    if (block) {
        block->dirty = false;  // 由于有备份，无需传递脏信息
    }
    return latency;
}

int Memory::write(int addr, int size)
{
    this->write_counter++;
    return latency;
}

void Memory::summary() const
{
    printf("Memory access: %d\n", read_counter + write_counter);
    printf("Memory loads: %d\n", read_counter);
    printf("Memory stores: %d\n", write_counter);
}