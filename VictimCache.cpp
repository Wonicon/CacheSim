//
// Created by wonicon on 16-4-12.
//

#include "debug.h"
#include "VictimCache.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>

VictimCache::VictimCache(const char *name, unsigned int cache_size, unsigned int block_size, Storage &depend, bool use_lru) :
Cache(name, cache_size, block_size, cache_size / block_size, 1, depend, use_lru)
{ }

int VictimCache::read(int addr, int size, CacheLine *block)
{
    count_rd_++;

    if (is_waiting_swap_) {
        int request = (addr >> offset_width_) << offset_width_;
        if (request == addr_to_swap_) {
            is_waiting_swap_ = false;
            if (block) {
                block->dirty = swap_line.dirty;  // 由于无备份，需要传递脏信息
                block->age = swap_line.age;
            }
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

    count_rd_miss_++;

    log("%6s  read miss at 0x%08x [tag %d index %d]",
            label_, addr, tag, index);

    // 既不载入、也不替换，相当于把访存转发给上层 cache
    return latency_ + depend_.read(addr, size);
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
                log("%6s finds 0x%08x to swap the evicted 0x%08x", label_, cause, addr);
                // 修改数据
                // Swap
                is_waiting_swap_ = true;
                addr_to_swap_ = (cause >> offset_width_) << offset_width_;
                swap_line = cache_line;
                this->extract(addr, tag, index, offset);
                //cache_line.tag = tag;
                cache_line.update_tag(tag);
                cache_line.dirty = is_dirty;
                return latency_;
            }
        }

        // Miss
    }

    count_wr_miss_++;

    // Victim cache does not load from backend.

    log("%6s  fail to find 0x%08x", label_, cause);

    int tag, index, offset;
    this->extract(addr, tag, index, offset);

    int latency = 0;
    int way = this->select_victim_way(index);

    if (ways_[way][index].valid) {
        log("%6s evict way %d [tag %d], which is %s",
                label_, way, ways_[way][index].tag, ways_[way][index].dirty ? "dirty" : "not dirty");
    }

    if (ways_[way][index].dirty) {
        latency = this->write_back_block(way, index);
    }

    ways_[way][index].valid = true;
    ways_[way][index].tag = tag;
    ways_[way][index].dirty = is_dirty;

    return latency_ + latency;
}
