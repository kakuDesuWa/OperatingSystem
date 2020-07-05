# Q&A

## Overview

本部分主要是问&答的集合, 一般来说, 是理解模块知识的基础上的综合, 概述性的描述.

## Catalogs

- [应用程序如何访问内存](#应用程序如何访问内存)


### 应用程序如何访问内存

应用程序是如何访问内存的?

初步的概括性回答:

**这只是初步的概述, 缺少细节和准确性! 更详细的内容参见本仓库关于深入理解操作系统中的总结**

要说清楚这个问题, 有三个基础知识需要明白:

1. 存储器层次结构. 2. 异常控制流. 3. 虚拟内存

首先看存储器层次结构:

1. 由上至下(由快至慢, 由大至小) CPU三级缓存 -> 主存 -> 磁盘 -> 网络存储空间.
2. 上一层存储是下一层存储的高速缓存! (这个核心观点对于理解虚拟内存的行为很重要!)

再看虚拟内存:

基于`上一层存储是下一层存储的高速缓存` & 不同存储层次的特点 -> CPU不直接访问磁盘存储空间的. CPU通过"虚拟内存地址"访问主存中的数据.

站在虚拟内存的视角来看, 主存是虚拟内存的高速缓存.

最后是异常控制流:

CPU拿到应用程序中的虚拟内存地址, 通过MMU(Memory Management Unit), 利用存放在主存的查询表(页表)查询实际主存的物理地址. 若在查询表(页表)中查询不在缓存中或未分配(细节参见虚拟内存的三种状态)(缺页), 触发缺页异常(然后调用内核中的缺页异常处理程序, 以页的方式再加载进主存), 异常返回(继续执行被中断的指令)时, 该虚拟地址已被加载, 随后正常访问主存数据.