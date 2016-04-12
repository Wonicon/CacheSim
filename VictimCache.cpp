//
// Created by wonicon on 16-4-12.
//

#include "VictimCache.h"
#include <cstdio>

VictimCache::VictimCache(unsigned int cache_size, unsigned int block_size, Storage& depend) :
Cache(cache_size, block_size, cache_size / block_size, 1, depend)
{ }

int VictimCache::read(int addr, int size, int& data)
{
    int tag, index, offset;
    this->extract(addr, tag, index, offset);

    count_rd_++;

    for (int i = 0; i < n_ways_; i++) {
        auto& cache_line = ways_[i][index];
        if (cache_line.valid && (cache_line.tag == tag)) {
            // TODO 读取块内数据
            return latency_;
        }
    }

    // Miss
    count_rd_miss_++;
    // 既不载入、也不替换，相当于把访存转发给上层 cache
    return latency_ + depend_.read(addr, size, data);
}