//
// Created by wonicon on 16-3-31.
//

#ifndef CACHESIM_STORAGE_H
#define CACHESIM_STORAGE_H

#include <vector>

/**
 * @brief 描述由组索引的一行 cache block 所具有的信息
 */
struct CacheLine {
    int tag;           /**< 标签 */
    bool valid;        /**< 有效位 */
    bool dirty;        /**< 脏位 */
    int age;           /**< LRU，每次访问都会增长，每次访问命中都不会增长*/
    std::vector<int> tag_history;
    CacheLine(): tag(0), valid(false), dirty(false), age(0) { }
    void update_tag(int new_tag) { if (valid) tag_history.push_back(tag); tag = new_tag; }
};

/**
 * @brief 存储器接口定义
 */
class Storage {
public:
    /**
     * @brief 存储器读操作
     * @param addr 访问地址
     * @param size 访问数据大小
     * @param block 向上传递块信息
     * @return 访存周期
     */
    virtual int read(int addr, int size, CacheLine *block = nullptr) = 0;
    /**
     * @brief 存储器写操作
     * @param addr 访问地址
     * @param size 访问数据大小
     * @return 访存周期
     */
    virtual int write(int addr, int size) = 0;
    /**
     * @brief 存储器接收写回数据，大部分情况下不处理
     *
     * TODO 用这个接口来实现写回行为
     */
    virtual int accept(int addr, int data[], int size, bool is_dirty, bool is_victim = false, int cause = 0) { return 0; }
};


#endif //CACHESIM_STORAGE_H
