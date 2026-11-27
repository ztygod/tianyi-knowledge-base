
## **Java 异常体系结构**

Java 中所有异常和错误都继承自 `Throwable`。
![[Pasted image 20261127164707.png|645]]
### **两种异常：Checked 与 Unchecked**

**Checked Exception（受检异常）**

编译器会检查你是否处理了这类异常。例如：

```java
public static void readFile() throws IOException {
    FileReader reader = new FileReader("test.txt");
}
```

如果不处理 `IOException`，就必须在方法声明中使用 `throws` 向外传播。

**Unchecked Exception（非受检异常）**

编译器不强制要求处理，通常是 `RuntimeException` 及其子类。

例如：

```java
int a = 10 / 0;
String s = null;
s.length();
```

这些操作可能产生：
- `ArithmeticException`
- `NullPointerException`

但编译器不会要求你使用 `try-catch` 或 `throws`。

## **try / catch / finally —— 异常处理**

### **try-catch 的基本结构**

当我们知道某段代码可能产生异常，并且希望在异常发生时采取措施，就可以使用 `try-catch`。

```java
public class Main {
    public static void main(String[] args) {
        try {
            int result = 10 / 0;
            System.out.println(result);
        } catch (ArithmeticException e) {
            System.out.println("Cannot divide by zero!");
        }
        System.out.println("Program continues...");
    }
}
```

执行过程：
1. 进入 `try`。
2. 执行 `10 / 0`。
3. 发生 `ArithmeticException`。
4. JVM 中断当前 `try` 代码块的正常执行。
5. 寻找匹配的 `catch`。
6. 执行异常处理逻辑。
7. 继续执行后面的程序。

### **finally 的典型用途**

假设我们打开一个文件：

```java
FileReader reader = null;

try {
    reader = new FileReader("test.txt");
    // Read file
} catch (IOException e) {
    System.out.println(e.getMessage());
} finally {
    if (reader != null) {
        try {
            reader.close();
        } catch (IOException e) {
            System.out.println("Failed to close file");
        }
    }
}
```

为什么要关闭文件？

因为文件属于操作系统管理的外部资源。打开文件意味着程序获得了相应的资源，使用结束后应当释放。如果一直不关闭，可能导致资源泄漏。这就引出了 Java 后面提供的一个更方便的机制：`try-with-resources`。

## **throws —— 异常传播**

### **为什么需要 throws？**

考虑一个文件读取函数：

```java
public static void readFile() {
    FileReader reader = new FileReader("test.txt");
}
```

这里会出现编译错误，因为 `FileReader` 的构造过程可能产生 `FileNotFoundException`，它是 Checked Exception。

有两种处理方式。

**方式一：在当前方法处理**

```java
public static void readFile() {
    try {
        FileReader reader = new FileReader("test.txt");
    } catch (FileNotFoundException e) {
        System.out.println("File not found");
    }
}
```

**方式二：向调用者传播**

```java
public static void readFile() throws FileNotFoundException {
    FileReader reader = new FileReader("test.txt");
}
```

这里的 `throws` 相当于告诉调用者：

> 这个方法可能产生 FileNotFoundException，你需要考虑如何处理。


## **throw 和 throws 的区别**

这两个关键字很容易混淆。

|关键字|作用|
|---|---|
|throw|主动抛出一个异常对象|
|throws|在方法签名中声明可能传播的异常类型|

## **try-with-resources —— 自动资源管理**

### **为什么需要它？**

之前我们使用 `finally` 手动关闭资源：

```java
FileReader reader = null;

try {
    reader = new FileReader("test.txt");
    // Read file
} catch (IOException e) {
    e.printStackTrace();
} finally {
    if (reader != null) {
        try {
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
```

代码比较复杂，而且需要自己处理关闭资源时产生的异常。Java 7 引入了 `try-with-resources`。

```java
try (FileReader reader = new FileReader("test.txt")) {
    // Read file
} catch (IOException e) {
    e.printStackTrace();
}
```

是不是简单很多？当 `try` 代码块执行完毕，无论是正常结束还是异常退出，Java 都会自动调用资源的 `close()` 方法。

### **为什么可以自动关闭？**

因为 Java 要求 try-with-resources 中的资源实现：

```java
AutoCloseable
```

其核心接口非常简单：

```java
public interface AutoCloseable {
    void close() throws Exception;
}
```

因此，只要一个对象实现了 `AutoCloseable`，就可以使用 try-with-resources 管理它。

常见的资源包括：
- FileReader
- BufferedReader
- FileWriter
- BufferedWriter
- Scanner
- PrintWriter

## **Java IO —— 文件输入输出**

### **Scanner —— 读取数据**

`Scanner` 是一个非常方便的输入工具。

例如，从控制台读取：

```java
Scanner scanner = new Scanner(System.in);

System.out.print("Enter your name: ");
String name = scanner.nextLine();

System.out.println("Hello " + name);
```

其中：

- `System.in`：标准输入流。
- `nextLine()`：读取一整行。
- `next()`：读取下一个以空白字符分隔的 token。
- `nextInt()`：读取一个整数。
- `nextDouble()`：读取一个浮点数。

### **从文件读取**

假设存在一个 `numbers.txt`：

```
10 20 30
40 50 60
```

使用 Scanner：

```java
try (Scanner scanner = new Scanner(new File("numbers.txt"))) {
    while (scanner.hasNextInt()) {
        int number = scanner.nextInt();
        System.out.println(number);
    }
} catch (FileNotFoundException e) {
    System.out.println("File not found");
}
```
