
## 一、 **Java 的编译与运行机制**

### 1. Java是怎么执行的

Java的一个特点就是跨平台执行，Java 既不像传统的 C++ 一样通常直接编译为目标平台的机器码，也不是简单地想JavaScript一样逐行解释执行源代码。

Java 首先将源代码编译为一种与平台无关的中间指令——**字节码（Bytecode）**，然后由 JVM 负责执行字节码。
![[Pasted image 20261124154044.png]]
JVM 可以解释执行字节码，也可以通过 JIT（Just-In-Time Compilation，即时编译）将热点代码编译为本地机器码，从而提高运行效率。

这就是 Java 所强调的：

**Write Once, Run Anywhere（一次编写，到处运行）。**
![[Pasted image 20261124154257.png]]
### 2. **JDK、JRE、JVM**
![[Pasted image 20261124154339.png]]
## 二、Java程序基本结构

```java
public class Main {
    public static void main(String[] args) {
        System.out.println("Hello Java!");
    }
}
```
![[Pasted image 20261124154558.png]]
### **1.为什么 main 方法需要 static？**

> 无需创建类的实例即可马上调用

在传统的 Java 程序启动方式中，JVM 需要调用 `main` 方法，此时不需要提前创建 `Main` 类的对象。

而 `static` 方法可以直接通过类调用：

```java
Main.main(args);
```

不需要先创建对象：

```java
Main obj = new Main();
```

因此，传统的 `main` 方法被设计为静态方法。较新版本的 Java 也支持其他简化的入口形式，但本课程统一采用传统写法。

### **2.文件名与类名**

如果声明了一个 `public` 类：

```java
public class Main {
}
```

那么源文件必须命名为 `Main.java`。

一个 Java 文件可以包含多个顶层类，但最多只能有一个 `public` 顶层类。

## 三、Java数据类型

Java 是一门静态类型语言，其数据类型可以分为两大类：**基本数据类型（Primitive Types）和引用类型（Reference Types）**。
![[Pasted image 20261124154828.png]]
![[Pasted image 20261124154901.png]]
### **1 int 与 Integer**

Java 为每一种基本数据类型都提供了对应的包装类（Wrapper Class）。
![[Pasted image 20261124155027.png]]Java 提供自动装箱（Autoboxing）和自动拆箱（Unboxing）机制：

```java
int a = 10;
Integer b = a; // 自动装箱
int c = b;     // 自动拆箱
```

为什么需要包装类？因为 Java 的部分机制需要使用对象类型，例如泛型：

```java
ArrayList<Integer> list = new ArrayList<>();
```

不能写成：

```java
ArrayList<int> list = new ArrayList<>(); // 编译错误
```

## 四、Java与其他语言区别
![[Pasted image 20261124155216.png]]