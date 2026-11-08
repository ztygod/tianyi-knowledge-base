>  _**CPU 虚拟化**_解决“让多个进程共享 CPU”，核心是调度和上下文切换；
> 
> _**内存**_ _**虚拟化**_解决“让多个进程安全共享内存”，核心是虚拟地址、页表和地址转换。进程切换时，CPU 状态和内存地址空间通常都会一起切换。
> 
> CPU虚拟化与内存虚拟化是本节的两大主题

### 第四章 抽象：进程

> **进程 ≈** **内存****（虚拟****地址空间****） + 寄存器状态 +** **PCB****（进程控制块）**

分别对应：

- **内存**：保存程序代码、全局变量、堆、栈等，回答“这个进程有哪些数据和代码”。
    
- **寄存器状态**：比如 `eip/pc`、`esp`、通用寄存器等，回答“这个进程现在执行到哪、执行现场是什么”。
    
- **PCB**：操作系统管理进程需要的信息，比如 PID、进程状态、父进程、打开文件、调度信息、内存管理信息等。
    

```JavaScript
Process
│
├── 内存
│   ├── mem
│   ├── sz
│   ├── code
│   ├── heap
│   └── stack
│
├── 寄存器 / CPU 上下文
│   └── struct context
│       ├── eip
│       ├── esp
│       ├── ebx
│       ├── ecx
│       └── ...
│
└── PCB / struct proc
    ├── pid
    ├── state
    ├── parent
    ├── ofile
    ├── cwd
    ├── context
    └── ...
```

#### **进程概念**

> 进程：进程就是运行中的程序。
> 
> 机器状态（machine state）：进程在运行时可以读取或者更新的内容，有两个重要组成部分：内容与寄存器


机器状态：CPU 想要暂停这个进程，之后还能从原来的位置继续执行，所必须保存的那一整套状态。它是**上下文切换 Context** **Switch**能够发生的基础（时分共享）。


