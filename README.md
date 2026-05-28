# PPL 2026 小型语言处理系统

本项目根据 `prompt/core_idea.md` 的项目大纲和 `2026.docx` 的功能清单实现。处理程序使用 C 编写，读取 `.ppl` 示例源文件，完成词法分析、语法分析、语义检查和 C 代码生成，最终生成可与 `runtime/` 一起编译运行的 C 程序。

整体链路：

```text
.ppl 示例源文件 -> build/pplc -> generated/*.c -> gcc + runtime/*.c -> 可执行程序
```

## 环境要求

- Linux
- `gcc`
- `make`
- `bash`

项目不依赖第三方库。

## 目录结构

```text
.
├── Makefile
├── README.md
├── examples/
│   ├── 2026_feature_demo.ppl
│   ├── option_demo.ppl
│   ├── functional_demo.ppl
│   └── array_scope_demo.ppl
├── generated/
│   └── README.md
├── processor/
│   ├── main.c
│   ├── cli.c / cli.h
│   ├── compile_pipeline.c / compile_pipeline.h
│   ├── source.c / source.h
│   ├── diagnostic.c / diagnostic.h
│   ├── lexer.c / lexer.h / token.h
│   ├── ast.c / ast.h
│   ├── parser.c / parser.h
│   ├── semantic.c / semantic.h
│   ├── codegen.c / codegen.h
│   └── text_buffer.c / text_buffer.h
├── runtime/
│   ├── ppl_runtime.h
│   ├── rt_value.c / rt_value.h
│   ├── rt_option.c / rt_option.h
│   ├── rt_array.c / rt_array.h
│   ├── rt_function.c / rt_function.h
│   ├── rt_builtin.c / rt_builtin.h
│   ├── rt_memory.c / rt_memory.h
│   └── rt_error.c / rt_error.h
└── tests/
    ├── fixtures/
    ├── golden/
    ├── run_tests.sh
    └── test_plan.md
```

## 快速开始

构建处理程序：

```bash
make all
```

运行全部自动测试：

```bash
make test
```

运行 2026 综合示例：

```bash
make demo
```

清理构建产物：

```bash
make clean
```

## 手动生成和运行

生成 C 文件：

```bash
./build/pplc examples/2026_feature_demo.ppl -o generated/2026_feature_demo.c
```

编译生成程序：

```bash
gcc -Iruntime -std=c11 -Wall -Wextra -pedantic -g \
  -o build/2026_feature_demo \
  generated/2026_feature_demo.c runtime/*.c
```

运行：

```bash
./build/2026_feature_demo
```

调试词法输出：

```bash
./build/pplc examples/option_demo.ppl -o generated/option_demo.c --dump-tokens
```

## 输出规则

`.ppl` 示例中的普通语句默认不打印。某条语句后带有 `// => ...` 注释时，生成程序会打印该语句的实际求值结果。

示例：

```text
let x = Some(10)
is_some(x) // => true
match x {
    Some(v) => v + 1
    None => 0
} // => 11
```

运行输出：

```text
true
11
```

`// =>` 后面的文本用于说明期望结果和编写 golden 文件；程序不会把该文本当作固定输出，也不会按行号或文件名做特殊匹配。

## 支持的源语言能力

Option：

- `Some(整数或表达式)`
- `None`
- `is_some(value)`
- `is_none(value)`
- Option 变量赋值和函数返回

match：

- `match option { Some(x) => expr None => expr }`
- `Some(x) if condition => expr`
- 分支按源码顺序匹配

函数式编程：

- 函数赋值给变量：`let double = fn(x) { x * 2 }`
- Lambda：`fn x => x * 2`
- 多参数 Lambda：`fn acc, x => acc + x`
- 函数返回函数：`fn a => fn b => a + b`
- 连续调用：`add(3)(4)`

闭包：

- 匿名函数可读取外层变量
- 返回的闭包保留创建时环境
- 闭包可更新捕获变量
- 不同闭包实例拥有独立状态

Option 组合：

- `map_opt(option, fn x => expr)`
- `unwrap_or(option, default_value)`
- `and_then(option, fn x => option_expr)`
- `None` 在组合调用中短路传播

数组：

- 数组字面量：`[1, 2, 3]`
- 空数组：`[]`
- 索引：`arr[0]`
- 长度：`len(arr)`
- 高阶函数：`map`、`filter`、`reduce`、`any`、`all`

作用域和控制流：

- 块级作用域：`{ let x = 20 x }`
- 变量遮蔽
- 外层变量访问
- 局部变量更新
- `if (condition) { ... } else { ... }`
- `return`
- `for_each(x in arr) { ... }`
- 递归函数

## 测试覆盖

`make test` 覆盖以下输入：

- `tests/fixtures/option_match.ppl`
- `tests/fixtures/option_chain.ppl`
- `tests/fixtures/functional_closure.ppl`
- `tests/fixtures/arrays.ppl`
- `tests/fixtures/recursion_guard.ppl`
- `examples/2026_feature_demo.ppl`

每个测试都会执行：

1. `.ppl` -> `generated/test_*.c`
2. `generated/test_*.c` + `runtime/*.c` -> `build/test_*.bin`
3. 运行 `build/test_*.bin`
4. 与 `tests/golden/*.out` 逐字节比对

## 已验证命令

当前实现已通过：

```bash
make test
make demo
```

`make test` 会输出：

```text
PASS option_match
PASS option_chain
PASS functional_closure
PASS arrays
PASS recursion_guard
PASS 2026_feature_demo
```
