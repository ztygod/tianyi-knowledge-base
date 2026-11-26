## 循环

### 三种基本循环

```java
// for
for (int i = 0; i < 5; i++) {
    System.out.println(i);
}

// while
int i = 0;
while (i < 5) {
    System.out.println(i++);
}

// do-while
int j = 0;
do {
    System.out.println(j++);
} while (j < 5);
```
![[Pasted image 20261125093100.png]]
### ### **增强 for 循环（Enhanced For Loop）**


Java 提供了类似 C++ Range-based for 和 JavaScript `for...of` 的语法。

```java
int[] nums = {10, 20, 30};

for (int num : nums) {
    System.out.println(num);
}
```

对于数组，上面的代码基本等价于：

```java
for (int i = 0; i < nums.length; i++) {
    int num = nums[i];
    System.out.println(num);
}
```

需要特别注意，下面的代码并不会修改数组中的元素。

```java
int[] nums = {1, 2, 3};
for (int num : nums) {
    num *= 2;
}
System.out.println(Arrays.toString(nums)); // [1, 2, 3]
```

因为 `num` 保存的是数组元素值的副本。

正确的修改方式是：

```java
for (int i = 0; i < nums.length; i++) {
    nums[i] *= 2;
}
```

如果遍历的是对象数组，则可以通过复制得到的对象引用修改对象内部状态，但重新赋值循环变量仍然不会改变原数组中的引用。

## **Java 数组（Array）**

Java 中的数组是一种**长度固定、元素类型统一、支持索引访问的对象**。

### 初始化

常见的三种初始化方式：

```java
// 指定长度
int[] a = new int[5];

// 指定初始元素
int[] b = {1, 2, 3, 4, 5};

// 显式创建数组对象
int[] c = new int[]{1, 2, 3, 4, 5};
```

数组创建之后，其长度就不能再改变。并且Java 创建数组时，会自动对数组元素进行初始化。

### **数组的引用模型与内存结构**

#### **数组本质上是对象**

```java
int[] nums = new int[]{10, 20, 30, 40};
```

从 JVM 的抽象内存模型来看，这行代码包含两个部分：

- 局部变量 `nums` 保存数组对象的引用。
- 数组对象包含长度、元素以及运行时所需的元数据。

#### **数组赋值与数组复制**

```java
int[] a = {1, 2, 3};
int[] b = a;

b[0] = 100;

System.out.println(a[0]); // 100
System.out.println(a == b); // true
```

`int[] b = a` 只是复制引用，不会创建新数组。这与 JavaScript 中的数组赋值行为基本一致。

#### Java 提供的复制方法

**方式一：手动复制**

```java
int[] a = {1, 2, 3};
int[] b = new int[a.length];

for (int i = 0; i < a.length; i++) {
    b[i] = a[i];
}
```

**方式二：clone()**

```java
int[] a = {1, 2, 3};
int[] b = a.clone();
```

**方式三：Arrays.copyOf()**

```java
import java.util.Arrays;

int[] a = {1, 2, 3};
int[] b = Arrays.copyOf(a, a.length);
```

`Arrays.copyOf()` 还可以指定新数组的长度。

```java
int[] a = {1, 2, 3};
int[] b = Arrays.copyOf(a, 5);

// [1, 2, 3, 0, 0]
```

## **Arrays 工具类**

Java 标准库提供了 `java.util.Arrays`，用于简化数组操作。

```java
import java.util.Arrays;

int[] nums = {5, 3, 1, 4, 2};

Arrays.sort(nums);
System.out.println(Arrays.toString(nums));

// [1, 2, 3, 4, 5]
```

常用方法：

|方法|作用|
|---|---|
|`Arrays.sort(arr)`|数组排序|
|`Arrays.binarySearch(arr, x)`|在有序数组中二分查找|
|`Arrays.fill(arr, x)`|填充数组|
|`Arrays.equals(a, b)`|比较一维数组元素|
|`Arrays.deepEquals(a, b)`|比较嵌套数组内容|
|`Arrays.copyOf(arr, n)`|复制数组|
|`Arrays.copyOfRange(arr, l, r)`|复制指定区间|
|`Arrays.toString(arr)`|将一维数组转换为字符串|
|`Arrays.deepToString(arr)`|输出嵌套数组内容|

注意，数组的比较与上一节学习的 String 比较有所不同。

## 数组为什么不能动态扩容？

数组创建时，其长度已经确定。所谓的数组扩容，本质上通常是创建一个更大的数组，再将原来的元素复制过去。

```java
int[] nums = {10, 20, 30};

nums = Arrays.copyOf(nums, 6);
```

但不过我们仍然还有动态数组：

| 语言     | 动态数组          |     |
| ------ | ------------- | --- |
| Java   | `ArrayList`   |     |
| C++    | `std::vector` |     |
| Rust   | `Vec<T>`      |     |
| Python | `list`        |     |
