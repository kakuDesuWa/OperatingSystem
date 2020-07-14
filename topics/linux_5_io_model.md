# I/O Model

Ref: https://notes.shichao.io/unp/ch6/#io-models

推荐: UNIX Network Programming 

重点: recvfrom 系统调用; I/O complete = data ready + copy to application memory

- [Block I/O Model](#block-io-model)
- [Non-Block I/O Model](#nonblocking-io-model)
- [I/O multiplexing (select and poll)](#io-multiplexing-model)
- [Signal-Driven I/O Model](#signal-driven-io-model)
- [Asynchronous I/O Model](#asynchronous-io-model)

There are normally two distinct phases for an input operation:

1. Waiting for the data to be ready. This involves waiting for data to arrive on the network. When packet arrives, it is copied into a buffer within the kernel.
2. Copying the data from the kernel to the process. This means copying the (ready) data from the kernel's buffer into our application buffer.

### Block I/O Model

最流行的I/O模型; 默认情况下, 所有sockets are blocking. The scenario is shown as below:

![block_io_model](https://notes.shichao.io/unp/figure_6.1.png)

上图是以 UDP 为例说明问题, UDP the concept of data being "ready" to read is simple: 要么整个数据报已经收到, 要么没有. TCP 则更为复杂...

上图中以 `recvfrom` 作为系统调用, 作为区分用户程序与内核边界...系统调用一般意味着:

There is normally a switch from running in the application to running in the kernel, followed at some time later by a return to the application.

正如上图所示: 重点在于 `recvfrom` 的调用 return until the datagram arrives and is copied into our application buffer, or an error occur. 此时应用程序继续执行.

谓之 "阻塞式 I/O 模型"

### Nonblocking I/O Model

上面说过, 默认socket是阻塞式的, 也即 read from socket(`recvfrom`)为阻塞式的;

当 socket is set to be nonblocking, we are telling kernel "when an I/O operation that I request cannot be completed without putting the process to sleep, do not put the process to sleep, but return an error instead", 如图:

![nonblock_io_model](https://notes.shichao.io/unp/figure_6.2.png)

- For the first three recvfrom, there is no data to return and the kernel immediately returns an error of EWOULDBLOCK.
- For the fourth time we call recvfrom, a datagram is ready, it is copied into our application buffer, and recvfrom returns successfully. We then process the data.

依赖于 application 的 polling 操作, 完成数据读取.

问题: 频繁的系统调用, this is ofen a waste of CPU time.

### I/O Multiplexing Model

在 I/O 多路复用中, 我们通过 `select` or `poll` and block in one of these system calls, 而非阻塞在一个 socket I/O system call. 即监听一组 I/O, 如图示:

![io_multiplexing_model](https://notes.shichao.io/unp/figure_6.3.png)

We block in a call to select, waiting for the datagram socket to be readable. When select returns that the socket is readable, we then call recvfrom to copy the datagram into our application buffer.

- Disadvantage: using select requires two system calls (select and recvfrom) instead of one
- Advantage: we can wait for more than one descriptor to be ready, `select`, `poll` 等可以监听一组file descriptor.

Another closely related I/O model is to use multithreading with blocking I/O. That model very closely resembles the model described above, except that instead of using select to block on multiple file descriptors, the program uses multiple threads (one per file descriptor), and each thread is then free to call blocking system calls like recvfrom.

### Signal-Driven I/O Model

通过让内核在描述符就绪时发送 SIGIO 信息通知我们. 称之为 信号驱动式I/O, 如图示:

![signal_driven_io_model](https://notes.shichao.io/unp/figure_6.4.png)

- We first enable the socket for signal-driven I/O (Section 25.2) and install a signal handler using the sigaction system call. The return from this system call is immediate and our process continues; it is not blocked.
- When the datagram is ready to be read, the SIGIO signal is generated for our process. We can either:
    - read the datagram from the signal handler by calling `recvfrom` and then notify the main loop that the data is ready to be processed (Section 25.3)
    - notify the main loop and let it read the datagram.

The advantage to this model is that we are not blocked while waiting for the datagram to arrive.
 
The main loop can continue executing and just wait to be notified by the signal handler that either the data is ready to process or the datagram is ready to be read.

### Asynchronous I/O Model

These functions work by telling the kernel to start the operation and to notify us when the entire operation (including the copy of the data from the kernel to our buffer) is complete. 

The main difference between this model and the signal-driven I/O model is that with signal-driven I/O, the kernel tells us when an I/O operation can be initiated, but with asynchronous I/O, the kernel tells us when an I/O operation is complete. See the figure below for example:

![asynchronous_io_model](https://notes.shichao.io/unp/figure_6.5.png)

异步I/O与其它I/O模型的差别在于 `recvfrom` 的调用与否, 或者说 kernel 是通知应用程序有I/O可读(就绪), 还是I/O已完成(已复制至 application buffer 中)

另外, 需要额外强调的是:

### Synchronous I/O versus Asynchronous I/O

POSIX defines these two terms as follows:

- A synchronous I/O operation causes the requesting process to be blocked until that I/O operation completes.
- An asynchronous I/O operation does not cause the requesting process to be blocked.

依据这个定义, 前4种I/O模型都可称为同步I/O, 因为始终需要有 `recvfrom` 从操作系统中读取I/O数据的过程(只是I/O就绪的时机不同); 只有最后一种I/O模型可以真正称为异步I/O.

Ref: ref: https://topic.alibabacloud.com/a/the-difference-between-java-bio-nio-and-aio-__java_1_27_20242144.html

what is blocking and non-blocking, what is synchronous and asynchronous?

synchronous and asynchronous are for application and kernel interaction:

- synchronization refers to the user process triggering IO operations and waiting or polling to see if the IO operation is ready
- Asynchronous means that the user process triggers the IO operation and then starts to do its own thing, and when the IO operation has been completed, it will be notified of IO completion.

And blocking and non-blocking are aimed at the process when accessing the data depending on how the IO operation is ready.

it is a way to read or write a function that reads or writes to the function, and the read or write functions will wait instead of blocking, and the read or write function immediately returns a status value.

就我的理解而言: 

block / non-block 是指调用结果(function is be called return or be holding?), 调用立即返回, 还是等待结果再返回?

sync / async 是指事件的处理方式(process way after trigger), A -> B -> A, 称为同步的方式处理事件B. 而 A->(trigger B)->A 称为异步的方式处理B, 触发B然后继续自己的事情(等或不等B完成通知) 称为异步的方式处理B, 触发B然后继续自己的事情(等或不等B完成通知).
