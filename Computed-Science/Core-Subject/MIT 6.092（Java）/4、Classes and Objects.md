
## **Class 与 Object**

类是用户定义的一种类型，它描述一组对象应当具有什么状态和行为。对象是类的具体实例（Instance）。

```java
class Student {
    String name;
    int age;

    void introduce() {
        System.out.println("My name is " + name);
    }
}

Student s1 = new Student();
s1.name = "Alice";
s1.age = 20;

Student s2 = new Student();
s2.name = "Bob";
s2.age = 21;

s1.introduce();
s2.introduce();
```

### 构造函数

构造函数的作用就是在创建对象时完成初始化工作，让对象从一开始就具备合理的状态。

```java
class Student {
    String name;
    int age;

    Student(String name, int age) {
        this.name = name;
        this.age = age;
    }

    void introduce() {
        System.out.println(name + ", " + age);
    }
}
```

构造函数具有以下特点：

|特性|说明|
|---|---|
|名称|必须与类名相同|
|返回值|没有返回值类型，甚至不能写 `void`|
|调用时机|对象创建和初始化过程中|
|重载|支持|
|主要职责|初始化对象，建立对象的初始状态|

### **this 关键字**

**this 是当前实例对象的引用**。当我们通过对象调用实例方法时，该方法能够通过 `this` 知道当前正在操作哪个对象。

```java
class Counter {
    int count = 0;

    void increment() {
        this.count++;
    }
}
```

### **static 关键字**

**实例字段属于具体对象，而静态字段属于类**，由该类的实例共享。

## **对象生命周期与垃圾回收**

### **对象的生命周期（Object Lifecycle）**
![[Pasted image 20261126093619.png]]

### **对象什么时候可以被回收**

如果没有其他有效的强引用能够使这个对象保持可达，那么该对象就可以成为垃圾回收的候选对象。

```java
Student s = new Student("Alice", 20);

s = null;
```