- **程序计数器** **PC / Instruction Pointer**：刚才执行到哪条指令了，下一条执行什么。
- **通用寄存器**：CPU 当前算到一半的数据、中间结果等。
- **栈指针** **SP**：当前函数调用栈在哪里。
- **状态/****标志寄存器**：比如上一次计算是否为 0、是否溢出等。

#### **进程创建**

进程的创建可以看作是操作系统的执行某一个程序

1. 操作系统现将代码和静态数据加载到内容中
2. 为程序的运行时栈（run-time stack或stack）分配内存，栈一般用于存放数据
3. 操作系统也可能为程序的堆（heap）分配一些内存，堆用于显示请求的动态分配内存
4. 其他的一些初始化任务

#### **进程状态**

- 运行（running）
- 就绪 （ready）
- 阻塞 （blocked）

进程状态的转换：

![](https://hcnz58qdrv4l.feishu.cn/space/api/box/stream/download/asynccode/?code=NTkwNDRiMWQ2MzQ5ZmEwYTQ0NWJhNGE4ZmNlOWMzZTlfbThiYTlzWm52YWZZVWw3OTRaeWxBUU55TXNsY2NISWVfVG9rZW46WFc3dmJRblk5b1BRbGh4NnQyU2N2dzY2bmVkXzE3OTQxMjY1OTk6MTc5NDEzMDE5OV9WNA&add_watermark=true&scene_type=CCM)

#### **进程数据结构**

一般保存在 **PCB（Process Control Block，进程控制块）** 中，包括：

```C++
// xv6 会保存和恢复这些寄存器，
// 用于暂停一个进程，并在之后重新恢复该进程的执行
struct context {
    int eip;
    int esp;
    int ebx;
    int ecx;
    int edx;
    int esi;
    int edi;
    int ebp;
};

// 进程可能处于的不同状态
enum proc_state {
    UNUSED,
    EMBRYO,
    SLEEPING,
    RUNNABLE,
    RUNNING,
    ZOMBIE
};

// xv6 为每个进程维护的信息，
// 包括它的寄存器上下文和进程状态
struct proc {
    char *mem;                   // 进程内存的起始地址
    uint sz;                     // 进程内存大小
    char *kstack;                // 当前进程内核栈的底部地址

    enum proc_state state;       // 进程状态
    int pid;                     // 进程 ID
    struct proc *parent;         // 父进程
    void *chan;                  // 如果非空，表示进程正在等待该 chan
    int killed;                  // 如果非 0，表示进程已经被要求终止
    struct file *ofile[NOFILE];  // 当前进程打开的文件
    struct inode *cwd;           // 当前工作目录
    struct context context;      // 上下文切换时，从这里恢复并运行进程
    struct trapframe *tf;        // 当前中断对应的陷阱帧
};
```

### 第五章 插叙：进程API

想要详细了解进程api在实际中作用可以阅读：[Shell重定向](https://hcnz58qdrv4l.feishu.cn/wiki/GgXswWUpyiNwSdkC1A7ccEYNnjh)

系统调用（System Call）就是：

> **用户程序向操作系统内核请求服务的接口。**

我们这里主要介绍三个进程api（系统调用）：fork，exec，wait；主要用于创建和控制进程。

#### `fork()`

> `fork()` 会基于当前进程创建一个新的子进程。父子进程都会从 `fork()` 的下一行继续执行，但 `fork()` 的返回值不同：
> 
> ```JavaScript
> 父进程：返回子进程的 PID
> 子进程：返回 0
> 失败：返回 -1
> ```

一般写法，注意：_**`fork()`**_ _**不是让子进程重新从**_ _**`main()`**_ _**开始执行，而是从**_ _**`fork()`**_ _**返回之后继续执行。就好像是它自己调用了fork()。**_

```JavaScript
int rc = fork();

if (rc < 0) {
    // 创建失败
} else if (rc == 0) {
    // 子进程
} else {
    // 父进程
}
```

#### `wait()`

> `wait()` 会阻塞父进程，直到某个子进程退出，然后返回这个子进程的 PID。

```C++
int rc = fork();

if (rc == 0) {
    printf("child\n");
    exit(0);
} else {
    wait(NULL); // 表示我只想等子进程结束，不关心它的退出状态。
    printf("parent\n");
}
```

子进程退出后，内核不会立刻把它所有信息都删掉，因为父进程可能还需要读取它的：

```Plain
PID
退出状态
资源使用信息
```

所以退出后的子进程会暂时成为：

> **僵尸进程（Zombie Process）**

父进程调用：

```Plain
wait();
```

之后，内核才真正完成回收。

所以 `wait()` 有两个核心作用：_**1. 等待子进程结束；2. 回收子进程，避免僵尸进程**_

#### `exec()`

> 它也是创建进程API的一个重要部分。这个系统调用可以让子进程执行与父进程不同的程序。简单来说就是，_**`exec`**_ _**不会创建新进程，而是用新程序替换当前进程的代码、数据和栈。**_

```C++
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int)getpid());

    int rc = fork();

    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int)getpid());

        char *myargs[3];
        myargs[0] = strdup("wc");    // program: "wc" (word count)
        myargs[1] = strdup("p3.c");  // argument: file to count
        myargs[2] = NULL;            // marks end of array

        execvp(myargs[0], myargs);   // runs word count

        printf("this shouldn't print out");
    } else {
        // parent goes down this path
        int wc = wait(NULL);

        printf(
            "hello, I am parent of %d (wc:%d) (pid:%d)\n",
            rc,
            wc,
            (int)getpid()
        );
    }

    return 0;
}

// 示例输出
prompt> ./p3
hello world (pid:29383)
hello, I am child (pid:29384)
29 107 1030 p3.c
hello, I am parent of 29384 (wc:29384) (pid:29383)
prompt>
```

最后，`fork()` 经常和 `exec()`、`wait()` 配合：

```JavaScript
fork
 ↓
创建子进程
 ↓
子进程 exec 新程序
 ↓
父进程 wait 等待子进程
```

---

### 第六章 机制：受限直接执行

> **简单来说就是：平时让用户程序直接在 CPU 上跑，保证性能；但通过**_**用户态/内核态、系统调用、中断和异常**_**等机制限制它，保证操作系统随时能拿回控制权。**

顺带一提：CPU 是真正“执行指令”的硬件，操作系统是负责管理 CPU、内存、磁盘、设备和进程的软件。程序就是一堆等待执行的指令，而 CPU真正执行这些指令的人

> _**CPU 是执行指令的硬件；操作系统是运行在 CPU 上、负责管理整个计算机资源的软件。**_
> 
> _**CPU 永远只是在执行某段代码，这段代码有时候属于用户程序，有时候属于操作系统内核。**_

**受限直接执行（Limited Direct Execution, LDE）** 是操作系统为了同时兼顾 **性能** 和 **控制权** 提出的一种运行程序的方式。受限直接执行带来了两个问题：

- 操作系统怎么能确保程序不做任何我们不希望它做的事，同时仍然高效地运行它
- 当我们运行一个进程时，操作系统如何让它停下来并切换到另一个进程，从而实现虚拟化CPU所需的时分共享？

#### 进程执行受限执行为（I/O读取）

一种方法是让我们进程执行他想执行的任何操作，这肯定能保证高效运行，但是如果完全不限制，用户进程就可能直接做这些事：随便访问别的进程内存 ；修改操作系统内存 ；直接操作磁盘、网卡等硬件，永远占着 CPU 不让别人运行。我们最终采取的方式是引入一种新的_**处理器模式：用户态/内核态**_。

- 用户态不能直接执行系统调用，比如I/O读取，文件读写，这样做会导致处理器引发异常，操作系统可能会终止进程。
- 内核态下代码（一般是操作系统的代码）**内核态代码拥有执行特权指令和访问受保护资源的权限**，包括一些特权事件和受限制的操作。


介绍完用户态和内核态之后，又出现了一个新的问题：如果我们用户态的代码想要去执行特权操作怎么办？

为了解决这个问题，几乎所有的现代硬件都提供了用户程序执行_**系统调用**_的能力。系统调用它允许内核小心地向用户程序暴露某些关键功能，例如访问文件系统、创建和销毁进程、与其他进程通信，以及分配更多内存。

- _**用户程序如果想请求操作系统帮它做事，就不能直接跳进内核代码，而要通过一种受控入口，也就是 trap。完成后，操作系统调用一个特殊的从陷阱返回（return-from-trap）指令**_
- _**进入陷阱后如何知道在OS内运行哪些代码：内核通过在启动时设置陷阱表（trap table）来实现**_

```Plain
用户程序
    |
    | read(...)
    v
C 库封装
    |
    | rax = read 的 syscall number
    | syscall
    v
================ 用户态 / 内核态 ================
    |
CPU 根据预先设置好的 syscall/trap 入口
    |
    v
系统调用入口代码
    |
    | 读取 syscall number
    v
syscall table
    |
    +---- 0 -> sys_read
    +---- 1 -> sys_write
    +---- 2 -> ...
    |
    v
sys_read()
    |
    v
执行内核代码
    |
    v
return-from-trap / sysret
================ 内核态 / 用户态 ================
    |
    v
用户程序继续执行
```

#### 在进程之间切换

进程切换看起来很简单，无非就是把一个进程的相关数据保存起来，之后加载执行另外一个进程。但是这里有一个盲点：假设我们只有一个cpu，当我们在执行另外一个进程的时候，操作系统的代码是没有在执行的，那他是怎么实现进程的切换的呢。如果操作系统没有运行，它怎么能做事情？

事实证明，如果不通过硬件支持，假设用户程序拒绝执行系统调用来保证控制权不会交给操作系统，那么操作系统将做不成任何事情。

_**最终的答案很简单：时钟中断（timer interrupt）。时钟设备可以编程为每隔几毫秒产生一次中断。产生中断时，当前正在运行的进程停止，操作系统中预先配置的中断处理程序（interrupt handler）会运行。在此时，中断处理程序保存和恢复上下文。操作系统重新获得CPU的控制权，因此可以做它想做的事：停止当前进程，并启动另一个进程**_。

---

### 第七章：进程调度：介绍 / 第八章：调度：多级反馈队列

进程调度的根本原因就是：可运行的进程很多，但 CPU 核心数量有限，一个CPU核同一时刻实际上只能执行一个进程，操作系统必须决定谁先运行、运行多久。操作系统必须要解决A 运行多久？ 下一个运行 B 还是 C？ 高优先级任务要不要插队？ 某个任务一直占 CPU 怎么办？

#### 调度指标

我们必须通过一些指标来了解我们调度策略的性能：

- 周转时间（turnaround time）：任务的周转时间定义为**任务完成时间减去任务到达系统的时间**。更正式的周转时间定义T周转时间是：_**T周转时间= T完成时间−T到达时间**_　
- 响应时间（response time）：响应时间定义为从**任务到达系统到首次运行的时间**。更正式的定义是：_**T响应时间= T首次运行−T到达时间**_

#### 工作负载（关于进程的简单假设）

我们对操作系统中运行的进程（有时也叫工作任务）做出如下的假设：

1. 每一个工作运行相同的时间。
2. 所有的工作同时到达。
3. 一旦开始，每个工作保持运行直到完成。
4. 所有的工作只是用CPU（即它们不执行IO操作）
5. 每个工作的运行时间是已知的。

> 很遗憾这里面的每一个假设在真实的操作系统中都是不存在的，接下来我将逐一介绍进程调度策略来解决工作负载缺失所带来的问题

#### 先进先出（FIFO）

先假设下图的ABC同时到达

先进先出（FIFO）也被称为先到先服务，顾名思义没有任何理解的困难。问题也很明显：一些耗时较少的潜在资源消费者被排在重量级的资源消费者之后，导致护航效应。

A先运行100s，B或C才有机会运行。因此，系统的平均周转时间是比较高的：令人不快的110s（（100 + 110 + 120）/ 3 = 110）

![](https://hcnz58qdrv4l.feishu.cn/space/api/box/stream/download/asynccode/?code=ZjUxNzc2NzlhYjI4MzFkMDVlMjk1MGI2OTQ4MjQ0NmFfNVFLTEVuWTdlR1VTaE00bmhIOTl2cVhnQTV0Q0lTQ3ZfVG9rZW46VG5yRGJwU3Fsb2Viem54MU93VGNySU1QbnFmXzE3OTQxMjY1OTk6MTc5NDEzMDE5OV9WNA&add_watermark=true&scene_type=CCM)

#### 最短任务优先（SJF）

一个很简单的策略解决了这个问题：先运行最短的任务，然后是次短的任务，如此下去。仅通过在A之前运行B和C，SJF将平均周转时间从110s降低到50s（（10 + 20 + 120）/3 = 50）。

![](https://hcnz58qdrv4l.feishu.cn/space/api/box/stream/download/asynccode/?code=MWUzZDY2NzVkOTUyZTA1YzM2ZTVlYzJlYTY1Njk5M2RfWlhkcko3SWJrdm9KMUE5MXFmcnl6czllcGFKcWxvaXdfVG9rZW46Q2FQT2I3VWZVb0VwRmF4bnpHamNHUWFEbk1oXzE3OTQxMjY1OTk6MTc5NDEzMDE5OV9WNA&add_watermark=true&scene_type=CCM)

#### 最短完成时间优先（STCF）

现在又有一个很明显的问题了，如果ABC不是在同一时间到达呢：假设A在t = 0时到达，且需要运行100s。而B和C在t = 10到达，且各需要运行10s。用纯SJF，我们可以一下调度。

![](https://hcnz58qdrv4l.feishu.cn/space/api/box/stream/download/asynccode/?code=NTRlMmUxNjk4NzE1ZTkwNGRmNjllZWNkYTkzMmVmMDZfMDg0aWxtRDRtcUpkVUV6Z3J6RmhJSEFpdzNtVExWQVBfVG9rZW46UmU0OGJ6VHV2bzdYNlJ4UUw1d2M4aWx2bmpiXzE3OTQxMjY1OTk6MTc5NDEzMDE5OV9WNA&add_watermark=true&scene_type=CCM)

即使B和C在A之后不久到达，它们仍然被迫等到A完成，从而遭遇同样的护航问题。这3项工作的平均周转时间为103.33s，即（100+（110−10）+（120−10））/3。

解决这个问题，也就是解决我们在工作负载所做的假设2，_**并不是所有任务都在同一个时刻到达**_。

幸运的是，我们有解决问题的调度方式：_**向SJF添加抢占，称为最短完成时间优先（Shortest Time-to-Completion First，STCF）或抢占式最短作业优先（Preemptive Shortest Job First ，PSJF）调度程序**_。每当新工作进入系统时，它就会确定剩余工作和新工作中，谁的剩余时间最少，然后调度该工作。因此，在我们的例子中，STCF将抢占A并运行B和C以完成。只有在它们完成后，才能调度A的剩余时间。

#### 轮转（RR）

虽然上面的方法在平均周期时间上表现很好，但是在响应时间上可以说是表现平平：例如，如果3个工作同时到达，第三个工作必须等待前两个工作全部运行后才能运行。这种方法虽然有很好的周转时间，但对于响应时间和交互性是相当糟糕的。

所幸我们提出了轮转：_**基本思想很简单：RR在一个时间片（time slice，有时称为调度量子，scheduling quantum）内运行一个工作，然后切换到运行队列中的下一个任务，而不是运行一个任务直到结束。它反复执行，直到所有任务完成**_。

> 这在无形中打破了工作负载第三条的限制

#### 重叠（overlap）结合 I/O

一个进程在事件中不可能不处理I/O事件（工作负载第四条）

调度程序显然要在工作发起I/O请求时做出决定_**，因为当前正在运行的作业在I/O期间不会使用CPU**_，它被阻塞等待I/O完成。因此，这时调度程序应该在CPU上安排另一项工作。

对比：

![](https://hcnz58qdrv4l.feishu.cn/space/api/box/stream/download/asynccode/?code=YWMxNjBlZDFlYzk4MWYzMzk1OTM0NTc5NGFlOTI4MDZfb1dNdEJyTVpzQmxmNzVTZkg2amo4c2VSbXNNcG0zamxfVG9rZW46RmFCSmJsaGthbzU2RXF4UlRZT2NaQUpEbktjXzE3OTQxMjY1OTk6MTc5NDEzMDE5OV9WNA&add_watermark=true&scene_type=CCM)

后：

![](https://hcnz58qdrv4l.feishu.cn/space/api/box/stream/download/asynccode/?code=MmU3MGVjMTlhMzA2ZThjMmNjNzVjNzAzZjQ4Njg3M2VfaUZuZkl6U3JHVFlHSUNNOWg4OGNyeGN4RThiQWNNbG9fVG9rZW46TnFnMWJ2OUhvb2dxRFZ4bmlsM2NDUHl0bklnXzE3OTQxMjY1OTk6MTc5NDEzMDE5OV9WNA&add_watermark=true&scene_type=CCM)

#### 多级反馈队列（Multi-level Feedback Queue，MLFQ）

接下来我们来到最后一个假设：我们事先就知道了每一个进程总的的执行所需时间，这在实际中几乎是不可能的。

多级反馈队列主要解决两个问题：

- 优化周转时间。在第7章中我们看到，这通过先执行短工作来实现。然而，操作系统通常不知道工作要运行多久，而这又是SJF（或STCF）等算法所必需的
- MLFQ希望给交互用户（如用户坐在屏幕前，等着进程结束）很好的交互体验，因此需要降低响应时间。然而，像轮转这样的算法虽然降低了响应时间，周转时间却很差。

_**归根到底，在实际运行中我们对进程一无所知，我们需要在执行过程中去学习进程的特征，从而做出更好的调度决策**_。

MLFQ基本准则：

MLFQ中有许多独立的队列（queue），每个队列有不同的优先级（priority level）。任何时刻，一个工作只能存在于一个队列中。MLFQ总是优先执行较高优先级的工作（即在较高级队列中的工作）。

- 规则1：如果A的优先级 > B的优先级，运行A（不运行B）。
- 规则2：如果A的优先级 = B的优先级，轮转运行A和B。
- 规则3：工作进入系统时，放在最高优先级（最上层队列）。
- 规则4：一旦工作用完了其在某一层中的时间配额（无论中间主动放弃了多少次CPU），就降低其优先级（移入低一级队列）。
- 规则5：经过一段时间S，就将系统中所有工作重新加入最高优先级队列。（防止饥饿）

---

### 第九章：调度：比例份额

比例份额调度不是直接追求“谁先运行、谁等待最短”，而是希望让每个进程按照预先分配的比例获得 CPU 时间。_**主要关注的是进程调度的公平性**_

之前学的 FIFO、SJF、RR，思路基本都是：现在应该选哪个进程运行？

而比例份额调度换了一个角度：每个进程应该获得多少 CPU？

#### 彩票调度（lottery scheduling）

> 给进程发彩票，谁的彩票多，谁被抽中的概率就高。

彩票数（ticket）代表了进程（或用户或其他）占有某个资源的份额。一个进程拥有的彩票数占总彩票数的百分比，就是它占有资源的份额。

下面来看一个例子。假设有两个进程A和B，A拥有75张彩票，B拥有25张。因此我们希望A占用75%的CPU时间，而B占用25%。通过不断定时地（比如，每个时间片）抽取彩票，彩票调度从概率上（但不是确定的）获得这种份额比例。

彩票调度的实现：

```C
winner = random(total_tickets);

counter = 0;

for each process p:
    counter += p.tickets;

    if counter > winner:
        run(p);
        break;
```

而彩票调度的缺陷也很明显：

- 在短期调度的情况下，因为随机性的原因导致最终调度结果不是按照比例来的
    
- 此外，系统的运行严重依赖于彩票的分配。对于给定的一组工作，彩票分配的问题依然没有最佳答案。
    

#### 步长调度（stride scheduling）

> Stride Scheduling 是为了弥补 Lottery 的随机性。不再随机抽彩票，而是用数学方式确定谁该运行。

系统中的每个工作都有自己的步长，这个值与票数值成反比。在上面的例子中，A、B、C这3个工作的票数分别是100、50和250，_**我们通过用一个大数分别除以他们的票数来获得每个进程的步长。比如用10000除以这些票数值，得到了3个进程的步长分别为100、200和40**_。我们称这个值为每个进程的_**步长（stride）**_。每次进程运行后，我们会让它的_**计数器 [称为行程（pass）值] 增加它的步长**_，记录它的总体进展。

调度方式很简单：当需要进行调度时，选择目前拥有_**最小行程值（pass）**_的进程，并且在运行之后将该进程的行程值增加一个步长。

当然步长调度也有他的缺点：如果有一个新加入的进程我们该怎么设置他的行程值，如果pass设置为0那他就相当于独占CPU了。而彩票调度有一个步长调度没有的优势——不需要全局状态。

虽然这两个比例份额调度方式在公平性上十分有效，但是在实际的CPU调度中没有被采用，_**一个原因是其中最难的票数分配问题并没有确定的解决方式，另外一个原因就是这两种方式都不太适合I/O。**_

### 第十章：多处理器调度

看不太懂，等我学完并发部分再来看