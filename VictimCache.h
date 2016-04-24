//
// Created by wonicon on 16-4-12.
//

#ifndef CACHESIM_VICTIMCACHE_H
#define CACHESIM_VICTIMCACHE_H

#include "Cache.h"

/**
 * @brief VictimCache - 只接收替换的 cache line
 *
 * 如何做到只接收替换出来的 cache line ？
 * 简单的做法是 read 请求造成的 miss 并不会影响 cache 的内容，
 * 只有 write 的会，而对 victim cache 的 write 显然是上层的 cache miss 导致的。
 *
 * 如果在前端选择了块 A 作为 victim, 然后在 victim cache 中找到了需要的块 B,
 * 则将二者交换，而不是纯粹地替换读取……
 */
class VictimCache : public Cache {
public:
    VictimCache(const char *name, unsigned int cache_size, unsigned block_size, Storage& depend);
    virtual int read(int addr, int size, CacheLine *block = nullptr);
    virtual int write(int addr, int size);
    virtual int accept(int addr, int data[], int size, bool is_dirty, bool is_victim = false, int cause = 0) override;
private:
    bool is_waiting_swap_ = false;
    int addr_to_swap_ = -1;
};


#endif //CACHESIM_VICTIMCACHE_H
