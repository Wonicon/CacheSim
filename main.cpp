//
// Created by wonicon on 16-3-31.
//

#include "debug.h"
#include "Cache.h"
#include "VictimCache.h"
#include "Memory.h"
#include "CPU.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (argc > 2) {
        logfile = fopen(argv[2], "w");
    }

    {
        printf("===== L1_1 64KB / 8B Direct =====\n");
        Memory ram(100);
        Cache L1("L1", 64 * 1024, 8, 1, 1, ram);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        cpu.summary();
    }
    {
        printf("===== L1_2 32KB / 32B / 4 ways LRU =====\n");
        Memory ram(100);
        Cache L1("L1", 32 * 1024, 32, 4, 1, ram, true);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        cpu.summary();
    }
    {
        printf("===== L1_3 8KB / 64B / Full Random =====\n");
        Memory ram(100);
        Cache L1("L1", 8 * 1024, 64, (8 * 1024 / 64), 1, ram);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        cpu.summary();
    }
    {
        printf("===== L1 + L2 =====\n");
        n_instr = 0;
        Memory ram(100);
        Cache L2("L2", 2 * 1024 * 1024, 128, 8, 10, ram, true);
        Cache L1("L1", 32 * 1024, 32, 4, 1, L2, true);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        cpu.summary();
    }
    {
        printf("===== L1 + Victim + L2 =====\n");
        n_instr = 0;
        Memory ram(100);
        Cache L2("L2", 2 * 1024 * 1024, 128, 8, 10, ram);
        VictimCache victim("Victim", 1 * 1024, 32, L2, true);
        Cache L1("L1", 32 * 1024, 32, 4, 1, victim);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        cpu.summary();
    }

    if (logfile != stdin) {
        fclose(logfile);
    }
    return 0;
}
