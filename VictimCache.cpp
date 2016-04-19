//
// Created by wonicon on 16-4-12.
//

#include "VictimCache.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>

VictimCache::VictimCache(unsigned int cache_size, unsigned int block_size, Storage& depend) :
Cache(cache_size, block_size, cache_size / block_size, 1, depend)
{ }

int VictimCache::read(int addr, int size, int& data)
{
    if (is_waiting_swap_) {
        int request = (addr >> offset_width_) << offset_width_;
        if (request == addr_to_swap_) {
            is_waiting_swap_ = false;
            return latency_;
        }
        assert(0);
    }

    int tag, index, offset;
    this->extract(addr, tag, index, offset);

    for (int i = 0; i < n_ways_; i++) {
        auto& cache_line = ways_[i][index];
        if (cache_line.valid && (cache_line.tag == tag)) {
            // TODO 读取块内数据
            return latency_;
        }
    }

    // 既不载入、也不替换，相当于把访存转发给上层 cache
    return latency_ + depend_.read(addr, size, data);
}

int VictimCache::write(int addr, int size)
{
    return 0;
}

int VictimCache::accept(int addr, int data[], int size, bool is_dirty, bool is_victim, int cause)
{
    count_wr_++;

    if (is_victim) {
        int tag, index, offset;
        this->extract(cause, tag, index, offset);

        for (int i = 0; i < n_ways_; i++) {
            auto& cache_line = ways_[i][index];
            if (cache_line.valid && (cache_line.tag == tag)) {
                // 修改数据
                // Swap
                is_waiting_swap_ = true;
                addr_to_swap_ = (cause >> offset_width_) << offset_width_;
                this->extract(addr, tag, index, offset);
                cache_line.tag = tag;
                cache_line.dirty = is_dirty;
                return latency_;
            }
        }

        // Miss
    }

    count_wr_miss_++;

    // Victim cache does not load from backend.

    int tag, index, offset;
    this->extract(addr, tag, index, offset);

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
