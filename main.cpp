//
// Created by wonicon on 16-3-31.
//

#include "Cache.h"
#include "VictimCache.h"
#include "Memory.h"
#include "CPU.h"
#include <cstdio>

int main(int argc, char *argv[])
{
    {
        printf("Test directly using ram\n");
        Memory ram(100);
        CPU cpu(ram);
        cpu.exec(argv[1]);
        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
    }
    {
        printf("Test using L1 cache\n");
        Memory ram(100);
        Cache L1(32 * 1024, 32, 4, 1, ram);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
        printf("miss rate %f\n", L1.get_miss_rate());
    }
    {
        printf("Test using L1 + L2 cache\n");
        Memory ram(100);
        Cache L2(2 * 1024 * 1024, 128, 8, 10, ram);
        Cache L1(32 * 1024, 32, 4, 1, L2);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
        printf("L2 miss rate: %f\n", L2.get_miss_rate());
        printf("L1 miss rate: %f\n", L1.get_miss_rate());
    }
    {
        printf("Test victim cache\n");
        Memory ram(100);
        Cache L2(2 * 1024 * 1024, 128, 8, 10, ram);
        VictimCache victim(32 * 32, 32, L2);
        Cache L1(32 * 1024, 32, 4, 1, victim);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
        printf("L2 miss rate: %f\n", L2.get_miss_rate());
        printf("victim miss rate: %f\n", victim.get_miss_rate());
        printf("L1 miss rate: %f\n", L1.get_miss_rate());
        printf("vic rd %d(%d) wr %d(%d)\n",
               victim.get_nr_read(), victim.get_nr_read_miss(),
               victim.get_nr_write(), victim.get_nr_write_miss());
    }
    return 0;
}
