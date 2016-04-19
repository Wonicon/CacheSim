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

int VictimCache::write(int addr, int size)
{
    return 0;
}

int VictimCache::accept(int addr, int data[], int size, bool is_dirty)
{
    count_wr_++;
    int tag, index, offset;
    this->extract(addr, tag, index, offset);

    for (int i = 0; i < n_ways_; i++) {
        auto& cache_line = ways_[i][index];
        if (cache_line.valid && (cache_line.tag == tag)) {
            // 修改数据
            cache_line.dirty = is_dirty;
            return latency_;
        }
    }

    // Miss
    count_wr_miss_++;
    int latency = 0;
    int way = this->select_victim_way(index);
    if (ways_[way][index].dirty) {
        latency = this->write_back_block(way, index);
    }

    ways_[way][index].valid = true;
    ways_[way][index].tag = tag;
    ways_[way][index].dirty = is_dirty;
    return latency_ + latency;
}
