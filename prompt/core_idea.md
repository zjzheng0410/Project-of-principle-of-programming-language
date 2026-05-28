# 项目总体架构设计

## 任务理解

本项目的核心目标是实现一个小型语言处理系统，而不是手写一份固定功能的 C 程序。项目必须包含一个可直接作为输入的示例文件，以及一个处理程序。处理程序读取示例文件，完成词法分析、语法分析、语义检查和代码生成，最终输出一个可被正常编译运行的 C 代码文件。

整体输入输出关系如下：

```text
示例文件 + 处理程序 -> 可正常编译并实现相应功能的 C 代码文件
```

示例文件需要用注释说明所覆盖的语言功能，并用 `// =>` 标出关键表达式或语句的期望输出。评分时可能会根据功能说明对示例文件做同功能范围内的修改，因此处理程序不能只匹配固定文本，必须按语言规则处理同类输入。

处理程序可以用 C 或 Python 实现，但生成目标必须是 C 代码。为了让项目更贴合“编译原理/程序语言处理”的要求，本设计建议处理程序使用 C 实现，运行时支持库也使用 C 实现；如果后续为了开发效率改用 Python，也应保持同样的模块边界和处理流程。

## 功能范围

功能清单以 `2026.docx` 为准，示例文件和处理程序至少需要覆盖以下能力：

- Option 基础能力：支持 `Some(整数)`、`None`、`is_some`、`is_none`、Option 赋值和返回。
- match 基本结构：支持 `match option { Some(x) => expr None => expr }`，并保证分支返回值类型一致。
- Option 实用场景：支持返回 `Some(value)` 或 `None` 的查找函数，如 `find(arr, target)`。
- 函数式编程：支持函数赋值给变量、匿名函数块形式、Lambda 表达式形式、函数返回函数和连续调用。
- 闭包能力：支持匿名函数捕获外层变量，并支持闭包拥有独立可变状态。
- Option 组合能力：支持 `map_opt`、`unwrap_or`、`and_then`，并体现 `None` 的链式短路传播。
- 数组处理能力：支持数组字面量、空数组、索引、`len`、遍历、`map`、`filter`、`reduce`、`any`、`all`。
- 作用域能力：支持块级作用域、变量遮蔽、局部变量更新和外层变量访问。
- 控制流能力：支持 `if/else` 条件表达式、`return`、`for_each` 遍历和递归函数。
- match 守卫：支持 `Some(x) if condition => expr`，并按分支顺序匹配。

## 语言处理流程

处理程序采用分阶段流水线设计，每一层只关心自己的输入输出，不跨层访问内部细节。

```text
源文件读取
  -> 词法分析
  -> 语法分析
  -> 语义检查
  -> 闭包捕获分析
  -> 中间表示降低
  -> C 代码生成
  -> 写出 generated C 文件
  -> 与 runtime 一起编译运行
```

各阶段职责边界如下：

- 源文件读取只负责读取文本、维护行列位置和提供源码片段，不理解语言语义。
- 词法分析只负责把文本转换为 token，不做表达式优先级、类型或作用域判断。
- 语法分析只负责构造 AST，不生成 C，不检查变量是否定义。
- 语义检查只负责名字绑定、类型一致性、作用域规则、函数调用规则和 Option/数组规则。
- 闭包捕获分析只负责计算匿名函数使用了哪些外层变量，以及这些变量是否需要可变环境。
- 中间表示降低只负责把高级结构转换为更接近 C 的结构，如闭包对象、match 分支和数组高阶调用。
- C 代码生成只负责输出 C 文本，不重新解释语义规则。
- 运行时库只提供通用数据结构和操作，不依赖处理程序内部 AST 或 IR。

## 建议目录树

