## 封装

随着程序规模扩大，会出现一个严重的问题：任何模块都可以随意修改对象的内部状态，导致对象进入不合法的状态。

```java
public class Counter {
    private int count;

    public void increment() {
        count++;
    }

    public int getCount() {
        return count;
    }
}
```

## **Java 的访问控制（Access Control）**

Java提供了四种访问权限，用于控制类、字段、构造函数和方法的可见性。

![[Pasted image 20261126104246.png]]
### **private**

表示私有成员，通常只能在声明它的类内部访问。

```java
public class Student {
    private String name;

    public Student(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }
}
```

有一个容易误解的地方：**private 是类级别的访问限制，而不是对象级别的访问限制。**

```java
public class Student {
    private String name;

    public boolean sameName(Student other) {
        return this.name.equals(other.name);
    }
}
```

虽然 `other` 是另一个对象，但这段代码完全合法，因为访问行为发生在 Student 类内部。

### **封装的进阶问题：引用泄漏**

即使将字段声明为 `private`，也不一定真正实现了封装。观察下面的代码：

```java
public class Classroom {
    private int[] scores = {90, 85, 95};

    public int[] getScores() {
        return scores;
    }
}
```

外部代码：

```java
Classroom classroom = new Classroom();
int[] scores = classroom.getScores();

scores[0] = -100;
```

虽然 `scores` 字段是私有的，但外部仍然能够修改其内部数组。一种解决方法是防御性复制（Defensive Copy）：

```java
public int[] getScores() {
    return scores.clone();
}
```

现在外部获取的是数组的副本，不会影响内部数组。

## **package：Java 的包机制**

### **package 的基本语法**

文件：`Student.java`

```java
package com.example.school;

public class Student {
    private String name;

    public Student(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }
}
```

这个类的完全限定名称（Fully Qualified Name，FQN）是：

```java
com.example.school.Student
```

`Student` 是简单类名（Simple Name），`com.example.school` 是包名。

## **import：导入机制**

假设我们想使用 `java.util.ArrayList`。可以直接写完全限定名称：

```java
java.util.ArrayList<String> names =
        new java.util.ArrayList<>();
```

但如果频繁使用，代码会比较冗长。因此 Java 提供了 `import`：

```java
import java.util.ArrayList;

public class Main {
    public static void main(String[] args) {
        ArrayList<String> names = new ArrayList<>();
    }
}
```

### **import 的本质**

**import 主要用于编译期间的名称解析，让我们可以使用简单名称代替完全限定名称。**

它不会将其他类的源代码复制到当前文件，也不意味着执行到 import 时就加载并初始化对应的类。
![[Pasted image 20261126110858.png]]
### **静态导入（Static Import）**

之前在 Crafting Interpreters 中见过类似写法：

```java
import static com.craftinginterpreters.lox.TokenType.*;
```

**静态导入允许直接使用其他类中符合访问规则的静态成员**。

普通写法：

```java
double result = Math.sqrt(16);
double pi = Math.PI;
```

静态导入：

```java
import static java.lang.Math.sqrt;
import static java.lang.Math.PI;

public class Main {
    public static void main(String[] args) {
        double result = sqrt(16);
        double pi = PI;
    }
}
```

不需要每次都写 `Math.`。