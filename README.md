# CacheSim

一个 cache 模拟器

## Build & Run

```
$ cd <path_to_repo>
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./CacheSim ../trace/gcc.trace
```

## Trace File

trace 文件描述了程序的访存行为，一行代表一次访存行为，格式如下：

```
[sl] <addr_value_in_hex> <interval_in_dec>

Examples:
l 0xcccccccc 10
s 0xaaaaaaaa 4
```

第三列表示距离上次访存的时钟周期数，不算访存自身的阻塞时间。