```text
project-root/
├── README.md
├── Makefile
├── examples/
│   ├── 2026_feature_demo.ppl
│   ├── option_demo.ppl
│   ├── functional_demo.ppl
│   └── array_scope_demo.ppl
├── processor/
│   ├── main.c
│   ├── compile_pipeline.h
│   ├── compile_pipeline.c
│   ├── cli.h
│   ├── cli.c
│   ├── source.h
│   ├── source.c
│   ├── diagnostic.h
│   ├── diagnostic.c
│   ├── token.h
│   ├── lexer.h
│   ├── lexer.c
│   ├── ast.h
│   ├── ast.c
│   ├── parser.h
│   ├── parser.c
│   ├── parser_expr.c
│   ├── parser_stmt.c
│   ├── parser_match.c
│   ├── symbol.h
│   ├── symbol.c
│   ├── scope.h
│   ├── scope.c
│   ├── type.h
│   ├── type.c
│   ├── semantic.h
│   ├── semantic.c
│   ├── builtin.h
│   ├── builtin.c
│   ├── capture.h
│   ├── capture.c
│   ├── ir.h
│   ├── ir.c
│   ├── lower.h
│   ├── lower.c
│   ├── codegen.h
│   ├── codegen.c
│   ├── emit_expr.c
│   ├── emit_stmt.c
│   ├── emit_function.c
│   ├── emit_match.c
│   ├── emit_runtime.c
│   ├── name_mangle.h
│   ├── name_mangle.c
│   ├── text_buffer.h
│   ├── text_buffer.c
│   ├── file_writer.h
│   ├── file_writer.c
│   ├── arena.h
│   └── arena.c
├── runtime/
│   ├── ppl_runtime.h
│   ├── rt_value.h
│   ├── rt_value.c
│   ├── rt_option.h
│   ├── rt_option.c
│   ├── rt_array.h
│   ├── rt_array.c
│   ├── rt_function.h
│   ├── rt_function.c
│   ├── rt_builtin.h
│   ├── rt_builtin.c
│   ├── rt_memory.h
│   ├── rt_memory.c
│   ├── rt_error.h
│   └── rt_error.c
├── generated/
│   ├── README.md
│   └── 2026_feature_demo.c
└── tests/
    ├── fixtures/
    │   ├── option_match.ppl
    │   ├── option_chain.ppl
    │   ├── functional_closure.ppl
    │   ├── arrays.ppl
    │   └── recursion_guard.ppl
    ├── golden/
    │   ├── option_match.out
    │   ├── option_chain.out
    │   ├── functional_closure.out
    │   ├── arrays.out
    │   └── recursion_guard.out
    └── test_plan.md
```

## 根目录文件职责

- `README.md`：说明项目目标、输入输出关系、构建命令、运行方式和支持的语言功能。它只面向使用者解释如何运行处理程序和编译生成文件，不承载核心实现逻辑。
- `Makefile`：统一构建处理程序、运行时库和生成后的 C 程序。它只组织编译流程，不写语言处理逻辑。

## `examples/` 示例文件职责

- `2026_feature_demo.ppl`：综合示例输入文件，完整覆盖 `2026.docx` 中的 Option、函数式编程、Option 组合、数组高阶函数、作用域、闭包状态、递归和 match 守卫。文件中每段示例都需要有注释说明功能点，并用 `// =>` 标明期望输出。
- `option_demo.ppl`：聚焦 `Some`、`None`、`is_some`、`is_none`、match、match 守卫、`map_opt`、`unwrap_or` 和 `and_then`。它用于验证 Option 相关逻辑不是针对综合示例硬编码。
- `functional_demo.ppl`：聚焦函数赋值、匿名函数、Lambda、函数返回函数、连续调用和闭包捕获。它用于验证函数值能作为普通值流动。
- `array_scope_demo.ppl`：聚焦数组字面量、空数组、索引、`len`、`for_each`、`map`、`filter`、`reduce`、`any`、`all`、块级作用域和变量遮蔽。

## `processor/` 处理程序文件职责

