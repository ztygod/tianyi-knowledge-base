
## 一、方法

Java 中的方法可以理解为定义在类中的函数，用于封装可以重复使用的程序逻辑。

```java
public class Main {
    public static int add(int a, int b) {
        return a + b;
    }
    public static void main(String[] args) {
        int result = add(10, 20);
        System.out.println(result);
    }
}
```

### 1. **静态方法与实例方法**

Java 中的方法可以分为静态方法（Static Method）和实例方法（Instance Method）。

```java
public class Calculator {
    int base = 10;

    public static int add(int a, int b) {
        return a + b;
    }

    public int increase(int x) {
        return base + x;
    }
}
```

两者最本质的区别在于：**实例方法具有当前对象上下文** `this`**，而静态方法没有。**

### 2. **方法重载（Overloading）**

Java 允许在同一个类中定义多个名称相同、参数列表不同的方法。

```java
public class Calculator {
    public static int add(int a, int b) {
        return a + b;
    }
    public static double add(double a, double b) {
        return a + b;
    }
    public static int add(int a, int b, int c) {
        return a + b + c;
    }
}
```

编译器会根据调用时的参数数量、编译期类型以及适用的重载规则选择对应的方法。

Java 的方法重载主要依据：
- 参数数量不同。
- 参数类型不同。
- 参数类型排列顺序不同。

！！！注意！！！返回类型不同不构成重载

**方法重载属于编译期多态（Compile-time Polymorphism）**

## 二、Java中的传参机制

**Java 只有值传递，没有引用传递。**

无论传入的是基本类型还是引用类型，方法接收到的都是实参值的副本。区别仅在于复制的值究竟是一个基本类型的值，还是一个对象引用。

### 1. 引用类型的值的传递

```java
public class Main {
    static void change(int[] arr) {
        arr[0] = 100;
    }
    public static void main(String[] args) {
        int[] nums = {1, 2, 3};
        change(nums);
        System.out.println(nums[0]); // 100
    }
}
```
因为复制的是引用，而不是整个数组对象。两个引用仍然指向同一个对象。

### 2. **修改对象与修改引用的区别**

```java
public class Main {
    static void change(int[] arr) {
        arr = new int[]{100, 200, 300};
    }
    public static void main(String[] args) {
        int[] nums = {1, 2, 3};
        change(nums);
        System.out.println(nums[0]); // 1
    }
}
```

虽然方法内部创建了一个新的数组，但外部的 `nums` 并没有指向新数组。

## 三、String 字符串

Java 中的 `String` 不是基本数据类型，而是标准库提供的一个类，位于 `java.lang` 包中。

### **1 、String 的基本使用**

```java
String s = "Hello Java";

s.length();          // 10
s.charAt(0);         // 'H'
s.substring(0, 5);  // "Hello"
s.indexOf("Java");   // 6
s.contains("Java");  // true
s.startsWith("He");  // true
s.toUpperCase();     // "HELLO JAVA"
```

### **2 、String 的不可变性（Immutability）**

Java 中的 String 是不可变对象。一个 String 对象创建之后，其字符串内容就不能被修改。

```java
String s = "hello";
s.toUpperCase();
System.out.println(s); // hello
```

`toUpperCase()` 返回转换后的字符串，并不会修改原对象。

如果想让变量保存转换后的结果，需要重新赋值：

```java
s = s.toUpperCase();
System.out.println(s); // HELLO
```


### **3**、 `==` **与** `equals()` **的区别**

```java
String a = new String("hello");
String b = new String("hello");

System.out.println(a == b);      // false
System.out.println(a.equals(b)); // true
```

|比较方式|比较内容|
|---|---|
|`==`（基本类型）|比较基本类型的值|
|`==`（引用类型）|判断是否引用同一个对象|
|`equals()`（String）|判断字符串内容是否相同|

但是下面的代码为什么会输出 `true`？

```java
String a = "hello";
String b = "hello";

System.out.println(a == b); // true
```

这涉及 Java 的字符串常量池（String Pool）。

### 4、字符常量池

Java 会对字符串字面量进行驻留（Interning），相同内容的字符串字面量可以共享同一个 String 对象，从而避免不必要的重复对象。

```java
String a = "hello";
String b = "hello";
String c = new String("hello");
```
![[Pasted image 20261124163634.png]]
### **5、StringBuilder**

由于 String 不可变，如果在循环中频繁进行字符串拼接，可能会产生大量中间对象和重复的数据复制。

```java
String s = "";
for (int i = 0; i < 10000; i++) {
    s += i;
}
```

更合适的做法是使用 `StringBuilder`：

```java
StringBuilder sb = new StringBuilder();
for (int i = 0; i < 10000; i++) {
    sb.append(i);
}
String result = sb.toString();
```

|类型|是否可变|线程安全|使用场景|
|---|---|---|---|
|`String`|否|不可变，可安全共享|普通字符串|
|`StringBuilder`|是|否|频繁修改、拼接|
|`StringBuffer`|是|单个操作同步|需要同步的字符串操作|

你可以将 StringBuilder 理解为一个能够动态扩容的字符缓冲区。

## 四、控制流

与其他语言类似，这里不做讨论。