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

int Memory::read(int addr, int size, int &data)
{
    this->read_counter++;
    return latency;
}

int Memory::write(int addr, int size)
{
    this->write_counter++;
    return latency;
}