- `main.c`：处理程序入口，只负责调用命令行解析和编译流水线，并根据结果返回进程退出码。它不直接包含词法、语法、语义或代码生成逻辑。
- `compile_pipeline.h`：声明编译流水线配置、输入输出路径和总入口函数。它是 `main.c` 与具体编译阶段之间的稳定边界。
- `compile_pipeline.c`：按顺序组织源码读取、词法分析、语法分析、语义检查、闭包捕获、IR 降低、C 代码生成和文件写出。它只调度阶段，不实现某个阶段的细节。
- `cli.h`：声明命令行参数结构和解析接口。它让处理程序入口不依赖具体参数格式。
- `cli.c`：实现输入示例文件、输出 C 文件、是否打印 token/AST 等调试参数的解析。它只处理命令行语义，不读取或编译源文件。
- `source.h`：声明源码缓冲区、源码位置、行列映射和片段提取接口。它为 lexer、parser 和 diagnostic 提供统一源码视图。
- `source.c`：实现示例文件读取、UTF-8 文本保存、行起始位置记录和源码片段查询。它不理解任何语言语法。
- `diagnostic.h`：声明错误、警告和源码位置报告接口。它统一所有阶段的错误表达格式。
- `diagnostic.c`：实现诊断信息格式化、错误计数和输出。它保证错误能定位到文件、行、列和相关源码片段。
- `token.h`：定义 token 类型、关键字、运算符、字面量范围和源码位置。它是 lexer 与 parser 之间唯一共享的词法契约。
- `lexer.h`：声明词法分析器初始化、读取 token、回看 token 和销毁接口。它隐藏扫描状态。
- `lexer.c`：实现标识符、整数、关键字、符号、注释、字符串化期望输出注释和空白处理。它只把源文本转换为 token，不做语法或类型判断。
- `ast.h`：声明 AST 节点类型、表达式、语句、函数、match 分支、模式和数组字面量结构。它表达源语言本身，不绑定 C 输出形式。
- `ast.c`：实现 AST 节点创建、链表管理、基础遍历和销毁。它集中管理语法树生命周期，避免 parser 与语义层重复分配细节。
- `parser.h`：声明语法分析总入口和解析结果结构。它对外隐藏表达式、语句和 match 子解析器的拆分。
- `parser.c`：实现顶层程序解析，负责把函数定义、顶层 `let`、表达式语句和块组织成完整 AST。它不处理表达式优先级细节。
- `parser_expr.c`：解析整数、布尔、变量、数组字面量、函数调用、连续调用、索引、匿名函数、Lambda、条件表达式和运算表达式。它负责优先级和结合性。
- `parser_stmt.c`：解析 `let`、赋值、`return`、块、函数体、`for_each` 和表达式语句。它维护语句边界，不处理 match 分支内部规则。
- `parser_match.c`：解析 match 表达式、`Some(x)` 模式、`None` 模式、守卫条件和分支表达式。它让匹配语法独立演进，避免主 parser 过长。
- `symbol.h`：声明符号表条目、名字类别、定义位置和查询接口。它是作用域检查与代码生成命名之间的桥梁。
- `symbol.c`：实现变量、函数、参数、闭包捕获变量和内置名称的登记与查找。它不做类型比较，只保证名字解析一致。
- `scope.h`：声明词法作用域栈、进入退出作用域、变量遮蔽和外层查找接口。它让块级作用域与闭包分析共享同一套作用域模型。
- `scope.c`：实现作用域进入、退出、当前层查询、外层查询和遮蔽规则。它确保局部变量、外层变量和捕获变量边界清晰。
- `type.h`：声明语言层类型描述，包括整数、布尔、数组、函数、Option、空数组待定类型和错误类型。它只定义类型概念，不负责检查过程。
- `type.c`：实现类型构造、类型比较、函数签名比较、Option 元素类型提取和类型名称格式化。它为语义检查和诊断提供基础能力。
- `semantic.h`：声明语义检查入口、语义结果和错误状态。它把 AST、符号表、作用域和类型检查组织成一个阶段。
- `semantic.c`：检查变量定义、变量遮蔽、赋值合法性、函数调用、递归引用、返回值、数组元素类型、数组索引、Option 操作、match 分支类型和守卫条件类型。它发现错误后直接报告，不做静默修正。
- `builtin.h`：声明内置函数、内置语法能力和运行时映射信息。它让 `len`、`map`、`filter`、`reduce`、`any`、`all`、`is_some`、`is_none`、`map_opt`、`unwrap_or`、`and_then` 有统一入口。
- `builtin.c`：维护内置名称、参数数量、参数类型规则、返回类型规则和对应运行时函数名。它避免内置规则散落在 parser、semantic 和 codegen 中。
- `capture.h`：声明闭包捕获分析结果、捕获变量列表和可变捕获标记。它描述匿名函数需要携带哪些外层状态。
- `capture.c`：分析匿名函数和返回函数中对外层变量的引用，标记只读捕获和可变捕获。它支撑闭包独立状态和函数返回函数。
- `ir.h`：声明中间表示节点、IR 函数、IR 语句、IR 表达式、IR 闭包环境和 IR match 分支。它把高级语言结构降为更接近 C 输出的形式。
- `ir.c`：实现 IR 对象创建、列表维护、遍历和销毁。它不直接读取源码，也不直接写 C 文件。
- `lower.h`：声明 AST 到 IR 的降低入口和降低上下文。它是语义检查与代码生成之间的边界。
- `lower.c`：把闭包、函数值、match、Option 组合、数组高阶函数、`for_each`、递归调用和条件表达式转换成可生成 C 的 IR。它不关心最终文本格式。
- `codegen.h`：声明 C 代码生成总入口、生成配置和生成结果。它向编译流水线隐藏具体发射细节。
- `codegen.c`：组织生成 C 文件的整体结构，包括标准库引用、`ppl_runtime.h` 引用、全局声明、函数顺序、`main` 包装和清理逻辑。它不塞入大段表达式或语句输出。
- `emit_expr.c`：负责表达式级 C 输出，包括字面量、变量、数组访问、函数调用、闭包调用、Option 值、内置函数调用和条件表达式。它只处理表达式片段。
- `emit_stmt.c`：负责语句级 C 输出，包括变量声明、赋值、返回、块、`if/else` 和 `for_each` 降低后的循环。它只处理语句和函数体结构。
- `emit_function.c`：负责普通函数、匿名函数提升后的静态函数、闭包调用适配函数和递归函数声明的输出。它让函数相关 C 生成不挤在 `codegen.c` 中。
- `emit_match.c`：负责 match 表达式和 match 守卫的 C 输出。它保证 `Some` 分支解包、`None` 分支和守卫条件按源语言顺序执行。
- `emit_runtime.c`：负责生成对运行时库的引用、初始化片段和清理片段。它集中管理生成代码与 `runtime/` 的耦合点。
- `name_mangle.h`：声明源语言名称到 C 符号名称的转换接口。它防止源语言名称与 C 关键字、运行时符号或闭包提升名称冲突。
- `name_mangle.c`：实现稳定、可读、可回溯的 C 命名策略。它不参与语义判断，只服务生成阶段。
- `text_buffer.h`：声明可增长文本缓冲区接口。它让代码生成阶段先写入内存缓冲，再统一落盘。
- `text_buffer.c`：实现追加字符串、格式化追加、缩进管理和缓冲区扩容。它不理解 C 语义，只提供文本拼接能力。
- `file_writer.h`：声明输出文件写入、目录检查和写入错误报告接口。它把代码生成结果与文件系统操作分离。
- `file_writer.c`：实现目标路径检查、C 文件写出和写入失败诊断。它只写生成结果，不修改输入示例文件。
- `arena.h`：声明处理程序内部临时内存分配接口。它用于统一管理 AST、IR、符号表和诊断对象等短生命周期数据。
- `arena.c`：实现分阶段释放的内存池。它降低复杂对象释放成本，并避免各模块互相负责释放内部对象。

