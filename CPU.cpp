//
// Created by wonicon on 16-3-31.
//

#include "debug.h"
#include "CPU.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>

CPU::CPU(Storage &storage) :
mem(storage),
cycles(0),
nr_instrs_(0)
{}

void CPU::exec(const char *trace_file)
{
    FILE *     trace;       // 输入文件流
    char       method;      // 访存方法
    int        addr;        // 地址，在文件中是 16 进制
    int        interval;    // 与上一次访存操作的时钟周期间隔，在 文件中是 10 进制
    int        data;        // 接收读数据的缓冲区
    int        no = 0;      // 循环计数
    const char LOAD  = 'l';
    const char STORE = 's';
    const int  SIZE = 4;

    if ((trace = fopen(trace_file, "r")) == NULL) {
        perror(trace_file);
        exit(-1);
    }

    // 执行主循环
    // 本来想用 std::hex 进行格式化，但是 std::hex 似乎会在超过 int 正数返回后导致 cin 失败
    while (fscanf(trace, "%s%x%d", &method, &addr, &interval) != EOF) {
        no++;
        this->cycles += interval;
        nr_instrs_++;
        int latency = 0;
        if (method == LOAD) {
            latency = this->mem.read(addr, SIZE);
        }
        else if (method == STORE) {
            latency = this->mem.write(addr, SIZE);
        }
        else {
            fprintf(stderr, "Unexpected method: %c\n", method);
            assert(0);
        }
        this->cycles += latency;
    }
}

int CPU::getCycles() const
{
    return cycles;
}

void CPU::summary() const
{
    printf("CPU instrs: %d\n", nr_instrs_);
    printf("CPU cycles: %d\n", cycles);
    mem.summary();
}
