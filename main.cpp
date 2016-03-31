//
// Created by wonicon on 16-3-31.
//

#include "Memory.h"
#include "CPU.h"
#include <cstdio>

int main(int argc, char *argv[])
{
    Memory ram(100);
    CPU cpu(ram);
    cpu.exec(argv[1]);
    printf("cpu cycles: %d\n", cpu.getCycles());
    printf("mem access: %d\n", ram.getAccessTimes());
    return 0;
}