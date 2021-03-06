//
// Created by wonicon on 16-4-8.
//

#include "Cache.h"
#include "debug.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

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

Cache::Cache(const char *name, unsigned int cache_size, unsigned int block_size, int n_ways, int latency, Storage &depend, bool use_lru) :
label_(name),
count_wr_(0),
count_rd_(0),
count_wb_(0),
count_wr_miss_(0),
count_rd_miss_(0),
n_ways_(n_ways),
latency_(latency),
offset_width_(log2(block_size)),
block_size_(block_size),
index_width_(log2(cache_size / block_size / n_ways)),
cache_depth_(cache_size / block_size / n_ways),
depend_(depend),
use_lru_(use_lru)
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
}

int Cache::read(int addr, int size, CacheLine *block)
{
    count_rd_++;

    int tag, index, offset;
    this->extract(addr, tag, index, offset);

    for (int i = 0; i < n_ways_; i++) {
        ways_[i][index].age++;
    }

    for (int i = 0; i < n_ways_; i++) {
        auto& cache_line = ways_[i][index];
        if (cache_line.valid && (cache_line.tag == tag)) {
            // TODO 读取块内数据
            if (block) {
                block->dirty = false;  // 由于有备份，无需传递脏信息
                block->age = 0;
            }
            return latency_;
        }
    }

    // Miss
    count_rd_miss_++;
    int way = this->select_victim_way(index);

    log("%6s  read miss at 0x%08x [tag %d index %d offset %d]",
            label_, addr, tag, index, offset);

    if (ways_[way][index].valid) {

        log("%6s evict way %d [tag %d], which is %s",
                label_, way, ways_[way][index].tag, ways_[way][index].dirty ? "dirty" : "not dirty");

        this->evict(way, index, addr);
    }

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
        ways_[i][index].age++;
    }

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
    int way = this->select_victim_way(index);

    log("%6s write miss at 0x%08x [tag %d index %d offset %d]",
            label_, addr, tag, index, offset);

    if (ways_[way][index].valid) {

        log("%6s evict way %d [tag %d], which is %s",
                label_, way, ways_[way][index].tag, ways_[way][index].dirty ? "dirty" : "not dirty");

        this->evict(way, index, addr);
    }

    if (ways_[way][index].dirty) {
        this->write_back_block(way, index);
    }
    int latency = this->load_block(way, addr);

    // TODO 修改块内数据
    ways_[way][index].dirty = true;
    if (!ways_[way][index].tag_history.empty()) *(ways_[way][index].tag_history.end() - 1) *= -1;
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
        latency = depend_.read(addr, block_size_, &ways_[way][index]);
    }
    else {
        fprintf(stderr, "bad way %d\n", way);
    }

    ways_[way][index].update_tag(tag);
    ways_[way][index].valid = true;
    return latency;
}

int Cache::write_back_block(int way, int line)
{
    count_wb_++;

    // 拼凑写回地址
    int addr = (ways_[way][line].tag << (index_width_ + offset_width_))
               + (line << offset_width_);

    log("%6s writes back 0x%08x [way %d]", label_, addr, way);
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

    if (use_lru_) {
        int max_way = 0;
        for (int i = 1; i < n_ways_; i++) {
            if (ways_[i][line].age > ways_[max_way][line].age) {
                max_way = i;
            }
        }
        return max_way;
    }
    else {
        // 然后是不脏的
        for (int i = 0; i < n_ways_; i++) {
            if (!ways_[i][line].dirty) {
                return i;
            }
        }
        return static_cast<unsigned>(rand()) % n_ways_;
    }
}

int Cache::evict(int way, int line, int cause)
{
    // 计算块地址
    int addr = (ways_[way][line].tag << (index_width_ + offset_width_)) + (line << offset_width_);
    return depend_.accept(addr, NULL, block_size_ / 4, ways_[way][line].dirty, true, cause);  // TODO Confirm element size!
}

void Cache::summary() const
{
    printf("%s access: %d\n", label_, count_rd_ + count_wr_);
    printf("%s access hit: %d\n", label_, count_rd_ + count_wr_ - count_rd_miss_ - count_wr_miss_);
    printf("%s loads: %d\n", label_, count_rd_);
    printf("%s loads hit: %d\n", label_, count_rd_ - count_rd_miss_);
    printf("%s stores: %d\n", label_, count_wr_);
    printf("%s stores hit: %d\n", label_, count_wr_ - count_wr_miss_);
    depend_.summary();
}
