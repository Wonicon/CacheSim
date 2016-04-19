//
// Created by wonicon on 16-3-31.
//

#ifndef CACHESIM_STORAGE_H
#define CACHESIM_STORAGE_H


/**
 * @brief 存储器接口定义
 */
class Storage {
public:
    /**
     * @brief 存储器读操作
     * @param addr 访问地址
     * @param size 访问数据大小
     * @param data 接收读取的数据
     * @return 访存周期
     */
    virtual int read(int addr, int size, int& data) = 0;
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