## `runtime/` 运行时文件职责

- `ppl_runtime.h`：运行时总入口头文件，汇总生成 C 文件需要包含的公共接口。生成文件应优先只包含这个总入口，避免直接依赖多个内部头文件。
- `rt_value.h`：声明运行时统一值表示，包括整数、布尔、数组、Option 和函数值。它为不同运行时模块提供共同数据边界。
- `rt_value.c`：实现值创建、复制、比较、打印和销毁。它避免数组、Option 和闭包模块重复管理值生命周期。
- `rt_option.h`：声明 `Some`、`None`、判定、解包、默认值、组合和链式传播接口。它只暴露 Option 语义，不暴露内部存储细节。
- `rt_option.c`：实现 Option 运行时行为，包括 `is_some`、`is_none`、`map_opt`、`unwrap_or` 和 `and_then`。它保证 `None` 在组合和链式调用中按语义短路。
- `rt_array.h`：声明数组创建、空数组、索引、长度、遍历和高阶处理接口。它统一数组能力，避免生成代码直接操作底层内存。
- `rt_array.c`：实现数组存储、边界检查、`map`、`filter`、`reduce`、`any`、`all` 和元素生命周期管理。它负责数组相关运行时错误定位。
- `rt_function.h`：声明函数值、闭包环境、调用适配和环境释放接口。它是匿名函数、函数返回函数、连续调用和闭包状态的运行时基础。
- `rt_function.c`：实现闭包对象创建、环境持有、可变捕获、调用分发和释放。它确保不同闭包实例拥有独立状态。
- `rt_builtin.h`：声明语言内置函数的运行时入口。它把数组、Option 和打印等公共能力集中暴露给生成代码。
- `rt_builtin.c`：实现内置函数到具体运行时模块的转发和参数检查。它保持生成代码简单，并让错误信息一致。
- `rt_memory.h`：声明运行时内存申请、释放和失败处理接口。它为值、数组和闭包提供统一内存边界。
- `rt_memory.c`：实现内存申请、释放和申请失败后的明确终止策略。它不做隐藏式容错。
- `rt_error.h`：声明运行时错误报告接口。它统一数组越界、类型不匹配、空值误用、函数调用错误和内存错误的表达。
- `rt_error.c`：实现运行时错误输出和退出策略。它让生成程序失败时能定位到操作类型和错误原因。

## `generated/` 输出目录职责

- `README.md`：说明该目录用于存放处理程序生成的 C 文件和临时构建产物。该目录内容应可删除并重新生成，不承载手写核心逻辑。
- `2026_feature_demo.c`：由处理程序根据 `examples/2026_feature_demo.ppl` 生成的目标 C 文件。它应能与 `runtime/` 一起正常编译运行，并输出示例中 `// =>` 标注的结果。

