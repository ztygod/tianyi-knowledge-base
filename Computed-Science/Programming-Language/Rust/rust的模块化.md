# 一、先一句话总结

Rust 的模块化系统主要由 Package、Crate、Module、Path 和 `use` 组成，**通过** **`mod`** **组织模块、****`pub`** **控制可见性、****`use`** **引入名称，实现代码的组织、封装与复用**。

与 JavaScript/TypeScript 不同，Rust 将模块组织、访问权限和编译单元明确区分，模块树并不完全等同于文件目录结构。

# 二、核心要点

## 1. Rust 的模块化有哪些层级？

Rust 的代码组织可以分为三个主要层级：

|   |   |   |
|---|---|---|
|概念|作用|例子|
|Package|Cargo 管理的项目包|Cargo.toml|
|Crate|Rust 的独立编译单元|main.rs、lib.rs|
|Module|Crate 内部的代码组织单元|mod parser|

假设我们开发一个 Rust Shell：

```Plain
rust-shell/
├── Cargo.toml
└── src/
    ├── main.rs
    ├── parser.rs
    ├── executor.rs
    └── redirect.rs
```

在 `main.rs` 中：

```Plain
mod parser;
mod executor;
mod redirect;
```

Rust 会将这些模块组织成一棵模块树：

![](https://hcnz58qdrv4l.feishu.cn/space/api/box/stream/download/asynccode/?code=ZDk0MDNmYmExMDdjMTIxNzNjZTU2NTQyM2E0ZTdmMzNfakZyUWVJOU1HNkl5bmo4NElZZjF0MnEzTFR4QjBpeTNfVG9rZW46WnpNOWJNbWc2b2FMaHV4YXFXamNhQUhXblE5XzE3OTQxNDE4MDE6MTc5NDE0NTQwMV9WNA&add_watermark=true&scene_type=CCM)

其中 `main.rs` 是二进制 Crate 的根文件，`lib.rs` 通常是库 Crate 的根文件。

一句话总结：Package 是项目管理单位，Crate 是编译单位，Module 是代码组织单位。

## 2. mod、use 和 pub 分别是什么？

这是面试官最可能追问的内容。

|   |   |   |
|---|---|---|
|关键字|作用|核心理解|
|mod|声明或定义模块|建立模块树|
|use|将已有路径引入作用域|简化名称访问|
|pub|控制条目是否公开|控制可见性|
|pub use|公开地重新导出名称|设计对外 API|
|crate::|从当前 Crate 根模块开始定位|绝对模块路径|
|super::|从父模块开始定位|相对模块路径|
|self::|从当前模块开始定位|相对模块路径|

例如：

`parser.rs`：

```Plain
pub fn parse(input: &str) {
    println!("Parsing: {input}");
}
```

`main.rs`：

```Plain
mod parser;

use crate::parser::parse;

fn main() {
    parse("echo hello");
}
```

这里的执行逻辑是：

1. `mod parser` 将 `parser.rs` 纳入当前 Crate 的模块树。
    
2. `pub fn parse` 使 `parse` 函数对模块外部可见。
    
3. `use crate::parser::parse` 将函数名引入当前作用域。
    
4. 最后可以直接调用 `parse()`，而不需要写完整路径。
    

需要注意，`use` 并不负责加载或编译模块，它只是引入一个已有路径的名称。

## 3. Rust 的 pub 有哪些可见性控制？

Rust 默认所有条目都是私有的，需要通过 `pub` 系列修饰符扩大访问范围。

|   |   |
|---|---|
|修饰符|可见范围|
|不写 pub|定义所在模块及其子孙模块|
|pub|公共可见，但仍受路径可达性约束|
|pub(crate)|当前整个 Crate|
|pub(super)|父模块及其子孙模块|
|pub(in path)|指定的祖先模块及其子孙模块|

例如：

```Plain
mod shell {
    pub fn run() {}

    pub(crate) fn init() {}

    pub(super) fn setup() {}

    fn cleanup() {}
}
```

这里：

- `run()`：公共函数，可以通过可访问的模块路径调用。
    
- `init()`：只允许当前 Crate 内使用。
    
- `setup()`：在 `shell` 的父模块及其子孙模块中可见。此处父模块是 Crate Root，因此相当于 `pub(crate)`。
    
- `cleanup()`：只有 `shell` 及其子孙模块可以调用。
    

需要强调一个容易忽略的点：

即使函数使用 `pub`，也不意味着外部一定能访问它，因为它所在的父模块可能是私有的。

例如：

```Plain
mod internal {
    pub fn hello() {}
}
```

这里 `hello()` 是公共的，但 `internal` 模块没有公开，因此其他 Crate 不能通过 `internal::hello()` 访问它。

如果想作为库的公共 API，需要确保模块路径也公开，或者通过 `pub use` 重新导出。

一句话总结：Rust 的可见性不仅取决于函数本身，还取决于访问路径上的模块是否可见。

## 4. pub use 和 use 有什么区别？

这也是面试中的一个加分点。

**普通** **`use`** **只是在当前作用域引入名称，而** **`pub use`** **可以将名称重新导出，供外部使用。**

例如开发一个 Rust 库：

```Plain
src/
├── lib.rs
└── parser.rs
```

`parser.rs`：

```Plain
pub struct Parser;

impl Parser {
    pub fn new() -> Self {
        Self
    }
}
```

`lib.rs`：

```Plain
mod parser;

pub use parser::Parser;
```

外部用户可以这样使用：

```Plain
use my_library::Parser;

let parser = Parser::new();
```

而不需要了解内部的 `parser` 模块。

这就是 Re-export（重新导出）。

**它最大的价值是将内部实现结构与公共 API 解耦。**

例如，将来库内部把 `parser.rs` 拆成多个模块，只要保持 `pub use` 对外导出路径不变，使用者通常就不需要修改代码。

一句话总结：`use` 是引入名称，`pub use` 是将名称重新公开给其他模块或 Crate。

# 三、补充理解

## 1. crate、self、super 的区别

假设项目模块树如下：

```Plain
crate
└── shell
    ├── parser
    └── executor
```

在 `executor` 模块中：

```Plain
mod shell {
    pub mod parser {
        pub fn parse() {}
    }

    pub mod executor {
        pub fn run() {
            crate::shell::parser::parse();
            super::parser::parse();
        }
    }
}
```

这里：

- `crate::` 从整个 Crate 根模块开始查找。
    
- `super::` 从当前模块的父模块开始查找。
    
- `self::` 从当前模块开始查找。
    

相当于文件路径中的绝对路径和相对路径，但 Rust 定位的是模块树，而不是磁盘目录。

## 2. use 还有哪些常见写法？

```Plain
// 引入单个函数
use std::fs::File;

// 引入多个条目
use std::io::{self, Read, Write};

// 使用别名
use std::fs::Result as FsResult;

// 引入所有公共名称
use std::collections::*;
```

其中 `*` 是 glob import，实际项目中通常应谨慎使用，因为可能造成命名冲突，也会降低代码可读性。

另外，`use` 可以用于引入 Trait。对于某些通过 Trait 提供的方法，Trait 需要处于作用域中才能正常使用。

例如：

```Plain
use std::io::Write;

fn write_message() -> std::io::Result<()> {
    let mut file = std::fs::File::create("test.txt")?;
    file.write_all(b"hello")?;
    Ok(())
}
```

这里 `write_all()` 就是由 `Write` Trait 提供的方法。

# 四、实际使用场景

在工程开发中，我一般会根据模块的职责来划分代码，而不是单纯按文件数量拆分。

例如实现 Rust Shell：

```Plain
src/
├── main.rs
├── parser.rs
├── executor.rs
├── builtin.rs
└── redirect.rs
```

可以在 `main.rs` 中定义：

```Plain
mod parser;
mod executor;
mod builtin;
mod redirect;

use crate::parser::parse_shell_input;
use crate::executor::run_external_command;
```

而在 `parser.rs` 中：

```Plain
pub(crate) fn parse_shell_input(
    input: &str,
) -> Option<(String, Vec<String>)> {
    // 解析实现
    todo!()
}
```

**这里我会优先使用** **`pub(crate)`** **而不是直接使用** **`pub`****，因为这个解析函数只是 Shell 内部使用的能力，不需要对其他 Crate 暴露**。

对于希望提供给外部使用的 Rust 库，则可以通过 `pub` 和 `pub use` 设计稳定的公共 API。

# 五、面试完整答案（可以直接背）

> Rust 的模块化主要由 Package、Crate 和 Module 三个层级构成。Package 是 Cargo 管理的项目单位，Crate 是独立的编译单元，Module 则负责组织 Crate 内部代码。
> 
> 在语法上，Rust 主要通过 `mod`、`use` 和 `pub` 实现模块化。`mod` 用于声明和组织模块，`use` 用于将已有路径引入作用域，`pub` 则用于控制可见性。
> 
> Rust 默认模块成员都是私有的，可以使用 `pub`、`pub(crate)`、`pub(super)`、`pub(in path)` 控制不同的访问范围。其中 `pub(crate)` 只能在当前 Crate 内使用，`pub(super)` 将访问范围限制在父模块及其子孙模块。
> 
> 另外 Rust 使用 `crate`、`self` 和 `super` 表达模块路径，分别对应当前 Crate 根模块、当前模块和父模块。
> 
> 还有一个重要机制是 `pub use`，它能够重新导出内部模块中的公共类型或函数，避免外部使用者依赖内部模块结构，有利于设计稳定的公共 API。
> 
> 实际开发中，我一般按职责划分模块，对内部功能优先使用私有或 `pub(crate)`，只公开必要的 API，从而提高代码的封装性、可维护性和扩展性。

# 六、加分一句

如果面试官想进一步考察你对 Rust 模块化的理解，可以补充：

Rust 的模块系统不仅是代码组织机制，也是 API 封装机制。Crate 定义编译和依赖边界，Module 定义命名空间，`pub` 系列修饰符控制访问权限，而 `pub use` 可以将公共 API 与内部模块结构解耦。

另外，如果面试官继续问你 Rust 模块系统和 JavaScript ESM 有什么区别，重点回答：

JavaScript 的 `import/export` 同时参与模块依赖关系和名称导出，而 Rust 把这些职责拆得更明确：Cargo 管理依赖，`mod` 建立模块树，`use` 引入名称，`pub` 控制可见性。这种设计更适合具有严格编译期检查和封装需求的系统级项目。