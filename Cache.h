//
// Created by wonicon on 16-4-8.
//

#ifndef CACHESIM_CACHE_H
#define CACHESIM_CACHE_H


#include "Storage.h"

class Cache : public Storage {
public:
    /**
     * @brief Cache 的构造函数
     * @param cache_size cache 的 data 区总大小，单位字节
     * @param block_size cache 块的大小，单位字节
     * @param n_ways 路数
     * @param lantency hit 的延迟，单位时钟周期
     * @param mem 依赖的外部存储
     */
    Cache(unsigned int cache_size, unsigned int block_size, int n_ways, int latency, Storage &depend);
    virtual ~Cache();
    virtual int read(int addr, int size, int& data);
    virtual int write(int addr, int size);
    double get_miss_rate() const
    { return (count_rd_miss_ + count_wr_miss_) * 100.0 / (count_wr_ + count_rd_); }
protected:
    /**
     * @brief 描述由组索引的一行 cache block 所具有的信息
     */
    struct CacheLine {
        int tag;           /**< 标签 */
        bool valid;        /**< 有效位 */
        bool dirty;        /**< 脏位 */
        CacheLine(): tag(0), valid(false), dirty(false) { }
    };
    /**
     * @brief 提取地址信息
     * @param addr 地址
     * @param tag 输出，标签
     * @param index 输出，组索引
     * @param offset 输出，块内偏移
     */
    void extract(int addr, int& tag, int& index, int& offset);
private:
    int count_wr_;               /**< 统计写请求次数 */
    int count_rd_;               /**< 统计读请求次数 */
    int count_wr_miss_;
    int count_rd_miss_;
    const int latency_;          /**< 延迟 */
    const int index_width_;      /**< 组索引宽度 */
    const int offset_width_;     /**< 块内偏移宽度 */
    const int n_ways_;           /**< 路数 */
    const int block_size_;       /**< 块大小，字节单位 */
    const int cache_depth_;      /**< 组数 */
    CacheLine **ways_;           /**< 优先检索路的 data array */
    Storage& depend_;            /**< 被缓存存储器对象 */
    int load_block(int way, int addr);
    int write_back_block(int way, int line);
    int select_victim_way(int line);
};

#endif //CACHESIM_CACHE_H
