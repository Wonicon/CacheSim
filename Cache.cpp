//
// Created by wonicon on 16-4-8.
//

#include "Cache.h"
#include <cstdio>
#include <cstdlib>

/**
 * @brief 计算表示一个数的对数
 * @param x 待求解的数
 * @return 计算结果
 *
 * c++11 的 constexpr 只支持递归迭代（因为只能有一句 return）
 * 且是內联函数
 */
constexpr unsigned int log2(unsigned int x)
{
    return x == 1 ? 0 : 1 + log2(x >> 1);
}

Cache::Cache(unsigned int cache_size, unsigned int block_size, int n_ways, int latency, Storage &depend) :
count_wr_(0),
count_rd_(0),
count_wr_miss_(0),
count_rd_miss_(0),
n_ways_(n_ways),
latency_(latency),
offset_width_(log2(block_size)),
block_size_(block_size),
index_width_(log2(cache_size / block_size / n_ways)),
cache_depth_(cache_size / block_size / n_ways),
depend_(depend)
{
    if (n_ways_ <= 0) {
        fprintf(stderr, "n_ways(%d) should > 0\n", n_ways_);
        exit(-1);
    }

    ways_ = new CacheLine *[n_ways_];
    for (int i = 0; i < n_ways_; i++) {
        ways_[i] = new CacheLine[cache_depth_];
    }
}

Cache::~Cache()
{
    for (int i = 0; i < n_ways_; i++) {
        delete[] ways_[i];
    }
    delete[] ways_;
    ways_ = nullptr;
}

void Cache::extract(int addr, int &tag, int &index, int &offset)
{
    tag = (~((1 << (index_width_ + offset_width_)) - 1) & static_cast<unsigned>(addr)) >> (index_width_ + offset_width_);
    index = ((((1 << index_width_) - 1) << offset_width_) & static_cast<unsigned>(addr)) >> offset_width_;
    offset = ((1 << offset_width_) - 1) & addr;
    //printf("A(0x%08x) => %x | %x | %x\n", addr, tag, index, offset);
}

int Cache::read(int addr, int size, int& data)
{
    count_rd_++;

    int tag, index, offset;
    this->extract(addr, tag, index, offset);

    for (int i = 0; i < n_ways_; i++) {
        auto& cache_line = ways_[i][index];
        if (cache_line.valid && (cache_line.tag == tag)) {
            // TODO 读取块内数据
            return latency_;
        }
    }

    // Miss
    count_rd_miss_++;
    int way = this->select_victim_way(index);

    this->evict(way, index);

    if (ways_[way][index].dirty) {
        this->write_back_block(way, index);
    }

    int latency = this->load_block(way, addr);

    // TODO 读取块内数据
    return latency_ + latency;
}

int Cache::write(int addr, int size)
{
    count_wr_++;

    int tag, index, offset;
    this->extract(addr, tag, index, offset);

    for (int i = 0; i < n_ways_; i++) {
        auto& cache_line = ways_[i][index];
        if (cache_line.valid && (cache_line.tag == tag)) {
            // TODO 修改块内数据
            cache_line.dirty = true;
            return latency_;
        }
    }

    // Miss
    count_wr_miss_++;
    int latency = 0;
    int way = this->select_victim_way(index);

    latency += this->evict(way, index);
    if (ways_[way][index].dirty) {
        latency += this->write_back_block(way, index);
    }
    latency += this->load_block(way, addr);

    // TODO 修改块内数据
    ways_[way][index].dirty = true;
    return latency_ + latency;
}

int Cache::load_block(int way, int addr)
{
    int tag, index, offset;
    this->extract(addr, tag, index, offset);

    addr = addr & ~((1 << offset_width_) - 1);  // 按块对齐

    int latency = 0;

    if (0 <= way && way < n_ways_) {
        // 逐字写入
        int data;
        latency = depend_.read(addr, block_size_, data);
    }
    else {
        fprintf(stderr, "bad way %d\n", way);
    }

    ways_[way][index].tag = tag;
    ways_[way][index].valid = true;
    return latency;
}

int Cache::write_back_block(int way, int line)
{
    // 拼凑写回地址
    int addr = (ways_[way][line].tag << (index_width_ + offset_width_))
               + (line << offset_width_);

    // 逐字写回
    return depend_.write(addr, block_size_);
}

int Cache::select_victim_way(int line)
{
    // 优先找无效的
    for (int i = 0; i < n_ways_; i++) {
        if (!ways_[i][line].valid) {
            return i;
        }
    }
    // 然后是不脏的
    for (int i = 0; i < n_ways_; i++) {
        if (!ways_[i][line].dirty) {
            return i;
        }
    }

    return static_cast<unsigned>(rand()) % n_ways_;
}

int Cache::evict(int way, int line)
{
    // 计算块地址
    int addr = (ways_[way][line].tag << (index_width_ + offset_width_)) + (line << offset_width_);
    return depend_.accept(addr, NULL, block_size_ / 4, ways_[way][line].dirty);  // TODO Confirm element size!
}
