//
// Created by wonicon on 16-3-31.
//

#ifndef CACHESIM_CPU_H
#define CACHESIM_CPU_H


#include "Storage.h"

/**
 * @brief 负责解析 trace 文件并执行访存操作
 *
 * 存储器采取引用依赖，而非指针占有。CPU 本身不输出存储器的统计信息，由外部定义的存储器本体来输出相关信息。
 */
class CPU {
public:
    /**
     * @brief 构造函数
     * @param 依赖的存储器
     */
    CPU(Storage& storage);
    /**
     * @brief 执行 trace 文件
     * @param trace 文件的路径
     */
    void exec(const char *trace);
    /**
     * @brief
     */
    int getCycles(void) const;
    /**
     * @brief 输出统计信息
     */
    void summary() const;
private:
    /**
     * @brief 依赖的存储器，抽象类型，内部可能有多层结构
     */
    Storage& mem;
    /**
     * @brief 时钟周期计数
     */
    int cycles;
    /**
     * @brief 指令计数
     */
    int nr_instrs_;
};


#endif //CACHESIM_CPU_H
