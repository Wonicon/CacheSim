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
 */
class VictimCache : public Cache {
public:
    VictimCache(unsigned int cache_size, unsigned block_size, Storage& depend);
    virtual int read(int addr, int size, int& data);
private:
};


#endif //CACHESIM_VICTIMCACHE_H
