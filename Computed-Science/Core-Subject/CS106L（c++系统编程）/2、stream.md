## Stream

> Streams provide a unified interface for interacting with external input.

你可以把 Stream 想象成一个字符缓冲区，它负责与外部数据源交互，并且能够自动完成字符与 C++ 数据类型之间的转换。

```c++
#include <iostream>
#include <sstream>
using namespace std;

int main() {
    istringstream ss("3.14 pi");

    double number;
    string word;

    ss >> number;
    ss >> word;

    cout << number << endl;
    cout << word << endl;
}
```

**外部数据源 → 字符缓冲 → Stream 格式化读取（**`>>`**）→ C++ 变量。**

这里有一个问题就是如何区分   >>  与 <<： [[c++中  >> 与 << 区别]]

以及 >> 和 << 为什么可以连续使用 [[c++中  >> 与 << 区别#^99bd3b]]


## stringstream

> A stringstream is not connected to any external source

**一句话概括：`stringstream` 是一种基于内存中字符串的输入输出流，它允许我们像使用 `cin` 和 `cout` 一样，对字符串进行读写和类型转换。

stringstream可以看作是istringstream和ostringstream的并集，之所以这两者还能存在是因为**接口设计应该尽可能明确地表达用途**。

## state bits
![[Pasted image 20261117163535.png]]
<div align="center">在 C++ 中，Stream 的 State Bits（状态位）是一组用于记录流当前状态的标志位，它们告诉程序：上一次输入输出操作是否成功、是否到达文件末尾、是否发生错误。</div>

```c++
#include <iostream>
#include <sstream>
using namespace std;

void printState(istream& stream) {
    cout << boolalpha;
    cout << "good: " << stream.good() << endl;
    cout << "eof:  " << stream.eof() << endl;
    cout << "fail: " << stream.fail() << endl;
    cout << "bad:  " << stream.bad() << endl;
}

int main() {
    istringstream ss("123 abc");
    int a, b;

    printState(ss);

    ss >> a;
    printState(ss);

    ss >> b;
    printState(ss);
}
```

| 状态        | 含义       | 触发场景     |     |
| --------- | -------- | -------- | --- |
| `goodbit` | 没有错误状态   | 流正常      |     |
| `eofbit`  | 到达输入末尾   | 读取遇到 EOF |     |
| `failbit` | 输入输出操作失败 | 类型转换失败   |     |
| `badbit`  | 严重的流错误   | 底层读写发生错误 |     |

