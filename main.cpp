//
// Created by wonicon on 16-3-31.
//

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
#if 1
    {
        printf("Test directly using ram\n>>>>>>>>>>>>>>>\n");
        Memory ram(100);
        CPU cpu(ram);
        cpu.exec(argv[1]);
        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
    }
    {
        printf("Test using L1 cache\n>>>>>>>>>>>>>>>\n");
        Memory ram(100);
        Cache L1(32 * 1024, 32, 4, 1, ram);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
        printf("miss rate %f\n", L1.get_miss_rate());
    }
    {
        printf("Test using L1 + L2 cache\n>>>>>>>>>>>>>>>\n");
        Memory ram(100);
        Cache L2(2 * 1024 * 1024, 128, 8, 10, ram);
        Cache L1(32 * 1024, 32, 4, 1, L2);
        CPU cpu(L1);
        cpu.exec(argv[1]);
        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
        printf("L2 miss rate: %f\n", L2.get_miss_rate());

        printf("L2 rd(miss): %d(%d)\n", L2.get_nr_read(), L2.get_nr_read_miss());
        printf("L2 wr(miss): %d(%d)\n", L2.get_nr_write(), L2.get_nr_write_miss());
        printf("L2 wb: %d\n", L2.get_nr_writeback());

        printf("L1 miss rate: %f\n", L1.get_miss_rate());

        printf("L1 rd(miss): %d(%d)\n", L1.get_nr_read(), L1.get_nr_read_miss());
        printf("L1 wr(miss): %d(%d)\n", L1.get_nr_write(), L1.get_nr_write_miss());
        printf("L1 wb: %d\n", L1.get_nr_writeback());
    }
    {
        printf("Test victim cache\n>>>>>>>>>>>>>>>\n");
        Memory ram(100);
        Cache L2(2 * 1024 * 1024, 128, 8, 10, ram);
        VictimCache victim(32 * 32, 32, L2);
        Cache L1(32 * 1024, 32, 4, 1, victim);
        CPU cpu(L1);
        cpu.exec(argv[1]);

        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
        printf("L2 miss rate: %f\n", L2.get_miss_rate());

        printf("L2 rd(miss): %d(%d)\n", L2.get_nr_read(), L2.get_nr_read_miss());
        printf("L2 wr(miss): %d(%d)\n", L2.get_nr_write(), L2.get_nr_write_miss());
        printf("L2 wb: %d\n", L2.get_nr_writeback());

        printf("victim miss rate: %f\n", victim.get_miss_rate());
        printf("vic rd %d(%d) wr %d(%d)\n",
               victim.get_nr_read(), victim.get_nr_read_miss(),
               victim.get_nr_write(), victim.get_nr_write_miss());
        printf("vic wb %d\n", victim.get_nr_writeback());

        printf("L1 miss rate: %f\n", L1.get_miss_rate());

        printf("L1 rd(miss): %d(%d)\n", L1.get_nr_read(), L1.get_nr_read_miss());
        printf("L1 wr(miss): %d(%d)\n", L1.get_nr_write(), L1.get_nr_write_miss());
        printf("L1 wb: %d\n", L1.get_nr_writeback());
    }
#else
    {
        Memory ram(100);
        Cache L2(2 * 1024 * 1024, 128, 8, 10, ram);
        VictimCache victim(32 * 32, 32, L2);
        Cache L1(16 * 256, 256, 1, 1, victim);
        CPU cpu(L1);
        cpu.exec(argv[1]);

        printf("cpu cycles: %d\n", cpu.getCycles());
        printf("mem access: %d\n", ram.getAccessTimes());
        printf("L2 miss rate: %f\n", L2.get_miss_rate());

        printf("L2 rd(miss): %d(%d)\n", L2.get_nr_read(), L2.get_nr_read_miss());
        printf("L2 wr(miss): %d(%d)\n", L2.get_nr_write(), L2.get_nr_write_miss());

        printf("victim miss rate: %f\n", victim.get_miss_rate());
        printf("L1 miss rate: %f\n", L1.get_miss_rate());
        printf("vic rd %d(%d) wr %d(%d)\n",
               victim.get_nr_read(), victim.get_nr_read_miss(),
               victim.get_nr_write(), victim.get_nr_write_miss());
    }
#endif
    return 0;
}
