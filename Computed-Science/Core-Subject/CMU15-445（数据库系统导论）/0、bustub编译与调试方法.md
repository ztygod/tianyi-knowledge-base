主要用 BusTub 做测试，可以记住这个流程：**进入项目 → 编译测试目标 → 运行测试 → 必要时用 LLDB 调试。**

**1. 进入项目目录**

后面的命令都在这里执行：

```
cd /Users/tianyi/zhoutianyi-project/CMU15-445/bustub
```

**2. 配置构建环境：首次运行或删除 `build` 后执行**

下面是已经在这台 Mac 上验证过的配置：

```
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  "-DCMAKE_CXX_FLAGS=-Wno-error=deprecated-declarations -Wno-error=deprecated-literal-operator -Wno-error=unused-result"
```

`Debug` 提供调试信息，并启用 AddressSanitizer 检查内存错误。其余参数用于兼容本机较新的 CMake 和编译器。

**3. 修改代码后，编译对应测试**

Project 0 使用：

```
cmake --build build --target hyperloglog_test -j 4
```

这是增量编译，只重新编译受影响的部分。**不用每次重新配置，也不用编译整个项目。**

**4. 运行测试**

运行 Project 0 全部测试：

```
./build/test/hyperloglog_test --gtest_also_run_disabled_tests
```

只运行第一个基础测试，适合边写边测：

```
./build/test/hyperloglog_test \
  --gtest_also_run_disabled_tests \
  --gtest_filter=HyperLogLogTest.DISABLED_BasicTest1
```

只运行 Presto 相关测试：

```
./build/test/hyperloglog_test \
  --gtest_also_run_disabled_tests \
  --gtest_filter='*Presto*'
```

查看所有测试名称：

```
./build/test/hyperloglog_test --gtest_list_tests
```

当前测试名称都带 `DISABLED_`，所以需要 `--gtest_also_run_disabled_tests`。以后去掉测试名称里的这个前缀，就可以省略该参数。

**5. 测试失败时怎么看**

|输出|含义|
|---|---|
|编译出现 `error:`|代码尚未编译成功，先修复，再运行测试|
|`[ FAILED ]`|测试已经运行，但结果不符合预期|
|`0 tests`|没有实际执行测试，检查是否启用了禁用测试、筛选条件是否匹配|
|`AddressSanitizer` 报错|存在越界访问、释放后使用等内存错误，按调用栈定位|

**编译失败后不要继续运行旧测试程序**，否则测到的可能还是修改前的代码。

**6. 用 LLDB 打断点调试**

启动单个测试：

```
lldb -- ./build/test/hyperloglog_test \
  --gtest_also_run_disabled_tests \
  --gtest_filter=HyperLogLogTest.DISABLED_BasicTest1
```

进入 LLDB 后执行：

```
breakpoint set --func-regex 'HyperLogLog.*AddElem'
run
```

常用命令：

|命令|作用|
|---|---|
|`next`|执行下一行，不进入函数|
|`step`|执行下一步，可进入函数|
|`frame variable`|查看当前局部变量|
|`p cardinality_`|查看指定变量或表达式|
|`bt`|查看调用栈|
|`continue`|继续运行|
|`quit`|退出调试|

日常最常用的就是下面这组；`&&` 能保证编译成功后才运行测试：

```
cmake --build build --target hyperloglog_test -j 4 &&
./build/test/hyperloglog_test \
  --gtest_also_run_disabled_tests \
  --gtest_filter=HyperLogLogTest.DISABLED_BasicTest1
```