## `tests/` 测试资料职责

- `fixtures/option_match.ppl`：保存 Option 赋值、判定、match 基本结构和 match 守卫的测试输入。它用于验证 Option 模式匹配和分支类型检查。
- `fixtures/option_chain.ppl`：保存 `map_opt`、`unwrap_or`、`and_then` 和安全除法链式传播测试输入。它用于验证 `None` 不会继续执行后续闭包。
- `fixtures/functional_closure.ppl`：保存函数赋值、Lambda、函数返回函数、闭包捕获和闭包独立状态测试输入。它用于验证函数值和闭包环境。
- `fixtures/arrays.ppl`：保存数组字面量、空数组、索引、`len`、`map`、`filter`、`reduce`、`any` 和 `all` 测试输入。它用于验证数组和高阶函数组合。
- `fixtures/recursion_guard.ppl`：保存递归阶乘、条件表达式、返回 Option 的局部函数和 match 守卫测试输入。它用于验证递归、控制流和 Option 返回值。
- `golden/option_match.out`：保存 Option 与 match 测试的期望输出，用于和生成程序运行结果做文本级比对。
- `golden/option_chain.out`：保存 Option 组合与链式传播测试的期望输出，用于验证 `Some` 与 `None` 分支稳定。
- `golden/functional_closure.out`：保存函数式与闭包测试的期望输出，用于验证连续调用、捕获变量和闭包状态。
- `golden/arrays.out`：保存数组测试的期望输出，用于验证高阶数组函数结果稳定。
- `golden/recursion_guard.out`：保存递归、条件表达式和 match 守卫测试的期望输出，用于验证控制流行为。
- `test_plan.md`：说明自动测试和手工测试范围，记录每类语言特性对应哪些输入文件、生成文件、编译命令和期望输出。

## 示例文件设计要求

- 示例文件必须能直接作为处理程序输入，不需要人工预处理。
- 示例文件中的注释需要说明功能点，例如“Option 赋值和判定”“闭包拥有独立状态”“match 守卫条件”。
- 示例文件中的关键结果应使用 `// =>` 标注，便于人工检查和自动测试提取。
- 示例文件应覆盖正常路径和边界路径，例如 `Some` 与 `None`、非空数组与空数组、闭包多次调用、递归终止条件。
- 示例文件只能描述源语言能力，不应包含任何 C 代码片段或处理程序专用指令。

## 解耦与封装原则

- `.h` 文件只暴露必要接口，`.c` 文件隐藏实现细节；跨模块通信只通过头文件和明确的数据结构完成。
- `main.c` 只负责入口和退出码，编译流程放在 `compile_pipeline.c`，避免入口函数膨胀。
- lexer 不理解 AST，parser 不做类型检查，semantic 不生成 C，lower 不写文本，codegen 不重新做语义判断。
- runtime 不依赖 processor，generated 不反向依赖 processor，examples 不包含处理程序内部实现约定。
- Option、数组、函数值、闭包、内存和错误处理分别放在独立运行时模块中，避免一个巨大的通用运行时文件。
- 内置函数规则集中在 `builtin.c`，不要散落在 parser、semantic、lower 和 codegen 中。
- match、函数发射、表达式发射和语句发射分别拆分，避免 `codegen.c` 变成难维护的大文件。
- 任何单个 `.c` 或 `.h` 文件接近 800 行时，应优先按职责继续拆分，而不是继续追加逻辑。
- 所有错误都应给出明确位置和阶段，例如“语义检查：未定义变量”“代码生成：无法写入输出文件”，不要静默忽略。
- 示例文件只作为语言能力展示和测试输入，不允许在处理程序中写针对示例文件名、行号或固定文本的特殊分支。

## 推荐实现顺序

1. 先完成源码读取、诊断、词法分析和最小语法分析，让处理程序能读取示例文件并报告可定位错误。
2. 再完成整数、布尔、变量、块级作用域、赋值、条件表达式和普通函数调用，形成“输入示例 -> 生成 C -> 编译运行”的最小闭环。
3. 加入 Option 基础能力、`is_some`、`is_none`、match 基本结构和返回 Option 的函数。
4. 加入数组字面量、空数组、索引、`len`、`for_each` 和数组运行时模块。
5. 加入函数值、Lambda、函数返回函数、闭包捕获和闭包独立状态。
6. 加入 `map_opt`、`unwrap_or`、`and_then`、数组 `map`、`filter`、`reduce`、`any`、`all` 等高阶能力。
7. 最后补齐递归函数、match 守卫、综合示例文件和按 `2026.docx` 功能清单设计的测试样例。
