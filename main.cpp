//
// Created by wonicon on 16-3-31.
//

#include "Cache.h"
#include "Memory.h"
#include "CPU.h"
#include <cstdio>

int main(int argc, char *argv[])
{
    {
        Memory ram(100);
        Cache cache(128 * 1024, 8 * 4, 4, 10, ram);
        CPU cpu(cache);
        cpu.exec(argv[1]);
        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
        printf("miss rate %f\n", cache.get_miss_rate());
    }
    {
        Memory ram(100);
        CPU cpu(ram);
        cpu.exec(argv[1]);
        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
    }
    return 0;
}